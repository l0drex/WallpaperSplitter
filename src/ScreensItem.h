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

private:
    ScalingMode scalingMode = ScalingMode::none;
    qreal maxScale;

    void addScreens();

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};


#endif //WALLPAPER_SPLITTER_SCREENSITEM_H
