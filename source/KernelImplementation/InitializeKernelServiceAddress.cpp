#include "InitializeKernelServiceAddress.h"

namespace KiInitializeKernelServiceAddress
{
    ULONG64 KiGetPspCidTable() {
        ULONG64 PspCidTable = 0, i = 0;
        ULONG64 PspReferenceCidTableEntry = (ULONG64)PsLookupProcessByProcessId;
        if (g_KiGlobalServiceContext.NTBuildNumber >= 27943)
        {
            PspCidTable = (ULONG64)KiFeatureCodeSearch::KiSearchFeatureCode(g_KiGlobalServiceContext.NTKernelModuleHandle, "\xE8\x00\x00\x00\x00\x48\x8B\x0D\x00\x00\x00\x00\x00\x00\x00\x00\x00\x45\x33\xC9\x45\x33\xC0\x48", "x????xxx?????????xxxxxxx") + 5;
            if (MmIsAddressValid((PVOID)PspCidTable))
            {
                PspCidTable = KiGeneralService::ResolveRelativeAddress(PspCidTable, 3);
                return PspCidTable;
            }
        }
        if ((g_KiGlobalServiceContext.NTBuildNumber >= 26100 && g_KiGlobalServiceContext.NTBuildNumber < 27802) || g_KiGlobalServiceContext.NTBuildNumber >= 27881)
        {
            PspCidTable = KiGeneralService::FindPatternEx<ULONG64>(PsLookupProcessByProcessId, 0x50, "\x48\x8B\x05\x00\x00\x00\x00\xF7", "xxx????x");
            if (MmIsAddressValid((PVOID)PspCidTable))
            {
                PspCidTable = KiGeneralService::ResolveRelativeAddress(PspCidTable, 3);
                return PspCidTable;
            }
            return NULL;
        }
        if ((g_KiGlobalServiceContext.NTBuildNumber < 26100 || g_KiGlobalServiceContext.NTBuildNumber >= 27802) && g_KiGlobalServiceContext.NTBuildNumber < 27881)
        {
            if (PspReferenceCidTableEntry)
            {
                if (PspReferenceCidTableEntry)
                {
                    for (i = 0; i <= 0x100; i++)
                    {
                        if (*(UCHAR*)PspReferenceCidTableEntry == 0xE8 && *(UCHAR*)(PspReferenceCidTableEntry + 5) == 0x48 && *(UCHAR*)(PspReferenceCidTableEntry + 6) == 0x8B
                            && *(UCHAR*)(PspReferenceCidTableEntry + 7) == 0xD8 && *(UCHAR*)(PspReferenceCidTableEntry + 8) == 0x48)
                        {
                            PspReferenceCidTableEntry++;
                            PspCidTable = PspReferenceCidTableEntry = KiGeneralService::ResolveRelativeAddress(PspReferenceCidTableEntry, 0);
                        }
                        ++PspReferenceCidTableEntry;
                    }
                    if (MmIsAddressValid((PVOID)PspReferenceCidTableEntry))
                    {
                        for (i = 0; i <= 0x30; i++)
                        {
                            if (*(UCHAR*)PspCidTable == 0x48 && *(UCHAR*)(PspCidTable + 1) == 0x8B && *(UCHAR*)(PspCidTable + 2) == 0x05)
                            {
                                PspCidTable = KiGeneralService::ResolveRelativeAddress(PspCidTable, 3);
                                return PspCidTable;
                            }
                            ++PspCidTable;
                        }
                    }
                    return NULL;
                }
            }
        }
        return NULL;
    }

    ULONG64 KiGetPspTerminateThreadByPointer() {
        ULONG64 PsTerminateSystemThreadAddress = (ULONG64)PsTerminateSystemThread, i = 0;
        for (i = 0; i < 0x30; i++)
        {
            if (*(UCHAR*)PsTerminateSystemThreadAddress == 0xE8 && (*(UCHAR*)(PsTerminateSystemThreadAddress + 5) == 0x48 || *(UCHAR*)(PsTerminateSystemThreadAddress + 5) == 0xEB))
            {
                PsTerminateSystemThreadAddress++;
                return KiGeneralService::ResolveRelativeAddress(PsTerminateSystemThreadAddress, 0);
            }
            ++PsTerminateSystemThreadAddress;
        }
        return NULL;
    }

    ULONG64 KiGetPspNotifyEnableMask() {
        ULONG64 PspNotifyEnableMaskAddress = NULL;
        ULONG64 PsSetLoadImageNotifyRoutineExAddress = KiGeneralService::GetExportFunction(L"PsSetLoadImageNotifyRoutineEx");
        for (ULONG i = 0; i < 0x300; i++)
        {
            if (*(UCHAR*)PsSetLoadImageNotifyRoutineExAddress == 0x8B && (*(UCHAR*)(PsSetLoadImageNotifyRoutineExAddress + 1) == 0x05 && (*(UCHAR*)(PsSetLoadImageNotifyRoutineExAddress + 6) == 0xA8)))
            {
                PsSetLoadImageNotifyRoutineExAddress++;
                PspNotifyEnableMaskAddress = KiGeneralService::ResolveRelativeAddress(PsSetLoadImageNotifyRoutineExAddress, 1);
                return PspNotifyEnableMaskAddress;
            }
            ++PsSetLoadImageNotifyRoutineExAddress;
        }
        return PspNotifyEnableMaskAddress;
    }

    ULONG64 KiGetObpCallPreOperationCallbacks()
    {
        ULONG64 ObpCallPreOperationCallbacksAddress = NULL;
		ULONG64 ObpCallPreOperationCallbacksCaller = (ULONG64)KiFeatureCodeSearch::KiSearchFeatureCode(g_KiGlobalServiceContext.NTKernelModuleHandle, "\xE8\x00\x00\x00\x00\x85\xC0\x78\x00\x45\x84\x00\x75\x00\x8B", "x????xxx?xx?x?x");
        if (MmIsAddressValid((PVOID)ObpCallPreOperationCallbacksCaller)) {
			ObpCallPreOperationCallbacksAddress = KiGeneralService::ResolveRelativeAddress(ObpCallPreOperationCallbacksCaller, 1);
            if (MmIsAddressValid((PVOID)ObpCallPreOperationCallbacksAddress)) {
                return ObpCallPreOperationCallbacksAddress;
			}
        }
        return NULL;
	}

    ULONG64 KiGetIopUnloadDriver()
    {
        ULONG64 IopUnloadDriverAddress = NULL;
        ULONG64 IopUnloadDriverCaller = (ULONG64)KiFeatureCodeSearch::KiSearchFeatureCode(g_KiGlobalServiceContext.NTKernelModuleHandle, "\xE8\x00\x00\x00\x00\x48\x8B\x00\x00\x00\x33\xD2\xE8\x00\x00\x00\x00\xE9\x00\x00\x00\x00\xCC", "x????xx???xxx????x????x");
        if (MmIsAddressValid((PVOID)IopUnloadDriverCaller)) {
            IopUnloadDriverAddress = KiGeneralService::ResolveRelativeAddress(IopUnloadDriverCaller, 1);
            if (MmIsAddressValid((PVOID)IopUnloadDriverAddress)) {
                return IopUnloadDriverAddress;
            }
        }
        return NULL;
    }

    ULONG64 KiGetPspCreateProcessNotifyRoutine()
    {
        ULONG64 PsSetCreateProcessNotifyRoutineAddress = KiGeneralService::GetExportFunction(L"PsSetCreateProcessNotifyRoutine");
        ULONG64 PspSetCreateProcessNotifyRoutineAddress = NULL;
        if (MmIsAddressValid((PVOID)PsSetCreateProcessNotifyRoutineAddress)) {
            for (ULONG i = 0; i < 0x300; i++) {
                if (*(UCHAR*)PsSetCreateProcessNotifyRoutineAddress == 0xE8 && (*(UCHAR*)(PsSetCreateProcessNotifyRoutineAddress + 5) == 0x48)) {
                    PspSetCreateProcessNotifyRoutineAddress = KiGeneralService::ResolveRelativeAddress(PsSetCreateProcessNotifyRoutineAddress, 1);
                    break;
                }
                ++PsSetCreateProcessNotifyRoutineAddress;
            }
		}
        if (PspSetCreateProcessNotifyRoutineAddress != NULL && MmIsAddressValid((PVOID)PspSetCreateProcessNotifyRoutineAddress)) {
            for (ULONG i = 0; i < 0x300; i++) {
                if (*(UCHAR*)PspSetCreateProcessNotifyRoutineAddress == 0x4C && (*(UCHAR*)(PspSetCreateProcessNotifyRoutineAddress + 1) == 0x8D)) {
                    ULONG64 PspCreateProcessNotifyRoutineAddress = KiGeneralService::ResolveRelativeAddress(PspSetCreateProcessNotifyRoutineAddress, 3);
                    if (MmIsAddressValid((PVOID)PspCreateProcessNotifyRoutineAddress)) {
                        return PspCreateProcessNotifyRoutineAddress;
					}
                }
                ++PspSetCreateProcessNotifyRoutineAddress;
            }
		}
        return NULL;
    }

    ULONG64 KiGetPspCreateThreadNotifyRoutine()
    {
        ULONG64 PsRemoveCreateThreadNotifyRoutineAddress = KiGeneralService::GetExportFunction(L"PsRemoveCreateThreadNotifyRoutine");
        if (MmIsAddressValid((PVOID)PsRemoveCreateThreadNotifyRoutineAddress)) {
            for (ULONG i = 0; i < 0x300; i++) {
                if (*(UCHAR*)PsRemoveCreateThreadNotifyRoutineAddress == 0x48 && (*(UCHAR*)(PsRemoveCreateThreadNotifyRoutineAddress + 1) == 0x8D) && (*(UCHAR*)(PsRemoveCreateThreadNotifyRoutineAddress + 2) == 0x0D)) {
                    ULONG64 PspCreateThreadNotifyRoutineAddress = KiGeneralService::ResolveRelativeAddress(PsRemoveCreateThreadNotifyRoutineAddress, 3);
                    if (MmIsAddressValid((PVOID)PspCreateThreadNotifyRoutineAddress)) {
                        return PspCreateThreadNotifyRoutineAddress;
					}
                    break;
                }
                ++PsRemoveCreateThreadNotifyRoutineAddress;
            }
        }
        return NULL;
    }

    ULONG64 KiGetCmCallbackListHead()
    {
        ULONG64 CmUnRegisterCallbackAddress = KiGeneralService::GetExportFunction(L"CmUnRegisterCallback");
        if (MmIsAddressValid((PVOID)CmUnRegisterCallbackAddress)) {
            for (ULONG i = 0; i < 0x300; i++) {
                if (*(UCHAR*)CmUnRegisterCallbackAddress == 0x48 && (*(UCHAR*)(CmUnRegisterCallbackAddress + 1) == 0x8D) && (*(UCHAR*)(CmUnRegisterCallbackAddress + 2) == 0x0D)) {
                    ULONG64 CallbackListHeadAddress = KiGeneralService::ResolveRelativeAddress(CmUnRegisterCallbackAddress, 3);
                    if (MmIsAddressValid((PVOID)CallbackListHeadAddress)) {
                        return CallbackListHeadAddress;
                    }
                    break;
                }
                ++CmUnRegisterCallbackAddress;
            }
        }
        return NULL;
    }

    ULONG64 KiGetPspLoadImageNotifyRoutine()
    {
        ULONG64 PsSetLoadImageNotifyRoutineExAddress = KiGeneralService::GetExportFunction(L"PsSetLoadImageNotifyRoutineEx");
        if (MmIsAddressValid((PVOID)PsSetLoadImageNotifyRoutineExAddress)) {
            for (ULONG i = 0; i < 0x300; i++) {
                if (*(UCHAR*)PsSetLoadImageNotifyRoutineExAddress == 0x48 && (*(UCHAR*)(PsSetLoadImageNotifyRoutineExAddress + 1) == 0x8D) && (*(UCHAR*)(PsSetLoadImageNotifyRoutineExAddress + 2) == 0x0D)) {
                    ULONG64 PspLoadImageNotifyRoutineAddress = KiGeneralService::ResolveRelativeAddress(PsSetLoadImageNotifyRoutineExAddress, 3);
                    if (MmIsAddressValid((PVOID)PspLoadImageNotifyRoutineAddress)) {
                        return PspLoadImageNotifyRoutineAddress;
                    }
                    break;
                }
                ++PsSetLoadImageNotifyRoutineExAddress;
            }
        }
        return NULL;
    }

	NTSTATUS InitializeKernelServiceAddressContext() {
		NTSTATUS status = STATUS_SUCCESS;
		g_KiGlobalAddressContext.NTOS_PspCidTable = KiGetPspCidTable();
		g_KiGlobalAddressContext.NTOS_PspTerminateThreadByPointer = KiGetPspTerminateThreadByPointer();
        g_KiGlobalAddressContext.NTOS_ObpCallPreOperationCallbacks = KiGetObpCallPreOperationCallbacks();
		g_KiGlobalAddressContext.NTOS_PspNotifyEnableMask = KiGetPspNotifyEnableMask();
		g_KiGlobalAddressContext.NTOS_IopUnloadDriver = KiGetIopUnloadDriver();
		g_KiGlobalAddressContext.NTOS_PspCreateProcessNotifyRoutine = KiGetPspCreateProcessNotifyRoutine();
		g_KiGlobalAddressContext.NTOS_PspCreateThreadNotifyRoutine = KiGetPspCreateThreadNotifyRoutine();
		g_KiGlobalAddressContext.NTOS_CmCallbackListHead = KiGetCmCallbackListHead();
		g_KiGlobalAddressContext.NTOS_PspLoadImageNotifyRoutine = KiGetPspLoadImageNotifyRoutine();
		return status;
	}
}