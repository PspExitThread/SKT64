#pragma once
#include <ntifs.h>
#include <wdm.h>
#include "GeneralService.h"
#include "PspCidTableParse.h"
#include "QueryProcessEx.h"
#include "ProcessOperationService.h"
#include "FileSystemIoRequestService.h"
#include "KernelModuleQuery.h"


typedef enum _GKISYSTEM_INFORMATION_CLASS
{
	GkiSystemProcessInformation = 0,
	GkiSystemProcessInformationEx = 1,
	GkiSystemKernelModuleInformation = 2,
	GkiSystemMiniFilterInformation = 3,
	GkiSystemKernelCallbackInformation = 4,
	GkiSystemKernelNotifyInformation = 5
} GKISYSTEM_INFORMATION_CLASS;

typedef enum _GKISERVICE_INFORMATION_CLASS
{
	GkiDebugPrintServiceContextInformation = 0
} GKISERVICE_INFORMATION_CLASS;


typedef enum _GKISETINFORMATION_PROCESS_OPERATION
{
	GkiSystemProcessTerminate = 0,
	GkiSystemProcessDisableApcInsert = 1,
	GkiSystemProcessSetBreakOnTermination = 2,
	GkiSystemProcessTerminateDeleteSourceFile = 3
} GKISETINFORMATION_PROCESS_OPERATION;

typedef enum _GKISETINFORMATION_KERNELMODULE_OPERATION
{
	GkiSystemKernelModuleUnload = 0
} GKISETINFORMATION_KERNELMODULE_OPERATION;

namespace KiInterface
{
	NTSTATUS SetDefaultSystemInformationLength(PULONG64 SystemInformationLength);
	NTSTATUS GkiQuerySystemInformation(GKISYSTEM_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
		ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10);
	NTSTATUS GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
		ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10);
	NTSTATUS GkiQueryServiceInformation(GKISERVICE_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
		ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10);
}