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

class InvalidArgumentException : public QException {
private:
    QString message;
public:
    InvalidArgumentException() = default;

    explicit InvalidArgumentException(QString message) {
        this -> message = std::move(message);
    }

    inline void raise() const override {
        if(message != nullptr) {
            qDebug() << message;
        }
        throw *this;
    }
    InvalidArgumentException *clone() const override {
        return new InvalidArgumentException(*this);
    }
};

inline QFileInfo get_image_path() {
    auto url = QFileDialog::getOpenFileUrl(
            nullptr,
            "Select a wallpaper image",
            "file://" + QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            QString("image")
            );
    if(url.isEmpty()) QApplication::quit();  // FIXME
    return url.path();
}

auto split_image(QFileInfo &fileInfo) {
    /**
     * Split the image so that it spans across all screens when applied to their workspace.
     *
     * It uses the current screen setup and their configured geometry.
     *
     * @param image: The image that should be split.
     * @param fileInfo: The path to the image that should be split.
     */

    if(!fileInfo.isFile()) {
        throw InvalidArgumentException("Provided filepath does not exist!");
    }
    auto image = new QImage(fileInfo.filePath());
    if(image -> isNull() || image -> sizeInBytes() < 0) {
        throw InvalidArgumentException("Image is empty!");
    }

    // ask user about offset, if needed
    QSize offset = QSize(0, 0);
    auto screens = QApplication::screens();
    // NOTE maybe this has to be calculated over every screen
    auto last_screen_geometry = screens.last() -> geometry();
    int width = last_screen_geometry.x() + last_screen_geometry.width();
    int height = last_screen_geometry.y() + last_screen_geometry.height();
    if(image -> height() > height || image -> width() > width) {
        offset = OffsetDialog::getOffset(*image, screens);
    } else {
        if(image -> height() < height || image -> width() < width) {
            // TODO scale the image
            qDebug() << "Image is too small and has to be scaled";
        }
    }


    QImage wallpaper;
    QString directory = fileInfo.absolutePath() + '/' + fileInfo.baseName() + "_split";
    QDir().mkdir(directory);
    // list of the paths to return
    auto paths = QStringList();

    // FIXME this is to prevent endless savings while debugging / coding
    return paths;

    std::for_each(screens.begin(), screens.end(), [&](const QScreen *screen){
        // copy a rectangle with size and position of the screen
        auto geometry = screen -> geometry();
        // add offset
        geometry.setX(geometry.x() + offset.width());
        geometry.setY(geometry.y() + offset.height());
        wallpaper = image -> copy(geometry);

        // images are saved in a subdirectory called split with a number as suffix
        auto fileName = directory + '/' + screen->name() + '.' + fileInfo.suffix();
        paths.append(fileName);

        // if this returns false, the save failed and the assertion fails
        bool success = wallpaper.save(fileName);
        assert(success);
    });

    return paths;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // get wallpaper image
    auto fileInfo = get_image_path();

    // split the image
    split_image(fileInfo);

    // TODO apply the wallpapers

    QApplication::quit();
    return 0;
}
