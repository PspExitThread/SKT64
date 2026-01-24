#pragma once
#include <ntifs.h>
#include "GeneralService.h"

typedef enum _PSPCIDTABLE_INFO_TYPE
{
	PspCidTableInfoProcess = 0,
	PspCidTableInfoThread = 1,
	PspCidTableInfoUnknown = 2
} PSPCIDTABLE_INFO_TYPE;

typedef struct _PSPCIDTABLE_INFO
{
	ULONG PsID;
	ULONG64 Object;
	PSPCIDTABLE_INFO_TYPE InfoType;
} PSPCIDTABLE_INFO, * PPSPCIDTABLE_INFO;

namespace KiPspCidTableParse
{
	VOID Parse_Table1(ULONG64 Address, INT index1, INT index2, PPSPCIDTABLE_INFO InfoList);
	VOID Parse_Table2(ULONG64 Address, INT index2, PPSPCIDTABLE_INFO InfoList);
	VOID Parse_Table3(ULONG64 Address, PPSPCIDTABLE_INFO InfoList);
	PPSPCIDTABLE_INFO ParseCidTable(SIZE_T ListSize);
	namespace RemovePspCidTableObject {
		VOID RemoveParse_Table1(ULONG64 Address, INT index1, INT index2, PVOID Object);
		VOID RemoveParse_Table2(ULONG64 Address, INT index2, PVOID Object);
		VOID RemoveParse_Table3(ULONG64 Address, PVOID Object);
		NTSTATUS RemoveObject(PVOID Object);
	}
}