//
// Created by l0drex on 30.09.21.
//

#ifndef WALLPAPER_SPLITTER_GRAPHICSVIEW_H
#define WALLPAPER_SPLITTER_GRAPHICSVIEW_H


#include <QGraphicsView>
#include "wallpapersplitter.h"

static const qreal ZOOM_AMOUNT = .1;

class GraphicsView : public QGraphicsView {
public:
    explicit GraphicsView(WallpaperSplitter *parent = nullptr);

private:
    WallpaperSplitter* parent;
    QPointF lastCursorPosition;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void dragMoveEvent(QDragMoveEvent *event) override;
};


#endif //WALLPAPER_SPLITTER_GRAPHICSVIEW_H
