#include <QApplication>
#include "wallpapersplitter.h"

QCoreApplication* createApplication(int &argc, char *argv[]) {
    if (argc <= 1) {
        return new QApplication(argc, argv);
    }
    return new QCoreApplication(argc, argv);
}

int main(int argc, char *argv[]) {
    // parts of this source code where taken from here: https://doc.qt.io/qt-5/qapplication.html
    QScopedPointer<QCoreApplication> app(createApplication(argc, argv));

    if (qobject_cast<QApplication *>(app.data())) {
        // start GUI
        WallpaperSplitter splitter;
        splitter.show();

        return QApplication::exec();
    } else {
        // TODO parse arguments and split according to them
    }
}
