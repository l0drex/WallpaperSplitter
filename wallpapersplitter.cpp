//
// Created by l0drex on 14.09.21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_WallpaperSplitter.h" resolved

#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QScreen>
#include <QGraphicsItemGroup>
#include <cmath>
#include "wallpapersplitter.h"
#include "ui_wallpapersplitter.h"
#include "offsetdialog.h"


WallpaperSplitter::WallpaperSplitter(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::WallpaperSplitter) {
    ui->setupUi(this);
    fileInfo = new QFileInfo();
    paths = new QStringList();
    offset = new QSize(0, 0);
    image = new QImage();

    QObject::connect(ui->imageButton, &QPushButton::clicked, this, &WallpaperSplitter::select_image);
    QObject::connect(ui->offsetButton, &QPushButton::clicked, this, &WallpaperSplitter::change_offset);
    QObject::connect(ui->splitButton, &QPushButton::clicked, this, &WallpaperSplitter::split_image);
    QObject::connect(ui->applyButton, &QPushButton::clicked, this, &WallpaperSplitter::apply_wallpapers);
}

void WallpaperSplitter::select_image() {
    const auto url = QFileDialog::getOpenFileUrl(
            this,
            "Select a wallpaper image",
            "file://" + QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            QString("image")
    );
    // if process was cancelled
    if(url.isEmpty()) QApplication::quit();
    fileInfo->setFile(url.path());
    image = new QImage(fileInfo->filePath());

    // scale the image so that it fits
    const QSize screenSize = getCombinedScreenSize();
    // TODO improvement should be possible
    if(image->width() < screenSize.width()){
        qDebug() << "Image is too small and has to be scaled";
        *image = image->scaledToWidth(screenSize.width(), Qt::TransformationMode::SmoothTransformation);
    }
    if(image->height() < screenSize.height()) {
        qDebug() << "Image is too small and has to be scaled";
        *image = image->scaledToHeight(screenSize.height(), Qt::TransformationMode::SmoothTransformation);
    }

    qDebug() << "Image" << fileInfo->fileName() << "selected.";
}

void WallpaperSplitter::change_offset() {
    const QSize screenSize = getCombinedScreenSize();

    if(image->height() > screenSize.height() || image->width() > screenSize.width()) {
        *offset = OffsetDialog::showOffsetDialog(this, *image);
    }
    qDebug() << offset->width() << offset->height();
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
    if(image -> isNull() || image -> sizeInBytes() < 0) {
        qDebug() << "Image is empty!";
        return;
    }

    QImage wallpaper;
    const QString directory = fileInfo->absolutePath() + '/' + fileInfo->baseName() + "_split";
    QDir().mkdir(directory);
    QRect geometry;
    QString fileName;

    std::for_each(screens.begin(), screens.end(), [&](const QScreen *screen){
        // copy a rectangle with size and position of the screen
        geometry = screen -> geometry();
        // add offset
        geometry.setX(geometry.x() + offset -> width());
        geometry.setY(geometry.y() + offset -> height());
        wallpaper = image -> copy(geometry);

        // images are saved in a subdirectory called split with a number as suffix
        fileName = directory + '/' + screen->name() + '.' + fileInfo->suffix();
        paths->append(fileName);

        // if this returns false, the save failed and the assertion fails
        bool success = wallpaper.save(fileName);
        assert(success);
    });
}

void WallpaperSplitter::apply_wallpapers() {
    // TODO
}

QSize WallpaperSplitter::getCombinedScreenSize() {
    // get combined height and width of all screens
    auto *screensRect = new QGraphicsItemGroup();
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* item){
        screensRect->addToGroup(new QGraphicsRectItem(item->geometry()));
    });
    int height = std::ceil(screensRect->boundingRect().height());
    int width  = std::ceil(screensRect->boundingRect().width());

    return {width, height};
}

WallpaperSplitter::~WallpaperSplitter() {
    delete ui;
}
