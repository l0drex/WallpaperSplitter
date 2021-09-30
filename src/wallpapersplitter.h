//
// Created by l0drex on 15.09.21.
//

#ifndef WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H
#define WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H

#include <QDialog>
#include <QFileInfo>
#include <QGraphicsItemGroup>
#include "screensitem.h"


QT_BEGIN_NAMESPACE
namespace Ui { class WallpaperSplitter; }
QT_END_NAMESPACE

class WallpaperSplitter : public QDialog {
Q_OBJECT

public:
    explicit WallpaperSplitter(QWidget *parent = nullptr);
    ~WallpaperSplitter() override;
    static QStringList splitImage(const QFileInfo &imageFile, const QString &path, QPoint topLeft = {0, 0}, QPoint bottomRight = {0, 0});
    static QStringList splitImage(const QFileInfo &imageFile, const QList<QRect> &screens, const QString &path);
    static QStringList splitImage(const QFileInfo &imageFile, QPoint topLeft = {0, 0}, QPoint bottomRight = {0, 0});
    void addImage(QImage &image);
    void addImage(const QUrl &url);

private:
    Ui::WallpaperSplitter *ui;
    ScreensItem *screenGroup{};
    QFileInfo *imageFile;

    void scaleView();
    QStringList splitImage(const QString &path);
    static inline QSize totalScreenSize();

private slots:
    void selectImage();
    void applyWallpaper();
    void saveWallpapers();

protected:
    void resizeEvent(QResizeEvent *event) override;
};


#endif //WALLPAPER_SPLITTER_WALLPAPERSPLITTER_H
