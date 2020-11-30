cmake_minimum_required(VERSION 3.1)

project(tp)

set(CMAKE_BUILD_TYPE DEBUG)
# set(CMAKE_BUILD_TYPE RELEASE)

# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# set(CMAKE_AUTOUIC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
# find_package(Qt5 COMPONENTS OpenGL REQUIRED)
# find_package(GLU REQUIRED)

# find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 11)

file(GLOB INCLUDES include/*.h)

file(GLOB SOURCES src/*.cpp)

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  include/
)

qt5_wrap_cpp(
  SOURCES
)

add_executable(
    ${PROJECT_NAME}
  ${SOURCES}
)

# target_link_libraries(
#   ${CMAKE_PROJECT_NAME}
#   ${OpenCV_LIBS}
#   ${CMAKE_THREAD_LIBS_INIT}
# )


# qt5_use_modules(${PROJECT_NAME} Widgets OpenGL)
qt5_use_modules(${PROJECT_NAME} Widgets)

# https://bugreports.qt.io/browse/QTBUG-42999 HELPED !!
target_link_libraries(
  ${PROJECT_NAME} 
  Qt5::Core 
  Qt5::Gui
  # GLU  
  # ${Qt5Gui_EGL_LIBRARIES} 
  # ${Qt5Gui_OPENGL_LIBRARIES}
  )