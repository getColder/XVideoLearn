/*///////////////////////////////
*** ������Ⱦ�ӿ��� ***
* ����SDLʵ��
* ��Ⱦ���������
* �̰߳�ȫ

�����ࣺXVideoview��Ⱦ�ӿ�
���ߺ�����void MSleep(uint ms)  ��ȷ��֡
///////////////////////////////*/

#pragma once
#include <mutex>
#include <fstream>

void MSleep(unsigned int ms);
long long NowMs();

struct AVFrame;
class XVideoView
{
public:
	enum Format
	{
		//�ȼ���ffmpeg
		YUV420P = 0,
		ARGB = 25,
		RGBA = 26,
		BGRA = 28,
	};
	enum RenderType
	{
		SDL = 0
	};
	//////////////////////////////////��ʼ����Ⱦ���� �̰߳�ȫ ��ε���(ע���ֹ�ڴ�й¶)////////////////////////////////
	// @para w ���ڿ��
	// @para h ���ڸ߶�
	// @para fmt �������ظ�ʽ
	// @return �Ƿ񴴽��ɹ�	
	////////////////////////////////////////////////////////////////////////////////
	virtual bool Init(int w, int h, Format fmt = RGBA) = 0;

	virtual void Close() = 0;

	virtual bool IsExit() = 0;

	//////////////////////////////////������Ⱦ�� �̰߳�ȫ////////////////////////////////
	// @para RenderType ��Ⱦ��������
	// @return �Ƿ񴴽��ɹ�	
	////////////////////////////////////////////////////////////////////////////////
	static XVideoView* Create(RenderType type);

	//////////////////////////////////��Ⱦͼ�� �̰߳�ȫ////////////////////////////////
	// Draw ͨ����Ⱦ
	// @para data ��Ⱦ�Ķ���������
	// @para linesize һ�����ݵ��ֽ�������linesize <= 0 �Զ����
	// @return ��Ⱦ�Ƿ񴴽��ɹ�	
	// 
	// Draw YUV��Ⱦ
	// @paras ��ƽ�����ӦԪ�ظ���
	// 
	// Draw AVFrame��Ⱦ����֧��FPS����
	// @paras AVFrame����
	////////////////////////////////////////////////////////////////////////////////
	virtual bool Draw(const unsigned char* data, int linesize = 0) = 0;
	virtual bool Draw(
		const unsigned char* y, int y_pitch,
		const unsigned char* u,	int u_pitch,
		const unsigned char* v,	int v_pitch
	) = 0;
	bool DrawFrame(AVFrame* frame);
	//����
	void Scale(int w, int h)
	{
		scale_w_ = w;
		scale_h_ = h;
	}
	//֡��
	int render_fps() { return render_fps_; }

	//���ļ�
	bool Open(std::string filepath);
	//��ȡһ֡��ÿ�ε��ûḲ����һ������
	AVFrame* Read(bool isMirror = false);
	//���ھ��
	void set_win_id(void* win) { win_id_ = win; }

protected:
	//���ʿ��
	int width_ = 0;
	int height_ = 0;
	//����
	int scale_w_ = 0;
	int scale_h_ = 0;
	//��ʽ
	Format fmt_ = RGBA;
	//��ʾ֡��
	int render_fps_ = 0;
	//��
	std::mutex mtx_;
	//���ھ��
	void* win_id_ = nullptr;
private:
	long long beg_ms_ = 0;	//��ʱ��ʼʱ��
	int count_ = 0;			//ͳ����ʾ����
	std::ifstream ifs_;		//��ȡ�ļ��ӿ�
	AVFrame* frame_ = nullptr;
};

