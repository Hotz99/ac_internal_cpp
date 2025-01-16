#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <thread>
#include <vector>

#include "../libs/minhook/minhook.h"
#include "../libs/imgui/imgui.h"
#include "../libs/imgui/imgui_impl_win32.h"
#include "../libs/imgui/imgui_impl_opengl3.h"
#include "./hooks/wndproc_hook.h"
#include "./menu/menu.h"
#include "./hacks/esp.h"
#include "./hacks/aimbot.h"
#include "./logger/logger.h"
#include "./game/ac_state.h"

bool terminateProgram = false;

// typedef for function pointer to original `wglSwapBuffers()`
// matches the original signature, since we want to replace it with our detour
// same return type, calling convention and parameters
typedef BOOL(__stdcall* wglSwapBuffers_t) (HDC hDeviceContext);
wglSwapBuffers_t wglSwapBuffersTrampoline;

// if we call the original function in our detour, we execute its function body from start
// our detour jmp will eventually be executed again, resulting in an infinite loop
// hence we jmp to trampoline function after executing our detour
// from trampoline function we jmp to original function
// to the byte AFTER jmp instruction to our detour, hence no infinite loop
BOOL __stdcall wglSwapBuffersHookPayload(HDC hDeviceContext)
{
    if (GetAsyncKeyState(VK_DELETE) & 1) {
        // objects are deleted in worker thread
        terminateProgram = true;
    }

    if (ImGui::GetCurrentContext()) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Menu::GetInstance().Render();

        // TODO draw onto imgui overlay instead of using opengl ?
        opengl::SetupOrtho();
        ESP::GetInstance().Render();
        opengl::RestoreGL();

        Aimbot::GetInstance().Work();
        Godmode::GetInstance().Work();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // call trampoline to the original function
    return wglSwapBuffersTrampoline(hDeviceContext);
}

DWORD __stdcall WorkerLogic(HMODULE hModule) {
    Logger::SetActive(true);

    AcState::GetInstance().Initialize();

    // hook target process's `wndProc` to intercept Windows messages
    // creates imgui context and initializes imgui impl win32 and opengl3
    WndProcHook::GetInstance().Initialize();

    HMODULE hOpenGl = nullptr;

    // wait for `opengl32.dll` to be loaded by target process
    while (!hOpenGl) {
		hOpenGl = GetModuleHandle("opengl32.dll");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

    // pointer to first instruction of original function's byte code
    void* wglSwapBuffers = GetProcAddress(hOpenGl, "wglSwapBuffers");
    if (MH_Initialize() != MH_OK) {
        Logger::Error() << "[dll_main] failed to initialize MinHook" << Logger::Endl;
		return 1;
	}
    
    // creates a hook for the original `wglSwapBuffers()`:
    // overwrites original function instructions with a jmp to our detour
    // copies these instructions to trampoline function
    // 1. original function jmp to hook payload
    // 2. hook payload executes then jmp to trampoline function
    // 3. trampoline function executes copied original function's instructions (stolen bytes), maintaining original flow
    // 4. trampoline function jmp to original function, to byte after jmp to hook payload

    // the third arg is a pointer to trampoline function pointer
    // `&` yields the address of the function pointer, resulting in a `void**`
    // then cast to `LPVOID*` (Windows typedef for `void*`) as required by `MH_CreateHook()`
    if (MH_CreateHook(wglSwapBuffers, wglSwapBuffersHookPayload, reinterpret_cast<LPVOID*>(&wglSwapBuffersTrampoline)) != MH_OK)
    {
		Logger::Error() << "[dll_main] failed to create wglSwapBuffers hook" << Logger::Endl;
		return 1;
    }
    else {
        Logger::Info() << "[dll_main] created wglSwapBuffers hook" << Logger::Endl;
    }

    if (MH_EnableHook(wglSwapBuffers) != MH_OK)
    {
        Logger
            ::Error() << "[dll_main] failed to enable wglSwapBuffers hook" << Logger::Endl;
        return 1;
    } else {
        Logger::Info() << "[dll_main] enabled wglSwapBuffers hook" << Logger::Endl;
	}


    while (!terminateProgram) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

    auto disableHookStatus = MH_DisableHook(wglSwapBuffers);

    if (disableHookStatus != MH_OK) {
		Logger::Error() << "[dll_main] failed to disable hook for wglSwapBuffers" << Logger::Endl;
		return 1;
	}

    if (MH_RemoveHook(wglSwapBuffers) != MH_OK) {
        Logger::Error() << "[dll_main] failed to remove hook for wglSwapBuffers" << Logger::Endl;
        return 1;
    }

    Logger::Shutdown();

    WndProcHook::GetInstance().Shutdown();
    Menu::GetInstance().Shutdown();
    // only `godmode` has hooks to remove, `esp` and `aimbot` don't
    Godmode::GetInstance().Shutdown();

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

// library entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved) {
    if (dwReasonForCall == DLL_PROCESS_ATTACH) {
        HANDLE hWorkerThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)WorkerLogic, hModule, 0, nullptr);

        if (hWorkerThread)
            CloseHandle(hWorkerThread);
    }

    return TRUE;
}