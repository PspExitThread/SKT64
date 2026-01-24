#include "KernelFeatureCodeSearch.h"

namespace KiFeatureCodeSearch
{
	PKISEARCH_MODULE_HANDLE KiOpenFeatureCodeSearchHandleW(PWCHAR Name) {
		if (g_KiGlobalServiceContext.pDriverObject == nullptr)
			return NULL;
		UNICODE_STRING ModuleName;
		RtlInitUnicodeString(&ModuleName, Name);
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
					PKISEARCH_MODULE_HANDLE pHandle = (PKISEARCH_MODULE_HANDLE)ExAllocatePool2(
						POOL_FLAG_NON_PAGED,
						sizeof(KISEARCH_MODULE_HANDLE),
						'MODH');
					if (pHandle != NULL) {
						pHandle->ModuleBase = pCurrentModule->DllBase;
						pHandle->ModuleSize = pCurrentModule->SizeOfImage;
						return pHandle;
					}
					return NULL;
				}
			}
			pCurrentListEntry = pCurrentListEntry->Flink;
		}
		return NULL;
	}


	PKISEARCH_MODULE_HANDLE KiOpenFeatureCodeSearchHandle(PUNICODE_STRING ModuleName) {
		if (g_KiGlobalServiceContext.pDriverObject == nullptr)
			return NULL;
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
				if (RtlCompareUnicodeString(&pCurrentModule->BaseDllName, ModuleName, TRUE) == 0) {
					PKISEARCH_MODULE_HANDLE pHandle = (PKISEARCH_MODULE_HANDLE)ExAllocatePool2(
						POOL_FLAG_NON_PAGED,
						sizeof(KISEARCH_MODULE_HANDLE),
						'MODH');
					if (pHandle != NULL) {
						pHandle->ModuleBase = pCurrentModule->DllBase;
						pHandle->ModuleSize = pCurrentModule->SizeOfImage;
						return pHandle;
					}
					return NULL;
				}
			}
			pCurrentListEntry = pCurrentListEntry->Flink;
		}
		return NULL;
	}

	VOID KiCloseFeatureCodeSearchHandle(PKISEARCH_MODULE_HANDLE Handle) {
		if (Handle != NULL) {
			ExFreePoolWithTag(Handle, 'MODH');
		}
	}

	PVOID KiSearchFeatureCode(PKISEARCH_MODULE_HANDLE Handle, PCCH Pattern, PCCH Mask)
	{
		if (Handle == NULL || Handle->ModuleBase == NULL || Handle->ModuleSize == NULL)
			return NULL;
		ULONG64 Address = KiGeneralService::FindPatternEx<ULONG64>(Handle->ModuleBase, Handle->ModuleSize, Pattern, Mask);
		return (PVOID)Address;
	}
}