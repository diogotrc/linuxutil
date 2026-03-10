<div align="center">

# LGL System Loadout

**A curated Fedora setup wizard for gaming, content creation, and development.**  
Pick your loadout. Hit install. Your system is ready.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Fedora](https://img.shields.io/badge/Fedora-43%2B-blue?logo=fedora&logoColor=white)](https://fedoraproject.org)
[![Qt](https://img.shields.io/badge/Qt-6-green?logo=qt&logoColor=white)](https://www.qt.io)
[![KDE Plasma](https://img.shields.io/badge/KDE-Plasma-1d99f3?logo=kde&logoColor=white)](https://kde.org)

</div>

---

## Overview

LGL System Loadout is a graphical wizard that gets a fresh Fedora install ready in minutes. Choose exactly what you want from a curated list of packages across gaming, multimedia, content creation, development tools, browsers, communication apps, GPU drivers, virtualisation, KDE theming, and the CachyOS kernel.

- No defaults — nothing is pre-selected
- Every item shows its current installed state before you commit
- All checks run concurrently so pages load instantly
- Installs only — nothing is removed without your knowledge

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

### Recommended — COPR (Fedora 43)

```bash
sudo dnf copr enable linuxgamerlife/lgl-system-loadout
sudo dnf install lgl-system-loadout
```

After installation the app appears in your KDE launcher under **Utilities** as **LGL System Loadout**. Launch it and a single password prompt will appear — the wizard then runs fully elevated.

### Build from source

```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel
unzip lgl-system-loadout-1.0.3.zip
cd lgl-system-loadout-1.0.1
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo ./lgl-system-loadout
```

> **Important:** Build on the machine you intend to run it on. A binary built against a newer Qt6 than your system has will fail with `version 'Qt_6.10' not found`. If you see this, run `sudo dnf upgrade qt6-qtbase` first, or build from source locally.

---

## Requirements

| | |
|---|---|
| **OS** | Fedora 43+ |
| **Desktop** | KDE Plasma |
| **Connection** | Internet required during install |

---

## What's included

| Category | Highlights |
|---|---|
| **System Update** | Guided `dnf upgrade --refresh` with kernel detection and reboot prompt |
| **Repositories** | RPM Fusion Free & NonFree |
| **System Tools** | btop, fastfetch, distrobox, timeshift, xrdp, and more |
| **System Tweaks** | Disable NetworkManager-wait-online · Clean DNF cache after install |
| **Python** | pip, pipx, yt-dlp, tldr |
| **Multimedia** | ffmpeg, GStreamer plugins, VLC |
| **Content Creation** | OBS Studio, Kdenlive, GIMP, Inkscape, Audacity, Blender |
| **GPU Drivers** | AMD (Mesa, Vulkan, VA-API, firmware) |
| **Gaming** | Steam, Lutris, Wine, Protontricks, MangoHud, vkBasalt, GOverlay, Heroic, ProtonUp-Qt, ProtonPlus, Flatseal |
| **Virtualisation** | virt-manager, libvirt, virt-install, virt-viewer |
| **Browsers** | Firefox, Chromium, Chrome, Brave, Vivaldi, LibreWolf |
| **Communication** | Thunderbird, Discord, Vesktop, Spotify |
| **KDE Theming** | KZones, Panel Colorizer |
| **CachyOS Kernel** | kernel-cachyos, scx-scheds, scx-manager, scx-tools |

---

## Release assets

| File | Description |
|---|---|
| `lgl-system-loadout-1.0.3.zip` | Source code |

---

<div align="center">
Made for <a href="https://fedoraproject.org">Fedora</a> · by <a href="https://www.youtube.com/@linuxgamerlife">LinuxGamerLife</a>
</div>
