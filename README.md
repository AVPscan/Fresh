[![Build on Linux](https://github.com/AVPscan/Code_editor/actions/workflows/Build%20on%20Linux.yml/badge.svg)](https://github.com/AVPscan/Code_editor/actions/workflows/Build%20on%20Linux.yml)
[![Build on macOS](https://github.com/AVPscan/Fresh/actions/workflows/Build%20on%20macos.yml/badge.svg)](https://github.com/AVPscan/Fresh/actions/workflows/Build%20on%20macos.yml)
[![Build on Windows](https://github.com/AVPscan/Fresh/actions/workflows/Build%20on%20windows.yml/badge.svg)](https://github.com/AVPscan/Fresh/actions/workflows/Build%20on%20windows.yml)
[![Linux musl static +(Cross-Platform)](https://github.com/AVPscan/Code_editor/actions/workflows/release.yml/badge.svg)](https://github.com/AVPscan/Code_editor/actions/workflows/release.yml)

# Fresh

**Fresh** — A high-performance, self-contained terminal engine designed for low-latency system software and embedded applications. Distributed under the **GPLv3** license.

Developed in pure **C** without any external dependencies (no ncurses, no bloated libraries). This makes it an ideal foundation for high-load utilities, embedded systems, and cross-platform system software.

## 🚀 Key Features

*   **Zero-Dependency:** Complete independence from third-party libraries. Built exclusively on raw system calls and the C standard library.
*   **RDTSC Precision Timing:** Leverages the hardware timestamp counter for microsecond-accurate hardware-level delays (`Delay_ms`). Features a hybrid sleep mechanism (nanosleep + active pause) for maximum responsiveness.
*   **Virtual VRAM Architecture:** Direct memory addressing for data, attributes, and offsets. Optimized for CPU cache efficiency through Data/Attr/Offset separation.
*   **Native UTF-8 Core:** Built-in UTF-8 parser with support for wide characters (CJK, emojis, combining marks) and RTL (Right-to-Left) directions. Designed for global compatibility.
*   **Smart Viewport Control:** Three navigation modes (Free, Sticky, Locked) with an integrated debounce mechanism for terminal window resizing.
*   **Asynchronous Real-Time Input:** Non-blocking handling of keyboard and mouse (including X10 protocols). Uses an RLE-compressed event buffer to integrate input "weight" and duration (`tic`) without event loss.
*   **Architecture Agnostic:** Decoupled from word size using a `Cell` abstraction, making it ready for 32-bit, 64-bit, and future wide-word architectures.

## 🛠 Technical Stack

*   **Language:** C11 / POSIX.
*   **Optimization:** Inline Assembly (RDTSC), bitwise mapping, manual memory layout.
*   **Interface:** Manual construction of ANSI/VT100 escape sequences for atomic `write` operations.

## 📊 Performance

The engine is engineered for resource-constrained environments. By managing output buffers manually and eliminating abstraction layers, the input-to-render latency is pushed to the physical limits of the terminal, even on single-core systems.

## 📂 Project Structure

- `sys.h` — Low-level macros, color palettes, and system constants.
- `engine.c` — The core: memory management, viewport logic, and event processing.
- `sys_linux.c` / `sys_windows.c` — Platform-specific system layers.

## 👨‍💻 Author

**Alexey Pozdnyakov**  
E-mail: [avp70ru@mail.ru](mailto:avp70ru@mail.ru)  
GitHub: [AVPscan](https://github.com)

---
*This software is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 3.*

