#pragma once
#include <ntifs.h>
#include "GeneralService.h"
#include "KernelInterface.h"

namespace IoRequestControl
{
	NTSTATUS DriverDefaultHandle(PDEVICE_OBJECT pDevObj, PIRP pIrp);
	NTSTATUS DispatchControlCodeCreate(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);
	NTSTATUS DispatchControlCode(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
}

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

#define IOCTL_QUERY_SYSTEM_INFORMATION		           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_SYSTEM_INFORMATION		           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
