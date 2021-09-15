//
// Created by l0drex on 15.09.21.
//

// You may need to build the project (run Qt uic code generator) to get "ui_WallpaperSplitter.h" resolved

#include "wallpapersplitter.h"
#include "ui_wallpapersplitter.h"


WallpaperSplitter::WallpaperSplitter(QWidget *parent) :
        QWidget(parent), ui(new Ui::WallpaperSplitter) {
    ui->setupUi(this);
}

WallpaperSplitter::~WallpaperSplitter() {
    delete ui;
}

