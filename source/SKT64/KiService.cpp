#include "KiService.h"

namespace KiService 
{
    BOOL GkiQuerySystemInformation(GKISYSTEM_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
        ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10)
    {
        if (!g_hKernelDevice) {
            LogService::printfLog("[GkiQuerySystemInformation] Failed to get kernel service: %d\n", LogColor::LOG_RED, GetLastError());
            return FALSE;
        }
        typedef struct _KIIO_INPUT
        {
            ULONG_PTR DATASize;
            PVOID DATAPoint;
        }KIIO_INPUT, * PKIIO_INPUT;
        KIIO_INPUT pInput = { 0 };
        pInput.DATAPoint = (PVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(KIIO_INPUT_DATA));
        pInput.DATASize = sizeof(KIIO_INPUT_DATA);
        LogService::printfLog("[GkiQuerySystemInformation] Allocate Buffer: %p Size: %x\n", LogColor::LOG_DEFAULT, pInput.DATAPoint, pInput.DATASize);
        PKIIO_INPUT_DATA pIoData = (PKIIO_INPUT_DATA)pInput.DATAPoint;
        pIoData->SystemInformationClass = InformationClass;
        pIoData->SystemInformation = SystemInformation;
        pIoData->SystemInformationLength = SystemInformationLength;
        pIoData->Parameter1 = Parameter1;
        pIoData->Parameter2 = Parameter2;
        pIoData->Parameter3 = Parameter3;
        pIoData->Parameter4 = Parameter4;
        pIoData->Parameter5 = Parameter5;
        pIoData->Parameter6 = Parameter6;
        pIoData->Parameter7 = Parameter7;
        pIoData->Parameter8 = Parameter8;
        pIoData->Parameter9 = Parameter9;
        pIoData->Parameter10 = Parameter10;
        DWORD bytesReturned;
        BOOL status = DeviceIoControl(g_hKernelDevice, IOCTL_QUERY_SYSTEM_INFORMATION, &pInput, sizeof(KIIO_INPUT), NULL, 0, &bytesReturned, NULL);
        if (status) {
            LogService::printfLog("[GkiQuerySystemInformation] IoRequestService successfully.\n", LogColor::LOG_DEFAULT);
        }
        else {
            LogService::printfLog("[GkiQuerySystemInformation] IoRequestService Fail.(STATUS: %d)\n", LogColor::LOG_DEFAULT, GetLastError());
        }
        HeapFree(GetProcessHeap(), 0, pInput.DATAPoint);
        return status;
    }

    BOOL GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
        ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10)
    {
        if (!g_hKernelDevice) {
            LogService::printfLog("[GkiSetSystemInformation] Failed to get kernel service: %d\n", LogColor::LOG_RED, GetLastError());
            return FALSE;
        }
        typedef struct _KIIO_INPUT
        {
            ULONG_PTR DATASize;
            PVOID DATAPoint;
        }KIIO_INPUT, * PKIIO_INPUT;
        KIIO_INPUT pInput = { 0 };
        pInput.DATAPoint = (PVOID)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(KIIO_INPUT_DATA));
        pInput.DATASize = sizeof(KIIO_INPUT_DATA);
        LogService::printfLog("[GkiSetSystemInformation] Allocate Buffer: %p Size: %x\n", LogColor::LOG_DEFAULT, pInput.DATAPoint, pInput.DATASize);
        PKIIO_INPUT_DATA pIoData = (PKIIO_INPUT_DATA)pInput.DATAPoint;
        pIoData->SystemInformationClass = InformationClass;
        pIoData->SystemInformation = SystemInformation;
        pIoData->SystemInformationLength = SystemInformationLength;
        pIoData->Parameter1 = Parameter1;
        pIoData->Parameter2 = Parameter2;
        pIoData->Parameter3 = Parameter3;
        pIoData->Parameter4 = Parameter4;
        pIoData->Parameter5 = Parameter5;
        pIoData->Parameter6 = Parameter6;
        pIoData->Parameter7 = Parameter7;
        pIoData->Parameter8 = Parameter8;
        pIoData->Parameter9 = Parameter9;
        pIoData->Parameter10 = Parameter10;
        DWORD bytesReturned;
        BOOL status = DeviceIoControl(g_hKernelDevice, IOCTL_SET_SYSTEM_INFORMATION, &pInput, sizeof(KIIO_INPUT), NULL, 0, &bytesReturned, NULL);
        if (status) {
            LogService::printfLog("[GkiSetSystemInformation] IoRequestService successfully.\n", LogColor::LOG_DEFAULT);
        }
        else {
            LogService::printfLog("[GkiSetSystemInformation] IoRequestService Fail.(STATUS: %d)\n", LogColor::LOG_DEFAULT, GetLastError());
        }
        HeapFree(GetProcessHeap(), 0, pInput.DATAPoint);
        return status;
    }

}