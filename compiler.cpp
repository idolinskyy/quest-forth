#include "compiler.h"
#include "words.h"

#include <cctype>
#include <charconv>
#include <cstdlib>
#include <optional>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

Compiler::Compiler() { register_words(dictionary); }

static std::string parse_string_literal(std::string_view source, size_t &idx) {
  std::string out;
  const size_t len = source.size();
  while (idx < len && source[idx] != '"') {
    if (source[idx] == '\\') {
      ++idx;
      if (idx >= len)
        throw std::runtime_error("Unfinished escape sequence in string");
      switch (source[idx]) {
      case 'n':
        out.push_back('\n');
        break;
      case 't':
        out.push_back('\t');
        break;
      case 'r':
        out.push_back('\r');
        break;
      case '\\':
        out.push_back('\\');
        break;
      case '"':
        out.push_back('"');
        break;
      case '0':
        out.push_back('\0');
        break;
      default:
        throw std::runtime_error(std::string("Unknown escape sequence: \\") + source[idx]);
      }
      ++idx;
      continue;
    }
    out.push_back(source[idx++]);
  }
  if (idx >= len)
    throw std::runtime_error("Unterminated string literal");
  ++idx;
  return out;
}

static int line_at(std::string_view source, size_t idx) {
  int line = 1;
  for (size_t i = 0; i < idx && i < source.size(); ++i) {
    if (source[i] == '\n')
      ++line;
  }
  return line;
}

void Compiler::compile(std::string_view source, VM &vm) {
  std::stack<ControlEntry> control_stack;
  std::unordered_map<std::string, size_t> user_words;
  std::optional<ColonDef> current_colon;

  // Keep code_line in sync if compiling onto existing code
  if (vm.code_line.size() > vm.code.size())
    vm.code_line.resize(vm.code.size());
  while (vm.code_line.size() < vm.code.size())
    vm.code_line.push_back(0);

  auto emit = [&](Cell cell, int line) {
    vm.code.push_back(std::move(cell));
    vm.code_line.push_back(line);
  };

  size_t idx = 0;
  const size_t len = source.size();
  std::string current_compiling_scene;

  while (idx < len) {
    while (idx < len && std::isspace(static_cast<unsigned char>(source[idx])))
      ++idx;
    if (idx >= len)
      break;

    if (source[idx] == '\\') {
      while (idx < len && source[idx] != '\n' && source[idx] != '\r')
        ++idx;
      continue;
    }

    if (source[idx] == '(' &&
        (idx + 1 < len && std::isspace(static_cast<unsigned char>(source[idx + 1])))) {
      idx += 2;
      while (idx < len && source[idx] != ')')
        ++idx;
      if (idx < len && source[idx] == ')')
        ++idx;
      continue;
    }

    if (source[idx] == '"') {
      const int line = line_at(source, idx);
      ++idx;
      String str_val = parse_string_literal(source, idx);
      emit(word_lit_str, line);
      emit(std::move(str_val), line);
      continue;
    }

    size_t start = idx;
    while (idx < len && !std::isspace(static_cast<unsigned char>(source[idx])))
      ++idx;
    std::string_view token = source.substr(start, idx - start);
    const int line = line_at(source, start);

    if (token == "IF") {
      emit(word_branch_if_zero, line);
      size_t patch_addr = vm.code.size();
      emit(Number{0}, line);
      control_stack.push({ControlType::IfBlock, patch_addr});
      continue;
    }

    if (token == "ELSE") {
      if (control_stack.empty() || control_stack.top().type != ControlType::IfBlock)
        throw std::runtime_error("ELSE without matching IF");
      auto if_entry = control_stack.top();
      control_stack.pop();
      emit(word_branch, line);
      size_t else_patch_addr = vm.code.size();
      emit(Number{0}, line);
      vm.code[if_entry.patch_address] = static_cast<Number>(vm.code.size());
      control_stack.push({ControlType::ElseBlock, else_patch_addr});
      continue;
    }

    if (token == "THEN") {
      if (control_stack.empty())
        throw std::runtime_error("THEN without matching IF or ELSE");
      auto entry = control_stack.top();
      control_stack.pop();
      if (entry.type != ControlType::IfBlock && entry.type != ControlType::ElseBlock)
        throw std::runtime_error("THEN does not close an IF/ELSE");
      vm.code[entry.patch_address] = static_cast<Number>(vm.code.size());
      continue;
    }

    if (token == "BEGIN") {
      control_stack.push({ControlType::BeginBlock, vm.code.size()});
      continue;
    }

    if (token == "UNTIL") {
      if (control_stack.empty() || control_stack.top().type != ControlType::BeginBlock)
        throw std::runtime_error("UNTIL without matching BEGIN");
      auto begin_entry = control_stack.top();
      control_stack.pop();
      emit(word_branch_if_zero, line);
      emit(static_cast<Number>(begin_entry.patch_address), line);
      continue;
    }

    if (token == "WHILE") {
      if (control_stack.empty() || control_stack.top().type != ControlType::BeginBlock)
        throw std::runtime_error("WHILE without matching BEGIN");
      emit(word_branch_if_zero, line);
      size_t exit_patch = vm.code.size();
      emit(Number{0}, line);
      control_stack.push({ControlType::WhileBlock, exit_patch});
      continue;
    }

    if (token == "REPEAT") {
      if (control_stack.size() < 2)
        throw std::runtime_error("REPEAT without BEGIN/WHILE");
      auto while_entry = control_stack.top();
      control_stack.pop();
      auto begin_entry = control_stack.top();
      control_stack.pop();
      if (while_entry.type != ControlType::WhileBlock ||
          begin_entry.type != ControlType::BeginBlock)
        throw std::runtime_error("REPEAT expects BEGIN ... WHILE ...");
      emit(word_branch, line);
      emit(static_cast<Number>(begin_entry.patch_address), line);
      vm.code[while_entry.patch_address] = static_cast<Number>(vm.code.size());
      continue;
    }

    if (token == "DO") {
      emit(word_do, line);
      control_stack.push({ControlType::DoBlock, vm.code.size()});
      continue;
    }

    if (token == "LOOP") {
      if (control_stack.empty() || control_stack.top().type != ControlType::DoBlock)
        throw std::runtime_error("LOOP without matching DO");
      auto do_entry = control_stack.top();
      control_stack.pop();
      emit(word_loop, line);
      emit(static_cast<Number>(do_entry.patch_address), line);
      continue;
    }

    if (token == ":") {
      if (current_colon.has_value())
        throw std::runtime_error("Nested ':' definitions are not supported");
      while (idx < len && std::isspace(static_cast<unsigned char>(source[idx])))
        ++idx;
      size_t n_start = idx;
      while (idx < len && !std::isspace(static_cast<unsigned char>(source[idx])))
        ++idx;
      if (n_start == idx)
        throw std::runtime_error("Expected word name after ':'");
      std::string name(source.substr(n_start, idx - n_start));
      emit(word_branch, line);
      size_t skip_patch = vm.code.size();
      emit(Number{0}, line);
      const size_t body_addr = vm.code.size();
      user_words[name] = body_addr;
      current_colon = ColonDef{std::move(name), skip_patch, body_addr};
      continue;
    }

    if (token == ";") {
      if (!current_colon.has_value())
        throw std::runtime_error("';' without matching ':'");
      emit(word_return, line);
      vm.code[current_colon->skip_patch] = static_cast<Number>(vm.code.size());
      current_colon.reset();
      continue;
    }

    if (token == "SCENE:") {
      while (idx < len && std::isspace(static_cast<unsigned char>(source[idx])))
        ++idx;
      size_t s_start = idx;
      while (idx < len && !std::isspace(static_cast<unsigned char>(source[idx])))
        ++idx;
      std::string scene_name(source.substr(s_start, idx - s_start));
      current_compiling_scene = scene_name;
      vm.scenes[scene_name] = vm.code.size();
      emit(word_set_scene, line);
      emit(scene_name, line);
      continue;
    }

    if (token == ";SCENE") {
      emit(word_scene_end_trap, line);
      emit(current_compiling_scene.empty() ? String{"unknown_scene"} : current_compiling_scene,
           line);
      current_compiling_scene.clear();
      continue;
    }

    if (token == "GOTO:") {
      while (idx < len && std::isspace(static_cast<unsigned char>(source[idx])))
        ++idx;
      size_t s_start = idx;
      while (idx < len && !std::isspace(static_cast<unsigned char>(source[idx])))
        ++idx;
      std::string target_scene(source.substr(s_start, idx - s_start));
      emit(word_goto, line);
      emit(std::move(target_scene), line);
      continue;
    }

    std::string token_str(token);
    bool looks_real = token_str.find('.') != std::string::npos ||
                      token_str.find('e') != std::string::npos ||
                      token_str.find('E') != std::string::npos;
    if (looks_real) {
      char *end = nullptr;
      Real r = std::strtod(token_str.c_str(), &end);
      if (end && end == token_str.c_str() + token_str.size()) {
        emit(word_lit_real, line);
        emit(r, line);
        continue;
      }
    } else {
      Number number = 0;
      auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), number);
      if (ec == std::errc{} && ptr == token.data() + token.size()) {
        emit(word_lit_int, line);
        emit(number, line);
        continue;
      }
    }

    if (auto it = dictionary.find(token_str); it != dictionary.end()) {
      emit(it->second, line);
      continue;
    }

    if (auto uit = user_words.find(token_str); uit != user_words.end()) {
      emit(word_call, line);
      emit(static_cast<Number>(uit->second), line);
      continue;
    }

    throw std::runtime_error("Unknown word: " + token_str);
  }

  if (current_colon.has_value()) {
    throw std::runtime_error("Unclosed ':' definition for word '" + current_colon->name + "'");
  }
  if (!control_stack.empty()) {
    const auto &top = control_stack.top();
    if (top.type == ControlType::BeginBlock)
      throw std::runtime_error("Unbalanced BEGIN (missing UNTIL or REPEAT)");
    if (top.type == ControlType::WhileBlock)
      throw std::runtime_error("WHILE without REPEAT");
    if (top.type == ControlType::DoBlock)
      throw std::runtime_error("DO without LOOP");
    throw std::runtime_error("Unbalanced IF / ELSE / THEN");
  }
}
