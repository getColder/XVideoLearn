#pragma once

#include <QtWidgets/QWidget>
#include "ui_sdl_qt_yuv.h"
#include <thread>

class SDL_QT_YUV : public QWidget
{
    Q_OBJECT

public:
    SDL_QT_YUV(QWidget *parent = nullptr);
    ~SDL_QT_YUV();

    void timerEvent(QTimerEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;

private:
    unsigned char* VideoData();
    unsigned char* VideoDataMirror();
    bool isMirror = false;

    void Refresh();

signals:
    void ViewS();

private slots:
    void View();

private:
    Ui::SDL_QT_RGBClass ui;
    //äÖÈ¾Ë¢ÐÂÏß³Ì
    std::thread th_;
    bool isExit = false;

   
};
