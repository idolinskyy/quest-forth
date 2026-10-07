#pragma once

#include "compiler.h"
#include "vm.h"

#include <QDateTime>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>

class GameController : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString logText READ logText NOTIFY logTextChanged)
  Q_PROPERTY(QStringList inventory READ inventory NOTIFY inventoryChanged)
  Q_PROPERTY(QString questTitle READ questTitle NOTIFY questTitleChanged)
  Q_PROPERTY(QString questAuthor READ questAuthor NOTIFY questMetadataChanged)
  Q_PROPERTY(QString questVersion READ questVersion NOTIFY questMetadataChanged)
  Q_PROPERTY(QString currentLocation READ currentLocation NOTIFY locationChanged)
  Q_PROPERTY(QStringList choices READ choices NOTIFY choicesChanged)
  Q_PROPERTY(bool isGameOver READ isGameOver NOTIFY gameOverChanged)
  Q_PROPERTY(QString gameOutcome READ gameOutcome NOTIFY gameOverChanged)
  Q_PROPERTY(QString endMessage READ endMessage NOTIFY gameOverChanged)
  Q_PROPERTY(bool hasCatalog READ hasCatalog NOTIFY catalogChanged)
  Q_PROPERTY(bool mainMenuVisible READ mainMenuVisible NOTIFY mainMenuVisibleChanged)
  Q_PROPERTY(QVariantList catalogQuests READ catalogQuests NOTIFY catalogChanged)
  Q_PROPERTY(QStringList canvasOps READ canvasOps NOTIFY canvasOpsChanged)
  Q_PROPERTY(bool canvasVisible READ canvasVisible NOTIFY canvasOpsChanged)
  Q_PROPERTY(QVariantList playHistory READ playHistory NOTIFY playHistoryChanged)
  Q_PROPERTY(QVariantMap playStats READ playStats NOTIFY playHistoryChanged)
  Q_PROPERTY(
    bool historyVisible READ historyVisible WRITE setHistoryVisible NOTIFY historyVisibleChanged)

public:
  explicit GameController(QObject *parent = nullptr);

  QString logText() const;
  QStringList inventory() const;
  QString questTitle() const;
  QString questAuthor() const;
  QString questVersion() const;
  QString currentLocation() const;
  QStringList choices() const;
  bool isGameOver() const;
  QString endMessage() const;
  QString gameOutcome() const;
  bool hasCatalog() const;
  bool mainMenuVisible() const;
  QVariantList catalogQuests() const;
  QStringList canvasOps() const;
  bool canvasVisible() const;
  QVariantList playHistory() const;
  QVariantMap playStats() const;
  bool historyVisible() const;

  Q_INVOKABLE void loadQuestFile(const QUrl &fileUrl);
  Q_INVOKABLE void loadQuestPath(const QString &localPath);
  Q_INVOKABLE void restartQuest();
  Q_INVOKABLE void makeChoice(int index);
  Q_INVOKABLE void returnToMainMenu();
  Q_INVOKABLE void refreshCatalog();
  Q_INVOKABLE void clearCanvas();
  Q_INVOKABLE void setHistoryVisible(bool visible);
  Q_INVOKABLE void clearPlayHistory();

signals:
  void logTextChanged();
  void inventoryChanged();
  void questTitleChanged();
  void questMetadataChanged();
  void locationChanged();
  void choicesChanged();
  void gameOverChanged();
  void catalogChanged();
  void mainMenuVisibleChanged();
  void canvasOpsChanged();
  void playHistoryChanged();
  void historyVisibleChanged();

private:
  void bindVmCallbacks();
  void resetVmState();
  void syncInventoryFromVm();
  void syncCanvasFromVm();
  void recompileAndRun(const std::string &script, const QString &errorPrefix);
  void loadQuestFromLocalPath(const QString &localPath);
  void setMainMenuVisible(bool visible);
  void resumeAfterDelay();
  void recordPlayResult(GameOutcome outcome, const QString &message);
  void loadPlayHistory();
  void savePlayHistory() const;
  void rebuildPlayStats();
  QString historyFilePath() const;
  QString findQuestsRoot() const;
  static QString readTitleFromScript(const QString &path);
  static QString categoryLabel(const QString &folderName);

  VM m_vm;
  Compiler m_compiler;
  QString m_logText;
  QStringList m_inventory;
  QString m_questTitle = "QuestForth";
  QString m_questAuthor;
  QString m_questVersion;
  QString m_currentLocation = "Game start";
  QStringList m_choices;
  QStringList m_canvasOps;
  std::string m_rawScript;
  QString m_endMessage;
  QVariantList m_catalog;
  bool m_mainMenuVisible = true;
  bool m_questLoaded = false;
  bool m_historyVisible = false;
  class QTimer *m_delayTimer = nullptr;

  QString m_currentQuestPath;
  QDateTime m_playStartedAt;
  bool m_resultRecorded = false;

  QVariantList m_playHistory;
  QVariantMap m_playStats;
  static constexpr int kMaxHistoryEntries = 200;
};
