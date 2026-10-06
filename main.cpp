#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "GameController.h"

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  QGuiApplication::setApplicationName("QuestForth");
  QGuiApplication::setOrganizationName("QuestForth");

  QQmlApplicationEngine engine;

  GameController controller;
  engine.rootContext()->setContextProperty("gameController", &controller);

  const QUrl url(QStringLiteral("qrc:/QuestForth/main.qml"));
  QObject::connect(
    &engine, &QQmlApplicationEngine::objectCreated, &app,
    [url](QObject *obj, const QUrl &objUrl) {
      if (!obj && url == objUrl)
        QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);
  engine.load(url);

  return QGuiApplication::exec();
}
