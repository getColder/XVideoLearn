#include "XVideoView.h"
#include "xsdl.h"
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avutil.lib")

XVideoView* XVideoView::Create(RenderType type)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}
	return nullptr;
}

bool XVideoView::DrawFrame(AVFrame* frame)
{
	if (!frame || !frame->data[0]) return false;

	//帧率计算
	++count_;
	if (beg_ms_ <= 0)
	{
		//首次计时
		beg_ms_ = clock();
	}
	else if ((clock() - beg_ms_) / (CLOCKS_PER_SEC / 1000) >= 1000)
	{
		//每秒计算一次帧率
		render_fps_ = count_;
		beg_ms_ = clock();
		count_ = 0;
	}

	//渲染格式
	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
		return Draw(
			frame->data[0], frame->linesize[0],
			frame->data[1], frame->linesize[1],
			frame->data[2], frame->linesize[2]);
	case AV_PIX_FMT_BGRA:
		return Draw(
			frame->data[0], frame->linesize[0]);
	default:
		break;
	}
	return true;
}

void MSleep(unsigned int ms)
{
	using namespace std;

	auto beg = clock();
	for (int i = 0; i < ms; ++i)
	{
		this_thread::sleep_for(1ms);
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
			break;
	}
}