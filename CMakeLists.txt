cmake_minimum_required(VERSION 3.20)
project(wallpaper_splitter)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

find_package(KF6ConfigWidgets)

add_executable(wallpaper_splitter 
        src/main.cpp 
        src/wallpapersplitter.cpp 
        src/wallpapersplitter.h 
        src/wallpapersplitter.ui 
        src/screensitem.cpp 
        src/screensitem.h 
        src/graphicsview.cpp 
        src/graphicsview.h
)

target_link_libraries(wallpaper_splitter
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt::DBus
        KF6::ConfigWidgets
)
