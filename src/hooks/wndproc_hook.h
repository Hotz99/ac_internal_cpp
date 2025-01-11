#pragma once
#include <windows.h>
#include "../../libs/imgui/imgui.h"
#include "../../libs/imgui/imgui_impl_win32.h"
#include "../../libs/imgui/imgui_impl_opengl3.h"
#include "../logger/logger.h"
#include "../menu/menu.h"

class WndProcHook {
private:
    HWND m_WindowPtr;

public:
    static WndProcHook& GetInstance() {
        static WndProcHook inst;
        return inst;
    }

    HWND GetWindow() const { return m_WindowPtr; }

    void Initialize();
    void Shutdown();
};
