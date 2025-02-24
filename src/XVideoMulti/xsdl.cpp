#include "xsdl.h"
#include <sdl/SDL.h>
#include <iostream>
#pragma comment(lib,"SDL2.lib")


static bool InitVideo()
{
	static bool is_first = true;
	static std::mutex mux;
	std::unique_lock<std::mutex> sdl_lock(mux);
	if (!is_first) return true;
	is_first = false;
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << SDL_GetError() << std::endl;
		return false;
	}
	//选择缩放算法，抗锯齿
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	return true;
}

bool XSDL::Init(int w, int h, Format fmt/* = RGBA */)
{
	if (w <= 0 || h <= 0) return false;
	if (!InitVideo()) return false;
	std::unique_lock<std::mutex> sdl_lock(mtx_);
	width_ = w;
	height_ = h;
	fmt_ = fmt;
	//防止重复创建
	if (texture_)
		SDL_DestroyTexture(texture_);
	if (render_)
		SDL_DestroyRenderer(render_);
	//1、创建窗口
	if (!win_)
	{
		if (!win_id_)
			win_ = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		else 
			win_ = SDL_CreateWindowFrom(win_id_);
		if (!win_) 
			std::cerr << SDL_GetError() << std::endl;
	}
	//2、创建渲染器
	render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
	if (!render_)
		std::cerr << SDL_GetError() << std::endl;
	//3、创建材质
	unsigned int sdl_fmt = SDL_PIXELFORMAT_ARGB8888;
	switch (fmt_)
	{
	case XVideoView::RGBA:
		sdl_fmt = SDL_PIXELFORMAT_RGBA32;
		break;
	case XVideoView::ARGB:
		sdl_fmt = SDL_PIXELFORMAT_ARGB32;
		break;
	case XVideoView::BGRA:
		sdl_fmt = SDL_PIXELFORMAT_BGRA32;
		break;
	case XVideoView::YUV420P:
		sdl_fmt = SDL_PIXELFORMAT_IYUV;
		break;
	default:
		break;
	}
	texture_ = SDL_CreateTexture(render_, sdl_fmt, SDL_TEXTUREACCESS_STREAMING, w, h);
	return true;
}

void XSDL::Close()
{
	//保证线程安全
	std::unique_lock<std::mutex> sdl_lock(mtx_);
	if (texture_)
	{
		SDL_DestroyTexture(texture_);
		texture_ = nullptr;
	}
	if (render_)
	{
		SDL_DestroyRenderer(render_);
		render_ = nullptr;
	}
	if (win_)
	{
		SDL_DestroyWindow(win_);
		win_ = nullptr;
	}
}

bool XSDL::IsExit()
{
	SDL_Event ev;
	SDL_WaitEventTimeout(&ev, 1);
	if (ev.type == SDL_QUIT)
		return true;
	return false;
}

bool XSDL::Draw(const unsigned char* data, int linesize/* = 0 */)
{
	if (!data) return false;
	std::unique_lock<std::mutex> sdl_lock(mtx_);
	if (!texture_ || !render_ || !win_ || width_ <= 0 || height_ <= 0)
		return false;
	if (linesize <= 0)
	{
		//自动识别
		switch (fmt_)
		{
		case XVideoView::RGBA:
			break;
		case XVideoView::ARGB:
			linesize = width_ * 4;
			break;
		case XVideoView::YUV420P:
			linesize = width_;
			break;
		default:
			break;
		}
	}
	if (linesize <= 0)
		return false;
	//纹理更新
	int ret = SDL_UpdateTexture(texture_, NULL, data, linesize);
	if (ret)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	SDL_RenderClear(render_);
	switch (rMode_)
	{
	case RenderMode::Normal:
		return RenderNormal(scale_w_, scale_h_);
	case RenderMode::Copy:
		return RenderCopy(scale_w_, scale_h_);
	case RenderMode::Mirror:
		return RenderMirror(scale_w_, scale_h_);
	default:
		break;
	}
	return RenderNormal(scale_w_, scale_h_);
}

bool XSDL::Draw(
	const unsigned char* y, int y_pitch,
	const unsigned char* u, int u_pitch,
	const unsigned char* v, int v_pitch)
{
	//参数检查
	if (!y || !u || !v) return false;
	std::unique_lock<std::mutex> sdl_lock(mtx_);
	if (!texture_ || !render_ || !win_ || width_ <= 0 || height_ <= 0)
		return false;
	//纹理更新
	int ret = SDL_UpdateYUVTexture(texture_, NULL,
		y, y_pitch, u, u_pitch, v, v_pitch);
	if (ret)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	SDL_RenderClear(render_);
	switch (rMode_)
	{
	case RenderMode::Normal:
		return RenderNormal(scale_w_, scale_h_);
	case RenderMode::Copy:
		return RenderCopy(scale_w_, scale_h_);
	case RenderMode::Mirror:
		return RenderMirror(scale_w_, scale_h_);
	default:
		break;
	}
	return RenderNormal(scale_w_, scale_h_);
}

bool XSDL::RenderNormal(int w, int h)
{
	SDL_Rect* pRect = nullptr;
	SDL_Rect rect_user = { 0, 0, w, h};
	if (w > 0 && h > 0)
		pRect = &rect_user;
	int ret = SDL_RenderCopy(render_, texture_, NULL, pRect);
	if (ret)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	//渲染
	SDL_RenderPresent(render_);
	return true;
}

bool XSDL::RenderCopy(int w, int h)
{
	SDL_Rect* pRect = nullptr;
	SDL_Rect rect_user = { 0, scale_h_ / 4, w / 2, h /2};
	if (w > 0 && h > 0)
		pRect = &rect_user;
	int ret = SDL_RenderCopy(render_, texture_, NULL, pRect);
	if (ret)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	//复制
	SDL_Rect rect_copy = { scale_w_ / 2,  scale_h_ / 4, scale_w_ / 2 , scale_h_ / 2 };
	ret = SDL_RenderCopy(render_, texture_, NULL, &rect_copy);
	if (ret)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	//渲染
	SDL_RenderPresent(render_);
	return true;
}

bool XSDL::RenderMirror(int w, int h)
{
	SDL_Rect* pRect = nullptr;
	SDL_Rect rect_user = { 0, scale_h_ / 4, w / 2, h / 2 };
	if (w > 0 && h > 0)
		pRect = &rect_user;
	int ret = SDL_RenderCopy(render_, texture_, NULL, pRect);
	if (ret)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	//复制
	SDL_Rect rect_copy = { scale_w_ / 2, scale_h_ / 4, scale_w_ / 2 , scale_h_ / 2 };
	SDL_Point sdl_point ={ 0,0 };
	ret = SDL_RenderCopyEx(render_, texture_, NULL, &rect_copy, 0, &sdl_point, SDL_FLIP_HORIZONTAL);
	if (ret)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	//渲染
	SDL_RenderPresent(render_);
	return true;
}