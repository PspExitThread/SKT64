# SKT64
- SKT64 是一个windows平台上的ARK(Anti-Rootkit)工具,拥有最高的RING0权限
- 兼容windows10-windows11的系统(如果你在低版本点系统上运行SKT64将会得到UNSUPPORTED_PROCESSOR错误检查)

# 功能  

### 进程 & 线程 管理  
- **进程操作**  
  - 查看进程  
  - 终止进程  
  - 将进程设置为系统关键进程  
  - 挂起进程 
  - 恢复进程
  - 隐藏进程
  - 修改进程保护属性(PPL)  
  - 强制注入dll
- **线程操作**  
  - 查看进程线程
  - 终止进程线程
  - 挂起进程线程
  - 恢复进程线程

### 内核模块管理
- 查看内核模块
- 强制卸载内核模块
- 隐藏内核模块
- 修改内核模块基址
- 查看内核模块Irp派遣函数

### 内核 回调 & 通知 管理  
- **查看/移除 回调/通知**  
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
- 关闭 ObRegisterCallbacks Callback  
- 关闭 Thread/LoadImage/Module Notify
- 关闭 CmRegisterCallback Callback

### Filter & Hook Management  
- View/Remove MiniFilter  
- View/UnHook SSDT  
- View/UnHook Shadow SSDT  
- Scan/Unhook MSRHook (Supports MSR/Inline/VMM Hook including EPTHook/NPTHook)  
- Scan/Unhook IrpHook  

### Windows Filtering Platform (WFP)  
- View/Remove WFP Function  
- View/Remove WFP Callout  

### File System Operations  
- View/Delete/Protect/Rename/Move File (Ignores IRP occupation/Handle occupation/Hard Link)  

### System Monitoring & Control  
- System Kernel Monitor  
- VMM (Virtualization Technology)  
- Disable PatchGuard  
- Ignore Driver Signature Enforcement (Load Driver)  
- Fast Shutdown/REBOOT/BSOD  
- Memory Editor  
- Sandbox
- Taskmgr-Editor
- Colorful BSOD  

### Firmware Operations  
- Hot flash to firmware  
- Lock Firmware  

### Security Restrictions  
- Prohibit:  
  - Create Process  
  - Load Driver  
  - Create File  
  - Modify Registry  
  - Modify Disk Boot Sector  
  - Unload Driver  

### Miscellaneous System Views  
- View IoTimer  
- View Object-Type  
- View/Terminate/Suspend/Resume System-Thread  
- View IDT/GDT  
- View UnloadDrivers  
- View HalDispatchTable/HalPrivateDispatchTable
- View PiDDbCacheTable


# Screenshot
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/1.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/2.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/3.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/4.png"/>


# EMAIL
kebugcheck2$outlook.com（$change to @）



# WARNING
## The author shall not be held liable for any losses incurred by your use of this software.
Do not use Disable PatchGuard on a physical machine!


# Known but unfixed bugs
1. "Refresh" is misspelled.