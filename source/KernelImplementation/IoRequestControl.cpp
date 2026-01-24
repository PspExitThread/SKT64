#include "IoRequestControl.h"


namespace IoRequestControl
{
    NTSTATUS DriverDefaultHandle(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
    {
        UNREFERENCED_PARAMETER(pDeviceObject);
        NTSTATUS status = STATUS_SUCCESS;
        pIrp->IoStatus.Status = status;
        pIrp->IoStatus.Information = 0;
        IoCompleteRequest(pIrp, IO_NO_INCREMENT);
        return status;
    }

    NTSTATUS DispatchControlCodeCreate(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
    {
        UNREFERENCED_PARAMETER(pDeviceObject);
        pIrp->IoStatus.Information = 0;
        pIrp->IoStatus.Status = STATUS_SUCCESS;
        IoCompleteRequest(pIrp, IO_NO_INCREMENT);
        return STATUS_SUCCESS;
    }

    NTSTATUS DispatchControlCode(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
    {
        UNREFERENCED_PARAMETER(DeviceObject);
        NTSTATUS status = STATUS_SUCCESS;
        PIO_STACK_LOCATION irpStack = NULL;
        ULONG IoControlCode;
        irpStack = IoGetCurrentIrpStackLocation(Irp);
        IoControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

        switch (IoControlCode) {
        case IOCTL_QUERY_SYSTEM_INFORMATION:
        {
            typedef struct _KIIO_INPUT
            {
                ULONG_PTR DATASize;
                PVOID DATAPoint;
            }KIIO_INPUT, * PKIIO_INPUT;
            PKIIO_INPUT pInput = NULL;
            __try {
                pInput = (PKIIO_INPUT)Irp->AssociatedIrp.SystemBuffer;
                ProbeForWrite(pInput->DATAPoint, pInput->DATASize, 1);
                PKIIO_INPUT_DATA pIoData = (PKIIO_INPUT_DATA)pInput->DATAPoint;
                status = KiInterface::GkiQuerySystemInformation(pIoData->SystemInformationClass, pIoData->SystemInformation, pIoData->SystemInformationLength, pIoData->Parameter1, pIoData->Parameter2,
                    pIoData->Parameter3, pIoData->Parameter4, pIoData->Parameter5, pIoData->Parameter6, pIoData->Parameter7, pIoData->Parameter8, pIoData->Parameter9, pIoData->Parameter10);
            }
            __except (1)
            {
                status = GetExceptionCode();
            }
            break;
        }
        case IOCTL_SET_SYSTEM_INFORMATION:
        {
            typedef struct _KIIO_INPUT
            {
                ULONG_PTR DATASize;
                PVOID DATAPoint;
            }KIIO_INPUT, * PKIIO_INPUT;
            PKIIO_INPUT pInput = NULL;
            __try {
                pInput = (PKIIO_INPUT)Irp->AssociatedIrp.SystemBuffer;
                ProbeForWrite(pInput->DATAPoint, pInput->DATASize, 1);
                PKIIO_INPUT_DATA pIoData = (PKIIO_INPUT_DATA)pInput->DATAPoint;
                status = KiInterface::GkiSetSystemInformation(pIoData->SystemInformationClass, pIoData->SystemInformation, pIoData->SystemInformationLength, pIoData->Parameter1, pIoData->Parameter2,
                    pIoData->Parameter3, pIoData->Parameter4, pIoData->Parameter5, pIoData->Parameter6, pIoData->Parameter7, pIoData->Parameter8, pIoData->Parameter9, pIoData->Parameter10);
            }
            __except (1)
            {
                status = GetExceptionCode();
            }
            break;
        }
        default:
            break;
        }
        Irp->IoStatus.Status = status;
        if (irpStack)
        {
            if (status == STATUS_SUCCESS)
                Irp->IoStatus.Information = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
            else
                Irp->IoStatus.Information = 0;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
        }
        return status;
    }
}