# Zoho Setu Project 2026 - Project Submission

## Baseline kernel OS 🌌

<video src="zoho.mp4" width="800" controls autoplay loop muted></video>
*A minimal, high-performance 64-bit microkernel demonstration.*

---

### 📝 Submission Details

| Field | Details |
| :--- | :--- |
| **Student Name** | Atharva Bodade |
| **Branch** | Information Technology |
| **Class** | Third Year |
| **College Name** | Shri Sant Gajanan Maharaj College of Engineering, Shegaon |
| **Project Type** | Linux |
| **Category** | Software Based |

---

## 🚀 Overview
**Baseline kernel OS** is an educational x86_64 microkernel designed with a focus on modularity and observability. The goal of this project is to create a minimal Linux-like kernel with core components including drivers, memory management, and a basic functional shell.

## 🏗️ System Architecture

Baseline kernel OS implements a **Modular Monolithic Architecture**. While core services run in Ring 0 for performance, the system is logically divided into distinct layers:

| Layer | Components |
| :--- | :--- |
| **User Space (Ring 3)** | Waybar / GUI, User Shell, System Dashboard |
| **System Call Interface** | INT 0x80 / Syscall Entry |
| **Kernel Core (Ring 0)** | SMP Scheduler, VFS (TAR/EXT2), TCP/IP Stack, VMM (4-Level Paging), PMM (Bitmap) |
| **Hardware Abstraction (HAL)** | PCI/ACPI, ATA Storage, E1000 Network, XHCI USB, HID (Kbd/Mouse) |

### 🛡️ Core Features
- **64-bit Long Mode**: Full utilization of modern x86_64 architectural features.
- **SMP Scheduler**: Multicore multitasking with task stealing and load balancing.
- **4-Level Paging**: Secure hardware-enforced isolation between kernel and userland.
- **Observability Stack**: Integrated high-speed tracing (**KTrace**) and real-time telemetry (**KStats**).
- **GUI Engine**: Custom window manager with "Dirty Rectangle" redraw optimization.

---

## 🛠️ Build & Run

### 📦 Prerequisites
You will need `gcc`, `nasm`, `ld`, `make`, `qemu-system-x86_64`, `grub-mkrescue`, and `xorriso`.

### 🔨 Compilation
```bash
make iso
```

### 🏃 Execution
```bash
make run
```

---

## 📂 Project Structure
- `src/boot/`: Assembly entry and 64-bit bootstrapping.
- `src/kernel/`: Core executive, memory management, and drivers.
- `src/apps/`: User-space applications and system shell.
- `include/`: Unified kernel API and subsystem headers.

## 📜 Documentation
A comprehensive technical deep-dive is available in the [Technical Report (PDF)](documentation.pdf).

---

## ⚖️ License
This project is licensed under the **MIT License**.

Copyright (c) 2026 Atharva Bodade

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
