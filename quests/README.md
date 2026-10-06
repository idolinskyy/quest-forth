# Sample quests

Ship-with-engine scenarios for the game_quest player.

| Folder        | Role                               |
| ------------- | ---------------------------------- |
| `warmup/`     | Short one-node puzzles             |
| `adventures/` | Longer multi-location stories      |
| `CATALOG.txt` | Human-readable list (not a script) |

Validate:

```bash
./build/validate_quests quests
```

Story text in these files is intentionally left as authored (often Ukrainian).
Engine documentation is English — see [`../LANGUAGE.md`](../LANGUAGE.md).
