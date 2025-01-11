#pragma once

#include <windows.h>
#include "../../libs/minhook/minhook.h"
#include "./settings.h"
#include "../logger/logger.h"
#include "../game/ac_state.h"

class Godmode {
private:
    // `DWORD` is a `windows.h` typedef for `unsigned long`
    // we use `DWORD` instead of `uintptr_t` bc the value below 
    // will be used in inline assembly, compiled by MSVC
    DWORD m_LocalPlayerState;
    LPVOID* m_ReturnAddr;
    
    AcState* m_AcState;
    bool m_IsEnabled = false;

    void Enable();
    void Disable();
public:
    Godmode();
    ~Godmode();

    static Godmode& GetInstance() {
		static Godmode s_instance;
		return s_instance;
	}

    void Work();
};
