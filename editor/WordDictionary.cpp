#include "WordDictionary.h"

WordDictionary &WordDictionary::instance() {
  static WordDictionary dict;
  return dict;
}

WordDictionary::WordDictionary() {
  auto add = [this](const QString &w, const QString &sig, const QString &help, const QString &cat) {
    m_words.insert(w, WordInfo{sig, help, cat});
  };

  add("+", "a b → a+b", "Add numbers or concatenate strings", "Stack");
  add("-", "a b → a−b", "Subtract numbers", "Stack");
  add("*", "a b → a×b", "Multiply", "Stack");
  add("/", "a b → a÷b", "Divide (integer or real); /0 errors", "Stack");
  add("MOD", "a b → a%b", "Remainder (integers only)", "Stack");
  add("dup", "a → a a", "Duplicate top of stack", "Stack");
  add("drop", "a →", "Discard top of stack", "Stack");
  add("swap", "a b → b a", "Swap top two items", "Stack");
  add(".", "a →", "Print value to the event log", "Stack");

  add("=", "a b → flag", "Equality (numbers, strings, arrays, objects)", "Logic");
  add(">", "a b → flag", "Greater than", "Logic");
  add("<", "a b → flag", "Less than", "Logic");
  add(">=", "a b → flag", "Greater or equal", "Logic");
  add("<=", "a b → flag", "Less or equal", "Logic");
  add("AND", "a b → flag", "Logical AND (truthy)", "Logic");
  add("OR", "a b → flag", "Logical OR", "Logic");
  add("NOT", "a → flag", "Logical NOT", "Logic");

  add("!", "value name →", "Store variable: 42 \"hp\" !", "Variables");
  add("@", "name → value", "Fetch variable (missing → 0)", "Variables");
  add("ITEM+", "count name →", "Add item to inventory", "Inventory");
  add("ITEM-", "count name →", "Remove item from inventory", "Inventory");
  add("ITEM?", "name → count", "Item count", "Inventory");
  add(".INVENTORY", "—", "Print inventory to the log", "Inventory");

  add("IF", "flag IF … THEN", "Conditional; true branch", "Control");
  add("ELSE", "IF … ELSE … THEN", "False branch", "Control");
  add("THEN", "IF … THEN", "End of conditional", "Control");
  add("BEGIN", "BEGIN … UNTIL | WHILE … REPEAT", "Start loop", "Control");
  add("UNTIL", "BEGIN … cond UNTIL", "Repeat while condition is false", "Control");
  add("WHILE", "BEGIN cond WHILE body REPEAT", "Condition before body", "Control");
  add("REPEAT", "BEGIN … WHILE … REPEAT", "End of WHILE loop", "Control");
  add("DO", "limit start DO … LOOP", "Counted loop [start, limit)", "Control");
  add("LOOP", "DO … LOOP", "End of DO loop", "Control");
  add("I", "— → index", "Current DO index", "Control");
  add(":", ": name … ;", "Define a word (call model)", "Control");
  add(";", ": name … ;", "End of word definition", "Control");

  add("SCENE:", "SCENE: name … ;SCENE", "Declare a scene", "Scenes");
  add(";SCENE", "SCENE: … ;SCENE", "End of scene", "Scenes");
  add("GOTO:", "GOTO: name", "Jump to scene", "Scenes");
  add("CHOICE", "text →", "Add a choice button", "Scenes");
  add("WAIT_CHOICE", "—", "Show choices and pause (result 1..n)", "Scenes");

  add("TITLE:", "s →", "Quest title", "Meta");
  add("AUTHOR:", "s →", "Author", "Meta");
  add("VERSION:", "s →", "Version", "Meta");
  add("LOCATION:", "s →", "Current location badge", "Meta");
  add("CLS", "—", "Clear the event log", "UI");
  add("VICTORY", "msg →", "Win the quest", "Ending");
  add("DEFEAT", "msg →", "Lose the quest", "Ending");
  add("FINISH", "msg →", "Alias for VICTORY", "Ending");
  add("HALT", "—", "Stop execution", "Ending");

  add("RANDOM", "min max → n", "Integer in [min, max] inclusive", "Misc");
  add("DELAY", "ms →", "Pause (ms); GUI uses a timer", "Misc");
  add("SAVE", "slot →", "Save progress to .gqsave", "Misc");
  add("LOAD", "slot →", "Load; jump to saved scene", "Misc");

  add("CANVAS.CLEAR", "—", "Clear canvas", "Canvas");
  add("CANVAS.COLOR", "r g b →", "Color 0–255", "Canvas");
  add("CANVAS.RECT", "x y w h →", "Filled rectangle", "Canvas");
  add("CANVAS.LINE", "x1 y1 x2 y2 →", "Line", "Canvas");
  add("CANVAS.TEXT", "x y text →", "Text on canvas", "Canvas");

  add("ARRAY", "v1…vn n → arr", "Build array from n items", "Arrays");
  add("OBJECT", "k1 v1…kn vn n → obj", "Build object from n pairs", "Objects");
  add("LEN", "coll → n", "Length of array / object / string", "Arrays");
  add("[]@", "arr i → v", "Array element (0-based)", "Arrays");
  add("[]!", "arr i v →", "Store array element", "Arrays");
  add("APPEND", "arr v →", "Append to array", "Arrays");
  add("[]POP", "arr → v", "Pop last array element", "Arrays");
  add("{}@", "obj key → v", "Object field (missing → 0)", "Objects");
  add("{}!", "obj key v →", "Store object field", "Objects");
  add("HAS?", "obj key → flag", "Key present?", "Objects");
  add("DEL", "obj key →", "Delete key", "Objects");
  add("KEYS", "obj → arr", "Array of keys", "Objects");
}

QStringList WordDictionary::allWords() const {
  QStringList keys = m_words.keys();
  keys.sort(Qt::CaseSensitive);
  return keys;
}

bool WordDictionary::contains(const QString &word) const { return m_words.contains(word); }

WordInfo WordDictionary::info(const QString &word) const { return m_words.value(word, WordInfo{}); }

QString WordDictionary::tooltipHtml(const QString &word) const {
  if (!m_words.contains(word))
    return {};
  const WordInfo &i = m_words[word];
  return QStringLiteral("<b>%1</b> <span style='color:#888'>%2</span><br/>"
                        "<code>%3</code><br/>%4")
    .arg(word.toHtmlEscaped(), i.category.toHtmlEscaped(), i.signature.toHtmlEscaped(),
         i.help.toHtmlEscaped());
}
