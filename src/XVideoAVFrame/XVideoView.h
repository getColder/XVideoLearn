/*///////////////////////////////
*** 适配渲染接口类 ***
* 隐藏SDL实现
* 渲染方案可替代
* 线程安全

抽象类：XVideoview渲染接口
工具函数：void MSleep(uint ms)  精确控帧
///////////////////////////////*/

#pragma once
#include <mutex>

struct AVFrame;
class XVideoView
{
public:
	enum Format
	{
		RGBA = 0,
		ARGB,
		YUV420P
	};
	enum RenderType
	{
		SDL = 0
	};
	//////////////////////////////////初始化渲染窗口 线程安全 多次调用(注意防止内存泄露)////////////////////////////////
	// @para w 窗口宽度
	// @para h 窗口高度
	// @para fmt 绘制像素格式
	// @para win_id 窗口句柄，如果为空，创建新窗口
	// @return 是否创建成功	
	////////////////////////////////////////////////////////////////////////////////
	virtual bool Init(int w, int h, Format fmt = RGBA, void* wind_id = nullptr) = 0;

	virtual void Close() = 0;

	virtual bool IsExit() = 0;

	//////////////////////////////////创建渲染器 线程安全////////////////////////////////
	// @para RenderType 渲染引擎类型
	// @return 是否创建成功	
	////////////////////////////////////////////////////////////////////////////////
	static XVideoView* Create(RenderType type);

	//////////////////////////////////渲染图像 线程安全////////////////////////////////
	// Draw 通用渲染
	// @para data 渲染的二进制数据
	// @para linesize 一行数据的字节数，若linesize <= 0 自动算出
	// @return 渲染是否创建成功	
	// 
	// Draw YUV渲染
	// @paras 各平面与对应元素个数
	// 
	// Draw AVFrame渲染，且支持FPS计数
	// @paras AVFrame对象
	////////////////////////////////////////////////////////////////////////////////
	virtual bool Draw(const unsigned char* data, int linesize = 0) = 0;
	virtual bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u,	int u_pitch,
		const unsigned char* v,	int v_pitch
	) = 0;
	bool DrawFrame(AVFrame* frame);
	//缩放
	void Scale(int w, int h)
	{
		scale_w_ = w;
		scale_h_ = h;
	}
	//帧率
	int render_fps() { return render_fps_; }

protected:
	//材质宽高
	int width_ = 0;
	int height_ = 0;
	//缩放
	int scale_w_ = 0;
	int scale_h_ = 0;
	//格式
	Format fmt_ = RGBA;
	//显示帧率
	int render_fps_ = 0;
	//锁
	std::mutex mtx_;
private:
	long long beg_ms_ = 0;	//计时开始时间
	int count_ = 0;			//统计显示次数
};

void MSleep(unsigned int ms);

