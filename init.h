#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using Number = int64_t;
using Real = double;
using String = std::string;

class VM;

struct ArrayData;
struct ObjectData;

using Array = std::shared_ptr<ArrayData>;
using Object = std::shared_ptr<ObjectData>;

using Value = std::variant<Number, Real, String, Array, Object>;

struct ArrayData {
  std::vector<Value> items;
};

struct ObjectData {
  std::unordered_map<std::string, Value> fields;
};

using Primitive = void (*)(VM &);
using Cell = std::variant<Primitive, Number, Real, String>;

enum class ScriptError { EmptyStack, NumberExpected, StringExpected };

enum class GameOutcome { InProgress, Victory, Defeat };

inline Array make_array() { return std::make_shared<ArrayData>(); }

inline Object make_object() { return std::make_shared<ObjectData>(); }

inline bool is_array(const Value &v) { return std::holds_alternative<Array>(v); }
inline bool is_object(const Value &v) { return std::holds_alternative<Object>(v); }

Array expect_array(Value v);
Object expect_object(Value v);
bool values_equal(const Value &a, const Value &b);
std::string value_to_string(const Value &v);
std::string value_to_json(const Value &v);
Value value_from_json(std::string_view s);
