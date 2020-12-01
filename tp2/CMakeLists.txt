cmake_minimum_required(VERSION 3.1)

project(tp LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE DEBUG)
# set(CMAKE_BUILD_TYPE RELEASE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS OpenGL REQUIRED)
find_package(OpenGL REQUIRED)


# Copie le dossier data dans le dossier de build 
file(COPY "data" DESTINATION ${PROJECT_BINARY_DIR})

file(GLOB SOURCES src/*.cpp)

qt5_wrap_cpp(
  SOURCES
)

add_executable(
  ${PROJECT_NAME}
  ${SOURCES}
)

#qt5_use_modules(${PROJECT_NAME} Widgets OpenGL)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  "${PROJECT_SOURCE_DIR}/include")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
  Qt5::Core
  Qt5::Gui
  OpenGL::GL
)

