#pragma once
#include "../game/ac_state.h"

typedef void(*ActiveChangedCallback)(bool);

class Hack {
protected:
    void CallActiveChangedCallback() {
        if (m_Callback) {
            m_Callback(m_IsActive);
        }
    };

    ActiveChangedCallback m_Callback;
    AcState* m_AcState;
    bool m_IsActive;

public:
    Hack() : Hack(false, nullptr) {};
    Hack(bool isActive, ActiveChangedCallback callback) {
        m_IsActive = isActive;
        m_Callback = callback;
        m_AcState = &AcState::GetInstance();
    };
    ~Hack() {
        Deactivate();
        m_AcState = nullptr;
    };
    virtual void Activate() {
        m_IsActive = true;
        CallActiveChangedCallback();
    };
    virtual void Deactivate() {
        m_IsActive = false;
        CallActiveChangedCallback();
    };
    bool IsActive() {
        return m_IsActive;
    };
    virtual void Tick() {};
};

