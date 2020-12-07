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

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB UI_FILES "${PROJECT_SOURCE_DIR}/src/*.ui")

qt5_wrap_cpp(
  SOURCES
)

qt5_wrap_ui(
  "${UI_FILES}"
  SOURCES
)

add_executable(
  ${PROJECT_NAME}
  "${SOURCES}"
)

add_definitions(${Qt5Widgets_DEFINITONS})

target_include_directories(${PROJECT_NAME}
  PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_SOURCE_DIR}/src"
  "${PROJECT_BUILD_DIR}")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  OpenGL::GL
  
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
)
