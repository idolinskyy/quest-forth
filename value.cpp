#include "init.h"

#include <cctype>
#include <charconv>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace {

void skip_ws(std::string_view s, size_t &i) {
  while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i])))
    ++i;
}

std::string json_escape(std::string_view in) {
  std::string out;
  out.reserve(in.size() + 8);
  for (char c : in) {
    switch (c) {
    case '"':
      out += "\\\"";
      break;
    case '\\':
      out += "\\\\";
      break;
    case '\n':
      out += "\\n";
      break;
    case '\r':
      out += "\\r";
      break;
    case '\t':
      out += "\\t";
      break;
    default:
      out.push_back(c);
      break;
    }
  }
  return out;
}

Value parse_json_value(std::string_view s, size_t &i);

std::string parse_json_string(std::string_view s, size_t &i) {
  if (i >= s.size() || s[i] != '"')
    throw std::runtime_error("JSON: string expected");
  ++i;
  std::string out;
  while (i < s.size() && s[i] != '"') {
    if (s[i] == '\\') {
      ++i;
      if (i >= s.size())
        throw std::runtime_error("JSON: truncated escape");
      switch (s[i]) {
      case '"':
      case '\\':
      case '/':
        out.push_back(s[i]);
        break;
      case 'n':
        out.push_back('\n');
        break;
      case 'r':
        out.push_back('\r');
        break;
      case 't':
        out.push_back('\t');
        break;
      default:
        throw std::runtime_error("JSON: unknown escape");
      }
      ++i;
    } else {
      out.push_back(s[i++]);
    }
  }
  if (i >= s.size() || s[i] != '"')
    throw std::runtime_error("JSON: unterminated string");
  ++i;
  return out;
}

Value parse_json_value(std::string_view s, size_t &i) {
  skip_ws(s, i);
  if (i >= s.size())
    throw std::runtime_error("JSON: unexpected end");

  if (s[i] == '"')
    return parse_json_string(s, i);

  if (s[i] == '[') {
    ++i;
    auto arr = make_array();
    skip_ws(s, i);
    if (i < s.size() && s[i] == ']') {
      ++i;
      return arr;
    }
    while (true) {
      arr->items.push_back(parse_json_value(s, i));
      skip_ws(s, i);
      if (i < s.size() && s[i] == ',') {
        ++i;
        continue;
      }
      if (i < s.size() && s[i] == ']') {
        ++i;
        break;
      }
      throw std::runtime_error("JSON: expected ',' or ']'");
    }
    return arr;
  }

  if (s[i] == '{') {
    ++i;
    auto obj = make_object();
    skip_ws(s, i);
    if (i < s.size() && s[i] == '}') {
      ++i;
      return obj;
    }
    while (true) {
      skip_ws(s, i);
      std::string key = parse_json_string(s, i);
      skip_ws(s, i);
      if (i >= s.size() || s[i] != ':')
        throw std::runtime_error("JSON: expected ':'");
      ++i;
      obj->fields[std::move(key)] = parse_json_value(s, i);
      skip_ws(s, i);
      if (i < s.size() && s[i] == ',') {
        ++i;
        continue;
      }
      if (i < s.size() && s[i] == '}') {
        ++i;
        break;
      }
      throw std::runtime_error("JSON: expected ',' or '}'");
    }
    return obj;
  }

  // number
  size_t start = i;
  if (s[i] == '-' || s[i] == '+')
    ++i;
  bool saw_digit = false;
  while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
    saw_digit = true;
    ++i;
  }
  bool is_real = false;
  if (i < s.size() && s[i] == '.') {
    is_real = true;
    ++i;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
      saw_digit = true;
      ++i;
    }
  }
  if (i < s.size() && (s[i] == 'e' || s[i] == 'E')) {
    is_real = true;
    ++i;
    if (i < s.size() && (s[i] == '+' || s[i] == '-'))
      ++i;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])))
      ++i;
  }
  if (!saw_digit)
    throw std::runtime_error("JSON: value expected");
  std::string num(s.substr(start, i - start));
  if (is_real)
    return std::stod(num);
  return static_cast<Number>(std::stoll(num));
}

} // namespace

Array expect_array(Value v) {
  if (!std::holds_alternative<Array>(v) || !std::get<Array>(v))
    throw std::runtime_error("Type error: array expected");
  return std::get<Array>(v);
}

Object expect_object(Value v) {
  if (!std::holds_alternative<Object>(v) || !std::get<Object>(v))
    throw std::runtime_error("Type error: object expected");
  return std::get<Object>(v);
}

bool values_equal(const Value &a, const Value &b) {
  if (a.index() != b.index()) {
    // numeric int/real compare
    if ((std::holds_alternative<Number>(a) || std::holds_alternative<Real>(a)) &&
        (std::holds_alternative<Number>(b) || std::holds_alternative<Real>(b))) {
      Real ra = std::holds_alternative<Number>(a) ? static_cast<Real>(std::get<Number>(a))
                                                  : std::get<Real>(a);
      Real rb = std::holds_alternative<Number>(b) ? static_cast<Real>(std::get<Number>(b))
                                                  : std::get<Real>(b);
      return ra == rb;
    }
    return false;
  }
  if (std::holds_alternative<Number>(a))
    return std::get<Number>(a) == std::get<Number>(b);
  if (std::holds_alternative<Real>(a))
    return std::get<Real>(a) == std::get<Real>(b);
  if (std::holds_alternative<String>(a))
    return std::get<String>(a) == std::get<String>(b);
  if (std::holds_alternative<Array>(a)) {
    const auto &aa = std::get<Array>(a);
    const auto &bb = std::get<Array>(b);
    if (!aa || !bb)
      return aa == bb;
    if (aa.get() == bb.get())
      return true;
    if (aa->items.size() != bb->items.size())
      return false;
    for (size_t i = 0; i < aa->items.size(); ++i) {
      if (!values_equal(aa->items[i], bb->items[i]))
        return false;
    }
    return true;
  }
  if (std::holds_alternative<Object>(a)) {
    const auto &oa = std::get<Object>(a);
    const auto &ob = std::get<Object>(b);
    if (!oa || !ob)
      return oa == ob;
    if (oa.get() == ob.get())
      return true;
    if (oa->fields.size() != ob->fields.size())
      return false;
    for (const auto &[k, va] : oa->fields) {
      auto it = ob->fields.find(k);
      if (it == ob->fields.end() || !values_equal(va, it->second))
        return false;
    }
    return true;
  }
  return false;
}

std::string value_to_string(const Value &v) {
  if (std::holds_alternative<Number>(v))
    return std::to_string(std::get<Number>(v));
  if (std::holds_alternative<Real>(v)) {
    std::ostringstream os;
    os << std::get<Real>(v);
    return os.str();
  }
  if (std::holds_alternative<String>(v))
    return std::get<String>(v);
  return value_to_json(v);
}

std::string value_to_json(const Value &v) {
  if (std::holds_alternative<Number>(v))
    return std::to_string(std::get<Number>(v));
  if (std::holds_alternative<Real>(v)) {
    std::ostringstream os;
    os << std::get<Real>(v);
    return os.str();
  }
  if (std::holds_alternative<String>(v))
    return "\"" + json_escape(std::get<String>(v)) + "\"";
  if (std::holds_alternative<Array>(v)) {
    const auto &arr = std::get<Array>(v);
    std::string out = "[";
    if (arr) {
      for (size_t i = 0; i < arr->items.size(); ++i) {
        if (i)
          out += ",";
        out += value_to_json(arr->items[i]);
      }
    }
    out += "]";
    return out;
  }
  if (std::holds_alternative<Object>(v)) {
    const auto &obj = std::get<Object>(v);
    std::string out = "{";
    bool first = true;
    if (obj) {
      for (const auto &[k, val] : obj->fields) {
        if (!first)
          out += ",";
        first = false;
        out += "\"" + json_escape(k) + "\":" + value_to_json(val);
      }
    }
    out += "}";
    return out;
  }
  return "null";
}

Value value_from_json(std::string_view s) {
  size_t i = 0;
  Value v = parse_json_value(s, i);
  skip_ws(s, i);
  if (i != s.size())
    throw std::runtime_error("JSON: trailing characters after value");
  return v;
}
