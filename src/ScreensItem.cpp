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
    if(event->buttons() == Qt::MouseButton::RightButton) {
        const auto posDelta = event->buttonDownPos(Qt::MouseButton::RightButton) - event->pos();
        setScale({posDelta.x(), posDelta.y()});
    } else if (event->buttons() == Qt::MouseButton::LeftButton) {
        setPos(pos() -
               (event->buttonDownPos(Qt::MouseButton::LeftButton) - event->pos()));
    } else {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void ScreensItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(event->button() == Qt::MouseButton::RightButton) scalingMode = ScalingMode::none;
    QGraphicsItem::mouseReleaseEvent(event);
}

void ScreensItem::setPos(QPointF pos) {
    const double dx = (pos.x() + boundingRect().width()) - (parentItem()->x() + parentItem()->boundingRect().width());
    if(pos.x() < parentItem()->x()) {
        pos = {parentItem()->x(), pos.y()};
    } else if(dx > 0) {
        pos = {pos.x() - dx, pos.y()};
    }

    const double dy = (pos.y() + boundingRect().height()) - (parentItem()->y() + parentItem()->boundingRect().height());
    if(pos.y() < parentItem()->y()) {
        pos = {pos.x(), parentItem()->y()};
    } else if(dy > 0) {
        pos = {pos.x(), pos.y() - dy};
    }

    QGraphicsItemGroup::setPos(pos);
}

void ScreensItem::setScale(const QSizeF delta) {
    if(scalingMode == ScalingMode::none) {
        if(delta == QSizeF(0, 0)) return;
        if(delta.width() == 0) scalingMode = ScalingMode::vertical;
        else if(delta.height() == 0) scalingMode = ScalingMode::horizontal;
        else {
            // FIXME this code is never reached
            const auto ratio = delta.width() / delta.height();
            qDebug() << ratio;
            if (ratio > 1.5) scalingMode = ScalingMode::horizontal;
            else if (ratio < 0.5) scalingMode = ScalingMode::vertical;
            else scalingMode = ScalingMode::diagonal;
        }
    }

    auto previous_scale = scale();
    switch(scalingMode) {
        case vertical:
            previous_scale *= 1 - delta.height() / childrenBoundingRect().height();
            break;
        case horizontal:
            previous_scale *= 1 - delta.width() / childrenBoundingRect().width();
            break;
        case diagonal:
            qDebug() << "diagonal";
            // TODO
            break;
        default:
            qDebug() << "Scaling mode is not defined!";
            return;
    }
    // FIXME what if it is now to big?
    // FIXME bounding rectangle does not update
    // transform from the middle
    setTransformOriginPoint(childrenBoundingRect().width() / 2, childrenBoundingRect().height() / 2);
    QGraphicsItem::setScale(previous_scale);
    setTransformOriginPoint(pos());
}
