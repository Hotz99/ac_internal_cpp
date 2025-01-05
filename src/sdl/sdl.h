#pragma once

#include <Windows.h>
#include "../logger/logger.h"

constexpr auto sdlModuleName = "SDL2.dll";

typedef int(*SDL_SetRelativeMouseMode_t)(int enabled);
typedef int(*SDL_SetWindowGrab_t)(void* window, int grabbed);
typedef void(*SDL_WarpMouseInWindow_t)(void* window, float x, float y);

class SDLUtils {
private:
	SDL_SetRelativeMouseMode_t m_SDL_SetRelativeMouseMode = nullptr;
	SDL_SetWindowGrab_t m_SDL_SetWindowGrab = nullptr;
	SDL_WarpMouseInWindow_t m_SDL_WarpMouseInWindow = nullptr;
public:
	static SDLUtils& GetInstance();
	int SetRelativeMouseMode(int enabled);
	int SetWindowGrab(void* window, int grabbed);
	void WarpMouseInWindow(void* window, float x, float y);
};