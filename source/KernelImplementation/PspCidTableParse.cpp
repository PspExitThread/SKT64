#include "PspCidTableParse.h"

namespace KiPspCidTableParse
{
    VOID Parse_Table1(ULONG64 Address, INT index1, INT index2, PPSPCIDTABLE_INFO InfoList)
    {
        INT PID = 0;
        for (INT i = 0; i < 256; i++) {
            if (!MmIsAddressValid((PVOID64)(Address + i * 16)))
                continue;
            ULONG64 pRecord = *(PULONG64)(Address + i * 16);
            ULONG64 pObject = (LONG64)pRecord >> 0x10;
            pObject &= 0xfffffffffffffff0;
            PID = i * 4 + 1024 * index1 + 512 * index2 * 1024;
            if (pObject != NULL) {
                if (g_KiGlobalServiceContext.PspCidTableInfoListCount <= g_KiGlobalServiceContext.PspCidTableInfoListMaxCount) {
                    if (ObGetObjectType((PVOID)pObject) == *PsProcessType) {
                        InfoList[g_KiGlobalServiceContext.PspCidTableInfoListCount].PsID = PID;
                        InfoList[g_KiGlobalServiceContext.PspCidTableInfoListCount].Object = pObject;
                        InfoList[g_KiGlobalServiceContext.PspCidTableInfoListCount].InfoType = PspCidTableInfoProcess;
                        g_KiGlobalServiceContext.PspCidTableInfoListCount++;
                    }
                    else if (ObGetObjectType((PVOID)pObject) == *PsThreadType) {
                        InfoList[g_KiGlobalServiceContext.PspCidTableInfoListCount].PsID = PID;
                        InfoList[g_KiGlobalServiceContext.PspCidTableInfoListCount].Object = pObject;
                        InfoList[g_KiGlobalServiceContext.PspCidTableInfoListCount].InfoType = PspCidTableInfoThread;
                        g_KiGlobalServiceContext.PspCidTableInfoListCount++;
                    }
                }
            }
        }
    }

    VOID Parse_Table2(ULONG64 Address, INT index2, PPSPCIDTABLE_INFO InfoList)
    {
        ULONG64 Table1Address = 0;
        for (INT i = 0; i < 512; i++) {
            if (!MmIsAddressValid((PVOID64)(Address + i * 8)))
                continue;
            if (!MmIsAddressValid((PVOID64) * (PULONG64)(Address + i * 8)))
                continue;
            Table1Address = *(PULONG64)(Address + i * 8);
            Parse_Table1(Table1Address, i, index2, InfoList);
        }
    }

    VOID Parse_Table3(ULONG64 Address, PPSPCIDTABLE_INFO InfoList) {
        ULONG64 Table2Address = 0;
        for (INT i = 0; i < 512; i++) {
            if (!MmIsAddressValid((PVOID64)(Address + i * 8)))
                continue;
            if (!MmIsAddressValid((PVOID64) * (PULONG64)(Address + i * 8)))
                continue;
            Table2Address = *(PULONG64)(Address + i * 8);
            Parse_Table2(Table2Address, i, InfoList);
        }
    }

    PPSPCIDTABLE_INFO ParseCidTable(SIZE_T ListSize)
    {
        if (ListSize % sizeof(PSPCIDTABLE_INFO) != 0) {
            ListSize = ListSize - (ListSize % sizeof(PSPCIDTABLE_INFO));
            if (ListSize < sizeof(PSPCIDTABLE_INFO) * 100)
                return NULL;
        }
        PPSPCIDTABLE_INFO InfoList = (PPSPCIDTABLE_INFO)ExAllocatePool2(POOL_FLAG_NON_PAGED, ListSize, 'CIDT');
        if (!InfoList)
			return NULL;
        g_KiGlobalServiceContext.PspCidTableInfoListCount = 0;
		g_KiGlobalServiceContext.PspCidTableInfoListMaxCount = (ULONG64)(ListSize / sizeof(PSPCIDTABLE_INFO));
        ULONG64 pTableCode = *(PULONG64)(((ULONG64) * (PULONG64)g_KiGlobalAddressContext.NTOS_PspCidTable) + 8);
        INT TableIndex = pTableCode & 3;
        switch (TableIndex)
		{
		case 0:
            Parse_Table1(pTableCode & (~3), 0, 0, InfoList);
            break;
        case 1:
            Parse_Table2(pTableCode & (~3), 0, InfoList);
            break;
		case 2:
            Parse_Table3(pTableCode & (~3), InfoList);
            break;
        default:
            return NULL;
        }
        return InfoList;
    }

    namespace RemovePspCidTableObject {
        VOID RemoveParse_Table1(ULONG64 Address, INT index1, INT index2, PVOID Object) {
			UNREFERENCED_PARAMETER(index1);
			UNREFERENCED_PARAMETER(index2);
            for (INT i = 0; i < 256; i++) {
                if (!MmIsAddressValid((PVOID64)(Address + i * 16)))
                    continue;
                ULONG64 pRecord = *(PULONG64)(Address + i * 16);
                ULONG64 pObject = (LONG64)pRecord >> 0x10;
                pObject &= 0xfffffffffffffff0;
                if (pObject != NULL) {
                    if (pObject == (ULONG64)Object) {
                        *(PULONG64)(Address + i * 16) = 0; //clear entry
                    }
                }
            }
        }

        VOID RemoveParse_Table2(ULONG64 Address, INT index2, PVOID Object) {
            ULONG64 Table1Address = 0;
            for (INT i = 0; i < 512; i++) {
                if (!MmIsAddressValid((PVOID64)(Address + i * 8)))
                    continue;
                if (!MmIsAddressValid((PVOID64) * (PULONG64)(Address + i * 8)))
                    continue;
                Table1Address = *(PULONG64)(Address + i * 8);
                RemoveParse_Table1(Table1Address, i, index2, Object);
            }
        }

        VOID RemoveParse_Table3(ULONG64 Address, PVOID Object) {
            ULONG64 Table2Address = 0;
            for (INT i = 0; i < 512; i++) {
                if (!MmIsAddressValid((PVOID64)(Address + i * 8)))
                    continue;
                if (!MmIsAddressValid((PVOID64) * (PULONG64)(Address + i * 8)))
                    continue;
                Table2Address = *(PULONG64)(Address + i * 8);
                RemoveParse_Table2(Table2Address, i, Object);
            }
        }

        NTSTATUS RemoveObject(PVOID Object) {
            ULONG64 pTableCode = *(PULONG64)(((ULONG64) * (PULONG64)g_KiGlobalAddressContext.NTOS_PspCidTable) + 8);
            INT TableIndex = pTableCode & 3;
            switch (TableIndex)
            {
            case 0:
                RemoveParse_Table1(pTableCode & (~3), 0, 0, Object);
                break;
            case 1:
                RemoveParse_Table2(pTableCode & (~3), 0, Object);
                break;
            case 2:
                RemoveParse_Table3(pTableCode & (~3), Object);
                break;
            default:
                return NULL;
            }
            return STATUS_SUCCESS;
        }
    }
}