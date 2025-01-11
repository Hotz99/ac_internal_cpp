#include "./ac_state.h"

AcState::AcState() {
    // TODO understand this cast
    m_ModuleBaseAddress = reinterpret_cast<uintptr_t>(GetModuleHandle("ac_client.exe"));

    if (!m_ModuleBaseAddress) {
		Logger::Error() << "[ac_state] failed to resolve module base address" << Logger::Endl;
    }
    else {
        Logger::Debug() << "[ac_state] module base address: 0x" << m_ModuleBaseAddress << Logger::Endl;
    }

    MODULEINFO moduleInfo = {};
    GetModuleInformation(GetCurrentProcess(), (HMODULE)m_ModuleBaseAddress, &moduleInfo, sizeof(moduleInfo));
    if (!moduleInfo.SizeOfImage) {
        Logger::Error() << "[ac_state] failed to resolve module size" << Logger::Endl;
    }
    else {
		m_ModuleSize = moduleInfo.SizeOfImage;
    }
}

void AcState::Initialize() {
    AcState::ScanForSignatures();
}

bool AcState::ScanForSignatures() {
    Signature_t signatures[] = {
        // sigNoRecoil
        Signature_t { "83 EC ? 53 55 8B 6C ? ? 56 57 8B F9" },
        // sigDecreaseAmmo
        Signature_t { "FF 08 8D 44" },
        // sigDecreaseHealth
        Signature_t { "2B F1 29 73", 2 },
        // sigIntersectClosest
        Signature_t { "83 EC ? A1 ? ? ? ? ? ? ? ? 24" },
        // sigIntersectGeometry
        Signature_t { "55 8B EC 83 E4 ? 81 EC ? ? ? ? 53 8B DA 8B D1" },
        // sigGameMode
        Signature_t { "89 15 ? ? ? ? 53", 2 },
        // sigMatrix
        Signature_t { "F3 0F ? ? ? ? ? ? F3 0F ? ? 0F 28 ? 0F C6 C3 ? F3 0F ? ? ? ? ? ? F3 0F ? ? F3 0F ? ? F2 0F ? ? ? ? ? ? 0F 28 ? 0F 54 ? ? ? ? ? 0F 5A ? 66 0F ? ? 77 ? F3 0F", 4 },
        // sigLocalPlayer
        Signature_t { "8B 0D ? ? ? ? 56 57 8B 3D", 2 },
        // sigEntityList
        Signature_t { "A1 ? ? ? ? ? ? ? ? F6 0F 84 5F", 1 },
        // sigPlayerCount
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

    Logger::Info() << "[ac_state] found NoRecoilFn signature at 0x" << (void*)signatures[0].address << Logger::Endl;
    m_NoRecoilFnPtr = signatures[0].address;

    Logger::Info() << "[ac_state] found DecreaseAmmoFn signature at 0x" << (void*)signatures[1].address << Logger::Endl;
    m_DecreaseAmmoFnPtr = signatures[1].address;

    Logger::Info() << "[ac_state] found DecreaseHealthFn signature at 0x" << (void*)signatures[2].address << Logger::Endl;
    m_DecreaseHealthFnPtr = signatures[2].address;

    Logger::Info() << "[ac_state] found IntersectClosestFn signature at 0x" << (void*)signatures[3].address << Logger::Endl;
    m_IntersectClosestFnPtr = signatures[3].address;

    Logger::Info() << "[ac_state] found IntersectGeometryFn signature at 0x" << (void*)signatures[4].address << Logger::Endl;
    m_IntersectGeometryFnPtr = signatures[4].address;

    Logger::Info() << "[ac_state] found GameModeFn signature at 0x" << (void*)signatures[5].address << Logger::Endl;
    // TODO employ `reinterpret_cast` instead of C-style casts ?
    m_GameModePtr = (int*)(*(uintptr_t*)signatures[5].address);

    Logger::Info() << "[ac_state] found ViewMatrixFn signature at 0x" << (void*)signatures[6].address << Logger::Endl;
    m_ViewMatrixPtr = (float*)(*(uintptr_t*)signatures[6].address);

    Logger::Info() << "[ac_state] found LocalPlayerPtr signature at 0x" << (void*)signatures[7].address << Logger::Endl;
    m_LocalPlayerPtr = *(AcEntity**)(*(uintptr_t*)signatures[7].address);

    Logger::Info() << "[ac_state] found EntityListPtr signature at 0x" << (void*)signatures[8].address << Logger::Endl;
    m_EntityListPtr = *(AcEntityList**)(*(uintptr_t*)signatures[8].address);

    Logger::Info() << "[ac_state] found PlayerCountPtr signature at 0x" << (void*)signatures[9].address << Logger::Endl;
    m_PlayerCountPtr = (int*)(*(uintptr_t*)signatures[9].address);
    return true;
}

void AcState::Destroy() {
	// calls destructor
    delete this;
    Logger::Debug() << "[ac_state] destroyed" << Logger::Endl;
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