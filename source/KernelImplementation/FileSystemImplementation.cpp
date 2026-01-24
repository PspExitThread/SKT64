#include "FileSystemImplementation.h"



namespace KiFileSystemImplementation
{
	namespace NTFSImplementation {
		PKINTFSIMP_DEVICE_HANDLE KiOpenNTFSServiceHandle(PDEVICE_OBJECT pTargetDiskDevice) {
			NTSTATUS status = STATUS_UNSUCCESSFUL;
			if (pTargetDiskDevice == nullptr)
				return NULL;
			PKINTFSIMP_DEVICE_HANDLE pHandle = (PKINTFSIMP_DEVICE_HANDLE)ExAllocatePool2(
				POOL_FLAG_NON_PAGED,
				sizeof(PKINTFSIMP_DEVICE_HANDLE),
				'MODH');
			if (pHandle != NULL) {
				PIRP pIrp;
				IO_STATUS_BLOCK IoStatusBlock;
				KEVENT kEvent;
				KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);
				DRIVE_LAYOUT_INFORMATION_EX pDriveLayout = { 0 };
				pIrp = IoBuildDeviceIoControlRequest(
					IOCTL_DISK_GET_DRIVE_LAYOUT_EX,
					pTargetDiskDevice,
					NULL, 0,
					&pDriveLayout, sizeof(DRIVE_LAYOUT_INFORMATION_EX),
					FALSE,
					&kEvent,
					&IoStatusBlock
				);
				if (!pIrp)
					return NULL;
				status = IofCallDriver(pTargetDiskDevice, pIrp);
				if (status == STATUS_PENDING) {
					KeWaitForSingleObject(&kEvent, Executive, KernelMode, FALSE, NULL);
					status = IoStatusBlock.Status;
				}
				if (NT_SUCCESS(status)) {
					for (ULONG i = 0; i < pDriveLayout.PartitionCount; i++) {
						DbgPrintEx(0, 0, "[PART] StartingOffset = 0x%x PartitionLength = 0x%x\n", pDriveLayout.PartitionEntry[i].StartingOffset.QuadPart, pDriveLayout.PartitionEntry[i].PartitionLength);
					}
				}
				return pHandle;
			}
			return NULL;
		}
		NTSTATUS test()
		{
			PDRIVER_OBJECT ptDiskDriverObject = NULL;
			UNICODE_STRING TargetDriverName;
			RtlInitUnicodeString(&TargetDriverName, L"\\Driver\\Disk");
			NTSTATUS status = ObReferenceObjectByName(&TargetDriverName, OBJ_CASE_INSENSITIVE, 0, 0, *IoDriverObjectType, KernelMode, 0, reinterpret_cast<PVOID*>(&ptDiskDriverObject));
			if (NT_SUCCESS(status) && ptDiskDriverObject != NULL)
			{
				PDEVICE_OBJECT pCurrentDevice = ptDiskDriverObject->DeviceObject;

				while (pCurrentDevice != NULL)
				{
					KiOpenNTFSServiceHandle(pCurrentDevice);
					pCurrentDevice = pCurrentDevice->NextDevice;
				}
				ObDereferenceObject(ptDiskDriverObject);
			}
			return 0;
		}
	}
}