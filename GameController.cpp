#include "GameController.h"

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSet>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>
#include <QVariantMap>

#include <algorithm>
#include <cmath>

GameController::GameController(QObject *parent) : QObject(parent) {
  m_delayTimer = new QTimer(this);
  m_delayTimer->setSingleShot(true);
  connect(m_delayTimer, &QTimer::timeout, this, &GameController::resumeAfterDelay);

  const QString data = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  m_vm.save_directory = (data + "/saves").toStdString();
  QDir().mkpath(data);

  bindVmCallbacks();
  loadPlayHistory();
  rebuildPlayStats();
  refreshCatalog();
  m_mainMenuVisible = hasCatalog();
}

void GameController::bindVmCallbacks() {
  m_vm.onOutput = [this](const std::string &text) {
    if (text == "__CLEAR_SCREEN__") {
      m_logText.clear();
    } else {
      m_logText += QString::fromStdString(text) + "\n";
    }
    emit logTextChanged();
  };

  m_vm.onInventoryChanged = [this]() {
    syncInventoryFromVm();
    emit inventoryChanged();
  };

  m_vm.onTitleChanged = [this](const std::string &title) {
    m_questTitle = QString::fromStdString(title);
    emit questTitleChanged();
  };

  m_vm.onAuthorChanged = [this](const std::string &author) {
    m_questAuthor = QString::fromStdString(author);
    emit questMetadataChanged();
  };

  m_vm.onVersionChanged = [this](const std::string &ver) {
    m_questVersion = QString::fromStdString(ver);
    emit questMetadataChanged();
  };

  m_vm.onLocationChanged = [this](const std::string &loc) {
    m_currentLocation = QString::fromStdString(loc);
    emit locationChanged();
  };

  m_vm.onChoicesChanged = [this](const std::vector<std::string> &choices) {
    m_choices.clear();
    for (const auto &c : choices)
      m_choices.append(QString::fromStdString(c));
    emit choicesChanged();
  };

  m_vm.onFinished = [this](GameOutcome outcome, const std::string &msg) {
    m_endMessage = QString::fromStdString(msg);
    recordPlayResult(outcome, m_endMessage);
    emit gameOverChanged();
  };

  m_vm.onDelay = [this](Number ms) { m_delayTimer->start(static_cast<int>(ms)); };

  m_vm.onCanvasChanged = [this]() {
    syncCanvasFromVm();
    emit canvasOpsChanged();
  };
}

void GameController::resumeAfterDelay() {
  if (!m_vm.is_waiting_delay)
    return;
  m_vm.is_waiting_delay = false;
  m_vm.pending_delay_ms = 0;
  try {
    m_vm.run();
  } catch (const std::exception &e) {
    m_logText += QString("[Error after DELAY: %1]\n").arg(e.what());
    emit logTextChanged();
  }
}

void GameController::resetVmState() {
  m_vm.reset();
  m_inventory.clear();
  m_choices.clear();
  m_logText.clear();
  m_endMessage.clear();
  m_canvasOps.clear();
  m_resultRecorded = false;
  if (m_delayTimer)
    m_delayTimer->stop();
}

void GameController::syncInventoryFromVm() {
  m_inventory.clear();
  for (const auto &[item, count] : m_vm.inventory)
    m_inventory.append(QString("%1 - %2").arg(QString::fromStdString(item)).arg(count));
}

void GameController::syncCanvasFromVm() {
  m_canvasOps.clear();
  for (const auto &op : m_vm.canvas_ops)
    m_canvasOps.append(QString::fromStdString(op));
}

void GameController::clearCanvas() {
  m_vm.canvas_ops.clear();
  m_canvasOps.clear();
  emit canvasOpsChanged();
}

void GameController::recompileAndRun(const std::string &script, const QString &errorPrefix) {
  try {
    m_compiler.compile(script, m_vm);
    m_vm.run();
  } catch (const std::exception &e) {
    m_logText += QString("[%1: %2]\n").arg(errorPrefix, e.what());
    emit logTextChanged();
  }
}

QString GameController::logText() const { return m_logText; }
QStringList GameController::inventory() const { return m_inventory; }
QString GameController::questTitle() const { return m_questTitle; }
QString GameController::questAuthor() const { return m_questAuthor; }
QString GameController::questVersion() const { return m_questVersion; }
QString GameController::currentLocation() const { return m_currentLocation; }
QStringList GameController::choices() const { return m_choices; }
bool GameController::isGameOver() const { return m_vm.outcome != GameOutcome::InProgress; }
QString GameController::endMessage() const { return m_endMessage; }
bool GameController::hasCatalog() const { return !m_catalog.isEmpty(); }
bool GameController::mainMenuVisible() const { return m_mainMenuVisible; }
QVariantList GameController::catalogQuests() const { return m_catalog; }
QStringList GameController::canvasOps() const { return m_canvasOps; }
bool GameController::canvasVisible() const { return !m_canvasOps.isEmpty(); }
QVariantList GameController::playHistory() const { return m_playHistory; }
QVariantMap GameController::playStats() const { return m_playStats; }
bool GameController::historyVisible() const { return m_historyVisible; }

QString GameController::gameOutcome() const {
  switch (m_vm.outcome) {
  case GameOutcome::Victory:
    return "victory";
  case GameOutcome::Defeat:
    return "defeat";
  default:
    return "none";
  }
}

void GameController::setMainMenuVisible(bool visible) {
  if (m_mainMenuVisible == visible)
    return;
  m_mainMenuVisible = visible;
  emit mainMenuVisibleChanged();
}

void GameController::setHistoryVisible(bool visible) {
  if (m_historyVisible == visible)
    return;
  m_historyVisible = visible;
  emit historyVisibleChanged();
}

QString GameController::categoryLabel(const QString &folderName) {
  if (folderName == "warmup")
    return "Warmup";
  if (folderName == "adventures")
    return "Adventures";
  if (folderName.isEmpty() || folderName == ".")
    return "Other";
  return folderName;
}

QString GameController::readTitleFromScript(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return QFileInfo(path).baseName();
  QTextStream in(&file);
  static const QRegularExpression re(QStringLiteral("\"([^\"]+)\"\\s+TITLE:"));
  while (!in.atEnd()) {
    const auto m = re.match(in.readLine());
    if (m.hasMatch())
      return m.captured(1);
  }
  return QFileInfo(path).baseName();
}

QString GameController::findQuestsRoot() const {
  QStringList candidates;
#ifdef QUESTFORTH_QUESTS_DIR
  candidates << QString::fromUtf8(QUESTFORTH_QUESTS_DIR);
#endif
  const QString appDir = QCoreApplication::applicationDirPath();
  candidates << appDir + "/quests";
  candidates << appDir + "/../quests";
  candidates << QDir::currentPath() + "/quests";
  for (const QString &c : candidates) {
    const QDir dir(QDir::cleanPath(c));
    if (dir.exists())
      return dir.absolutePath();
  }
  return {};
}

QString GameController::historyFilePath() const {
  const QString data = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return data + "/play_history.json";
}

void GameController::loadPlayHistory() {
  m_playHistory.clear();
  QFile file(historyFilePath());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;
  const auto doc = QJsonDocument::fromJson(file.readAll());
  if (!doc.isArray())
    return;
  for (const auto &v : doc.array()) {
    if (!v.isObject())
      continue;
    const QJsonObject o = v.toObject();
    QVariantMap row;
    row.insert("timestamp", o.value("timestamp").toString());
    row.insert("title", o.value("title").toString());
    row.insert("path", o.value("path").toString());
    row.insert("outcome", o.value("outcome").toString());
    row.insert("message", o.value("message").toString());
    row.insert("durationSec", o.value("durationSec").toInt());
    m_playHistory.append(row);
  }
}

void GameController::savePlayHistory() const {
  QJsonArray arr;
  for (const auto &item : m_playHistory) {
    const QVariantMap row = item.toMap();
    QJsonObject o;
    o.insert("timestamp", row.value("timestamp").toString());
    o.insert("title", row.value("title").toString());
    o.insert("path", row.value("path").toString());
    o.insert("outcome", row.value("outcome").toString());
    o.insert("message", row.value("message").toString());
    o.insert("durationSec", row.value("durationSec").toInt());
    arr.append(o);
  }
  QFile file(historyFilePath());
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    return;
  file.write(QJsonDocument(arr).toJson(QJsonDocument::Indented));
}

void GameController::rebuildPlayStats() {
  int plays = 0;
  int wins = 0;
  int losses = 0;
  QSet<QString> uniquePlayed;
  QSet<QString> uniqueWon;
  qint64 totalDuration = 0;

  for (const auto &item : m_playHistory) {
    const QVariantMap row = item.toMap();
    const QString outcome = row.value("outcome").toString();
    const QString path = row.value("path").toString();
    const QString title = row.value("title").toString();
    const QString key = path.isEmpty() ? title : path;
    ++plays;
    uniquePlayed.insert(key);
    totalDuration += row.value("durationSec").toInt();
    if (outcome == "victory") {
      ++wins;
      uniqueWon.insert(key);
    } else if (outcome == "defeat") {
      ++losses;
    }
  }

  const double winRate =
    plays > 0 ? (100.0 * static_cast<double>(wins) / static_cast<double>(plays)) : 0.0;

  m_playStats.clear();
  m_playStats.insert("plays", plays);
  m_playStats.insert("victories", wins);
  m_playStats.insert("defeats", losses);
  m_playStats.insert("winRate", std::round(winRate * 10.0) / 10.0);
  m_playStats.insert("uniqueQuests", uniquePlayed.size());
  m_playStats.insert("uniqueVictories", uniqueWon.size());
  m_playStats.insert("totalDurationSec", static_cast<qlonglong>(totalDuration));
  emit playHistoryChanged();
}

void GameController::recordPlayResult(GameOutcome outcome, const QString &message) {
  if (m_resultRecorded)
    return;
  if (outcome != GameOutcome::Victory && outcome != GameOutcome::Defeat)
    return;
  m_resultRecorded = true;

  int durationSec = 0;
  if (m_playStartedAt.isValid())
    durationSec = static_cast<int>(m_playStartedAt.secsTo(QDateTime::currentDateTimeUtc()));

  QVariantMap row;
  row.insert("timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
  row.insert("title", m_questTitle);
  row.insert("path", m_currentQuestPath);
  row.insert("outcome", outcome == GameOutcome::Victory ? QStringLiteral("victory")
                                                        : QStringLiteral("defeat"));
  row.insert("message", message);
  row.insert("durationSec", durationSec);

  m_playHistory.prepend(row);
  while (m_playHistory.size() > kMaxHistoryEntries)
    m_playHistory.removeLast();

  savePlayHistory();
  rebuildPlayStats();
  if (m_mainMenuVisible)
    refreshCatalog();
}

void GameController::clearPlayHistory() {
  m_playHistory.clear();
  savePlayHistory();
  rebuildPlayStats();
  refreshCatalog();
}

void GameController::refreshCatalog() {
  m_catalog.clear();
  const QString root = findQuestsRoot();
  if (root.isEmpty()) {
    emit catalogChanged();
    return;
  }

  // Aggregate per-path stats from history for catalog badges
  QHash<QString, int> playsByPath;
  QHash<QString, int> winsByPath;
  for (const auto &item : m_playHistory) {
    const QVariantMap row = item.toMap();
    const QString path = row.value("path").toString();
    if (path.isEmpty())
      continue;
    playsByPath[path] += 1;
    if (row.value("outcome").toString() == "victory")
      winsByPath[path] += 1;
  }

  QDirIterator it(root, {"*.forth", "*.fth"}, QDir::Files, QDirIterator::Subdirectories);
  QList<QVariantMap> entries;
  while (it.hasNext()) {
    const QString path = it.next();
    const QFileInfo fi(path);
    QVariantMap row;
    row.insert("title", readTitleFromScript(path));
    row.insert("path", fi.absoluteFilePath());
    row.insert("fileName", fi.fileName());
    const QString folder = fi.dir().dirName();
    row.insert("category",
               categoryLabel(folder == QFileInfo(root).fileName() ? QString() : folder));
    int order = 50;
    if (folder == "warmup")
      order = 10;
    else if (folder == "adventures")
      order = 20;
    row.insert("sortKey", QString("%1/%2").arg(order, 2, 10, QChar('0')).arg(fi.fileName()));
    row.insert("plays", playsByPath.value(fi.absoluteFilePath(), 0));
    row.insert("victories", winsByPath.value(fi.absoluteFilePath(), 0));
    entries.push_back(row);
  }
  std::sort(entries.begin(), entries.end(), [](const QVariantMap &a, const QVariantMap &b) {
    return a.value("sortKey").toString() < b.value("sortKey").toString();
  });
  for (const auto &e : entries)
    m_catalog.append(e);
  emit catalogChanged();
}

void GameController::loadQuestFromLocalPath(const QString &localPath) {
  QFile file(localPath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    m_logText += QString("[Error: cannot open file %1]\n").arg(localPath);
    emit logTextChanged();
    return;
  }
  QTextStream in(&file);
  m_rawScript = in.readAll().toStdString();
  file.close();

  resetVmState();
  m_currentQuestPath = QFileInfo(localPath).absoluteFilePath();
  m_playStartedAt = QDateTime::currentDateTimeUtc();
  m_questTitle = QFileInfo(localPath).fileName();
  m_questAuthor.clear();
  m_questVersion.clear();
  m_currentLocation = "Unknown location";
  m_questLoaded = true;
  setHistoryVisible(false);
  setMainMenuVisible(false);

  emit logTextChanged();
  emit inventoryChanged();
  emit questTitleChanged();
  emit questMetadataChanged();
  emit locationChanged();
  emit choicesChanged();
  emit gameOverChanged();
  emit canvasOpsChanged();

  recompileAndRun(m_rawScript, "Compile/runtime error");
}

void GameController::loadQuestFile(const QUrl &fileUrl) {
  loadQuestFromLocalPath(fileUrl.toLocalFile());
}

void GameController::loadQuestPath(const QString &localPath) { loadQuestFromLocalPath(localPath); }

void GameController::restartQuest() {
  if (m_rawScript.empty())
    return;
  resetVmState();
  m_playStartedAt = QDateTime::currentDateTimeUtc();
  setMainMenuVisible(false);
  emit gameOverChanged();
  emit inventoryChanged();
  emit choicesChanged();
  emit logTextChanged();
  emit canvasOpsChanged();
  recompileAndRun(m_rawScript, "Restart error");
}

void GameController::returnToMainMenu() {
  refreshCatalog();
  resetVmState();
  m_rawScript.clear();
  m_currentQuestPath.clear();
  m_questLoaded = false;
  m_questTitle = "QuestForth";
  m_questAuthor.clear();
  m_questVersion.clear();
  m_currentLocation.clear();
  m_vm.outcome = GameOutcome::InProgress;
  setHistoryVisible(false);
  emit questTitleChanged();
  emit questMetadataChanged();
  emit locationChanged();
  emit inventoryChanged();
  emit choicesChanged();
  emit logTextChanged();
  emit gameOverChanged();
  emit canvasOpsChanged();
  setMainMenuVisible(hasCatalog());
}

void GameController::makeChoice(int index) {
  if (!m_vm.is_waiting_choice)
    return;
  m_vm.push(int64_t{index + 1});
  m_vm.is_waiting_choice = false;
  m_vm.pending_choices.clear();
  m_choices.clear();
  emit choicesChanged();
  try {
    m_vm.run();
  } catch (const std::exception &e) {
    m_logText += QString("[Runtime error: %1]\n").arg(e.what());
    emit logTextChanged();
  }
}
