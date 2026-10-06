#pragma once

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class ForthHighlighter : public QSyntaxHighlighter {
  Q_OBJECT
public:
  explicit ForthHighlighter(QTextDocument *parent = nullptr);

protected:
  void highlightBlock(const QString &text) override;

private:
  QTextCharFormat m_comment;
  QTextCharFormat m_string;
  QTextCharFormat m_number;
  QTextCharFormat m_keyword;
  QTextCharFormat m_scene;
  QTextCharFormat m_def;
};
