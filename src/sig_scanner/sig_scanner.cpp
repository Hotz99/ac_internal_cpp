#include "sig_scanner.h"

// incorrect way to delegate constructors
// creates a temporary `Signature` object, which is dropped when constructor function scope ends
//Signature::Signature(const char* pattern) {
//    Signature(pattern, 0);
//}

Signature_t::Signature_t(const char* pattern, int addressByteOffset) {
    this->pattern = pattern;
    this->addressByteOffset = addressByteOffset;

    for (int i = 0, charCount = 0; pattern[i] != '\0'; i++) {
        if (pattern[i] == ' ') {
            continue;
        }

        if (pattern[i] == '?') {
            AddByte(SIGNATURE_WILDCARD_BYTE, '?');
            continue;
        }

        // a byte in hexadecimal notation is 2 characters
        if (++charCount == 2) {
            AddByte(Decode(pattern + i - 1), 'x');
            charCount = 0;
        }
    }
}

void Signature_t::AddByte(int byte, char mask) {
    bytes[length] = byte;
    byteMask[length] = mask;
    length++;
}

int Signature_t::Decode(const char* byteCharPair) {
    int out, i, t, hn, ln;
    for (t = 0, i = 0; i < 2; i += 2, ++t) {
        hn = byteCharPair[i] > '9' ? byteCharPair[i] - 'A' + 10 : byteCharPair[i] - '0';
        ln = byteCharPair[i + 1] > '9' ? byteCharPair[i + 1] - 'A' + 10 : byteCharPair[i + 1] - '0';
        ((int*)&out)[t] = (hn << 4) | ln;
    }
    return out;
}

SignatureScanner::SignatureScanner(uintptr_t moduleBaseAddress, int moduleSize) {
    m_ModuleBaseAddress = moduleBaseAddress;
    m_ModuleSize = moduleSize;
}


bool SignatureScanner::Scan(Signature_t signatures[], int signatureCount) {
    int foundCount = 0;
    
    for (int memIdx = 0; memIdx < m_ModuleSize; memIdx++) {
        for (int sigIdx = 0; sigIdx < signatureCount; sigIdx++) {
            if (signatures[sigIdx].address != NULL)
                continue;

            if (CheckSignature(m_ModuleBaseAddress + memIdx, &signatures[sigIdx]))
                foundCount++;
        }
    }

    return foundCount == signatureCount;
}

bool SignatureScanner::CheckSignature(uintptr_t address, Signature_t* signature) {
    for (int signatureIndex = 0; signatureIndex < signature->length; signatureIndex++) {
        if (signature->byteMask[signatureIndex] == '?') {
            continue;
        }
        if (signature->bytes[signatureIndex] != *(char*)(address + signatureIndex)) {
            return false;
        }
    }

    signature->address = address + signature->addressByteOffset;
    return true;
}

