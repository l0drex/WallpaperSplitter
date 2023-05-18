//
// Created by l0drex on 30.09.21.
//

#include <QMouseEvent>
#include <QMimeData>
#include <QGuiApplication>
#include <QDebug>
#include <QImageReader>
#include <QMimeDatabase>
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
        setTransformationAnchor(AnchorUnderMouse);
        scale(amount, amount);
        event->accept();
    } else
        QGraphicsView::wheelEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        setCursor(Qt::DragMoveCursor);
        lastCursorPosition = event->pos();
        event->accept();
    }
    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {
    if (false && event->buttons() == Qt::MiddleButton) {
        // FIXME scene is always moved from the top left corner
        setTransformationAnchor(NoAnchor);
        const auto movement = mapToScene(event->pos() - lastCursorPosition);
        qDebug() << movement;
        translate(movement.x(), movement.y());
        event->accept();
    } else
        QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    unsetCursor();
    QGraphicsView::mouseReleaseEvent(event);
}

bool isLocalImageFile(const QUrl& url)
{
    if (!url.isLocalFile()) return false;

    QMimeDatabase mimeDatabase;
    QMimeType mimeType = mimeDatabase.mimeTypeForUrl(url);
    auto mimeTypeName = mimeType.name();

    // Check if the MIME type indicates an image
    return mimeTypeName.startsWith("image/");
}

bool checkDrop(QDragMoveEvent *event) {
    if (event->mimeData()->hasImage()) {
        return true;
    } else if (event->mimeData()->hasUrls()) {
        if (isLocalImageFile(event->mimeData()->urls().first())) {
            return true;
        }
    }

    return false;
}

void GraphicsView::dragEnterEvent(QDragEnterEvent *event) {
    if (checkDrop(event)) {
        event->acceptProposedAction();
    } else {
        QGraphicsView::dragEnterEvent(event);
    }
}

void GraphicsView::dragMoveEvent(QDragMoveEvent *event) {
    if (checkDrop(event)) {
        event->acceptProposedAction();
    } else {
        QGraphicsView::dragMoveEvent(event);
    }
}

void GraphicsView::dropEvent(QDropEvent *event) {
    if (event->mimeData()->hasImage()) {
        qDebug() << "New image dropped";
        auto image = QImage::fromData(event->mimeData()->imageData().toByteArray());
        parent->addImage(image);
    } else if (event->mimeData()->hasUrls()) {
        auto url = event->mimeData()->urls().first();
        if (isLocalImageFile(url)) {
            qDebug() << "New url dropped";
            parent->addImage(url);
        }
    } else
        QGraphicsView::dropEvent(event);
}
