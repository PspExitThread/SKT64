#pragma once
#include <ntifs.h>
#include <wdm.h>
#include "GeneralService.h"


namespace KiDiskAccessService
{
	NTSTATUS IoRequestCompleteRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context);
	NTSTATUS DirectReadDisk(PDEVICE_OBJECT TargetDisk, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset);
	NTSTATUS DirectWriteDisk(PDEVICE_OBJECT TargetDisk, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset);
}
