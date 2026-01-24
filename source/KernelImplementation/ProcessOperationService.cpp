#include "ProcessOperationService.h"

namespace KiProcessOperation
{
	NTSTATUS DisableProcessApcInsert(PEPROCESS pProcess, SIZE_T ListSize, PULONG64 SystemInformationLength)
	{
		UNREFERENCED_PARAMETER(pProcess);
		NTSTATUS status = STATUS_SUCCESS; //default success
		if (pProcess != NULL) {
			PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(ListSize);
			if (CidTableInfo) {
				for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
					if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoThread) {
						if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsThreadType) {
							PEPROCESS ThreadProcess = IoThreadToProcess((PETHREAD)CidTableInfo[PsIndex].Object);
							if (ThreadProcess != NULL && ObGetObjectType(ThreadProcess) == *PsProcessType) {
								if (*(ULONG64*)((ULONG64)ThreadProcess + g_KiGlobalOffsetContext.EPROCESS_ObjectTable) != NULL) {
									if (ThreadProcess == pProcess) {
										ULONG64 ApcQueueableValue = *(PULONG64)((ULONG64)CidTableInfo[PsIndex].Object + g_KiGlobalOffsetContext.ETHREAD_ApcQueueable) & 0xFFFFFFFFFBFFF;
										*(PULONG64)((ULONG64)CidTableInfo[PsIndex].Object + g_KiGlobalOffsetContext.ETHREAD_ApcQueueable) = ApcQueueableValue;
									}
								}
							}
						}
					}
				}
				ExFreePoolWithTag(CidTableInfo, 'CIDT');
			}
			if (SystemInformationLength != NULL) {
				if (KiGeneralService::IsAddressInUserSpace(SystemInformationLength)) {
					__try {
						ProbeForWrite(SystemInformationLength, sizeof(ULONG64), 1);
						if (SystemInformationLength != NULL) {
							*SystemInformationLength = 0;
						}
					}
					__except (1) {
						status = GetExceptionCode();
					}
				}
				else {
					if (MmIsAddressValid(SystemInformationLength)) {
						if (SystemInformationLength != NULL) {
							*SystemInformationLength = 0;
						}
					}
				}
				if (SystemInformationLength != NULL) {
					*SystemInformationLength = 0;
				}
			}
		}
		return status;
	}
}