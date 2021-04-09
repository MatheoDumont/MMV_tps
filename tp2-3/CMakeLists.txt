cmake_minimum_required(VERSION 3.13)

if(MSVC)
  add_compile_options(/W3 /WX)
else()
  add_compile_options(-W -Wall -Wextra -pedantic)
endif()

project(tp LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Copie le dossier data dans le dossier de build
file(COPY "data" DESTINATION ${PROJECT_BINARY_DIR})
file(COPY "shaders" DESTINATION ${PROJECT_BINARY_DIR})

set(SOURCES
  "main.cpp"
  "vec.cpp"
  "box2d.cpp"
  "grid.cpp"
  "scalar_field.cpp"
  "height_field.cpp"
  "camera.cpp"
  "road.cpp"
  )
list(TRANSFORM SOURCES PREPEND "src/")

set(QT_SOURCES
  "mainwindow.cpp"
  "bounddialog.cpp"
  "displayglwidget.cpp"
  "labelimage.cpp"
  )
list(TRANSFORM QT_SOURCES PREPEND "qtfiles/")

set(UI_FILES
  "mainwindow.ui"
  "bounddialog.ui"
  )
list(TRANSFORM UI_FILES PREPEND "qtfiles/")

# https://cmake.org/cmake/help/latest/manual/cmake-qt.7.html#autouic
# l'ordre est important :
# il faut preciser les sources en premiers
# et CMAKE_AUTOUIC ira generer les ui_*.h ne voyant les includes
qt5_wrap_cpp(
  "${QT_SOURCES}"
)
qt5_wrap_ui(
  "${UI_FILES}"
)

add_executable(
  ${PROJECT_NAME}
  "${SOURCES}"
  "${QT_SOURCES}"
)

add_definitions(${Qt5Widgets_DEFINITONS})

target_include_directories(${PROJECT_NAME}
  PUBLIC
  "${PROJECT_SOURCE_DIR}/include"

  PRIVATE
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_SOURCE_DIR}/qtfiles"
  "${PROJECT_BUILD_DIR}"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  OpenGL::GL
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
)
