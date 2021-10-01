//
// Created by l0drex on 30.09.21.
//

#include <QMouseEvent>
#include <QMimeData>
#include "graphicsview.h"

GraphicsView::GraphicsView(WallpaperSplitter *parent) : QGraphicsView(parent) {
    this->parent = parent;
    setAcceptDrops(true);
}

void GraphicsView::wheelEvent(QWheelEvent *event) {
    if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
        // 1 if zooming in, -1 if zooming out
        const auto scaleUp = 2*(event->angleDelta().y() < 0) - 1;
        const auto amount = 1 - ZOOM_AMOUNT * scaleUp;
        scale(amount, amount);

        // TODO zoom to mouse position
        event->accept();
    } else
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

void GraphicsView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasImage() || event->mimeData()->hasUrls())
        event->acceptProposedAction();
    QGraphicsView::dragEnterEvent(event);
}

void GraphicsView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasImage() || event->mimeData()->hasUrls())
        return;
    QGraphicsView::dragMoveEvent(event);
}

void GraphicsView::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasImage()) {
        auto image = QImage::fromData(event->mimeData()->imageData().toByteArray());
        parent->addImage(image);
    } else if (event->mimeData()->hasUrls())
        parent->addImage(event->mimeData()->urls().first());
    QGraphicsView::dropEvent(event);
}
