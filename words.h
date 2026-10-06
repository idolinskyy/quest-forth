#pragma once

#include "init.h"
#include "vm.h"

#include <string>
#include <unordered_map>

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
