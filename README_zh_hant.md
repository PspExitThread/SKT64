# SKT64  
- SKT64 是一個Windows平台上的ARK(Anti-Rootkit)工具，擁有最高的RING0權限  
- 相容Windows10至Windows11系統(若在低於15063版本的系統上運行SKT64，將會得到UNSUPPORTED_PROCESSOR錯誤檢查)(15063 - 27891)

# 功能  

### 進程 & 線程 管理  
- **進程操作**  
  - 查看進程  
  - 終止進程  
  - 將進程設為系統關鍵進程  
  - 暫停進程  
  - 恢復進程  
  - 隱藏進程  
  - 修改進程保護屬性(PPL)  
  - 強制注入DLL  
- **線程操作**  
  - 查看進程線程  
  - 終止進程線程  
  - 暫停進程線程  
  - 恢復進程線程  

### 內核模組管理  
- 查看內核模組  
- 強制卸載內核模組  
- 隱藏內核模組  
- 修改內核模組基址  
- 查看內核模組Irp派遣函數  

### 內核 回調 & 通知 管理  
- **查看/移除 回調/通知**  
  - CreateProcessNotify  
  - CreateThreadNotify  
  - LoadImageNotify  
  - RegistryCallback  
  - BugCheckCallback  
  - BugCheckReasonCallback  
  - ShutdownNotify  
  - LastChanceShutdownNotify  
  - FsChangeNotify  
  - PowerSettingCallback  
  - CoalescingCallback  
  - PriorityCallback  
  - DebugPrintCallback  
  - ExCallback  
  - PlugPlayNotify  
  - EmpCallback  
- 關閉 ObRegisterCallbacks Callback  
- 關閉 Thread/LoadImage/Module Notify  
- 關閉 CmRegisterCallback Callback  

### 過濾 & 鉤子 管理  
- 查看/移除 MiniFilter  
- 查看/移除 SSDT Hook  
- 查看/移除 Shadow SSDT Hook  
- 掃描/移除 系統呼叫鉤子（支援掃描 msr鉤子/內聯鉤子/虛擬化鉤子(EPThook/NPThook)）  
- 查看/移除 Irp派遣函數鉤子  

### Windows 過濾平台 (WFP)  
- 查看 WFP Function  
- 查看/移除 WFP Callout  

### 檔案系統操作  
- 查看/刪除/保護/重新命名/移動 檔案（無視 Irp佔坑/句柄佔用/硬連結 等）  

### 系統監控 & 控制  
- 系統內核監控  
- VMM（虛擬化技術）  
- 完全關閉PatchGuard  
- 無視驅動簽名強制加載驅動  
- 快速關機/快速重啟/藍屏  
- 記憶體編輯器  
- 沙盒  
- 修改工作管理員  
- 彩色的藍屏  

### 韌體操作  
- 熱刷寫韌體  
- 鎖死韌體  

### 安全限制功能  
- 禁止：  
  - 建立進程  
  - 加載驅動  
  - 建立檔案  
  - 修改登錄檔  
  - 修改磁碟引導扇區  
  - 卸載驅動  

### 系統雜項功能查看  
- 查看 IoTimer  
- 查看 Object-Type  
- 查看/終止/暫停/恢復 系統線程  
- 查看 IDT/GDT  
- 查看 已卸載的驅動  
- 查看 HalDispatchTable/HalPrivateDispatchTable  
- 查看 PiDDbCacheTable  

# 截圖  
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/1.png"/>  
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/2.png"/>  
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/3.png"/>  
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/4.png"/>  

# 聯絡方式  
kebugcheck2$outlook.com（$改為 @）  

# 警告  
## 使用本軟體給您造成的所有損失作者不承擔任何責任！  

# 已知但未修復的bug  
1. "Refresh" 拼寫錯誤。