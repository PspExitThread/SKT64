#pragma once
#include <ntifs.h>
#include <wdm.h>
#include "GeneralService.h"

namespace KiFileSystemIoRequestService
{
	NTSTATUS KiFileSystemIoRequestServiceIoRequestCompleteRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context);
	NTSTATUS IrpCreateFile(OUT PFILE_OBJECT* ppFileObject, IN ACCESS_MASK DesiredAccess, IN PUNICODE_STRING pustrFilePath, OUT PIO_STATUS_BLOCK IoStatusBlock, IN PLARGE_INTEGER AllocationSize OPTIONAL, IN ULONG FileAttributes, IN ULONG ShareAccess, IN ULONG CreateDisposition, IN ULONG CreateOptions, IN PVOID EaBuffer OPTIONAL, IN ULONG EaLength);
	NTSTATUS IrpWriteFile(IN PFILE_OBJECT pFileObject, OUT PIO_STATUS_BLOCK IoStatusBlock, IN PVOID Buffer, IN ULONG Length, IN PLARGE_INTEGER ByteOffset OPTIONAL);
	NTSTATUS IrpQueryInformationFile(IN PFILE_OBJECT pFileObject, OUT PIO_STATUS_BLOCK IoStatusBlock, OUT PVOID FileInformation, IN ULONG Length, IN FILE_INFORMATION_CLASS FileInformationClass);
	NTSTATUS IrpSetInformationFile(IN PFILE_OBJECT pFileObject, OUT PIO_STATUS_BLOCK IoStatusBlock, IN PVOID FileInformation, IN ULONG Length, IN FILE_INFORMATION_CLASS FileInformationClass);
}