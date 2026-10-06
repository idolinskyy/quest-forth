#pragma once

#include "init.h"

#include <functional>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

class VM {
public:
  std::vector<Cell> code;
  size_t ip = 0;
  std::vector<Value> data_stack;
  std::string current_location;
  std::string current_scene; // last SCENE / GOTO name (for SAVE)

  std::unordered_map<std::string, Value> variables;
  std::unordered_map<std::string, size_t> inventory;

  std::string quest_title;
  std::string quest_author;
  std::string quest_version;

  std::vector<std::string> pending_choices;
  bool is_waiting_choice = false;

  // DELAY: pause in milliseconds (GUI resumes via onDelay)
  bool is_waiting_delay = false;
  Number pending_delay_ms = 0;

  std::unordered_map<std::string, size_t> scenes;

  GameOutcome outcome = GameOutcome::InProgress;
  std::string finish_message;

  std::mt19937 rng;

  // Return addresses after CALL; also (limit, index) pairs for DO/LOOP
  // DO/LOOP stores limit then index (index on top).
  std::vector<size_t> return_stack;

  // Save directory; empty means relative "saves"
  std::string save_directory = "saves";

  // Canvas ops: CLEAR | COLOR r g b | RECT x y w h | LINE x1 y1 x2 y2 | TEXT x y len msg
  std::vector<std::string> canvas_ops;

  std::function<void(const std::string &)> onOutput;
  std::function<void()> onInventoryChanged;
  std::function<void(GameOutcome, const std::string &)> onFinished;
  std::function<void(const std::string &)> onTitleChanged;
  std::function<void(const std::string &)> onAuthorChanged;
  std::function<void(const std::string &)> onVersionChanged;
  std::function<void(const std::string &)> onLocationChanged;
  std::function<void(const std::vector<std::string> &)> onChoicesChanged;
  std::function<void(Number ms)> onDelay; // GUI: start timer
  std::function<void()> onCanvasChanged;  // GUI: redraw
  std::function<void()> onSaved;
  std::function<void()> onLoaded;

  void push(Value val);
  Value pop();
  Number pop_int(); // integer only
  Real pop_real();  // int or real -> double
  std::string pop_str();
  bool pop_truthy(); // 0 / 0.0 / "" / empty collection -> false

  static bool is_numeric(const Value &v);
  static Real to_real(const Value &v);

  void reset();
  void emit_text(const std::string &s);
  void emit_canvas(std::string op);
  void run();
  /// Execute one instruction (debugger). Returns false if stopped.
  bool step_once();

  /// Parallel to code: 1-based source line per cell; 0 = none.
  std::vector<int> code_line;
  /// Current source line (at next instruction / after last).
  int current_source_line() const;

  void save_game(const std::string &slot);
  void load_game(const std::string &slot);

  VM() : rng(std::random_device{}()) {}
  explicit VM(Number seed) : rng(seed) {}

  void setSeed(Number seed) { rng.seed(seed); }
};
