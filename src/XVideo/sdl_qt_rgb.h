#pragma once

#include <QtWidgets/QWidget>
#include "ui_sdl_qt_rgb.h"

class SDL_QT_RGB : public QWidget
{
    Q_OBJECT

public:
    SDL_QT_RGB(QWidget *parent = nullptr);
    ~SDL_QT_RGB();

    void timerEvent(QTimerEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;

private:
    void YUVRender();
    QImage* img1;
    QImage* img2;

private:
    Ui::SDL_QT_RGBClass ui;
   
};
