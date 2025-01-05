#include "menu.h"

// re-use static global singleton
Menu& Menu::GetInstance() {
	static Menu instance;
	return instance;
}

bool Menu::IsOpen() {
	return m_IsOpen;
}

void Menu::Toggle(bool state) {
    m_IsOpen = state;

    Logger::Debug() << "menu is now " << (m_IsOpen ? "open" : "closed") << Logger::Endl;

    SDLUtils::GetInstance().SetRelativeMouseMode(!m_IsOpen);
    // TODO
    //auto inputSystem = InputSystem::GetInstance();

    //if (inputSystem && inputSystem->IsRelativeMouseMode()) {
    //    const ImVec2 screenCenter = ImGui::GetIO().DisplaySize * 0.5f;

    //    SDLUtils::GetInstance().SetRelativeMouseMode(!m_IsOpen);
    //    SDLUtils::GetInstance().SetWindowGrab(inputSystem->GetSDLWindow(), !m_IsOpen);
    //    SDLUtils::GetInstance().WarpMouseInWindow(nullptr, screenCenter.x, screenCenter.y);
    //}
}

void Menu::Render() {
    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false)) {
        Logger::Debug() << "[menu] menu key pressed" << Logger::Endl;
        Toggle(!m_IsOpen);
    }

    if (!m_IsOpen) {
        return;
    }
    
    ImGuiIO& IO = ImGui::GetIO();
    IO.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

    SDLUtils::GetInstance().SetRelativeMouseMode(false);

    constexpr auto windowStartWidth = 256.f;

    ImGui::SetNextWindowSize(ImVec2 { windowStartWidth, 0 }, ImGuiCond_Once);
    ImGui::SetNextWindowPos(IO.DisplaySize * 0.5f, ImGuiCond_Once, ImVec2 { 0.5f, 0.5f });
    ImGui::Begin("main menu", nullptr);
    {
        ImGui::SeparatorText("Visuals");
    }
    ImGui::End();
}

void Menu::Shutdown() {
    Toggle(false);
}