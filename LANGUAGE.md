# game_quest scripting language

Full language reference for the _Game Quests_ engine (`game_quest`).
You can write a valid `.forth` quest from this document alone.

- File extensions: `.forth`, `.fth`, `.txt`
- Encoding: UTF-8
- Paradigm: stack language in the Forth style (reverse Polish notation)
- Value types: **integer** (`int64`), **real** (`double`), **string**, **array**, **object**

---

## 1. Mental model

Operands go on the stack **before** the operator word.

```
5 3 +          \ stack: 5, 3 → 8
"Hello" .      \ print string to the event log
```

After `WAIT_CHOICE`, the player presses a button; the stack receives the **1-based**
choice index (first button = `1`). Scripts usually compare that number and `GOTO:`.

---

## 2. Lexicon

### 2.1. Whitespace

Tokens are separated by spaces, tabs, or newlines.

### 2.2. Comments

```
\ line comment to end of line

( block comment in Forth style —
  required space after the opening parenthesis )
```

### 2.3. Strings

Double-quoted. Escape sequences:

| Escape | Meaning |
| ------ | ------- |
| `\n`   | newline |
| `\t`   | tab |
| `\r`   | carriage return |
| `\\`   | backslash |
| `\"`   | quote inside string |
| `\0`   | NUL byte |

Unknown escapes or an unclosed quote are compile errors.
You can also print multiple paragraphs with separate `.` calls.

### 2.4. Numbers

Integers (including negatives): `0 42 -3`

Reals if the token contains `.` or `e`/`E`: `3.14  -0.5  1e3`

If either operand is real, arithmetic yields a real; two integers stay integer
(`/` is truncating integer division for ints).

### 2.5. Names

Scene/word identifiers have no spaces: `lobby`, `has_key`.
Spaces are fine inside string literals and inventory item names.

---

## 3. Typical quest layout

```
"Quest title" TITLE:
"Author" AUTHOR:
"1.0" VERSION:

0 "flag" !

: helper
  ...
;

SCENE: start
  CLS
  "Location" LOCATION:
  "Description…" .
  "Option A" CHOICE
  "Option B" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: path_a
  ELSE
    GOTO: path_b
  THEN
;SCENE

SCENE: path_a
  ...
  "Victory text" VICTORY
;SCENE
```

**Scene rules**

1. Every scene is `SCENE: name` … `;SCENE`
2. A scene must not fall through: end with `WAIT_CHOICE`, `GOTO:`,
   `VICTORY` / `DEFEAT` / `FINISH`, or `HALT`.
3. Otherwise the engine warns and stops.

`TITLE:` / `AUTHOR:` / `VERSION:` are recommended (title appears in the catalog).

---

## 4. Stack words

| Word | Stack | Notes |
| ---- | ----- | ----- |
| integer / real | — → n | literal |
| `"…"` | — → s | string literal (with escapes) |
| `+` | a b → a+b | numbers **or** string concat |
| `-` `*` `/` | a b → … | numbers only; `/0` errors |
| `MOD` | a b → a%b | integers only |
| `dup` `drop` `swap` | … | stack ops |
| `.` | a → | print to the **event log** |

---

## 5. Variables

| Word | Stack | Notes |
| ---- | ----- | ----- |
| `!` | value name → | store: `42 "hp" !` |
| `@` | name → value | fetch; missing → `0` |

---

## 6. Inventory

| Word | Stack | Notes |
| ---- | ----- | ----- |
| `ITEM+` | count name → | add items |
| `ITEM-` | count name → | remove; drops the entry at 0 |
| `ITEM?` | name → count | count (0 if absent) |
| `.INVENTORY` | — | print inventory |

`count ≤ 0` is ignored for `ITEM+` / `ITEM-`.

---

## 7. Comparison and logic

Results are `1` / `0`.

| Word | Notes |
| ---- | ----- |
| `=` | equality (numbers with coercion; strings; deep array/object) |
| `>` `<` `>=` `<=` | numbers |
| `AND` `OR` `NOT` | truthy values |

**Truthy:** non-zero number/real, non-empty string, non-empty array/object.

---

## 8. Conditionals: `IF` / `ELSE` / `THEN`

```
cond IF
  ... true ...
ELSE
  ... false ...
THEN
```

`ELSE` is optional. Nesting is allowed.

---

## 9. Loops

### 9.1. `BEGIN` … `UNTIL`

Body runs at least once; repeat while the condition is false.

### 9.2. `BEGIN` … `WHILE` … `REPEAT`

Condition before the body; body may run zero times.

### 9.3. `DO` … `LOOP` and `I`

```
limit start DO
  ... I ...
LOOP
```

Index runs from `start` inclusive to `limit` exclusive. `I` is the inner index.

---

## 10. Colon definitions: `: name ... ;`

```
: double dup + ;
5 double          \ → 10
```

No nested `:`; recursion is allowed; primitives win over user names.

---

## 11. Scenes

```
SCENE: name
  ...
;SCENE

GOTO: other_scene
```

Names after `SCENE:` / `GOTO:` are a single token (no spaces).

Hub pattern: `dup 1 = IF drop GOTO: a THEN` …

---

## 12. Choices

| Word | Stack | Notes |
| ---- | ----- | ----- |
| `CHOICE` | text → | add button |
| `WAIT_CHOICE` | — | show buttons and **pause** |

After a click, the 1-based index is on the stack.
`WAIT_CHOICE` with no `CHOICE` is an error.

---

## 13. Metadata and UI

| Word | Effect |
| ---- | ------ |
| `TITLE:` `AUTHOR:` `VERSION:` | window / status |
| `LOCATION:` | location badge |
| `CLS` | clear event log |

---

## 14. Endings

| Word | Notes |
| ---- | ----- |
| `VICTORY` / `FINISH` | win; message shown in overlay |
| `DEFEAT` | lose |
| `HALT` | stop without win/lose overlay |

---

## 15. `RANDOM`

`min max RANDOM` → integer in `[min, max]` inclusive.
`min == max` returns that value; `min > max` errors.

---

## 16. `DELAY`

`ms DELAY` — pause in milliseconds (GUI timer; instant in unit tests without callback).

---

## 17. `SAVE` / `LOAD`

```
"slot1" SAVE
"slot1" LOAD   \ restores vars/inventory; jumps to saved scene
```

Typical pattern: a `boot` scene that `LOAD`s, and checkpoints that `SAVE`.
Do not put `LOAD` only inside the same scene you resume into without a separate boot entry.

Files live under `saves/` (AppData in the GUI), extension `.gqsave`.

---

## 18. Canvas: `CANVAS.*`

| Word | Stack | Notes |
| ---- | ----- | ----- |
| `CANVAS.CLEAR` | — | clear |
| `CANVAS.COLOR` | r g b → | 0–255 |
| `CANVAS.RECT` | x y w h → | filled rect |
| `CANVAS.LINE` | x1 y1 x2 y2 → | line |
| `CANVAS.TEXT` | x y text → | text |

No audio in the language.

---

## 19. Arrays and objects

By reference (`shared_ptr`): `dup` / storing in a variable does **not** deep-copy;
mutations via `APPEND` / `[]!` / `{}!` are shared.

### Arrays

| Word | Stack | Notes |
| ---- | ----- | ----- |
| `ARRAY` | v1…vn n → arr | build (bottom → index 0) |
| `LEN` | arr → n | also works on objects/strings |
| `[]@` / `[]!` | arr i [v] | 0-based; out of range errors |
| `APPEND` | arr v → | push back |
| `[]POP` | arr → v | pop last |

### Objects

| Word | Stack | Notes |
| ---- | ----- | ----- |
| `OBJECT` | k1 v1 … kn vn n → obj | n pairs |
| `{}@` | obj key → v | missing → `0` |
| `{}!` | obj key v → | set field |
| `HAS?` / `DEL` / `KEYS` | … | key ops |

`=` compares arrays/objects by deep value. Empty array/object is falsy.
`SAVE`/`LOAD` persist nested structures.

There are no `[…]` / `{…}` literals — only `ARRAY` / `OBJECT`.

---

## 20. Not in the language

- Nested `: ... ;`
- Sound / animation tweens
- `+LOOP`, `LEAVE`, `J`

---

## 21. Checklist

1. `TITLE:` present (for the catalog).
2. Every `SCENE:` closed with `;SCENE`.
3. Every scene ends with choice / `GOTO:` / ending / `HALT`.
4. All `GOTO:` targets exist.
5. Balanced `IF`/`THEN`, loops, and `;`.
6. Choice index handled after `WAIT_CHOICE`.
7. Meaningful victory/defeat messages.
8. UTF-8; no spaces in scene names.

```bash
./build/validate_quests path/to/script.forth
./build/validate_quests quests
./build/quest_editor quests/warmup/01_lost_badge.forth
```

See `editor/README.md`.

---

## 22. Minimal example

```
"Doors" TITLE:
"Demo" AUTHOR:
"1.0" VERSION:

0 "has_key" !

SCENE: room
  CLS
  "Room" LOCATION:
  "A locked door." .
  "Search for a key" CHOICE
  "Open the door" CHOICE
  WAIT_CHOICE
  1 = IF
    GOTO: search
  ELSE
    GOTO: door
  THEN
;SCENE

SCENE: search
  CLS
  1 "has_key" !
  1 "key" ITEM+
  "Back" CHOICE
  WAIT_CHOICE
  drop GOTO: room
;SCENE

SCENE: door
  CLS
  "has_key" @ IF
    "You leave." VICTORY
  ELSE
    "Locked." .
    "Back" CHOICE
    WAIT_CHOICE
    drop GOTO: room
  THEN
;SCENE
```

---

## 23. Longer quest patterns

1. Boolean fact flags (`0`/`1` variables).
2. Hub scene with returns.
3. Finale: `AND` of clues → `VICTORY` / `DEFEAT`.
4. Inventory as evidence.
5. Colon helpers for repeated checks.
6. `CLS` + `LOCATION:` on scene entry.
7. Checkpoints with `SAVE` / boot `LOAD`.
8. Arrays of clues / NPC objects instead of many scalars.

---

## 24. Word index

**Compiler constructs:**  
`IF` `ELSE` `THEN` `BEGIN` `UNTIL` `WHILE` `REPEAT` `DO` `LOOP`  
`: ` `;` `SCENE:` `;SCENE` `GOTO:`

**Dictionary:**  
`!` `@` `.` `.INVENTORY` `*` `+` `-` `/` `<` `<=` `=` `>` `>=`  
`AND` `APPEND` `ARRAY` `AUTHOR:`  
`CANVAS.CLEAR` `CANVAS.COLOR` `CANVAS.LINE` `CANVAS.RECT` `CANVAS.TEXT`  
`CHOICE` `CLS` `DEFEAT` `DELAY` `DEL` `FINISH` `HALT` `HAS?` `I`  
`ITEM+` `ITEM-` `ITEM?` `KEYS` `LEN` `LOAD` `LOCATION:` `MOD` `NOT` `OBJECT` `OR`  
`RANDOM` `SAVE` `TITLE:` `VERSION:` `VICTORY` `WAIT_CHOICE`  
`[]!` `[]@` `[]POP` `{}!` `{}@`  
`drop` `dup` `swap`

Anything else is a user word or an “Unknown word” error.

---

_This document matches the game_quest engine (Compiler + VM + words). Update it whenever the language changes._
