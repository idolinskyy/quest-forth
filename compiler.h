#pragma once

#include "init.h"
#include "vm.h"

#include <string>
#include <string_view>
#include <unordered_map>

class Compiler {
private:
  std::unordered_map<std::string, Primitive> dictionary;

  enum class ControlType {
    IfBlock,
    ElseBlock,
    BeginBlock, // BEGIN ... UNTIL or BEGIN ... WHILE (loop start)
    WhileBlock, // WHILE: forward exit patch
    DoBlock     // DO ... LOOP (body start)
  };

  struct ControlEntry {
    ControlType type;
    size_t patch_address;
  };

  struct ColonDef {
    std::string name;
    size_t skip_patch;
    size_t body_addr;
  };

public:
  Compiler();
  void compile(std::string_view source, VM &vm);
};
