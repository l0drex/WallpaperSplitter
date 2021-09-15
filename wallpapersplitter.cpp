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
#include <QDBusMessage>
#include <QDBusConnection>


WallpaperSplitter::WallpaperSplitter(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::WallpaperSplitter) {
    ui->setupUi(this);
    fileInfo = new QFileInfo();
    paths = new QStringList();
    offset = new QPoint(0, 0);
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
    ui->offsetButton->setEnabled(true);
    qDebug() << "Image" << fileInfo->fileName() << "selected.";
}

void WallpaperSplitter::change_offset() {
    const QSize screenSize = getCombinedScreenSize();

    if(image->height() > screenSize.height() || image->width() > screenSize.width()) {
        *offset = OffsetDialog::showOffsetDialog(this, *image);
    }
    ui->splitButton->setEnabled(true);
    qDebug() << offset->x() << offset->y();
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
        geometry.setX(geometry.x() + offset -> x());
        geometry.setY(geometry.y() + offset -> y());
        geometry.setWidth(geometry.width() + offset -> x());
        geometry.setHeight(geometry.height() + offset -> y());
        wallpaper = image -> copy(geometry);

        // images are saved in a subdirectory called split with a number as suffix
        fileName = directory + '/' + screen->name() + '.' + fileInfo->suffix();
        paths->append(fileName);

        // if this returns false, the save failed and the assertion fails
        bool success = wallpaper.save(fileName);
        assert(success);
    });

    ui->applyButton->setEnabled(true);
}

void WallpaperSplitter::apply_wallpapers() {
    // TODO
    QString script;
    QTextStream out(&script);
    assert(!paths->isEmpty());
    qDebug() << paths->join("', '");
    // FIXME this script is not reliable
    // language=JavaScript
    out << "var paths = ['" + paths->join("', '") + "'];"
        << "var path_iterator = 0;"
        << "var activity = currentActivity();"
        << "for(var key in desktopsForActivity(activity)) {"
        << "    var d = desktopsForActivity(activity)[key];"
        << "    d.wallpaperPlugin = 'org.kde.image';"
        << "    d.currentConfigGroup = ['Wallpaper', 'org.kde.image', 'General'];"
        << "    if(key > 0) {"
        << "        d.writeConfig('Image', paths[path_iterator]);"
        << "        path_iterator++;"
        << "}}";
    auto message = QDBusMessage::createMethodCall(
            "org.kde.plasmashell",
            "/PlasmaShell", "org.kde.PlasmaShell",
            "evaluateScript");
    message.setArguments(QVariantList() << QVariant(script));
    auto reply = QDBusConnection::sessionBus().call(message);
    if(reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "Something went wrong.";
        qDebug() << reply.errorMessage();
    }
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
