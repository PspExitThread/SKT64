#pragma once
#include <Windows.h>
#include <iostream>

typedef enum _GKISYSTEM_INFORMATION_CLASS
{
	GkiSystemProcessInformation = 0,
	GkiSystemProcessInformationEx = 1,
	GkiSystemKernelModuleInformation = 2,
	GkiSystemMiniFilterInformation = 3,
	GkiSystemKernelCallbackInformation = 4,
	GkiSystemKernelNotifyInformation = 5
} GKISYSTEM_INFORMATION_CLASS;

typedef struct _KIIO_INPUT_DATA
{
	GKISYSTEM_INFORMATION_CLASS SystemInformationClass;
	PVOID SystemInformation;
	PULONG64 SystemInformationLength;
	ULONG_PTR Parameter1;
	ULONG_PTR Parameter2;
	ULONG_PTR Parameter3;
	ULONG_PTR Parameter4;
	ULONG_PTR Parameter5;
	ULONG_PTR Parameter6;
	ULONG_PTR Parameter7;
	ULONG_PTR Parameter8;
	ULONG_PTR Parameter9;
	ULONG_PTR Parameter10;
}KIIO_INPUT_DATA, * PKIIO_INPUT_DATA;

#define KIMAX_PATHEX 500

typedef struct _QUERY_PROCESS_INFO
{
	WCHAR PROCESSNAME[KIMAX_PATHEX];
	PVOID PPROCESSOBJECT;
	ULONG64 PROCESSPID;
	WCHAR PROCESSPATH[KIMAX_PATHEX];
} QUERY_PROCESS_INFO, * PQUERY_PROCESS_INFO;

typedef struct _QUERY_KERNEL_MODULE_INFO
{
	ULONG64 BaseAddress;
	ULONG64 EntryPoint;
	ULONG SizeOfImage;
	PVOID pDriverObject;
	WCHAR FullModuleName[KIMAX_PATHEX];
	WCHAR ModuleName[KIMAX_PATHEX];
} QUERY_KERNEL_MODULE_INFO, * PQUERY_KERNEL_MODULE_INFO;

typedef enum _MODE {
	KernelMode,
	UserMode,
	MaximumMode
} MODE;

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


#define IOCTL_QUERY_SYSTEM_INFORMATION		           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_SYSTEM_INFORMATION		           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
