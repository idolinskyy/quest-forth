#include "MainWindow.h"

#include <QApplication>
#include <QFont>
#include <QFontDatabase>
#include <QFontInfo>
#include <QStyleFactory>

namespace {

QFont pickMonoFont(int pointSize) {
  const QStringList candidates = {"JetBrains Mono",  "Cascadia Code", "Fira Code",
                                  "Source Code Pro", "Consolas",      "DejaVu Sans Mono"};
  for (const QString &name : candidates) {
    QFont f(name, pointSize);
    if (QFontInfo(f).family().contains(name, Qt::CaseInsensitive))
      return f;
  }
  QFont f = QFontDatabase::systemFont(QFontDatabase::FixedFont);
  f.setPointSize(pointSize);
  f.setStyleHint(QFont::Monospace);
  return f;
}

const char *kStyleSheet = R"(
QMainWindow, QWidget {
  background-color: #12141a;
  color: #d8dce6;
  font-family: "IBM Plex Sans", "Segoe UI", Ubuntu, Cantarell, sans-serif;
  font-size: 12px;
}
QMenuBar {
  background-color: #1a1d26;
  border-bottom: 1px solid #252a35;
  padding: 2px 4px;
  spacing: 4px;
}
QMenuBar::item {
  background: transparent;
  padding: 5px 10px;
  border-radius: 4px;
  color: #8b929e;
}
QMenuBar::item:selected, QMenuBar::item:pressed {
  background-color: #222632;
  color: #d8dce6;
}
QMenu {
  background-color: #1a1d26;
  border: 1px solid #2e3440;
  padding: 4px;
}
QMenu::item {
  padding: 6px 28px 6px 16px;
  border-radius: 4px;
  color: #d8dce6;
}
QMenu::item:selected {
  background-color: #2a3040;
}
QMenu::separator {
  height: 1px;
  background: #252a35;
  margin: 4px 8px;
}
QToolBar {
  background-color: #1a1d26;
  border-bottom: 1px solid #252a35;
  spacing: 4px;
  padding: 4px 8px;
}
QToolBar::separator {
  background: #2e3440;
  width: 1px;
  margin: 6px 6px;
}
QToolButton {
  background-color: transparent;
  border: 1px solid transparent;
  border-radius: 5px;
  padding: 5px 10px;
  color: #d8dce6;
}
QToolButton:hover {
  background-color: #222632;
  border-color: #2e3440;
}
QToolButton:pressed {
  background-color: #2a3040;
}
QToolButton:disabled {
  color: #5c6370;
}
QStatusBar {
  background-color: #1a1d26;
  border-top: 1px solid #252a35;
  color: #5c6370;
}
QStatusBar QLabel {
  color: #5c6370;
  padding: 0 6px;
}
QSplitter::handle {
  background-color: #12141a;
}
QSplitter::handle:horizontal {
  width: 4px;
}
QSplitter::handle:vertical {
  height: 4px;
}
QPlainTextEdit, QTextEdit {
  background-color: #161920;
  color: #d8dce6;
  border: 1px solid #252a35;
  border-radius: 6px;
  selection-background-color: #2a4060;
  selection-color: #ffffff;
  padding: 4px;
  font-family: "JetBrains Mono", "Cascadia Code", "Fira Code", "DejaVu Sans Mono",
               "Noto Sans Mono", "Liberation Mono", "Courier New", monospace;
  font-size: 12pt;
}
QListWidget {
  background-color: #161920;
  color: #d8dce6;
  border: 1px solid #252a35;
  border-radius: 6px;
  outline: none;
  padding: 2px;
  font-family: "JetBrains Mono", "Cascadia Code", "Fira Code", "DejaVu Sans Mono",
               "Noto Sans Mono", "Liberation Mono", "Courier New", monospace;
  font-size: 11pt;
}
QListWidget::item {
  padding: 5px 8px;
  border-radius: 3px;
}
QListWidget::item:selected {
  background-color: #2a4060;
  color: #ffffff;
}
QListWidget::item:hover:!selected {
  background-color: #222632;
}
QLabel#panelTitle {
  color: #8b929e;
  font-weight: 600;
  font-size: 11px;
  padding: 2px 2px 4px 2px;
}
QLabel#debugBanner {
  color: #8b929e;
  background-color: #1a1d26;
  border: 1px solid #252a35;
  border-radius: 6px;
  padding: 8px 12px;
  font-size: 12px;
}
QLabel#debugBanner[active="true"] {
  color: #c9a227;
  border-color: #4a4030;
}
QScrollBar:vertical {
  background: #12141a;
  width: 10px;
  margin: 0;
}
QScrollBar::handle:vertical {
  background: #2e3440;
  border-radius: 4px;
  min-height: 24px;
}
QScrollBar::handle:vertical:hover {
  background: #3a4150;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
  height: 0;
}
QScrollBar:horizontal {
  background: #12141a;
  height: 10px;
}
QScrollBar::handle:horizontal {
  background: #2e3440;
  border-radius: 4px;
  min-width: 24px;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
  width: 0;
}
QToolTip {
  background-color: #222632;
  color: #d8dce6;
  border: 1px solid #2e3440;
  padding: 6px 8px;
}
)";

} // namespace

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("QuestForthEditor");
  QApplication::setOrganizationName("QuestForth");
  app.setStyle(QStyleFactory::create("Fusion"));

  QPalette dark;
  dark.setColor(QPalette::Window, QColor("#12141a"));
  dark.setColor(QPalette::WindowText, QColor("#d8dce6"));
  dark.setColor(QPalette::Base, QColor("#161920"));
  dark.setColor(QPalette::AlternateBase, QColor("#1a1d26"));
  dark.setColor(QPalette::Text, QColor("#d8dce6"));
  dark.setColor(QPalette::Button, QColor("#222632"));
  dark.setColor(QPalette::ButtonText, QColor("#d8dce6"));
  dark.setColor(QPalette::Highlight, QColor("#2a4060"));
  dark.setColor(QPalette::HighlightedText, Qt::white);
  dark.setColor(QPalette::ToolTipBase, QColor("#222632"));
  dark.setColor(QPalette::ToolTipText, QColor("#d8dce6"));
  dark.setColor(QPalette::PlaceholderText, QColor("#5c6370"));
  dark.setColor(QPalette::Link, QColor("#6cb2e3"));
  app.setPalette(dark);
  app.setFont(QFont("IBM Plex Sans", 10));
  app.setStyleSheet(QString::fromUtf8(kStyleSheet));

  // Expose mono font via application property for widgets that need it
  app.setProperty("monoFont", pickMonoFont(12));
  app.setProperty("monoFontSmall", pickMonoFont(11));

  MainWindow w;
  w.show();
  if (argc > 1)
    w.openPath(QString::fromLocal8Bit(argv[1]));

  return app.exec();
}
