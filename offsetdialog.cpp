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


OffsetDialog::OffsetDialog(QImage &image, QWidget *parent) :
        QDialog(parent), ui(new Ui::OffsetDialog) {
    ui->setupUi(this);
    ui->imageView->setScene(new QGraphicsScene(this));

    addImage(image);
    addScreens(QApplication::screens());
}

OffsetDialog::~OffsetDialog() {
    delete ui;
}

void OffsetDialog::addImage(QImage &image) {
    /**
     * Adds the image to the graphics view
     */
    const auto pixmap = new QPixmap();
    pixmap->convertFromImage(image);
    const auto item = ui->imageView->scene()->addPixmap(*pixmap);
    item->setFlag(QGraphicsItem::ItemClipsChildrenToShape);  // FIXME group escapes somehow
}

void OffsetDialog::addScreens(QList<QScreen *> screens) {
    // get the currently used color scheme
    const auto colorScheme = KColorScheme(QPalette::Normal, KColorScheme::ColorSet::Complementary);

    // draw a rectangle for every screen
    // all rectangles are in a group that is a child of the image, so that they clip to the borders
    const auto image = ui->imageView->scene()->items().first();
    const auto group = new QGraphicsItemGroup(image);
    const auto pen = QPen(colorScheme.foreground(), 2);
    std::for_each(screens.begin(), screens.end(), [&](const QScreen* screen){
        const auto rect = ui->imageView->scene()->addRect(screen->geometry(), pen);
        group -> addToGroup(rect);
    });

    // darken the background around the screen rectangles
    auto backgroundColor = colorScheme.shade(KColorScheme::ShadeRole::DarkShade);
    backgroundColor.setAlpha(128);
    auto background = QPolygonF(ui->imageView->sceneRect());
    background = background.subtracted(group->childrenBoundingRect());
    ui->imageView->scene()->addPolygon(background,
                                       QPen(QColor("transparent")),
                                       backgroundColor);

    // make the screen item movable by the user
    group->setAcceptedMouseButtons(Qt::MouseButton::LeftButton);
    group->setFlag(QGraphicsItem::ItemIsMovable);
}

void OffsetDialog::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    scaleView();
}

void OffsetDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    scaleView();
}

void OffsetDialog::done(int i) {
    QDialog::done(i);
    exit(0);
}

void OffsetDialog::scaleView() {
    // FIXME ensure that this is indeed the image and not something else
    ui->imageView->fitInView(ui->imageView->scene()->items().first()->boundingRect(),
                             Qt::AspectRatioMode::KeepAspectRatio);
}

QSize OffsetDialog::showOffsetDialog(QImage &image) {
    // set up the dialog
    const auto offsetDialog = new OffsetDialog(image);
    offsetDialog -> exec();

    // the offset is the position of the screen rectangle element
    auto screenRect = QRect();  // = offsetDialog->ui->imageView->findChild<QRect>("ScreenRect");
    auto offset = QSize(screenRect.x(), screenRect.y());
    return offset;
}
