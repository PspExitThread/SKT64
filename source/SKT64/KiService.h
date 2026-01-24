#pragma once
#include "SKT64.h"
#include "LogService.h"

namespace KiService
{
	BOOL GkiQuerySystemInformation(GKISYSTEM_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
		ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10);
	BOOL GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS InformationClass, PVOID SystemInformation, PULONG64 SystemInformationLength, ULONG_PTR Parameter1, ULONG_PTR Parameter2, ULONG_PTR Parameter3, ULONG_PTR Parameter4,
		ULONG_PTR Parameter5, ULONG_PTR Parameter6, ULONG_PTR Parameter7, ULONG_PTR Parameter8, ULONG_PTR Parameter9, ULONG_PTR Parameter10);
}