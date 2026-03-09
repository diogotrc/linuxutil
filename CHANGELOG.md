# Changelog

All notable changes to LGL System Loadout are documented here.

---

## [1.0.0] — 2026-03-09

### Added
- **pkexec / polkit support** — the app now ships with a `.desktop` file and a polkit policy (`lgl-system-loadout.policy`). When launched from the app launcher, the system authentication dialog prompts for a password once and the wizard runs fully elevated from there. No need to launch from a terminal with `sudo`.
- **System Update page** — new page 2, before all selection pages. Runs `dnf upgrade --refresh` with a live scrolling log output. Skipping is optional but warned against. On completion, detects whether a new kernel was installed and presents a **Reboot Now** button (with confirmation) or **Continue Anyway**.
- **LibreWolf** added to Browsers page as a Flatpak option (`io.gitlab.librewolf-community`), under a new "Privacy-focused" section.

### Changed
- **Thunderbird** switched from DNF to Flatpak (`org.mozilla.Thunderbird`). Moved to the Flatpak section on the Communications page.
- **wget and git** removed as individual checkboxes from the System Tools page. Both are always installed via the bootstrap step at the start of every run.
- **Fedora version fallback** updated from 41 to 43.
- **NetworkManager-wait-online disable** and **DNF cache cleanup** moved from hardcoded always-run steps to opt-in checkboxes in a new "System Tweaks" section on the System Tools page.
- **Virtualisation page** simplified — only virt-manager, libvirt, virt-install, and virt-viewer are shown. qemu-kvm, edk2-ovmf, swtpm, and libvirt daemon components install automatically as dependencies of virt-manager. A note on the page explains this.
- **Bootstrap step** now checks whether all four packages are already present before running and skips itself if so.

### Fixed
- **Badge detection broken on all pages** — `findChild<QLabel*>()` without an object name was matching the internal label inside `QCheckBox` instead of the badge label. Fixed by setting `badge->setObjectName("badge")` in `makeItemRow()` and updating all async callbacks accordingly.
- **AMD GPU badge checks always showing "Not Installed"** — the async check loop was passing the map key (e.g. `mesa_dri`) to `isDnfInstalled()` instead of the actual package name (e.g. `mesa-dri-drivers`). Fixed with an explicit key→package name map.
- **Panel Colorizer badge showing "Not Installed" when installed** — `isPlasmaAppletInstalled()` was checking the filesystem for the full kpackagetool6 ID (`com.github.luisbocanegra.panel.colorizer`) but the plasmoid installs to a directory without the `com.github.` prefix. Fixed by stripping the prefix for filesystem checks while keeping the full ID for the kpackagetool6 fallback.
- **LibreWolf Flatpak app ID** — corrected to `io.gitlab.librewolf-community` after two previous incorrect IDs both failed at install time.
- **Chrome post-install errors** — Chrome's post-install script may fail to import its signing key during installation (exits 7). This is a known upstream quirk and harmless. `InstallWorker` now treats it as a warning rather than a failure.
- **Mouse wheel scrolling** — `SmoothScrollArea` reworked: focus policy changed to `WheelFocus` so scroll events are received without requiring a prior click; event filter added on inner widget so wheel events on child widgets (checkboxes, labels) bubble up correctly; scroll distance per notch corrected (was `angleDelta / 15` producing jumps of 320px, now a proportional 60px per notch).

### Changed (continued)
- **Wine** now installs both `wine` and `wine.i686` to provide full 32-bit support. Without the 32-bit package, Winetricks warns that some applications and games may not work correctly.
- **Protontricks** description updated to note that Steam must be launched and fully set up at least once before Protontricks can function.

### Removed
The following were removed because they install automatically as dependencies of other selected packages, making standalone steps redundant:

- **`gamemode`** — weak dependency of Steam
- **`gamescope`** — weak dependency of Lutris
- **`winetricks`** — dependency of Protontricks
- **`libvirt-daemon-config-network`**, **`libvirt-daemon-kvm`**, **`qemu-kvm`**, **`edk2-ovmf`**, **`swtpm`** — all pulled in by virt-manager

The following were removed because they are part of the Fedora 43 KDE base installation:

- **`curl`**, **`firewall-config`** — System Tools
- **`python3`** — Python page
- **`gstreamer1-plugins-bad-free`**, **`gstreamer1-plugins-good`**, **`gstreamer1-plugins-good-extras`**, **`gstreamer1-plugins-base`**, **`gstreamer1-libav`**, **`lame`/`lame-libs`**, **VA-API libraries** — Multimedia
- **`kvantum`**, **`kvantum-manager`** — Theming
- **`mesa-vdpau-drivers`** — AMD GPU Drivers (superseded by VA-API)
- **`amdvlk`** — AMD GPU Drivers (Mesa RADV is the recommended Vulkan driver on Fedora)

The following were removed for other reasons:

- **`scxctl`** — CachyOS Kernel page. Package does not exist in current Fedora/COPR repos.
- **"Full system upgrade" checkbox** — Repositories page. Redundant now that the System Update page handles this.

---

## [0.1.0] — Initial release

- 17-page Qt6 wizard: Welcome, System Update, Repositories, System Tools, Python, Multimedia, Content Creation, GPU Drivers, Gaming, Virtualisation, Browsers, Communication, Theming, CachyOS Kernel, Review, Install, Done
- Install-only — no removal or uninstall functionality
- No defaults — every checkbox starts unchecked
- Async badge checks via `QtConcurrent` — pages render immediately, installed status updates in background
- `SmoothScrollArea` on all scrollable pages
- Flatpak and Flathub injected automatically if any Flatpak item is selected
- KDE-specific items (KZones, Panel Colorizer) gated to KDE Plasma only
- Live install log with per-step detail panel and progress bar
- Patience message shown during long installs
- Kernel update detection on the System Update page with reboot prompt
- Disk space estimation on the Review page
- Target user detection via `SUDO_USER` / `logname`
- Fedora version detection from the host system
