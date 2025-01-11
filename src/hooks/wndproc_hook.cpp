#include "./wndproc_hook.h"

// hook target process's `wndProc` to intercept Windows messages 
// such as input events (WM_KEYDOWN, WM_MOUSEMOVE)
// and pass them to imgui for handling

static WNDPROC g_WndProc;
static std::once_flag g_InputInit;

static LRESULT WndProcHookPayload(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    std::call_once(g_InputInit, [hWnd]() {
        ImGui::CreateContext();
        Logger::Info() << "[wndproc] imgui context created" << Logger::Endl;
        ImGui_ImplWin32_Init(hWnd);
        Logger::Info() << "[wndproc] imgui win32 initialized" << Logger::Endl;
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = io.LogFilename = nullptr;
        ImGui_ImplOpenGL3_Init();
        Logger::Info() << "[wndproc] imgui opengl3 initialized" << Logger::Endl;
        });

    LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
    if (Menu::GetInstance().IsOpen()) {
        // messages handled by `ImGui_ImplWin32_WndProcHandler` are not passed to the original `wndProc`
        switch (uMsg) {
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_CHAR:
        case WM_SETCURSOR:
        case WM_DEVICECHANGE:
            return 1;
        }
    }

    // pass remaining messages to original `wndProc`
    return CallWindowProc(g_WndProc, hWnd, uMsg, wParam, lParam);
}

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
    const auto isMainWindow = [handle]() {
        return GetWindow(handle, GW_OWNER) == nullptr && IsWindowVisible(handle) && handle != GetConsoleWindow();
        };

    DWORD processId = 0;
    GetWindowThreadProcessId(handle, &processId);

    if (GetCurrentProcessId() != processId || !isMainWindow()) {
        return TRUE;
    }

    *reinterpret_cast<HWND*>(lParam) = handle;
    return FALSE;
}

void WndProcHook::Initialize() {
    // resolve main window handle of target process
    EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&m_WindowPtr));
    if (!m_WindowPtr) {
        Logger::Error() << "[wndproc] EnumWindows() returned nullptr" << Logger::Endl;
        return;
    }

    Logger::Info() << "[wndproc] WndProcHooks::m_Window -> " << m_WindowPtr << Logger::Endl;
    
    // hook target process's `wndProc` to intercept Windows messages
    g_WndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(m_WindowPtr, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHookPayload)));

    if (!g_WndProc) {
		Logger::Error() << "[wndproc] SetWindowLongPtr() returned nullptr" << Logger::Endl;
    }
    else {
        Logger::Info() << "[wndproc] WndProcHooks::g_WndProc -> " << g_WndProc << Logger::Endl;
    }
}

void WndProcHook::Shutdown() {
    if (g_WndProc) {
        SetWindowLongPtr(m_WindowPtr, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_WndProc));
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplOpenGL3_Shutdown();
    }
}
