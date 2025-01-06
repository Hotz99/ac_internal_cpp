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

// TODO should we set members to `nullptr` ?
AcState::~AcState() {}

AcState& AcState::GetInstance() {
    static AcState instance;
    return instance;
}

void AcState::Initialize() {
    AcState::ScanForSignatures();
}

bool AcState::ScanForSignatures() {
    Signature signatures[] = {
        // sigNoRecoil
        Signature { "83 EC ? 53 55 8B 6C ? ? 56 57 8B F9" },
        // sigDecreaseAmmo
        Signature { "FF 08 8D 44" },
        // sigDecreaseHealth
        Signature { "2B F1 29 73", 2 },
        // sigIntersectClosest
        Signature { "83 EC ? A1 ? ? ? ? ? ? ? ? 24" },
        // sigIntersectGeometry
        Signature { "55 8B EC 83 E4 ? 81 EC ? ? ? ? 53 8B DA 8B D1" },
        // sigGameMode
        Signature { "89 15 ? ? ? ? 53", 2 },
        // sigMatrix
        Signature { "F3 0F ? ? ? ? ? ? F3 0F ? ? 0F 28 ? 0F C6 C3 ? F3 0F ? ? ? ? ? ? F3 0F ? ? F3 0F ? ? F2 0F ? ? ? ? ? ? 0F 28 ? 0F 54 ? ? ? ? ? 0F 5A ? 66 0F ? ? 77 ? F3 0F", 4 },
        // sigLocalPlayer
        Signature { "8B 0D ? ? ? ? 56 57 8B 3D", 2 },
        // sigEntityList
        Signature { "A1 ? ? ? ? ? ? ? ? F6 0F 84 5F", 1 },
        // sigPlayerCount
        Signature { "8B 0D ? ? ? ? 46 3B ? 7C ? 8B 35", 2 },
    };

    SignatureScanner* scanner = new SignatureScanner(m_ModuleBaseAddress, m_ModuleSize);
    
    // `signatures` is implicitly converted to pointer to first element of array
    // when we pass an array to a function, it decays to a pointer
    // hence we can't `sizeof()` the array inside the function to correctly compute the size
    // it will return the size of a pointer
    int signatureCount = sizeof(signatures) / sizeof(Signature);

    if (!scanner->Scan(signatures, signatureCount)) {
        Logger::Error() << "[ac_state] failed to resolve some signatures" << Logger::Endl;
        return false;
    }

    Logger::Info() << "[ac_state] found NoRecoil signature at 0x" << (void*)signatures[0].address << " ..." << Logger::Endl;
    NoRecoil = signatures[0].address;

    Logger::Info() << "[ac_state] found DecreaseAmmo signature at 0x" << (void*)signatures[1].address << " ..." << Logger::Endl;
    DecreaseAmmo = signatures[1].address;

    Logger::Info() << "[ac_state] found DecreaseHealth signature at 0x" << (void*)signatures[2].address << " ..." << Logger::Endl;
    DecreaseHealth = signatures[2].address;

    Logger::Info() << "[ac_state] found IntersectClosest signature at 0x" << (void*)signatures[3].address << " ..." << Logger::Endl;
    IntersectClosest = signatures[3].address;

    Logger::Info() << "[ac_state] found IntersectGeometry signature at 0x" << (void*)signatures[4].address << " ..." << Logger::Endl;
    IntersectGeometry = signatures[4].address;

    Logger::Info() << "[ac_state] found GameMode signature at 0x" << (void*)signatures[5].address << " ..." << Logger::Endl;
    // TODO employ `reinterpret_cast` instead of C-style casts
    GameMode = (int*)(*(uintptr_t*)signatures[5].address);

    Logger::Info() << "[ac_state] found Matrix signature at 0x" << (void*)signatures[6].address << " ..." << Logger::Endl;
    Matrix = (float*)(*(uintptr_t*)signatures[6].address);

    Logger::Info() << "[ac_state] found LocalPlayer signature at 0x" << (void*)signatures[7].address << " ..." << Logger::Endl;
    LocalPlayer = *(AcEntity**)(*(uintptr_t*)signatures[7].address);

    Logger::Info() << "[ac_state] found EntityList signature at 0x" << (void*)signatures[8].address << " ..." << Logger::Endl;
    EntityList = *(AcEntityList**)(*(uintptr_t*)signatures[8].address);

    Logger::Info() << "[ac_state] found PlayerCount signature at 0x" << (void*)signatures[9].address << " ..." << Logger::Endl;
    PlayerCount = (int*)(*(uintptr_t*)signatures[9].address);
    return true;
}