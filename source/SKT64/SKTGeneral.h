#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include "SKT64.h"
#include "DriverInterface.h"
#include "LogService.h"
#include "KiService.h"

namespace GerneralService
{
	namespace KernelModuleLoader {
		bool LoadDriver(LPCWSTR driverPath, LPCWSTR serviceName);
		void SafeCloseServiceHandle(SC_HANDLE& handle);
		bool UnloadDriver(LPCWSTR serviceName);
		bool CheckServiceStatus(LPCWSTR serviceName);
	}
	BOOL ReleaseCoreDriver();
	std::string WStringToUTF8(const std::wstring& wstr);
}