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
#include <KColorScheme>
#include <cmath>
#include "wallpapersplitter.h"
#include "ui_wallpapersplitter.h"


WallpaperSplitter::WallpaperSplitter(QWidget *parent) :
        QDialog(parent), ui(new Ui::WallpaperSplitter) {
    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene());
    ui->graphicsView->scene()->addText(tr("Press 'open' to select an image"));
    image_file = new QFileInfo();
    image = new QImage();

    connect(ui->buttonBoxOpen, &QDialogButtonBox::accepted,
            this, &WallpaperSplitter::select_image);
    connect(ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok), &QPushButton::pressed,
            this, &WallpaperSplitter::apply_wallpaper);
    connect(ui->buttonBox->button(QDialogButtonBox::StandardButton::Save), &QPushButton::pressed,
            this, &WallpaperSplitter::save_wallpapers);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

WallpaperSplitter::~WallpaperSplitter() {
    delete ui;
}

void WallpaperSplitter::select_image() {
    /**
     * Opens a dialog that asks the user to select an image.
     *
     * Then sets the file info and image attribute, displays the image and calls add_screens().
     * The image will be scaled to fit on all screens, also the graphics view will be scaled to show the whole image.
     */
    const auto url = QFileDialog::getOpenFileUrl(
            this,
            "Select a wallpaper image",
            "file://" + QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            QString("image")
    );
    // if process was cancelled
    if(url.isEmpty()) return;

    image_file = new QFileInfo(url.path());
    image = new QImage(image_file->filePath());

    // scale the image so that it fits
    const auto screenSize = total_screen_size();
    // TODO improvement should be possible
    if(image->width() < screenSize.width()){
        qDebug() << "Image is too small and has to be scaled";
        *image = image->scaledToWidth(screenSize.width(), Qt::TransformationMode::SmoothTransformation);
    }
    if(image->height() < screenSize.height()) {
        qDebug() << "Image is too small and has to be scaled";
        *image = image->scaledToHeight(screenSize.height(), Qt::TransformationMode::SmoothTransformation);
    }

    ui->graphicsView->scene()->clear();
    auto image_item = ui->graphicsView->scene()->addPixmap(QPixmap::fromImage(*image));
    image_item->setFlag(QGraphicsItem::ItemClipsChildrenToShape);  // FIXME does not what I thought it does
    scaleView();
    add_screens();

    qDebug() << "Image" << image_file->fileName() << "selected.";
}

inline QPoint WallpaperSplitter::get_offset() {
    /**
     * Returns the offset of the screen rectangles
     */
    return screen_group->pos().toPoint();
}

void WallpaperSplitter::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    scaleView();
}

void WallpaperSplitter::scaleView() {
    ui->graphicsView->fitInView(image->rect(), Qt::AspectRatioMode::KeepAspectRatio);
}

QStringList WallpaperSplitter::split_image(QString &path) {
    /**
     * Splits the previously selected image and returns a list to all paths where the images were saved.
     */
    // FIXME there might be a better way. Qt does not use exceptions
    if(!image_file->isFile()) {
        qDebug() << "No existing file selected!";
        return {};
    }
    if(image -> isNull() || image -> sizeInBytes() < 0) {
        qDebug() << "Image was not loaded correctly!";
        QApplication::quit();
    }

    QImage wallpaper;
    QRect geometry;
    QString fileName;
    QStringList paths{};
    auto screens = QApplication::screens();
    auto offset = get_offset();
    QDir().mkdir(path);

    std::for_each(screens.begin(), screens.end(), [&](const QScreen *screen){
        // copy a rectangle with size and position of the screen
        geometry = screen -> geometry();
        // add offset
        geometry.setX(geometry.x() + offset.x());
        geometry.setY(geometry.y() + offset.y());
        geometry.setWidth(geometry.width() + offset.x());
        geometry.setHeight(geometry.height() + offset.y());
        wallpaper = image -> copy(geometry);

        // images are saved in a subdirectory called split with a number as suffix
        fileName = path + '/' + screen->name() + '.' + image_file->suffix();
        paths.append(fileName);

        // if this returns false, the save failed and the assertion fails
        bool success = wallpaper.save(fileName);
        assert(success);
    });

    return paths;
}

void WallpaperSplitter::apply_wallpaper() {
    /**
     * Applies the selected image to all screens in the current activity.
     */
    // use a temporary directory
    QString temp_path = QDir::tempPath();
    auto paths = split_image(temp_path);

    // apply the wallpapers via a dbus call
    QString script;
    QTextStream out(&script);
    assert(!paths.isEmpty());
    qDebug() << "Applying image" << paths.join(", ");
    // language=JavaScript
    out << "var paths = ['" + paths.join("', '") + "'];"
        << "var path_iterator = 0;"
        << "var activity = currentActivity();"
        << "for(var key in desktopsForActivity(activity)) {"
        << "    var d = desktopsForActivity(activity)[key];"
        << "    d.wallpaperPlugin = 'org.kde.image';"
        << "    d.currentConfigGroup = ['Wallpaper', 'org.kde.image', 'General'];"
        << "    d.writeConfig('Image', paths[path_iterator]);"
        << "    if(key > 0) {"
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
        QApplication::quit();
    }

    QApplication::quit();
}

void WallpaperSplitter::save_wallpapers() {
    /**
     * Splits the image and saves the resulting wallpapers in a subdirectory
     */
    QString path = image_file->absolutePath() + '/' + image_file->baseName() + "_split";
    split_image(path);
    qDebug() << "Image was split, pieces have been saved in" << path;
}

void WallpaperSplitter::add_screens() {
    /**
     * Adds the screen rectangles to the graphics view
     */
    auto screens = QApplication::screens();
    // get the currently used color scheme
    const auto colorScheme = KColorScheme();

    // draw a rectangle for every screen
    // all rectangles are in a group that is a child of the image, so that they clip to the borders
    const auto image_item = ui->graphicsView->scene()->items().first();
    screen_group = new QGraphicsItemGroup(image_item);
    const auto pen = QPen(colorScheme.foreground(KColorScheme::ForegroundRole::ActiveText), 2);
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* screen){
        const auto rect = ui->graphicsView->scene()->addRect(
                screen->geometry(), pen,
                colorScheme.background(KColorScheme::BackgroundRole::ActiveBackground));
        rect->setOpacity(0.5);
        screen_group -> addToGroup(rect);
    });

    // make the screen item movable by the user
    screen_group->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    screen_group->setFlag(QGraphicsItem::ItemIsMovable);
}

QSize WallpaperSplitter::total_screen_size() {
    /**
     * Calculates the total size of all screens combined.
     */
    auto screens = QApplication::screens();
    // get combined height and width of all screens
    auto *screensRect = new QGraphicsItemGroup();
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* item){
        screensRect->addToGroup(new QGraphicsRectItem(item->geometry()));
    });
    int height = std::ceil(screensRect->boundingRect().height());
    int width  = std::ceil(screensRect->boundingRect().width());

    return {width, height};
}
