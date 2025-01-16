#pragma once

struct Settings_t {
    bool m_EnableAimbot;
    bool m_EnableESP;
    bool m_EnableGodmode;

    bool m_RescanSignatures;

    bool m_PlayerNames;
    bool m_PlayerBoxes;
    bool m_PlayerHealthBar;
    bool m_PlayerSnaplines;
};

// `extern` declares a variable as existing without defining or allocating memory for it
// tells the compiler this variable exists somewhere else (in a .cpp file)
extern Settings_t g_Settings;