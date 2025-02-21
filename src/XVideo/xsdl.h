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
	bool Draw(unsigned char* data, int linesize = 0) override;
private:
	SDL_Window* win_ = nullptr;
	SDL_Renderer* render_ = nullptr;
	SDL_Texture* texture_ = nullptr;
};

