#include "vm.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <variant>

namespace fs = std::filesystem;

void VM::push(Value val) { data_stack.push_back(std::move(val)); }

Value VM::pop() {
  if (data_stack.empty()) {
    throw std::runtime_error("Stack underflow");
  }
  Value val = std::move(data_stack.back());
  data_stack.pop_back();
  return val;
}

bool VM::is_numeric(const Value &v) {
  return std::holds_alternative<Number>(v) || std::holds_alternative<Real>(v);
}

Real VM::to_real(const Value &v) {
  if (std::holds_alternative<Number>(v))
    return static_cast<Real>(std::get<Number>(v));
  if (std::holds_alternative<Real>(v))
    return std::get<Real>(v);
  throw std::runtime_error("Type error: number expected");
}

Number VM::pop_int() {
  Value val = pop();
  if (!std::holds_alternative<Number>(val)) {
    throw std::runtime_error("Type error: integer expected");
  }
  return std::get<Number>(val);
}

Real VM::pop_real() {
  Value val = pop();
  return to_real(val);
}

std::string VM::pop_str() {
  Value val = pop();
  if (!std::holds_alternative<std::string>(val)) {
    throw std::runtime_error("Type error: string expected");
  }
  return std::get<std::string>(val);
}

bool VM::pop_truthy() {
  Value val = pop();
  if (std::holds_alternative<Number>(val))
    return std::get<Number>(val) != 0;
  if (std::holds_alternative<Real>(val))
    return std::get<Real>(val) != 0.0;
  if (std::holds_alternative<String>(val))
    return !std::get<String>(val).empty();
  if (std::holds_alternative<Array>(val)) {
    const auto &a = std::get<Array>(val);
    return a && !a->items.empty();
  }
  if (std::holds_alternative<Object>(val)) {
    const auto &o = std::get<Object>(val);
    return o && !o->fields.empty();
  }
  return false;
}

void VM::reset() {
  code.clear();
  code_line.clear();
  data_stack.clear();
  return_stack.clear();
  variables.clear();
  inventory.clear();
  pending_choices.clear();
  scenes.clear();
  canvas_ops.clear();
  is_waiting_choice = false;
  is_waiting_delay = false;
  pending_delay_ms = 0;
  ip = 0;
  outcome = GameOutcome::InProgress;
  finish_message.clear();
  quest_title.clear();
  quest_author.clear();
  quest_version.clear();
  current_location.clear();
  current_scene.clear();
}

void VM::emit_text(const std::string &s) {
  if (onOutput)
    onOutput(s);
}

void VM::emit_canvas(std::string op) {
  canvas_ops.push_back(std::move(op));
  if (onCanvasChanged)
    onCanvasChanged();
}

void VM::run() {
  while (ip < code.size() && !is_waiting_choice && !is_waiting_delay &&
         outcome == GameOutcome::InProgress) {
    const auto &cell = code[ip++];
    if (std::holds_alternative<Primitive>(cell)) {
      std::get<Primitive>(cell)(*this);
    } else {
      throw std::runtime_error("Attempted to execute an operand as an instruction");
    }
  }
}

bool VM::step_once() {
  if (ip >= code.size() || is_waiting_choice || is_waiting_delay ||
      outcome != GameOutcome::InProgress)
    return false;
  const auto &cell = code[ip++];
  if (!std::holds_alternative<Primitive>(cell))
    throw std::runtime_error("Attempted to execute an operand as an instruction");
  std::get<Primitive>(cell)(*this);
  return true;
}

int VM::current_source_line() const {
  if (ip < code_line.size())
    return code_line[ip];
  if (!code_line.empty() && ip > 0 && ip - 1 < code_line.size())
    return code_line[ip - 1];
  return 0;
}

static std::string escape_field(const std::string &s) {
  std::string out;
  out.reserve(s.size());
  for (char c : s) {
    if (c == '\\' || c == '|' || c == '\n' || c == '\r') {
      out.push_back('\\');
      if (c == '\n')
        out.push_back('n');
      else if (c == '\r')
        out.push_back('r');
      else
        out.push_back(c);
    } else {
      out.push_back(c);
    }
  }
  return out;
}

static std::string unescape_field(const std::string &s) {
  std::string out;
  out.reserve(s.size());
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '\\' && i + 1 < s.size()) {
      ++i;
      if (s[i] == 'n')
        out.push_back('\n');
      else if (s[i] == 'r')
        out.push_back('\r');
      else
        out.push_back(s[i]);
    } else {
      out.push_back(s[i]);
    }
  }
  return out;
}

void VM::save_game(const std::string &slot) {
  if (slot.empty())
    throw std::runtime_error("SAVE: empty slot name");

  fs::path dir = save_directory.empty() ? fs::path("saves") : fs::path(save_directory);
  fs::create_directories(dir);
  fs::path path = dir / (slot + ".gqsave");

  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("SAVE: failed to write " + path.string());

  out << "GQSAVE2\n";
  out << "title|" << escape_field(quest_title) << "\n";
  out << "author|" << escape_field(quest_author) << "\n";
  out << "version|" << escape_field(quest_version) << "\n";
  out << "location|" << escape_field(current_location) << "\n";
  out << "scene|" << escape_field(current_scene) << "\n";
  out << "ip|" << ip << "\n";
  out << "outcome|" << static_cast<int>(outcome) << "\n";
  out << "finish|" << escape_field(finish_message) << "\n";

  for (const auto &[name, val] : variables) {
    if (std::holds_alternative<Number>(val)) {
      out << "var|i|" << escape_field(name) << "|" << std::get<Number>(val) << "\n";
    } else if (std::holds_alternative<Real>(val)) {
      out << "var|f|" << escape_field(name) << "|" << std::get<Real>(val) << "\n";
    } else if (std::holds_alternative<String>(val)) {
      out << "var|s|" << escape_field(name) << "|" << escape_field(std::get<String>(val)) << "\n";
    } else {
      // array / object as JSON field
      out << "var|j|" << escape_field(name) << "|" << escape_field(value_to_json(val)) << "\n";
    }
  }
  for (const auto &[item, count] : inventory) {
    out << "inv|" << escape_field(item) << "|" << count << "\n";
  }

  out.close();
  if (onSaved)
    onSaved();
}

void VM::load_game(const std::string &slot) {
  if (slot.empty())
    throw std::runtime_error("LOAD: empty slot name");

  fs::path dir = save_directory.empty() ? fs::path("saves") : fs::path(save_directory);
  fs::path path = dir / (slot + ".gqsave");

  std::ifstream in(path);
  if (!in)
    throw std::runtime_error("LOAD: file not found: " + path.string());

  std::string line;
  if (!std::getline(in, line) || (line != "GQSAVE1" && line != "GQSAVE2"))
    throw std::runtime_error("LOAD: unknown save format");

  variables.clear();
  inventory.clear();
  data_stack.clear();
  return_stack.clear();
  pending_choices.clear();
  is_waiting_choice = false;
  is_waiting_delay = false;

  auto next_field = [](const std::string &s, size_t &pos) -> std::string {
    size_t start = pos;
    while (pos < s.size() && s[pos] != '|')
      ++pos;
    std::string part = s.substr(start, pos - start);
    if (pos < s.size() && s[pos] == '|')
      ++pos;
    return part;
  };

  while (std::getline(in, line)) {
    if (line.empty())
      continue;
    size_t pos = 0;
    std::string tag = next_field(line, pos);
    if (tag == "title") {
      quest_title = unescape_field(next_field(line, pos));
    } else if (tag == "author") {
      quest_author = unescape_field(next_field(line, pos));
    } else if (tag == "version") {
      quest_version = unescape_field(next_field(line, pos));
    } else if (tag == "location") {
      current_location = unescape_field(next_field(line, pos));
    } else if (tag == "scene") {
      current_scene = unescape_field(next_field(line, pos));
    } else if (tag == "ip") {
      ip = static_cast<size_t>(std::stoll(next_field(line, pos)));
    } else if (tag == "outcome") {
      outcome = static_cast<GameOutcome>(std::stoi(next_field(line, pos)));
    } else if (tag == "finish") {
      finish_message = unescape_field(next_field(line, pos));
    } else if (tag == "var") {
      std::string kind = next_field(line, pos);
      std::string name = unescape_field(next_field(line, pos));
      std::string raw = next_field(line, pos);
      if (kind == "i")
        variables[name] = static_cast<Number>(std::stoll(raw));
      else if (kind == "f")
        variables[name] = std::stod(raw);
      else if (kind == "j")
        variables[name] = value_from_json(unescape_field(raw));
      else
        variables[name] = unescape_field(raw);
    } else if (tag == "inv") {
      std::string item = unescape_field(next_field(line, pos));
      size_t count = static_cast<size_t>(std::stoull(next_field(line, pos)));
      inventory[item] = count;
    }
  }

  // Prefer resuming at the saved scene start when known
  if (!current_scene.empty()) {
    auto it = scenes.find(current_scene);
    if (it != scenes.end())
      ip = it->second;
  }

  if (onLoaded)
    onLoaded();
  if (onInventoryChanged)
    onInventoryChanged();
  if (onTitleChanged)
    onTitleChanged(quest_title);
  if (onAuthorChanged)
    onAuthorChanged(quest_author);
  if (onVersionChanged)
    onVersionChanged(quest_version);
  if (onLocationChanged)
    onLocationChanged(current_location);
}
