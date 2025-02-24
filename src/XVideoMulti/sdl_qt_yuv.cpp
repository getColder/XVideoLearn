#include "sdl_qt_yuv.h"
#include <sdl/SDL.h>
#include <fstream>
#include <sstream>
#include <vector>

#include <qdebug.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include "XVideoView.h"
#include "xsdl.h"

static std::vector<XVideoView*> views;
static XVideoView* view = nullptr;
//static int sdl_width = 400;
//static int sdl_height = 300;
//static int pixel_size = 4;
//static std::ifstream yuv_file;
//static long long file_size = 0;
//static AVFrame* frame = nullptr;
//static AVFrame* VideoDataMirror();
#define USER_VIDEO_PATH "videos/400_300_25.yuv"

SDL_QT_YUV::SDL_QT_YUV(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.label->setFixedWidth(800);
    ui.label->setFixedHeight(600);
    ui.widget_2->hide();
    ui.label_2->hide();
    connect(ui.btn_mirror, &QPushButton::clicked, [this](){
            isMirror = !isMirror;
            ui.btn_mirror->setText(isMirror ? "Origin" : "Mirror");
        });
    connect(ui.radioButton, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Copy" << checked;
            dynamic_cast<XSDL*>(views[0])->SetRenderCopy();
        }});
    connect(ui.radioButton_2, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Normal" << checked;
            dynamic_cast<XSDL*>(views[0])->SetRenderNormal();
        }});
    connect(ui.radioButton_3, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Mirror" << checked;
            dynamic_cast<XSDL*>(views[0])->SetRenderMirror();
        }});
    connect(ui.radioButton_4, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Normal" << checked;
            dynamic_cast<XSDL*>(views[1])->SetRenderNormal();
        }});
    connect(ui.radioButton_5, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Copy" << checked;
            dynamic_cast<XSDL*>(views[1])->SetRenderCopy();
        }});
    connect(ui.radioButton_6, &QRadioButton::toggled, [this](bool checked) {
        if (checked)
        {
            qDebug() << "Mirror" << checked;
            dynamic_cast<XSDL*>(views[1])->SetRenderMirror();
        }});
    connect(ui.checkMulti, &QCheckBox::stateChanged, [&](int state)
        {
            qDebug() << ui.widget_videos->size();
            if (state == Qt::Checked)
            {
                ui.label->setFixedWidth(ui.widget_videos->width() / 2);
                ui.label->setFixedHeight(ui.widget_videos->height() / 2);
                ui.label_2->setFixedWidth(ui.widget_videos->width() / 2);
                ui.label_2->setFixedHeight(ui.widget_videos->height() / 2);
                ui.label_2->setEnabled(true);
                ui.label_2->show();
                ui.label->move(0, (ui.widget_videos->height() - ui.label->height()) / 2);
                ui.label_2->move(ui.label->width(), (ui.widget_videos->height() - ui.label_2->height()) / 2);
                ui.widget_2->show();
            }
            else
            {
                ui.widget_2->hide();
                ui.label->setFixedWidth(ui.widget_videos->width());
                ui.label->setFixedHeight(ui.widget_videos->height());
                ui.label->move(0, 0);
                ui.label_2->setDisabled(true);
                ui.label_2->hide();
                ui.radioButton->setEnabled(true);
                ui.radioButton_2->setEnabled(true);
                ui.radioButton_3->setEnabled(true);
            }
            //调整布局expanding
            Resize();
            qDebug() << ui.widget_videos->size();
        });
    connect(this, SIGNAL(ViewS()), this, SLOT(View()));
    ////创建XVideoView
    //view = XVideoView::Create(XVideoView::SDL);
    //view->Init(sdl_width, sdl_height, XVideoView::YUV420P, (void*)ui.label->winId());
    ////view->Init(sdl_width, sdl_height, XVideoView::YUV420P, NULL);   //SDL独立窗口
    //
    ////打开YUV文件
    //yuv_file.open(USER_VIDEO_PATH, std::ios::binary);
    //if (!yuv_file)
    //{
    //    QMessageBox::information(this, "", "open yuv failed!");
    //    return;
    //}
    //yuv_file.seekg(0, std::ios::end);
    //file_size = yuv_file.tellg();       //指针到文件末尾获取文件大小
    //yuv_file.seekg(0, std::ios::beg);
    ////创建AVFrame
    //frame = av_frame_alloc();
    //frame->width = sdl_width;
    //frame->height = sdl_height;
    //frame->format = AV_PIX_FMT_YUV420P;
    ////////////////////////////
    // YYYY
    // UVUV
    // YYYY   为1组，两行共享一组UV，且UV为一半。
    ////////////////////////////
    //frame->linesize[0] = sdl_width; //导出的视频没有对齐，故手动设置
    //frame->linesize[1] = sdl_width / 2;
    //frame->linesize[2] = sdl_width / 2;
    //int ret = av_frame_get_buffer(frame, 0);
    //if (ret != 0)
    //{
    //    char buf[1024];
    //    av_strerror(ret, buf, sizeof(buf));
    //    qDebug() << buf;
    //}

    //开始渲染
    //startTimer(10); //QT定时器渲染
    views.push_back(XVideoView::Create(XVideoView::SDL));
    views.push_back(XVideoView::Create(XVideoView::SDL));
    views[0]->set_win_id((void*)ui.label->winId());
    views[1]->set_win_id((void*)ui.label_2->winId());
    th_ = std::thread(&SDL_QT_YUV::Refresh, this);
}

void SDL_QT_YUV::Resize()
{
    //调整布局expanding
    resize(width() - 1,  height());
    resize(width() + 1,  height());
}

SDL_QT_YUV::~SDL_QT_YUV()
{
    isExit = true;
    th_.join();
}

AVFrame* VideoDataMirror()
{
    //Mirror Frame
    //yyyyuvuv -> YYYYUVUVUV
    //for (int j = 0; j < sdl_height; ++j)
    //{
    //    int row = j * sdl_width;
    //    for (int i = 0; i < sdl_width / 2; ++i)
    //    {
    //        unsigned char temp = frame->data[0][i + row];
    //        frame->data[0][i + row] = frame->data[0][sdl_width - i - 1 + row];
    //        frame->data[0][sdl_width - i - 1 + row] = temp;
    //    }
    //}
    //int uv_line_size = sdl_width / 2;
    //for (int j = 0; j < sdl_height / 2; ++j)
    //{
    //    int row = j * uv_line_size;
    //    for (int i = 0; i < uv_line_size / 2; ++i)
    //    {
    //        unsigned char temp1 = frame->data[1][i + row];
    //        frame->data[1][i + row] = frame->data[1][uv_line_size - i - 1 + row];
    //        frame->data[1][uv_line_size - i - 1 + row] = temp1;
    //        unsigned char temp2 = frame->data[2][i + row];
    //        frame->data[2][i + row] = frame->data[2][uv_line_size - i - 1 + row];
    //        frame->data[2][uv_line_size - i - 1 + row] = temp2;
    //    }
    //}
    //return frame;
    return nullptr;
}

void SDL_QT_YUV::timerEvent(QTimerEvent* ev)
{
    unsigned char* data = nullptr;
    //yuv_file.read((char*)frame->data[0], sdl_width * sdl_height);   //Y
    //yuv_file.read((char*)frame->data[1], sdl_width * sdl_height / 4);   //U
    //yuv_file.read((char*)frame->data[2], sdl_width * sdl_height / 4);   //V
    //view->Draw(data);
    //view->DrawFrame(frame);
    //处理SDL窗口关闭事件
    if (view->IsExit()) {
        view->Close();
        exit(0);
    }
}

void SDL_QT_YUV::resizeEvent(QResizeEvent* ev)
{
    ui.label->resize(ui.spinBox_w->value(), ui.spinBox_h->value());
    ui.label_2->resize(ui.spinBox_w_2->value(), ui.spinBox_h_2->value());
    views[0]->Scale(ui.spinBox_w->value(), ui.spinBox_h->value());
    views[1]->Scale(ui.spinBox_w_2->value(), ui.spinBox_h_2->value());
}


void SDL_QT_YUV::Refresh()
{
    using namespace std;
    while (!isExit)
    {
        emit ViewS();
        MSleep(1);
    }
}

//渲染
void SDL_QT_YUV::View()
{
    static int last_pts[32] = { 0 };
    static int fps_arr[2] = { 0 };
    fps_arr[0] = ui.spinBox_fps->value();
    fps_arr[1] = ui.spinBox_fps2->value();
    for (int i = 0;i < 2; ++i)
    {
        auto view = views[i];
        //按帧率渲染
        int ms = 1000 / fps_arr[i];
        if (fps_arr[i] <= 0) continue;
        if (NowMs() - last_pts[i] < ms) continue;       //未到渲染时间
        last_pts[i] = NowMs();
        //显示帧率
        std::stringstream ss;
        ss << "fps: " << view->render_fps();
        if (i == 0)
            ui.label_fps->setText(ss.str().c_str());
        else
            ui.label_fps_2->setText(ss.str().c_str());
        //读取与渲染
        AVFrame* frame = view->Read(isMirror);
        if (frame)
        {
            view->DrawFrame(frame);
        }
        //处理SDL窗口关闭事件
        if (view->IsExit()) {
            view->Close();
            exit(0);
        }
        
    }
    //循环播放
    //if (file_size == yuv_file.tellg())
    //{
    //    yuv_file.seekg(0);
    //}
}

void SDL_QT_YUV::Open(int i)
{
    QFileDialog fd;
    auto filename = fd.getOpenFileName();
    if (filename.isEmpty()) return;
    qDebug() << filename << endl;
    //view 打开文件
    if (!views[i]->Open(filename.toStdString()))
    {
        return;
    }
    //初始化窗口材质
    int w = 0;
    int h = 0;
    QString sFmt = 0;
    if (i == 0)
    {
        w = ui.spinBox_w->value();
        h = ui.spinBox_h->value();
        sFmt = ui.comboBox1_fmt->currentText();
    }
    else {
        w = ui.spinBox_w_2->value();
        h = ui.spinBox_h_2->value();
        sFmt = ui.comboBox2_fmt->currentText();
    }
    XVideoView::Format fmt = XVideoView::YUV420P;
    if (sFmt == "YUV420P")
    {
        fmt = XVideoView::YUV420P;
    }
    else if(sFmt == "ARGB"){
        fmt = XVideoView::ARGB;
    }
    else if (sFmt == "BGRA") {
        fmt = XVideoView::BGRA;
    }
    else if (sFmt == "RGBA") {
        fmt = XVideoView::RGBA;
    }
    views[i]->Init(w, h, fmt);
}

void SDL_QT_YUV::Open1()
{
    Open(0);
}

void SDL_QT_YUV::Open2()
{
    Open(1);
}