#pragma once

#include <QPlainTextEdit>
#include <QSet>

class CodeEditor : public QPlainTextEdit {
  Q_OBJECT
public:
  explicit CodeEditor(QWidget *parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent *event);
  int lineNumberAreaWidth() const;
  void lineNumberAreaClicked(int y);

  void setBreakpoint(int line1based, bool on);
  void toggleBreakpoint(int line1based);
  bool hasBreakpoint(int line1based) const;
  QSet<int> breakpoints() const { return m_breakpoints; }
  void setCurrentDebugLine(int line1based); // 0 = clear
  int currentDebugLine() const { return m_debugLine; }

signals:
  void breakpointToggled(int line1based, bool on);

protected:
  void resizeEvent(QResizeEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  bool event(QEvent *event) override;

private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect &rect, int dy);

private:
  QWidget *m_lineNumberArea = nullptr;
  QSet<int> m_breakpoints;
  int m_debugLine = 0;
  class QCompleter *m_completer = nullptr;

  QString wordUnderCursor() const;
  void insertCompletion(const QString &completion);
};

class LineNumberArea : public QWidget {
public:
  explicit LineNumberArea(CodeEditor *editor) : QWidget(editor), m_editor(editor) {}
  QSize sizeHint() const override { return {m_editor->lineNumberAreaWidth(), 0}; }

protected:
  void paintEvent(QPaintEvent *event) override { m_editor->lineNumberAreaPaintEvent(event); }
  void mousePressEvent(QMouseEvent *event) override;

private:
  CodeEditor *m_editor;
};
