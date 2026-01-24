#include "QueryProcessEx.h"


namespace KiQueryProcessEx
{
	NTSTATUS GkiQueryProcessEx(PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Mode, ULONG_PTR ListSize) {
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		ULONG64 ListCount = 0;
		PVOID pFilePoint = NULL;
		POBJECT_NAME_INFORMATION pObjectNameInfo = NULL;
		UNICODE_STRING ProcessName;
		BOOLEAN AlreadyExists = FALSE;
		if (Mode == UserMode && KiGeneralService::IsAddressInUserSpace(SystemInformation)) {
			__try {
				ProbeForWrite(SystemInformation, ListSize, 1);
				PQUERY_PROCESS_INFO QueryProcessInfoList = (PQUERY_PROCESS_INFO)SystemInformation;
				PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(ListSize);
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
					for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
						if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoThread) {
							if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsThreadType) {
								PEPROCESS ThreadProcess = IoThreadToProcess((PETHREAD)CidTableInfo[PsIndex].Object);
								if (ThreadProcess != NULL && ObGetObjectType(ThreadProcess) == *PsProcessType) {
									if (*(ULONG64*)((ULONG64)ThreadProcess + g_KiGlobalOffsetContext.EPROCESS_ObjectTable) != NULL) {
										AlreadyExists = FALSE;
										for (ULONG Index = 0; Index < ListCount; Index++) {
											if (QueryProcessInfoList[Index].PPROCESSOBJECT == ThreadProcess) {
												AlreadyExists = TRUE;
												break;
											}
										}
										if (!AlreadyExists) {
											QueryProcessInfoList[ListCount].PPROCESSOBJECT = (PVOID)ThreadProcess;
											QueryProcessInfoList[ListCount].PROCESSPID = (ULONG64)PsGetProcessId(ThreadProcess);
											status = PsReferenceProcessFilePointer((PEPROCESS)ThreadProcess, &pFilePoint);
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
													UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)ThreadProcess);
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
												UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)ThreadProcess);
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
						}
					}
					if (g_KiGlobalServiceContext.pIdleProcess != NULL) {
						AlreadyExists = FALSE;
						for (ULONG Index = 0; Index < ListCount; Index++) {
							if (QueryProcessInfoList[Index].PPROCESSOBJECT == g_KiGlobalServiceContext.pIdleProcess) {
								AlreadyExists = TRUE;
								break;
							}
						}
						if (!AlreadyExists) {
							QueryProcessInfoList[ListCount].PPROCESSOBJECT = (PVOID)g_KiGlobalServiceContext.pIdleProcess;
							QueryProcessInfoList[ListCount].PROCESSPID = (ULONG64)PsGetProcessId(g_KiGlobalServiceContext.pIdleProcess);
							UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)g_KiGlobalServiceContext.pIdleProcess);
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
							ListCount++;
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
		else if (Mode == KernelMode) {
			if (MmIsAddressValid(SystemInformation))
			{
				PQUERY_PROCESS_INFO QueryProcessInfoList = (PQUERY_PROCESS_INFO)SystemInformation;
				PPSPCIDTABLE_INFO CidTableInfo = KiPspCidTableParse::ParseCidTable(ListSize);
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
					for (INT PsIndex = 0; PsIndex < g_KiGlobalServiceContext.PspCidTableInfoListCount; PsIndex++) {
						if (CidTableInfo[PsIndex].InfoType == PSPCIDTABLE_INFO_TYPE::PspCidTableInfoThread) {
							if (CidTableInfo[PsIndex].Object != NULL && ObGetObjectType((PVOID)CidTableInfo[PsIndex].Object) == *PsThreadType) {
								PEPROCESS ThreadProcess = IoThreadToProcess((PETHREAD)CidTableInfo[PsIndex].Object);
								if (ThreadProcess != NULL && ObGetObjectType(ThreadProcess) == *PsProcessType) {
									if (*(ULONG64*)((ULONG64)ThreadProcess + g_KiGlobalOffsetContext.EPROCESS_ObjectTable) != NULL) {
										AlreadyExists = FALSE;
										for (ULONG Index = 0; Index < ListCount; Index++) {
											if (QueryProcessInfoList[Index].PPROCESSOBJECT == ThreadProcess) {
												AlreadyExists = TRUE;
												break;
											}
										}
										if (!AlreadyExists) {
											QueryProcessInfoList[ListCount].PPROCESSOBJECT = (PVOID)ThreadProcess;
											QueryProcessInfoList[ListCount].PROCESSPID = (ULONG64)PsGetProcessId(ThreadProcess);
											status = PsReferenceProcessFilePointer((PEPROCESS)ThreadProcess, &pFilePoint);
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
													UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)ThreadProcess);
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
												UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)ThreadProcess);
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
						}
					}
					if (g_KiGlobalServiceContext.pIdleProcess != NULL) {
						AlreadyExists = FALSE;
						for (ULONG Index = 0; Index < ListCount; Index++) {
							if (QueryProcessInfoList[Index].PPROCESSOBJECT == g_KiGlobalServiceContext.pIdleProcess) {
								AlreadyExists = TRUE;
								break;
							}
						}
						if (!AlreadyExists) {
							QueryProcessInfoList[ListCount].PPROCESSOBJECT = (PVOID)g_KiGlobalServiceContext.pIdleProcess;
							QueryProcessInfoList[ListCount].PROCESSPID = (ULONG64)PsGetProcessId(g_KiGlobalServiceContext.pIdleProcess);
							UCHAR* ansiName = PsGetProcessImageFileName((PEPROCESS)g_KiGlobalServiceContext.pIdleProcess);
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
							ListCount++;
						}
					}
				}
				if (CidTableInfo) {
					ExFreePoolWithTag(CidTableInfo, 'CIDT');
				}
			}
		}
		if (Mode == UserMode && KiGeneralService::IsAddressInUserSpace(SystemInformationLength)) {
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
		else if (Mode == KernelMode) {
			if (MmIsAddressValid(SystemInformationLength)) {
				if (SystemInformationLength != NULL) {
					*SystemInformationLength = ListCount;
				}
			}
		}
		return status;
	}
}