#include "words.h"

#include <cstdio>
#include <iomanip>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

namespace {

bool truthy_value(const Value &v) {
  if (std::holds_alternative<Number>(v))
    return std::get<Number>(v) != 0;
  if (std::holds_alternative<Real>(v))
    return std::get<Real>(v) != 0.0;
  if (std::holds_alternative<String>(v))
    return !std::get<String>(v).empty();
  if (std::holds_alternative<Array>(v)) {
    const auto &a = std::get<Array>(v);
    return a && !a->items.empty();
  }
  if (std::holds_alternative<Object>(v)) {
    const auto &o = std::get<Object>(v);
    return o && !o->fields.empty();
  }
  return false;
}

void cmp_reals(VM &vm, bool (*pred)(Real, Real)) {
  Value b = vm.pop();
  Value a = vm.pop();
  if (!VM::is_numeric(a) || !VM::is_numeric(b))
    throw std::runtime_error("Comparison: numbers expected");
  vm.push(pred(VM::to_real(a), VM::to_real(b)) ? Number{1} : Number{0});
}

bool pred_gt(Real a, Real b) { return a > b; }
bool pred_ge(Real a, Real b) { return a >= b; }
bool pred_lt(Real a, Real b) { return a < b; }
bool pred_le(Real a, Real b) { return a <= b; }

} // namespace

void word_lit_int(VM &vm) {
  if (vm.ip >= vm.code.size())
    throw std::runtime_error("Expected integer after LIT_INT");
  vm.push(std::get<Number>(vm.code[vm.ip++]));
}

void word_lit_real(VM &vm) {
  if (vm.ip >= vm.code.size())
    throw std::runtime_error("Expected real after LIT_REAL");
  vm.push(std::get<Real>(vm.code[vm.ip++]));
}

void word_lit_str(VM &vm) {
  if (vm.ip >= vm.code.size())
    throw std::runtime_error("Expected string after LIT_STR");
  vm.push(std::get<String>(vm.code[vm.ip++]));
}

void word_add(VM &vm) {
  Value right = vm.pop();
  Value left = vm.pop();
  if (std::holds_alternative<String>(left) && std::holds_alternative<String>(right)) {
    vm.push(std::get<String>(left) + std::get<String>(right));
    return;
  }
  if (VM::is_numeric(left) && VM::is_numeric(right)) {
    const bool any_real = std::holds_alternative<Real>(left) || std::holds_alternative<Real>(right);
    const Real r = VM::to_real(left) + VM::to_real(right);
    if (any_real)
      vm.push(r);
    else
      vm.push(static_cast<Number>(r));
    return;
  }
  throw std::runtime_error("Type error in '+'");
}

void word_sub(VM &vm) {
  Value right = vm.pop();
  Value left = vm.pop();
  if (!VM::is_numeric(left) || !VM::is_numeric(right))
    throw std::runtime_error("Type error in '-'");
  const bool any_real = std::holds_alternative<Real>(left) || std::holds_alternative<Real>(right);
  const Real r = VM::to_real(left) - VM::to_real(right);
  if (any_real)
    vm.push(r);
  else
    vm.push(static_cast<Number>(r));
}

void word_mul(VM &vm) {
  Value right = vm.pop();
  Value left = vm.pop();
  if (!VM::is_numeric(left) || !VM::is_numeric(right))
    throw std::runtime_error("Type error in '*'");
  const bool any_real = std::holds_alternative<Real>(left) || std::holds_alternative<Real>(right);
  const Real r = VM::to_real(left) * VM::to_real(right);
  if (any_real)
    vm.push(r);
  else
    vm.push(static_cast<Number>(r));
}

void word_div(VM &vm) {
  Value right = vm.pop();
  Value left = vm.pop();
  if (!VM::is_numeric(left) || !VM::is_numeric(right))
    throw std::runtime_error("Type error in '/'");
  const bool any_real = std::holds_alternative<Real>(left) || std::holds_alternative<Real>(right);
  if (!any_real) {
    Number b = std::get<Number>(right);
    Number a = std::get<Number>(left);
    if (b == 0)
      throw std::runtime_error("Division by zero in '/'");
    vm.push(a / b);
    return;
  }
  const Real b = VM::to_real(right);
  if (b == 0.0)
    throw std::runtime_error("Division by zero in '/'");
  vm.push(VM::to_real(left) / b);
}

void word_mod(VM &vm) {
  Number b = vm.pop_int();
  Number a = vm.pop_int();
  if (b == 0)
    throw std::runtime_error("Division by zero in 'MOD'");
  vm.push(a % b);
}

void word_random(VM &vm) {
  Number max_val = vm.pop_int();
  Number min_val = vm.pop_int();
  if (min_val == max_val) {
    vm.push(min_val);
    return;
  }
  if (min_val > max_val)
    throw std::runtime_error("RANDOM: min greater than max");
  std::uniform_int_distribution<Number> dist(min_val, max_val);
  vm.push(dist(vm.rng));
}

void word_dup(VM &vm) {
  if (vm.data_stack.empty())
    throw std::runtime_error("Stack underflow");
  vm.push(vm.data_stack.back());
}

void word_drop(VM &vm) { vm.pop(); }

void word_swap(VM &vm) {
  Value b = vm.pop();
  Value a = vm.pop();
  vm.push(std::move(b));
  vm.push(std::move(a));
}

void word_dot(VM &vm) {
  Value val = vm.pop();
  vm.emit_text(value_to_string(val));
}

void word_store(VM &vm) {
  std::string name = vm.pop_str();
  Value val = vm.pop();
  vm.variables[std::move(name)] = std::move(val);
}

void word_fetch(VM &vm) {
  std::string name = vm.pop_str();
  auto it = vm.variables.find(name);
  if (it != vm.variables.end())
    vm.push(it->second);
  else
    vm.push(Number{0});
}

void word_item_add(VM &vm) {
  std::string item = vm.pop_str();
  Number count = vm.pop_int();
  if (count <= 0)
    return;
  vm.inventory[std::move(item)] += static_cast<size_t>(count);
  if (vm.onInventoryChanged)
    vm.onInventoryChanged();
}

void word_item_remove(VM &vm) {
  std::string item = vm.pop_str();
  Number count = vm.pop_int();
  if (count <= 0)
    return;
  auto it = vm.inventory.find(item);
  if (it == vm.inventory.end())
    return;
  if (it->second <= static_cast<size_t>(count))
    vm.inventory.erase(it);
  else
    it->second -= static_cast<size_t>(count);
  if (vm.onInventoryChanged)
    vm.onInventoryChanged();
}

void word_item_count(VM &vm) {
  std::string item = vm.pop_str();
  auto it = vm.inventory.find(item);
  vm.push(static_cast<Number>(it != vm.inventory.end() ? it->second : 0));
}

void word_inventory_print(VM &vm) {
  vm.emit_text("--- Inventory ---");
  if (vm.inventory.empty()) {
    vm.emit_text("[Empty]");
  } else {
    for (const auto &[item, count] : vm.inventory)
      vm.emit_text(" - " + item + ": " + std::to_string(count));
  }
  vm.emit_text("-----------------------");
}

void word_title(VM &vm) {
  std::string title = vm.pop_str();
  vm.quest_title = title;
  if (vm.onTitleChanged)
    vm.onTitleChanged(title);
}

void word_author(VM &vm) {
  std::string author = vm.pop_str();
  vm.quest_author = author;
  if (vm.onAuthorChanged)
    vm.onAuthorChanged(author);
}

void word_version(VM &vm) {
  std::string ver = vm.pop_str();
  vm.quest_version = ver;
  if (vm.onVersionChanged)
    vm.onVersionChanged(ver);
}

void word_location(VM &vm) {
  std::string loc = vm.pop_str();
  vm.current_location = loc;
  if (vm.onLocationChanged)
    vm.onLocationChanged(loc);
}

void word_branch_if_zero(VM &vm) {
  Number target_ip = std::get<Number>(vm.code[vm.ip++]);
  if (!vm.pop_truthy())
    vm.ip = static_cast<size_t>(target_ip);
}

void word_branch(VM &vm) {
  Number target_ip = std::get<Number>(vm.code[vm.ip++]);
  vm.ip = static_cast<size_t>(target_ip);
}

void word_call(VM &vm) {
  if (vm.ip >= vm.code.size() || !std::holds_alternative<Number>(vm.code[vm.ip]))
    throw std::runtime_error("CALL: expected word body address");
  const Number target = std::get<Number>(vm.code[vm.ip++]);
  vm.return_stack.push_back(vm.ip);
  vm.ip = static_cast<size_t>(target);
}

void word_return(VM &vm) {
  if (vm.return_stack.empty())
    throw std::runtime_error("RETURN: return stack empty");
  vm.ip = vm.return_stack.back();
  vm.return_stack.pop_back();
}

void word_do(VM &vm) {
  Number start = vm.pop_int();
  Number limit = vm.pop_int();
  vm.return_stack.push_back(static_cast<size_t>(limit));
  vm.return_stack.push_back(static_cast<size_t>(start));
}

void word_loop(VM &vm) {
  if (vm.return_stack.size() < 2)
    throw std::runtime_error("LOOP without active DO");
  Number target = std::get<Number>(vm.code[vm.ip++]);
  size_t index = vm.return_stack.back();
  size_t limit = vm.return_stack[vm.return_stack.size() - 2];
  ++index;
  if (index < limit) {
    vm.return_stack.back() = index;
    vm.ip = static_cast<size_t>(target);
  } else {
    vm.return_stack.pop_back();
    vm.return_stack.pop_back();
  }
}

void word_i(VM &vm) {
  if (vm.return_stack.size() < 2)
    throw std::runtime_error("I outside DO loop");
  vm.push(static_cast<Number>(vm.return_stack.back()));
}

void word_eq(VM &vm) {
  Value b = vm.pop();
  Value a = vm.pop();
  vm.push(values_equal(a, b) ? Number{1} : Number{0});
}

void word_gt(VM &vm) { cmp_reals(vm, pred_gt); }
void word_gt_or_eq(VM &vm) { cmp_reals(vm, pred_ge); }
void word_lt(VM &vm) { cmp_reals(vm, pred_lt); }
void word_lt_or_eq(VM &vm) { cmp_reals(vm, pred_le); }

void word_and(VM &vm) {
  Value b = vm.pop();
  Value a = vm.pop();
  vm.push((truthy_value(a) && truthy_value(b)) ? Number{1} : Number{0});
}

void word_or(VM &vm) {
  Value b = vm.pop();
  Value a = vm.pop();
  vm.push((truthy_value(a) || truthy_value(b)) ? Number{1} : Number{0});
}

void word_not(VM &vm) {
  Value a = vm.pop();
  vm.push(truthy_value(a) ? Number{0} : Number{1});
}

void word_choice(VM &vm) { vm.pending_choices.push_back(vm.pop_str()); }

void word_wait_choice(VM &vm) {
  if (vm.pending_choices.empty())
    throw std::runtime_error("WAIT_CHOICE with no CHOICE");
  if (vm.onChoicesChanged)
    vm.onChoicesChanged(vm.pending_choices);
  vm.is_waiting_choice = true;
}

void word_set_scene(VM &vm) { vm.current_scene = std::get<String>(vm.code[vm.ip++]); }

void word_goto(VM &vm) {
  std::string scene_name = std::get<String>(vm.code[vm.ip++]);
  auto it = vm.scenes.find(scene_name);
  if (it == vm.scenes.end())
    throw std::runtime_error("Scene not found: " + scene_name);
  vm.current_scene = scene_name;
  vm.ip = it->second;
}

void word_cls(VM &vm) { vm.emit_text("__CLEAR_SCREEN__"); }

void word_victory(VM &vm) {
  vm.finish_message = vm.pop_str();
  vm.outcome = GameOutcome::Victory;
  vm.pending_choices.clear();
  if (vm.onChoicesChanged)
    vm.onChoicesChanged({});
  if (vm.onFinished)
    vm.onFinished(GameOutcome::Victory, vm.finish_message);
}

void word_defeat(VM &vm) {
  vm.finish_message = vm.pop_str();
  vm.outcome = GameOutcome::Defeat;
  vm.pending_choices.clear();
  if (vm.onChoicesChanged)
    vm.onChoicesChanged({});
  if (vm.onFinished)
    vm.onFinished(GameOutcome::Defeat, vm.finish_message);
}

void word_halt(VM &vm) {
  vm.is_waiting_choice = false;
  vm.is_waiting_delay = false;
  vm.ip = vm.code.size();
}

void word_scene_end_trap(VM &vm) {
  std::string scene_name = std::get<String>(vm.code[vm.ip++]);
  vm.emit_text("[Warning: scene '" + scene_name +
               "' ended without WAIT_CHOICE, GOTO, or an ending]");
  vm.is_waiting_choice = false;
  vm.ip = vm.code.size();
}

void word_delay(VM &vm) {
  Number ms = vm.pop_int();
  if (ms < 0)
    ms = 0;
  vm.pending_delay_ms = ms;
  vm.is_waiting_delay = true;
  if (vm.onDelay)
    vm.onDelay(ms);
  else {
    // Without GUI callback, DELAY is immediate (unit tests)
    vm.is_waiting_delay = false;
    vm.pending_delay_ms = 0;
  }
}

void word_save(VM &vm) { vm.save_game(vm.pop_str()); }

void word_load(VM &vm) { vm.load_game(vm.pop_str()); }

void word_canvas_clear(VM &vm) {
  vm.canvas_ops.clear();
  vm.emit_canvas("CLEAR");
}

void word_canvas_color(VM &vm) {
  Number b = vm.pop_int();
  Number g = vm.pop_int();
  Number r = vm.pop_int();
  vm.emit_canvas("COLOR " + std::to_string(r) + " " + std::to_string(g) + " " + std::to_string(b));
}

void word_canvas_rect(VM &vm) {
  Number h = vm.pop_int();
  Number w = vm.pop_int();
  Number y = vm.pop_int();
  Number x = vm.pop_int();
  vm.emit_canvas("RECT " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(w) +
                 " " + std::to_string(h));
}

void word_canvas_line(VM &vm) {
  Number y2 = vm.pop_int();
  Number x2 = vm.pop_int();
  Number y1 = vm.pop_int();
  Number x1 = vm.pop_int();
  vm.emit_canvas("LINE " + std::to_string(x1) + " " + std::to_string(y1) + " " +
                 std::to_string(x2) + " " + std::to_string(y2));
}

void word_canvas_text(VM &vm) {
  std::string text = vm.pop_str();
  Number y = vm.pop_int();
  Number x = vm.pop_int();
  vm.emit_canvas("TEXT " + std::to_string(x) + " " + std::to_string(y) + " " +
                 std::to_string(text.size()) + " " + text);
}

void word_array(VM &vm) {
  Number n = vm.pop_int();
  if (n < 0)
    throw std::runtime_error("ARRAY: negative length");
  auto arr = make_array();
  arr->items.resize(static_cast<size_t>(n));
  for (Number i = n - 1; i >= 0; --i)
    arr->items[static_cast<size_t>(i)] = vm.pop();
  vm.push(arr);
}

void word_object(VM &vm) {
  Number n = vm.pop_int();
  if (n < 0)
    throw std::runtime_error("OBJECT: negative pair count");
  auto obj = make_object();
  for (Number i = 0; i < n; ++i) {
    Value val = vm.pop();
    std::string key = vm.pop_str();
    obj->fields[std::move(key)] = std::move(val);
  }
  vm.push(obj);
}

void word_len(VM &vm) {
  Value v = vm.pop();
  if (std::holds_alternative<Array>(v)) {
    const auto &a = expect_array(std::move(v));
    vm.push(static_cast<Number>(a->items.size()));
    return;
  }
  if (std::holds_alternative<Object>(v)) {
    const auto &o = expect_object(std::move(v));
    vm.push(static_cast<Number>(o->fields.size()));
    return;
  }
  if (std::holds_alternative<String>(v)) {
    vm.push(static_cast<Number>(std::get<String>(v).size()));
    return;
  }
  throw std::runtime_error("LEN: array, object, or string expected");
}

void word_array_fetch(VM &vm) {
  Number idx = vm.pop_int();
  Array arr = expect_array(vm.pop());
  if (idx < 0 || static_cast<size_t>(idx) >= arr->items.size())
    throw std::runtime_error("[]@: index out of range");
  vm.push(arr->items[static_cast<size_t>(idx)]);
}

void word_array_store(VM &vm) {
  Value val = vm.pop();
  Number idx = vm.pop_int();
  Array arr = expect_array(vm.pop());
  if (idx < 0 || static_cast<size_t>(idx) >= arr->items.size())
    throw std::runtime_error("[]!: index out of range");
  arr->items[static_cast<size_t>(idx)] = std::move(val);
}

void word_append(VM &vm) {
  Value val = vm.pop();
  Array arr = expect_array(vm.pop());
  arr->items.push_back(std::move(val));
}

void word_array_pop(VM &vm) {
  Array arr = expect_array(vm.pop());
  if (arr->items.empty())
    throw std::runtime_error("[]POP: array is empty");
  Value val = std::move(arr->items.back());
  arr->items.pop_back();
  vm.push(std::move(val));
}

void word_object_fetch(VM &vm) {
  std::string key = vm.pop_str();
  Object obj = expect_object(vm.pop());
  auto it = obj->fields.find(key);
  if (it != obj->fields.end())
    vm.push(it->second);
  else
    vm.push(Number{0});
}

void word_object_store(VM &vm) {
  Value val = vm.pop();
  std::string key = vm.pop_str();
  Object obj = expect_object(vm.pop());
  obj->fields[std::move(key)] = std::move(val);
}

void word_has(VM &vm) {
  std::string key = vm.pop_str();
  Object obj = expect_object(vm.pop());
  vm.push(obj->fields.count(key) ? Number{1} : Number{0});
}

void word_del(VM &vm) {
  std::string key = vm.pop_str();
  Object obj = expect_object(vm.pop());
  obj->fields.erase(key);
}

void word_keys(VM &vm) {
  Object obj = expect_object(vm.pop());
  auto arr = make_array();
  arr->items.reserve(obj->fields.size());
  for (const auto &[k, _] : obj->fields)
    arr->items.push_back(k);
  vm.push(arr);
}

void register_words(std::unordered_map<std::string, Primitive> &dictionary) {
  dictionary["+"] = word_add;
  dictionary["-"] = word_sub;
  dictionary["*"] = word_mul;
  dictionary["/"] = word_div;
  dictionary["MOD"] = word_mod;
  dictionary["dup"] = word_dup;
  dictionary["drop"] = word_drop;
  dictionary["swap"] = word_swap;
  dictionary["."] = word_dot;

  dictionary["="] = word_eq;
  dictionary[">"] = word_gt;
  dictionary["<"] = word_lt;
  dictionary["<="] = word_lt_or_eq;
  dictionary[">="] = word_gt_or_eq;
  dictionary["AND"] = word_and;
  dictionary["OR"] = word_or;
  dictionary["NOT"] = word_not;

  dictionary["!"] = word_store;
  dictionary["@"] = word_fetch;
  dictionary["ITEM+"] = word_item_add;
  dictionary["ITEM-"] = word_item_remove;
  dictionary["ITEM?"] = word_item_count;
  dictionary[".INVENTORY"] = word_inventory_print;

  dictionary["ARRAY"] = word_array;
  dictionary["OBJECT"] = word_object;
  dictionary["LEN"] = word_len;
  dictionary["[]@"] = word_array_fetch;
  dictionary["[]!"] = word_array_store;
  dictionary["APPEND"] = word_append;
  dictionary["[]POP"] = word_array_pop;
  dictionary["{}@"] = word_object_fetch;
  dictionary["{}!"] = word_object_store;
  dictionary["HAS?"] = word_has;
  dictionary["DEL"] = word_del;
  dictionary["KEYS"] = word_keys;

  dictionary["RANDOM"] = word_random;
  dictionary["I"] = word_i;

  dictionary["TITLE:"] = word_title;
  dictionary["AUTHOR:"] = word_author;
  dictionary["VERSION:"] = word_version;
  dictionary["LOCATION:"] = word_location;

  dictionary["CHOICE"] = word_choice;
  dictionary["WAIT_CHOICE"] = word_wait_choice;

  dictionary["CLS"] = word_cls;
  dictionary["HALT"] = word_halt;
  dictionary["DELAY"] = word_delay;
  dictionary["SAVE"] = word_save;
  dictionary["LOAD"] = word_load;

  dictionary["CANVAS.CLEAR"] = word_canvas_clear;
  dictionary["CANVAS.COLOR"] = word_canvas_color;
  dictionary["CANVAS.RECT"] = word_canvas_rect;
  dictionary["CANVAS.LINE"] = word_canvas_line;
  dictionary["CANVAS.TEXT"] = word_canvas_text;

  dictionary["VICTORY"] = word_victory;
  dictionary["DEFEAT"] = word_defeat;
  dictionary["FINISH"] = word_victory;
}

namespace {

const std::unordered_map<Primitive, std::string> &primitive_names() {
  static const std::unordered_map<Primitive, std::string> names = [] {
    std::unordered_map<Primitive, std::string> m;
    std::unordered_map<std::string, Primitive> dict;
    register_words(dict);
    // Prefer Forth spellings; first registration wins for shared pointers.
    for (const auto &[k, v] : dict) {
      if (!m.count(v))
        m.emplace(v, k);
    }
    // Compiler-internal opcodes (not in the user dictionary)
    m[word_lit_int] = "LIT_INT";
    m[word_lit_real] = "LIT_REAL";
    m[word_lit_str] = "LIT_STR";
    m[word_branch_if_zero] = "0BRANCH";
    m[word_branch] = "BRANCH";
    m[word_call] = "CALL";
    m[word_return] = "RETURN";
    m[word_do] = "DO";
    m[word_loop] = "LOOP";
    m[word_set_scene] = "SET_SCENE";
    m[word_goto] = "GOTO";
    m[word_scene_end_trap] = "SCENE_END";
    return m;
  }();
  return names;
}

bool opcode_takes_operand(const std::string &name) {
  return name == "LIT_INT" || name == "LIT_REAL" || name == "LIT_STR" || name == "0BRANCH" ||
         name == "BRANCH" || name == "CALL" || name == "LOOP" || name == "SET_SCENE" ||
         name == "GOTO" || name == "SCENE_END";
}

std::string format_operand(const Cell &cell) {
  if (std::holds_alternative<Number>(cell))
    return std::to_string(std::get<Number>(cell));
  if (std::holds_alternative<Real>(cell)) {
    std::ostringstream os;
    os << std::get<Real>(cell);
    return os.str();
  }
  if (std::holds_alternative<String>(cell)) {
    std::string s = std::get<String>(cell);
    std::string out = "\"";
    for (char c : s) {
      if (c == '\\' || c == '"')
        out.push_back('\\');
      if (c == '\n') {
        out += "\\n";
        continue;
      }
      if (c == '\t') {
        out += "\\t";
        continue;
      }
      out.push_back(c);
    }
    out.push_back('"');
    return out;
  }
  return "<primitive?>";
}

} // namespace

std::string primitive_name(Primitive p) {
  const auto &names = primitive_names();
  if (auto it = names.find(p); it != names.end())
    return it->second;
  char buf[64];
  std::snprintf(buf, sizeof(buf), "PRIM@%p", reinterpret_cast<void *>(p));
  return buf;
}

namespace {

size_t utf8_codepoints(const std::string &s) {
  size_t n = 0;
  for (unsigned char c : s) {
    if ((c & 0xc0) != 0x80)
      ++n;
  }
  return n;
}

std::string utf8_prefix(const std::string &s, size_t max_chars) {
  size_t n = 0;
  size_t i = 0;
  while (i < s.size() && n < max_chars) {
    const unsigned char c = static_cast<unsigned char>(s[i]);
    size_t cp = 1;
    if ((c & 0x80) == 0)
      cp = 1;
    else if ((c & 0xe0) == 0xc0)
      cp = 2;
    else if ((c & 0xf0) == 0xe0)
      cp = 3;
    else if ((c & 0xf8) == 0xf0)
      cp = 4;
    if (i + cp > s.size())
      break;
    i += cp;
    ++n;
  }
  return s.substr(0, i);
}

/// Fit to exactly `width` display columns (UTF-8 code points); truncate with … if needed.
std::string fit_column(const std::string &s, int width) {
  if (width <= 0)
    return {};
  const size_t len = utf8_codepoints(s);
  if (static_cast<int>(len) <= width)
    return s + std::string(static_cast<size_t>(width - static_cast<int>(len)), ' ');
  if (width == 1)
    return "…";
  return utf8_prefix(s, static_cast<size_t>(width - 1)) + "…";
}

std::string pad_right_ascii(const std::string &s, int width) {
  if (static_cast<int>(s.size()) >= width)
    return s;
  return std::string(static_cast<size_t>(width - static_cast<int>(s.size())), ' ') + s;
}

} // namespace

std::vector<DisassemblyLine> disassemble_vm_lines(const VM &vm) {
  // Fixed columns (monospace, UTF-8 code-point width):
  //   AAAAA  OPCODE________  OPERAND______________________________  ; line N
  constexpr int kAddrW = 5;
  constexpr int kOpcodeW = 12;
  constexpr int kOperandW = 48;

  auto make_row = [&](size_t addr, const std::string &opcode, const std::string &operand,
                      int src_line) -> DisassemblyLine {
    DisassemblyLine row;
    const bool truncated = utf8_codepoints(operand) > static_cast<size_t>(kOperandW);
    row.text = pad_right_ascii(std::to_string(addr), kAddrW) + "  " + fit_column(opcode, kOpcodeW) +
               "  " + fit_column(operand, kOperandW);
    if (src_line > 0)
      row.text += "  ; line " + std::to_string(src_line);
    if (truncated)
      row.tooltip = operand;
    return row;
  };

  std::vector<DisassemblyLine> lines;

  {
    std::string header = "; QuestForth bytecode — " + std::to_string(vm.code.size()) + " cells";
    if (!vm.scenes.empty())
      header += ", " + std::to_string(vm.scenes.size()) + " scenes";
    lines.push_back({header, {}});
  }
  lines.push_back({"; " + pad_right_ascii("addr", kAddrW) + "  " + fit_column("opcode", kOpcodeW) +
                     "  " + fit_column("operand", kOperandW) + "  comment",
                   {}});

  if (!vm.scenes.empty()) {
    lines.push_back({"; --- scenes ---", {}});
    std::map<size_t, std::string> by_addr;
    for (const auto &[name, addr] : vm.scenes)
      by_addr[addr] = name;
    for (const auto &[addr, name] : by_addr)
      lines.push_back({";   SCENE " + name + " @ " + std::to_string(addr), {}});
    lines.push_back({"; --------------", {}});
  }

  std::unordered_map<size_t, std::string> scene_at;
  for (const auto &[name, addr] : vm.scenes)
    scene_at[addr] = name;

  size_t i = 0;
  while (i < vm.code.size()) {
    if (auto sit = scene_at.find(i); sit != scene_at.end())
      lines.push_back({"; >>> SCENE: " + sit->second, {}});

    const int src_line = (i < vm.code_line.size()) ? vm.code_line[i] : 0;

    if (!std::holds_alternative<Primitive>(vm.code[i])) {
      lines.push_back(make_row(i, "???", format_operand(vm.code[i]), src_line));
      ++i;
      continue;
    }

    const Primitive prim = std::get<Primitive>(vm.code[i]);
    const std::string name = primitive_name(prim);

    if (opcode_takes_operand(name) && i + 1 < vm.code.size()) {
      lines.push_back(make_row(i, name, format_operand(vm.code[i + 1]), src_line));
      i += 2;
    } else {
      lines.push_back(make_row(i, name, "", src_line));
      ++i;
    }
  }

  return lines;
}

std::string disassemble_vm(const VM &vm) {
  const auto lines = disassemble_vm_lines(vm);
  std::ostringstream out;
  for (size_t i = 0; i < lines.size(); ++i) {
    if (i)
      out << '\n';
    out << lines[i].text;
  }
  return out.str();
}
