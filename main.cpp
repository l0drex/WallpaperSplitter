#include <QApplication>
#include <QDebug>
#include <QScreen>
#include <QSize>
#include <QImage>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QStandardPaths>
#include <QException>
#include <utility>
#include "offsetdialog.h"
#include "wallpapersplitter.h"

int main(int argc, char *argv[]) {
    // TODO add support for non-gui version
    QApplication a(argc, argv);

    WallpaperSplitter splitter;
    splitter.show();

    return QApplication::exec();
}
