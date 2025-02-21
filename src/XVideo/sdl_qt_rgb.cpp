#include "sdl_qt_rgb.h"
#include <qdebug.h>
#include <sdl/SDL.h>
#include <fstream>
#include <qimage.h>
#include <qmessagebox.h>
#include "XVideoView.h"
#include "xsdl.h"
#pragma comment(lib,"SDL2.lib")

static XVideoView* view = nullptr;
static int sdl_width = 400;
static int sdl_height = 300;
static unsigned char* yuv_mem = nullptr;
static int pixel_size = 4;
static std::ifstream yuv_file;

SDL_QT_RGB::SDL_QT_RGB(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.label->resize(sdl_width, sdl_height);
    view = XVideoView::Create(XVideoView::SDL);
    view->Init(sdl_width, sdl_height, XVideoView::YUV420P, (void*)ui.label->winId());
    yuv_file.open("videos/400_300_25.yuv", std::ios::binary);
    if (!yuv_file)
    {
        QMessageBox::information(this, "", "open yuv failed!");
        return;
    }
    yuv_mem = new unsigned char[sdl_width * sdl_height * pixel_size];
    startTimer(10);
}

SDL_QT_RGB::~SDL_QT_RGB()
{
    if (yuv_mem)
    {
        delete[] yuv_mem;
    }
}

void SDL_QT_RGB::timerEvent(QTimerEvent* ev)
{
    yuv_file.read((char*)yuv_mem, sdl_width * sdl_height * 1.5);
    view->Draw(yuv_mem);
}

void SDL_QT_RGB::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(size());
    ui.label->move(0, 0);
    view->Scale(ui.label->width(), ui.label->height());
}
