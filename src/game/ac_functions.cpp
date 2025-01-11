#include "./ac_functions.h"

// wrapper for original `intersectgeometry` function at `m_IntersectGeometryFnPtr`:
// int __fastcall IntersectGeometry(float* a1, int WorldPos) 
void ac_functions::IntersectGeometry(const ac_structs::vec& from, ac_structs::vec& to) {
    DWORD intersectGeometryFnAddr = AcState::GetInstance().m_IntersectGeometryFnPtr;

    void* fromPtr = (void*)&from;
    void* toPtr = (void*)&to;

    // MSVS inline assembly syntax
    // for clang or gcc: https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
    __asm {
        mov ecx, fromPtr
        mov edx, toPtr
        call intersectGeometryFnAddr
    }
}