#pragma once
#include <ntifs.h>
#include "GeneralService.h"

typedef struct _KIKERNEL_MODULE_INFO
{
	ULONG64 BaseAddress;
	ULONG64 EntryPoint;
	ULONG SizeOfImage;
	PDRIVER_OBJECT pDriverObject;
	WCHAR FullModuleName[KIMAX_PATHEX];
	WCHAR ModuleName[KIMAX_PATHEX];
} KIKERNEL_MODULE_INFO, * PKIKERNEL_MODULE_INFO;

namespace KiKernelModuleQueryService 
{
	NTSTATUS ParseDirectoryObjectModules(PKIKERNEL_MODULE_INFO ModuleInfoList, PVOID pDirectoryObject, POBJECT_TYPE IoDirectoryObjectType);
	PKIKERNEL_MODULE_INFO QueryKernelModuleInformation(SIZE_T ListSize);
	VOID ExecuteDriverUnloadThread(IN PVOID lpParam);
	NTSTATUS ForceUnloadDriver(PDRIVER_OBJECT pDriverObject);
}