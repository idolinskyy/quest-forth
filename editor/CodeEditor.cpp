#include "CodeEditor.h"
#include "WordDictionary.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QCompleter>
#include <QHelpEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextBlock>
#include <QToolTip>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent) {
  m_lineNumberArea = new LineNumberArea(this);

  connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
  connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
  connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();

  setFont(QFont("JetBrains Mono", 11));
  setTabStopDistance(fontMetrics().horizontalAdvance(QLatin1Char(' ')) * 4);
  setMouseTracking(true);

  m_completer = new QCompleter(WordDictionary::instance().allWords(), this);
  m_completer->setWidget(this);
  m_completer->setCompletionMode(QCompleter::PopupCompletion);
  m_completer->setCaseSensitivity(Qt::CaseSensitive);
  m_completer->setFilterMode(Qt::MatchStartsWith);
  connect(m_completer, QOverload<const QString &>::of(&QCompleter::activated), this,
          &CodeEditor::insertCompletion);
}

int CodeEditor::lineNumberAreaWidth() const {
  int digits = 1;
  int max = qMax(1, blockCount());
  while (max >= 10) {
    max /= 10;
    ++digits;
  }
  return 8 + 18 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void CodeEditor::updateLineNumberAreaWidth(int) {
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
  if (dy)
    m_lineNumberArea->scroll(0, dy);
  else
    m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
  if (rect.contains(viewport()->rect()))
    updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
  QPlainTextEdit::resizeEvent(e);
  QRect cr = contentsRect();
  m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine() {
  QList<QTextEdit::ExtraSelection> extras;

  if (!isReadOnly()) {
    QTextEdit::ExtraSelection sel;
    sel.format.setBackground(QColor("#2c313a"));
    sel.format.setProperty(QTextFormat::FullWidthSelection, true);
    sel.cursor = textCursor();
    sel.cursor.clearSelection();
    extras.append(sel);
  }

  if (m_debugLine > 0) {
    QTextBlock block = document()->findBlockByNumber(m_debugLine - 1);
    if (block.isValid()) {
      QTextEdit::ExtraSelection dbg;
      dbg.format.setBackground(QColor("#3e4452"));
      dbg.format.setProperty(QTextFormat::FullWidthSelection, true);
      dbg.cursor = QTextCursor(block);
      dbg.cursor.clearSelection();
      extras.append(dbg);
    }
  }

  setExtraSelections(extras);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
  QPainter painter(m_lineNumberArea);
  painter.fillRect(event->rect(), QColor("#21252b"));

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
  int bottom = top + qRound(blockBoundingRect(block).height());

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      const int line = blockNumber + 1;
      if (m_breakpoints.contains(line)) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#e06c75"));
        painter.drawEllipse(4, top + 4, 10, 10);
      }
      if (m_debugLine == line) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#e5c07b"));
        QPolygon tri;
        tri << QPoint(16, top + 4) << QPoint(16, top + 14) << QPoint(22, top + 9);
        painter.drawPolygon(tri);
      }
      painter.setPen(QColor("#5c6370"));
      painter.drawText(0, top, m_lineNumberArea->width() - 4, fontMetrics().height(),
                       Qt::AlignRight, QString::number(line));
    }
    block = block.next();
    top = bottom;
    bottom = top + qRound(blockBoundingRect(block).height());
    ++blockNumber;
  }
}

void CodeEditor::setBreakpoint(int line1based, bool on) {
  if (on)
    m_breakpoints.insert(line1based);
  else
    m_breakpoints.remove(line1based);
  m_lineNumberArea->update();
  emit breakpointToggled(line1based, on);
}

void CodeEditor::toggleBreakpoint(int line1based) {
  setBreakpoint(line1based, !m_breakpoints.contains(line1based));
}

bool CodeEditor::hasBreakpoint(int line1based) const { return m_breakpoints.contains(line1based); }

void CodeEditor::setCurrentDebugLine(int line1based) {
  m_debugLine = line1based;
  highlightCurrentLine();
  m_lineNumberArea->update();
  if (line1based > 0) {
    QTextBlock block = document()->findBlockByNumber(line1based - 1);
    if (block.isValid()) {
      QTextCursor c(block);
      setTextCursor(c);
      centerCursor();
    }
  }
}

QString CodeEditor::wordUnderCursor() const {
  QTextCursor tc = textCursor();
  tc.select(QTextCursor::WordUnderCursor);
  // WordUnderCursor is weak for []@ etc. — scan manually
  tc = textCursor();
  QString block = tc.block().text();
  int pos = tc.positionInBlock();
  if (pos > block.size())
    pos = block.size();
  int start = pos;
  while (start > 0 && !block[start - 1].isSpace())
    --start;
  int end = pos;
  while (end < block.size() && !block[end].isSpace())
    ++end;
  return block.mid(start, end - start);
}

void CodeEditor::insertCompletion(const QString &completion) {
  if (m_completer->widget() != this)
    return;
  QTextCursor tc = textCursor();
  const QString prefix = m_completer->completionPrefix();
  for (int i = 0; i < prefix.size(); ++i)
    tc.deletePreviousChar();
  tc.insertText(completion);
  setTextCursor(tc);
}

void CodeEditor::keyPressEvent(QKeyEvent *e) {
  if (m_completer && m_completer->popup()->isVisible()) {
    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Escape:
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
      e->ignore();
      return;
    default:
      break;
    }
  }

  const bool isShortcut =
    (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_Space);
  if (!m_completer || !isShortcut)
    QPlainTextEdit::keyPressEvent(e);

  const bool ctrlOrShift =
    e->modifiers().testFlag(Qt::ControlModifier) || e->modifiers().testFlag(Qt::ShiftModifier);
  if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
    return;

  static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-=");
  const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
  QString completionPrefix = wordUnderCursor();

  if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 1 ||
                      eow.contains(e->text().right(1)))) {
    m_completer->popup()->hide();
    return;
  }

  if (completionPrefix != m_completer->completionPrefix()) {
    m_completer->setCompletionPrefix(completionPrefix);
    m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
  }
  QRect cr = cursorRect();
  cr.setWidth(m_completer->popup()->sizeHintForColumn(0) +
              m_completer->popup()->verticalScrollBar()->sizeHint().width());
  m_completer->complete(cr);
}

bool CodeEditor::event(QEvent *event) {
  if (event->type() == QEvent::ToolTip) {
    auto *help = static_cast<QHelpEvent *>(event);
    QTextCursor tc = cursorForPosition(help->pos());
    // temporarily move cursor for wordUnderCursor
    QTextCursor old = textCursor();
    setTextCursor(tc);
    const QString w = wordUnderCursor();
    setTextCursor(old);
    const QString html = WordDictionary::instance().tooltipHtml(w);
    if (!html.isEmpty())
      QToolTip::showText(help->globalPos(), html, this);
    else
      QToolTip::hideText();
    return true;
  }
  return QPlainTextEdit::event(event);
}

void CodeEditor::lineNumberAreaClicked(int y) {
  QTextBlock block = firstVisibleBlock();
  int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
  int bottom = top + qRound(blockBoundingRect(block).height());
  int blockNumber = block.blockNumber();
  while (block.isValid()) {
    if (y >= top && y < bottom) {
      toggleBreakpoint(blockNumber + 1);
      return;
    }
    block = block.next();
    top = bottom;
    bottom = top + qRound(blockBoundingRect(block).height());
    ++blockNumber;
  }
}

void LineNumberArea::mousePressEvent(QMouseEvent *event) {
  if (event->button() != Qt::LeftButton)
    return;
  m_editor->lineNumberAreaClicked(static_cast<int>(event->position().y()));
}
