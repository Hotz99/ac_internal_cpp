#pragma once
#include "./sdl.h"

// `reinterpret_cast` is explicit and type safe, used for low level pointer conversions (e.g. void* to int*)
// C-style casts are ambiguous and try multiple casting types (e.g. static, const, reinterpret)

int SDLUtils::SetRelativeMouseMode(int enabled) {
    if (!m_SDL_SetRelativeMouseMode) {
        m_SDL_SetRelativeMouseMode = reinterpret_cast<SDL_SetRelativeMouseMode_t>(
            GetProcAddress(GetModuleHandleA(SDL_MODULE_NAME), "SDL_SetRelativeMouseMode"));
        if (!m_SDL_SetRelativeMouseMode) {
            Logger::Error() << "[sdl_utils] failed to get SDL_SetRelativeMouseMode" << Logger::Endl;
            return -1;
        }
    }
    return m_SDL_SetRelativeMouseMode(enabled);
}

int SDLUtils::SetWindowGrab(void* window, int grabbed) {
    if (!m_SDL_SetWindowGrab) {
        m_SDL_SetWindowGrab = reinterpret_cast<SDL_SetWindowGrab_t>(
            GetProcAddress(GetModuleHandleA(SDL_MODULE_NAME), "SDL_SetWindowGrab"));
        if (!m_SDL_SetWindowGrab) {
            Logger::Error() << "[sdl_utils] failed to get SDL_SetWindowGrab" << Logger::Endl;
            return -1;
        }
    }
    return m_SDL_SetWindowGrab(window, grabbed);
}

void SDLUtils::WarpMouseInWindow(void* window, float x, float y) {
    if (!m_SDL_WarpMouseInWindow) {
        m_SDL_WarpMouseInWindow = reinterpret_cast<SDL_WarpMouseInWindow_t>(
            GetProcAddress(GetModuleHandleA(SDL_MODULE_NAME), "SDL_WarpMouseInWindow"));
        if (!m_SDL_WarpMouseInWindow) {
            Logger::Error() << "[sdl_utils] failed to get SDL_WarpMouseInWindow" << Logger::Endl;
            return;
        }
    }
    m_SDL_WarpMouseInWindow(window, x, y);
}