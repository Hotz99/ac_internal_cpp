#pragma once
#include <windows.h>
#include "../logger/logger.h"

// IDA Pro signature format uses `?` as wildcard byte
#define SIGNATURE_WILDCARD_BYTE 0xFF
#define SIGNATURE_MAX_SIZE 255

struct Signature_t {
    // TODO should we use `std::string` instead of `const char*` ?
    const char* pattern;
    int addressByteOffset;
    uintptr_t address = NULL;

    int length = 0;
    char bytes[SIGNATURE_MAX_SIZE] = {};
    char byteMask[SIGNATURE_MAX_SIZE] = {};

    // constructor delegation
    Signature_t(const char* pattern) : Signature_t(pattern, 0) {};
    Signature_t(const char* pattern, int addressByteOffset);

private:
    void AddByte(int byte, char mask);
    int Decode(const char* input);
};

class SignatureScanner {
private:
    bool CheckSignature(uintptr_t address, Signature_t* signature);

    uintptr_t m_ModuleBaseAddress;
    int m_ModuleSize;
public:
    SignatureScanner(uintptr_t moduleBaseAddress, int moduleSize);
    ~SignatureScanner() {};

    bool Scan(Signature_t signatures[], int signatureCount);
};