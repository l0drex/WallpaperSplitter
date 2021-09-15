//
// Created by l0drex on 14.09.21.
//

#ifndef WALLPAPER_SPLITTER_WALLPAPERSPLITTER_OLD_H
#define WALLPAPER_SPLITTER_WALLPAPERSPLITTER_OLD_H

#include <QMainWindow>
#include <QFileInfo>
#include <QApplication>


QT_BEGIN_NAMESPACE
namespace Ui { class WallpaperSplitter; }
QT_END_NAMESPACE

class WallpaperSplitter : public QMainWindow {
Q_OBJECT

public:
    explicit WallpaperSplitter(QWidget *parent = nullptr);

    ~WallpaperSplitter() override;

private:
    Ui::WallpaperSplitter *ui;
    QFileInfo *fileInfo;
    QImage *image;
    QPoint *offset;
    const QList<QScreen*> screens = QApplication::screens();
    QStringList *paths;

    QSize getCombinedScreenSize();

public slots:
    void select_image();
    void change_offset();
    void split_image();
    void apply_wallpapers();
};


#endif //WALLPAPER_SPLITTER_WALLPAPERSPLITTER_OLD_H
