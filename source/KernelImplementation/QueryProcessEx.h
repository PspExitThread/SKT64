#pragma once
#include <ntifs.h>
#include "GeneralService.h"
#include "KernelInterface.h"

namespace KiQueryProcessEx
{
	NTSTATUS GkiQueryProcessEx(PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Mode, ULONG_PTR ListSize);
}