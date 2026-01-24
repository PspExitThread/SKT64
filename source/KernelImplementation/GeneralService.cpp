#include "GeneralService.h"


EXTERN_C KIGLOBAL_SERVICE_CONTEXT g_KiGlobalServiceContext = { 0 };
EXTERN_C KIGLOBAL_OFFSET_CONTEXT g_KiGlobalOffsetContext = { 0 };
EXTERN_C KISERVICE_STATUS_CONTEXT g_KiGlobalStatusContext = { 0 };
EXTERN_C KIGLOBAL_ADDRESS_CONTEXT g_KiGlobalAddressContext = { 0 };
EXTERN_C __vmm_context* g_vmm_context = 0;

namespace KiGeneralService
{
	NTSTATUS GkiInitializeContext(PDRIVER_OBJECT pDriverObject)
	{
		if (pDriverObject == nullptr)
			return STATUS_INVALID_PARAMETER;
		RtlZeroMemory(&g_KiGlobalServiceContext, sizeof(KIGLOBAL_SERVICE_CONTEXT));
		RtlZeroMemory(&g_KiGlobalOffsetContext, sizeof(KIGLOBAL_OFFSET_CONTEXT));
		RtlZeroMemory(&g_KiGlobalStatusContext, sizeof(KISERVICE_STATUS_CONTEXT));
		RtlZeroMemory(&g_KiGlobalAddressContext, sizeof(KIGLOBAL_ADDRESS_CONTEXT));
		g_KiGlobalServiceContext.pDriverObject = pDriverObject;
		g_KiGlobalServiceContext.g_TempDataBuffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, 409600, 'BTRK');
		g_KiGlobalOffsetContext.EPROCESS_ActiveProcessLinks = 0x448; //default for Windows 10 x64
		g_KiGlobalOffsetContext.EPROCESS_UniqueProcessId = 0x440;
		g_KiGlobalOffsetContext.EPROCESS_Token = 0x4b8;
		g_KiGlobalOffsetContext.EPROCESS_Protection = 0x87a;
		g_KiGlobalOffsetContext.EPROCESS_ObjectTable = 0x570;
		g_KiGlobalOffsetContext.EPROCESS_VadRoot = 0x7d8;
		g_KiGlobalOffsetContext.EPROCESS_VadCount = 0x7e8;
		g_KiGlobalOffsetContext.EPROCESS_Flags = 0x464;
		g_KiGlobalOffsetContext.ETHREAD_StartAddress = 0x4a0;
		g_KiGlobalOffsetContext.ETHREAD_Win32StartAddress = 0x520;
		g_KiGlobalOffsetContext.ETHREAD_Terminated = 0x560;
		g_KiGlobalOffsetContext.ETHREAD_Process = 0x220;
		g_KiGlobalOffsetContext.ETHREAD_ContextSwitches = 0x154;
		g_KiGlobalOffsetContext.ETHREAD_State = 0x184;
		g_KiGlobalOffsetContext.ETHREAD_PreviousMode = 0x232;
		g_KiGlobalOffsetContext.ETHREAD_ApcQueueable = 0x074;
		g_KiGlobalOffsetContext.PFILTER_Operation = 0x1A8;
		g_KiGlobalStatusContext.MiniFilterConfigInitialized = FALSE;
		g_KiGlobalStatusContext.VMMInitialized = FALSE;
		return STATUS_SUCCESS;
	}

	ULONG GkiGetBuildNumber()
	{
		RTL_OSVERSIONINFOW osi = { 0 };
		osi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
		RtlGetVersion(&osi);
		return osi.dwBuildNumber;
	}

	PEPROCESS GkiGetIdleEprocess()
	{
		PEPROCESS pEprocess;
		PKPCR pKPCR = (PKPCR)__readmsr(0xC0000101);
		PETHREAD pIdle_ETHREAD = (PETHREAD)(*(ULONG64*)((ULONG64)pKPCR->CurrentPrcb + 0x18));
		pEprocess = IoThreadToProcess(pIdle_ETHREAD);
		return pEprocess;
	}

	NTSTATUS InitMiniFilterConfig(IN PUNICODE_STRING RegistryString, IN PWSTR Altitude)
	{
		NTSTATUS Status = STATUS_UNSUCCESSFUL;
		WCHAR RegText[MAX_PATH] = { 0 }, DataText[MAX_PATH] = { 0 };
		ULONG64 RegData = 0;
		PWSTR NeedText = NULL;

		NeedText = wcsrchr(RegistryString->Buffer, L'\\');
		if (!MmIsAddressValid(NeedText))
			return STATUS_INVALID_PARAMETER;

		RtlStringCbPrintfW(RegText, NTSTRSAFE_MAX_CCH * sizeof(WCHAR), L"%ws\\Instances", NeedText);
		Status = RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, RegText);

		RtlStringCbPrintfW(DataText, NTSTRSAFE_MAX_CCH * sizeof(WCHAR), L"%ws Instance", NeedText + 1);
		RegData = (wcslen(DataText) + 1) * sizeof(WCHAR);
		Status = RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, RegText, L"DefaultInstance", REG_SZ, DataText, (ULONG)RegData);

		RtlStringCbPrintfW(RegText, NTSTRSAFE_MAX_CCH * sizeof(WCHAR), L"%ws\\Instances%ws Instance\0", NeedText, NeedText);
		Status = RtlCreateRegistryKey(RTL_REGISTRY_SERVICES, RegText);
		RegData = (wcslen(Altitude) + 1) * sizeof(WCHAR);
		Status = RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, RegText, L"Altitude", REG_SZ, Altitude, (ULONG)RegData);
		RegData = 0;
		Status = RtlWriteRegistryValue(RTL_REGISTRY_SERVICES, RegText, L"Flags", REG_DWORD, &RegData, sizeof(RegData));

		return Status;
	}

	ULONG64 FindPattern(ULONG64 base, SIZE_T size, PCHAR pattern)
	{
		//find pattern utils
#define InRange(x, a, b) (x >= a && x <= b) 
#define GetBits(x) (InRange(x, '0', '9') ? (x - '0') : ((x - 'A') + 0xA))
#define GetByte(x) ((BYTE)(GetBits(x[0]) << 4 | GetBits(x[1])))

		PBYTE ModuleStart = (PBYTE)base;
		PBYTE ModuleEnd = (PBYTE)(ModuleStart + size);

		//scan pattern main
		PBYTE FirstMatch = nullptr;
		const char* CurPatt = pattern;
		for (; ModuleStart < ModuleEnd; ++ModuleStart)
		{
			bool SkipByte = (*CurPatt == '\?');
			if (SkipByte || *ModuleStart == GetByte(CurPatt)) {
				if (!FirstMatch) FirstMatch = ModuleStart;
				SkipByte ? CurPatt += 2 : CurPatt += 3;
				if (CurPatt[-1] == 0) return (ULONG64)FirstMatch;
			}

			else if (FirstMatch) {
				ModuleStart = FirstMatch;
				FirstMatch = nullptr;
				CurPatt = pattern;
			}
		}
		return NULL;
	}

	ULONG64 GetNTKernelBase()
	{
		if (g_KiGlobalServiceContext.pDriverObject == nullptr)
			return NULL;
		UNICODE_STRING ModuleName;
		RtlInitUnicodeString(&ModuleName, L"ntoskrnl.exe");
		PLDR_DATA_TABLE_ENTRY pLdr = NULL;
		PLIST_ENTRY pListEntry = NULL;
		PLIST_ENTRY pCurrentListEntry = NULL;
		PLDR_DATA_TABLE_ENTRY pCurrentModule = NULL;
		pLdr = (PLDR_DATA_TABLE_ENTRY)g_KiGlobalServiceContext.pDriverObject->DriverSection;
		pListEntry = pLdr->InLoadOrderLinks.Flink;
		pCurrentListEntry = pListEntry->Flink;
		while (pCurrentListEntry != pListEntry) {
			pCurrentModule = CONTAINING_RECORD(pCurrentListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
			if (pCurrentModule->BaseDllName.Buffer != 0) {
				if (RtlCompareUnicodeString(&pCurrentModule->BaseDllName, &ModuleName, TRUE) == 0) {
					return (ULONG64)pCurrentModule->DllBase;
				}
			}
			pCurrentListEntry = pCurrentListEntry->Flink;
		}
		return NULL;
	}

	NTSTATUS GkiInitializeVMM() {
		if (VMM_Init()) {
			g_KiGlobalStatusContext.VMMInitialized = TRUE;
			return STATUS_SUCCESS;
		}
		return STATUS_UNSUCCESSFUL;
	}

	NTSTATUS GkiUninitializeVMM() {
		if (g_vmm_context) {
			if (g_vmm_context->vcpu_table[0]->vcpu_status.vmm_launched) {
				HV::RemoveAllHook();
				HV::VMOFF();
			}
		}
		HV::Disable_vmx();
		Free_vmm_context();
		g_KiGlobalStatusContext.VMMInitialized = FALSE;
		return STATUS_SUCCESS;
	}

	ULONG64 ResolveRelativeAddress(ULONG64 Address, ULONG Offset)
	{
		if (Address)
			return Address + *(INT*)(Address + Offset) + Offset + sizeof(INT);

		return 0;
	}

	BOOLEAN IsAddressInUserSpace(PVOID Address)
	{
		if (Address == NULL) {
			return FALSE;
		}
		return ((ULONG_PTR)Address < (ULONG_PTR)MmUserProbeAddress);
	}

	NTSTATUS GetFileNameFromUnicodeStringPath(PCUNICODE_STRING FullPath, PUNICODE_STRING FileName) {
		if (FullPath == NULL || FileName == NULL) {
			return STATUS_INVALID_PARAMETER;
		}

		if (FullPath->Length == 0 || FullPath->Buffer == NULL) {
			FileName->Buffer = NULL;
			FileName->Length = 0;
			FileName->MaximumLength = 0;
			return STATUS_SUCCESS;
		}

		PWCHAR lastBackslash = NULL;
		for (PWCHAR p = FullPath->Buffer; p < &FullPath->Buffer[FullPath->Length / sizeof(WCHAR)]; p++) {
			if (*p == L'\\') {
				lastBackslash = p;
			}
		}

		if (lastBackslash != NULL) {
			PWCHAR fileNameStart = lastBackslash + 1;

			SIZE_T remainingLength = FullPath->Length -
				((fileNameStart - FullPath->Buffer) * sizeof(WCHAR));

			FileName->Buffer = fileNameStart;
			FileName->Length = (USHORT)min(remainingLength, FullPath->Length);
			FileName->MaximumLength = FileName->Length;
		}
		else {
			FileName->Buffer = FullPath->Buffer;
			FileName->Length = FullPath->Length;
			FileName->MaximumLength = FullPath->MaximumLength;
		}

		return STATUS_SUCCESS;
	}

	BOOLEAN KernelSleepImp(ULONG MillionSecond)
	{
		NTSTATUS status;
		LARGE_INTEGER DelayTime;
		DelayTime = RtlConvertLongToLargeInteger(-10000 * MillionSecond);
		status = KeDelayExecutionThread(KernelMode, FALSE, &DelayTime);
		return (NT_SUCCESS(status));
	}
}