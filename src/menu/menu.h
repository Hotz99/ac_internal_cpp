#pragma once
#include "../logger/logger.h"
#include <vector>
#include "../../libs/imgui/imgui.h"
#include "../sdl/sdl.h"

class Menu {
private:
    bool m_IsOpen;
public:
    static Menu& GetInstance();
    bool IsOpen();
    void Toggle(bool state);
    void Render();
    void Shutdown();
};

