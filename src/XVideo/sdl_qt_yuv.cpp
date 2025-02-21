#include "sdl_qt_yuv.h"
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
static struct{
    unsigned char* mem = nullptr;
    size_t sz = 0;
} yuv_mem;
unsigned char* yuv_mirror = nullptr;
static int pixel_size = 4;
static std::ifstream yuv_file;

#define USER_VIDEO_PATH "videos/400_300_25.yuv"

SDL_QT_YUV::SDL_QT_YUV(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.btn_mirror, &QPushButton::clicked, [this](){
            isMirror = !isMirror;
            ui.btn_mirror->setText(isMirror ? "Origin" : "Mirror");
        });
    connect(ui.radioButton, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Copy" << checked;
            dynamic_cast<XSDL*>(view)->SetRenderCopy();
        }});
    connect(ui.radioButton_2, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Normal" << checked;
            dynamic_cast<XSDL*>(view)->SetRenderNormal();
        }});
    connect(ui.radioButton_3, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Mirror" << checked;
            dynamic_cast<XSDL*>(view)->SetRenderMirror();
        }});
    ui.label->resize(sdl_width, sdl_height);
    view = XVideoView::Create(XVideoView::SDL);
    view->Init(sdl_width, sdl_height, XVideoView::YUV420P, (void*)ui.label->winId());
    //view->Init(sdl_width, sdl_height, XVideoView::YUV420P, NULL);   //SDL独立窗口
    yuv_file.open(USER_VIDEO_PATH, std::ios::binary);
    if (!yuv_file)
    {
        QMessageBox::information(this, "", "open yuv failed!");
        return;
    }
    yuv_mem.sz = sdl_width * sdl_height * pixel_size;
    yuv_mem.mem = new unsigned char[yuv_mem.sz];
    yuv_mirror = new unsigned char[yuv_mem.sz];
    startTimer(10);
}

SDL_QT_YUV::~SDL_QT_YUV()
{
    if (yuv_mem.mem)
    {
        delete[] yuv_mem.mem;
    }
}

unsigned char* SDL_QT_YUV::VideoData()
{
    int frame_size = sdl_width * sdl_height;
    yuv_file.read((char*)yuv_mem.mem, frame_size * 1.5);
    return yuv_mem.mem;
}

unsigned char* SDL_QT_YUV::VideoDataMirror()
{
    //yyyyuvuv -> YYYYUVUVUV
    int frame_size = sdl_width * sdl_height;
    yuv_file.read((char*)yuv_mem.mem, frame_size * 1.5);
    memcpy(yuv_mirror, yuv_mem.mem, yuv_mem.sz);
    for (int j = 0; j < sdl_height; ++j)
    {
        int row = j * sdl_width;
        for (int i = 1; i <= sdl_width; ++i)
        {
            yuv_mirror[row + i - 1] = yuv_mem.mem[row + sdl_width - i];
        }
    }
    int line_size_uv = sdl_width / 2;
    for (int j = 0; j < sdl_height; ++j)
    {
        int row = j * line_size_uv + frame_size;
        for (int i = 1; i <= line_size_uv; ++i)
        {
            yuv_mirror[row + i - 1] = yuv_mem.mem[row + line_size_uv - i];
        }
    }
    return yuv_mirror;
}

void SDL_QT_YUV::timerEvent(QTimerEvent* ev)
{
    unsigned char* data = nullptr;
    if (isMirror) {
        data = VideoDataMirror();
    }
    else {
        data = VideoData();
    }
    view->Draw(data);
    //处理SDL窗口关闭事件
    if (view->IsExit()) {
        view->Close();
        exit(0);
    }
}

void SDL_QT_YUV::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(size());
    ui.label->move(0, 0);
    //手动缩放
    view->Scale(ui.label->width(), ui.label->height());
}
