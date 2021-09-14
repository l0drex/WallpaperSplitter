//
// Created by l0drex on 14.09.21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_WallpaperSplitter.h" resolved

#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QScreen>
#include "wallpapersplitter.h"
#include "ui_wallpapersplitter.h"
#include "offsetdialog.h"


WallpaperSplitter::WallpaperSplitter(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::WallpaperSplitter) {
    ui->setupUi(this);
    screens = QApplication::screens();
    fileInfo = new QFileInfo();
    paths = new QStringList();
    QObject::connect(ui->imageButton, &QPushButton::clicked, this, &WallpaperSplitter::select_image);
    QObject::connect(ui->splitButton, &QPushButton::clicked, this, &WallpaperSplitter::split_image);
    QObject::connect(ui->applyButton, &QPushButton::clicked, this, &WallpaperSplitter::apply_wallpapers);
}

void WallpaperSplitter::select_image() {
    auto url = QFileDialog::getOpenFileUrl(
            nullptr,
            "Select a wallpaper image",
            "file://" + QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            QString("image")
    );
    // if process was cancelled
    if(url.isEmpty()) QApplication::quit();
    fileInfo->setFile(url.path());
}

QSize WallpaperSplitter::change_offset(QImage &image) {
    QSize offset = QSize(0, 0);
    // NOTE maybe this has to be calculated over every screen
    auto last_screen_geometry = screens.last() -> geometry();
    int width = last_screen_geometry.x() + last_screen_geometry.width();
    int height = last_screen_geometry.y() + last_screen_geometry.height();
    if(image.height() > height || image.width() > width) {
        offset = OffsetDialog::getOffset(image, screens);
    } else {
        if(image.height() < height || image.width() < width) {
            // TODO scale the image
            qDebug() << "Image is too small and has to be scaled";
        }
    }

    return offset;
}

void WallpaperSplitter::split_image() {
/**
     * Split the image so that it spans across all screens when applied to their workspace.
     *
     * It uses the current screen setup and their configured geometry.
     *
     * @param image: The image that should be split.
     * @param fileInfo: The path to the image that should be split.
     */

    // FIXME there might be a better way. Qt does not use exceptions
    if(!fileInfo->isFile()) {
        qDebug() << "File does not exist!";
        return;
    }
    auto image = new QImage(fileInfo->filePath());
    if(image -> isNull() || image -> sizeInBytes() < 0) {
        qDebug() << "Image is empty!";
        return;
    }

    // ask user about offset, if needed
    auto offset = change_offset(*image);

    QImage wallpaper;
    QString directory = fileInfo->absolutePath() + '/' + fileInfo->baseName() + "_split";
    // QDir().mkdir(directory);
    // list of the paths to return

    // FIXME this is to prevent endless savings while debugging / coding
    return;

    std::for_each(screens.begin(), screens.end(), [&](const QScreen *screen){
        // copy a rectangle with size and position of the screen
        auto geometry = screen -> geometry();
        // add offset
        geometry.setX(geometry.x() + offset.width());
        geometry.setY(geometry.y() + offset.height());
        wallpaper = image -> copy(geometry);

        // images are saved in a subdirectory called split with a number as suffix
        auto fileName = directory + '/' + screen->name() + '.' + fileInfo->suffix();
        paths->append(fileName);

        // if this returns false, the save failed and the assertion fails
        bool success = wallpaper.save(fileName);
        assert(success);
    });
}

void WallpaperSplitter::apply_wallpapers() {
    // TODO
}

WallpaperSplitter::~WallpaperSplitter() {
    delete ui;
}
