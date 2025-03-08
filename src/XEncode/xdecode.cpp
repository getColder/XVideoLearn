#include "xdecode.h"
#include <mutex>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
}



bool XDecode::Send(const AVPacket* pkt)
{
	using namespace std;

	unique_lock<mutex> lock(mtx_);
	if (!ctx_) return false;
	auto re = avcodec_send_packet(ctx_, pkt);
	if (re != 0) return false;
	return true;
}


bool XDecode::Recv(AVFrame* frame)
{
	using namespace std;

	unique_lock<mutex> lock(mtx_);
	if (!ctx_) return false;
	AVFrame* f = frame;
	//Ӳ��������Ҫ����
	if (ctx_->hw_device_ctx)
	{
		f = av_frame_alloc();
	}
	auto ret = avcodec_receive_frame(ctx_, f);
	if (ret == 0)
	{
		//GPU������Ҫ�Դ��ڴ�ת��
		if (ctx_->hw_device_ctx)
		{
			ret = av_hwframe_transfer_data(frame, f, 0);
			av_frame_free(&f);
			if (ret != 0)
			{
				PrintErr(ret);
				return false;
			}
		}
		return true;
	}
	//����ʧ��
	if (ctx_->hw_device_ctx)
		av_frame_free(&f);
	return false;
}

std::vector<AVFrame*> XDecode::End()
{
	std::vector<AVFrame*> res;
	//ȡ�������е�����
	int ret = 0;
	avcodec_send_packet(ctx_, NULL);
	while (ret >= 0)
	{
		AVFrame* frame = av_frame_alloc();
		ret = avcodec_receive_frame(ctx_, frame);
		if (ret < 0)
		{
			av_frame_free(&frame);
			break;
		}
		res.push_back(frame);
	}
	return res;
}

bool XDecode::InitHW(int type)
{
	using namespace std;

	unique_lock<mutex> lock(mtx_);
	AVBufferRef *ctx = nullptr;
	auto ret = av_hwdevice_ctx_create(&ctx, (AVHWDeviceType)type, NULL, NULL, 0);
	if (ret != 0)
	{
		PrintErr(ret);
		return false;
	}
	ctx_->hw_device_ctx = ctx;
	cout << "Ӳ������: " << type << endl;
	return true;
}