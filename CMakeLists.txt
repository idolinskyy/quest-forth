cmake_minimum_required(VERSION 3.16)

project(
  game_quest
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

add_library(quest_engine STATIC ${ENGINE_SOURCES})
target_include_directories(quest_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Engine unit tests (no Qt) ---
enable_testing()

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4)
FetchContent_MakeAvailable(Catch2)

add_executable(test_engine tests/test_engine.cpp)
target_link_libraries(test_engine PRIVATE quest_engine Catch2::Catch2WithMain)
add_test(NAME engine COMMAND test_engine)

add_executable(validate_quests tools/validate_quests.cpp)
target_link_libraries(validate_quests PRIVATE quest_engine)

# --- Qt GUI apps (optional if Qt6 is installed) ---
find_package(Qt6 6.2 COMPONENTS Quick Widgets QUIET)
if(Qt6_FOUND)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)

  # --- Quest player (Quick) ---
  if(TARGET Qt6::Quick)
    qt_add_executable(appgame_quest main.cpp GameController.cpp)
    target_link_libraries(appgame_quest PRIVATE quest_engine Qt6::Quick)
    target_compile_definitions(appgame_quest PRIVATE GAME_QUESTS_DIR="${CMAKE_SOURCE_DIR}/quests")

    add_custom_command(
      TARGET appgame_quest
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/quests"
              "$<TARGET_FILE_DIR:appgame_quest>/quests")

    qt_add_qml_module(
      appgame_quest
      URI
      game_quest
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
      appgame_quest
      PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
                 MACOSX_BUNDLE_SHORT_VERSION_STRING
                 ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
                 MACOSX_BUNDLE TRUE
                 WIN32_EXECUTABLE TRUE)

    include(GNUInstallDirs)
    install(
      TARGETS appgame_quest
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
    qt_add_executable(quest_editor ${EDITOR_SOURCES})
    target_link_libraries(quest_editor PRIVATE quest_engine Qt6::Widgets)
    target_include_directories(quest_editor PRIVATE ${CMAKE_SOURCE_DIR}/editor)
    set_target_properties(quest_editor PROPERTIES WIN32_EXECUTABLE TRUE)
  endif()
else()
  message(WARNING "Qt6 not found — building engine tests only")
endif()
