//
// Created by l0drex on 16.09.21.
//

#ifndef WALLPAPER_SPLITTER_SCREENSITEM_H
#define WALLPAPER_SPLITTER_SCREENSITEM_H


#include <QGraphicsItemGroup>

class ScreensItem : public QGraphicsItemGroup {
public:
    explicit ScreensItem(QGraphicsItem *parent);
    void setPos(QPointF pos);
private:
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void addScreens();
};


#endif //WALLPAPER_SPLITTER_SCREENSITEM_H
