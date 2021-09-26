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
    void setScale(QSizeF delta);
private:
    ScalingMode scalingMode = ScalingMode::none;

    void addScreens();
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};


#endif //WALLPAPER_SPLITTER_SCREENSITEM_H
