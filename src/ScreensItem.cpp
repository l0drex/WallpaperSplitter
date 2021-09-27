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
    if(event->buttons() == Qt::RightButton) {
        auto posDelta = event->buttonDownPos(Qt::RightButton) - event->pos();
        qreal newScale;

        if(posDelta.x() > posDelta.y()) {
            newScale = 1 - posDelta.x() / boundingRect().width();
        } else {
            newScale = 1 - posDelta.y() / boundingRect().height();
        }

        setScale(scale() * newScale);
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void ScreensItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(event->button() == Qt::MouseButton::RightButton) scalingMode = ScalingMode::none;
    QGraphicsItem::mouseReleaseEvent(event);
}

QVariant ScreensItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if(change == ItemPositionChange && parentItem()) {
        QPointF newPos = value.toPointF();
        QPointF newBottomRight = newPos + sceneBoundingRect().bottomRight();
        QRectF rect = parentItem()->sceneBoundingRect();
        if(!rect.contains(newPos) || !rect.contains(newBottomRight)) {
            newPos.setX(qMin(rect.right() - sceneBoundingRect().width(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom() - sceneBoundingRect().height(), qMax(newPos.y(), rect.top())));
            return newPos;
        }
    } else if(change == ItemScaleChange && parentItem()) {
        qreal newScale = value.toDouble();
        if(newScale > maxScale) newScale = maxScale;

        QPointF newPos = pos();
        QPointF newBottomRight = newPos + sceneBoundingRect().bottomRight();
        QRectF rect = parentItem()->sceneBoundingRect();
        if(!rect.contains(newBottomRight)) {
            newPos.setX(qMin(rect.right() - sceneBoundingRect().width(), qMax(newPos.x(), rect.left())));
            newPos.setY(qMin(rect.bottom() - sceneBoundingRect().height(), qMax(newPos.y(), rect.top())));
            setPos(newPos);
        }

        return newScale;
    }

    return QGraphicsItem::itemChange(change, value);
}
