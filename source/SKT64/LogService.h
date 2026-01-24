#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <bitset>
#include <chrono>
#include <thread>


enum class LogColor {
    LOG_BLACK = 0,
    LOG_BLUE = 1,
    LOG_GREEN = 2,
    LOG_CYAN = 3,
    LOG_RED = 4,
    LOG_MAGENTA = 5,
    LOG_YELLOW = 6,
    LOG_WHITE = 7,
    LOG_DEFAULT = 7
};

namespace LogService
{
    void setConsoleColor(LogColor color);
    std::string getCurrentTime();
    void printfLog(const char* format, LogColor color = LogColor::LOG_DEFAULT, ...);
}