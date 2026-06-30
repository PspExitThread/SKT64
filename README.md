# ARK Tool (Anti-Rootkit) & System Kernel Research Tool

[![Platform](https://img.shields.io/badge/platform-Windows%20Only-blue.svg)](https://www.microsoft.com/windows)
[![Status](https://img.shields.io/badge/status-Refactoring%20In%20Progress-orange.svg)]()

> **⚠️ Project Status: Under Heavy Refactoring**  
> The entire codebase is currently being restructured and modernized. APIs, architecture, and feature availability are subject to change without notice.

---

## 🗑️ Legacy Version (`SKT64-Release.exe`)

| Property | Detail |
|:---|:---|
| **Status** | **Deprecated / No Longer Maintained** |
| **Recommendation** | ❌ **NOT recommended for use** |
| **Supported Builds** | Windows Build `15063` – `29550` only |
| **Documentation** | No further documentation will be provided for the legacy version. |

> The `Release.exe` binary in this repository is the **Legacy** artifact. It exists purely for historical reference.  
> All active development efforts have moved to the **Next Generation (NextGen) Version**.

---

## 🚀 Next Generation Version (Preview)

> **Current Phase:** *Technology Preview*  
> This version is a complete rewrite with a plugin-oriented, modular architecture. Expect frequent breaking changes.

---

### 🧠 Kernel Management

The NextGen Kernel Management subsystem provides deep introspection and manipulation capabilities against the Windows kernel state. All operations are designed to work even in the presence of active anti-forensic or rootkit techniques.

#### Callback / Notify Inspection & Removal

| Category | Supported Objects |
|:---|:---|
| **Process Notifications** | `CreateProcessNotify` · `CreateProcessExNotify` · `CreateProcessEx2Notify` |
| **Thread Notifications** | `CreateThreadNotify` |
| **Image Notifications** | `LoadImageNotify` · `LoadImageExNotify` |
| **Registry Notifications** | `RegistryCallback` |
| **Object Notifications** | `ObjectCallback` |
| **BugCheck Callbacks** | `BugCheckCallback` · `BugCheckReasonCallback` |
| **I/O Shutdown Notifications** | `IoShutdownNotify` · `IoLastChanceShutdownNotify` |
| **File System Notifications** | `FsRegistrationChangeNotify` |
| **Power Callbacks** | `PowerSettingCallback` |
| **Miscellaneous System Callbacks** | `CoalescingCallback` · `PriorityCallback` · `DbgPrintCallback` · `NmiCallback` · `EmpCallback` · `PlugPlayCallback` · `ExCallback` |

#### Filter & Hook Management

| Feature | Capability |
|:---|:---|
| **MiniFilter Inspection** | Enumerate & Force Removal |
| **Standard Filter (Attach Filter)** | Enumerate & Force Removal |
| **SSDT Hook Detection** | Scan & Restore (Inline-Hook / SSDT-Hook) |
| **Shadow SSDT Hook Detection** | Scan & Restore (Inline-Hook / SSSDT-Hook) |

#### Kernel Object Enumeration & Control

| Feature | Capability |
|:---|:---|
| **System Threads** | Enumerate & Terminate |
| **I/O Timer** | Enumerate & Remove |
| **DPC Timer** | Enumerate & Remove |
| **Object Types Table** | Full Dump & Inspection |

#### Advanced Kernel Structures

| Structure | Analysis Capability |
|:---|:---|
| **IDT (Interrupt Descriptor Table)** | View & Hook Detection (Inline-Hook / IDT-Hook) |
| **GDT (Global Descriptor Table)** | Full Table View |
| **PiDDB Cache Table** | View Cached Device Objects |
| **Unloaded Kernel Modules** | Enumerate Traces of Unloaded Drivers |

---

### 🔧 Process Management

> *Basic functionality implemented for the current preview.*

| Operation | Supported |
|:---|:---:|
| Process Enumeration | ✅ |
| Terminate Process | ✅ |
| Force Terminate Process | ✅ |
| Handle Inspection | ❌ (Planned) |
| More... | Planning |

---

### 🧩 Kernel Module Management

| Operation | Supported |
|:---|:---:|
| Module Enumeration | ✅ |
| Force Unload | ✅ |
| View MajorFunction Table | ✅ |

---

### 📁 Disk & File System Management

The File Management module is **not** a traditional file browser. It is a **self-contained File System Parser** that operates directly against the disk—bypassing the Windows storage stack entirely when necessary.

#### Supported File Systems

| File System | Versions / Variants |
|:---|:---|
| **NTFS** | All Windows versions |
| **ReFS** | v1.x · v2.x · v3.x |
| **FAT Family** | FAT12 · FAT16 · FAT32 |
| **ExFAT** | Full support |
| **EXT Family** | EXT2 · EXT3 · EXT4 |
| **BtrFS** | Modern Linux workloads |
| **APFS** | Apple File System |
| **HFS / HFS+** | Legacy Apple File Systems |

---

### 💾 Disk Editor

The Disk Editor shares the same low-level **Read/Write Disk Interface** as the File System Parser. It is designed to operate under the most restrictive conditions:

- **Critical Sector Bypass:** Reads and Writes completely ignore Windows Critical Sector Protection. No `FSCTL_LOCK_VOLUME` required.
- **Sector-Level R/W:** Read and write arbitrary physical sectors by LBA.

#### I/O Backend Implementations

| Layer | Method | Description |
|:---|:---|:---|
| **Disk Layer** | `IRP` → `DISK.SYS` | Standard Windows IRP dispatch. Highest compatibility, lowest privilege escalation. |
| **StorPort Layer** | `SCSI SRB` → Miniport | Directly constructs SCSI Request Blocks and sends them to the vendor storage driver. Bypasses class-level filters. |
| **Direct PCI DMA** | Physical Bus Mastering | Sends commands directly on the PCI bus. Supports **AHCI**, **NVMe**, and **IDE** controllers. Coexists transparently with the Windows storage stack. |

---

## ⚠️ Disclaimer

This tool is intended for **security research, kernel-level debugging, and educational purposes only**. Misuse may lead to system instability, data loss, or violation of applicable laws. The author assumes no liability for any damage caused by this software.

---

*More features, refined architecture, and updated documentation are under active development. Contributions and feedback are welcome once the repository exits the refactoring phase.*