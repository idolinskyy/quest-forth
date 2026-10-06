#include "MainWindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName("QuestForthEditor");
  QApplication::setOrganizationName("QuestForth");
  app.setStyle(QStyleFactory::create("Fusion"));

  QPalette dark;
  dark.setColor(QPalette::Window, QColor("#181a1f"));
  dark.setColor(QPalette::WindowText, QColor("#abb2bf"));
  dark.setColor(QPalette::Base, QColor("#21252b"));
  dark.setColor(QPalette::AlternateBase, QColor("#2c313a"));
  dark.setColor(QPalette::Text, QColor("#abb2bf"));
  dark.setColor(QPalette::Button, QColor("#2c313a"));
  dark.setColor(QPalette::ButtonText, QColor("#abb2bf"));
  dark.setColor(QPalette::Highlight, QColor("#3a3f4b"));
  dark.setColor(QPalette::HighlightedText, Qt::white);
  dark.setColor(QPalette::ToolTipBase, QColor("#2c313a"));
  dark.setColor(QPalette::ToolTipText, QColor("#abb2bf"));
  app.setPalette(dark);

  MainWindow w;
  w.show();
  if (argc > 1)
    w.openPath(QString::fromLocal8Bit(argv[1]));

  return app.exec();
}
