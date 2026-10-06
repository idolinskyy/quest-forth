#pragma once

#include "compiler.h"
#include "vm.h"

#include <QMainWindow>
#include <memory>

class CodeEditor;
class QListWidget;
class QPlainTextEdit;
class QLabel;
class QComboBox;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  explicit MainWindow(QWidget *parent = nullptr);
  void openPath(const QString &path);

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void newFile();
  void openFile();
  void saveFile();
  void saveFileAs();
  void checkCompile();
  void debugStart();
  void debugContinue();
  void debugStep();
  void debugStop();
  void makeDebugChoice(int index);
  void markDirty();

private:
  void setupUi();
  void setupMenus();
  bool maybeSave();
  void setCurrentPath(const QString &path);
  void refreshDebugViews();
  void setDebugUiActive(bool active);
  bool prepareSession(bool resetVm);
  void runUntilBreakOrWait();
  QString valueToDisplay(const Value &v) const;

  CodeEditor *m_editor = nullptr;
  QPlainTextEdit *m_output = nullptr;
  QListWidget *m_stackView = nullptr;
  QListWidget *m_varsView = nullptr;
  QListWidget *m_invView = nullptr;
  QListWidget *m_choicesView = nullptr;
  QLabel *m_status = nullptr;
  QLabel *m_debugStatus = nullptr;

  QString m_path;
  bool m_dirty = false;

  Compiler m_compiler;
  VM m_vm;
  bool m_debugging = false;
  bool m_running = false;
};
