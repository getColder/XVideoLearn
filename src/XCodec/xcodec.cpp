#include "xcodec.h"
#include <iostream>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

//通过预处理指令导入库
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

using namespace std;

void PrintErr(int err)
{
	char buff[1024] = { 0 };
	av_strerror(err, buff, sizeof(buff) - 1);
	cout << buff << endl;
}

AVCodecContext* XCodec::Create(int codec_id, bool isEncode)
{
	//找到编码器
	AVCodec* codec = nullptr;
	if (isEncode)
		codec = avcodec_find_encoder((AVCodecID)codec_id);
	else
		codec = avcodec_find_decoder((AVCodecID)codec_id);
	if (!codec)
	{
		cerr << (isEncode ? "avcodec_find_encoder failed: " : "avcodec_find_decoder failed: ") << codec_id << endl;
		return nullptr;
	}
	//创建上下文
	AVCodecContext* ctx = avcodec_alloc_context3(codec);
	if (!ctx)
	{
		cerr << "avcodec_alloc_context3 failed: " << codec_id << endl;
		return nullptr;
	}
	//设置参数默认值
	ctx->time_base = { 1, 25 };
	ctx->pix_fmt = AV_PIX_FMT_YUV420P;
	ctx->thread_count = 16;
	ctx->codec = codec;
	return ctx;
}

void XCodec::setContext(AVCodecContext* c)
{
	unique_lock<mutex> lock(mtx_);
	if (ctx_)
	{
		avcodec_free_context(&ctx_);
	}
	ctx_ = c;
}

bool XCodec::SetOpt(const char* key, const char* val)
{
	unique_lock<mutex> lock(mtx_);
	if (!ctx_)
	{
		return false;
	}
	auto ret = av_opt_set(ctx_->priv_data, key, val, 0);
	if (ret)
	{
		PrintErr(ret);
		return false;
	}
	return true;
}


bool XCodec::SetOpt(const char* key, int val)
{
	unique_lock<mutex> lock(mtx_);
	if (!ctx_)
	{
		return false;
	}
	auto ret = av_opt_set_int(ctx_->priv_data, key, val, 0);
	if (ret)
	{
		PrintErr(ret);
		return false;
	}
	return true;
}

bool XCodec::Open()
{
	unique_lock<mutex> lock(mtx_);
	if (!ctx_)
	{
		return false;
	}
	auto ret = avcodec_open2(ctx_, NULL, NULL);
	if (ret)
	{
		PrintErr(ret);
		return false;
	}
	return true;
}

AVFrame* XCodec::CreateFrame()
{
	using namespace std;

	unique_lock<mutex> lock(mtx_);
	if (!ctx_) return nullptr;
	auto frame = av_frame_alloc();
	frame->width = ctx_->width;
	frame->height = ctx_->height;
	frame->format = ctx_->pix_fmt;
	auto ret = av_frame_get_buffer(frame, 0);
	if (ret != 0)
	{
		av_frame_free(&frame);
		PrintErr(ret);
		return nullptr;
	}
	return frame;
}