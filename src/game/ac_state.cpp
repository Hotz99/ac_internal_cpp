#include "./ac_state.h"

AcState::AcState() {
    // TODO understand this cast
    m_ModuleBaseAddress = reinterpret_cast<uintptr_t>(GetModuleHandle("ac_client.exe"));

    if (!m_ModuleBaseAddress)
		Logger::Error() << "[ac_state] failed to resolve module base address" << Logger::Endl;

    MODULEINFO moduleInfo = {};
    GetModuleInformation(GetCurrentProcess(), (HMODULE)m_ModuleBaseAddress, &moduleInfo, sizeof(moduleInfo));
    if (!moduleInfo.SizeOfImage) {
        Logger::Error() << "[ac_state] failed to resolve module size" << Logger::Endl;
        return;
    }
    
    m_ModuleSize = moduleInfo.SizeOfImage;
}

AcState::~AcState() {
    m_ModuleBaseAddress = NULL;
    m_ModuleSize = 0;

    m_NoRecoilFnPtr = NULL;
    m_DecreaseAmmoFnPtr = NULL;
    m_DecreaseHealthFnPtr = NULL;
    m_IntersectClosestFnPtr = NULL;
    m_IntersectGeometryFnPtr = NULL;
}

void AcState::Initialize() {
    AcState::ScanForSignatures();
}

bool AcState::ScanForSignatures() {
    Signature_t signatures[] = {
        // recoil function address
        Signature_t { "83 EC ? 53 55 8B 6C ? ? 56 57 8B F9" },
        // decrease ammo function address
        Signature_t { "FF 08 8D 44" },
        // `dodamage()` function address
        Signature_t { "2B F1 29 73", 2 },
        // `intersectClosest()` function address
        Signature_t { "83 EC ? A1 ? ? ? ? ? ? ? ? 24" },
        // `intersectGeometry()` function address
        Signature_t { "55 8B EC 83 E4 ? 81 EC ? ? ? ? 53 8B DA 8B D1" },
        // game mode pointer address
        Signature_t { "89 15 ? ? ? ? 53", 2 },
        // view matrix pointer address
        Signature_t { "F3 0F ? ? ? ? ? ? F3 0F ? ? 0F 28 ? 0F C6 C3 ? F3 0F ? ? ? ? ? ? F3 0F ? ? F3 0F ? ? F2 0F ? ? ? ? ? ? 0F 28 ? 0F 54 ? ? ? ? ? 0F 5A ? 66 0F ? ? 77 ? F3 0F", 4 },
        // local player pointer address
        Signature_t { "8B 0D ? ? ? ? 56 57 8B 3D", 2 },
        // entity list pointer address
        Signature_t { "A1 ? ? ? ? ? ? ? ? F6 0F 84 5F", 1 },
        // player count pointer address
        Signature_t { "8B 0D ? ? ? ? 46 3B ? 7C ? 8B 35", 2 },
    };

    SignatureScanner* scanner = new SignatureScanner(m_ModuleBaseAddress, m_ModuleSize);
    
    // `signatures` is implicitly converted to pointer to first element of array
    // when we pass an array to a function, it decays to a pointer
    // hence we can't `sizeof()` the array inside the function to correctly compute the size
    // it will return the size of a pointer
    int signatureCount = sizeof(signatures) / sizeof(Signature_t);

    if (!scanner->Scan(signatures, signatureCount)) {
        Logger::Error() << "[ac_state] failed to resolve some signatures" << Logger::Endl;
        return false;
    }

    Logger::Debug() << "[ac_state] all signatures found" << Logger::Endl;

    m_NoRecoilFnPtr = signatures[0].address;

    m_DecreaseAmmoFnPtr = signatures[1].address;

    m_DecreaseHealthFnPtr = signatures[2].address;

    m_IntersectClosestFnPtr = signatures[3].address;

    m_IntersectGeometryFnPtr = signatures[4].address;

    // gpt said `reinterpret_cast` is explicit and type safe, hence being used over C-style casts, may be unnecessary ?
    m_GameModePtr = reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(signatures[5].address));

    m_ViewMatrixPtr = reinterpret_cast<float*>(*reinterpret_cast<uintptr_t*>(signatures[6].address));

    m_LocalPlayerPtr = *reinterpret_cast<AcEntity**>(*reinterpret_cast<uintptr_t*>(signatures[7].address));

    m_EntityListPtr = *reinterpret_cast<AcEntityList**>(*reinterpret_cast<uintptr_t*>(signatures[8].address));

    m_PlayerCountPtr = reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(signatures[9].address));
    
    return true;
}

void AcState::Destroy() {
	// `delete` calls `this`'s destructor
    delete this;
    Logger::Debug() << "[ac_state] destroyed" << Logger::Endl;
}

bool AcState::IsTeamGame() {
    return *m_GameModePtr == 0 || *m_GameModePtr == 4 || *m_GameModePtr == 5 || *m_GameModePtr == 7
        || *m_GameModePtr == 11 || *m_GameModePtr == 13 || *m_GameModePtr == 14 || *m_GameModePtr == 16
        || *m_GameModePtr == 17 || *m_GameModePtr == 20 || *m_GameModePtr == 21;
}

bool AcState::IsEnemy(AcEntity* entity) {
    return !IsTeamGame() || entity->Team != m_LocalPlayerPtr->Team;
}

bool AcState::IsValidEntity(AcEntity* entity) {
    return entity && entity->Health > 0;
}