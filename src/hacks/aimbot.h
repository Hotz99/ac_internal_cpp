#pragma once
#include <windows.h>
#include "./hack.h"
#include "../game/ac_functions.h"
#include "../geometry/geometry.h"
#include "../logger/logger.h"
#include "./settings.h"

// public inheritance: members of `Hack` maintain their access levels in `Aimbot`
// as they were in `Hack`
class Aimbot : public Hack {
private:
    AcEntity* GetNextTarget();

    geometry::Vector3 ComputeAimAngle(AcEntity* entity);
    void AimAtEntity(AcEntity* entity);

    bool IsVisible(AcEntity* entity);

public:
    static Aimbot& GetInstance() {
		static Aimbot instance;
		return instance;
	}

    void Work();
};

