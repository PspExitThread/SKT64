#include "DiskAccessService.h"

namespace KiDiskAccessService 
{
    NTSTATUS IoRequestCompleteRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
    {
		UNREFERENCED_PARAMETER(Context);
		UNREFERENCED_PARAMETER(DeviceObject);
        *pIrp->UserIosb = pIrp->IoStatus;
        if (pIrp->UserEvent) {
            KeSetEvent(pIrp->UserEvent, IO_NO_INCREMENT, FALSE);
        }
        if (pIrp->MdlAddress) {
            IoFreeMdl(pIrp->MdlAddress);
            pIrp->MdlAddress = NULL;
        }
        IoFreeIrp(pIrp);
        pIrp = NULL;
        return STATUS_MORE_PROCESSING_REQUIRED;
    }

    NTSTATUS DirectReadDisk(PDEVICE_OBJECT TargetDisk, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer,  ULONG Length, PLARGE_INTEGER ByteOffset)
    {
        NTSTATUS status = STATUS_SUCCESS;
        PIRP pIrp = NULL;
        KEVENT kEvent = { 0 };
        PIO_STACK_LOCATION pIoStackLocation = NULL;
        if ((NULL == TargetDisk)) {
            return STATUS_UNSUCCESSFUL;
        }
        pIrp = IoAllocateIrp(TargetDisk->StackSize, FALSE);
        if (NULL == pIrp) {
            return STATUS_UNSUCCESSFUL;
        }
        KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);
        RtlZeroMemory(Buffer, Length);
        pIrp->MdlAddress = MmCreateMdl(NULL, Buffer, Length);
        if (NULL == pIrp->MdlAddress){
            IoFreeIrp(pIrp);
            return STATUS_INSUFFICIENT_RESOURCES;
        }
        MmBuildMdlForNonPagedPool(pIrp->MdlAddress);
        pIrp->UserEvent = &kEvent;
        pIrp->UserIosb = IoStatusBlock;
        pIrp->Flags = IRP_READ_OPERATION;
        pIrp->RequestorMode = KernelMode;
        pIrp->Tail.Overlay.Thread = PsGetCurrentThread();
        pIoStackLocation = IoGetNextIrpStackLocation(pIrp);
        pIoStackLocation->MajorFunction = IRP_MJ_READ;
        pIoStackLocation->MinorFunction = IRP_MN_NORMAL;
        pIoStackLocation->DeviceObject = TargetDisk;
        pIoStackLocation->Parameters.Read.Length = Length;
        pIoStackLocation->Parameters.Read.ByteOffset = *ByteOffset;
        IoSetCompletionRoutine(pIrp, IoRequestCompleteRoutine, NULL, TRUE, TRUE, TRUE);
        status = IofCallDriver(TargetDisk, pIrp);
        if (STATUS_PENDING == status){
            KeWaitForSingleObject(&kEvent, Executive, KernelMode, FALSE, NULL);
        }
        status = IoStatusBlock->Status;
        if (!NT_SUCCESS(status)){
            return status;
        }
        return status;
    }

    NTSTATUS DirectWriteDisk(PDEVICE_OBJECT TargetDisk, PIO_STATUS_BLOCK IoStatusBlock, PVOID Buffer, ULONG Length, PLARGE_INTEGER ByteOffset)
    {
        NTSTATUS status = STATUS_SUCCESS;
        PIRP pIrp = NULL;
        KEVENT kEvent = { 0 };
        PIO_STACK_LOCATION pIoStackLocation = NULL;
        if ((NULL == ByteOffset)) {
            return STATUS_UNSUCCESSFUL;
        }
        pIrp = IoAllocateIrp(TargetDisk->StackSize, FALSE);
        if (NULL == pIrp) {
            return STATUS_UNSUCCESSFUL;
        }
        KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);
        pIrp->MdlAddress = MmCreateMdl(NULL, Buffer, Length);
        if (NULL == pIrp->MdlAddress) {
            IoFreeIrp(pIrp);
            return STATUS_INSUFFICIENT_RESOURCES;
        }
        MmBuildMdlForNonPagedPool(pIrp->MdlAddress);
        pIrp->UserEvent = &kEvent;
        pIrp->UserIosb = IoStatusBlock;
        pIrp->Flags = IRP_WRITE_OPERATION;
        pIrp->RequestorMode = KernelMode;
        pIrp->Tail.Overlay.Thread = PsGetCurrentThread();
        pIoStackLocation = IoGetNextIrpStackLocation(pIrp);
        pIoStackLocation->MajorFunction = IRP_MJ_WRITE;
        pIoStackLocation->MinorFunction = IRP_MN_NORMAL;
        pIoStackLocation->DeviceObject = TargetDisk;
        pIoStackLocation->Parameters.Write.Length = Length;
        pIoStackLocation->Parameters.Write.ByteOffset = *ByteOffset;
        IoSetCompletionRoutine(pIrp, IoRequestCompleteRoutine, NULL, TRUE, TRUE, TRUE);
        status = IofCallDriver(TargetDisk, pIrp);
        if (STATUS_PENDING == status){
            KeWaitForSingleObject(&kEvent, Executive, KernelMode, FALSE, NULL);
        }
        status = IoStatusBlock->Status;
        if (!NT_SUCCESS(status)) {
            return status;
        }
        return status;
    }
}