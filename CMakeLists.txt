cmake_minimum_required(VERSION 4.0)
project(OpenView)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/home/liugantang/Qt/6.9.1/gcc_64/lib/cmake/Qt6")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(OpenView src/main.cpp
        src/ui/MainWindow.cpp
        src/ui/MainWindow.h
        src/ui/MainWindow.ui
        src/ActionManager.cpp
        src/ActionManager.h)
target_link_libraries(OpenView
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

