#include "FileSystemIoRequestService.h"
#pragma warning(disable:4996)

namespace KiFileSystemIoRequestService
{
    NTSTATUS KiFileSystemIoRequestServiceIoRequestCompleteRoutine(IN PDEVICE_OBJECT DeviceObject, IN PIRP pIrp, IN PVOID Context)
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

    NTSTATUS IrpCreateFile(OUT PFILE_OBJECT* ppFileObject, IN ACCESS_MASK DesiredAccess, IN PUNICODE_STRING pustrFilePath, OUT PIO_STATUS_BLOCK IoStatusBlock, IN PLARGE_INTEGER AllocationSize OPTIONAL, IN ULONG FileAttributes, IN ULONG ShareAccess, IN ULONG CreateDisposition, IN ULONG CreateOptions, IN PVOID EaBuffer OPTIONAL, IN ULONG EaLength)
    {
        UNREFERENCED_PARAMETER(AllocationSize);
        NTSTATUS status = STATUS_SUCCESS;
        ULONG ulFileNameMaxSize = 512;
        WCHAR wszName[100] = { 0 };
        UNICODE_STRING ustrRootPath;
        OBJECT_ATTRIBUTES objectAttributes = { 0 };
        HANDLE hRootFile = NULL;
        PFILE_OBJECT pRootFileObject = NULL, pFileObject = NULL;
        PDEVICE_OBJECT RootDeviceObject = NULL, RootRealDevice = NULL;
        PIRP pIrp = NULL;
        KEVENT kEvent = { 0 };
        ACCESS_STATE accessData = { 0 };
        AUX_ACCESS_DATA auxAccessData = { 0 };
        IO_SECURITY_CONTEXT ioSecurityContext = { 0 };
        PIO_STACK_LOCATION pIoStackLocation = NULL;
        wcscpy(wszName, L"\\??\\A:\\");
        wszName[4] = pustrFilePath->Buffer[0];
        RtlInitUnicodeString(&ustrRootPath, wszName);
        InitializeObjectAttributes(&objectAttributes, &ustrRootPath, OBJ_KERNEL_HANDLE, NULL, NULL);
        status = IoCreateFile(&hRootFile, GENERIC_READ | SYNCHRONIZE,
            &objectAttributes, IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0, CreateFileTypeNone,
            NULL, IO_NO_PARAMETER_CHECKING);
        if (!NT_SUCCESS(status)) {
            return status;
        }
        status = ObReferenceObjectByHandle(hRootFile, FILE_READ_ACCESS, *IoFileObjectType, KernelMode, (PVOID*)&pRootFileObject, NULL);
        if (!NT_SUCCESS(status)) {
            ZwClose(hRootFile);
            return status;
        }
        RootDeviceObject = pRootFileObject->Vpb->DeviceObject;
        RootRealDevice = pRootFileObject->Vpb->RealDevice;
        ObDereferenceObject(pRootFileObject);
        ZwClose(hRootFile);
        pIrp = IoAllocateIrp(RootDeviceObject->StackSize, FALSE);
        if (NULL == pIrp) {
            ObDereferenceObject(pFileObject);
            return STATUS_UNSUCCESSFUL;
        }
        KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);
        InitializeObjectAttributes(&objectAttributes, NULL, OBJ_CASE_INSENSITIVE, NULL, NULL);
        status = ObCreateObject(KernelMode, *IoFileObjectType, &objectAttributes, KernelMode, NULL, sizeof(FILE_OBJECT), 0, 0, (PVOID*)&pFileObject);
        if (!NT_SUCCESS(status)) {
            return status;
        }
        RtlZeroMemory(pFileObject, sizeof(FILE_OBJECT));
        pFileObject->Type = IO_TYPE_FILE;
        pFileObject->Size = sizeof(FILE_OBJECT);
        pFileObject->DeviceObject = RootRealDevice;
        pFileObject->Flags = FO_SYNCHRONOUS_IO;
        pFileObject->FileName.Buffer = (PWCHAR)ExAllocatePool(NonPagedPool, ulFileNameMaxSize);
        pFileObject->FileName.MaximumLength = (USHORT)ulFileNameMaxSize;
        pFileObject->FileName.Length = pustrFilePath->Length - 4;
        RtlZeroMemory(pFileObject->FileName.Buffer, ulFileNameMaxSize);
        RtlCopyMemory(pFileObject->FileName.Buffer, &pustrFilePath->Buffer[2], pFileObject->FileName.Length);
        KeInitializeEvent(&pFileObject->Lock, SynchronizationEvent, FALSE);
        KeInitializeEvent(&pFileObject->Event, NotificationEvent, FALSE);
        RtlZeroMemory(&auxAccessData, sizeof(auxAccessData));
        status = SeCreateAccessState(&accessData, &auxAccessData, DesiredAccess, IoGetFileObjectGenericMapping());
        if (!NT_SUCCESS(status)) {
            IoFreeIrp(pIrp);
            ObDereferenceObject(pFileObject);
            return status;
        }
        ioSecurityContext.SecurityQos = NULL;
        ioSecurityContext.AccessState = &accessData;
        ioSecurityContext.DesiredAccess = DesiredAccess;
        ioSecurityContext.FullCreateOptions = 0;
        RtlZeroMemory(IoStatusBlock, sizeof(IO_STATUS_BLOCK));
        pIrp->MdlAddress = NULL;
        pIrp->AssociatedIrp.SystemBuffer = EaBuffer;
        pIrp->Flags = IRP_CREATE_OPERATION | IRP_SYNCHRONOUS_API;
        pIrp->RequestorMode = KernelMode;
        pIrp->UserIosb = IoStatusBlock;
        pIrp->UserEvent = &kEvent;
        pIrp->PendingReturned = FALSE;
        pIrp->Cancel = FALSE;
        pIrp->CancelRoutine = NULL;
        pIrp->Tail.Overlay.Thread = PsGetCurrentThread();
        pIrp->Tail.Overlay.AuxiliaryBuffer = NULL;
        pIrp->Tail.Overlay.OriginalFileObject = pFileObject;
        pIoStackLocation = IoGetNextIrpStackLocation(pIrp);
        pIoStackLocation->MajorFunction = IRP_MJ_CREATE;
        pIoStackLocation->DeviceObject = RootDeviceObject;
        pIoStackLocation->FileObject = pFileObject;
        pIoStackLocation->Parameters.Create.SecurityContext = &ioSecurityContext;
        pIoStackLocation->Parameters.Create.Options = (CreateDisposition << 24) | CreateOptions;
        pIoStackLocation->Parameters.Create.FileAttributes = (USHORT)FileAttributes;
        pIoStackLocation->Parameters.Create.ShareAccess = (USHORT)ShareAccess;
        pIoStackLocation->Parameters.Create.EaLength = EaLength;
        IoSetCompletionRoutine(pIrp, KiFileSystemIoRequestServiceIoRequestCompleteRoutine, NULL, TRUE, TRUE, TRUE);
        status = IoCallDriver(RootDeviceObject, pIrp);
        if (STATUS_PENDING == status) {
            KeWaitForSingleObject(&kEvent, Executive, KernelMode, TRUE, NULL);
        }
        status = IoStatusBlock->Status;
        if (!NT_SUCCESS(status)) {
            ObDereferenceObject(pFileObject);
            return status;
        }
        InterlockedIncrement(&pFileObject->DeviceObject->ReferenceCount);
        if (pFileObject->Vpb) {
            InterlockedIncrement((PLONG)&pFileObject->Vpb->ReferenceCount);
        }
        *ppFileObject = pFileObject;
        return status;
    }

    NTSTATUS IrpWriteFile(IN PFILE_OBJECT pFileObject, OUT PIO_STATUS_BLOCK IoStatusBlock, IN PVOID Buffer, IN ULONG Length, IN PLARGE_INTEGER ByteOffset OPTIONAL)
    {
        NTSTATUS status = STATUS_SUCCESS;
        PIRP pIrp = NULL;
        KEVENT kEvent = { 0 };
        PIO_STACK_LOCATION pIoStackLocation = NULL;
        PDEVICE_OBJECT pDeviceObject = NULL;
        if ((NULL == pFileObject) ||
            (NULL == pFileObject->Vpb) ||
            (NULL == pFileObject->Vpb->DeviceObject)) {
            return STATUS_UNSUCCESSFUL;
        }

        if (NULL == ByteOffset) {
            if (0 == (FO_SYNCHRONOUS_IO & pFileObject->Flags)) {
                return STATUS_INVALID_PARAMETER;
            }
            ByteOffset = &pFileObject->CurrentByteOffset;
        }
        pDeviceObject = pFileObject->Vpb->DeviceObject;
        pIrp = IoAllocateIrp(pDeviceObject->StackSize, FALSE);
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
        pIrp->Tail.Overlay.OriginalFileObject = pFileObject;
        pIoStackLocation = IoGetNextIrpStackLocation(pIrp);
        pIoStackLocation->MajorFunction = IRP_MJ_WRITE;
        pIoStackLocation->MinorFunction = IRP_MN_NORMAL;
        pIoStackLocation->DeviceObject = pDeviceObject;
        pIoStackLocation->FileObject = pFileObject;
        pIoStackLocation->Parameters.Write.Length = Length;
        pIoStackLocation->Parameters.Write.ByteOffset = *ByteOffset;
        IoSetCompletionRoutine(pIrp, KiFileSystemIoRequestServiceIoRequestCompleteRoutine, NULL, TRUE, TRUE, TRUE);
        status = IoCallDriver(pDeviceObject, pIrp);
        if (STATUS_PENDING == status) {
            KeWaitForSingleObject(&kEvent, Executive, KernelMode, FALSE, NULL);
        }
        status = IoStatusBlock->Status;
        return status;
    }

    NTSTATUS IrpQueryInformationFile(IN PFILE_OBJECT pFileObject, OUT PIO_STATUS_BLOCK IoStatusBlock, OUT PVOID FileInformation, IN ULONG Length, IN FILE_INFORMATION_CLASS FileInformationClass)
    {
        NTSTATUS status = STATUS_SUCCESS;
        PIRP pIrp = NULL;
        KEVENT kEvent = { 0 };
        PIO_STACK_LOCATION pIoStackLocation = NULL;
        PDEVICE_OBJECT pDeviceObject = NULL;
        if ((NULL == pFileObject) ||
            (NULL == pFileObject->Vpb) ||
            (NULL == pFileObject->Vpb->DeviceObject)) {
            return STATUS_UNSUCCESSFUL;
        }
        pDeviceObject = pFileObject->Vpb->DeviceObject;
        pIrp = IoAllocateIrp(pDeviceObject->StackSize, FALSE);
        if (NULL == pIrp) {
            return STATUS_UNSUCCESSFUL;
        }
        KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);
        RtlZeroMemory(FileInformation, Length);
        pIrp->UserEvent = &kEvent;
        pIrp->UserIosb = IoStatusBlock;
        pIrp->AssociatedIrp.SystemBuffer = FileInformation;
        pIrp->RequestorMode = KernelMode;
        pIrp->Tail.Overlay.Thread = PsGetCurrentThread();
        pIrp->Tail.Overlay.OriginalFileObject = pFileObject;
        pIoStackLocation = IoGetNextIrpStackLocation(pIrp);
        pIoStackLocation->MajorFunction = IRP_MJ_QUERY_INFORMATION;
        pIoStackLocation->DeviceObject = pDeviceObject;
        pIoStackLocation->FileObject = pFileObject;
        pIoStackLocation->Parameters.QueryFile.Length = Length;
        pIoStackLocation->Parameters.QueryFile.FileInformationClass = FileInformationClass;
        IoSetCompletionRoutine(pIrp, KiFileSystemIoRequestServiceIoRequestCompleteRoutine, NULL, TRUE, TRUE, TRUE);
        status = IoCallDriver(pDeviceObject, pIrp);
        if (STATUS_PENDING == status) {
            KeWaitForSingleObject(&kEvent, Executive, KernelMode, FALSE, NULL);
        }
        status = IoStatusBlock->Status;
        return status;
    }

    NTSTATUS IrpSetInformationFile(IN PFILE_OBJECT pFileObject, OUT PIO_STATUS_BLOCK IoStatusBlock, IN PVOID FileInformation, IN ULONG Length, IN FILE_INFORMATION_CLASS FileInformationClass)
    {
        NTSTATUS status = STATUS_SUCCESS;
        PIRP pIrp = NULL;
        KEVENT kEvent = { 0 };
        PIO_STACK_LOCATION pIoStackLocation = NULL;
        PDEVICE_OBJECT pDeviceObject = NULL;
        if ((NULL == pFileObject) ||
            (NULL == pFileObject->Vpb) ||
            (NULL == pFileObject->Vpb->DeviceObject)) {
            return STATUS_UNSUCCESSFUL;
        }
        pDeviceObject = pFileObject->Vpb->DeviceObject;
        pIrp = IoAllocateIrp(pDeviceObject->StackSize, FALSE);
        if (pIrp == NULL) {
            return STATUS_UNSUCCESSFUL;
        }
        KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);
        pIrp->UserEvent = &kEvent;
        pIrp->UserIosb = IoStatusBlock;
        pIrp->AssociatedIrp.SystemBuffer = FileInformation;
        pIrp->RequestorMode = KernelMode;
        pIrp->Tail.Overlay.Thread = PsGetCurrentThread();
        pIrp->Tail.Overlay.OriginalFileObject = pFileObject;
        pIoStackLocation = IoGetNextIrpStackLocation(pIrp);
        pIoStackLocation->MajorFunction = IRP_MJ_SET_INFORMATION;
        pIoStackLocation->DeviceObject = pDeviceObject;
        pIoStackLocation->FileObject = pFileObject;
        pIoStackLocation->Parameters.SetFile.Length = Length;
        pIoStackLocation->Parameters.SetFile.FileInformationClass = FileInformationClass;
        IoSetCompletionRoutine(pIrp, KiFileSystemIoRequestServiceIoRequestCompleteRoutine, NULL, TRUE, TRUE, TRUE);
        status = IoCallDriver(pDeviceObject, pIrp);
        if (STATUS_PENDING == status) {
            KeWaitForSingleObject(&kEvent, Executive, KernelMode, FALSE, NULL);
        }
        status = IoStatusBlock->Status;
        return status;
    }

}