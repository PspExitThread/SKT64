#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <stdio.h>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tlhelp32.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <winternl.h>
#include <filesystem>
#include <atlconv.h>
#include <fwpmu.h>
#include "DriverInterface.h"
#include <d3d9.h>
#include <tchar.h>
#include <bitset>
#include <commctrl.h>
#include <shellapi.h>
#include <capstone/capstone.h>
#include <Wininet.h>
#include <unordered_map>
#include <map>
#include <cwchar>
#include <ctime>
#include <random>
#include <wincrypt.h>
#include <wintrust.h>
#include <softpub.h>
#include "LogService.h"
#include "SKTGeneral.h"
#include "resource.h"

struct Process_ListItem {
	std::wstring name;
	ULONG64 id;
	PVOID eprocess;
	std::wstring Path;
	bool selected = false;
};

struct KernelModule_ListItem {
	ULONG Index;
	std::wstring name;
	ULONG64 BaseAddress;
	ULONG Size;
	ULONG64 EntryPoint;
	PVOID pDriverObject;
	std::wstring Path;
	bool selected = false;
};


static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static bool                     g_DeviceLost = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
EXTERN_C HANDLE g_hKernelDevice;
