#pragma once
#include <windows.h>
#include "../game/ac_functions.h"
#include "../game/ac_state.h"
#include "../geometry/geometry.h"
#include "../logger/logger.h"
#include "./settings.h"

class Aimbot {
private:
    AcState& m_AcState;
    AcEntity* GetNextTarget();

    geometry::Vector3 ComputeAimAngle(AcEntity* entity);
    void AimAtEntity(AcEntity* entity);

    bool IsVisible(AcEntity* entity);

public:
    Aimbot() 
        : m_AcState(AcState::GetInstance())
    {}

    static Aimbot& GetInstance() {
        static Aimbot s_Instance;
		return s_Instance;
	}

    void Work();
};

