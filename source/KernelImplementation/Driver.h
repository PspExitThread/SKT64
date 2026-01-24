#pragma once
#include <ntifs.h>
#include <wdm.h>
#include <bcrypt.h>
#include "GeneralService.h"
#include "KernelFeatureCodeSearch.h"
#include "InitializeKernelServiceAddress.h"
#include "KernelInterface.h"
#include "PspCidTableParse.h"
#include "IoRequestControl.h"
#include "KernelModuleQuery.h"
#include "FileSystemImplementation.h"
#pragma comment(lib, "bcrypt.lib")

VOID KiServiceDriverUnload(PDRIVER_OBJECT pDriverObject);
EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryString);