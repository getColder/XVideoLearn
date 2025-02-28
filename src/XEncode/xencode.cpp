#include "xencode.h"
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

static void PrintErr(int err)
{
	char buff[1024] = { 0 };
	av_strerror(err, buff, sizeof(buff) - 1);
	cout << buff << endl;
}

AVCodecContext* XEncode::Create(int codec_id)
{
	//找到编码器
	auto codec = avcodec_find_encoder((AVCodecID)codec_id);
	if (!codec)
	{
		cerr << "avcodec_find_encoder failed: " << codec_id << endl;
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

void XEncode::setContext(AVCodecContext* c)
{
	unique_lock<mutex> lock(mtx_);
	if (ctx_)
	{
		avcodec_free_context(&ctx_);
	}
	ctx_ = c;
}

bool XEncode::SetOpt(const char* key, const char* val)
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


bool XEncode::SetOpt(const char* key, int val)
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

bool XEncode::Open()
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

AVPacket* XEncode::Encode(const AVFrame* frame)
{
	if (frame == nullptr) return nullptr;
	unique_lock<mutex> lock(mtx_);
	if (!ctx_)
	{
		return nullptr;
	}
	auto ret = avcodec_send_frame(ctx_, frame);
	if (ret != 0) return nullptr;
	auto pkt = av_packet_alloc();
	ret = avcodec_receive_packet(ctx_, pkt);
	if (ret == 0)
	{
		return pkt;
	}
	av_packet_free(&pkt);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
	{
		return nullptr;
	}
	if (ret < 0)
	{
		PrintErr(ret);
	}
	return nullptr;
}

AVFrame* XEncode::CreateFrame()
{
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