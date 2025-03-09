#pragma once
#include "xcodec.h"
#include <iostream>

struct AVBufferRef;
class XDecode : public XCodec
{
public:
	bool Send(const AVPacket* pkt);	//发送编码
	bool Recv(AVFrame* frame);//发送编码
	std::vector<AVFrame*> End();	//缓冲区已解码帧

	//初始化硬件加速
	enum HW_CODEC_ID
	{
		AV_HWDEVICE_TYPE_DXVA2 = 4
	};
	bool InitHW(int type = 4);
};

