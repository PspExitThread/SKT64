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
- View/Delete/Protect File (Ignores IRP occupation/Handle occupation/Hard Link)  

### System Monitoring & Control  
- System Kernel Monitor  
- VMM (Virtualization Technology)  
- Disable PatchGuard  
- Ignore Driver Signature Enforcement (Load Driver)  
- Fast Shutdown/REBOOT/BSOD  
- Memory Editor  
- Sandbox  

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


# Screenshot
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/1.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/2.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/3.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/4.png"/>


# Customized versions(Paid services)
1. SKT64 without VMP version
2. Partial source code of SKT64
3. The complete source code of the SKT64
4. WHQL Version

EMAIL： kebugcheck2$outlook.com（$change to @）



# WARNING
Do not use Disable PatchGuard on a physical machine!
