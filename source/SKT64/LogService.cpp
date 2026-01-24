#include "LogService.h"

namespace LogService
{
    void setConsoleColor(LogColor color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
    }

    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm;
        localtime_s(&tm, &in_time_t);

        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);

        char timeStr[84];
        sprintf_s(timeStr, "%s.%03d", buffer, static_cast<int>(ms.count()));

        return timeStr;
    }

    void printfLog(const char* format, LogColor color, ...)
    {
        setConsoleColor(LogColor::LOG_YELLOW);
        std::cout << "[" << getCurrentTime() << "] ";
        setConsoleColor(color);
        va_list args;
        va_start(args, color);
        vprintf(format, args);
        va_end(args);
        setConsoleColor(LogColor::LOG_DEFAULT);
    }
}