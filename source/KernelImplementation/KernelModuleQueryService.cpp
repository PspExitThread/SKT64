#include "KernelModuleQuery.h"

namespace KiKernelModuleQueryService 
{
	NTSTATUS ParseDirectoryObjectModules(PKIKERNEL_MODULE_INFO ModuleInfoList, PVOID pDirectoryObject, POBJECT_TYPE IoDirectoryObjectType) {
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		BOOLEAN AlreadyExists = FALSE;
		BOOLEAN DriverInserted = FALSE;
		if (pDirectoryObject != NULL && ModuleInfoList != NULL) {
			__try {
				POBJECT_DIRECTORY pObjectDirectory = (POBJECT_DIRECTORY)pDirectoryObject;
				for (ULONG HashBucketIndex = 0; HashBucketIndex < 37; HashBucketIndex++) {
					POBJECT_DIRECTORY_ENTRY pDirectoryEntry = pObjectDirectory->HashBuckets[HashBucketIndex];
					for (; (ULONG_PTR)pDirectoryEntry > 0 && MmIsAddressValid(pDirectoryEntry); pDirectoryEntry = pDirectoryEntry->ChainLink) {
						if (MmIsAddressValid(pDirectoryEntry->Object)) {
							POBJECT_TYPE pObjectType = (POBJECT_TYPE)ObGetObjectType(pDirectoryEntry->Object);
							if (pObjectType == IoDirectoryObjectType) {
								ParseDirectoryObjectModules(ModuleInfoList, pDirectoryEntry->Object, IoDirectoryObjectType);
							}
							else if (pObjectType == *IoDriverObjectType) {
								PDEVICE_OBJECT pDeviceObject = NULL;
								PDRIVER_OBJECT pCurrentDriverObject = (PDRIVER_OBJECT)pDirectoryEntry->Object;
								for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
									if (ModuleInfoList[ModuleIndex].pDriverObject != NULL && ModuleInfoList[ModuleIndex].pDriverObject == pCurrentDriverObject) {
										AlreadyExists = TRUE;
										break;
									}
								}
								if (!AlreadyExists) {
									for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
										if (pCurrentDriverObject->DriverStart != NULL) {
											if (ModuleInfoList[ModuleIndex].BaseAddress == (ULONG64)pCurrentDriverObject->DriverStart) {
												ModuleInfoList[ModuleIndex].pDriverObject = pCurrentDriverObject;
												DriverInserted = TRUE;
												break;
											}
										}
									}
									if (!DriverInserted) {
										PLDR_DATA_TABLE_ENTRY pLdrEntry = (PLDR_DATA_TABLE_ENTRY)pCurrentDriverObject->DriverSection;
										if (pLdrEntry) {
											ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].BaseAddress = (ULONG64)pLdrEntry->DllBase;
											ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].EntryPoint = (ULONG64)pCurrentDriverObject->DriverStart;
											ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].SizeOfImage = pLdrEntry->SizeOfImage;
											RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName), &pLdrEntry->BaseDllName);
											RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName), &pLdrEntry->FullDllName);
											ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].pDriverObject = pCurrentDriverObject;
											g_KiGlobalServiceContext.KernelModuleInfoListCount++;
										}
									}
									DriverInserted = FALSE;
								}
								AlreadyExists = FALSE;
								for (pDeviceObject = ((PDRIVER_OBJECT)pDirectoryEntry->Object)->DeviceObject; pDeviceObject && MmIsAddressValid(pDeviceObject); pDeviceObject = pDeviceObject->AttachedDevice) {
									for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
										if (ModuleInfoList[ModuleIndex].pDriverObject != NULL && ModuleInfoList[ModuleIndex].pDriverObject == pCurrentDriverObject) {
											AlreadyExists = TRUE;
											break;
										}
									}
									if (!AlreadyExists) {
										for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
											if (pCurrentDriverObject->DriverStart != NULL) {
												if (ModuleInfoList[ModuleIndex].BaseAddress == (ULONG64)pCurrentDriverObject->DriverStart) {
													ModuleInfoList[ModuleIndex].pDriverObject = pCurrentDriverObject;
													DriverInserted = TRUE;
													break;
												}
											}
										}
										if (!DriverInserted) {
											PLDR_DATA_TABLE_ENTRY pLdrEntry = (PLDR_DATA_TABLE_ENTRY)pCurrentDriverObject->DriverSection;
											if (pLdrEntry) {
												ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].BaseAddress = (ULONG64)pLdrEntry->DllBase;
												ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].EntryPoint = (ULONG64)pCurrentDriverObject->DriverStart;
												ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].SizeOfImage = pLdrEntry->SizeOfImage;
												RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName), &pLdrEntry->BaseDllName);
												RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName), &pLdrEntry->FullDllName);
												ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].pDriverObject = pCurrentDriverObject;
												g_KiGlobalServiceContext.KernelModuleInfoListCount++;
											}
										}
										DriverInserted = FALSE;
									}
								}
							}
							else if (pObjectType == *IoDeviceObjectType) {
								PDEVICE_OBJECT pDeviceObject = (PDEVICE_OBJECT)pDirectoryEntry->Object;
								for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
									if (ModuleInfoList[ModuleIndex].pDriverObject != NULL && ModuleInfoList[ModuleIndex].pDriverObject == pDeviceObject->DriverObject) {
										AlreadyExists = TRUE;
										break;
									}
								}
								if (!AlreadyExists) {
									for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
										if (pDeviceObject->DriverObject->DriverStart != NULL) {
											if (ModuleInfoList[ModuleIndex].BaseAddress == (ULONG64)pDeviceObject->DriverObject->DriverStart) {
												ModuleInfoList[ModuleIndex].pDriverObject = pDeviceObject->DriverObject;
												DriverInserted = TRUE;
												break;
											}
										}
									}
									if (!DriverInserted) {
										PLDR_DATA_TABLE_ENTRY pLdrEntry = (PLDR_DATA_TABLE_ENTRY)pDeviceObject->DriverObject->DriverSection;
										if (pLdrEntry) {
											ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].BaseAddress = (ULONG64)pLdrEntry->DllBase;
											ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].EntryPoint = (ULONG64)pDeviceObject->DriverObject->DriverStart;
											ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].SizeOfImage = pLdrEntry->SizeOfImage;
											RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName), &pLdrEntry->BaseDllName);
											RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName), &pLdrEntry->FullDllName);
											ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].pDriverObject = pDeviceObject->DriverObject;
											g_KiGlobalServiceContext.KernelModuleInfoListCount++;
										}
									}
									DriverInserted = FALSE;
								}
								AlreadyExists = FALSE;
								for (pDeviceObject = ((PDEVICE_OBJECT)pDirectoryEntry->Object)->AttachedDevice; pDeviceObject && MmIsAddressValid(pDeviceObject); pDeviceObject = pDeviceObject->AttachedDevice) {
									for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
										if (ModuleInfoList[ModuleIndex].pDriverObject != NULL && ModuleInfoList[ModuleIndex].pDriverObject == pDeviceObject->DriverObject) {
											AlreadyExists = TRUE;
											break;
										}
									}
									if (!AlreadyExists) {
										for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
											if (pDeviceObject->DriverObject->DriverStart != NULL) {
												if (ModuleInfoList[ModuleIndex].BaseAddress == (ULONG64)pDeviceObject->DriverObject->DriverStart) {
													ModuleInfoList[ModuleIndex].pDriverObject = pDeviceObject->DriverObject;
													DriverInserted = TRUE;
													break;
												}
											}
										}
										if (!DriverInserted) {
											PLDR_DATA_TABLE_ENTRY pLdrEntry = (PLDR_DATA_TABLE_ENTRY)pDeviceObject->DriverObject->DriverSection;
											if (pLdrEntry) {
												ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].BaseAddress = (ULONG64)pLdrEntry->DllBase;
												ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].EntryPoint = (ULONG64)pDeviceObject->DriverObject->DriverStart;
												ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].SizeOfImage = pLdrEntry->SizeOfImage;
												RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName), &pLdrEntry->BaseDllName);
												RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName), &pLdrEntry->FullDllName);
												ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].pDriverObject = pDeviceObject->DriverObject;
												g_KiGlobalServiceContext.KernelModuleInfoListCount++;
											}
										}
										DriverInserted = FALSE;
									}
								}
							}
						}
					}
				}
			}
			__except (1) {
				status = GetExceptionCode();
			}
		}
		return status;
	}

	PKIKERNEL_MODULE_INFO QueryKernelModuleInformation(SIZE_T ListSize) {
		NTSTATUS status = STATUS_SUCCESS;
		OBJECT_ATTRIBUTES objectAttributes = { 0 };
		UNICODE_STRING RootObjectDirectoryName = RTL_CONSTANT_STRING(L"\\");
		HANDLE RootObjectDirectoryHandle = NULL;
		PVOID pRootDirectoryObject = NULL;
		if (ListSize % sizeof(KIKERNEL_MODULE_INFO) != 0) {
			ListSize = ListSize - (ListSize % sizeof(KIKERNEL_MODULE_INFO));
			if (ListSize < sizeof(KIKERNEL_MODULE_INFO) * 500)
				return NULL;
		}
        g_KiGlobalServiceContext.KernelModuleInfoListCount = 0;
        g_KiGlobalServiceContext.KernelModuleInfoListMaxCount = (ULONG64)(ListSize / sizeof(KIKERNEL_MODULE_INFO));
		PKIKERNEL_MODULE_INFO ModuleInfoList = (PKIKERNEL_MODULE_INFO)ExAllocatePool2(POOL_FLAG_NON_PAGED, ListSize, 'KMQL');
		if (!ModuleInfoList)
			return NULL;
		PLDR_DATA_TABLE_ENTRY LdrEntry = NULL, LdrEntryFirst = NULL;
		if (!MmIsAddressValid(PsLoadedModuleList)) {
			ExFreePoolWithTag(ModuleInfoList, 'KMQL');
			return NULL;
		}
		LdrEntry = LdrEntryFirst = (PLDR_DATA_TABLE_ENTRY)PsLoadedModuleList->Flink;
        __try {
            do {
                if (LdrEntry->DllBase != NULL && LdrEntry->SizeOfImage > 0) {
					if (g_KiGlobalServiceContext.KernelModuleInfoListCount >= g_KiGlobalServiceContext.KernelModuleInfoListMaxCount) {
						break;
					}
					ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].BaseAddress = (ULONG64)LdrEntry->DllBase;
					ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].EntryPoint = (ULONG64)LdrEntry->EntryPoint;
					ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].SizeOfImage = LdrEntry->SizeOfImage;
					RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].ModuleName), &LdrEntry->BaseDllName);
					RtlStringCbCopyUnicodeString(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName, sizeof(ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].FullModuleName), &LdrEntry->FullDllName);
                    if (ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].BaseAddress == g_KiGlobalServiceContext.NTKernelBase) {
                        PDRIVER_OBJECT NTKernelDriverObject = NULL;
                        UNICODE_STRING UnicodeName = RTL_CONSTANT_STRING(L"\\FileSystem\\RAW");
                        ObReferenceObjectByName(&UnicodeName, OBJ_CASE_INSENSITIVE, 0, 0, *IoDriverObjectType, KernelMode, 0, reinterpret_cast<PVOID*>(&NTKernelDriverObject));
						ModuleInfoList[g_KiGlobalServiceContext.KernelModuleInfoListCount].pDriverObject = NTKernelDriverObject;
                    }
					g_KiGlobalServiceContext.KernelModuleInfoListCount++;
                }
				LdrEntry = (PLDR_DATA_TABLE_ENTRY)LdrEntry->InLoadOrderLinks.Flink;
            } while (LdrEntry && LdrEntry != LdrEntryFirst);
        }
        __except (1) {
            status = GetExceptionCode();
        }
		if (g_KiGlobalServiceContext.KernelModuleInfoListCount > 0) {
			InitializeObjectAttributes(&objectAttributes, &RootObjectDirectoryName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
			status = ZwOpenDirectoryObject(&RootObjectDirectoryHandle, 0, &objectAttributes);
			if (NT_SUCCESS(status) && RootObjectDirectoryHandle != NULL) {
				status = ObReferenceObjectByHandle(RootObjectDirectoryHandle, 0x10000000, 0, KernelMode, &pRootDirectoryObject, NULL);
				if (NT_SUCCESS(status) && pRootDirectoryObject != NULL) {
					status = ParseDirectoryObjectModules(ModuleInfoList, pRootDirectoryObject, (POBJECT_TYPE)ObGetObjectType(pRootDirectoryObject));
					ObDereferenceObject(pRootDirectoryObject);
				}
				ZwClose(RootObjectDirectoryHandle);
			}
		}
		return ModuleInfoList;
	}

	VOID ExecuteDriverUnloadThread(IN PVOID lpParam)
	{
		PDRIVER_OBJECT pDriverObject = (PDRIVER_OBJECT)lpParam;
		PDEVICE_OBJECT DeviceObject = NULL;
		if (pDriverObject) {
			PDRIVER_UNLOAD DriverUnload = pDriverObject->DriverUnload;
			if (DriverUnload) {
				DriverUnload(pDriverObject);
				pDriverObject->FastIoDispatch = NULL;
				memset(pDriverObject->MajorFunction, 0, sizeof(pDriverObject->MajorFunction));
				pDriverObject->DriverUnload = NULL;
				ObMakeTemporaryObject(pDriverObject);
				ObfDereferenceObject(pDriverObject);
			}
			else {
				pDriverObject->FastIoDispatch = NULL;
				memset(pDriverObject->MajorFunction, 0, sizeof(pDriverObject->MajorFunction));
				pDriverObject->DriverUnload = NULL;
				DeviceObject = pDriverObject->DeviceObject;
				while (DeviceObject && MmIsAddressValid(DeviceObject)) {
					IoDeleteDevice(DeviceObject);
					DeviceObject = DeviceObject->NextDevice;
				}
				ObMakeTemporaryObject(pDriverObject);
				ObfDereferenceObject(pDriverObject);
			}
		}

		PsTerminateSystemThread(STATUS_SUCCESS);
	}

	NTSTATUS ForceUnloadDriver(PDRIVER_OBJECT pDriverObject) {
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		if (pDriverObject != NULL) {
			if (MmIsAddressValid(pDriverObject) && MmIsAddressValid(pDriverObject->DriverSection) && ObGetObjectType(pDriverObject) == *IoDriverObjectType) {
				HANDLE SystemThreadHandle = NULL;
				status = PsCreateSystemThread(&SystemThreadHandle, 0, NULL, NULL, NULL, ExecuteDriverUnloadThread, pDriverObject);
				if (NT_SUCCESS(status))
				{
					PETHREAD pThread = NULL;
					status = ObReferenceObjectByHandle(SystemThreadHandle, 0, NULL, KernelMode, (PVOID*)&pThread, NULL);
					if (NT_SUCCESS(status)) {
						LARGE_INTEGER timeout = { 0 };
						timeout.QuadPart = -10 * 1000 * 1000 * 3;
						status = KeWaitForSingleObject(pThread, Executive, KernelMode, TRUE, &timeout);
						ObfDereferenceObject(pThread);
					}
					ZwClose(SystemThreadHandle);
				}
			}
		}
		return status;
	}
}