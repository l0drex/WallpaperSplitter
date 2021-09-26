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
    if(event->buttons() == Qt::MouseButton::RightButton) {
        const auto posDelta = event->buttonDownPos(Qt::MouseButton::RightButton) - event->pos();
        setScale(posDelta);
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
    const double dx = (pos.x() + boundingRect().width()) - (parentItem()->boundingRect().width());
    if(pos.x() < 0) {
        pos = {0, pos.y()};
    } else if(dx > 0) {
        pos = {pos.x() - dx, pos.y()};
    }

    const double dy = (pos.y() + boundingRect().height()) - (parentItem()->boundingRect().height());
    if(pos.y() < 0) {
        pos = {pos.x(), 0};
    } else if(dy > 0) {
        pos = {pos.x(), pos.y() - dy};
    }

    QGraphicsItemGroup::setPos(pos);
}

void ScreensItem::setScale(const QPointF delta) {
    // choose a scaling mode
    if(scalingMode == ScalingMode::none) {
        if(delta == QPointF(0, 0) || delta.manhattanLength() < 100) return;
        else if(delta.x() == 0) scalingMode = ScalingMode::vertical;
        else if(delta.y() == 0) scalingMode = ScalingMode::horizontal;
        else {
            const auto ratio = abs(delta.x() / delta.y());
            if (ratio > 1.5) scalingMode = ScalingMode::horizontal;
            else if (ratio < 0.5) scalingMode = ScalingMode::vertical;
            else scalingMode = ScalingMode::diagonal;
        }
    }

    // calculate scale
    qreal newScale;
    switch(scalingMode) {
        case horizontal:
            newScale = 1 - delta.x() / childrenBoundingRect().width();
            break;
        case vertical:
            newScale = 1 - delta.y() / childrenBoundingRect().height();
            break;
        case diagonal:
            if(delta.y() < delta.x()) newScale = 1 - delta.x() / childrenBoundingRect().width();
            else newScale = 1 - delta.y() / childrenBoundingRect().height();
            break;
        default:
            // This should never happen
            qDebug() << "Scaling mode is not defined!";
            return;
    }

    // apply scale
    if((newScale * scale()) > maxScale) QGraphicsItem::setScale(maxScale);
    else {
        prepareGeometryChange();
        QGraphicsItem::setScale(newScale * scale());
    }
    // move back into the view if necessary
    setPos(pos());
}

QRectF ScreensItem::boundingRect() const {
    return {pos(), QGraphicsItemGroup::boundingRect().size() * scale()};
}
