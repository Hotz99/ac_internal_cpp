#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>

// macro to convert pointer to void* for logging
#define LOG_PTR(ptr) reinterpret_cast<void*>(ptr)

struct HexCharStruct {
    unsigned char c;
    HexCharStruct(unsigned char _c) : c(_c) {}
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs) {
    return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex_format(unsigned char _c) {
    return HexCharStruct(_c);
}

enum LogLevel {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

class Logger {
public:
    static std::ostream& Print(LogLevel level);
    static std::ostream& Debug() { return Logger::Print(LOG_DEBUG); };
    static std::ostream& Info() { return Logger::Print(LOG_INFO); };
    static std::ostream& Warning() { return Logger::Print(LOG_WARNING); };
    static std::ostream& Error() { return Logger::Print(LOG_ERROR); };

    static std::ostream& Endl(std::ostream& ostream);

    // TODO fix byte definition import
    //static void Bytes(byte* bytes, unsigned int length);

    static void SetActive(bool active);
    static void Destroy();

protected:
    Logger();
    ~Logger();

    static void CreateIfNotExist();

    static void AllocateConsole();
    static void DeallocateConsole();

    static std::string GetTimestamp();
    static std::string LogLevelToString(LogLevel l);

    static Logger* Instance;

    static bool IsActive;
    static FILE* ConsoleStream;
    static std::ofstream NullStream;
};