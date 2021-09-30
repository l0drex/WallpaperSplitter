//
// Created by l0drex on 16.09.21.
//

#include <KColorScheme>
#include <QScreen>
#include <QPen>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include "screensitem.h"

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
        rectangles.append(rect);
    });
}

const QList<QGraphicsRectItem *> &ScreensItem::getRectangles() const {
    return rectangles;
}

void ScreensItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if(event->button() == Qt::RightButton) {
        const auto pos = transformOriginPoint() - event->pos();
        if (-.3 * sceneBoundingRect().height() < pos.y() && pos.y() < .3 * sceneBoundingRect().height()) {
            setCursor(Qt::SizeHorCursor);
            scalingMode = horizontal;
        } else if (-.3 * sceneBoundingRect().width() < pos.x() && pos.x() < .3 * sceneBoundingRect().width()) {
            setCursor(Qt::SizeVerCursor);
            scalingMode = vertical;
        } else {
            scalingMode = diagonal;
            if (pos.x() * pos.y() > 0) setCursor(Qt::SizeFDiagCursor);
            else setCursor(Qt::SizeBDiagCursor);
        }
    }
    QGraphicsItem::mousePressEvent(event);
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
                    if (qAbs(newScale - scale()) > 1) return;
                    break;

                default:
                    qWarning() << "No scaling mode is set!";
                    return;
            }
            setScale(scale() * (1 - newScale));
            break;
        }
        case Qt::LeftButton:
            setCursor(Qt::DragMoveCursor);
            break;
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
            if(newScale < 0.1) return 0.1;
            setPos(itemChange(ItemPositionChange, pos()).toPointF());
            break;
        }
    }

    return QGraphicsItem::itemChange(change, value);
}
