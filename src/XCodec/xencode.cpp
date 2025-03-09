#include "xencode.h"
#include <iostream>
#include <mutex>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}

//通过预处理指令导入库
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

AVPacket* XEncode::Encode(const AVFrame* frame)
{
	using namespace std;

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

std::vector<AVPacket*> XEncode::End()
{
	using namespace std;

	vector<AVPacket*> res;
	unique_lock<mutex> lock(mtx_);
	if (!ctx_) return res;
	auto re = avcodec_send_frame(ctx_, NULL);
	if (re != 0) return res;
	while (re >= 0)
	{
		auto pkt = av_packet_alloc();
		re = avcodec_receive_packet(ctx_, pkt);
		if (re != 0)
		{
			av_packet_free(&pkt);
			break;
		}
		res.push_back(pkt);
	}
	return res;
}