#pragma once
#include <windows.h>
#include <psapi.h>
#include <iostream>

#include "../logger/logger.h"
#include "../sig_scanner/sig_scanner.h"
#include "./ac_structs.h"


class AcState {
private:
    // `std::nullptr_t` is a type in C++ that represents a null pointer, not a number
    // `uintptr_t` is an integer type meant to hold pointer values
    // `NULL` is a macro that expands to `0` or `nullptr`
    // hence we can assign `NULL` to `uintptr_t` OR assign a `std::nullptr_t` cast to `uintptr_t`
    uintptr_t m_ModuleBaseAddress = NULL;
    int m_ModuleSize = 0;
public:
    // ctors and dtors must have implementation, otherwise linker error
    AcState();
    ~AcState();

    static AcState& GetInstance() {
        // `static` ensures the instance lifetime is the same as the program
        static AcState instance;
        return instance;
    }

    void Initialize();

    bool ScanForSignatures();

    void Destroy();

    bool IsTeamGame();
    bool IsEnemy(AcEntity* entity);
    bool IsValidEntity(AcEntity* entity);

    uintptr_t m_NoRecoilFnPtr = NULL;
    uintptr_t m_DecreaseAmmoFnPtr = NULL;
    uintptr_t m_DecreaseHealthFnPtr = NULL;
    uintptr_t m_IntersectClosestFnPtr = NULL;
    uintptr_t m_IntersectGeometryFnPtr = NULL;

    int* m_GameModePtr = nullptr;
    int* m_PlayerCountPtr = nullptr;
    // `ViewMatrix` is a 4x4 array of floats, below is a reference to the first element 
    float* m_ViewMatrixPtr = nullptr;
    AcEntity* m_LocalPlayerPtr = nullptr;
    AcEntityList* m_EntityListPtr = nullptr;
};

