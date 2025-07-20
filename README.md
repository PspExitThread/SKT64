# SKT64

- SKT64 is an Ark tool on the Windows platform
It's a completely free tool
- support Windows 10 - Windows 11(15063 - 27902)

<a href='./README_zh_cn.md'>简体中文</a>

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
- View/Delete/Protect/Rename/Move File/Copy File (Ignores IRP occupation/Handle occupation/Hard Link)  
- Physical Disk Parser
- NTFS DIRECT CONTROL（Extreme file deletion）

### System Monitoring & Control  
- System Kernel Monitor  
- VMM (Virtualization Technology)
- IOMMU-SLAT(Anti-DMA Read Write MEMORY)
- Disable PatchGuard  
- Ignore Driver Signature Enforcement (Load Driver)  
- Fast Shutdown/REBOOT/BSOD  
- Memory Editor  
- Sandbox
- Taskmgr-Editor
- Colorful BSOD
- Hide File
- Restart the restoration

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
### If you want to get the source code of SKT64, please contact me
## EMAIL: kebugcheck2$outlook.com（$change to @）



# WARNING
## The author shall not be held liable for any losses incurred by your use of this software.
Do not use Disable PatchGuard on a physical machine!


# Update log
- latest version: 7.2
1. Fixed some known bugs
2. Added the ability to hide files
3. Added the ability to Restart the restoration
4. and other content
