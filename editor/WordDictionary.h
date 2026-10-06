#pragma once

#include <QHash>
#include <QString>
#include <QStringList>

struct WordInfo {
  QString signature; // stack effect / syntax
  QString help;      // short description
  QString category;
};

class WordDictionary {
public:
  static WordDictionary &instance();

  QStringList allWords() const;
  bool contains(const QString &word) const;
  WordInfo info(const QString &word) const;
  QString tooltipHtml(const QString &word) const;

private:
  WordDictionary();
  QHash<QString, WordInfo> m_words;
};
