//
// Created by l0drex on 15.09.21.
//

#ifndef WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H
#define WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H

#include <QDialog>
#include <QFileInfo>
#include <QGraphicsItemGroup>


QT_BEGIN_NAMESPACE
namespace Ui { class WallpaperSplitter; }
QT_END_NAMESPACE

class WallpaperSplitter : public QDialog {
Q_OBJECT

public:
    explicit WallpaperSplitter(QWidget *parent = nullptr);
    ~WallpaperSplitter() override;

private:
    Ui::WallpaperSplitter *ui;
    QGraphicsItemGroup *screen_group{};
    QFileInfo *image_file;
    QImage *image;

    QPoint get_offset();
    void scaleView();
    QStringList split_image(QString &path);
    void add_screens();
    static QSize total_screen_size();

private slots:
    void select_image();
    void apply_wallpaper();
    void save_wallpapers();

protected:
    void resizeEvent(QResizeEvent *event) override;
};


#endif //WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H
