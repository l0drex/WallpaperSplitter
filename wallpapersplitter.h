//
// Created by l0drex on 15.09.21.
//

#ifndef WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H
#define WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui { class WallpaperSplitter; }
QT_END_NAMESPACE

class WallpaperSplitter : public QWidget {
Q_OBJECT

public:
    explicit WallpaperSplitter(QWidget *parent = nullptr);

    ~WallpaperSplitter() override;

private:
    Ui::WallpaperSplitter *ui;
};


#endif //WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H
