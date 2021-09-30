//
// Created by l0drex on 30.09.21.
//

#include <QMouseEvent>
#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {
    setAcceptDrops(true);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    // TODO zoom into scene
    QGraphicsView::wheelEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        setCursor(Qt::DragMoveCursor);
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        // TODO move the scene
    }
    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    unsetCursor();
    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::dropEvent(QDropEvent *event) {
    // TODO accept drops of images and add them to the scene
    QGraphicsView::dropEvent(event);
}
