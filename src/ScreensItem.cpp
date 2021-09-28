//
// Created by l0drex on 16.09.21.
//

#include <KColorScheme>
#include <QScreen>
#include <QPen>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "ScreensItem.h"

ScreensItem::ScreensItem(QGraphicsItem *parent) : QGraphicsItemGroup(parent) {
    addScreens();

    // make the screen item movable by the user
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptedMouseButtons(Qt::RightButton);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setTransformOriginPoint(sceneBoundingRect().center());

    // calculate maximum scale
    qreal maxScaleWidth = parentItem()->boundingRect().width() / childrenBoundingRect().width();
    qreal maxScaleHeight = parentItem()->boundingRect().height() / childrenBoundingRect().height();
    maxScale = std::min(maxScaleHeight, maxScaleWidth);
}

void ScreensItem::addScreens() {
    auto screens = QApplication::screens();
    // get the currently used color scheme
    const auto colorScheme = KColorScheme();
    const auto pen = QPen(colorScheme.foreground(KColorScheme::ForegroundRole::ActiveText), 2);

    // draw a rectangle for every screen
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* screen){
        const auto rect = new QGraphicsRectItem();
        rect->setRect(QRect(screen->geometry()));
        rect->setPen(pen);
        rect->setBrush(colorScheme.background(KColorScheme::BackgroundRole::ActiveBackground));
        rect->setOpacity(0.5);
        addToGroup(rect);
    });
}

void ScreensItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    switch (event->buttons()) {
        case Qt::RightButton: {
            const auto mouseMovement = event->pos() - event->buttonDownPos(Qt::RightButton);
            // this is a vector from the center of this item to the mouse movement start point
            const auto mouseStart = event->buttonDownPos(Qt::RightButton) - transformOriginPoint();

            qreal newScale;
            switch (scalingMode) {
                case horizontal:
                    newScale = mouseMovement.x() / transformOriginPoint().x();
                    if (mouseStart.x() > 0) newScale *= -1;
                    break;

                case vertical:
                    newScale = mouseMovement.y() / transformOriginPoint().y();
                    if (mouseStart.y() > 0) newScale *= -1;
                    break;

                case diagonal:
                    // FIXME this is very buggy
                    newScale = mouseMovement.manhattanLength() / transformOriginPoint().manhattanLength();
                    break;

                default:
                    // scaling mode is not set
                    if (mouseMovement.manhattanLength() < 100) return;
                    const qreal ratio = qAbs(mouseMovement.x() / mouseMovement.y());
                    const qreal diagonalRatio = qAbs(boundingRect().width() / boundingRect().height());
                    const qreal tolerance = diagonalRatio / 2;
                    if (ratio > diagonalRatio + tolerance) {
                        setCursor(Qt::SizeHorCursor);
                        scalingMode = horizontal;
                    } else if (ratio < diagonalRatio - tolerance) {
                        setCursor(Qt::SizeVerCursor);
                        scalingMode = vertical;
                    } else {
                        scalingMode = diagonal;
                        if (mouseStart.x() * mouseStart.y() > 0) setCursor(Qt::SizeFDiagCursor);
                        else setCursor(Qt::SizeBDiagCursor);
                    }
                    return;
            }
            setScale(scale() * (1 - newScale));
            break;
        }
        case Qt::LeftButton:
            setCursor(Qt::DragMoveCursor);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void ScreensItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(event->button() == Qt::MouseButton::RightButton) scalingMode = ScalingMode::none;
    unsetCursor();
    QGraphicsItem::mouseReleaseEvent(event);
}

QVariant ScreensItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    switch (change) {
        case QGraphicsItem::ItemPositionChange: {
            // Because of the changed transformation origin, there is a difference between the top left point
            // of the item and the position. The difference is this delta vector.
            const QPointF delta = sceneBoundingRect().topLeft() - pos();
            QPointF newTopLeft = value.toPointF()  + delta;
            const QPointF newBottomRight = newTopLeft + sceneBoundingRect().bottomLeft() + QPoint(1, 1);
            QRectF rect = parentItem()->sceneBoundingRect();
            if(!rect.contains(newTopLeft) || !rect.contains(newBottomRight)) {
                newTopLeft.setX(qMin(qMax(rect.left(), newTopLeft.x()), rect.right() + 1 - sceneBoundingRect().width()));
                newTopLeft.setY(qMin(qMax(rect.top(), newTopLeft.y()), rect.bottom() + 1 - sceneBoundingRect().height()));
                return newTopLeft - delta;
            }
            break;
        }
        case QGraphicsItem::ItemScaleChange: {
            qreal newScale = value.toDouble();
            if(newScale > maxScale) return maxScale;

            QPointF newPos = pos();
            QPointF newBottomRight = newPos + sceneBoundingRect().bottomRight();
            QRectF rect = parentItem()->sceneBoundingRect();
            if(!rect.contains(newBottomRight)) {
                newPos.setX(qMin(rect.right() - sceneBoundingRect().width(), qMax(newPos.x(), rect.left())));
                newPos.setY(qMin(rect.bottom() - sceneBoundingRect().height(), qMax(newPos.y(), rect.top())));
                setPos(newPos);
            }
            break;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}
