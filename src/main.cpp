#include <QApplication>
#include "wallpapersplitter.h"

int main(int argc, char *argv[]) {
    // TODO add support for non-gui version
    QApplication a(argc, argv);

    WallpaperSplitter splitter;
    splitter.show();

    return QApplication::exec();
}
