#include "Driver.h"
#pragma warning(disable:4702) //fix STATUS_NOT_SUPPORTED unreachable code warning

VOID KiServiceDriverUnload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	if (g_KiGlobalServiceContext.g_TempDataBuffer) {
		ExFreePoolWithTag(g_KiGlobalServiceContext.g_TempDataBuffer, 'BTRK');
		g_KiGlobalServiceContext.g_TempDataBuffer = nullptr;
	}
	if (g_KiGlobalServiceContext.NTKernelModuleHandle) {
		KiFeatureCodeSearch::KiCloseFeatureCodeSearchHandle(g_KiGlobalServiceContext.NTKernelModuleHandle);
		g_KiGlobalServiceContext.NTKernelModuleHandle = nullptr;
	}
	if (g_KiGlobalStatusContext.VMMInitialized == TRUE) {
		KiGeneralService::GkiUninitializeVMM();
		g_KiGlobalStatusContext.VMMInitialized = FALSE;
	}
	UNICODE_STRING SymbolLinkName = RTL_CONSTANT_STRING(L"\\??\\GeneralKernelImplementation");
	IoDeleteSymbolicLink(&SymbolLinkName);
	IoDeleteDevice(pDriverObject->DeviceObject);
	RtlZeroMemory(&g_KiGlobalServiceContext, sizeof(KIGLOBAL_SERVICE_CONTEXT));
	RtlZeroMemory(&g_KiGlobalOffsetContext, sizeof(KIGLOBAL_OFFSET_CONTEXT));
	RtlZeroMemory(&g_KiGlobalStatusContext, sizeof(KISERVICE_STATUS_CONTEXT));
	RtlZeroMemory(&g_KiGlobalAddressContext, sizeof(KIGLOBAL_ADDRESS_CONTEXT));
}

EXTERN_C
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryString)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	UNREFERENCED_PARAMETER(pRegistryString);
	NTSTATUS status = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObject = NULL;
	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\GeneralKernelImplementation");
	UNICODE_STRING SymbolLinkName = RTL_CONSTANT_STRING(L"\\??\\GeneralKernelImplementation");
	RtlZeroMemory(&g_KiGlobalServiceContext, sizeof(KIGLOBAL_SERVICE_CONTEXT));
	status = KiGeneralService::GkiInitializeContext(pDriverObject);
	if (!NT_SUCCESS(status)) {
		RtlZeroMemory(&g_KiGlobalServiceContext, sizeof(KIGLOBAL_SERVICE_CONTEXT));
		return status;
	}
	g_KiGlobalServiceContext.NTBuildNumber = KiGeneralService::GkiGetBuildNumber();
	g_KiGlobalServiceContext.pIdleProcess = KiGeneralService::GkiGetIdleEprocess();
	g_KiGlobalServiceContext.KernelDefaultDispatchFunction = (ULONG64)pDriverObject->MajorFunction[IRP_MJ_CREATE];
	g_KiGlobalServiceContext.NTKernelBase = KiGeneralService::GetNTKernelBase();
	if (g_KiGlobalServiceContext.NTBuildNumber < 15063) {
		RtlZeroMemory(&g_KiGlobalServiceContext, sizeof(KIGLOBAL_SERVICE_CONTEXT));
		KeBugCheckEx(UNSUPPORTED_PROCESSOR, g_KiGlobalServiceContext.NTBuildNumber, (ULONG_PTR)DriverEntry, (ULONG_PTR)KiGeneralService::GkiGetBuildNumber, NULL);
		return STATUS_NOT_SUPPORTED;
	}
	if (g_KiGlobalServiceContext.NTBuildNumber >= 22000) {
		// Windows 11 x64
		g_KiGlobalOffsetContext.EPROCESS_ActiveProcessLinks = 0x1d8;
		g_KiGlobalOffsetContext.EPROCESS_UniqueProcessId = 0x1d0;
		g_KiGlobalOffsetContext.EPROCESS_Token = 0x248;
		g_KiGlobalOffsetContext.EPROCESS_Protection = 0x5fa;
		g_KiGlobalOffsetContext.EPROCESS_ObjectTable = 0x300;
		g_KiGlobalOffsetContext.EPROCESS_VadRoot = 0x558;
		g_KiGlobalOffsetContext.EPROCESS_VadCount = 0x568;
		g_KiGlobalOffsetContext.EPROCESS_Flags = 0x1f4;
		g_KiGlobalOffsetContext.ETHREAD_StartAddress = 0x4e0;
		g_KiGlobalOffsetContext.ETHREAD_Win32StartAddress = 0x560;
		g_KiGlobalOffsetContext.ETHREAD_Terminated = 0x5a0;
		g_KiGlobalOffsetContext.ETHREAD_Process = 0x220;
		g_KiGlobalOffsetContext.ETHREAD_ContextSwitches = 0x154;
		g_KiGlobalOffsetContext.ETHREAD_State = 0x184;
		g_KiGlobalOffsetContext.ETHREAD_PreviousMode = 0x232;
		g_KiGlobalOffsetContext.ETHREAD_ApcQueueable = 0x074;
		g_KiGlobalOffsetContext.PFILTER_Operation = 0x1b0;
	}
	status = KiGeneralService::InitMiniFilterConfig(pRegistryString, L"240000");
	if (NT_SUCCESS(status)) {
		g_KiGlobalStatusContext.MiniFilterConfigInitialized = TRUE;
	}
	pDriverObject->DriverUnload = KiServiceDriverUnload;
	g_KiGlobalServiceContext.pServiceDriverUnloadFunction = KiServiceDriverUnload;
	g_KiGlobalServiceContext.NTKernelModuleHandle = KiFeatureCodeSearch::KiOpenFeatureCodeSearchHandleW(L"ntoskrnl.exe");
	status = KiInitializeKernelServiceAddress::InitializeKernelServiceAddressContext();
	for (ULONG i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		pDriverObject->MajorFunction[i] = IoRequestControl::DriverDefaultHandle;
	}
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = IoRequestControl::DispatchControlCodeCreate;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoRequestControl::DispatchControlCode;
	status = IoCreateDevice(pDriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDeviceObject);
	if (NT_SUCCESS(status)) {
		pDeviceObject->Flags |= DO_BUFFERED_IO;
		status = IoCreateSymbolicLink(&SymbolLinkName, &DeviceName);
		if (!NT_SUCCESS(status)) {
			IoDeleteDevice(pDeviceObject);
		}
	}
	return status;
}