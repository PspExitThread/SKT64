#pragma once
#include <ntifs.h>
#include <wdm.h>
#include "GeneralService.h"


namespace KiFeatureCodeSearch
{
	PKISEARCH_MODULE_HANDLE KiOpenFeatureCodeSearchHandleW(PWCHAR Name);
	PKISEARCH_MODULE_HANDLE KiOpenFeatureCodeSearchHandle(PUNICODE_STRING ModuleName);
	VOID KiCloseFeatureCodeSearchHandle(PKISEARCH_MODULE_HANDLE Handle);
	PVOID KiSearchFeatureCode(PKISEARCH_MODULE_HANDLE Handle, PCCH Pattern, PCCH Mask);
}
