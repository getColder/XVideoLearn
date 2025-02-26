#include "XVideoView.h"
#include "xsdl.h"
#include <iostream>
#include <algorithm>
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "avutil.lib")

XVideoView* XVideoView::Create(RenderType type)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}
	return nullptr;
}

bool XVideoView::DrawFrame(AVFrame* frame)
{
	if (!frame || !frame->data[0]) return false;

	//֡�ʼ���
	++count_;
	if (beg_ms_ <= 0)
	{
		//�״μ�ʱ
		beg_ms_ = clock();
	}
	else if ((clock() - beg_ms_) / (CLOCKS_PER_SEC / 1000) >= 1000)
	{
		//ÿ�����һ��֡��
		render_fps_ = count_;
		beg_ms_ = clock();
		count_ = 0;
	}

	//��Ⱦ��ʽ
	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
		return Draw(
			frame->data[0], frame->linesize[0],
			frame->data[1], frame->linesize[1],
			frame->data[2], frame->linesize[2]);
	case AV_PIX_FMT_RGBA:
		return Draw(frame->data[0], frame->linesize[0]);
	case AV_PIX_FMT_BGRA:
		return Draw(frame->data[0], frame->linesize[0]);
	case AV_PIX_FMT_ARGB:
		return Draw(frame->data[0], frame->linesize[0]);
	case AV_PIX_FMT_RGB24:
		return Draw(frame->data[0], frame->linesize[0]);
	default:
		break;
	}
	return true;
}

//���ļ�
bool XVideoView::Open(std::string filepath)
{
	if (ifs_.is_open())
	{
		ifs_.close();
	}
	ifs_.open(filepath, std::ios::binary);
	return ifs_.is_open();

}

AVFrame* XVideoView::Read(bool isMirror/* = false */)
{
	if (width_ <= 0 || height_ <= 0 || !ifs_) return NULL;
	//��������frame������һ�����ÿռ�
	if(frame_)
	{
		if (frame_->width != width_ || frame_->height != height_ || frame_->format != fmt_)
		{
			//�ͷź���������ռ�
			av_frame_free(&frame_);
		}
	}
	//�����AVFrameһ��ʼ�����ڻ�������
	if (!frame_)
	{
		frame_ = av_frame_alloc();
		frame_->width = width_;
		frame_->height = height_;
		frame_->format = fmt_;
		frame_->linesize[0] = width_ * 4;
		if (fmt_ == XVideoView::YUV420P)
		{
			frame_->linesize[0] = width_;		//Y
			frame_->linesize[1] = width_ / 2;	//U
			frame_->linesize[2] = width_ / 2;	//V
		}else if (fmt_ == XVideoView::RGB)
		{
			frame_->linesize[0] = width_ * 3;
		}
		auto re = av_frame_get_buffer(frame_, 0);
		if (re != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(re, buf, sizeof(buf) - 1);
			std::cout << buf << std::endl;
			av_frame_free(&frame_);
			return NULL;
		}
	}
	//��ȡһ֡����
	if (frame_->format == AV_PIX_FMT_YUV420P)
	{
		int count0 = frame_->linesize[0] * height_;
		int count1 = frame_->linesize[1] * height_ / 2;
		int count2 = frame_->linesize[2] * height_ / 2;
		ifs_.read((char*)frame_->data[0], count0);
		ifs_.read((char*)frame_->data[1], count1);
		ifs_.read((char*)frame_->data[2], count2);
		//����
		if (isMirror)
		{
			for (int i = 0; i < height_; ++i)
			{
				int row = i * width_;
				std::reverse(frame_->data[0] + row, frame_->data[0] + width_ + row);
			}
			for (int i = 0; i < height_ / 2; ++i)
			{
				int row = i * width_ / 2;
				std::reverse(frame_->data[1] + row, frame_->data[1] + width_ / 2 + row);
			}
			for (int i = 0; i < height_ / 2; ++i)
			{
				int row = i * width_ / 2;
				std::reverse(frame_->data[2] + row, frame_->data[2] + width_ / 2 + row);
			}
		}
	}else
	{
		ifs_.read((char*)frame_->data[0], frame_->linesize[0] * height_);
		//����
		if (isMirror)
		{
			int lsz = frame_->linesize[0];
			int pixe_size = fmt_ == XVideoView::RGB ? 3 : 4;
			for (int i = 0; i < height_; ++i)
			{
				int row = lsz * i;
				MirrorRGB((unsigned char*)frame_->data[0] + row, lsz, pixe_size);
			}
		}
	}
	if (ifs_.gcount() == 0)
		return NULL;
	return frame_;
}

void MSleep(unsigned int ms)
{
	using namespace std;

	auto beg = clock();
	for (int i = 0; i < ms; ++i)
	{
		this_thread::sleep_for(1ms);
		if ((clock() - beg) / (CLOCKS_PER_SEC / 1000) >= ms)
			break;
	}
}

long long NowMs()
{
	return clock() / (CLOCKS_PER_SEC / 1000);
}

//RGB���ֽ����龵��
void MirrorRGB(unsigned char * data, int lineSize, int pixel_size)
{
	for (int j = 0; j < lineSize / 2; j += pixel_size)
	{
		for (int i = 0; i < pixel_size; ++i)
		{
			int temp = data[j + i];
			data[j + i] = data[lineSize - j - (pixel_size - i)];
			data[lineSize - j - (pixel_size - i)] = temp;
		}
	}
}