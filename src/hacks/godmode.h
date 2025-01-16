#pragma once

#include <windows.h>
#include "../../libs/minhook/minhook.h"
#include "./settings.h"
#include "../logger/logger.h"
#include "../game/ac_state.h"

class Godmode {
private:    
    AcState* m_AcState;
    bool m_IsEnabled = false;

    void Enable();
    void Disable();
public:
    Godmode();

    static Godmode& GetInstance() {
		static Godmode s_Instance;
		return s_Instance;
	}

    void Work();
    void Shutdown();
};
