//
// Created by l0drex on 14.09.21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_OffsetDialog.h" resolved

#include "offsetdialog.h"
#include "ui_offsetdialog.h"
#include <QDebug>
#include <QScreen>
#include <QPen>
#include <QBrush>
#include <KColorScheme>
#include <QGraphicsRectItem>


OffsetDialog::OffsetDialog(QWidget *parent) :
        QDialog(parent), ui(new Ui::OffsetDialog) {
    ui->setupUi(this);
}

OffsetDialog::~OffsetDialog() {
    delete ui;
}

void OffsetDialog::addImage(QImage &image) {
    auto pixmap = new QPixmap();
    pixmap->convertFromImage(image);
    auto scene = new QGraphicsScene(this);
    scene->addPixmap(*pixmap);
    ui->imageView->setScene(scene);
}

void OffsetDialog::addScreens(QList<QScreen *> screens) {
    auto colorScheme = KColorScheme(QPalette::Normal, KColorScheme::ColorSet::Complementary);
    auto backgroundColor = colorScheme.shade(KColorScheme::ShadeRole::DarkShade);
    backgroundColor.setAlpha(128);
    ui->imageView->scene()->addRect(ui->imageView->sceneRect(), QPen(), backgroundColor);
    auto screenRectList = QList<QGraphicsItem *>();
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* screen){
        // Todo merge rectangles in one widget or something
        // TODO make it moveable by the user
        screenRectList.append(new QGraphicsRectItem(screen->geometry()));
        //ui->imageView->scene()->addRect(screen->geometry(), pen);
    });
    auto group = ui->imageView->scene()->createItemGroup(screenRectList);
}

QSize OffsetDialog::getOffset(QImage &image, QList<QScreen *> &screens) {
    // TODO exit on cancel or closing window
    auto offsetDialog = new OffsetDialog();
    offsetDialog -> addImage(image);
    offsetDialog -> addScreens(screens);
    offsetDialog -> exec();
    auto screenRect = QRect();  // = offsetDialog->ui->imageView->findChild<QRect>("ScreenRect");
    auto offset = QSize(screenRect.x(), screenRect.y());
    return offset;
}
