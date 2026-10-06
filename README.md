# game_quest

Interactive text quests powered by a small Forth-like scripting language, a Qt 6 player, and a desktop quest editor.

**License:** GNU GPL v3 or later (see [`LICENSE`](LICENSE)).  
**Language reference:** [`LANGUAGE.md`](LANGUAGE.md)

Sample quests under `quests/` are written in Ukrainian; the engine, docs, and tools use English.

## Features

- Stack VM with integers, reals, strings, arrays, and objects
- Scenes, choices, inventory, variables, save/load
- Loops (`BEGIN`/`UNTIL`, `BEGIN`/`WHILE`/`REPEAT`, `DO`/`LOOP`), colon definitions
- Optional canvas drawing and `DELAY`
- Qt Quick player with catalog menu
- Qt Widgets editor: highlighting, autocomplete, hover help, breakpoints, step debugger
- Headless tests (`Catch2`) and `validate_quests` for batch compile checks

## Requirements

- C++23 compiler (GCC 13+ / Clang 16+ recommended)
- CMake 3.16+
- Qt 6.2+ (Quick for the player, Widgets for the editor)
- Catch2 is fetched automatically for tests

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$HOME/Qt/6.x.x/gcc_64"   # if Qt is not on the default path
cmake --build build -j
```

Targets:

| Binary            | Role                                  |
| ----------------- | ------------------------------------- |
| `appgame_quest`   | Quest player (Qt Quick)               |
| `quest_editor`    | Script editor / debugger (Qt Widgets) |
| `test_engine`     | Unit tests                            |
| `validate_quests` | Compile-check quest files             |

Without Qt, engine tests and `validate_quests` still build.

## Run

```bash
./build/appgame_quest
./build/quest_editor quests/warmup/01_lost_badge.forth
./build/test_engine
./build/validate_quests quests
```

## Project layout

```
compiler.* vm.* words.* value.* init.h   engine
GameController.* main.cpp main.qml       player
editor/                                  quest editor
quests/                                  sample scenarios
tests/                                   Catch2 suite
tools/validate_quests.cpp                batch validator
LANGUAGE.md                              language spec
```

## Contributing

- Keep engine comments and docs in English.
- Do not add CI auto-deploy workflows unless agreed.
- Quest content may stay in the author’s language; leave existing `quests/*.forth` stories intact unless intentionally rewriting them.

## Copyright

Copyright (C) 2026 Ihor Dolinskyi
