#pragma once
#include <windows.h>
#include <psapi.h>
#include <iostream>

#include "../logger/logger.h"
#include "../sig_scanner/sig_scanner.h"
#include "./ac_structs.h"


class AcState {
private:
    // ctors and dtors must have implementation, otherwise linker error
    AcState();
    ~AcState();

    bool CheckReady();
    void LoadModules();
    bool ScanForSignatures();

    // `std::nullptr_t` is a type in C++ that represents a null pointer, not a number
    // `uintptr_t` is an integer type meant to hold pointer values
    // `NULL` is a macro that expands to `0` or `nullptr`
    // hence we can assign `NULL` to `uintptr_t` OR assign a `std::nullptr_t` cast to `uintptr_t`
    uintptr_t m_ModuleBaseAddress = NULL;
    int m_ModuleSize = 0;
public:
    static AcState& GetInstance();
    void Initialize();
    static void Reload();
    static void Destroy();

    bool IsReady();

    bool IsTeamGame();
    bool IsEnemy(AcEntity* entity);
    bool IsValidEntity(AcEntity* entity);

    uintptr_t ModuleBase = NULL;

    uintptr_t NoRecoil = NULL;
    uintptr_t DecreaseAmmo = NULL;
    uintptr_t DecreaseHealth = NULL;
    uintptr_t IntersectClosest = NULL;
    uintptr_t IntersectGeometry = NULL;

    int* GameMode = nullptr;
    int* PlayerCount = nullptr;
    float* Matrix = nullptr;
    AcEntity* LocalPlayer = nullptr;
    AcEntityList* EntityList = nullptr;
};

