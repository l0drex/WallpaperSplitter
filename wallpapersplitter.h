//
// Created by l0drex on 14.09.21.
//

#ifndef WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H
#define WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H

#include <QMainWindow>
#include <QFileInfo>


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
    QList<QScreen*> screens;
    QStringList *paths;

    QSize change_offset(QImage &image);

public slots:
    void select_image();
    void split_image();
    void apply_wallpapers();
};


#endif //WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H
