#pragma once
//编解基类

#include <mutex>
#include <vector>

struct AVCodecContext;
struct AVPacket;
struct AVFrame;
void PrintErr(int err);

class XCodec
{

public:
	/////////////////////////////////////////////
	/// 创建编码器上下文
	///	@para codec_id 编码器ID号，数字对应ffmpeg
	/// @return 编码器上下文，失败返回NULL
	/////////////////////////////////////////////
	static AVCodecContext* Create(int codec_id, bool is_encode = true);
	/////////////////////////////////////////////
	/// 设置对象编码器上下文，上下文传递到对象中，资源由XEncode维护
	///	@para c 编码器上下文, 如果ctx不为空将先清理资源
	/////////////////////////////////////////////
	void setContext(AVCodecContext* c);

	/////////////////////////////////////////////
	/// 设置编码参数， 线程安全
	/////////////////////////////////////////////
	bool SetOpt(const char* key, const char* val);
	bool SetOpt(const char* key, int val);

	/////////////////////////////////////////////
	/// 打开编码器， 线程安全
	/////////////////////////////////////////////
	bool Open();

	/////////////////////////////////////////////
	// 创建一个AVFrame， 线程安全
	/// @return 失败范围nullptr 返回AVPacket要手动清理
	/////////////////////////////////////////////
	AVFrame* CreateFrame();

protected:
	AVCodecContext* ctx_ = nullptr;	//编码器上下文
	std::mutex mtx_;
};

