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

bool ScreenComparator(const QScreen *a, const QScreen *b) {
    return a->geometry().x() < b->geometry().x();
}

ScreensItem::ScreensItem(QGraphicsItem *parent) : QGraphicsItemGroup(parent) {
    addScreens();

    // make the screen item movable by the user
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptedMouseButtons(Qt::RightButton);
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setTransformOriginPoint(boundingRect().center());

    // calculate maximum scale
    qreal maxScaleWidth = parentItem()->boundingRect().width() / childrenBoundingRect().width();
    qreal maxScaleHeight = parentItem()->boundingRect().height() / childrenBoundingRect().height();
    maxScale = std::min(maxScaleHeight, maxScaleWidth);
}

void ScreensItem::addScreens() {
    auto screens = QApplication::screens();
    // get the currently used color scheme
    const auto colorScheme = KColorScheme();
    const auto pen = QPen(colorScheme.foreground(KColorScheme::ForegroundRole::ActiveText), 10);

    // I don't fully understand why this is necessary
    // most likely this will fail in different configs
    std::sort(screens.begin(), screens.end(), ScreenComparator);
    // draw a rectangle for every screen
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* screen){
        const auto rect = new QGraphicsRectItem();
        rect->setRect(screen->geometry());
        rect->setPen(pen);
        rect->setBrush(colorScheme.background(KColorScheme::BackgroundRole::ActiveBackground));
        rect->setOpacity(0.5);

        auto name = new QGraphicsTextItem(screen->model());
        name->adjustSize();
        name->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        // todo does not work
        name->setTransformOriginPoint(name->boundingRect().center());
        name->setPos(screen->geometry().center());
        addToGroup(name);

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

        // check if cursor is far enough to the edge  before setting a scaling mode
        // this prevents uncontrollable behaviour in the middle, where the mode cannot be detected
        if (qAbs(pos.x()) > qAbs(transformOriginPoint().x() * .6) || qAbs(pos.y()) > qAbs(transformOriginPoint().y() * .6)) {
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
            event->accept();
        }
    } else if (event->button() == Qt::LeftButton) {
        setCursor(Qt::DragMoveCursor);
    }
    QGraphicsItem::mousePressEvent(event);
}

void ScreensItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->buttons() == Qt::RightButton) {
        const auto mouseMovement = event->pos() - event->buttonDownPos(Qt::RightButton);
        // this is the start position of the action relative to the transform origin (the center) of this item
        const auto mouseStart = event->buttonDownPos(Qt::RightButton) - transformOriginPoint();

        // diagonal scaling uses the max difference
        if (scalingMode == diagonal) {
            if (mouseMovement.x() >= mouseMovement.y())
                scalingMode = horizontal;
            else
                scalingMode = vertical;
        }

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

            default:
                event->ignore();
                return;
        }
        setScale(scale() * (1 - newScale));
        event->accept();
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void ScreensItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(event->button() == Qt::MouseButton::RightButton) scalingMode = ScalingMode::none;
    unsetCursor();
    event->accept();
    QGraphicsItem::mouseReleaseEvent(event);
}

QVariant ScreensItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    switch (change) {
        case QGraphicsItem::ItemPositionChange: {
            // Because of the changed transformation origin, there is a difference between the top left point
            // of the item and the position. The difference is this delta vector.
            const QPointF delta = sceneBoundingRect().topLeft() - pos();
            QPointF newTopLeft = value.toPointF()  + delta;
            const QPointF newBottomRight = newTopLeft + sceneBoundingRect().bottomRight() + QPoint(1, 1);
            QRectF rect = parentItem()->sceneBoundingRect();

            if(!rect.contains(newTopLeft) || !rect.contains(newBottomRight)) {
                newTopLeft.setX(qBound(rect.left(), newTopLeft.x(), rect.right() + 1 - sceneBoundingRect().width()));
                newTopLeft.setY(qBound(rect.top(), newTopLeft.y(), rect.bottom() + 1 - sceneBoundingRect().height()));
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
