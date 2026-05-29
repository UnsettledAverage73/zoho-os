# Baseline kernel OS 🌌

<video src="docs/video/zoho.gif" width="800" controls autoplay loop muted></video>
*A minimal, high-performance 64-bit microkernel demonstration.*

---

## 🚀 Overview
**Baseline kernel OS** is an educational x86_64 microkernel designed with a focus on modularity and observability. It implements modern kernel paradigms including symmetric multiprocessing (SMP), advanced memory isolation, and a custom high-resolution graphics engine.

## ✨ Core Features

### 🛡️ Microkernel Architecture
- **64-bit Long Mode**: Native x86_64 execution.
- **SMP Scheduler**: Load-balanced multitasking across multiple CPU cores.
- **4-Level Paging**: Hardware-enforced isolation between kernel and userland.
- **PMM/VMM**: Hybrid bitmap and stack-based memory management.

### 🔌 Hardware Abstraction Layer
- **XHCI Support**: Foundation for USB 3.0 device integration.
- **Networking**: Integrated TCP/IP stack with DHCP and Intel E1000 support.
- **VFS**: Virtual File System supporting TAR and EXT2.
- **TTY**: Unified text output abstraction for VGA and Serial COM1.

### 📊 Integrated Observability
- **KTrace**: Non-blocking high-speed execution tracing.
- **KStats**: Real-time system telemetry (Uptime, Memory, CPU load).
- **KLog**: Structured multi-level kernel logging system.

### 🖥️ User Experience
- **GUI Engine**: Custom window manager with "Dirty Rectangle" optimization.
- **Interactive Shell**: Userland terminal with POSIX-like command support.

---

## 🛠️ Build & Run

### 📦 Prerequisites
Install `gcc`, `nasm`, `ld`, `make`, `qemu`, `grub-mkrescue`, and `xorriso`.

### 🔨 Compilation
```bash
make iso
```

### 🏃 Execution
```bash
make run
```

---

## 📂 Architecture
- `src/boot/`: Bootstrapping and CPU initialization.
- `src/kernel/`: Core executive and hardware drivers.
- `src/apps/`: User-space applications and system shell.
- `include/`: Unified kernel API and headers.

## 📜 Technical Report
For a deep dive into the implementation details, refer to [documentation.pdf](documentation.pdf).

---

**Atharva Bodade** | Shri Sant Gajanan Maharaj College of Engineering, Shegaon  
[GitHub Repository](https://github.com/UnsettledAverage73/zoho-os)
