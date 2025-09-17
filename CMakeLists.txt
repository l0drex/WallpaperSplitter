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

install(TARGETS wallpaper_splitter RUNTIME DESTINATION bin)

# Install metadata
install(FILES assets/icon.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
    RENAME io.github.l0drex.wallpaper_splitter.svg)

install(FILES assets/metainfo.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

install(FILES assets/wallpaper_splitter.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    RENAME io.github.l0drex.wallpaper_splitter.desktop)
