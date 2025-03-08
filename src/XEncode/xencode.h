#pragma once
#include "xcodec.h"

class XEncode : public XCodec
{
public:
	XEncode() {}

	/////////////////////////////////////////////
	/// �������ݣ� �̰߳�ȫ
	/// @para frame���û�ά��������const��
	/// @return ʧ�ܷ�Χnullptr ����AVPacket�û�Ҫ�ֶ�����
	/////////////////////////////////////////////
	AVPacket* Encode(const AVFrame* frame);

	//�������б��뻺���е�AVPacket
	std::vector<AVPacket*> End();
private:
	AVCodecContext* ctx_ = nullptr;	//������������
	std::mutex mtx_;
};

