# Zoho OS 🌌

![Zoho Logo](docs/images/zoho_logo.png)

**Zoho OS** is a minimal, educational 64-bit microkernel developed from scratch. It features a modular monolithic architecture, a custom graphics engine, and a suite of observability tools.

## 📺 Video Demonstration
<video src="zoho.mp4" width="800" controls></video>
*Zoho OS boot and shell demonstration.*

---

## 🚀 Features

### 🛡️ Core Kernel
- **64-bit Long Mode**: Full utilization of x86_64 architecture.
- **SMP Scheduler**: Multicore support with task stealing and load balancing.
- **4-Level Paging**: Secure memory isolation between kernel and user space.
- **Physical Memory Manager**: Bitmap-based frame allocator.
- **System Call Interface**: Standardized API for userland applications.

### 🔌 Hardware & Drivers
- **XHCI (USB 3.0)**: Foundation for high-speed USB device support.
- **Network Stack**: Integrated TCP/IP, DHCP, and Intel E1000 driver.
- **Storage**: VFS with support for TAR and EXT2 filesystems.
- **TTY**: Unified abstraction for VGA and Serial (COM1) output.
- **HID**: Support for PS/2 Keyboard and Mouse.

### 📊 Observability & Debugging
- **KTrace**: High-speed kernel execution tracing.
- **KStats**: Real-time monitoring of memory, uptime, and tasks.
- **KLog**: Multi-level logging (DEBUG, INFO, WARN, ERROR).

### 🖥️ Graphics & UI
- **Window Manager**: Custom engine with "Dirty Rectangle" optimization.
- **GUI Dashboard**: Real-time system monitoring app.
- **Interactive Shell**: Userland terminal with basic file management commands.

---

## 🛠️ Getting Started

### Prerequisites
You will need the following tools installed:
- `gcc` (Cross-compiler for x86_64-elf recommended)
- `nasm`
- `ld`
- `make`
- `qemu-system-x86_64`
- `grub-mkrescue`
- `xorriso`

### Building the OS
```bash
make iso
```

### Running in QEMU
```bash
make run
```

---

## 📂 Project Structure
- `src/boot/`: Assembly entry point and 64-bit initialization.
- `src/kernel/`: Core C kernel implementation.
- `src/apps/`: Userland applications (Shell, etc.).
- `include/kernel/`: Kernel headers.
- `docs/`: Technical documentation and assets.

---

## 📜 Documentation
Full technical documentation is available in `documentation.pdf`. To regenerate the PDF from source:
```bash
make docs
```

---

## 👤 Author
**Atharva Bodade**  
Shri Sant Gajanan Maharaj College of Engineering, Shegaon  
[GitHub](https://github.com/UnsettledAverage73/zoho-os)
