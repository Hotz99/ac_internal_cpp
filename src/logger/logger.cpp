#include "logger.h"

Logger* Logger::Instance = nullptr;

bool Logger::IsActive;
FILE* Logger::ConsoleStream;
std::ofstream Logger::NullStream;

std::ostream& Logger::Print(LogLevel level) {
    Logger::CreateIfNotExist();

    if (Logger::IsActive && Logger::ConsoleStream) {
        return std::cout << Logger::GetTimestamp() << " - " << Logger::LogLevelToString(level) << " - ";
    }

    return Logger::NullStream;
}

//void Logger::Bytes(byte* bytes, unsigned int length) {
//    for (unsigned int i = 0; i < length; i++) {
//        std::cout << hex_format(bytes[i]) << " ";
//    }
//    std::cout << " ..." << std::endl;
//};

std::ostream& Logger::Endl(std::ostream& ostream) {
    return std::endl(ostream);
}

void Logger::SetActive(bool active) {
    Logger::CreateIfNotExist();

    if (Logger::IsActive == active) {
        return;
    }

    Logger::IsActive = active;
    if (Logger::IsActive) {
        Logger::AllocateConsole();
    }
    else {
        Logger::DeallocateConsole();
    }
}

void Logger::Destroy() {
    delete Logger::Instance;
}

void Logger::CreateIfNotExist() {
    if (!Logger::Instance) {
        Logger::Instance = new Logger();
    }
}

Logger::Logger() {
    Logger::IsActive = false;
    Logger::ConsoleStream = nullptr;
    Logger::NullStream.open("nul", std::ofstream::out | std::ofstream::app);
}

Logger::~Logger() {
    Logger::DeallocateConsole();
    Logger::NullStream.close();
    delete Logger::ConsoleStream;
}

void Logger::AllocateConsole() {
    if (Logger::ConsoleStream) {
        return;
    }

    AllocConsole();
    freopen_s(&Logger::ConsoleStream, "CONOUT$", "w", stdout);
}

void Logger::DeallocateConsole() {
    if (!Logger::ConsoleStream) {
        return;
    }

    FreeConsole();
    fclose(Logger::ConsoleStream);

    Logger::ConsoleStream = nullptr;
}

std::string Logger::GetTimestamp() {
    char buffer[20];
    tm locale_now = {};

    std::time_t now = std::time(nullptr);
    localtime_s(&locale_now, &now);

    strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", &locale_now);
    return std::string(buffer);
}

std::string Logger::LogLevelToString(LogLevel l) {
    switch (l) {
    case LOG_DEBUG:
        return "DEBUG";
    case LOG_WARNING:
        return "WARN ";
    case LOG_ERROR:
        return "ERROR";
    default:
        return "INFO ";
    }
}
