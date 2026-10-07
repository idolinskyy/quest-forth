#include "MainWindow.h"
#include "CodeEditor.h"
#include "ForthHighlighter.h"

#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QStringConverter>
#include <QStyle>
#include <QTextStream>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

namespace {
QFont appMono(const char *prop, int fallbackPt) {
  const QVariant v = qApp->property(prop);
  if (v.canConvert<QFont>())
    return v.value<QFont>();
  QFont f(QStringLiteral("monospace"), fallbackPt);
  f.setStyleHint(QFont::Monospace);
  return f;
}
} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setupUi();
  setupMenus();
  newFile();
  resize(1280, 840);
  setWindowTitle("QuestForth Editor");
}

void MainWindow::setupUi() {
  auto *central = new QWidget(this);
  auto *root = new QHBoxLayout(central);
  root->setContentsMargins(8, 8, 8, 8);
  root->setSpacing(0);

  auto *splitter = new QSplitter(Qt::Horizontal, central);
  splitter->setChildrenCollapsible(false);

  auto *editorPane = new QWidget(splitter);
  auto *editorLayout = new QVBoxLayout(editorPane);
  editorLayout->setContentsMargins(0, 0, 4, 0);
  editorLayout->setSpacing(6);

  auto *editorTitle = new QLabel("Source", editorPane);
  editorTitle->setObjectName("panelTitle");
  editorLayout->addWidget(editorTitle);

  m_editor = new CodeEditor(editorPane);
  new ForthHighlighter(m_editor->document());
  connect(m_editor, &CodeEditor::textChanged, this, &MainWindow::markDirty);
  editorLayout->addWidget(m_editor, 1);

  auto *right = new QWidget(splitter);
  auto *rv = new QVBoxLayout(right);
  rv->setContentsMargins(4, 0, 0, 0);
  rv->setSpacing(8);

  m_debugStatus = new QLabel("Debug idle — F5 to start, click gutter for breakpoints", right);
  m_debugStatus->setObjectName("debugBanner");
  m_debugStatus->setProperty("active", false);
  rv->addWidget(m_debugStatus);

  auto *dbgSplit = new QSplitter(Qt::Vertical, right);
  dbgSplit->setChildrenCollapsible(false);

  auto *outPane = new QWidget(dbgSplit);
  auto *outLayout = new QVBoxLayout(outPane);
  outLayout->setContentsMargins(0, 0, 0, 0);
  outLayout->setSpacing(4);
  auto *outTitle = new QLabel("Output", outPane);
  outTitle->setObjectName("panelTitle");
  outLayout->addWidget(outTitle);
  m_output = new QPlainTextEdit(outPane);
  m_output->setReadOnly(true);
  m_output->setPlaceholderText("Compile and debug output appears here…");
  m_output->setFont(appMono("monoFontSmall", 10));
  outLayout->addWidget(m_output, 1);

  auto *inspect = new QWidget(dbgSplit);
  auto *ih = new QHBoxLayout(inspect);
  ih->setContentsMargins(0, 0, 0, 0);
  ih->setSpacing(8);

  const QFont listFont = appMono("monoFontSmall", 9);

  auto makeList = [&listFont](const QString &title, QWidget *parent) {
    auto *box = new QWidget(parent);
    auto *v = new QVBoxLayout(box);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(4);
    auto *label = new QLabel(title, box);
    label->setObjectName("panelTitle");
    v->addWidget(label);
    auto *list = new QListWidget(box);
    list->setFont(listFont);
    list->setAlternatingRowColors(true);
    v->addWidget(list, 1);
    return std::pair{box, list};
  };

  auto [stackBox, stackList] = makeList("Stack", inspect);
  m_stackView = stackList;
  auto [varsBox, varsList] = makeList("Variables", inspect);
  m_varsView = varsList;
  auto [invBox, invList] = makeList("Inventory", inspect);
  m_invView = invList;
  auto [chBox, chList] = makeList("Choices", inspect);
  m_choicesView = chList;
  m_choicesView->setToolTip("Double-click a choice to answer WAIT_CHOICE");
  connect(m_choicesView, &QListWidget::itemDoubleClicked, this,
          [this](QListWidgetItem *item) { makeDebugChoice(m_choicesView->row(item)); });

  ih->addWidget(stackBox, 1);
  ih->addWidget(varsBox, 1);
  ih->addWidget(invBox, 1);
  ih->addWidget(chBox, 1);

  dbgSplit->addWidget(outPane);
  dbgSplit->addWidget(inspect);
  dbgSplit->setStretchFactor(0, 2);
  dbgSplit->setStretchFactor(1, 3);
  rv->addWidget(dbgSplit, 1);

  splitter->addWidget(editorPane);
  splitter->addWidget(right);
  splitter->setStretchFactor(0, 3);
  splitter->setStretchFactor(1, 2);
  splitter->setSizes({720, 480});

  root->addWidget(splitter);
  setCentralWidget(central);

  m_status = new QLabel(this);
  statusBar()->addWidget(m_status, 1);
  statusBar()->addPermanentWidget(new QLabel("QuestForth Editor", this));

  auto *tb = addToolBar("Debug");
  tb->setMovable(false);
  tb->setFloatable(false);
  tb->setIconSize(QSize(16, 16));
  tb->addAction("Start", this, &MainWindow::debugStart)->setToolTip("Start debugging (F5)");
  tb->addAction("Continue", this, &MainWindow::debugContinue)->setToolTip("Continue (F5)");
  tb->addAction("Step", this, &MainWindow::debugStep)->setToolTip("Step (F10)");
  tb->addAction("Stop", this, &MainWindow::debugStop)->setToolTip("Stop (Shift+F5)");
  tb->addSeparator();
  tb->addAction("Check", this, &MainWindow::checkCompile)->setToolTip("Check compile (F7)");
}

void MainWindow::setupMenus() {
  auto *file = menuBar()->addMenu("&File");
  file->addAction("&New", QKeySequence::New, this, &MainWindow::newFile);
  file->addAction("&Open…", QKeySequence::Open, this, &MainWindow::openFile);
  file->addAction("&Save", QKeySequence::Save, this, &MainWindow::saveFile);
  file->addAction("Save &As…", QKeySequence::SaveAs, this, &MainWindow::saveFileAs);
  file->addSeparator();
  file->addAction("&Quit", QKeySequence::Quit, this, &QWidget::close);

  auto *run = menuBar()->addMenu("&Run");
  run->addAction("Check compile", QKeySequence(Qt::Key_F7), this, &MainWindow::checkCompile);
  run->addAction("Start debugging", QKeySequence(Qt::Key_F5), this, &MainWindow::debugStart);
  run->addAction("Continue", QKeySequence(Qt::Key_F5), this, &MainWindow::debugContinue);
  run->addAction("Step", QKeySequence(Qt::Key_F10), this, &MainWindow::debugStep);
  run->addAction("Stop", QKeySequence(Qt::ShiftModifier | Qt::Key_F5), this,
                 &MainWindow::debugStop);

  auto *help = menuBar()->addMenu("&Help");
  help->addAction("About", this, [this] {
    QMessageBox::information(this, "QuestForth Editor",
                             "Standalone editor for QuestForth scripts.\n\n"
                             "• Line numbers and breakpoints (click the gutter)\n"
                             "• Syntax highlighting\n"
                             "• Word autocomplete (Ctrl+Space or while typing)\n"
                             "• Hover help for words\n"
                             "• Debugging: F5 start/continue, F10 step, stack/vars/choices\n\n"
                             "Full language spec: LANGUAGE.md");
  });
}

void MainWindow::markDirty() {
  m_dirty = true;
  setWindowModified(true);
}

void MainWindow::setCurrentPath(const QString &path) {
  m_path = path;
  m_dirty = false;
  setWindowModified(false);
  const QString name =
    path.isEmpty() ? QStringLiteral("untitled.forth") : QFileInfo(path).fileName();
  setWindowTitle(QStringLiteral("%1[*] — QuestForth Editor").arg(name));
  m_status->setText(path.isEmpty() ? QStringLiteral("New file") : path);
}

bool MainWindow::maybeSave() {
  if (!m_dirty)
    return true;
  const auto r =
    QMessageBox::warning(this, "Unsaved changes", "Save changes?",
                         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  if (r == QMessageBox::Save) {
    saveFile();
    return !m_dirty;
  }
  return r != QMessageBox::Cancel;
}

void MainWindow::newFile() {
  if (!maybeSave())
    return;
  debugStop();
  m_editor->setPlainText(
    QStringLiteral("\"New quest\" TITLE:\n\"Author\" AUTHOR:\n\"0.1\" VERSION:\n\n"
                   "SCENE: start\n"
                   "  CLS\n"
                   "  \"Start\" LOCATION:\n"
                   "  \"Welcome to the quest editor.\" .\n"
                   "  \"Continue\" CHOICE\n"
                   "  WAIT_CHOICE\n"
                   "  drop\n"
                   "  \"Demo complete.\" VICTORY\n"
                   ";SCENE\n"));
  setCurrentPath({});
  m_dirty = false;
  setWindowModified(false);
}

void MainWindow::openPath(const QString &path) {
  QFile f(path);
  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "Error", "Could not open file:\n" + path);
    return;
  }
  debugStop();
  m_editor->setPlainText(QString::fromUtf8(f.readAll()));
  setCurrentPath(path);
}

void MainWindow::openFile() {
  if (!maybeSave())
    return;
  const QString path = QFileDialog::getOpenFileName(
    this, "Open quest", {}, "Quests (*.forth *.fth *.txt);;All files (*.*)");
  if (path.isEmpty())
    return;
  openPath(path);
}

void MainWindow::saveFile() {
  if (m_path.isEmpty()) {
    saveFileAs();
    return;
  }
  QFile f(m_path);
  if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "Error", "Could not save");
    return;
  }
  QTextStream out(&f);
  out.setEncoding(QStringConverter::Utf8);
  out << m_editor->toPlainText();
  setCurrentPath(m_path);
}

void MainWindow::saveFileAs() {
  const QString path = QFileDialog::getSaveFileName(this, "Save quest", "quest.forth",
                                                    "Quests (*.forth *.fth);;All files (*.*)");
  if (path.isEmpty())
    return;
  m_path = path;
  saveFile();
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (maybeSave())
    event->accept();
  else
    event->ignore();
}

QString MainWindow::valueToDisplay(const Value &v) const {
  return QString::fromStdString(value_to_string(v));
}

void MainWindow::checkCompile() {
  VM probe;
  Compiler c;
  try {
    c.compile(m_editor->toPlainText().toStdString(), probe);
    m_output->appendPlainText(QStringLiteral("[OK] Compile succeeded: %1 ops, %2 scenes")
                                .arg(probe.code.size())
                                .arg(probe.scenes.size()));
    m_status->setText("Compile OK");
  } catch (const std::exception &e) {
    m_output->appendPlainText(QStringLiteral("[FAIL] %1").arg(e.what()));
    m_status->setText("Compile error");
    QMessageBox::warning(this, "Compile", e.what());
  }
}

bool MainWindow::prepareSession(bool resetVm) {
  try {
    if (resetVm) {
      m_vm = VM{};
      m_vm.save_directory = "saves";
      m_vm.onOutput = [this](const std::string &s) {
        if (s == "__CLEAR_SCREEN__")
          m_output->clear();
        else
          m_output->appendPlainText(QString::fromStdString(s));
      };
      // No onDelay → DELAY is immediate in the editor
      m_compiler.compile(m_editor->toPlainText().toStdString(), m_vm);
    }
    return true;
  } catch (const std::exception &e) {
    QMessageBox::warning(this, "Compile", e.what());
    m_output->appendPlainText(QStringLiteral("[FAIL] %1").arg(e.what()));
    return false;
  }
}

void MainWindow::setDebugUiActive(bool active) {
  m_debugging = active;
  m_editor->setReadOnly(active);
  m_debugStatus->setText(active ? "Debugging — F10 step · F5 continue · Shift+F5 stop"
                                : "Debug idle — F5 to start, click gutter for breakpoints");
  m_debugStatus->setProperty("active", active);
  m_debugStatus->style()->unpolish(m_debugStatus);
  m_debugStatus->style()->polish(m_debugStatus);
}

void MainWindow::refreshDebugViews() {
  m_stackView->clear();
  for (int i = static_cast<int>(m_vm.data_stack.size()) - 1; i >= 0; --i) {
    m_stackView->addItem(QStringLiteral("[%1] %2").arg(i).arg(
      valueToDisplay(m_vm.data_stack[static_cast<size_t>(i)])));
  }

  m_varsView->clear();
  for (const auto &[k, v] : m_vm.variables)
    m_varsView->addItem(
      QStringLiteral("%1 = %2").arg(QString::fromStdString(k), valueToDisplay(v)));

  m_invView->clear();
  for (const auto &[k, c] : m_vm.inventory)
    m_invView->addItem(QStringLiteral("%1 × %2").arg(QString::fromStdString(k)).arg(c));

  m_choicesView->clear();
  for (const auto &ch : m_vm.pending_choices)
    m_choicesView->addItem(QString::fromStdString(ch));

  const int line = m_vm.current_source_line();
  m_editor->setCurrentDebugLine(line);

  QString st = QStringLiteral("ip=%1 line=%2 scene=%3")
                 .arg(m_vm.ip)
                 .arg(line)
                 .arg(QString::fromStdString(m_vm.current_scene));
  if (m_vm.is_waiting_choice)
    st += " | WAIT_CHOICE";
  if (m_vm.outcome == GameOutcome::Victory)
    st += " | VICTORY";
  if (m_vm.outcome == GameOutcome::Defeat)
    st += " | DEFEAT";
  m_status->setText(st);
}

void MainWindow::runUntilBreakOrWait() {
  try {
    while (m_vm.ip < m_vm.code.size() && m_vm.outcome == GameOutcome::InProgress &&
           !m_vm.is_waiting_choice) {
      const int lineBefore = m_vm.current_source_line();
      if (!m_vm.step_once())
        break;

      const int lineAfter = m_vm.current_source_line();
      if (lineAfter > 0 && m_editor->hasBreakpoint(lineAfter) && lineAfter != lineBefore)
        break;
      if (m_vm.is_waiting_choice || m_vm.outcome != GameOutcome::InProgress)
        break;
    }
  } catch (const std::exception &e) {
    m_output->appendPlainText(QStringLiteral("[runtime] %1").arg(e.what()));
    QMessageBox::critical(this, "Runtime", e.what());
  }
  refreshDebugViews();
}

void MainWindow::debugStart() {
  m_output->clear();
  if (!prepareSession(true))
    return;
  setDebugUiActive(true);
  // Stop at first breakpoint, or stay at start
  const auto bps = m_editor->breakpoints();
  if (!bps.isEmpty()) {
    runUntilBreakOrWait();
  } else {
    refreshDebugViews();
    m_output->appendPlainText("[debug] Session started. F10 step, F5 continue.");
  }
}

void MainWindow::debugContinue() {
  if (!m_debugging) {
    debugStart();
    return;
  }
  if (m_vm.is_waiting_choice) {
    m_status->setText("Waiting for choice — double-click in Choices");
    return;
  }
  // Step once to leave the current breakpoint, then run to the next
  try {
    m_vm.step_once();
  } catch (const std::exception &e) {
    m_output->appendPlainText(QStringLiteral("[runtime] %1").arg(e.what()));
  }
  runUntilBreakOrWait();
}

void MainWindow::debugStep() {
  if (!m_debugging) {
    if (!prepareSession(true))
      return;
    setDebugUiActive(true);
  }
  if (m_vm.is_waiting_choice) {
    m_status->setText("Waiting for choice — double-click in Choices");
    refreshDebugViews();
    return;
  }
  try {
    // Step at source-line granularity
    const int startLine = m_vm.current_source_line();
    do {
      if (!m_vm.step_once())
        break;
      if (m_vm.is_waiting_choice || m_vm.outcome != GameOutcome::InProgress)
        break;
    } while (m_vm.current_source_line() == startLine && startLine != 0);
  } catch (const std::exception &e) {
    m_output->appendPlainText(QStringLiteral("[runtime] %1").arg(e.what()));
    QMessageBox::critical(this, "Runtime", e.what());
  }
  refreshDebugViews();
}

void MainWindow::debugStop() {
  m_vm = VM{};
  setDebugUiActive(false);
  m_editor->setCurrentDebugLine(0);
  m_stackView->clear();
  m_varsView->clear();
  m_invView->clear();
  m_choicesView->clear();
  m_status->setText("Debug stopped");
}

void MainWindow::makeDebugChoice(int index) {
  if (!m_debugging || !m_vm.is_waiting_choice)
    return;
  if (index < 0 || index >= static_cast<int>(m_vm.pending_choices.size()))
    return;
  m_vm.push(static_cast<Number>(index + 1));
  m_vm.is_waiting_choice = false;
  m_vm.pending_choices.clear();
  runUntilBreakOrWait();
}
