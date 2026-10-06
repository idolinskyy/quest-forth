# Quest editor (`quest_editor`)

Standalone Qt Widgets app for writing and debugging `game_quest` `.forth` scripts.

## Build

```bash
cmake -S .. -B ../build \
  -DCMAKE_PREFIX_PATH="$HOME/Qt/6.x.x/gcc_64" \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build ../build -j --target quest_editor
../build/quest_editor
../build/quest_editor ../quests/warmup/01_lost_badge.forth
```

Needs `Qt6::Widgets` and `quest_engine`.

## Features

| Feature               | How                                          |
| --------------------- | -------------------------------------------- |
| Line numbers          | Left gutter                                  |
| Breakpoints           | Click the gutter                             |
| Syntax highlight      | Keywords, scenes, strings, numbers, comments |
| Autocomplete          | While typing or **Ctrl+Space**               |
| Word help             | Hover tooltip                                |
| Compile check         | **F7** or toolbar “Check”                    |
| Debug start           | **F5**                                       |
| Step (source line)    | **F10**                                      |
| Continue to BP / wait | **F5**                                       |
| Answer `CHOICE`       | Double-click in the Choices panel            |
| Stop                  | **Shift+F5**                                 |

Right panels: log, stack, variables, inventory, active choices.

Full language: [`../LANGUAGE.md`](../LANGUAGE.md).
