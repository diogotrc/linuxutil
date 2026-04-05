<div align="center">

# Rapidora

**A curated Fedora setup wizard for gaming, content creation, and development.**  
Pick your loadout. Hit install. Your system is ready.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Fedora](https://img.shields.io/badge/Fedora-43%2B-blue?logo=fedora&logoColor=white)](https://fedoraproject.org)
[![Qt](https://img.shields.io/badge/Qt-6-green?logo=qt&logoColor=white)](https://www.qt.io)
[![KDE Plasma](https://img.shields.io/badge/KDE-Plasma-1d99f3?logo=kde&logoColor=white)](https://kde.org)

</div>

---

## Overview

Rapidora is an incredibly sophisticated graphical installer (Fork) that gets a fresh Fedora install ready in minutes. It features a fully rewritten UI using a modern, animated "Card-based" Dark Mode with Glow effects, driven by an asynchronous QProcess backend in C++ and Qt6. Choose exactly what you want from a curated repository encompassing Gaming, Multimedia, AI Development Ecosystems, Browsers with PGP injections, and significantly more.

- Written from scratch with a luxurious, animated, and responsive interface.
- No terminal needed if you use the pre-built RPM packages from Releases.
- Isolated tools verified dynamically on boot (RPM vs Flatpaks).
- Fully asynchronous smart-threading architecture ensuring zero UI stutter!
- This repository is an **Elevated Fork** actively maintained by **Diogo Travassos**, building upon the foundational framework originally envisioned by the *LinuxGamerLife* YouTube channel.

---

## Requirements

| | |
|---|---|
| **OS** | Fedora 43+ (developed and tested on Fedora 43) |
| **Desktop** | KDE Plasma (some items are KDE-specific) |
| **Connection** | Internet required |

> The wizard includes a guided system update step on page 2. It is recommended to let it run before making any selections.

---

## Installation

### No terminal
Download the RPM in releases and run it. It will open in Discover, click install at the top and enter your password. Once installed, there will be a new icon in Utilities



After installation the app appears in your KDE launcher under **Utilities** as **Rapidora**. Launch it and a single password prompt will appear — the wizard then runs fully elevated.


> **Note — Qt version compatibility:** Always build the binary on the same machine you intend to run it on, or on a machine with the same Qt6 version. A binary built against Qt 6.10 will not run on a system with an older Qt6 installed (`version 'Qt_6.10' not found`). If you see this error, either build from source on the target machine or update Qt6 first:
> ```bash
> sudo dnf upgrade qt6-qtbase
> ```

---

## What's included

| Category | Highlights |
|---|---|
| **System Update** | Supervised `dnf upgrade --refresh` dynamic key upgrade |
| **Repositories** | Automatic Global RPM Fusion Free/NonFree ecosystem injection |
| **System Tools** | C++ Monitored: btop, fastfetch, distrobox, timeshift, and native Fedora utilities |
| **System Tweaks** | Disable local NetworkManager-wait-online and Automatic GNOME/KDE Cloud Account fixes |
| **Multimedia** | Full ffmpeg deployment (RPM fusion full-swap), GStreamer configs, VLC |
| **Content Creation** | OBS Studio for Broadcasters, Kdenlive, GIMP, Inkscape, Audacity, and Sandboxed Blender |
| **GPU Drivers** | Auto Hardware Detection (LSPCI) -> NVIDIA Drivers (Modules/CUDA) or MESA RADEON |
| **Gaming** | Grid-Tree UI Interface. Includes Steam, Heroic, Lutris, ProtonUp-Qt, kernel-modules-extra. Smart Runner modifications. |
| **Virtualisation** | libvirt, virt-manager fully scaled with native automatic usermod grouping. |
| **Browsers** | Hybrid Setup: PGP Dynamic Repo Injection for Brave/Vivaldi/Chrome, paired with Flatpaks/RPM native integrations for Chromium/Firefox/LibreWolf. |
| **Communication & Productivity** | Google Antigravity (AI Dev), ZapZap, Telegram Autostarts, Stellarium, OnlyOffice, and Bazaar Tools. |
| **KDE Theming** | KZones, Panel Colorizer |
| **CachyOS Kernel** | kernel-cachyos, scx-scheds, scx-manager, scx-tools |

---

## License

MIT — see [LICENSE](LICENSE) for details.

---

<div align="center">
Extensively developed by <b>Diogo Travassos</b> (Fork Maintainer) and the Antigravity AI Model.<br>
<br>
<sub><i>Rapidora</i> owes its original pillars and structural backend foundation to <a href="https://github.com/LinuxGamerLife">LinuxGamerLife</a> and his original Claude AI assistants. We honor and preserve the open-source license.</sub>
</div>
