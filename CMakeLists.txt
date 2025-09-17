cmake_minimum_required(VERSION 3.20)
project(wallpaper_splitter VERSION 1.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
        Core
        Gui
        Widgets)

qt_standard_project_setup()

find_package(KF6ConfigWidgets)

qt_add_executable(wallpaper_splitter 
        src/main.cpp 
        src/wallpapersplitter.cpp 
        src/wallpapersplitter.ui 
        src/screensitem.cpp 
        src/graphicsview.cpp 
)

target_link_libraries(wallpaper_splitter PRIVATE
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
