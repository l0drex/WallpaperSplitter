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
    setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    setAcceptedMouseButtons(Qt::MouseButton::RightButton);
    setFlag(QGraphicsItem::ItemIsMovable);
    setTransformOriginPoint(childrenBoundingRect().width() / 2, childrenBoundingRect().height() / 2);
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
    // TODO implement resizing
    if(false and event->buttons() == Qt::MouseButton::RightButton) {
        const auto posDelta = event->buttonDownPos(Qt::MouseButton::RightButton) - event->pos();
        qreal scale = 0;
        if(posDelta.x() >= posDelta.y()) {
            scale = 1/posDelta.x();
        } else {
            scale = 1/posDelta.y();
        }
        qDebug() << scale;
        setScale(scale);
    } else if (event->buttons() == Qt::MouseButton::LeftButton) {
        setPos(pos() -
               (event->buttonDownPos(Qt::MouseButton::LeftButton) - event->pos()));
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void ScreensItem::setPos(QPointF pos) {
    const double dx = (pos.x() + childrenBoundingRect().width()) - (parentItem()->x() + parentItem()->boundingRect().width());
    if(pos.x() < parentItem()->x()) {
        pos = {parentItem()->x(), pos.y()};
    } else if(dx > 0) {
        pos = {pos.x() - dx, pos.y()};
    }

    const double dy = (pos.y() + childrenBoundingRect().height()) - (parentItem()->y() + parentItem()->boundingRect().height());
    if(pos.y() < parentItem()->y()) {
        pos = {pos.x(), parentItem()->y()};
    } else if(dy > 0) {
        pos = {pos.x(), pos.y() - dy};
    }

    QGraphicsItemGroup::setPos(pos);
}
