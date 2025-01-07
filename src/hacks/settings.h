#pragma once

struct Settings_t {
    bool m_EnableAimbot;
    bool m_EnableTriggerbot;
    bool m_EnableGodmode;
    bool m_EnableESP;

    bool m_RescanSignatures;

    bool m_PlayerNames;
    bool m_PlayerBoxes;
    bool m_PlayerHealthBar;
    bool m_PlayerSnaplines;
};

extern Settings_t g_Settings;