//
// Created by l0drex on 14.09.21.
//

#ifndef WALLPAPER_SPLITTER_OFFSETDIALOG_H
#define WALLPAPER_SPLITTER_OFFSETDIALOG_H

#include <QDialog>
#include <QGraphicsScene>


QT_BEGIN_NAMESPACE
namespace Ui { class OffsetDialog; }
QT_END_NAMESPACE

class OffsetDialog : public QDialog {
Q_OBJECT

public:
    explicit OffsetDialog(QWidget *parent = nullptr);
    ~OffsetDialog() override;
    void addImage(QImage &image);
    static QSize getOffset(QImage &image, QList<QScreen *> &screens);

private:
    Ui::OffsetDialog *ui;

    void addScreens(QList<QScreen *> &screens);
};



#endif //WALLPAPER_SPLITTER_OFFSETDIALOG_H
