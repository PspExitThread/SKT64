#include "KernelInterface.h"
#pragma warning(disable:4702)

namespace KiInterface
{
	NTSTATUS SetDefaultSystemInformationLength(PULONG64 SystemInformationLength) {
		NTSTATUS status = STATUS_SUCCESS;
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
		return status;
	}

	NTSTATUS GkiQuerySystemInformation(GKISYSTEM_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
		ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10)
	{
		UNREFERENCED_PARAMETER(SystemInformation);
		UNREFERENCED_PARAMETER(SystemInformationLength);
		UNREFERENCED_PARAMETER(Parameter1);
		UNREFERENCED_PARAMETER(Parameter2);
		UNREFERENCED_PARAMETER(Parameter3);
		UNREFERENCED_PARAMETER(Parameter4);
		UNREFERENCED_PARAMETER(Parameter5);
		UNREFERENCED_PARAMETER(Parameter6);
		UNREFERENCED_PARAMETER(Parameter7);
		UNREFERENCED_PARAMETER(Parameter8);
		UNREFERENCED_PARAMETER(Parameter9);
		UNREFERENCED_PARAMETER(Parameter10);

		NTSTATUS status = STATUS_UNSUCCESSFUL;
		switch (InformationClass)
		{
			case GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformation:
			{
				ULONG64 ListCount = 0;
				PVOID pFilePoint = NULL;
				POBJECT_NAME_INFORMATION pObjectNameInfo = NULL;
				UNICODE_STRING ProcessName;
				if (Parameter1 == UserMode && KiGeneralService::IsAddressInUserSpace(SystemInformation)) {
					__try {
						ProbeForWrite(SystemInformation, Parameter2, 1);
						PQUERY_PROCESS_INFO QueryProcessInfoList = (PQUERY_PROCESS_INFO)SystemInformation;
						PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(Parameter2);
						if (CidTableInfo) {
							for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
								if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoProcess) {
									if (*(ULONG64*)((ULONG64)CidTableInfo[PsIndex].Object + g_KiGlobalOffsetContext.EPROCESS_ObjectTable) != NULL) { //skip terminated process
										QueryProcessInfoList[ListCount].PPROCESSOBJECT = (PVOID)CidTableInfo[PsIndex].Object;
										QueryProcessInfoList[ListCount].PROCESSPID = CidTableInfo[PsIndex].PsID;
										status = PsReferenceProcessFilePointer((PEPROCESS)CidTableInfo[PsIndex].Object, &pFilePoint);
										if (NT_SUCCESS(status) && pFilePoint != NULL) {
											status = IoQueryFileDosDeviceName((PFILE_OBJECT)pFilePoint, &pObjectNameInfo);
											if (NT_SUCCESS(status)) {
												RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSPATH, sizeof(QueryProcessInfoList[ListCount].PROCESSPATH), &pObjectNameInfo->Name);
												status = KiGeneralService::GetFileNameFromUnicodeStringPath(&pObjectNameInfo->Name, &ProcessName);
												if (NT_SUCCESS(status)) {
													RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSNAME, sizeof(QueryProcessInfoList[ListCount].PROCESSNAME), &ProcessName);
												}
												if (pObjectNameInfo) {
													ExFreePool(pObjectNameInfo);
													pObjectNameInfo = NULL;
												}
											}
											else {
												UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)CidTableInfo[PsIndex].Object);
												if (ansiName) {
													ANSI_STRING ansiString;
													UNICODE_STRING unicodeString;
													RtlInitAnsiString(&ansiString, (PCSZ)ansiName);
													unicodeString.Buffer = QueryProcessInfoList[ListCount].PROCESSNAME;
													unicodeString.MaximumLength = sizeof(QueryProcessInfoList[ListCount].PROCESSNAME);
													status = RtlAnsiStringToUnicodeString(&unicodeString, &ansiString, FALSE); //not RtlFreeUnicodeString
													if (!NT_SUCCESS(status)) {
														QueryProcessInfoList[ListCount].PROCESSNAME[0] = L'\0';
													}
													RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSPATH, sizeof(QueryProcessInfoList[ListCount].PROCESSPATH), &unicodeString); //path
													RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSNAME, sizeof(QueryProcessInfoList[ListCount].PROCESSNAME), &unicodeString); //name
												}
											}
										}
										else {
											UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)CidTableInfo[PsIndex].Object);
											if (ansiName) {
												ANSI_STRING ansiString;
												UNICODE_STRING unicodeString;
												RtlInitAnsiString(&ansiString, (PCSZ)ansiName);
												unicodeString.Buffer = QueryProcessInfoList[ListCount].PROCESSNAME;
												unicodeString.MaximumLength = sizeof(QueryProcessInfoList[ListCount].PROCESSNAME);
												status = RtlAnsiStringToUnicodeString(&unicodeString, &ansiString, FALSE); //not RtlFreeUnicodeString
												if (!NT_SUCCESS(status)) {
													QueryProcessInfoList[ListCount].PROCESSNAME[0] = L'\0';
												}
												RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSPATH, sizeof(QueryProcessInfoList[ListCount].PROCESSPATH), &unicodeString); //path
												RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSNAME, sizeof(QueryProcessInfoList[ListCount].PROCESSNAME), &unicodeString); //name
											}
										}
										if (pFilePoint != NULL) {
											ObDereferenceObject(pFilePoint);
											pFilePoint = NULL;
										}
										ListCount++;
									}
								}
							}
						}
						if (CidTableInfo) {
							ExFreePoolWithTag(CidTableInfo, 'CIDT');
						}
					}
					__except (1) {
						status = GetExceptionCode();
						if (pObjectNameInfo) {
							ExFreePool(pObjectNameInfo);
							pObjectNameInfo = NULL;
						}
						if (pFilePoint != NULL) {
							ObDereferenceObject(pFilePoint);
							pFilePoint = NULL;
						}
					}
				}
				else if (Parameter1 == KernelMode) {
					if (MmIsAddressValid(SystemInformation))
					{
						PQUERY_PROCESS_INFO QueryProcessInfoList = (PQUERY_PROCESS_INFO)SystemInformation;
						PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(Parameter2);
						if (CidTableInfo) {
							for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
								if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoProcess) {
									if (*(ULONG64*)((ULONG64)CidTableInfo[PsIndex].Object + g_KiGlobalOffsetContext.EPROCESS_ObjectTable) != NULL) { //skip terminated process
										QueryProcessInfoList[ListCount].PPROCESSOBJECT = (PVOID)CidTableInfo[PsIndex].Object;
										QueryProcessInfoList[ListCount].PROCESSPID = CidTableInfo[PsIndex].PsID;
										status = PsReferenceProcessFilePointer((PEPROCESS)CidTableInfo[PsIndex].Object, &pFilePoint);
										if (NT_SUCCESS(status) && pFilePoint != NULL) {
											status = IoQueryFileDosDeviceName((PFILE_OBJECT)pFilePoint, &pObjectNameInfo);
											if (NT_SUCCESS(status)) {
												RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSPATH, sizeof(QueryProcessInfoList[ListCount].PROCESSPATH), &pObjectNameInfo->Name);
												status = KiGeneralService::GetFileNameFromUnicodeStringPath(&pObjectNameInfo->Name, &ProcessName);
												if (NT_SUCCESS(status)) {
													RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSNAME, sizeof(QueryProcessInfoList[ListCount].PROCESSNAME), &ProcessName);
												}
												if (pObjectNameInfo) {
													ExFreePool(pObjectNameInfo);
													pObjectNameInfo = NULL;
												}
											}
											else {
												UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)CidTableInfo[PsIndex].Object);
												if (ansiName) {
													ANSI_STRING ansiString;
													UNICODE_STRING unicodeString;
													RtlInitAnsiString(&ansiString, (PCSZ)ansiName);
													unicodeString.Buffer = QueryProcessInfoList[ListCount].PROCESSNAME;
													unicodeString.MaximumLength = sizeof(QueryProcessInfoList[ListCount].PROCESSNAME);
													status = RtlAnsiStringToUnicodeString(&unicodeString, &ansiString, FALSE); //not RtlFreeUnicodeString
													if (!NT_SUCCESS(status)) {
														QueryProcessInfoList[ListCount].PROCESSNAME[0] = L'\0';
													}
													RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSPATH, sizeof(QueryProcessInfoList[ListCount].PROCESSPATH), &unicodeString); //path
													RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSNAME, sizeof(QueryProcessInfoList[ListCount].PROCESSNAME), &unicodeString); //name
												}
											}
										}
										else {
											UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)CidTableInfo[PsIndex].Object);
											if (ansiName) {
												ANSI_STRING ansiString;
												UNICODE_STRING unicodeString;
												RtlInitAnsiString(&ansiString, (PCSZ)ansiName);
												unicodeString.Buffer = QueryProcessInfoList[ListCount].PROCESSNAME;
												unicodeString.MaximumLength = sizeof(QueryProcessInfoList[ListCount].PROCESSNAME);
												status = RtlAnsiStringToUnicodeString(&unicodeString, &ansiString, FALSE); //not RtlFreeUnicodeString
												if (!NT_SUCCESS(status)) {
													QueryProcessInfoList[ListCount].PROCESSNAME[0] = L'\0';
												}
												RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSPATH, sizeof(QueryProcessInfoList[ListCount].PROCESSPATH), &unicodeString); //path
												RtlStringCbCopyUnicodeString(QueryProcessInfoList[ListCount].PROCESSNAME, sizeof(QueryProcessInfoList[ListCount].PROCESSNAME), &unicodeString); //name
											}
										}
										if (pFilePoint != NULL) {
											ObDereferenceObject(pFilePoint);
											pFilePoint = NULL;
										}
										ListCount++;
									}
								}
							}
						}
						if (CidTableInfo) {
							ExFreePoolWithTag(CidTableInfo, 'CIDT');
						}
					}
				}
				if (Parameter1 == UserMode && KiGeneralService::IsAddressInUserSpace(SystemInformationLength)) {
					__try {
						ProbeForWrite(SystemInformationLength, sizeof(ULONG64), 1);
						if (SystemInformationLength != NULL) {
							*SystemInformationLength = ListCount;
						}
					}
					__except (1) {
						status = GetExceptionCode();
					}
				}
				else if (Parameter1 == KernelMode) {
					if (MmIsAddressValid(SystemInformationLength)) {
						if (SystemInformationLength != NULL) {
							*SystemInformationLength = ListCount;
						}
					}
				}
				break;
			}
			case GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformationEx:
			{
				status = KiQueryProcessEx::GkiQueryProcessEx(SystemInformation, SystemInformationLength, Parameter1, Parameter2);
				break;
			}
			case GKISYSTEM_INFORMATION_CLASS::GkiSystemKernelModuleInformation:
			{
				ULONG64 ListCount = 0;
				if (Parameter1 == UserMode && KiGeneralService::IsAddressInUserSpace(SystemInformation)) {
					__try {
						ProbeForWrite(SystemInformation, Parameter2, 1);
						PKIKERNEL_MODULE_INFO QueryKernelModuleInfoList = (PKIKERNEL_MODULE_INFO)SystemInformation;
						PKIKERNEL_MODULE_INFO pKernelModuleList = KiKernelModuleQueryService::QueryKernelModuleInformation(Parameter2);
						if (pKernelModuleList) {
							for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
								QueryKernelModuleInfoList[ListCount].BaseAddress = pKernelModuleList[ModuleIndex].BaseAddress;
								QueryKernelModuleInfoList[ListCount].EntryPoint = pKernelModuleList[ModuleIndex].EntryPoint;
								QueryKernelModuleInfoList[ListCount].pDriverObject = pKernelModuleList[ModuleIndex].pDriverObject;
								QueryKernelModuleInfoList[ListCount].SizeOfImage = pKernelModuleList[ModuleIndex].SizeOfImage;
								RtlStringCbCopyW(QueryKernelModuleInfoList[ListCount].FullModuleName, sizeof(QueryKernelModuleInfoList[ListCount].FullModuleName), pKernelModuleList[ModuleIndex].FullModuleName);
								RtlStringCbCopyW(QueryKernelModuleInfoList[ListCount].ModuleName, sizeof(QueryKernelModuleInfoList[ListCount].ModuleName), pKernelModuleList[ModuleIndex].ModuleName);
								ListCount++;
							}
							status = STATUS_SUCCESS;
							ExFreePoolWithTag(pKernelModuleList, 'KMQL');
						}
					}
					__except (1) {
						status = GetExceptionCode();
					}
				}
				else if (Parameter1 == KernelMode) {
					if (MmIsAddressValid(SystemInformation)) {
						PKIKERNEL_MODULE_INFO QueryKernelModuleInfoList = (PKIKERNEL_MODULE_INFO)SystemInformation;
						PKIKERNEL_MODULE_INFO pKernelModuleList = KiKernelModuleQueryService::QueryKernelModuleInformation(Parameter2);
						if (pKernelModuleList) {
							for (ULONG ModuleIndex = 0; ModuleIndex < g_KiGlobalServiceContext.KernelModuleInfoListCount; ModuleIndex++) {
								QueryKernelModuleInfoList[ListCount].BaseAddress = pKernelModuleList[ModuleIndex].BaseAddress;
								QueryKernelModuleInfoList[ListCount].EntryPoint = pKernelModuleList[ModuleIndex].EntryPoint;
								QueryKernelModuleInfoList[ListCount].pDriverObject = pKernelModuleList[ModuleIndex].pDriverObject;
								QueryKernelModuleInfoList[ListCount].SizeOfImage = pKernelModuleList[ModuleIndex].SizeOfImage;
								RtlStringCbCopyW(QueryKernelModuleInfoList[ListCount].FullModuleName, sizeof(QueryKernelModuleInfoList[ListCount].FullModuleName), pKernelModuleList[ModuleIndex].FullModuleName);
								RtlStringCbCopyW(QueryKernelModuleInfoList[ListCount].ModuleName, sizeof(QueryKernelModuleInfoList[ListCount].ModuleName), pKernelModuleList[ModuleIndex].ModuleName);
								ListCount++;
							}
							status = STATUS_SUCCESS;
							ExFreePoolWithTag(pKernelModuleList, 'KMQL');
						}
					}
				}
				if (Parameter1 == UserMode && KiGeneralService::IsAddressInUserSpace(SystemInformationLength)) {
					__try {
						ProbeForWrite(SystemInformationLength, sizeof(ULONG64), 1);
						if (SystemInformationLength != NULL) {
							*SystemInformationLength = ListCount;
						}
					}
					__except (1) {
						status = GetExceptionCode();
					}
				}
				else if (Parameter1 == KernelMode) {
					if (MmIsAddressValid(SystemInformationLength)) {
						if (SystemInformationLength != NULL) {
							*SystemInformationLength = ListCount;
						}
					}
				}
				break;
			}
			case GKISYSTEM_INFORMATION_CLASS::GkiSystemMiniFilterInformation:
			{
				break;
			}
			case GKISYSTEM_INFORMATION_CLASS::GkiSystemKernelCallbackInformation:
			{
				break;
			}
			case GKISYSTEM_INFORMATION_CLASS::GkiSystemKernelNotifyInformation:
			{
				break;
			}

		default:
			break;
		}
		return status;
	}

	NTSTATUS GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
		ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10)
	{
		UNREFERENCED_PARAMETER(SystemInformation);
		UNREFERENCED_PARAMETER(SystemInformationLength);
		UNREFERENCED_PARAMETER(Parameter1);
		UNREFERENCED_PARAMETER(Parameter2);
		UNREFERENCED_PARAMETER(Parameter3);
		UNREFERENCED_PARAMETER(Parameter4);
		UNREFERENCED_PARAMETER(Parameter5);
		UNREFERENCED_PARAMETER(Parameter6);
		UNREFERENCED_PARAMETER(Parameter7);
		UNREFERENCED_PARAMETER(Parameter8);
		UNREFERENCED_PARAMETER(Parameter9);
		UNREFERENCED_PARAMETER(Parameter10);

		NTSTATUS status = STATUS_UNSUCCESSFUL;
		switch (InformationClass)
		{
		case GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformation:
		{
			switch (Parameter1)
			{
				case GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessTerminate:
				{
					if (Parameter2 != NULL) {
						HANDLE pProcessHandle;
						status = ObOpenObjectByPointer((PVOID)Parameter2, NULL, NULL, STANDARD_RIGHTS_ALL, *PsProcessType, KernelMode, &pProcessHandle);
						if (NT_SUCCESS(status) && pProcessHandle != NULL) {
							status = ZwTerminateProcess(pProcessHandle, STATUS_SUCCESS);
							ZwClose(pProcessHandle); //Please do not ObDereferenceObject, this will cause the system to crash.
						}
						if (!NT_SUCCESS(status) && status != STATUS_OBJECT_TYPE_MISMATCH && status != STATUS_INVALID_HANDLE && status != STATUS_PROCESS_IS_TERMINATING) {
							auto pPspTerminateThreadByPointer = reinterpret_cast<NTSTATUS(*)(PETHREAD, NTSTATUS, BOOLEAN)>(g_KiGlobalAddressContext.NTOS_PspTerminateThreadByPointer);
							if (pPspTerminateThreadByPointer) {
								PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(Parameter3);
								if (CidTableInfo) {
									for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
										if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoThread) {
											if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsThreadType) {
												PEPROCESS ThreadProcess = IoThreadToProcess((PETHREAD)CidTableInfo[PsIndex].Object);
												if (ThreadProcess != NULL && ObGetObjectType(ThreadProcess) == *PsProcessType) {
													if (*(ULONG64*)((ULONG64)ThreadProcess + g_KiGlobalOffsetContext.EPROCESS_ObjectTable) != NULL) {
														if (ThreadProcess == (PEPROCESS)Parameter2) {
															pPspTerminateThreadByPointer((PETHREAD)CidTableInfo[PsIndex].Object, STATUS_SUCCESS, TRUE);
														}
													}
												}
											}
										}
									}
									ExFreePoolWithTag(CidTableInfo, 'CIDT');
								}
							}
						}
					}
					if (!NT_SUCCESS(status)) {
						SetDefaultSystemInformationLength(SystemInformationLength);
					}
					else {
						status = SetDefaultSystemInformationLength(SystemInformationLength);
					}
					break;
				}
				case GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessDisableApcInsert:
				{
					status = KiProcessOperation::DisableProcessApcInsert((PEPROCESS)Parameter2, Parameter3, SystemInformationLength);
					break;
				}
				case GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessSetBreakOnTermination:
				{
					if (Parameter2 != NULL) {
						HANDLE pProcessHandle;
						status = ObOpenObjectByPointer((PVOID)Parameter2, NULL, NULL, STANDARD_RIGHTS_ALL, *PsProcessType, KernelMode, &pProcessHandle);
						if (NT_SUCCESS(status) && pProcessHandle != NULL) {
							ULONG BreakOnTermination = (ULONG)Parameter3; //This thing is actually a pointer.
							status = ZwSetInformationProcess(pProcessHandle, ProcessBreakOnTermination, &BreakOnTermination, sizeof(ULONG));
							ZwClose(pProcessHandle);
						}
					}
					if (!NT_SUCCESS(status)) {
						SetDefaultSystemInformationLength(SystemInformationLength);
					}
					else {
						status = SetDefaultSystemInformationLength(SystemInformationLength);
					}
					break;
				}
				case GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessTerminateDeleteSourceFile:
				{
					PFILE_OBJECT pFileObject = NULL;
					PFILE_OBJECT pDeleteAccessFileObject = NULL;
					IO_STATUS_BLOCK IoStatus = { 0 };
					FILE_BASIC_INFORMATION FileBasicInfo = { 0 };
					FILE_DISPOSITION_INFORMATION FileDispositionInfo = { 0 };
					POBJECT_NAME_INFORMATION pObjectNameInfo = NULL;
					SECTION_OBJECT_POINTERS pSectionObjPointers = { 0 };
					HANDLE pProcessHandle;
					if (Parameter2 != NULL) {
						status = PsReferenceProcessFilePointer((PEPROCESS)Parameter2, (PVOID*)&pFileObject); //Read Only File Object
						if (NT_SUCCESS(status) && pFileObject != NULL) {
							status = IoQueryFileDosDeviceName((PFILE_OBJECT)pFileObject, &pObjectNameInfo);
							if (NT_SUCCESS(status)) {
								status = ObOpenObjectByPointer((PVOID)Parameter2, NULL, NULL, STANDARD_RIGHTS_ALL, *PsProcessType, KernelMode, &pProcessHandle);
								if (NT_SUCCESS(status) && pProcessHandle != NULL) {
									status = ZwTerminateProcess(pProcessHandle, STATUS_SUCCESS);
									ZwClose(pProcessHandle); //Please do not ObDereferenceObject, this will cause the system to crash.
								}
								FileBasicInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;
								FileDispositionInfo.DeleteFile = TRUE;
								status = KiFileSystemIoRequestService::IrpCreateFile(&pDeleteAccessFileObject, DELETE | FILE_READ_ATTRIBUTES | SYNCHRONIZE, &pObjectNameInfo->Name, &IoStatus, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
								if (NT_SUCCESS(status) && pDeleteAccessFileObject != NULL) {
									status = KiFileSystemIoRequestService::IrpSetInformationFile(pDeleteAccessFileObject, &IoStatus, &FileBasicInfo, sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
									if (NT_SUCCESS(status)) {
										if (pFileObject->SectionObjectPointer != NULL) {
											pSectionObjPointers.DataSectionObject = pFileObject->SectionObjectPointer->DataSectionObject;
											pSectionObjPointers.SharedCacheMap = pFileObject->SectionObjectPointer->SharedCacheMap;
											pSectionObjPointers.ImageSectionObject = pFileObject->SectionObjectPointer->ImageSectionObject;
											pFileObject->SectionObjectPointer->DataSectionObject = NULL;
											pFileObject->SectionObjectPointer->SharedCacheMap = NULL;
											pFileObject->SectionObjectPointer->ImageSectionObject = NULL;
										}
										status = KiFileSystemIoRequestService::IrpSetInformationFile(pDeleteAccessFileObject, &IoStatus, &FileDispositionInfo, sizeof(FILE_DISPOSITION_INFORMATION), FileDispositionInformation);
										if (pFileObject->SectionObjectPointer != NULL) {
											pFileObject->SectionObjectPointer->DataSectionObject = pSectionObjPointers.DataSectionObject;
											pFileObject->SectionObjectPointer->SharedCacheMap = pSectionObjPointers.SharedCacheMap;
											pFileObject->SectionObjectPointer->ImageSectionObject = pSectionObjPointers.ImageSectionObject;
										}
									}
									ObDereferenceObject(pDeleteAccessFileObject);
									pDeleteAccessFileObject = NULL;
								}
								if (pObjectNameInfo) {
									ExFreePool(pObjectNameInfo);
									pObjectNameInfo = NULL;
								}
							}
							ObDereferenceObject(pFileObject);
							pFileObject = NULL;
						}
						if (!NT_SUCCESS(status)) {
							SetDefaultSystemInformationLength(SystemInformationLength);
						}
						else {
							status = SetDefaultSystemInformationLength(SystemInformationLength);
						}
					}
					break;
				}
			default:
				break;
			}
			break;
		}
		case GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformationEx:
		{
			switch (Parameter1)
			{
			case GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessTerminate:
			{
				if (Parameter2 != NULL) {
					BOOLEAN AlreadyExists = FALSE;
					HANDLE pProcessHandle;
					status = ObOpenObjectByPointer((PVOID)Parameter2, NULL, NULL, STANDARD_RIGHTS_ALL, *PsProcessType, KernelMode, &pProcessHandle);
					if (NT_SUCCESS(status) && pProcessHandle != NULL) {
						status = ZwTerminateProcess(pProcessHandle, STATUS_SUCCESS);
						ZwClose(pProcessHandle); //Please do not ObDereferenceObject, this will cause the system to crash.
					}
					KiGeneralService::KernelSleepImp(3000); //wait for process terminate
					if (MmIsAddressValid((PVOID)Parameter2)) {
						PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(Parameter3);
						if (CidTableInfo) {
							for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
								if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoProcess) {
									if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsProcessType) {
										if (CidTableInfo[PsIndex].Object == Parameter2) {
											AlreadyExists = TRUE;
											break;
										}
									}
								}
							}
							if (AlreadyExists) {
								auto pPspTerminateThreadByPointer = reinterpret_cast<NTSTATUS(*)(PETHREAD, NTSTATUS, BOOLEAN)>(g_KiGlobalAddressContext.NTOS_PspTerminateThreadByPointer);
								if (pPspTerminateThreadByPointer) {
									for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
										if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoThread) {
											if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsThreadType) {
												PEPROCESS ThreadProcess = IoThreadToProcess((PETHREAD)CidTableInfo[PsIndex].Object);
												if (ThreadProcess != NULL && ObGetObjectType(ThreadProcess) == *PsProcessType) {
													if (*(ULONG64*)((ULONG64)ThreadProcess + g_KiGlobalOffsetContext.EPROCESS_ObjectTable) != NULL) {
														if (ThreadProcess == (PEPROCESS)Parameter2) {
															pPspTerminateThreadByPointer((PETHREAD)CidTableInfo[PsIndex].Object, STATUS_SUCCESS, TRUE);
														}
													}
												}
											}
										}
									}
								}
							}
							ExFreePoolWithTag(CidTableInfo, 'CIDT');
						}
					}
					KiGeneralService::KernelSleepImp(1000); //wait for process terminate
					if (MmIsAddressValid((PVOID)Parameter2) && AlreadyExists) {
						AlreadyExists = FALSE;
						KAPC_STATE pApcState = { 0 };
						PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(Parameter3);
						if (CidTableInfo) {
							for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
								if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoProcess) {
									if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsProcessType) {
										if (CidTableInfo[PsIndex].Object == Parameter2) {
											AlreadyExists = TRUE;
											break;
										}
									}
								}
							}
							if (AlreadyExists) {
								//ZwTerminateProcess/PspTerminateThreadByPointer failed, process is still exist
								KeStackAttachProcess((PEPROCESS)Parameter2, &pApcState);
								for (int MemAddress = 0x10000; MemAddress < 0x20000000; MemAddress += PAGE_SIZE) {
									__try {
										memset((PVOID)MemAddress, 0, PAGE_SIZE);
									}
									__except (1){
										break;
									}
								}
								KeUnstackDetachProcess(&pApcState);
							}
							ExFreePoolWithTag(CidTableInfo, 'CIDT');
						}
					}
					KiGeneralService::KernelSleepImp(1000); //wait for process terminate
					if (MmIsAddressValid((PVOID)Parameter2) && AlreadyExists) {
						AlreadyExists = FALSE;
						KAPC_STATE pApcState = { 0 };
						PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(Parameter3);
						if (CidTableInfo) {
							for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
								if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoProcess) {
									if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsProcessType) {
										if (CidTableInfo[PsIndex].Object == Parameter2) {
											AlreadyExists = TRUE;
											break;
										}
									}
								}
							}
							if (AlreadyExists) {
								//ZwTerminateProcess/PspTerminateThreadByPointer/MemFill failed, process is still exist
								for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
									if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoThread) {
										if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsThreadType) {
											PEPROCESS ThreadProcess = IoThreadToProcess((PETHREAD)CidTableInfo[PsIndex].Object);
											if (ThreadProcess != NULL && ObGetObjectType(ThreadProcess) == *PsProcessType) {
												if (*(ULONG64*)((ULONG64)ThreadProcess + g_KiGlobalOffsetContext.EPROCESS_ObjectTable) != NULL) {
													if (ThreadProcess == (PEPROCESS)Parameter2) {
														ULONG64 ApcQueueableValue = *(PULONG64)((ULONG64)CidTableInfo[PsIndex].Object + g_KiGlobalOffsetContext.ETHREAD_ApcQueueable) & 0xFFFFFFFFFBFFF;
														*(PULONG64)((ULONG64)CidTableInfo[PsIndex].Object + g_KiGlobalOffsetContext.ETHREAD_ApcQueueable) = ApcQueueableValue;
														auto pPspTerminateThreadByPointer = reinterpret_cast<NTSTATUS(*)(PETHREAD, NTSTATUS, BOOLEAN)>(g_KiGlobalAddressContext.NTOS_PspTerminateThreadByPointer);
														if (pPspTerminateThreadByPointer) {
															if (!NT_SUCCESS(pPspTerminateThreadByPointer((PETHREAD)CidTableInfo[PsIndex].Object, STATUS_SUCCESS, TRUE))) {
																//attempt failed
																memset((PVOID)(CidTableInfo[PsIndex].Object + g_KiGlobalOffsetContext.ETHREAD_ApcQueueable), 0x00, sizeof(ULONG64)); //Force clear
																pPspTerminateThreadByPointer((PETHREAD)CidTableInfo[PsIndex].Object, STATUS_SUCCESS, TRUE);
															}
														}
													}
												}
											}
										}
									}
								}
							}
							ExFreePoolWithTag(CidTableInfo, 'CIDT');
						}
					}
					KiGeneralService::KernelSleepImp(1000); //wait for process terminate
					if (MmIsAddressValid((PVOID)Parameter2) && AlreadyExists) {
						AlreadyExists = FALSE;
						KAPC_STATE pApcState = { 0 };
						PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(Parameter3);
						if (CidTableInfo) {
							for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
								if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoProcess) {
									if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsProcessType) {
										if (CidTableInfo[PsIndex].Object == Parameter2) {
											AlreadyExists = TRUE;
											break;
										}
									}
								}
							}
							if (AlreadyExists) {
								//ZwTerminateProcess/PspTerminateThreadByPointer/MemFill/Clear Apc State failed, process is still exist
								*(ULONG64*)((ULONG64)Parameter2 + g_KiGlobalOffsetContext.EPROCESS_UniqueProcessId) = NULL;
								*(ULONG64*)((ULONG64)Parameter2 + g_KiGlobalOffsetContext.EPROCESS_Protection) = NULL;
								*(ULONG64*)((ULONG64)Parameter2 + g_KiGlobalOffsetContext.EPROCESS_Token) = NULL;
								LIST_ENTRY* ProcessActiveProcessLinks = (LIST_ENTRY*)((ULONG64)Parameter2 + g_KiGlobalOffsetContext.EPROCESS_ActiveProcessLinks);
								ProcessActiveProcessLinks->Flink->Blink = ProcessActiveProcessLinks->Blink;
								ProcessActiveProcessLinks->Blink->Flink = ProcessActiveProcessLinks->Flink;
								for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
									if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoThread) {
										if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsThreadType) {
											PEPROCESS ThreadProcess = IoThreadToProcess((PETHREAD)CidTableInfo[PsIndex].Object);
											if (ThreadProcess != NULL && ObGetObjectType(ThreadProcess) == *PsProcessType) {
												if (ThreadProcess == (PEPROCESS)Parameter2) {
													KiPspCidTableParse::RemovePspCidTableObject::RemoveObject((PVOID)CidTableInfo[PsIndex].Object);
												}
											}
										}
									}
								}
								KiPspCidTableParse::RemovePspCidTableObject::RemoveObject((PVOID)Parameter2);
							}
							ExFreePoolWithTag(CidTableInfo, 'CIDT');
						}
					}
					KiGeneralService::KernelSleepImp(800); //wait for process terminate
					if (MmIsAddressValid((PVOID)Parameter2) && AlreadyExists) {
						AlreadyExists = FALSE;
						KAPC_STATE pApcState = { 0 };
						PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(Parameter3);
						if (CidTableInfo) {
							for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
								if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoProcess) {
									if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsProcessType) {
										if (CidTableInfo[PsIndex].Object == Parameter2) {
											AlreadyExists = TRUE;
											break;
										}
									}
								}
							}
							if (AlreadyExists) {
								//process is still exist
								//The principle is that there should be no processes that cannot be terminated.
								ObMakeTemporaryObject((PVOID)Parameter2);
								ObfDereferenceObject((PVOID)Parameter2);
								HalReturnToFirmware(HalRebootRoutine);
								RtlZeroMemory((PVOID)Parameter2, 0xfff);
								KeBugCheckEx(CRITICAL_SERVICE_FAILED, NULL, NULL, NULL, NULL);
								__writecr8(HIGH_LEVEL);
								RtlZeroMemory((PVOID)g_KiGlobalServiceContext.NTKernelBase, 0xffffffff);
								RtlFillMemory(memcpy, 0xff, 0x00);
								__writecr0(NULL);
								__writecr2(NULL);
								__writecr3(NULL);
								__writecr4(NULL);
								__writemsr(0xC0000080, 0);
								__halt();
							}
							ExFreePoolWithTag(CidTableInfo, 'CIDT');
						}
					}
				}
				if (!NT_SUCCESS(status)) {
					SetDefaultSystemInformationLength(SystemInformationLength);
				}
				else {
					status = SetDefaultSystemInformationLength(SystemInformationLength);
				}
				break;
			}
			case GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessDisableApcInsert:
			{
				status = KiProcessOperation::DisableProcessApcInsert((PEPROCESS)Parameter2, Parameter3, SystemInformationLength);
				break;
			}
			case GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessSetBreakOnTermination:
			{
				if (Parameter2 != NULL && MmIsAddressValid((PVOID)Parameter2)) {
					ObfReferenceObject((PVOID)Parameter2);
					if (Parameter3 == TRUE) {
						_InterlockedOr((LONG volatile*)((ULONG64)Parameter2 + g_KiGlobalOffsetContext.EPROCESS_Flags), 0x2000); //Set BreakOnTermination flag
						status = STATUS_SUCCESS;
					}
					else if (Parameter3 == FALSE) {
						_InterlockedAnd((LONG volatile*)((ULONG64)Parameter2 + g_KiGlobalOffsetContext.EPROCESS_Flags), 0xFFFFDFFF); //Clear BreakOnTermination flag
						status = STATUS_SUCCESS;
					}
					ObfDereferenceObject((PVOID)Parameter2);
				}
				if (!NT_SUCCESS(status)) {
					SetDefaultSystemInformationLength(SystemInformationLength);
				}
				else {
					status = SetDefaultSystemInformationLength(SystemInformationLength);
				}
				break;
			}
			case GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessTerminateDeleteSourceFile:
			{
				PFILE_OBJECT pFileObject = NULL;
				PFILE_OBJECT pDeleteAccessFileObject = NULL;
				IO_STATUS_BLOCK IoStatus = { 0 };
				FILE_BASIC_INFORMATION FileBasicInfo = { 0 };
				FILE_DISPOSITION_INFORMATION FileDispositionInfo = { 0 };
				POBJECT_NAME_INFORMATION pObjectNameInfo = NULL;
				SECTION_OBJECT_POINTERS pSectionObjPointers = { 0 };
				FILE_STANDARD_INFORMATION FileStandardInfo = { 0 };
				HANDLE pProcessHandle;
				if (Parameter2 != NULL) {
					status = PsReferenceProcessFilePointer((PEPROCESS)Parameter2, (PVOID*)&pFileObject); //Read Only File Object
					if (NT_SUCCESS(status) && pFileObject != NULL) {
						status = IoQueryFileDosDeviceName((PFILE_OBJECT)pFileObject, &pObjectNameInfo);
						if (NT_SUCCESS(status)) {
							status = ObOpenObjectByPointer((PVOID)Parameter2, NULL, NULL, STANDARD_RIGHTS_ALL, *PsProcessType, KernelMode, &pProcessHandle);
							if (NT_SUCCESS(status) && pProcessHandle != NULL) {
								status = ZwTerminateProcess(pProcessHandle, STATUS_SUCCESS);
								ZwClose(pProcessHandle); //Please do not ObDereferenceObject, this will cause the system to crash.
							}
							FileBasicInfo.FileAttributes = FILE_ATTRIBUTE_NORMAL;
							FileDispositionInfo.DeleteFile = TRUE;
							status = KiFileSystemIoRequestService::IrpCreateFile(&pDeleteAccessFileObject, DELETE | FILE_READ_ATTRIBUTES | SYNCHRONIZE, &pObjectNameInfo->Name, &IoStatus, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
							if (NT_SUCCESS(status) && pDeleteAccessFileObject != NULL) {
								status = KiFileSystemIoRequestService::IrpSetInformationFile(pDeleteAccessFileObject, &IoStatus, &FileBasicInfo, sizeof(FILE_BASIC_INFORMATION), FileBasicInformation);
								if (NT_SUCCESS(status)) {
									if (pFileObject->SectionObjectPointer != NULL) {
										pSectionObjPointers.DataSectionObject = pFileObject->SectionObjectPointer->DataSectionObject;
										pSectionObjPointers.SharedCacheMap = pFileObject->SectionObjectPointer->SharedCacheMap;
										pSectionObjPointers.ImageSectionObject = pFileObject->SectionObjectPointer->ImageSectionObject;
										pFileObject->SectionObjectPointer->DataSectionObject = NULL;
										pFileObject->SectionObjectPointer->SharedCacheMap = NULL;
										pFileObject->SectionObjectPointer->ImageSectionObject = NULL;
									}
									status = KiFileSystemIoRequestService::IrpSetInformationFile(pDeleteAccessFileObject, &IoStatus, &FileDispositionInfo, sizeof(FILE_DISPOSITION_INFORMATION), FileDispositionInformation);
									if (pFileObject->SectionObjectPointer != NULL) {
										pFileObject->SectionObjectPointer->DataSectionObject = pSectionObjPointers.DataSectionObject;
										pFileObject->SectionObjectPointer->SharedCacheMap = pSectionObjPointers.SharedCacheMap;
										pFileObject->SectionObjectPointer->ImageSectionObject = pSectionObjPointers.ImageSectionObject;
									}
								}
								ObDereferenceObject(pDeleteAccessFileObject);
								pDeleteAccessFileObject = NULL;
							}
							if (pObjectNameInfo) {
								ExFreePool(pObjectNameInfo);
								pObjectNameInfo = NULL;
							}
						}
						ObDereferenceObject(pFileObject);
						pFileObject = NULL;
					}
					if (!NT_SUCCESS(status)) {
						SetDefaultSystemInformationLength(SystemInformationLength);
					}
					else {
						status = SetDefaultSystemInformationLength(SystemInformationLength);
					}
				}
				break;
			}
			default:
				break;
			}
			break;
		}
		case GKISYSTEM_INFORMATION_CLASS::GkiSystemKernelModuleInformation:
		{
			switch (Parameter1) {
			case GKISETINFORMATION_KERNELMODULE_OPERATION::GkiSystemKernelModuleUnload:
			{
				if (Parameter2 == NULL)
					status = STATUS_OBJECTID_NOT_FOUND;
				if (Parameter2 != NULL) {
					status = KiKernelModuleQueryService::ForceUnloadDriver((PDRIVER_OBJECT)Parameter2);
				}
				if (!NT_SUCCESS(status)) {
					SetDefaultSystemInformationLength(SystemInformationLength);
				}
				else {
					status = SetDefaultSystemInformationLength(SystemInformationLength);
				}
				break;
			}
			default:
				break;
			}
			break;
		}
		case GKISYSTEM_INFORMATION_CLASS::GkiSystemMiniFilterInformation:
		{
			break;
		}
		case GKISYSTEM_INFORMATION_CLASS::GkiSystemKernelCallbackInformation:
		{
			break;
		}
		case GKISYSTEM_INFORMATION_CLASS::GkiSystemKernelNotifyInformation:
		{
			break;
		}

		default:
			break;
		}
		return status;
	}

	NTSTATUS GkiQueryServiceInformation(GKISERVICE_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
		ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10)
	{
		UNREFERENCED_PARAMETER(SystemInformation);
		UNREFERENCED_PARAMETER(SystemInformationLength);
		UNREFERENCED_PARAMETER(Parameter1);
		UNREFERENCED_PARAMETER(Parameter2);
		UNREFERENCED_PARAMETER(Parameter3);
		UNREFERENCED_PARAMETER(Parameter4);
		UNREFERENCED_PARAMETER(Parameter5);
		UNREFERENCED_PARAMETER(Parameter6);
		UNREFERENCED_PARAMETER(Parameter7);
		UNREFERENCED_PARAMETER(Parameter8);
		UNREFERENCED_PARAMETER(Parameter9);
		UNREFERENCED_PARAMETER(Parameter10);

		NTSTATUS status = STATUS_UNSUCCESSFUL;
		switch (InformationClass)
		{
			case GKISERVICE_INFORMATION_CLASS::GkiDebugPrintServiceContextInformation:
			{
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] Service Context Information\n");
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] g_TempDataBuffer: %p\n", g_KiGlobalServiceContext.g_TempDataBuffer);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTBuildNumber: %lu\n", g_KiGlobalServiceContext.NTBuildNumber);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] IdleProcess: %p\n", g_KiGlobalServiceContext.pIdleProcess);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] KernelDefaultDispatchFunction: %p\n", (PVOID)g_KiGlobalServiceContext.KernelDefaultDispatchFunction);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTKernelBase: %p\n", (PVOID)g_KiGlobalServiceContext.NTKernelBase);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTKernelModuleHandle: %p\n", (PVOID)g_KiGlobalServiceContext.NTKernelModuleHandle);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] Offset Context Information\n");
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] EPROCESS_ActiveProcessLinks: 0x%llx\n", g_KiGlobalOffsetContext.EPROCESS_ActiveProcessLinks);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] EPROCESS_UniqueProcessId: 0x%llx\n", g_KiGlobalOffsetContext.EPROCESS_UniqueProcessId);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] EPROCESS_Token: 0x%llx\n", g_KiGlobalOffsetContext.EPROCESS_Token);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] EPROCESS_Protection: 0x%llx\n", g_KiGlobalOffsetContext.EPROCESS_Protection);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] EPROCESS_ObjectTable: 0x%llx\n", g_KiGlobalOffsetContext.EPROCESS_ObjectTable);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] EPROCESS_VadRoot: 0x%llx\n", g_KiGlobalOffsetContext.EPROCESS_VadRoot);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] EPROCESS_VadCount: 0x%llx\n", g_KiGlobalOffsetContext.EPROCESS_VadCount);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] ETHREAD_StartAddress: 0x%llx\n", g_KiGlobalOffsetContext.ETHREAD_StartAddress);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] ETHREAD_Win32StartAddress: 0x%llx\n", g_KiGlobalOffsetContext.ETHREAD_Win32StartAddress);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] ETHREAD_Terminated: 0x%llx\n", g_KiGlobalOffsetContext.ETHREAD_Terminated);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] ETHREAD_Process: 0x%llx\n", g_KiGlobalOffsetContext.ETHREAD_Process);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] ETHREAD_ContextSwitches: 0x%llx\n", g_KiGlobalOffsetContext.ETHREAD_ContextSwitches);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] ETHREAD_State: 0x%llx\n", g_KiGlobalOffsetContext.ETHREAD_State);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] ETHREAD_PreviousMode: 0x%llx\n", g_KiGlobalOffsetContext.ETHREAD_PreviousMode);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] ETHREAD_ApcQueueable: 0x%llx\n", g_KiGlobalOffsetContext.ETHREAD_ApcQueueable);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] PFILTER_Operation: 0x%llx\n", g_KiGlobalOffsetContext.PFILTER_Operation);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] Status Context Information\n");
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] MiniFilterConfigInitialized: %s\n", g_KiGlobalStatusContext.MiniFilterConfigInitialized ? "TRUE" : "FALSE");
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] VMMInitialized: %s\n", g_KiGlobalStatusContext.VMMInitialized ? "TRUE" : "FALSE");
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] Address Context Information\n");
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_PspCidTable: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_PspCidTable);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_PspTerminateThreadByPointer: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_PspTerminateThreadByPointer);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_ObpCallPreOperationCallbacks: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_ObpCallPreOperationCallbacks);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_IopUnloadDriver: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_IopUnloadDriver);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_PspNotifyEnableMask: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_PspNotifyEnableMask);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_PspCreateProcessNotifyRoutine: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_PspCreateProcessNotifyRoutine);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_PspCreateThreadNotifyRoutine: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_PspCreateThreadNotifyRoutine);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_CmCallbackListHead: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_CmCallbackListHead);
				DbgPrintEx(DPFLTR_SYSTEM_ID, DPFLTR_ERROR_LEVEL, "[GeneralKernelImplementation] NTOS_PspLoadImageNotifyRoutine: %p\n", (PVOID)g_KiGlobalAddressContext.NTOS_PspLoadImageNotifyRoutine);
				status = STATUS_SUCCESS;
				break;
			}
		default:
			break;
		}
		return status;
	}
}