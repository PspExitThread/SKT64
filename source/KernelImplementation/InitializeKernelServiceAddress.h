#pragma once
#include <ntifs.h>
#include <wdm.h>
#include "GeneralService.h"
#include "KernelFeatureCodeSearch.h"

namespace KiInitializeKernelServiceAddress
{
	ULONG64 KiGetPspCidTable();
	ULONG64 KiGetPspTerminateThreadByPointer();
	NTSTATUS InitializeKernelServiceAddressContext();
}