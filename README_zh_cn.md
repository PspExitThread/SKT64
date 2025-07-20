# SKT64

- SKT64 是一个windows平台上的ARK(Anti-Rootkit)工具,拥有最高的RING0权限
- 兼容windows10-windows11的系统(如果你在低于15063版本的系统上运行SKT64将会得到UNSUPPORTED_PROCESSOR错误检查)(15063 - 27902)

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

### 过滤 & 钩子 管理  
- 查看/移除 MiniFilter  
- 查看/移除 SSDT Hook
- 查看/移除 Shadow SSDT Hoik
- 扫描/移除 系统调用钩子 (支持扫描 msr钩子/内联钩子/虚拟化钩子(EPThook/NPThook))  
- 查看/移除 Irp派遣函数钩子

### Windows 过滤平台 (WFP)  
- 查看 WFP Function  
- 查看/移除 WFP Callout  

### 文件系统操作
- 查看/删除/保护/重命名/移动/复制 文件(无视 Irp占坑/句柄占用/硬链接 等)
- 物理磁盘解析 
- NTFS DIRECT CONTROL(极端删除文件(无视 Irp占坑/双重Irp占坑(Irp占坑后发送删除请求会导致文件无法打开)/hardlink/NTFSFSDHook/句柄占用/MiniFilter/对象回调/隐藏文件/文件对象劫持/重定向/安全描述符修改/文件属性锁定/文件只读 等……))

### 系统监控 & 控制
- 系统内核监控
- VMM (虚拟化技术)  
- IOMMU  
- 完全关闭PatchGuard
- 无视驱动签名强制加载驱动
- 快速关机/快速重启/蓝屏
- 内存编辑器
- 沙盒
- 修改任务管理器
- 彩色的蓝屏
- 隐藏文件
- 重启还原

### 固件操作
- 热刷写固件
- 锁死固件

### 安全限制功能
- 禁止:  
  - 创建进程  
  - 加载驱动  
  - 创建文件  
  - 修改注册表  
  - 修改磁盘引导扇区
  - 卸载驱动

### 系统杂项功能查看
- 查看 IoTimer
- 查看 Object-Type
- 查看/终止/挂起/恢复 系统线程 
- 查看 IDT/GDT  
- 查看 已卸载的驱动
- 查看 HalDispatchTable/HalPrivateDispatchTable
- 查看 PiDDbCacheTable


# 截图
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/1.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/2.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/3.png"/>
<img src="https://github.com/PspExitThread/SKT64/blob/main/Screenshot/4.png"/>


# 联系方式
### 如果你想获取SKT64的源代码请联系我
### 邮箱: kebugcheck2$outlook.com（$改为 @）



# 警告
## 使用本软件给您造成的所有损失作者不承担任何责任!
