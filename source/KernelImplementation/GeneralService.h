#pragma once
#include <ntifs.h>
#include <wdm.h>
#include <bcrypt.h>
#include <intrin.h>
#include <Ntstrsafe.h>
#include <ntdef.h>
#include <windef.h>
#include "hypervisor_routines.h"
#include "hypervisor_gateway.h"
#include "vmm.h"
#pragma comment(lib, "bcrypt.lib")
#pragma warning(disable:4201)
#pragma warning(disable:4348)


typedef struct _KISEARCH_MODULE_HANDLE
{
    PVOID ModuleBase;
    ULONG ModuleSize;
} KISEARCH_MODULE_HANDLE, * PKISEARCH_MODULE_HANDLE;

typedef struct _KIGLOBAL_SERVICE_CONTEXT
{
	PDRIVER_OBJECT pDriverObject;
	ULONG NTBuildNumber;
	PEPROCESS pIdleProcess;
	ULONG64 KernelDefaultDispatchFunction;
	PVOID g_TempDataBuffer;
	PDRIVER_UNLOAD pServiceDriverUnloadFunction;
	ULONG64 NTKernelBase;
	PKISEARCH_MODULE_HANDLE NTKernelModuleHandle;
    ULONG64 PspCidTableInfoListCount;
    ULONG64 PspCidTableInfoListMaxCount;
    ULONG64 KernelModuleInfoListCount;
    ULONG64 KernelModuleInfoListMaxCount;
} KIGLOBAL_SERVICE_CONTEXT, * PKIGLOBAL_SERVICE_CONTEXT;

typedef struct _KIGLOBAL_OFFSET_CONTEXT
{
	ULONG64 EPROCESS_UniqueProcessId;
	ULONG64 EPROCESS_Token;
	ULONG64 EPROCESS_Protection;
	ULONG64 EPROCESS_ActiveProcessLinks;
	ULONG64 EPROCESS_ObjectTable;
	ULONG64 EPROCESS_VadRoot;
	ULONG64 EPROCESS_VadCount;
    ULONG64 EPROCESS_Flags;
	ULONG64 ETHREAD_StartAddress;
	ULONG64 ETHREAD_Win32StartAddress;
	ULONG64 ETHREAD_Terminated;
	ULONG64 ETHREAD_Process;
	ULONG64 ETHREAD_ContextSwitches;
	ULONG64 ETHREAD_State;
	ULONG64 ETHREAD_PreviousMode;
	ULONG64 ETHREAD_ApcQueueable;
	ULONG64 PFILTER_Operation;
} KIGLOBAL_OFFSET_CONTEXT, * PKIGLOBAL_OFFSET_CONTEXT;

typedef struct _KISERVICE_STATUS_CONTEXT
{
	BOOLEAN MiniFilterConfigInitialized;
	BOOLEAN VMMInitialized;
} KISERVICE_STATUS_CONTEXT, * PKISERVICE_STATUS_CONTEXT;

typedef struct _KIGLOBAL_ADDRESS_CONTEXT
{
    ULONG64 NTOS_PspCidTable;
	ULONG64 NTOS_PspTerminateThreadByPointer;
    ULONG64 NTOS_ObpCallPreOperationCallbacks;
    ULONG64 NTOS_IopUnloadDriver;
    ULONG64 NTOS_PspNotifyEnableMask;
    ULONG64 NTOS_PspCreateProcessNotifyRoutine;
    ULONG64 NTOS_PspCreateThreadNotifyRoutine;
    ULONG64 NTOS_CmCallbackListHead;
    ULONG64 NTOS_PspLoadImageNotifyRoutine;
} KIGLOBAL_ADDRESS_CONTEXT, * PKIGLOBAL_ADDRESS_CONTEXT;

EXTERN_C KIGLOBAL_SERVICE_CONTEXT g_KiGlobalServiceContext;
EXTERN_C KIGLOBAL_OFFSET_CONTEXT g_KiGlobalOffsetContext;
EXTERN_C KISERVICE_STATUS_CONTEXT g_KiGlobalStatusContext;
EXTERN_C KIGLOBAL_ADDRESS_CONTEXT g_KiGlobalAddressContext;


typedef struct _LDR_DATA_TABLE_ENTRY
{
    struct _LIST_ENTRY InLoadOrderLinks;                                    //0x0
    struct _LIST_ENTRY InMemoryOrderLinks;                                  //0x10
    struct _LIST_ENTRY InInitializationOrderLinks;                          //0x20
    VOID* DllBase;                                                          //0x30
    VOID* EntryPoint;                                                       //0x38
    ULONG SizeOfImage;                                                      //0x40
    struct _UNICODE_STRING FullDllName;                                     //0x48
    struct _UNICODE_STRING BaseDllName;                                     //0x58
    union
    {
        UCHAR FlagGroup[4];                                                 //0x68
        ULONG Flags;                                                        //0x68
        struct
        {
            ULONG PackagedBinary : 1;                                         //0x68
            ULONG MarkedForRemoval : 1;                                       //0x68
            ULONG ImageDll : 1;                                               //0x68
            ULONG LoadNotificationsSent : 1;                                  //0x68
            ULONG TelemetryEntryProcessed : 1;                                //0x68
            ULONG ProcessStaticImport : 1;                                    //0x68
            ULONG InLegacyLists : 1;                                          //0x68
            ULONG InIndexes : 1;                                              //0x68
            ULONG ShimDll : 1;                                                //0x68
            ULONG InExceptionTable : 1;                                       //0x68
            ULONG ReservedFlags1 : 2;                                         //0x68
            ULONG LoadInProgress : 1;                                         //0x68
            ULONG LoadConfigProcessed : 1;                                    //0x68
            ULONG EntryProcessed : 1;                                         //0x68
            ULONG ProtectDelayLoad : 1;                                       //0x68
            ULONG ReservedFlags3 : 2;                                         //0x68
            ULONG DontCallForThreads : 1;                                     //0x68
            ULONG ProcessAttachCalled : 1;                                    //0x68
            ULONG ProcessAttachFailed : 1;                                    //0x68
            ULONG CorDeferredValidate : 1;                                    //0x68
            ULONG CorImage : 1;                                               //0x68
            ULONG DontRelocate : 1;                                           //0x68
            ULONG CorILOnly : 1;                                              //0x68
            ULONG ChpeImage : 1;                                              //0x68
            ULONG ReservedFlags5 : 2;                                         //0x68
            ULONG Redirected : 1;                                             //0x68
            ULONG ReservedFlags6 : 2;                                         //0x68
            ULONG CompatDatabaseProcessed : 1;                                //0x68
        };
    };
    USHORT ObsoleteLoadCount;                                               //0x6c
    USHORT TlsIndex;                                                        //0x6e
    struct _LIST_ENTRY HashLinks;                                           //0x70
    ULONG TimeDateStamp;                                                    //0x80
    struct _ACTIVATION_CONTEXT* EntryPointActivationContext;                //0x88
    VOID* Lock;                                                             //0x90
    struct _LDR_DDAG_NODE* DdagNode;                                        //0x98
    struct _LIST_ENTRY NodeModuleLink;                                      //0xa0
    struct _LDRP_LOAD_CONTEXT* LoadContext;                                 //0xb0
    VOID* ParentDllBase;                                                    //0xb8
    VOID* SwitchBackContext;                                                //0xc0
    struct _RTL_BALANCED_NODE BaseAddressIndexNode;                         //0xc8
    struct _RTL_BALANCED_NODE MappingInfoIndexNode;                         //0xe0
    ULONGLONG OriginalBase;                                                 //0xf8
    union _LARGE_INTEGER LoadTime;                                          //0x100
    ULONG BaseNameHashValue;                                                //0x108
    enum _LDR_DLL_LOAD_REASON LoadReason;                                   //0x10c
    ULONG ImplicitPathOptions;                                              //0x110
    ULONG ReferenceCount;                                                   //0x114
    ULONG DependentLoadFlags;                                               //0x118
    UCHAR SigningLevel;                                                     //0x11c
}LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef enum _FIRMWARE_REENTRY {
    HalHaltRoutine,
    HalPowerDownRoutine,
    HalRestartRoutine,
    HalRebootRoutine,
    HalInteractiveModeRoutine,
    HalMaximumRoutine
} FIRMWARE_REENTRY, * PFIRMWARE_REENTRY;

typedef struct _AUX_ACCESS_DATA {
    PPRIVILEGE_SET PrivilegesUsed;
    GENERIC_MAPPING GenericMapping;
    ACCESS_MASK AccessesToAudit;
    ACCESS_MASK MaximumAuditMask;
    ULONG Unknown[256];
} AUX_ACCESS_DATA, * PAUX_ACCESS_DATA;

typedef struct _OBJECT_DIRECTORY_ENTRY
{
    struct _OBJECT_DIRECTORY_ENTRY* ChainLink;
    PVOID Object;
    ULONG HashValue;
} OBJECT_DIRECTORY_ENTRY, * POBJECT_DIRECTORY_ENTRY;

typedef struct _OBJECT_DIRECTORY
{
    struct _OBJECT_DIRECTORY_ENTRY* HashBuckets[37];
} OBJECT_DIRECTORY, * POBJECT_DIRECTORY;

EXTERN_C_START
NTSTATUS ZwAdjustPrivilegesToken(IN HANDLE TokenHandle, IN BOOLEAN DisableAllPrivileges, IN PTOKEN_PRIVILEGES NewState OPTIONAL, IN ULONG BufferLength OPTIONAL, OUT PTOKEN_PRIVILEGES PreviousState OPTIONAL, OUT PULONG ReturnLength);
NTSTATUS ZwSetInformationProcess(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, IN PVOID ProcessInformation, IN ULONG ProcessInformationLength);
NTSTATUS NTAPI ZwQuerySystemInformation(DWORD32 systemInformationClass, PVOID systemInformation, ULONG systemInformationLength, PULONG returnLength);
NTKERNELAPI PVOID NTAPI ObGetObjectType(IN PVOID pObject);
NTKERNELAPI NTSTATUS PsReferenceProcessFilePointer(IN PEPROCESS Process, OUT PVOID* pFilePointer);
NTSYSAPI UCHAR* PsGetProcessImageFileName(PEPROCESS Process);
NTKERNELAPI VOID NTAPI HalReturnToFirmware(LONG lReturnType);
NTSTATUS WINAPI ZwQueryInformationProcess(_In_ HANDLE ProcessHandle, _In_ PROCESSINFOCLASS ProcessInformationClass, _Out_ PVOID ProcessInformation, _In_ ULONG ProcessInformationLength, _Out_opt_ PULONG ReturnLength);
NTSTATUS ObCreateObject(__in KPROCESSOR_MODE ProbeMode, __in POBJECT_TYPE ObjectType, __in POBJECT_ATTRIBUTES ObjectAttributes, __in KPROCESSOR_MODE OwnershipMode, __inout_opt PVOID ParseContext, __in ULONG ObjectBodySize, __in ULONG PagedPoolCharge, __in ULONG NonPagedPoolCharge, __out PVOID* Object);
NTSTATUS SeCreateAccessState(PACCESS_STATE AccessState, PVOID AuxData, ACCESS_MASK DesiredAccess, PGENERIC_MAPPING GenericMapping);
NTSTATUS NTAPI ObReferenceObjectByName(IN PUNICODE_STRING ObjectName, IN ULONG64 Attributes, IN PACCESS_STATE PassedAccessState OPTIONAL, IN ACCESS_MASK DesiredAccess OPTIONAL, IN POBJECT_TYPE ObjectType, IN KPROCESSOR_MODE AccessMode, IN OUT PVOID ParseContext OPTIONAL, OUT PVOID* Object);
EXTERN_C_END

extern "C" POBJECT_TYPE* IoDeviceObjectType; //LINK1169
extern "C" POBJECT_TYPE* IoDriverObjectType;
extern "C" PLIST_ENTRY PsLoadedModuleList;

#define KIMAX_PATHEX 500

typedef struct _QUERY_PROCESS_INFO
{
    WCHAR PROCESSNAME[KIMAX_PATHEX];
    PVOID PPROCESSOBJECT;
    ULONG64 PROCESSPID;
    WCHAR PROCESSPATH[KIMAX_PATHEX];
} QUERY_PROCESS_INFO, * PQUERY_PROCESS_INFO;

namespace KiGeneralService
{
	NTSTATUS GkiInitializeContext(PDRIVER_OBJECT pDriverObject);
	ULONG GkiGetBuildNumber();
	PEPROCESS GkiGetIdleEprocess();
	NTSTATUS InitMiniFilterConfig(IN PUNICODE_STRING RegistryString, IN PWSTR Altitude);
	ULONG64 FindPattern(ULONG64 base, SIZE_T size, PCHAR pattern);
    template <typename T = PVOID>
    T FindPatternEx(PVOID Start, ULONG64 Length, PCCH Pattern, PCCH Mask)
    {
        PCCH Data = static_cast<PCCH>(Start);
        size_t Pattern_length = strlen(Mask);
        for (INT i = 0; i <= Length - Pattern_length; i++)
        {
            bool Accumulative_found = true;
            for (size_t j = 0; j < Pattern_length; j++)
            {
                if (!MmIsAddressValid(reinterpret_cast<PVOID>(reinterpret_cast<ULONG64>(Data) + i + j)))
                {
                    Accumulative_found = false;
                    break;
                }
                if (Data[i + j] != Pattern[j] && Mask[j] != '?')
                {
                    Accumulative_found = false;
                    break;
                }
            }
            if (Accumulative_found)
            {
                return (T)(reinterpret_cast<ULONG64>(Data) + i);
            }
        }
        return (T)NULL;
    }
    template <typename T = ULONG64>
    T GetExportFunction(PWCHAR Name)
    {
        UNICODE_STRING s;
        RtlInitUnicodeString(&s, Name);
        return reinterpret_cast<T>(MmGetSystemRoutineAddress(&s));
    }

    ULONG64 GetNTKernelBase();
    NTSTATUS GkiInitializeVMM();
    NTSTATUS GkiUninitializeVMM();
    ULONG64 ResolveRelativeAddress(ULONG64 Address, ULONG Offset);
    BOOLEAN IsAddressInUserSpace(PVOID Address);
    NTSTATUS GetFileNameFromUnicodeStringPath(PCUNICODE_STRING FullPath, PUNICODE_STRING FileName);
    BOOLEAN KernelSleepImp(ULONG MillionSecond);
}