# SKT64
- SKT64 is an Ark tool on the Windows platform
It's a completely free tool
- support Windows 10 - Windows11

# Features  

### Process & Thread Management  
- **Process Operations**  
  - View Process  
  - Terminate Process  
  - Set Process to System Critical  
  - Suspend Process  
  - Resume Process  
  - Hide Process  
  - Set Process PPL (Protected Process Light)  
  - Inject DLL  
- **Thread Operations**  
  - View Process Threads  
  - Terminate Process Thread  
  - Suspend Process Thread  
  - Resume Process Thread  

### Kernel Module Management  
- View Kernel Module  
- Unload Kernel Module  
- Hide Kernel Module  
- Modify Kernel Module Image Base  
- View Kernel Module Major Functions  

### Callback & Notification Management  
- **View/Remove Callbacks**  
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
- Disable ObRegisterCallbacks Callback  
- Disable Thread/LoadImage/Module Notify
- Disable CmRegisterCallback Callback

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