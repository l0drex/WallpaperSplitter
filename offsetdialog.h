//
// Created by l0drex on 14.09.21.
//

#ifndef WALLPAPER_SPLITTER_OFFSETDIALOG_H
#define WALLPAPER_SPLITTER_OFFSETDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsItemGroup>


QT_BEGIN_NAMESPACE
namespace Ui { class OffsetDialog; }
QT_END_NAMESPACE

class OffsetDialog : public QDialog {
Q_OBJECT

public:
    explicit OffsetDialog(QImage &image, QWidget *parent = nullptr);
    ~OffsetDialog() override;
    static QPoint showOffsetDialog(QWidget *parent, QImage &image);

    QPointF getOffset();

private:
    Ui::OffsetDialog *ui;
    QGraphicsItemGroup *screenGroup{};
    QRect *image_rect;

    void addImage(QImage &image);
    void addScreens(QList<QScreen *> screens);
    void scaleView();

private slots:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
};



#endif //WALLPAPER_SPLITTER_OFFSETDIALOG_H
