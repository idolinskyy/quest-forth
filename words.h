#pragma once

#include "init.h"
#include "vm.h"

#include <string>
#include <unordered_map>
#include <vector>

void word_lit_int(VM &vm);
void word_lit_real(VM &vm);
void word_lit_str(VM &vm);
void word_branch_if_zero(VM &vm);
void word_branch(VM &vm);
void word_goto(VM &vm);
void word_scene_end_trap(VM &vm);
void word_set_scene(VM &vm);

void word_call(VM &vm);
void word_return(VM &vm);

void word_do(VM &vm);
void word_loop(VM &vm);
void word_i(VM &vm);

void register_words(std::unordered_map<std::string, Primitive> &dictionary);

/// Human-readable name for a primitive opcode (or "PRIM@addr" if unknown).
std::string primitive_name(Primitive p);

struct DisassemblyLine {
  std::string text;    ///< Display line (operands may be truncated for column alignment)
  std::string tooltip; ///< Full operand / detail when truncated; empty otherwise
};

/// Disassemble vm.code into aligned listing lines (with optional hover tooltips).
std::vector<DisassemblyLine> disassemble_vm_lines(const VM &vm);

/// Same listing joined with newlines (no tooltip metadata).
std::string disassemble_vm(const VM &vm);
