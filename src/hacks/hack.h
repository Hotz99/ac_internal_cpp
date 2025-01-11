#pragma once
#include "../game/ac_state.h"

typedef void(*ActiveChangedCallback)(bool);

class Hack {
protected:
    void CallActiveChangedCallback() {
        if (m_Callback) {
            m_Callback(m_IsEnabled);
        }
    };

    ActiveChangedCallback m_Callback;
    AcState* m_AcState;
    bool m_IsEnabled;

public:
    Hack() : Hack(false, nullptr) {};
    Hack(bool isActive, ActiveChangedCallback callback) {
        m_IsEnabled = isActive;
        m_Callback = callback;
        m_AcState = &AcState::GetInstance();
    };
    ~Hack() {
        Disable();
        m_AcState = nullptr;
    };
    virtual void Enable() {
        m_IsEnabled = true;
        CallActiveChangedCallback();
    };
    virtual void Disable() {
        m_IsEnabled = false;
        CallActiveChangedCallback();
    };
    bool IsActive() {
        return m_IsEnabled;
    };
    virtual void Tick() {};
};

