//
// Created by l0drex on 30.09.21.
//

#include <QMouseEvent>
#include "graphicsview.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {
    setAcceptDrops(true);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    // 1 if zooming in, -1 if zooming out
    const auto scaleUp = 2*(event->angleDelta().y() < 0) - 1;
    const auto amount = 1 - ZOOM_AMOUNT * scaleUp;
    scale(amount, amount);

    // TODO zoom to mouse position
    centerOn(event->position().toPoint());
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
