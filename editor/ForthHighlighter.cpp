#include "ForthHighlighter.h"
#include "WordDictionary.h"

ForthHighlighter::ForthHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent) {
  m_comment.setForeground(QColor("#7a7f8a"));
  m_comment.setFontItalic(true);

  m_string.setForeground(QColor("#98c379"));

  m_number.setForeground(QColor("#d19a66"));

  m_keyword.setForeground(QColor("#c678dd"));
  m_keyword.setFontWeight(QFont::Bold);

  m_scene.setForeground(QColor("#61afef"));
  m_scene.setFontWeight(QFont::Bold);

  m_def.setForeground(QColor("#e5c07b"));
  m_def.setFontWeight(QFont::Bold);
}

void ForthHighlighter::highlightBlock(const QString &text) {
  // Token scanner with strings and comments
  int i = 0;
  while (i < text.size()) {
    if (text[i] == QLatin1Char('\\') && (i == 0 || text[i - 1].isSpace())) {
      setFormat(i, static_cast<int>(text.size() - i), m_comment);
      break;
    }
    if (text[i] == QLatin1Char('(') && i + 1 < text.size() && text[i + 1].isSpace()) {
      int end = static_cast<int>(text.indexOf(QLatin1Char(')'), i + 2));
      if (end < 0)
        end = static_cast<int>(text.size() - 1);
      setFormat(i, end - i + 1, m_comment);
      i = end + 1;
      continue;
    }
    if (text[i] == QLatin1Char('"')) {
      int start = i++;
      while (i < text.size()) {
        if (text[i] == QLatin1Char('\\') && i + 1 < text.size()) {
          i += 2;
          continue;
        }
        if (text[i] == QLatin1Char('"')) {
          ++i;
          break;
        }
        ++i;
      }
      setFormat(start, i - start, m_string);
      continue;
    }
    if (text[i].isSpace()) {
      ++i;
      continue;
    }
    int start = i;
    while (i < text.size() && !text[i].isSpace())
      ++i;
    const QString tok = text.mid(start, i - start);

    bool okNum = false;
    tok.toLongLong(&okNum);
    if (!okNum)
      tok.toDouble(&okNum);

    if (tok == "SCENE:" || tok == ";SCENE" || tok == "GOTO:") {
      setFormat(start, i - start, m_scene);
    } else if (tok == ":" || tok == ";") {
      setFormat(start, i - start, m_def);
    } else if (okNum) {
      setFormat(start, i - start, m_number);
    } else if (WordDictionary::instance().contains(tok)) {
      setFormat(start, i - start, m_keyword);
    }
  }
}
