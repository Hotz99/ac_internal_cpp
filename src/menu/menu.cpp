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

    SDLUtils::GetInstance().SetRelativeMouseMode(!m_IsOpen);
}

void Menu::Render() {
    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false)) {
        Logger::Debug() << "[menu] menu key pressed" << Logger::Endl;
        Toggle(!m_IsOpen);
    }

    if (!m_IsOpen) {
        return;
    }
    
    // TODO should be `#define` in header ?
    constexpr auto windowStartWidth = 256.f;

    ImGui::SetNextWindowSize(ImVec2 { windowStartWidth, 0 }, ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize * 0.5f, ImGuiCond_Once, ImVec2 { 0.5f, 0.5f });
    ImGui::Begin("main menu", nullptr);
    {
        ImGui::SeparatorText("Aimbot");
        ImGui::Checkbox("Aimbot", &g_Settings.m_EnableAimbot);

        ImGui::SeparatorText("Visuals");
        ImGui::Checkbox("Master switch", &g_Settings.m_EnableESP);
        ImGui::Checkbox("Player names", &g_Settings.m_PlayerNames);
        ImGui::Checkbox("Player boxes", &g_Settings.m_PlayerBoxes);
        ImGui::Checkbox("Player health bar", &g_Settings.m_PlayerHealthBar);
        ImGui::Checkbox("Player snaplines", &g_Settings.m_PlayerSnaplines);

        ImGui::SeparatorText("Misc");
        ImGui::Checkbox("Godmode", &g_Settings.m_EnableGodmode);
        
        ImGui::Separator();

        if (ImGui::Button("Rescan Signatures")) {
            Logger::Debug() << "[menu] rescan signatures button pressed" << Logger::Endl;
            // TODO is this nasty coupling ?
            AcState::GetInstance().ScanForSignatures();
        }
    }
    ImGui::End();
}

void Menu::Shutdown() {
    Toggle(false);
}