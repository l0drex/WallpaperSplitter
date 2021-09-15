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
    explicit OffsetDialog(QImage &image, QWidget *parent = nullptr);
    ~OffsetDialog() override;
    void done(int i) override;
    static QSize showOffsetDialog(QImage &image);

private:
    Ui::OffsetDialog *ui;

    void addImage(QImage &image);
    void addScreens(QList<QScreen *> screens);
    void scaleView();

private slots:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
};



#endif //WALLPAPER_SPLITTER_OFFSETDIALOG_H
