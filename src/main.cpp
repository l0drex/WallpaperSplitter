#include <QApplication>
#include <QCommandLineParser>
#include <iostream>
#include <QDebug>
#include "wallpapersplitter.h"

QPoint stringToPoint(const QString& string) {
    if (string.isNull() || string.isEmpty()) {
        return {0, 0};
    }

    auto split = string.split(',', Qt::SkipEmptyParts);
    if (split.size() != 2) {
        qFatal("Point has too few numbers!");
    }
    bool ok;
    const QPoint result = {split[0].toInt(&ok), split[1].toInt(&ok)};
    if (!ok) {
        qFatal("Point includes things that are no numbers!");
    }

    return result;
}

int main(int argc, char *argv[]) {
    // parts of this source code where taken from here: https://doc.qt.io/qt-5/qapplication.html
    QApplication app(argc, argv);
    QApplication::setApplicationName("Wallpaper Splitter");

    if (argc <= 1) {
        // start GUI
        WallpaperSplitter splitter;
        splitter.show();

        return QApplication::exec();
    } else {
        // command line version
        QCommandLineParser parser;
        parser.setApplicationDescription(
                QCoreApplication::translate("main","Splits your favorite wallpaper image so you can apply it across all of your screens."));
        parser.addHelpOption();
        parser.addPositionalArgument("input",
                                     QCoreApplication::translate("main", "Image to split."));

        parser.addOptions({
            {{"d", "destination"},
             QCoreApplication::translate("commandline", "Save the resulting images into <directory>."),
             QCoreApplication::translate("commandline", "destination")},
            {{"p", "top-left"},
             QCoreApplication::translate("commandline", "Position of the top-left corner of the screen rectangle in the picture."),
             QCoreApplication::translate("commandline", "top-left")},
            {{"s", "bottom-right"},
             QCoreApplication::translate("commandline", "Position of the bottom-right corner of the screen rectangle in the picture."),
             QCoreApplication::translate("commandline", "bottom-right")}
        });

        parser.process(app);
        const QStringList args = parser.positionalArguments();

        QStringList filePaths;
        QFileInfo imageFile = args.at(0);
        QString path = parser.value("destination");
        QPoint topLeft = stringToPoint(parser.value("top-left"));
        QPoint bottomRight = stringToPoint(parser.value("bottom-right"));
        if (!path.isEmpty()) {
            filePaths = WallpaperSplitter::splitImage(imageFile, path, topLeft, bottomRight);
        } else {
            filePaths = WallpaperSplitter::splitImage(imageFile, topLeft, bottomRight);
        }

        std::for_each(filePaths.begin(), filePaths.end(), [&](const QString& item){
            std::cout << item.toStdString() << std::endl;
        });
    }
}
