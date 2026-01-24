#pragma once
#include <ntifs.h>
#include <wdm.h>
#include <ntdddisk.h>
#include "GeneralService.h"
#include "DiskAccessService.h"

typedef struct _KINTFSIMP_DEVICE_HANDLE
{
	PDEVICE_OBJECT pTargetDiskDevice;

} KINTFSIMP_DEVICE_HANDLE, * PKINTFSIMP_DEVICE_HANDLE;

namespace KiFileSystemImplementation 
{
	namespace NTFSImplementation {
		PKINTFSIMP_DEVICE_HANDLE KiOpenNTFSServiceHandle(PDEVICE_OBJECT pTargetDiskDevice);
		NTSTATUS test();
	}
}