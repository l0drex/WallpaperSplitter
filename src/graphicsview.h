//
// Created by l0drex on 30.09.21.
//

#ifndef WALLPAPER_SPLITTER_GRAPHICSVIEW_H
#define WALLPAPER_SPLITTER_GRAPHICSVIEW_H


#include <QGraphicsView>

class GraphicsView : public QGraphicsView {
public:
    explicit GraphicsView(QWidget *parent = nullptr);

protected:
    void dropEvent(QDropEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};


#endif //WALLPAPER_SPLITTER_GRAPHICSVIEW_H
