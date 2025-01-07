#include "./ac_functions.h"

// wrapper for original `intersectgeometry` function at `m_IntersectGeometryFnPtr`:
// int __fastcall IntersectGeometry(float* a1, int WorldPos) 
void ac_functions::IntersectGeometry(const ac_structs::vec& from, ac_structs::vec& to) {
    DWORD intersectGeometryFnAddr = AcState::GetInstance().m_IntersectGeometryFn;

    void* pFrom = (void*)&from;
    void* pTo = (void*)&to;

    __asm {
        mov ecx, pFrom
        mov edx, pTo
        call intersectGeometryFnAddr
    }
}