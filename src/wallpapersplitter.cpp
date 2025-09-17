//
// Created by l0drex on 15.09.21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_WallpaperSplitter.h" resolved

#include <QFileDialog>
#include <QStandardPaths>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include <QDBusMessage>
#include <QDBusConnection>
#include <QPushButton>
#include "wallpapersplitter.h"
#include "ui_wallpapersplitter.h"
#include "screensitem.h"
#include "graphicsview.h"


WallpaperSplitter::WallpaperSplitter(QWidget *parent) :
        QDialog(parent), ui(new Ui::WallpaperSplitter) {
    ui->setupUi(this);

    // replace the standard graphics view with my subclass
    auto graphicsView = new GraphicsView(this);
    delete ui->verticalLayout->replaceWidget(ui->graphicsView, graphicsView)->widget();
    ui->graphicsView = graphicsView;

    auto scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    auto text = ui->graphicsView->scene()->addText(tr("Drop an image here"));
    // make sure its fixed size
    text->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    // center it
    auto rect = text->boundingRect();
    text->setTransformOriginPoint(rect.center());
    text->setPos(-rect.width() / 2.0, -rect.height() / 2.0);
    ui->graphicsView->centerOn(text);
    imageFile = new QFileInfo();

    connect(ui->buttonBoxOpen, &QDialogButtonBox::accepted,
            this, &WallpaperSplitter::selectImage);
    connect(ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok), &QPushButton::pressed,
            this, &WallpaperSplitter::applyWallpaper);
    connect(ui->buttonBox->button(QDialogButtonBox::StandardButton::Save), &QPushButton::pressed,
            this, &WallpaperSplitter::saveWallpapers);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

/**
 * Opens a dialog that asks the user to select an image.
 *
 * Then sets the file info and image attribute, displays the image and calls addScreens().
 * The image will be scaled to fit on all screens, also the graphics view will be scaled to show the whole image.
 */
void WallpaperSplitter::selectImage() {
    const auto url = QFileDialog::getOpenFileUrl(
            this,
            tr("Select a wallpaper image"),
            "file://" + QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            QString("Images (*.jpg *.png *.bmp)")
    );
    addImage(url);
}

/**
 * Splits the selected image and returns a list to all paths where the images were saved.
 */
QStringList WallpaperSplitter::splitImage(const QImage &image, const QList<QRect> &screens, const QString &path) {
    if (screens.isEmpty()) {
        qFatal("No area to cut out provided!");
    }
    if (image.isNull() || image.sizeInBytes() < 0) {
        qFatal("Image could not be loaded");
    }

    QImage wallpaper;
    QString fileName;
    QStringList paths{};
    QDir().mkdir(path);
    int index = 0;

    std::for_each(screens.begin(), screens.end(), [&](const QRect screen){
        if (!image.rect().contains(screen.topLeft())) {
            qWarning("Image does not contain the top left corner of the provided rectangle!");
        }
        if (!image.rect().contains(screen.bottomRight())) {
            qWarning("Image does not contain the bottom right position of the provided rectangle!");
        }

        // copy a rectangle with size and position of the screen
        wallpaper = image.copy(screen);

        // images are saved as 0.png 1.png etc
        fileName = path + '/' + QString::number(index) + ".png";
        paths.append(fileName);

        // if this returns false, the save failed and the assertion fails
        bool success = wallpaper.save(fileName);
        assert(success);
        index++;
    });

    return paths;
}

QStringList WallpaperSplitter::splitImage(const QImage &image, const QString &path, const QPoint topLeft, const QPoint bottomRight) {
    QList<QRect> screenGeometries{};
    const auto screens = QApplication::screens();
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* screen){
        // set top-left corner
        QRect geometry = screen->geometry();
        QPoint delta = screen->geometry().topLeft() - screens.first()->geometry().topLeft();
        geometry.moveTopLeft(topLeft + delta);
        // set bottom-right to desired position, if possible
        if (bottomRight.manhattanLength() > 0) {
            geometry.setSize(geometry.size().scaled(bottomRight.x(), bottomRight.y(), Qt::KeepAspectRatio));
        }
        screenGeometries.append(geometry);
    });

    return splitImage(image, screenGeometries, path);
}

QStringList WallpaperSplitter::splitImage() {
    setCursor(Qt::WaitCursor);

    QList<QRect> screens = {};
    const auto screenItems = screenGroup->getRectangles();
    std::for_each(screenItems.begin(), screenItems.end(), [&](const QGraphicsRectItem *screen){
        screens.append(screenGroup->sceneTransform().mapRect(screen->rect().toRect()));
    });

    QString path;
    if (imageFile->isFile()) {
        path = QFileDialog::getExistingDirectory(
                this, "",
                imageFile->absolutePath(), QFileDialog::ShowDirsOnly);
    } else {
        path = QFileDialog::getExistingDirectory(
                this, "",
                QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)[0], QFileDialog::ShowDirsOnly);
    }

    unsetCursor();
    return WallpaperSplitter::splitImage(*wallpaper, screens, path);
}

/**
 * Applies the selected image to all screens in the current activity.
 */
void WallpaperSplitter::applyWallpaper() {
    auto paths = splitImage();
    assert(!paths.isEmpty());

    // apply the wallpapers via a dbus call
    QString script;
    QTextStream out(&script);
    // language=JavaScript
    out << "var paths = ['" + paths.join("', '") + "'];"
        << "var desktops = desktopsForActivity(currentActivity());"
        << "for(let i in desktops) {"
        << "    let d = desktops[i];"
        << "    d.wallpaperPlugin = 'org.kde.image';"
        << "    d.currentConfigGroup = ['Wallpaper', 'org.kde.image', 'General'];"
        << "    d.writeConfig('Image', paths[i]);"
        << "}";
    auto message = QDBusMessage::createMethodCall(
            "org.kde.plasmashell",
            "/PlasmaShell", "org.kde.PlasmaShell",
            "evaluateScript");
    message.setArguments(QVariantList() << QVariant(script));
    qDebug() << "Applying image" << paths.join(", ");
    auto reply = QDBusConnection::sessionBus().call(message);
    if(reply.type() == QDBusMessage::ErrorMessage) {
        qCritical() << "Something went wrong.";
        qCritical() << reply.errorMessage();
    }

    QApplication::quit();
}

/**
 * Splits the image and saves the resulting wallpapers in a subdirectory
 */
void WallpaperSplitter::saveWallpapers() {
    splitImage();
}

/**
 * Calculates the total size of all screens combined.
 */
QSize WallpaperSplitter::totalScreenSize() {
    auto screens = QApplication::screens();
    // get combined height and width of all screens
    auto *screensRect = new QGraphicsItemGroup();
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* item){
        screensRect->addToGroup(new QGraphicsRectItem(item->geometry()));
    });

    // subtract the width of the stroke
    return screensRect->sceneBoundingRect().size().toSize() - QSize(1, 1);
}

void WallpaperSplitter::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    scaleView();
}

void WallpaperSplitter::scaleView() {
    ui->graphicsView->fitInView(ui->graphicsView->scene()->itemsBoundingRect(), Qt::AspectRatioMode::KeepAspectRatio);
}

WallpaperSplitter::~WallpaperSplitter() {
    delete ui;
}

void WallpaperSplitter::addImage(QImage &image) {
    ui->graphicsView->scene()->clear();
    wallpaper = new QImage(image);
    auto imageItem = ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(image));
    imageItem->setFlag(QGraphicsItem::ItemContainsChildrenInShape);
    screenGroup = new ScreensItem(imageItem);

    // scale the desktops so that the image fits
    const auto screenSize = totalScreenSize();
    if(image.width() < screenSize.width() || image.height() < screenSize.height()) {
        // this is hacky and not performant at all
        auto imageScaled = image.scaled(screenSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        qreal scale = (float) imageScaled.width() / (float) image.width();
        screenGroup->setScale(1 / scale);
        screenGroup->setPos(imageItem->scenePos());
    }
    scaleView();
}

void WallpaperSplitter::addImage(const QUrl &url) {
    if(url.isEmpty()) return;

    imageFile = new QFileInfo(url.path());
    auto image = new QImage(imageFile->filePath());
    qDebug() << "Image" << imageFile->fileName() << "selected.";
    addImage(*image);
}
