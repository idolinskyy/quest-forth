cmake_minimum_required(VERSION 3.16)

project(
  QuestForth
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ENGINE_SOURCES
    vm.cpp
    value.cpp
    words.cpp
    compiler.cpp)

add_library(questforth_engine STATIC ${ENGINE_SOURCES})
target_include_directories(questforth_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Engine unit tests (no Qt) ---
enable_testing()

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4)
FetchContent_MakeAvailable(Catch2)

add_executable(test_engine tests/test_engine.cpp)
target_link_libraries(test_engine PRIVATE questforth_engine Catch2::Catch2WithMain)
add_test(NAME engine COMMAND test_engine)

add_executable(validate_quests tools/validate_quests.cpp)
target_link_libraries(validate_quests PRIVATE questforth_engine)

# --- Qt GUI apps (optional if Qt6 is installed) ---
find_package(Qt6 6.2 COMPONENTS Quick Widgets QUIET)
if(Qt6_FOUND)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  # Keep QML module trees under build/qml/ so they do not clash with binary names.
  set(QT_QML_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml")

  # --- Quest player (Quick) ---
  if(TARGET Qt6::Quick)
    qt_add_executable(questforth_app main.cpp GameController.cpp)
    target_link_libraries(questforth_app PRIVATE questforth_engine Qt6::Quick)
    target_compile_definitions(questforth_app
                               PRIVATE QUESTFORTH_QUESTS_DIR="${CMAKE_SOURCE_DIR}/quests")
    set_target_properties(questforth_app PROPERTIES OUTPUT_NAME QuestForth)

    add_custom_command(
      TARGET questforth_app
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/quests"
              "$<TARGET_FILE_DIR:questforth_app>/quests")

    qt_add_qml_module(
      questforth_app
      URI
      QuestForth
      VERSION
      1.0
      QML_FILES
      main.qml
      SOURCES
      vm.h
      words.h
      compiler.h
      init.h
      GameController.h)

    set_target_properties(
      questforth_app
      PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
                 MACOSX_BUNDLE_SHORT_VERSION_STRING
                 ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
                 MACOSX_BUNDLE TRUE
                 WIN32_EXECUTABLE TRUE)

    include(GNUInstallDirs)
    install(
      TARGETS questforth_app
      BUNDLE DESTINATION .
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  endif()

  # --- Quest editor (Widgets) ---
  if(TARGET Qt6::Widgets)
    set(EDITOR_SOURCES
        editor/main.cpp
        editor/MainWindow.cpp
        editor/CodeEditor.cpp
        editor/ForthHighlighter.cpp
        editor/WordDictionary.cpp
        editor/MainWindow.h
        editor/CodeEditor.h
        editor/ForthHighlighter.h
        editor/WordDictionary.h)
    qt_add_executable(questforth_editor ${EDITOR_SOURCES})
    target_link_libraries(questforth_editor PRIVATE questforth_engine Qt6::Widgets)
    target_include_directories(questforth_editor PRIVATE ${CMAKE_SOURCE_DIR}/editor)
    set_target_properties(
      questforth_editor PROPERTIES OUTPUT_NAME QuestForthEditor WIN32_EXECUTABLE TRUE)
  endif()
else()
  message(WARNING "Qt6 not found — building engine tests only")
endif()
