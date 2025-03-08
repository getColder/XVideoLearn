#pragma once
#include "xcodec.h"

class XEncode : public XCodec
{
public:
	XEncode() {}

	/////////////////////////////////////////////
	/// 编码数据， 线程安全
	/// @para frame由用户维护（传入const）
	/// @return 失败范围nullptr 返回AVPacket用户要手动清理
	/////////////////////////////////////////////
	AVPacket* Encode(const AVFrame* frame);

	//返回所有编码缓冲中的AVPacket
	std::vector<AVPacket*> End();
private:
	AVCodecContext* ctx_ = nullptr;	//编码器上下文
	std::mutex mtx_;
};

