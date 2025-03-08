#pragma once
#include "xcodec.h"
#include <iostream>

struct AVBufferRef;
class XDecode : public XCodec
{
public:
	bool Send(const AVPacket* pkt);	//���ͱ���
	bool Recv(AVFrame* frame);//���ͱ���
	std::vector<AVFrame*> End();	//�������ѽ���֡

	//��ʼ��Ӳ������
	enum HW_CODEC_ID
	{
		AV_HWDEVICE_TYPE_DXVA2 = 4
	};
	bool InitHW(int type = 4);
};

