#pragma once
#include <vector>

#include "../logger/logger.h"
#include "../../libs/imgui/imgui.h"
#include "../sdl/sdl.h"
#include "../hacks/settings.h"
#include "../game/ac_state.h"
#include "../hacks/godmode.h"

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

