#pragma once
#include "hack.h"
#include "../memory/hook.h"

class Health : public Hack {
private:
    memory::Hook* m_DoDamageHook;

public:
    Health();
    ~Health();

    void Activate();
    void Deactivate();
    void Tick();
};
