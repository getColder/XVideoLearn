/*///////////////////////////////
*** ‰÷»æSDL µœ÷¿‡ ***
///////////////////////////////*/

#pragma once
#include "XVideoView.h"
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

class XSDL : public XVideoView
{
public:
	bool Init(int w, int h, Format fmt = RGBA, void* wind_id = nullptr) override;
	void Close() override;
	bool Draw(unsigned char* data, int linesize = 0) override;
	bool IsExit() override;
	void SetRenderNormal() { rMode_ = RenderMode::Normal; }
	void SetRenderCopy() { rMode_ = RenderMode::Copy; }
	void SetRenderMirror() { rMode_ = RenderMode::Mirror; }
	enum class RenderMode
	{
		Normal = 0,
		Copy,
		Mirror
	};

private:
	bool RenderNormal(int w, int h);
	bool RenderCopy(int w, int h);
	bool RenderMirror(int w, int h);
private:
	SDL_Window* win_ = nullptr;
	SDL_Renderer* render_ = nullptr;
	SDL_Texture* texture_ = nullptr;
	RenderMode rMode_ = RenderMode::Normal;
};

