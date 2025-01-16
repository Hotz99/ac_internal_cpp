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
    if (ImGui::IsKeyPressed(ImGuiKey_Insert, false))
        Toggle(!m_IsOpen);

    if (!m_IsOpen) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2 { MENU_WIDTH, 0 }, ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize * 0.5f, ImGuiCond_Once, ImVec2 { 0.5f, 0.5f });
    ImGui::Begin("main menu", nullptr);
    {
        ImGui::Checkbox("aimbot", &g_Settings.m_EnableAimbot);

        ImGui::SeparatorText("VISUALS");
        ImGui::Checkbox("enable ESP", &g_Settings.m_EnableESP);
        ImGui::Checkbox("player names", &g_Settings.m_PlayerNames);
        ImGui::Checkbox("player boxes", &g_Settings.m_PlayerBoxes);
        ImGui::Checkbox("player health bar", &g_Settings.m_PlayerHealthBar);
        ImGui::Checkbox("player snaplines", &g_Settings.m_PlayerSnaplines);

        ImGui::SeparatorText("MISC");
        ImGui::Checkbox("godmode", &g_Settings.m_EnableGodmode);
        
        ImGui::Separator();

        if (ImGui::Button("rescan signatures")) {
            // TODO is this nasty coupling ?
            AcState::GetInstance().ScanForSignatures();
        }
    }
    ImGui::End();
}

void Menu::Shutdown() {
    Toggle(false);
}