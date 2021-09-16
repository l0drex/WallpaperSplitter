//
// Created by l0drex on 16.09.21.
//

#include <KColorScheme>
#include <QScreen>
#include <QPen>
#include <QApplication>
#include "ScreensItem.h"

ScreensItem::ScreensItem(QGraphicsItem *parent) : QGraphicsItemGroup(parent) {
    addScreens();

    // make the screen item movable by the user
    setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
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
