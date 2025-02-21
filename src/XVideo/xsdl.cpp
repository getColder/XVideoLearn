#include "xsdl.h"
#include <sdl/SDL.h>
#include <iostream>

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
	return true;
}

bool XSDL::Init(int w, int h, Format fmt/* = RGBA */, void* wind_id/* = nullptr*/)
{
	if (w <= 0 || h <= 0) return false;
	if (!InitVideo()) return false;
	std::unique_lock<std::mutex> sdl_lock(mtx_);
	width_ = w;
	height_ = h;
	fmt_ = fmt;

	//1、创建窗口
	if (!win_)
	{
		if (!wind_id)
		{
			win_ = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		}
		else {
			win_ = SDL_CreateWindowFrom(wind_id);
		}
		if (!win_) 
		{
			std::cerr << SDL_GetError() << std::endl;
		}
	}
	//2、创建渲染器
	render_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
	if (!render_)
	{
		std::cerr << SDL_GetError() << std::endl;
	}
	//3、创建材质
	unsigned int sdl_fmt = SDL_PIXELFORMAT_ARGB8888;
	switch (fmt_)
	{
	case XVideoView::RGBA:
		sdl_fmt = SDL_PIXELFORMAT_RGBA8888;
		break;
	case XVideoView::ARGB:
		sdl_fmt = SDL_PIXELFORMAT_ARGB8888;
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

bool XSDL::Draw(unsigned char* data, int linesize/* = 0 */)
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
	//材质复制
	if (scale_w_ <= 0) scale_w_ = width_;
	if (scale_h_ <= 0) scale_h_ = height_;
	SDL_Rect rect = {0, 0, scale_w_, scale_h_ };
	ret = SDL_RenderCopy(render_, texture_, NULL, &rect);
	if (ret)
	{
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	//渲染
	SDL_RenderPresent(render_);
	return true;
}

