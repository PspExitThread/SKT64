#include "SKT64.h"

EXTERN_C HANDLE g_hKernelDevice = INVALID_HANDLE_VALUE;

static std::vector<Process_ListItem> g_ProcessList;
static std::vector<KernelModule_ListItem> g_KernelModuleList;

namespace QueryService {
    bool QueryProcessList() {
        PQUERY_PROCESS_INFO infolist = (PQUERY_PROCESS_INFO)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(QUERY_PROCESS_INFO) * 65535);
        if (infolist) {
            ULONG64 ListCount = 0;
            if (KiService::GkiQuerySystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformationEx, infolist, &ListCount, UserMode, sizeof(QUERY_PROCESS_INFO) * 65535, 0, 0, 0, 0, 0, 0, 0, 0)) {
                LogService::printfLog("[QueryProcessList] Process Count: %d\n", LogColor::LOG_DEFAULT, ListCount);
                for (ULONG64 index = 0; index < ListCount; index++) {
                    if (infolist[index].PROCESSPID == 0) {
                        g_ProcessList.push_back({ infolist[index].PROCESSNAME, infolist[index].PROCESSPID, infolist[index].PPROCESSOBJECT, infolist[index].PROCESSPATH });
                    }
                }
                for (ULONG64 index = 0; index < ListCount; index++) {
                    if (infolist[index].PROCESSPID != 0) {
                        g_ProcessList.push_back({ infolist[index].PROCESSNAME, infolist[index].PROCESSPID, infolist[index].PPROCESSOBJECT, infolist[index].PROCESSPATH });
                    }
                }
            }
            else {
                LogService::printfLog("[QueryProcessList] GkiQuerySystemInformation Fail, Error Code: %d\n", LogColor::LOG_RED, GetLastError());
                return false;
            }
            HeapFree(GetProcessHeap(), 0, infolist);
        }
        return true;
    }

    bool QueryKernelModuleList() {
        PQUERY_KERNEL_MODULE_INFO infolist = (PQUERY_KERNEL_MODULE_INFO)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(QUERY_KERNEL_MODULE_INFO) * 4096);
        if (infolist) {
            ULONG64 ListCount = 0;
            if (KiService::GkiQuerySystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemKernelModuleInformation, infolist, &ListCount, UserMode, sizeof(QUERY_KERNEL_MODULE_INFO) * 4096, 0, 0, 0, 0, 0, 0, 0, 0)) {
                LogService::printfLog("[QueryKernelModuleList] KernelModule Count: %d\n", LogColor::LOG_DEFAULT, ListCount);
                for (ULONG64 index = 0; index < ListCount; index++) {
                    if (infolist[index].BaseAddress != 0) {
                        KernelModule_ListItem CurrentItem = { 0 };
						CurrentItem.Index = (ULONG)index;
						CurrentItem.name = infolist[index].ModuleName;
						CurrentItem.BaseAddress = infolist[index].BaseAddress;
						CurrentItem.Size = infolist[index].SizeOfImage;
						CurrentItem.EntryPoint = infolist[index].EntryPoint;
						CurrentItem.pDriverObject = infolist[index].pDriverObject;
						CurrentItem.Path = infolist[index].FullModuleName;
                        g_KernelModuleList.push_back(CurrentItem);
                    }
                }
            }
            else {
                LogService::printfLog("[QueryKernelModuleList] GkiQuerySystemInformation Fail, Error Code: %d\n", LogColor::LOG_RED, GetLastError());
                return false;
            }
            HeapFree(GetProcessHeap(), 0, infolist);
        }
        return true;
    }
}



int main(int, char**)
{
    bool isCurrectDirectoryDriver = false;
    std::wstring DriverFilePath = std::filesystem::current_path().wstring() + L"\\SKT64.sys";
    if (std::filesystem::exists(DriverFilePath)) {
        isCurrectDirectoryDriver = true;
        if (!GerneralService::KernelModuleLoader::LoadDriver(DriverFilePath.c_str(), L"SKT64-Kernel-Driver")) {
            isCurrectDirectoryDriver = false;
            GerneralService::ReleaseCoreDriver();
            GerneralService::KernelModuleLoader::LoadDriver(L"C:\\Windows\\System32\\drivers\\SKT64.sys", L"SKT64-Kernel-Driver");
        }
    }
    else {
        isCurrectDirectoryDriver = false;
        GerneralService::ReleaseCoreDriver();
        GerneralService::KernelModuleLoader::LoadDriver(L"C:\\Windows\\System32\\drivers\\SKT64.sys", L"SKT64-Kernel-Driver");
    }
    SetConsoleTitleW(L"SKT64-Log Console");
    g_hKernelDevice = CreateFileW(L"\\\\.\\GeneralKernelImplementation", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (g_hKernelDevice == INVALID_HANDLE_VALUE || g_hKernelDevice == NULL)
    {
        GerneralService::ReleaseCoreDriver();
        GerneralService::KernelModuleLoader::LoadDriver(L"C:\\Windows\\System32\\drivers\\SKT64.sys", L"SKT64-Kernel-Driver");
        g_hKernelDevice = CreateFileW(L"\\\\.\\GeneralKernelImplementation", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (g_hKernelDevice == INVALID_HANDLE_VALUE)
        {
            SetConsoleTitleW(L"SKT64-Log Console [Failed To Open Driver]");
        }
    }

    QueryService::QueryProcessList();
	QueryService::QueryKernelModuleList();

    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"SKT64_win", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"SKT64", WS_OVERLAPPEDWINDOW, 0, 0, 50, 50, nullptr, nullptr, wc.hInstance, nullptr);
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    ::ShowWindow(hwnd, SW_HIDE);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 15, nullptr, io.Fonts->GetGlyphRangesChineseFull());

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    bool MainWindow = true;
    bool MainWindowSizeSeted = false;


    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);
    bool done = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    while (!done)
    {
        if (!MainWindow)
            done = true;
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        if (g_DeviceLost)
        {
            HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
            if (hr == D3DERR_DEVICELOST)
            {
                ::Sleep(10);
                continue;
            }
            if (hr == D3DERR_DEVICENOTRESET)
                ResetDevice();
            g_DeviceLost = false;
        }

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        if (MainWindow)
        {
            ImGui::Begin("FullSKT64 [Kernel Mode Version: 12.3]", &MainWindow, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
            if (MainWindowSizeSeted != true)
            {
                ImGui::SetWindowSize(ImVec2(1200.0f, 600.0f));
                MainWindowSizeSeted = true;
            }
            if (ImGui::BeginTabBar("MainTabControl"))
            {
                if (ImGui::BeginTabItem("Process"))
                {
                    if (ImGui::BeginTable("process_table", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.25f);
                        ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("EPROCESS", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Path");
                        ImGui::TableHeadersRow();
                        for (size_t i = 0; i < g_ProcessList.size(); ++i) {
                            ImGui::PushID(static_cast<int>(i));
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            if (ImGui::Selectable(GerneralService::WStringToUTF8(g_ProcessList[i].name.c_str()).c_str(), g_ProcessList[i].selected, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                if (!((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)) {
                                    for (auto& item : g_ProcessList) item.selected = false;
                                }
                                g_ProcessList[i].selected = true;
                            }
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", g_ProcessList[i].id);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("0x%p", g_ProcessList[i].eprocess);
                            ImGui::TableSetColumnIndex(3);
                            ImGui::Text(GerneralService::WStringToUTF8(g_ProcessList[i].Path.c_str()).c_str());
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndTable();
                    if (ImGui::BeginPopupContextItem("Process_Menu"))
                    {
                        if (ImGui::MenuItem("Refesh"))
                        {
                            g_ProcessList.clear();
                            g_ProcessList.shrink_to_fit();
                            QueryService::QueryProcessList();
                        }
                        if (ImGui::MenuItem("Terminate"))
                        {
                            for (size_t i = 0; i < g_ProcessList.size(); ++i)
                            {
                                if (g_ProcessList[i].selected == true)
                                {
                                    LogService::printfLog("[TerminateProcess] Terminating Process: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_YELLOW, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                    if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformation, NULL, NULL, GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessTerminate, (ULONG_PTR)g_ProcessList[i].eprocess, sizeof(QUERY_PROCESS_INFO) * 65535, 0, 0, 0, 0, 0, 0, 0)) {
                                        LogService::printfLog("[TerminateProcess] Terminated Process: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_GREEN, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        g_ProcessList.erase(g_ProcessList.begin() + i);
                                    }
                                    else {
                                        LogService::printfLog("[TerminateProcess] Failed To Terminate Process: %ws, PID: %d, EPROCESS: 0x%p, Error Code: %d\n", LogColor::LOG_RED, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess, GetLastError());
                                    }
                                }
                            }
                        }
                        if (ImGui::MenuItem("Force Terminate[Ignore any process protections](Crash WARNING)"))
                        {
                            for (size_t i = 0; i < g_ProcessList.size(); ++i)
                            {
                                if (g_ProcessList[i].selected == true)
                                {
                                    LogService::printfLog("[ForceTerminateProcess] Terminating Process: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_YELLOW, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                    if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformationEx, NULL, NULL, GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessTerminate, (ULONG_PTR)g_ProcessList[i].eprocess, sizeof(QUERY_PROCESS_INFO) * 65535, 0, 0, 0, 0, 0, 0, 0)) {
                                        LogService::printfLog("[ForceTerminateProcess] Terminated Process: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_GREEN, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        g_ProcessList.erase(g_ProcessList.begin() + i);
                                    }
                                    else {
                                        LogService::printfLog("[ForceTerminateProcess] Failed To Terminate Process: %ws, PID: %d, EPROCESS: 0x%p, Error Code: %d\n", LogColor::LOG_RED, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess, GetLastError());
                                    }
                                }
                            }
                        }
                        if (ImGui::MenuItem("Terminate and delete source file"))
                        {
                            if (MessageBoxW(NULL, L"WARNING: This will delete the executable file", L"WARNING", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                                for (size_t i = 0; i < g_ProcessList.size(); ++i)
                                {
                                    if (g_ProcessList[i].selected == true)
                                    {
                                        LogService::printfLog("[TerminateProcessAndDeleteFile] Terminating Process: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_YELLOW, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformationEx, NULL, NULL, GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessTerminateDeleteSourceFile, (ULONG_PTR)g_ProcessList[i].eprocess, 0, 0, 0, 0, 0, 0, 0, 0)) {
                                            LogService::printfLog("[TerminateProcessAndDeleteFile] Terminated Process: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_GREEN, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                            g_ProcessList.erase(g_ProcessList.begin() + i);
                                        }
                                        else {
                                            LogService::printfLog("[TerminateProcessAndDeleteFile] Failed To Terminate Process: %ws, PID: %d, EPROCESS: 0x%p, Error Code: %d\n", LogColor::LOG_RED, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess, GetLastError());
                                        }
                                    }
                                }
                            }
                        }
                        if (ImGui::BeginMenu("Modify BreakOnTermination Status"))
                        {
                            if (ImGui::MenuItem("TRUE")) {
                                for (size_t i = 0; i < g_ProcessList.size(); ++i)
                                {
                                    if (g_ProcessList[i].selected == true)
                                    {
                                        LogService::printfLog("[ModifyBreakOnTermination] Attempting operation on process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_YELLOW, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformation, NULL, NULL, GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessSetBreakOnTermination, (ULONG_PTR)g_ProcessList[i].eprocess, TRUE, 0, 0, 0, 0, 0, 0, 0)) {
                                            LogService::printfLog("[ModifyBreakOnTermination] Successfully Enable BreakOnTermination for process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_GREEN, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        }
                                        else {
                                            LogService::printfLog("[ModifyBreakOnTermination] Operation failed for process: Name: %ws, PID: %d, EPROCESS: 0x%p, ErrorCode=0x%X (%d)\n", LogColor::LOG_RED, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess, GetLastError());
                                        }
                                    }
                                }
                            }
                            if (ImGui::MenuItem("FALSE")) {
                                for (size_t i = 0; i < g_ProcessList.size(); ++i)
                                {
                                    if (g_ProcessList[i].selected == true)
                                    {
                                        LogService::printfLog("[ModifyBreakOnTermination] Attempting operation on process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_YELLOW, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformation, NULL, NULL, GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessSetBreakOnTermination, (ULONG_PTR)g_ProcessList[i].eprocess, FALSE, 0, 0, 0, 0, 0, 0, 0)) {
                                            LogService::printfLog("[ModifyBreakOnTermination] Successfully Disable BreakOnTermination for process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_GREEN, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        }
                                        else {
                                            LogService::printfLog("[ModifyBreakOnTermination] Operation failed for process: Name: %ws, PID: %d, EPROCESS: 0x%p, ErrorCode=0x%X (%d)\n", LogColor::LOG_RED, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess, GetLastError());
                                        }
                                    }
                                }
                            }
                            if (ImGui::MenuItem("TRUE(Direct Kernel Object Modify)")) {
                                for (size_t i = 0; i < g_ProcessList.size(); ++i)
                                {
                                    if (g_ProcessList[i].selected == true)
                                    {
                                        LogService::printfLog("[ModifyBreakOnTermination] Attempting operation on process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_YELLOW, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformationEx, NULL, NULL, GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessSetBreakOnTermination, (ULONG_PTR)g_ProcessList[i].eprocess, TRUE, 0, 0, 0, 0, 0, 0, 0)) {
                                            LogService::printfLog("[ModifyBreakOnTermination] Successfully Enable BreakOnTermination for process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_GREEN, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        }
                                        else {
                                            LogService::printfLog("[ModifyBreakOnTermination] Operation failed for process: Name: %ws, PID: %d, EPROCESS: 0x%p, ErrorCode=0x%X (%d)\n", LogColor::LOG_RED, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess, GetLastError());
                                        }
                                    }
                                }
                            }
                            if (ImGui::MenuItem("FALSE(Direct Kernel Object Modify)")) {
                                for (size_t i = 0; i < g_ProcessList.size(); ++i)
                                {
                                    if (g_ProcessList[i].selected == true)
                                    {
                                        LogService::printfLog("[ModifyBreakOnTermination] Attempting operation on process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_YELLOW, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformationEx, NULL, NULL, GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessSetBreakOnTermination, (ULONG_PTR)g_ProcessList[i].eprocess, FALSE, 0, 0, 0, 0, 0, 0, 0)) {
                                            LogService::printfLog("[ModifyBreakOnTermination] Successfully Disable BreakOnTermination for process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_GREEN, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                        }
                                        else {
                                            LogService::printfLog("[ModifyBreakOnTermination] Operation failed for process: Name: %ws, PID: %d, EPROCESS: 0x%p, ErrorCode=0x%X (%d)\n", LogColor::LOG_RED, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess, GetLastError());
                                        }
                                    }
                                }
                            }
                            ImGui::EndMenu();
                        }
                        if (ImGui::MenuItem("Disable APC Insertion for Process(Crash WARNING)"))
                        {
                            for (size_t i = 0; i < g_ProcessList.size(); ++i)
                            {
                                if (g_ProcessList[i].selected == true)
                                {
                                    LogService::printfLog("[DisableProcessApcInsert] Attempting operation on process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_YELLOW, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                    if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemProcessInformationEx, NULL, NULL, GKISETINFORMATION_PROCESS_OPERATION::GkiSystemProcessDisableApcInsert, (ULONG_PTR)g_ProcessList[i].eprocess, sizeof(QUERY_PROCESS_INFO) * 65535, 0, 0, 0, 0, 0, 0, 0)) {
                                        LogService::printfLog("[DisableProcessApcInsert] Successfully disabled APC insertion for process: Name: %ws, PID: %d, EPROCESS: 0x%p\n", LogColor::LOG_GREEN, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess);
                                    }
                                    else {
                                        LogService::printfLog("[DisableProcessApcInsert] Operation failed for process: Name: %ws, PID: %d, EPROCESS: 0x%p, ErrorCode=0x%X (%d)\n", LogColor::LOG_RED, g_ProcessList[i].name.c_str(), g_ProcessList[i].id, g_ProcessList[i].eprocess, GetLastError());
                                    }
                                }
                            }
                        }
                        ImGui::EndPopup();
                    }
					ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Kernel-Module")) {
                    if (ImGui::BeginTable("Driver_table", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_NoHostExtendX))
                    {
                        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch, 0.25f);
                        ImGui::TableSetupColumn("ImageBase", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("EntryPoint", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("DriverObject", ImGuiTableColumnFlags_WidthFixed);
                        ImGui::TableSetupColumn("Path");
                        ImGui::TableHeadersRow();
                        for (size_t i = 0; i < g_KernelModuleList.size(); ++i) {
                            ImGui::PushID(static_cast<int>(i));
                            ImGui::TableNextRow();
                            //ImGui::TableNextColumn();
                            ImGui::TableSetColumnIndex(0);
                            if (ImGui::Selectable(GerneralService::WStringToUTF8(g_KernelModuleList[i].name.c_str()).c_str(), g_KernelModuleList[i].selected, ImGuiSelectableFlags_SpanAllColumns))
                            {
                                if (!((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)) {
                                    for (auto& item : g_KernelModuleList) item.selected = false;
                                }
                                g_KernelModuleList[i].selected = true;
                            }
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("0x%p", g_KernelModuleList[i].BaseAddress);
                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("0x%x", g_KernelModuleList[i].Size);
                            ImGui::TableSetColumnIndex(3);
                            ImGui::Text("0x%p", g_KernelModuleList[i].EntryPoint);
                            ImGui::TableSetColumnIndex(4);
                            ImGui::Text("0x%p", g_KernelModuleList[i].pDriverObject);
                            ImGui::TableSetColumnIndex(5);
                            ImGui::Text(GerneralService::WStringToUTF8(g_KernelModuleList[i].Path.c_str()).c_str());
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndTable();
                    if (ImGui::BeginPopupContextItem("Process_Menu"))
                    {
                        if (ImGui::MenuItem("Refesh"))
                        {
                            g_KernelModuleList.clear();
                            g_KernelModuleList.shrink_to_fit();
                            QueryService::QueryKernelModuleList();
                        }
                        if (ImGui::MenuItem("ForceUnload(Crash WARNING)"))
                        {
                            for (size_t i = 0; i < g_KernelModuleList.size(); ++i)
                            {
                                if (g_KernelModuleList[i].selected == true)
                                {
                                    LogService::printfLog("[ForceUnloadKernelModule] Unloading KernelModule: %ws, BaseAddress: %p, DrivrObject: 0x%p\n", LogColor::LOG_YELLOW, g_KernelModuleList[i].name.c_str(), g_KernelModuleList[i].BaseAddress, g_KernelModuleList[i].pDriverObject);
                                    if (KiService::GkiSetSystemInformation(GKISYSTEM_INFORMATION_CLASS::GkiSystemKernelModuleInformation, NULL, NULL, GKISETINFORMATION_KERNELMODULE_OPERATION::GkiSystemKernelModuleUnload, (ULONG_PTR)g_KernelModuleList[i].pDriverObject, 0, 0, 0, 0, 0, 0, 0, 0)) {
                                        LogService::printfLog("[ForceUnloadKernelModule] Terminated Process: %ws, BaseAddress: %p, DrivrObject: 0x%p\n", LogColor::LOG_YELLOW, g_KernelModuleList[i].name.c_str(), g_KernelModuleList[i].BaseAddress, g_KernelModuleList[i].pDriverObject);
                                        g_KernelModuleList.erase(g_KernelModuleList.begin() + i);
                                    }
                                    else {
                                        LogService::printfLog("[ForceUnloadKernelModule] Failed To Unload KernelModule: %ws, BaseAddress: %p, DrivrObject: 0x%p, Error Code: %d\n", LogColor::LOG_RED, g_KernelModuleList[i].name.c_str(), g_KernelModuleList[i].BaseAddress, g_KernelModuleList[i].pDriverObject, GetLastError());
                                    }
                                }
                            }
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::End();
        }
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST)
            g_DeviceLost = true;
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    CloseHandle(g_hKernelDevice);
    GerneralService::KernelModuleLoader::UnloadDriver(L"SKT64-Kernel-Driver");
    Sleep(3000);
    return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam);
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
