/*///////////////////////////////
*** ������Ⱦ�ӿ��� ***
* ����SDLʵ��
* ��Ⱦ���������
* �̰߳�ȫ
///////////////////////////////*/

#pragma once
#include <mutex>

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
	//////////////////////////////////��ʼ����Ⱦ���� �̰߳�ȫ ��ε���(ע���ֹ�ڴ�й¶)////////////////////////////////
	// @para w ���ڿ��
	// @para h ���ڸ߶�
	// @para fmt �������ظ�ʽ
	// @para win_id ���ھ�������Ϊ�գ������´���
	// @return �Ƿ񴴽��ɹ�	
	////////////////////////////////////////////////////////////////////////////////
	virtual bool Init(int w, int h, Format fmt = RGBA, void* wind_id = nullptr) = 0;

	virtual void Close() = 0;

	virtual bool IsExit() = 0;

	//////////////////////////////////������Ⱦ�� �̰߳�ȫ////////////////////////////////
	// @para RenderType ��Ⱦ��������
	// @return �Ƿ񴴽��ɹ�	
	////////////////////////////////////////////////////////////////////////////////
	static XVideoView* Create(RenderType type);

	//////////////////////////////////��Ⱦͼ�� �̰߳�ȫ////////////////////////////////
	// @para data ��Ⱦ�Ķ���������
	// @para linesize һ�����ݵ��ֽ�������linesize <= 0 �Զ����
	// @return ��Ⱦ�Ƿ񴴽��ɹ�	
	////////////////////////////////////////////////////////////////////////////////
	virtual bool Draw(unsigned char* data, int linesize = 0) = 0;

	//����
	void Scale(int w, int h)
	{
		scale_w_ = w;
		scale_h_ = h;
	}


protected:
	//���ʿ��
	int width_ = 0;
	int height_ = 0;
	//����
	int scale_w_ = 0;
	int scale_h_ = 0;

	Format fmt_ = RGBA;
	std::mutex mtx_;
};

