#include "SKTGeneral.h"



namespace GerneralService
{
	namespace KernelModuleLoader {
        bool LoadDriver(LPCWSTR driverPath, LPCWSTR serviceName) {
            SC_HANDLE scm = NULL;
            SC_HANDLE service = NULL;
            bool bSuccess = false;
            DWORD lastError = 0;

            if (!driverPath || !serviceName || wcslen(driverPath) == 0 || wcslen(serviceName) == 0) {
                LogService::printfLog("[LoadDriver] Invalid parameters: driverPath=%p, serviceName=%p\n",
                    LogColor::LOG_RED, driverPath, serviceName);
                return false;
            }

            DWORD fileAttr = GetFileAttributesW(driverPath);
            if (fileAttr == INVALID_FILE_ATTRIBUTES) {
                lastError = GetLastError();
                LogService::printfLog("[LoadDriver] Driver file does not exist or is inaccessible: %ls, Error code: %lu\n",
                    LogColor::LOG_RED, driverPath, lastError);
                return false;
            }

            scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
            if (!scm) {
                lastError = GetLastError();
                LogService::printfLog("[LoadDriver] Failed to open Service Control Manager, Error code: %lu\n",
                    LogColor::LOG_RED, lastError);
                return false;
            }

            LogService::printfLog("[LoadDriver] Service Control Manager opened successfully\n", LogColor::LOG_DEFAULT);

            service = OpenServiceW(scm, serviceName,
                SERVICE_ALL_ACCESS | SERVICE_CHANGE_CONFIG);

            if (service) {
                LogService::printfLog("[LoadDriver] Service already exists: %ls\n", LogColor::LOG_DEFAULT, serviceName);

                DWORD bytesNeeded = 0;
                QueryServiceConfigW(service, NULL, 0, &bytesNeeded);

                if (bytesNeeded > 0) {
                    LPQUERY_SERVICE_CONFIGW config = (LPQUERY_SERVICE_CONFIGW)LocalAlloc(LPTR, bytesNeeded);
                    if (config) {
                        if (QueryServiceConfigW(service, config, bytesNeeded, &bytesNeeded)) {
                            std::wstring currentPath = config->lpBinaryPathName ? config->lpBinaryPathName : L"";

                            std::wstring newPath = driverPath;

                            if (newPath.find(L'"') == std::wstring::npos && newPath.find(L' ') != std::wstring::npos) {
                                newPath = L"\"" + newPath + L"\"";
                            }

                            std::wstring currentPathClean = currentPath;
                            if (!currentPathClean.empty() && currentPathClean.front() == L'"' && currentPathClean.back() == L'"') {
                                currentPathClean = currentPathClean.substr(1, currentPathClean.length() - 2);
                            }

                            std::wstring newPathClean = newPath;
                            if (!newPathClean.empty() && newPathClean.front() == L'"' && newPathClean.back() == L'"') {
                                newPathClean = newPathClean.substr(1, newPathClean.length() - 2);
                            }

                            wchar_t currentLongPath[MAX_PATH] = { 0 };
                            wchar_t newLongPath[MAX_PATH] = { 0 };

                            GetLongPathNameW(currentPathClean.c_str(), currentLongPath, MAX_PATH);
                            GetLongPathNameW(newPathClean.c_str(), newLongPath, MAX_PATH);

                            bool pathsMatch = (_wcsicmp(currentLongPath, newLongPath) == 0) ||
                                (_wcsicmp(currentPathClean.c_str(), newPathClean.c_str()) == 0);

                            if (pathsMatch) {
                                LogService::printfLog("[LoadDriver] Driver paths are identical, starting service directly\n", LogColor::LOG_DEFAULT);

                                if (StartServiceW(service, 0, NULL)) {
                                    LogService::printfLog("[LoadDriver] Service started successfully: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                                    bSuccess = true;
                                }
                                else {
                                    lastError = GetLastError();
                                    if (lastError == ERROR_SERVICE_ALREADY_RUNNING) {
                                        LogService::printfLog("[LoadDriver] Service is already running: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                                        bSuccess = true;
                                    }
                                    else {
                                        LogService::printfLog("[LoadDriver] Failed to start service: %ls, Error code: %lu\n",
                                            LogColor::LOG_RED, serviceName, lastError);
                                    }
                                }
                            }
                            else {
                                LogService::printfLog("[LoadDriver] Driver paths differ, updating configuration\n", LogColor::LOG_DEFAULT);
                                LogService::printfLog("[LoadDriver] Current path: %ls\n", LogColor::LOG_DEFAULT, currentPath.c_str());
                                LogService::printfLog("[LoadDriver] New path: %ls\n", LogColor::LOG_DEFAULT, newPath.c_str());

                                SERVICE_STATUS status;
                                if (ControlService(service, SERVICE_CONTROL_STOP, &status)) {
                                    LogService::printfLog("[LoadDriver] Stopping service...\n", LogColor::LOG_DEFAULT);

                                    int retries = 0;
                                    while (QueryServiceStatus(service, &status) &&
                                        status.dwCurrentState == SERVICE_STOP_PENDING &&
                                        retries++ < 50) {
                                        Sleep(100);
                                    }

                                    if (status.dwCurrentState == SERVICE_STOPPED) {
                                        LogService::printfLog("%s Service stopped successfully\n", LogColor::LOG_DEFAULT);
                                    }
                                    else {
                                        LogService::printfLog("[LoadDriver] Service stop may have timed out, continuing anyway\n", LogColor::LOG_YELLOW);
                                    }
                                }
                                else {
                                    lastError = GetLastError();
                                    if (lastError != ERROR_SERVICE_NOT_ACTIVE) {
                                        LogService::printfLog("[LoadDriver] Failed to stop service, Error code: %lu\n",
                                            LogColor::LOG_YELLOW, lastError);
                                    }
                                    else {
                                        LogService::printfLog("[LoadDriver] Service is not running, proceeding with configuration update\n", LogColor::LOG_DEFAULT);
                                    }
                                }

                                if (ChangeServiceConfigW(service,
                                    SERVICE_NO_CHANGE,
                                    SERVICE_NO_CHANGE,
                                    SERVICE_NO_CHANGE,
                                    newPath.c_str(),
                                    NULL, NULL, NULL, NULL, NULL, NULL)) {
                                    LogService::printfLog("[LoadDriver] Service configuration updated successfully\n", LogColor::LOG_DEFAULT);

                                    if (StartServiceW(service, 0, NULL)) {
                                        LogService::printfLog("[LoadDriver] Service started successfully: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                                        bSuccess = true;
                                    }
                                    else {
                                        lastError = GetLastError();
                                        LogService::printfLog("[LoadDriver] Failed to start service: %ls, Error code: %lu\n",
                                            LogColor::LOG_RED, serviceName, lastError);
                                    }
                                }
                                else {
                                    lastError = GetLastError();
                                    LogService::printfLog("[LoadDriver] Failed to change service configuration: %ls, Error code: %lu\n",
                                        LogColor::LOG_RED, serviceName, lastError);
                                }
                            }
                        }
                        else {
                            lastError = GetLastError();
                            LogService::printfLog("[LoadDriver] Failed to query service configuration, Error code: %lu\n",
                                LogColor::LOG_RED, lastError);
                        }
                        LocalFree(config);
                    }
                    else {
                        LogService::printfLog("[LoadDriver] Failed to allocate memory for service configuration\n",
                            LogColor::LOG_RED);
                    }
                }

                CloseServiceHandle(service);
            }
            else {
                lastError = GetLastError();

                if (lastError == ERROR_SERVICE_DOES_NOT_EXIST) {
                    LogService::printfLog("[LoadDriver] Creating new service: %ls\n", LogColor::LOG_DEFAULT, serviceName);

                    std::wstring pathWithQuotes = driverPath;
                    if (pathWithQuotes.find(L'"') == std::wstring::npos && pathWithQuotes.find(L' ') != std::wstring::npos) {
                        pathWithQuotes = L"\"" + pathWithQuotes + L"\"";
                    }

                    service = CreateServiceW(scm,
                        serviceName,
                        serviceName,
                        SERVICE_ALL_ACCESS,
                        SERVICE_KERNEL_DRIVER,
                        SERVICE_DEMAND_START,
                        SERVICE_ERROR_NORMAL,
                        pathWithQuotes.c_str(),
                        NULL, NULL, NULL, NULL, NULL);

                    if (service) {
                        LogService::printfLog("[LoadDriver] Service created successfully\n", LogColor::LOG_DEFAULT);

                        if (StartServiceW(service, 0, NULL)) {
                            LogService::printfLog("[LoadDriver] Service started successfully: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                            bSuccess = true;
                        }
                        else {
                            lastError = GetLastError();
                            if (lastError == ERROR_SERVICE_ALREADY_RUNNING) {
                                LogService::printfLog("[LoadDriver] Service is already running: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                                bSuccess = true;
                            }
                            else {
                                LogService::printfLog("[LoadDriver] Failed to start service: %ls, Error code: %lu\n",
                                    LogColor::LOG_RED, serviceName, lastError);

                                if (DeleteService(service)) {
                                    LogService::printfLog("[LoadDriver] Deleted the failed service\n", LogColor::LOG_DEFAULT);
                                }
                                else {
                                    lastError = GetLastError();
                                    LogService::printfLog("[LoadDriver] Failed to delete the failed service, Error code: %lu\n",
                                        LogColor::LOG_YELLOW, lastError);
                                }
                            }
                        }

                        CloseServiceHandle(service);
                    }
                    else {
                        lastError = GetLastError();
                        LogService::printfLog("[LoadDriver] Failed to create service: %ls, Error code: %lu\n",
                            LogColor::LOG_RED, serviceName, lastError);
                    }
                }
                else {
                    LogService::printfLog("[LoadDriver] Failed to open service: %ls, Error code: %lu\n",
                        LogColor::LOG_RED, serviceName, lastError);
                }
            }

            if (scm) {
                CloseServiceHandle(scm);
            }

            if (bSuccess) {
                LogService::printfLog("[LoadDriver] Driver loading completed successfully: %ls\n", LogColor::LOG_DEFAULT, serviceName);
            }
            else {
                LogService::printfLog("[LoadDriver] Driver loading failed: %ls\n", LogColor::LOG_RED, serviceName);
            }

            return bSuccess;
        }

        void SafeCloseServiceHandle(SC_HANDLE& handle) {
            if (handle) {
                CloseServiceHandle(handle);
                handle = NULL;
            }
        }

        bool UnloadDriver(LPCWSTR serviceName) {
            SC_HANDLE scm = NULL;
            SC_HANDLE service = NULL;
            bool bSuccess = false;
            DWORD lastError = 0;

            scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
            if (!scm) {
                lastError = GetLastError();
                LogService::printfLog("[UnloadDriver] Failed to open Service Control Manager, Error code: %lu\n",
                    LogColor::LOG_RED, lastError);
                return false;
            }

            service = OpenServiceW(scm, serviceName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
            if (!service) {
                lastError = GetLastError();
                if (lastError == ERROR_SERVICE_DOES_NOT_EXIST) {
                    LogService::printfLog("[UnloadDriver] Service does not exist: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                    bSuccess = true;
                }
                else {
                    LogService::printfLog("[UnloadDriver] Failed to open service: %ls, Error code: %lu\n",
                        LogColor::LOG_RED, serviceName, lastError);
                }
            }
            else {
                SERVICE_STATUS status;
                if (ControlService(service, SERVICE_CONTROL_STOP, &status)) {
                    LogService::printfLog("[UnloadDriver] Stopping service...\n", LogColor::LOG_DEFAULT);

                    int retries = 0;
                    while (QueryServiceStatus(service, &status) &&
                        status.dwCurrentState == SERVICE_STOP_PENDING &&
                        retries++ < 50) {
                        Sleep(100);
                    }

                    if (status.dwCurrentState == SERVICE_STOPPED) {
                        LogService::printfLog("[UnloadDriver] Service stopped successfully\n", LogColor::LOG_DEFAULT);
                    }
                    else {
                        LogService::printfLog("[UnloadDriver] Service stop may have timed out\n", LogColor::LOG_YELLOW);
                    }
                }
                else {
                    lastError = GetLastError();
                    if (lastError != ERROR_SERVICE_NOT_ACTIVE) {
                        LogService::printfLog("[UnloadDriver] Failed to stop service, Error code: %lu\n",
                            LogColor::LOG_YELLOW, lastError);
                    }
                    else {
                        LogService::printfLog("[UnloadDriver] Service is not running\n", LogColor::LOG_DEFAULT);
                    }
                }

                if (DeleteService(service)) {
                    LogService::printfLog("[UnloadDriver] Service deleted successfully: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                    bSuccess = true;
                }
                else {
                    lastError = GetLastError();
                    LogService::printfLog("[UnloadDriver] Failed to delete service: %ls, Error code: %lu\n",
                       LogColor::LOG_RED, serviceName, lastError);
                }

                CloseServiceHandle(service);
            }

            CloseServiceHandle(scm);
            return bSuccess;
        }

        bool CheckServiceStatus(LPCWSTR serviceName) {
            SC_HANDLE scm = NULL;
            SC_HANDLE service = NULL;
            bool bRunning = false;
            DWORD lastError = 0;

            scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
            if (!scm) {
                lastError = GetLastError();
                LogService::printfLog("[CheckServiceStatus] Failed to open Service Control Manager, Error code: %lu\n",
                    LogColor::LOG_RED, lastError);
                return false;
            }

            service = OpenServiceW(scm, serviceName, SERVICE_QUERY_STATUS);
            if (!service) {
                lastError = GetLastError();
                if (lastError == ERROR_SERVICE_DOES_NOT_EXIST) {
                    LogService::printfLog("[CheckServiceStatus] Service does not exist: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                }
                else {
                    LogService::printfLog("[CheckServiceStatus] Failed to open service: %ls, Error code: %lu\n",
                        LogColor::LOG_RED, serviceName, lastError);
                }
            }
            else {
                SERVICE_STATUS status;
                if (QueryServiceStatus(service, &status)) {
                    if (status.dwCurrentState == SERVICE_RUNNING) {
                        LogService::printfLog("[CheckServiceStatus] Service is running: %ls\n", LogColor::LOG_GREEN, serviceName);
                        bRunning = true;
                    }
                    else if (status.dwCurrentState == SERVICE_STOPPED) {
                        LogService::printfLog("[CheckServiceStatus] Service is stopped: %ls\n", LogColor::LOG_DEFAULT, serviceName);
                    }
                    else if (status.dwCurrentState == SERVICE_START_PENDING) {
                        LogService::printfLog("[CheckServiceStatus] Service is starting: %ls\n", LogColor::LOG_YELLOW, serviceName);
                    }
                    else if (status.dwCurrentState == SERVICE_STOP_PENDING) {
                        LogService::printfLog("[CheckServiceStatus] Service is stopping: %ls\n", LogColor::LOG_YELLOW, serviceName);
                    }
                    else {
                        LogService::printfLog("[CheckServiceStatus] Service state: %lu for %ls\n", LogColor::LOG_DEFAULT, status.dwCurrentState, serviceName);
                    }
                }
                else {
                    lastError = GetLastError();
                    LogService::printfLog("[CheckServiceStatus] Failed to query service status, Error code: %lu\n",
                        LogColor::LOG_RED, lastError);
                }

                CloseServiceHandle(service);
            }

            CloseServiceHandle(scm);
            return bRunning;
        }
	}

    BOOL ReleaseCoreDriver()
    {
        HMODULE hModule = GetModuleHandle(NULL);
        if (hModule == NULL) {
            LogService::printfLog("[ReleaseCoreDriver] Failed To Create Driver File,Error code: %lu\n",
                LogColor::LOG_RED, GetLastError());
            return FALSE;
        }
        HRSRC hRsrc = FindResource(hModule, MAKEINTRESOURCE(IDR_GKISERVICE1), TEXT("GKIService"));
        if (hRsrc == NULL) {
            LogService::printfLog("[ReleaseCoreDriver] Failed To Create Driver File,Error code: %lu\n",
                LogColor::LOG_RED, GetLastError());
            return FALSE;
        }
        DWORD dwSize = SizeofResource(hModule, hRsrc);
        if (dwSize == 0) {
            LogService::printfLog("[ReleaseCoreDriver] Failed To Create Driver File,Error code: %lu\n",
                LogColor::LOG_RED, GetLastError());
            return FALSE;
        }
        HGLOBAL hGlobal = LoadResource(hModule, hRsrc);
        if (hGlobal == NULL) {
            LogService::printfLog("[ReleaseCoreDriver] Failed To Create Driver File,Error code: %lu\n",
                LogColor::LOG_RED, GetLastError());
            return FALSE;
        }

        LPVOID lpVoid = LockResource(hGlobal);
        if (lpVoid == NULL) {
            LogService::printfLog("[ReleaseCoreDriver] Failed To Create Driver File,Error code: %lu\n",
                LogColor::LOG_RED, GetLastError());
            FreeResource(hGlobal);
            return FALSE;
        }

        FILE* fp = fopen("C:\\Windows\\System32\\drivers\\SKT64.sys", "wb+");
        if (fp == NULL) {
            LogService::printfLog("[ReleaseCoreDriver] Failed To Create Driver File,Error code: %lu\n",
                LogColor::LOG_RED, GetLastError());
            FreeResource(hGlobal);
            return FALSE;
        }
        fwrite(lpVoid, sizeof(char), dwSize, fp);
        fclose(fp);
        FreeResource(hGlobal);
        return TRUE;
    }

    std::string WStringToUTF8(const std::wstring& wstr) {
        if (wstr.empty()) return {};
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string str(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, nullptr, nullptr);
        return str;
    }
}