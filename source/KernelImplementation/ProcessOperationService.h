#pragma once
#include <ntifs.h>
#include "GeneralService.h"
#include "KernelInterface.h"

namespace KiProcessOperation
{
	NTSTATUS DisableProcessApcInsert(PEPROCESS pProcess, SIZE_T ListSize, PULONG64 SystemInformationLength);
}