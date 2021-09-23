//
// Created by l0drex on 16.09.21.
//

#ifndef WALLPAPER_SPLITTER_SCREENSITEM_H
#define WALLPAPER_SPLITTER_SCREENSITEM_H


#include <QGraphicsItemGroup>

enum ScalingMode {none, vertical, horizontal, diagonal};

class ScreensItem : public QGraphicsItemGroup {
public:
    explicit ScreensItem(QGraphicsItem *parent);
    void setPos(QPointF pos);
    void setScale(QPointF posDelta);
private:
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    ScalingMode scalingMode = ScalingMode::none;
    qreal scale = 1;

    void addScreens();
};


#endif //WALLPAPER_SPLITTER_SCREENSITEM_H
