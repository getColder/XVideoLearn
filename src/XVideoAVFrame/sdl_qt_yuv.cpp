#include "sdl_qt_yuv.h"
#include <qdebug.h>
#include <sdl/SDL.h>
#include <fstream>
#include <sstream>
#include <qimage.h>
#include <qmessagebox.h>
#include "XVideoView.h"
#include "xsdl.h"
extern "C"
{
#include <libavcodec/avcodec.h>
}

static XVideoView* view = nullptr;
static int sdl_width = 400;
static int sdl_height = 300;
static int pixel_size = 4;
static std::ifstream yuv_file;
static long long file_size = 0;
static AVFrame* frame = nullptr;
#define USER_VIDEO_PATH "videos/400_300_25.yuv"

SDL_QT_YUV::SDL_QT_YUV(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.label->resize(sdl_width, sdl_height);
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
    connect(this, SIGNAL(ViewS()), this, SLOT(View()));
    //创建XVideoView
    view = XVideoView::Create(XVideoView::SDL);
    view->Init(sdl_width, sdl_height, XVideoView::YUV420P, (void*)ui.label->winId());
    //view->Init(sdl_width, sdl_height, XVideoView::YUV420P, NULL);   //SDL独立窗口
    
    //打开YUV文件
    yuv_file.open(USER_VIDEO_PATH, std::ios::binary);
    if (!yuv_file)
    {
        QMessageBox::information(this, "", "open yuv failed!");
        return;
    }
    yuv_file.seekg(0, std::ios::end);
    file_size = yuv_file.tellg();       //指针到文件末尾获取文件大小
    yuv_file.seekg(0, std::ios::beg);
    //创建AVFrame
    frame = av_frame_alloc();
    frame->width = sdl_width;
    frame->height = sdl_height;
    frame->format = AV_PIX_FMT_YUV420P;
    ////////////////////////////
    // YYYY
    // UVUV
    // YYYY   为1组，两行共享一组UV，且UV为一半。
    ////////////////////////////
    frame->linesize[0] = sdl_width; //导出的视频没有对齐，故手动设置
    frame->linesize[1] = sdl_width / 2;
    frame->linesize[2] = sdl_width / 2;
    int ret = av_frame_get_buffer(frame, 0);
    if (ret != 0)
    {
        char buf[1024];
        av_strerror(ret, buf, sizeof(buf));
        qDebug() << buf;
    }

    //开始渲染
    //startTimer(10); //QT定时器渲染
    th_ = std::thread(&SDL_QT_YUV::Refresh, this);
}

SDL_QT_YUV::~SDL_QT_YUV()
{
    isExit = true;
    th_.join();
}

unsigned char* SDL_QT_YUV::VideoData()
{
    return NULL;
}

unsigned char* SDL_QT_YUV::VideoDataMirror()
{
    return NULL;
}

void SDL_QT_YUV::timerEvent(QTimerEvent* ev)
{
    unsigned char* data = nullptr;
    yuv_file.read((char*)frame->data[0], sdl_width * sdl_height);   //Y
    yuv_file.read((char*)frame->data[1], sdl_width * sdl_height / 4);   //U
    yuv_file.read((char*)frame->data[2], sdl_width * sdl_height / 4);   //V
    //view->Draw(data);
    view->DrawFrame(frame);
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


void SDL_QT_YUV::Refresh()
{
    using namespace std;
    while (!isExit)
    {
        emit ViewS();
        //this_thread::sleep_for(40ms);
        int fps = ui.spinBox->value();
        if (fps > 0)
        {
            MSleep(1000 / fps);
        }
        else
        {
            MSleep(16);
        }
    }
}

//渲染
void SDL_QT_YUV::View()
{
    unsigned char* data = nullptr;
    yuv_file.read((char*)frame->data[0], sdl_width * sdl_height);   //Y
    yuv_file.read((char*)frame->data[1], sdl_width * sdl_height / 4);   //U
    yuv_file.read((char*)frame->data[2], sdl_width * sdl_height / 4);   //V
    view->DrawFrame(frame);
    //循环播放
    if (file_size == yuv_file.tellg())
    {
        yuv_file.seekg(0);
    }
    //处理SDL窗口关闭事件
    if (view->IsExit()) {
        view->Close();
        exit(0);
    }
    std::stringstream ss;
    ss << "fps: " << view->render_fps();
    ui.label_fps->setText(ss.str().c_str());
}