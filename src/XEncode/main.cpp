#include "test.h"
#include <iostream>
#include <fstream>
#include "xencode.h"

extern "C"
{
#include <libavcodec/codec.h>
#include <libavutil/avutil.h>
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")

using namespace std;

int main(int argc, char* argv[])
{
	//1、找到编码器
	string fileName = "./videos/400_300_25";
	AVCodecID codec_id = AV_CODEC_ID_H264;
	if (argc > 1)
	{
		string codec = argv[1];
		if (codec == "h265" || codec == "hevc")
		{
			codec_id = AV_CODEC_ID_HEVC;
		}
	}
	if (codec_id == AV_CODEC_ID_H264)
	{
		fileName += ".h264";
	}else if (codec_id == AV_CODEC_ID_HEVC)
	{
		fileName += ".h265";
	}
	ofstream ofs;
	ofs.open(fileName, ios::binary);
	
	XEncode en;
	auto ctx = en.Create(codec_id);
	if (!ctx)
		return -1;
	int ret = 0;
	ctx->width = 400;
	ctx->height = 300;
	en.setContext(ctx);
	if (!en.SetOpt("crf", 18)) return -2;
	if (!en.Open()) return -3;

	//创建AVFrame空间
	auto frame = en.CreateFrame();
	if (!frame)
	{
		return -4;
	}
	//十秒视频
	for (int i = 0; i < 500; ++i)
	{
		//Y
		for (int y = 0; y < ctx->height; ++y)
		{
			for (int x = 0; x < ctx->height; ++x)
			{
				frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
			}
		}
		//UV
		for (int y = 0; y < ctx->height / 2; ++y)
		{
			for (int x = 0; x < ctx->height / 2; ++x)
			{
				frame->data[1][y * frame->linesize[1] + x] = x + y + i * 3;
				frame->data[2][y * frame->linesize[1] + x] = 128 + y + i;
				frame->data[2][y * frame->linesize[1] + x] = 50 + y + 5 * i;
			}
		}
		//pts
		frame->pts = i;
		//发送原始帧到线程中压缩
		auto pkt = en.Encode(frame);
		if (pkt)
		{
			ofs.write((char*)pkt->data, pkt->size);
			av_packet_free(&pkt);
		}
	}

	//释放上下文
	avcodec_free_context(&ctx);
	av_frame_free(&frame);
	ofs.close();
	return 0;
}