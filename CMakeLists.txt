cmake_minimum_required(VERSION 3.20)
project(wallpaper_splitter)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
find_package(KF5ConfigWidgets)

add_executable(wallpaper_splitter main.cpp offsetdialog.cpp offsetdialog.h offsetdialog.ui wallpapersplitter.cpp wallpapersplitter.h wallpapersplitter.ui)
target_link_libraries(wallpaper_splitter
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt::DBus
        KF5::ConfigWidgets
        )
