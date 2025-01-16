#include "./godmode.h"

// `dodamage()` original instructions:
// .text:0041C223 sub[ebx + 4], esi		- 29 73 04 
// .text:0041C226 mov     eax, esi		- 8B C6

// `DWORD` is a `windows.h` typedef for `unsigned long`
// we use `DWORD` instead of `uintptr_t` bc the value below 
// will be used in inline assembly, compiled by MSVC
DWORD localPlayerState;
DWORD returnAddr;

void _declspec(naked) DecreaseHealthHookPayload() {
    __asm {
        cmp ebx, [localPlayerState];    // `ebx` contains `this` pointer
        jne is_bot;                     // jump if above `cmp` is false
        mov esi, 0;                     // else set `esi` (register containing damage value) to 0
    is_bot:
        sub[ebx + 4], esi;              // execute original instructions
        mov eax, esi;
        jmp returnAddr;                 // jump back to original function
    }
}

Godmode::Godmode() {
    m_AcState = &AcState::GetInstance();

    // first 5 bytes of `dodamage()` function are overwritten by our hook
    // jump back to original function after executing our hook, avoiding infinite loop
    returnAddr = (m_AcState->m_DecreaseHealthFnPtr + 5);

    if (MH_CreateHook(reinterpret_cast<LPVOID>(m_AcState->m_DecreaseHealthFnPtr), DecreaseHealthHookPayload, NULL) != MH_OK) {
		Logger::Error() << "[godmode] failed to create dodamage() hook" << Logger::Endl;
		return;
    }
    else {
		Logger::Info() << "[godmode] created dodamage() hook" << Logger::Endl;
	}
}

void Godmode::Enable() {
    localPlayerState = reinterpret_cast<DWORD>(&m_AcState->m_LocalPlayerPtr->EntityState);

    if (MH_EnableHook(reinterpret_cast<LPVOID>(m_AcState->m_DecreaseHealthFnPtr)) != MH_OK) {
        Logger::Error() << "[godmode] failed to enable dodamage() hook" << Logger::Endl;
        return;
    }
    else {
        Logger::Info() << "[godmode] enabled dodamage() hook" << Logger::Endl;
    }

    m_IsEnabled = true;
}

void Godmode::Disable() {
    if (MH_DisableHook(reinterpret_cast<LPVOID>(m_AcState->m_DecreaseHealthFnPtr)) != MH_OK) {
        Logger::Error() << "[godmode] failed to disable dodamage() hook" << Logger::Endl;
        return;
    }
    else {
        Logger::Info() << "[godmode] disabled dodamage() hook" << Logger::Endl;
    }

    m_IsEnabled = false;
}

void Godmode::Work() {
    if (g_Settings.m_EnableGodmode) {
        if (!m_IsEnabled)
            Enable();
    } else {
		if (m_IsEnabled)
			Disable();
    }
}

void Godmode::Shutdown() {
    MH_RemoveHook(reinterpret_cast<LPVOID>(m_AcState->m_DecreaseHealthFnPtr));
}
