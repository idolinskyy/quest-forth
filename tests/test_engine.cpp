#include "compiler.h"
#include "vm.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace {

struct CapturedOutput {
  std::vector<std::string> lines;
};

void attach_output(VM &vm, CapturedOutput &out) {
  vm.onOutput = [&out](const std::string &s) { out.lines.push_back(s); };
}

Number top_int(VM &vm) {
  REQUIRE(vm.data_stack.size() > 0);
  // Avoid vector::empty()/back() — clangd 18 + libstdc++ (GCC 16) breaks on __normal_iterator
  const Value &val = vm.data_stack.data()[vm.data_stack.size() - 1];
  REQUIRE(std::holds_alternative<Number>(val));
  return std::get<Number>(val);
}

} // namespace

TEST_CASE("arithmetic and stack words", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile("2 3 + dup", vm);
  vm.run();

  REQUIRE(vm.data_stack.size() == 2);
  REQUIRE(top_int(vm) == 5);
  vm.pop();
  REQUIRE(top_int(vm) == 5);

  vm.reset();
  compiler.compile("10 3 -", vm);
  vm.run();
  REQUIRE(top_int(vm) == 7);

  vm.reset();
  compiler.compile("4 5 *", vm);
  vm.run();
  REQUIRE(top_int(vm) == 20);

  vm.reset();
  compiler.compile("10 3 /", vm);
  vm.run();
  REQUIRE(top_int(vm) == 3);

  vm.reset();
  compiler.compile("1 2 drop", vm);
  vm.run();
  REQUIRE(vm.data_stack.size() == 1);
  REQUIRE(top_int(vm) == 1);

  vm.reset();
  compiler.compile("1 2 swap", vm);
  vm.run();
  REQUIRE(vm.data_stack.size() == 2);
  REQUIRE(top_int(vm) == 1);
  vm.pop();
  REQUIRE(top_int(vm) == 2);
}

TEST_CASE("string concatenation with +", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"("hello" " " + "world" + .)", vm);
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "hello world");
}

TEST_CASE("Division by zero", "[engine]") {
  Compiler compiler;
  VM vm;

  SECTION("Compile and run 10 0 / throws runtime_error") {
    compiler.compile("10 0 /", vm);
    REQUIRE_THROWS_AS(vm.run(), std::runtime_error);
  }

  SECTION("Check exact error message") {
    compiler.compile("10 0 /", vm);
    REQUIRE_THROWS_WITH(vm.run(), "Division by zero in '/'");
  }
}

TEST_CASE("RANDOM generates random numbers 10 times", "[engine]") {
  Compiler compiler;
  VM vm;
  for (int i = 0; i < 10; i++) {
    compiler.compile(R"(0 10 RANDOM)", vm);
    vm.run();
    REQUIRE(top_int(vm) >= 0);
    REQUIRE(top_int(vm) <= 10);
    vm.reset();
  }
}

TEST_CASE("RANDOM generates random numbers with min and max equal", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(20 20 RANDOM)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 20);
}

TEST_CASE("RANDOM generates random numbers with min greater than max", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(20 10 RANDOM)", vm);
  REQUIRE_THROWS_AS(vm.run(), std::runtime_error);
}

TEST_CASE("comparisons and logic", "[engine]") {
  Compiler compiler;
  VM vm;

  auto expect_int = [&](const char *src, Number expected) {
    vm.reset();
    compiler.compile(src, vm);
    vm.run();
    REQUIRE(top_int(vm) == expected);
  };

  expect_int("5 5 =", 1);
  expect_int("5 4 =", 0);
  expect_int(R"("a" "a" =)", 1);
  expect_int(R"("a" "b" =)", 0);

  expect_int("5 3 >", 1);
  expect_int("3 5 >", 0);
  expect_int("3 5 <", 1);
  expect_int("5 3 <", 0);

  expect_int("5 5 >=", 1);
  expect_int("4 5 >=", 0);
  expect_int("5 5 <=", 1);
  expect_int("6 5 <=", 0);

  expect_int("1 1 AND", 1);
  expect_int("1 0 AND", 0);
  expect_int("0 1 OR", 1);
  expect_int("0 0 OR", 0);
  expect_int("0 NOT", 1);
  expect_int("7 NOT", 0);
}

TEST_CASE("IF without ELSE", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"(1 IF "yes" . THEN "after" .)", vm);
  vm.run();
  REQUIRE(out.lines.size() == 2);
  REQUIRE(out.lines[0] == "yes");
  REQUIRE(out.lines[1] == "after");

  vm.reset();
  out.lines.clear();
  compiler.compile(R"(0 IF "yes" . THEN "after" .)", vm);
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "after");
}

TEST_CASE("IF ELSE THEN both branches", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"(1 IF "yes" . ELSE "no" . THEN)", vm);
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "yes");

  vm.reset();
  out.lines.clear();
  compiler.compile(R"(0 IF "yes" . ELSE "no" . THEN)", vm);
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "no");
}

TEST_CASE("nested IF ELSE THEN", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  auto expect_line = [&](const char *src, const char *expected) {
    vm.reset();
    out.lines.clear();
    compiler.compile(src, vm);
    vm.run();
    REQUIRE(out.lines.size() == 1);
    REQUIRE(out.lines[0] == expected);
  };

  expect_line(R"(
    1 IF
      1 IF "inner-yes" . ELSE "inner-no" . THEN
    ELSE
      "outer-no" .
    THEN
  )",
              "inner-yes");

  expect_line(R"(
    1 IF
      0 IF "inner-yes" . ELSE "inner-no" . THEN
    ELSE
      "outer-no" .
    THEN
  )",
              "inner-no");

  expect_line(R"(
    0 IF
      1 IF "inner-yes" . ELSE "inner-no" . THEN
    ELSE
      "outer-no" .
    THEN
  )",
              "outer-no");

  expect_line(R"(
    0 IF
      "outer-yes" .
    ELSE
      1 IF "else-inner-yes" . ELSE "else-inner-no" . THEN
    THEN
  )",
              "else-inner-yes");

  expect_line(R"(
    0 IF
      "outer-yes" .
    ELSE
      0 IF "else-inner-yes" . ELSE "else-inner-no" . THEN
    THEN
  )",
              "else-inner-no");
}

TEST_CASE("variables store and fetch", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(42 "hp" ! "hp" @)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 42);

  vm.reset();
  compiler.compile(R"("missing" @)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 0);
}

TEST_CASE("inventory ITEM+ ITEM? ITEM-", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(3 "меч" ITEM+ "меч" ITEM?)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 3);
  REQUIRE(vm.inventory.at("меч") == 3);

  vm.pop();
  compiler.compile(R"(1 "меч" ITEM- "меч" ITEM?)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 2);

  vm.pop();
  compiler.compile(R"(5 "меч" ITEM- "меч" ITEM?)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 0);
  REQUIRE(vm.inventory.find("меч") == vm.inventory.end());
}

TEST_CASE("SCENE GOTO and ;SCENE trap name", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  // Fall through first scene into trap, then never reach second without GOTO
  compiler.compile(R"(
    SCENE: start
      "hello" .
    ;SCENE
    SCENE: other
      "other" .
      HALT
    ;SCENE
  )",
                   vm);
  vm.run();

  REQUIRE(out.lines.size() >= 2);
  REQUIRE(out.lines[0] == "hello");
  REQUIRE(out.lines[1].find("start") != std::string::npos);
  REQUIRE(out.lines[1].find("unknown_scene") == std::string::npos);

  vm.reset();
  out.lines.clear();
  compiler.compile(R"(
    SCENE: a
      GOTO: b
    ;SCENE
    SCENE: b
      "arrived" .
      HALT
    ;SCENE
  )",
                   vm);
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "arrived");
}

TEST_CASE("CHOICE and WAIT_CHOICE pause", "[engine]") {
  Compiler compiler;
  VM vm;
  bool choices_notified = false;
  vm.onChoicesChanged = [&](const std::vector<std::string> &c) {
    choices_notified = true;
    REQUIRE(c.size() == 2);
    REQUIRE(c[0] == "left");
    REQUIRE(c[1] == "right");
  };

  compiler.compile(R"("left" CHOICE "right" CHOICE WAIT_CHOICE)", vm);
  vm.run();

  REQUIRE(vm.is_waiting_choice);
  REQUIRE(vm.pending_choices.size() == 2);
  REQUIRE(choices_notified);
}

TEST_CASE("VICTORY and DEFEAT", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"("ok" VICTORY)", vm);
  vm.run();
  REQUIRE(vm.outcome == GameOutcome::Victory);
  REQUIRE(vm.finish_message == "ok");

  vm.reset();
  compiler.compile(R"("fail" DEFEAT)", vm);
  vm.run();
  REQUIRE(vm.outcome == GameOutcome::Defeat);
  REQUIRE(vm.finish_message == "fail");

  vm.reset();
  compiler.compile(R"("done" FINISH)", vm);
  vm.run();
  REQUIRE(vm.outcome == GameOutcome::Victory);
  REQUIRE(vm.finish_message == "done");
}

TEST_CASE("quest metadata and location", "[engine]") {
  Compiler compiler;
  VM vm;
  std::string title;
  std::string author;
  std::string version;
  std::string location;

  vm.onTitleChanged = [&](const std::string &t) { title = t; };
  vm.onAuthorChanged = [&](const std::string &a) { author = a; };
  vm.onVersionChanged = [&](const std::string &v) { version = v; };
  vm.onLocationChanged = [&](const std::string &l) { location = l; };

  compiler.compile(R"(
    "Демо" TITLE:
    "Автор" AUTHOR:
    "1.2" VERSION:
    "Ліс" LOCATION:
  )",
                   vm);
  vm.run();

  REQUIRE(vm.quest_title == "Демо");
  REQUIRE(vm.quest_author == "Автор");
  REQUIRE(vm.quest_version == "1.2");
  REQUIRE(vm.current_location == "Ліс");
  REQUIRE(title == "Демо");
  REQUIRE(author == "Автор");
  REQUIRE(version == "1.2");
  REQUIRE(location == "Ліс");
}

TEST_CASE("CLS clears via special marker", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"("before" . CLS "after" .)", vm);
  vm.run();

  REQUIRE(out.lines.size() == 3);
  REQUIRE(out.lines[0] == "before");
  REQUIRE(out.lines[1] == "__CLEAR_SCREEN__");
  REQUIRE(out.lines[2] == "after");
}

TEST_CASE("comments are ignored", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    \ цей рядок — коментар
    2 3 + ( сума ) 1 +
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 6);
}

TEST_CASE("unknown word throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile("foobar", vm), std::runtime_error);
}

TEST_CASE("unbalanced IF throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile("1 IF", vm), std::runtime_error);
}

TEST_CASE("ELSE without IF throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile("ELSE THEN", vm), std::runtime_error);
}

TEST_CASE("THEN without IF throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile("THEN", vm), std::runtime_error);
}

TEST_CASE("unclosed string literal throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile("\"abc", vm), std::runtime_error);
}

TEST_CASE("WAIT_CHOICE without CHOICE throws", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile("WAIT_CHOICE", vm);
  REQUIRE_THROWS_AS(vm.run(), std::runtime_error);
}

TEST_CASE("GOTO missing scene throws", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile("GOTO: nowhere", vm);
  REQUIRE_THROWS_AS(vm.run(), std::runtime_error);
}

TEST_CASE("dot prints numbers and strings", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"(42 . "hi" .)", vm);
  vm.run();
  REQUIRE(out.lines.size() == 2);
  REQUIRE(out.lines[0] == "42");
  REQUIRE(out.lines[1] == "hi");
}

TEST_CASE(".INVENTORY emits to journal callback", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"(2 "ключ" ITEM+ .INVENTORY)", vm);
  vm.run();

  REQUIRE(out.lines.size() > 0);
  bool found_item = false;
  for (size_t i = 0; i < out.lines.size(); ++i) {
    if (out.lines.data()[i].find("ключ") != std::string::npos)
      found_item = true;
  }
  REQUIRE(found_item);
}

TEST_CASE("HALT stops execution", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"("a" . HALT "b" .)", vm);
  vm.run();

  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "a");
}

TEST_CASE("type mismatch on + throws", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(1 "x" +)", vm);
  REQUIRE_THROWS_AS(vm.run(), std::runtime_error);
}

TEST_CASE("empty inventory print", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(".INVENTORY", vm);
  vm.run();

  REQUIRE(out.lines.size() >= 2);
  bool found_empty = false;
  for (size_t i = 0; i < out.lines.size(); ++i) {
    if (out.lines.data()[i].find("[Empty]") != std::string::npos)
      found_empty = true;
  }
  REQUIRE(found_empty);
}

// ---------------------------------------------------------------------------
// BEGIN ... UNTIL
// ---------------------------------------------------------------------------

TEST_CASE("BEGIN UNTIL counts to 5", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    0 "i" !
    BEGIN
      "i" @ 1 + "i" !
      "i" @ 5 >=
    UNTIL
    "i" @
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 5);
}

TEST_CASE("BEGIN UNTIL body runs at least once", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"(
    BEGIN
      "once" .
      1
    UNTIL
  )",
                   vm);
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "once");
}

TEST_CASE("nested BEGIN UNTIL", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    0 "sum" !
    0 "o" !
    BEGIN
      0 "i" !
      BEGIN
        "sum" @ 1 + "sum" !
        "i" @ 1 + "i" !
        "i" @ 2 >=
      UNTIL
      "o" @ 1 + "o" !
      "o" @ 3 >=
    UNTIL
    "sum" @
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 6);
}

TEST_CASE("BEGIN inside IF and IF inside BEGIN", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    1 IF
      0 "n" !
      BEGIN
        "n" @ 1 + "n" !
        "n" @ 3 >=
      UNTIL
      "n" @
    ELSE
      0
    THEN
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 3);
}

TEST_CASE("UNTIL without BEGIN throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile("1 UNTIL", vm), std::runtime_error);
}

TEST_CASE("BEGIN without UNTIL throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile("BEGIN 1", vm), std::runtime_error);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TEST_CASE("colon word double", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    : double dup + ;
    5 double
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 10);
}

TEST_CASE("colon word calls another colon word", "[engine]") {
  Compiler compiler;
  VM vm;

  // quadruple = double ∘ double
  compiler.compile(R"(
    : double dup + ;
    : quadruple double double ;
    3 quadruple
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 12);
}

TEST_CASE("colon word with BEGIN UNTIL inside", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    : add3
      0 "c" !
      BEGIN
        1 +
        "c" @ 1 + "c" !
        "c" @ 3 >=
      UNTIL
    ;
    10 add3
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 13);
}

TEST_CASE("recursive colon word", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    0 "s" !
    : add_down
      dup 0 >
      IF
        dup "s" @ + "s" !
        1 - add_down
      ELSE
        drop
      THEN
    ;
    3 add_down
    "s" @
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 6);
}

TEST_CASE("semicolon without colon throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile(";", vm), std::runtime_error);
}

TEST_CASE("colon without semicolon throws", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile(": foo 1", vm), std::runtime_error);
}

TEST_CASE("nested colon definitions throw", "[engine]") {
  Compiler compiler;
  VM vm;
  REQUIRE_THROWS_AS(compiler.compile(": a : b 1 ; ;", vm), std::runtime_error);
}

TEST_CASE("definition body is skipped at top level", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"(
    : boom "should-not-run" . ;
    "ok" .
  )",
                   vm);
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "ok");
}

// ---------------------------------------------------------------------------
// MOD, float, escapes, WHILE/REPEAT, DO/LOOP, SAVE/LOAD, DELAY, CANVAS
// ---------------------------------------------------------------------------

TEST_CASE("MOD remainder", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile("10 3 MOD", vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);

  vm.reset();
  compiler.compile("10 0 MOD", vm);
  REQUIRE_THROWS_AS(vm.run(), std::runtime_error);
}

TEST_CASE("float literals and arithmetic", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile("1.5 2.5 +", vm);
  vm.run();
  REQUIRE(vm.data_stack.size() == 1);
  REQUIRE(std::holds_alternative<Real>(vm.data_stack[0]));
  REQUIRE(std::get<Real>(vm.data_stack[0]) == Catch::Approx(4.0));

  vm.reset();
  compiler.compile("10 2.5 /", vm);
  vm.run();
  REQUIRE(std::holds_alternative<Real>(vm.data_stack[0]));
  REQUIRE(std::get<Real>(vm.data_stack[0]) == Catch::Approx(4.0));

  vm.reset();
  compiler.compile("1.25 1.25 =", vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);

  vm.reset();
  compiler.compile("2.0 1.5 >", vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);
}

TEST_CASE("string escape sequences", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"("line1\nline2\t\"x\\" .)", vm);
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "line1\nline2\t\"x\\");

  vm.reset();
  REQUIRE_THROWS_AS(compiler.compile(R"("bad\q")", vm), std::runtime_error);
}

TEST_CASE("BEGIN WHILE REPEAT", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    0 "i" !
    BEGIN
      "i" @ 5 <
    WHILE
      "i" @ 1 + "i" !
    REPEAT
    "i" @
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 5);
}

TEST_CASE("BEGIN WHILE REPEAT zero iterations", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(
    0 "n" !
    BEGIN
      0
    WHILE
      1 "n" !
    REPEAT
    "n" @
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 0);
}

TEST_CASE("DO LOOP I", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"(
    0 "sum" !
    5 0 DO
      I "sum" @ + "sum" !
    LOOP
    "sum" @
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 10); // 0+1+2+3+4
}

TEST_CASE("nested DO LOOP", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(
    0 "c" !
    3 0 DO
      2 0 DO
        "c" @ 1 + "c" !
      LOOP
    LOOP
    "c" @
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 6);
}

TEST_CASE("SAVE and LOAD roundtrip", "[engine]") {
  namespace fs = std::filesystem;
  const fs::path save_dir = fs::temp_directory_path() / "gq_test_saves";
  fs::create_directories(save_dir);

  {
    Compiler compiler;
    VM vm;
    vm.save_directory = save_dir.string();
    compiler.compile(R"(
      SCENE: room
        "Кімната" LOCATION:
        7 "hp" !
        1.5 "ratio" !
        "hello" "note" !
        2 "ключ" ITEM+
        "slot_a" SAVE
        HALT
      ;SCENE
    )",
                     vm);
    vm.run();
    REQUIRE(std::get<Number>(vm.variables["hp"]) == 7);
    REQUIRE(vm.inventory["ключ"] == 2);
  }

  {
    Compiler compiler;
    VM vm;
    vm.save_directory = save_dir.string();
    compiler.compile(R"(
      SCENE: boot
        "slot_a" LOAD
      ;SCENE

      SCENE: room
        "hp" @
      ;SCENE
    )",
                     vm);
    vm.run();
    REQUIRE(top_int(vm) == 7);
    REQUIRE(vm.inventory["ключ"] == 2);
    REQUIRE(std::holds_alternative<Real>(vm.variables["ratio"]));
    REQUIRE(std::get<Real>(vm.variables["ratio"]) == Catch::Approx(1.5));
    REQUIRE(std::get<String>(vm.variables["note"]) == "hello");
    REQUIRE(vm.current_location == "Кімната");
  }

  fs::remove_all(save_dir);
}

TEST_CASE("DELAY without GUI callback continues", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);

  compiler.compile(R"(100 DELAY "after" .)", vm);
  vm.run();
  REQUIRE_FALSE(vm.is_waiting_delay);
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "after");
}

TEST_CASE("DELAY with GUI callback pauses then resumes", "[engine]") {
  Compiler compiler;
  VM vm;
  CapturedOutput out;
  attach_output(vm, out);
  Number seen_ms = -1;
  vm.onDelay = [&](Number ms) { seen_ms = ms; };

  compiler.compile(R"(250 DELAY "later" .)", vm);
  vm.run();
  REQUIRE(vm.is_waiting_delay);
  REQUIRE(seen_ms == 250);
  REQUIRE(out.lines.empty());

  vm.is_waiting_delay = false;
  vm.run();
  REQUIRE(out.lines.size() == 1);
  REQUIRE(out.lines[0] == "later");
}

TEST_CASE("CANVAS primitives", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(
    CANVAS.CLEAR
    255 128 0 CANVAS.COLOR
    10 20 30 40 CANVAS.RECT
    0 0 100 100 CANVAS.LINE
    5 15 "Hi" CANVAS.TEXT
  )",
                   vm);
  vm.run();
  REQUIRE(vm.canvas_ops.size() >= 5);
  REQUIRE(vm.canvas_ops[0] == "CLEAR");
  REQUIRE(vm.canvas_ops[1] == "COLOR 255 128 0");
  REQUIRE(vm.canvas_ops[2] == "RECT 10 20 30 40");
  REQUIRE(vm.canvas_ops[3] == "LINE 0 0 100 100");
  REQUIRE(vm.canvas_ops[4] == "TEXT 5 15 2 Hi");
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

TEST_CASE("ARRAY create fetch store append pop", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"("a" "b" "c" 3 ARRAY LEN)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 3);

  vm.reset();
  compiler.compile(R"(
    "a" "b" "c" 3 ARRAY "arr" !
    "arr" @ 1 []@
  )",
                   vm);
  vm.run();
  REQUIRE(std::get<String>(vm.data_stack[0]) == "b");

  vm.reset();
  compiler.compile(R"(
    0 ARRAY "arr" !
    "arr" @ "x" APPEND
    "arr" @ "y" APPEND
    "arr" @ LEN
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 2);

  vm.reset();
  compiler.compile(R"(
    10 20 2 ARRAY "arr" !
    "arr" @ 0 99 []!
    "arr" @ 0 []@
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 99);

  vm.reset();
  compiler.compile(R"(
    1 2 3 3 ARRAY "arr" !
    "arr" @ []POP
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 3);
}

TEST_CASE("ARRAY shared reference via dup", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(
    0 ARRAY dup
    "hi" APPEND
    LEN
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);
}

TEST_CASE("ARRAY index out of bounds", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(1 1 ARRAY 5 []@)", vm);
  REQUIRE_THROWS_AS(vm.run(), std::runtime_error);
}

TEST_CASE("OBJECT create fetch store has del keys", "[engine]") {
  Compiler compiler;
  VM vm;

  compiler.compile(R"(
    "name" "Ann" "hp" 10 2 OBJECT "npc" !
    "npc" @ "hp" {}@
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 10);

  vm.reset();
  compiler.compile(R"(
    0 OBJECT "o" !
    "o" @ "k" 42 {}!
    "o" @ "k" HAS?
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);

  vm.reset();
  compiler.compile(R"(
    "a" 1 1 OBJECT "o" !
    "o" @ "missing" {}@
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 0);

  vm.reset();
  compiler.compile(R"(
    "a" 1 "b" 2 2 OBJECT "o" !
    "o" @ "a" DEL
    "o" @ LEN
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);

  vm.reset();
  compiler.compile(R"(
    "x" 1 1 OBJECT KEYS LEN
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);
}

TEST_CASE("nested array in object and equality", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"(
    1 2 2 ARRAY "items" swap 1 OBJECT
    dup "items" {}@ 0 []@
  )",
                   vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);

  vm.reset();
  compiler.compile(R"(1 2 2 ARRAY 1 2 2 ARRAY =)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);

  vm.reset();
  compiler.compile(R"("a" 1 1 OBJECT "a" 1 1 OBJECT =)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 1);
}

TEST_CASE("LEN on string", "[engine]") {
  Compiler compiler;
  VM vm;
  compiler.compile(R"("abcd" LEN)", vm);
  vm.run();
  REQUIRE(top_int(vm) == 4);
}

TEST_CASE("SAVE LOAD with array and object", "[engine]") {
  namespace fs = std::filesystem;
  const fs::path save_dir = fs::temp_directory_path() / "gq_test_saves_ao";
  fs::create_directories(save_dir);

  {
    Compiler compiler;
    VM vm;
    vm.save_directory = save_dir.string();
    compiler.compile(R"(
      SCENE: room
        "Кімната" LOCATION:
        "a" "b" 2 ARRAY "clues" !
        "hp" 5 "name" "Bob" 2 OBJECT "npc" !
        "slot_ao" SAVE
        HALT
      ;SCENE
    )",
                     vm);
    vm.run();
  }

  {
    Compiler compiler;
    VM vm;
    vm.save_directory = save_dir.string();
    compiler.compile(R"(
      SCENE: boot
        "slot_ao" LOAD
      ;SCENE
      SCENE: room
        "clues" @ LEN
      ;SCENE
    )",
                     vm);
    vm.run();
    REQUIRE(top_int(vm) == 2);
    REQUIRE(std::holds_alternative<Object>(vm.variables["npc"]));
    auto npc = std::get<Object>(vm.variables["npc"]);
    REQUIRE(std::get<Number>(npc->fields["hp"]) == 5);
    REQUIRE(std::get<String>(npc->fields["name"]) == "Bob");
  }

  fs::remove_all(save_dir);
}
