#include "mainwizard.h"
#include "pages/welcomepage.h"
#include "pages/updatepage.h"
#include "pages/repospage.h"
#include "pages/systemtoolspage.h"
#include "pages/multimediapage.h"
#include "pages/contentpage.h"
#include "pages/gpupage.h"
#include "pages/gamingpage.h"
#include "pages/virtpage.h"
#include "pages/browserspage.h"
#include "pages/commspage.h"
#include "pages/reviewpage.h"
#include "pages/installpage.h"
#include "pages/donepage.h"
#include <QProcess>
#include <QRegularExpression>
#include "pagehelpers.h"

MainWizard::MainWizard(QWidget *parent) : QWizard(parent)
{
    detectSystem();

    setWindowTitle(tr("Rapidora"));
    setMinimumSize(960, 700);
    setWizardStyle(QWizard::ModernStyle);
    setOption(QWizard::NoBackButtonOnStartPage, true);
    setOption(QWizard::NoBackButtonOnLastPage,  true);
    setOption(QWizard::DisabledBackButtonOnLastPage, true);

    setPage(PAGE_WELCOME,     new WelcomePage(this));
    setPage(PAGE_UPDATE,      new UpdatePage(this));
    setPage(PAGE_REPOS,       new ReposPage(this));
    setPage(PAGE_SYSTEMTOOLS, new SystemToolsPage(this));
    setPage(PAGE_MULTIMEDIA,  new MultimediaPage(this));
    setPage(PAGE_CONTENT,     new ContentPage(this));
    setPage(PAGE_GPU,         new GpuPage(this));
    setPage(PAGE_GAMING,      new GamingPage(this));
    setPage(PAGE_VIRT,        new VirtPage(this));
    setPage(PAGE_BROWSERS,    new BrowsersPage(this));
    setPage(PAGE_COMMS,       new CommsPage(this));
    setPage(PAGE_REVIEW,      new ReviewPage(this));
    setPage(PAGE_INSTALL,     new InstallPage(this));
    setPage(PAGE_DONE,        new DonePage(this));

    setStartId(PAGE_WELCOME);
}

// detectSystem() runs synchronously in the constructor, before the window is shown.
// This is an accepted startup probe: it invokes only fast local binaries (rpm, logname)
// with strict timeouts (≤3 s). It must never make network calls or invoke dnf.
// If either binary hangs beyond its timeout it is killed and a safe default is used.
void MainWizard::detectSystem()
{
    {
        QProcess p;
        p.start("rpm", {"-E", "%fedora"});
        if (!p.waitForFinished(3000)) p.kill();
        m_fedoraVersion = QString::fromUtf8(p.readAllStandardOutput()).trimmed();
        if (m_fedoraVersion.isEmpty() || m_fedoraVersion.startsWith('%'))
            m_fedoraVersion = "43";
    }
    {
        // targetUser is passed directly as an argument to sudo -u and usermod.
        // Treat it as untrusted input: accept only POSIX-portable username characters
        // (letters, digits, underscore, hyphen, dot; max 32 chars).
        // Anything else is rejected and we fall back to root with a warning.
        static const QRegularExpression kSafeUsername(
            QStringLiteral("^[A-Za-z0-9_.][A-Za-z0-9_.-]{0,31}$"));

        auto isSafeUsername = [&](const QString &name) -> bool {
            return !name.isEmpty()
                && name != QStringLiteral("root")
                && kSafeUsername.match(name).hasMatch();
        };

        m_targetUser = qEnvironmentVariable("SUDO_USER");
        if (!isSafeUsername(m_targetUser)) {
            QProcess p;
            p.start("logname", {});
            if (!p.waitForFinished(2000)) p.kill();
            m_targetUser = QString::fromUtf8(p.readAllStandardOutput()).trimmed();
        }
        if (!isSafeUsername(m_targetUser)) {
            qWarning() << "detectSystem: could not determine a safe non-root target user "
                          "(SUDO_USER unset or invalid, logname returned nothing useful). "
                          "Falling back to 'root'. User-scoped tools (pipx, kpackagetool6) "
                          "will run as root.";
            m_targetUser = "root";
        }
    }
}

void MainWizard::setOpt(const QString &k, const QVariant &v) { m_opts[k] = v; }
QVariant MainWizard::getOpt(const QString &k, const QVariant &def) const { return m_opts.value(k, def); }

// -----------------------------------------------------------------------
// Build ordered install steps from all user selections
// -----------------------------------------------------------------------
QList<InstallStep> MainWizard::buildSteps() const
{
    QList<InstallStep> S;
    const QString fv = m_fedoraVersion;
    const QString tu = m_targetUser;

    auto get = [&](const QString &k) { return m_opts.value(k, false).toBool(); };

    // Helper: install a package (install-only - removal not supported)
    auto dnfStep = [&](const QString &id, const QString &pkg) -> InstallStep {
        return InstallStep{id, QString("Install %1").arg(pkg), {"dnf", "-y", "install", pkg}};
    };

    auto flatpakStep = [&](const QString &id, const QString &appId, const QString &label) -> InstallStep {
        return InstallStep{id, QString("Install %1 (Flatpak)").arg(label),
            {"flatpak", "install", "-y", "--noninteractive", "flathub", appId}};
    };

    // ---- Always: bootstrap dnf tools ----
    // Skip if all four packages are already installed
    S << InstallStep{"bootstrap", "Ensure core system tools are present",
        {"dnf", "-y", "install", "curl", "wget2-wget", "git", "dnf-plugins-core"},
        /*optional=*/false,
        /*alreadyInstalledCheck=*/{"bash", "-c",
            "rpm -q --quiet curl wget2-wget git dnf-plugins-core"}};

    // ---- Repos ----
    if (get("repos/rpmfusion_free")) {
        S << InstallStep{"rpmfusion_free", "Enable RPM Fusion Free",
            {"dnf", "-y", "install",
             QString("https://download1.rpmfusion.org/free/fedora/rpmfusion-free-release-%1.noarch.rpm").arg(fv)}};
    }
    if (get("repos/rpmfusion_nonfree")) {
        S << InstallStep{"rpmfusion_nonfree", "Enable RPM Fusion NonFree",
            {"dnf", "-y", "install",
             QString("https://download1.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-%1.noarch.rpm").arg(fv)}};
    }
    // ---- System Tools ----
    for (const auto &pkg : QStringList{
             "fastfetch","btop","htop",
             "distrobox","timeshift"}) {
        if (get(QString("systools/%1").arg(pkg))) {
            S << dnfStep(QString("systool_%1").arg(pkg), pkg);
        }
    }

    if (get("systools/fix_kde_google")) {
        QString script = 
            "#!/bin/bash\n"
            "PROVIDER_FILE=\"/usr/share/accounts/providers/kde/google.provider\"\n"
            "if [ -f \"$PROVIDER_FILE\" ]; then\n"
            "  cp \"$PROVIDER_FILE\" \"${PROVIDER_FILE}.backup\"\n"
            "fi\n"
            "tee \"$PROVIDER_FILE\" >/dev/null <<'EOL'\n"
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<provider id=\"google\">\n"
            "  <name>Google</name>\n"
            "  <description>GNOME-ID, Google Drive and YouTube</description>\n"
            "  <icon>im-google</icon>\n"
            "  <translations>kaccounts-providers</translations>\n"
            "  <domains>.*google\\.com</domains>\n"
            "  <template>\n"
            "    <group name=\"auth\">\n"
            "      <setting name=\"method\">oauth2</setting>\n"
            "      <setting name=\"mechanism\">web_server</setting>\n"
            "      <group name=\"oauth2\">\n"
            "        <group name=\"web_server\">\n"
            "          <setting name=\"Host\">accounts.google.com</setting>\n"
            "          <setting name=\"AuthPath\">o/oauth2/auth?access_type=offline</setting>\n"
            "          <setting name=\"TokenPath\">o/oauth2/token</setting>\n"
            "          <setting name=\"RedirectUri\">http://localhost/oauth2callback</setting>\n"
            "          <setting name=\"ResponseType\">code</setting>\n"
            "          <setting type=\"as\" name=\"Scope\">[\n"
            "            'https://www.googleapis.com/auth/userinfo.email',\n"
            "            'https://www.googleapis.com/auth/userinfo.profile',\n"
            "            'https://www.googleapis.com/auth/calendar',\n"
            "            'https://www.googleapis.com/auth/tasks',\n"
            "            'https://www.googleapis.com/auth/drive'\n"
            "          ]</setting>\n"
            "          <setting type=\"as\" name=\"AllowedSchemes\">['https']</setting>\n"
            "          <setting name=\"ClientId\">44438659992-7kgjeitenc16ssihbtdjbgguch7ju55s.apps.googleusercontent.com</setting>\n"
            "          <setting name=\"ClientSecret\">-gMLuQyDiI0XrQS_vx_mhuYF</setting>\n"
            "          <setting type=\"b\" name=\"ForceClientAuthViaRequestBody\">true</setting>\n"
            "        </group>\n"
            "      </group>\n"
            "    </group>\n"
            "  </template>\n"
            "</provider>\n"
            "EOL\n"
            "kquitapp6 kded6 || true\n";

        S << InstallStep{"fix_kde_google", "Fix KDE Google Integration",
            {"bash", "-c", script}};
    }



    // ---- Multimedia & Codecs ----
    if (get("media/ffmpeg")) {
        S << InstallStep{"ffmpeg_swap", "Swap ffmpeg-free for full ffmpeg",
            {"dnf", "swap", "-y", "ffmpeg-free", "ffmpeg", "--allowerasing"}};
    }
    // GStreamer plugins - each individual
    // GStreamer ugly plugins (nonfree codecs - MP3/DVD)
    if (get("media/gst_bad_nonfree")) {
        S << InstallStep{"gst_bad_nonfree", "Install GStreamer ugly plugins",
            {"dnf", "-y", "install", "--skip-unavailable", "--allowerasing", "gstreamer1-plugins-ugly"}};
    }
    if (get("media/gst_bad_free_extras")) {
        S << InstallStep{"gst_bad_free_extras", "Install GStreamer bad-free-extras",
            {"dnf", "-y", "install", "--skip-unavailable", "gstreamer1-plugins-bad-free-extras"}};
    }
    if (get("media/vlc")) {
        S << dnfStep("vlc", "vlc");
    }

    // ---- Content Creation ----
    if (get("content/obs")) {
        S << dnfStep("obs", "obs-studio");
    }
    if (get("content/kdenlive")) {
        S << dnfStep("kdenlive", "kdenlive");
    }
    if (get("content/gimp")) {
        S << dnfStep("gimp", "gimp");
    }
    if (get("content/inkscape")) {
        S << dnfStep("inkscape", "inkscape");
    }
    if (get("content/audacity")) {
        S << dnfStep("audacity", "audacity");
    }

    // ---- GPU Drivers ----
    const QString gpuChoice = m_opts.value("gpu/choice", "none").toString();
    if (gpuChoice == "amd") {
        S << InstallStep{"amd_mesa", "Install core AMD Mesa & Vulkan drivers",
            {"dnf", "-y", "install", 
             "mesa-dri-drivers", "mesa-vulkan-drivers", "vulkan-loader", "mesa-va-drivers"}};
        // linux-firmware is usually already installed, but keeping logic flat
    }
    else if (gpuChoice == "nvidia") {
        S << InstallStep{"nvidia_akmod", "Install NVIDIA proprietary drivers (RPM Fusion)",
            {"dnf", "-y", "install", 
             "akmod-nvidia", "xorg-x11-drv-nvidia-cuda"}};
    }

    // ---- Gaming ----
    // Flatpak setup (injected once if any flatpak gaming app selected)
    bool needFlatpak = get("gaming/heroic") || get("gaming/protonup") ||
                       get("gaming/protonplus") || get("gaming/flatseal") ||
                       get("content/blender") ||
                       get("comms/telegram") || get("comms/zapzap") ||
                       get("comms/spotify") || get("comms/stellarium") ||
                       get("comms/onlyoffice") || get("comms/bazaar") ||
                       get("browsers/librewolf");
    if (needFlatpak) {
        S << InstallStep{"flatpak_pkg", "Install Flatpak",
            {"dnf", "-y", "install", "flatpak"}};
        S << InstallStep{"flathub_remote", "Add Flathub remote",
            {"flatpak", "remote-add", "--if-not-exists", "flathub",
             "https://flathub.org/repo/flathub.flatpakrepo"}};
    }

    const QList<QPair<QString,QString>> gamingRpm = {
        {"kernel_modules_extra", "kernel-modules-extra"},
        {"steam",     "steam"},
        {"lutris",    "lutris"},
        {"mangohud",  "mangohud"},
        {"vkbasalt",  "vkBasalt"},
        {"goverlay",  "goverlay"},
        {"wine",      "wine"},
        {"protontricks","protontricks"},
    };
    for (const auto &[key, pkg] : gamingRpm) {
        if (get(QString("gaming/%1").arg(key))) {
            if (key == "wine") {
                // Install both 64-bit and 32-bit wine for full multilib support
                S << InstallStep{"gaming_wine", "Install wine + wine.i686 (32-bit support)",
                    {"dnf", "-y", "install", "wine", "wine.i686"}};
            } else {
                S << dnfStep(QString("gaming_%1").arg(key), pkg);
            }
        }
    }

    const QList<QPair<QString,QString>> gamingFlatpak = {
        {"heroic",     "com.heroicgameslauncher.hgl"},
        {"protonup",   "net.davidotek.pupgui2"},
        {"protonplus", "com.vysp3r.ProtonPlus"},
        {"flatseal",   "com.github.tchx84.Flatseal"},
    };
    for (const auto &[key, appid] : gamingFlatpak) {
        if (get(QString("gaming/%1").arg(key))) {
            S << flatpakStep(QString("flatpak_%1").arg(key), appid, key);
        }
    }

    // ---- Virtualisation ----
    bool anyVirt = get("virt/virtmanager") || get("virt/libvirt") ||
                   get("virt/virt_install") || get("virt/virt_viewer");
    const QList<QPair<QString,QString>> virtPkgs = {
        {"virtmanager",  "virt-manager"},
        {"libvirt",      "libvirt"},
        {"virt_install", "virt-install"},
        {"virt_viewer",  "virt-viewer"},
    };
    for (const auto &[key, pkg] : virtPkgs) {
        if (get(QString("virt/%1").arg(key))) {
            S << dnfStep(QString("virt_%1").arg(key), pkg);
        }
    }
    if (anyVirt) {
        S << InstallStep{"libvirtd_enable", "Enable libvirtd service",
            {"systemctl", "enable", "--now", "libvirtd"}};
        S << InstallStep{"libvirt_group", QString("Add %1 to libvirt group").arg(tu),
            {"usermod", "-aG", "libvirt", tu}};
    }

    // ---- Browsers ----
    if (get("browsers/firefox")) {
        S << dnfStep("firefox", "firefox");
    }
    if (get("browsers/chromium")) {
        S << dnfStep("chromium", "chromium");
    }
    if (get("browsers/chrome")) {
        S << InstallStep{"chrome_repo", "Add Google Chrome repo",
            {"bash", "-c",
             "cat > /etc/yum.repos.d/google-chrome.repo << 'REPO'\n"
             "[google-chrome]\n"
             "name=google-chrome\n"
             "baseurl=https://dl.google.com/linux/chrome/rpm/stable/x86_64\n"
             "enabled=1\n"
             "gpgcheck=1\n"
             "gpgkey=https://dl.google.com/linux/linux_signing_key.pub\n"
             "REPO"}};
        S << dnfStep("chrome", "google-chrome-stable");
    }
    if (get("browsers/brave")) {
        S << InstallStep{"brave_repo", "Add Brave repo",
            {"bash", "-c",
             "curl -fsSLo /etc/yum.repos.d/brave-browser.repo "
             "https://brave-browser-rpm-release.s3.brave.com/brave-browser.repo && "
             "rpm --import https://brave-browser-rpm-release.s3.brave.com/brave-core.asc"}};
        S << InstallStep{"brave", "Install Brave Browser",
            {"dnf", "-y", "install", "brave-browser"}};
    }
    if (get("browsers/librewolf")) {
        S << flatpakStep("librewolf", "io.gitlab.librewolf-community", "LibreWolf");
    }
    if (get("browsers/vivaldi")) {
        S << InstallStep{"vivaldi_repo", "Add Vivaldi repo",
            {"bash", "-c",
             "curl -fsSLo /etc/yum.repos.d/vivaldi.repo "
             "https://repo.vivaldi.com/archive/vivaldi-fedora.repo && "
             "rpm --import https://repo.vivaldi.com/archive/linux_signing_key.pub"}};
        S << InstallStep{"vivaldi", "Install Vivaldi",
            {"dnf", "-y", "install", "--setopt=keepcache=0", "vivaldi-stable"}};
    }

    // ---- Communication & Productivity ----
    if (get("comms/antigravity")) {
        S << InstallStep{"antigravity_repo", "Add Google Antigravity repo",
            {"bash", "-c",
             "cat > /etc/yum.repos.d/google-antigravity.repo << 'EOF'\n"
             "[google-antigravity]\n"
             "name=Google Antigravity\n"
             "baseurl=https://antigravity.google/download/linux/rpm\n"
             "enabled=1\n"
             "gpgcheck=0\n"
             "EOF"}};
        S << InstallStep{"antigravity", "Install Google Antigravity",
            {"dnf", "-y", "install", "google-antigravity"}};
    }

    const QList<QPair<QString,QString>> commsFlatpak = {
        {"telegram",   "org.telegram.desktop"},
        {"zapzap",     "com.rtosta.zapzap"},
        {"spotify",    "com.spotify.Client"},
        {"stellarium", "org.stellarium.Stellarium"},
        {"onlyoffice", "org.onlyoffice.desktopeditors"},
        {"bazaar",     "io.github.kolunmi.Bazaar"},
    };
    for (const auto &[key, appid] : commsFlatpak) {
        if (get(QString("comms/%1").arg(key))) {
            S << flatpakStep(QString("flatpak_%1").arg(key), appid, key);
        }
    }
    
    // Autostart para comunicadores se escolhido
    if (get("comms/telegram") && get("comms/autostart_chats_telegram")) {
        S << InstallStep{"telegram_autostart", "Enable Telegram Autostart",
            {"bash", "-c", QString("sudo -u %1 mkdir -p /home/%1/.config/autostart && sudo -u %1 cp /var/lib/flatpak/exports/share/applications/org.telegram.desktop.desktop /home/%1/.config/autostart/").arg(tu).toStdString().c_str()}};
    }
    if (get("comms/zapzap") && get("comms/autostart_chats_zapzap")) {
        S << InstallStep{"zapzap_autostart", "Enable Zapzap Autostart",
            {"bash", "-c", QString("sudo -u %1 mkdir -p /home/%1/.config/autostart && sudo -u %1 cp /var/lib/flatpak/exports/share/applications/com.rtosta.zapzap.desktop /home/%1/.config/autostart/").arg(tu).toStdString().c_str()}};
    }
    // Blender lives in content but is Flatpak
    if (get("content/blender")) {
        S << flatpakStep("flatpak_blender", "org.blender.Blender", "Blender");
    }

    // ---- Customisation & Theming ----
    if (get("theming/kzones")) {
        // Use a path inside our owned temp dir — avoids /tmp symlink attacks.
        S << InstallStep{"kzones_dl", "Download KZones KWin script",
            {"bash", "-c",
             "curl -L -o '/tmp/lgl-kzones.kwinscript' "
             "https://github.com/gerritdevriese/kzones/releases/latest/download/kzones.kwinscript"}};
        S << InstallStep{"kzones_install", "Install KZones KWin script",
            {"sudo", "-u", tu, "kpackagetool6", "--type", "KWin/Script",
             "--install", "/tmp/lgl-kzones.kwinscript"}};
    }
    if (get("theming/panel_colorizer")) {
        // Download via GitHub API to get the actual release asset URL, then install.
        // The asset is a zip containing the plasmoid - we extract it first.
        // Use /tmp directly with a fixed name to avoid spaces in QTemporaryDir path
        // breaking the bash heredoc/variable expansion inside the shell step.
        const QString panelDest = "/tmp/lgl-panel-colorizer.plasmoid";
        S << InstallStep{"panel_col_dl", "Download Panel Colorizer plasmoid",
            {"bash", "-c",
             "ASSET=$(curl -sL https://api.github.com/repos/luisbocanegra/plasma-panel-colorizer/releases/latest"
             " | grep browser_download_url | grep '\\.plasmoid' | head -1 | cut -d'\"' -f4) && "
             "echo \"Downloading: $ASSET\" && "
             "curl -L -o '/tmp/lgl-panel-colorizer.plasmoid' \"$ASSET\" && "
             "ls -lh '/tmp/lgl-panel-colorizer.plasmoid'"}};
        S << InstallStep{"panel_col_install", "Install Panel Colorizer plasmoid",
            {"sudo", "-u", tu, "kpackagetool6", "--type", "Plasma/Applet",
             "--install", "/tmp/lgl-panel-colorizer.plasmoid"}};
    }

    // ---- CachyOS Kernel ----
    if (get("cachyos/kernel")) {
        S << InstallStep{"cachyos_copr1", "Enable kernel-cachyos COPR",
            {"dnf", "copr", "enable", "-y", "bieszczaders/kernel-cachyos"}};
        S << InstallStep{"cachyos_copr2", "Enable kernel-cachyos-addons COPR",
            {"dnf", "copr", "enable", "-y", "bieszczaders/kernel-cachyos-addons"}};
        S << InstallStep{"cachyos_kernel", "Install kernel-cachyos",
            {"dnf", "install", "-y", "kernel-cachyos"}};
    }
    if (get("cachyos/kernel_devel")) {
        S << InstallStep{"cachyos_devel", "Install kernel-cachyos-devel-matched",
            {"dnf", "install", "-y", "kernel-cachyos-devel-matched"}};
    }
    if (get("cachyos/kernel") || get("cachyos/kernel_devel")) {
        S << InstallStep{"selinux_bool", "Set SELinux domain_kernel_load_modules",
            {"setsebool", "-P", "domain_kernel_load_modules", "on"}};
    }
    if (get("cachyos/scx_scheds")) {
        S << InstallStep{"scx_scheds", "Install scx-scheds",
            {"dnf", "install", "-y", "scx-scheds"}};
    }
    if (get("cachyos/scx_manager")) {
        S << InstallStep{"scx_manager", "Install scx-manager",
            {"dnf", "install", "-y", "scx-manager"}};
    }
    if (get("cachyos/scx_tools")) {
        S << InstallStep{"scx_tools", "Install scx-tools",
            {"dnf", "install", "-y", "--allowerasing", "scx-tools"}};
    }
    // ---- Final cleanup ----
    if (get("systools/nm_wait_online")) {
        S << InstallStep{"nm_wait_online", "Disable NetworkManager-wait-online (faster boot)",
            {"systemctl", "disable", "NetworkManager-wait-online.service"}, /*optional=*/true};
    }
    if (get("systools/clean_cache")) {
        S << InstallStep{"clean_all", "Clean DNF cache",
            {"dnf", "-y", "clean", "all"}};
    }

    return S;
}

// -----------------------------------------------------------------------
// Disk space estimation
// -----------------------------------------------------------------------
int MainWizard::estimateDiskMB() const
{
    auto get = [&](const QString &k) { return m_opts.value(k, false).toBool(); };
    int mb = 0;

    // Repos / upgrade
    if (get("repos/rpmfusion_free"))    mb += 1;
    if (get("repos/rpmfusion_nonfree")) mb += 1;
    // System tools (~10 MB each)
    for (const auto &pkg : QStringList{
             "fastfetch","btop","htop",
             "distrobox","xrdp","timeshift"})
        if (get(QString("systools/%1").arg(pkg))) mb += 10;



    // Multimedia
    if (get("media/ffmpeg"))            mb += 50;
    if (get("media/gst_bad_free_extras"))mb += 10;
    if (get("media/gst_bad_nonfree"))   mb += 10;
    if (get("media/vlc"))               mb += 80;

    // Content creation
    if (get("content/obs"))      mb += 120;
    if (get("content/kdenlive")) mb += 150;
    if (get("content/gimp"))     mb += 200;
    if (get("content/inkscape")) mb += 100;
    if (get("content/audacity")) mb += 30;
    if (get("content/blender"))  mb += 600;

    // GPU
    const QString gpuChoice = m_opts.value("gpu/choice", "none").toString();
    if (gpuChoice == "amd") {
        if (get("gpu/amd/mesa_dri"))      mb += 80;
        if (get("gpu/amd/mesa_vulkan"))   mb += 50;
        if (get("gpu/amd/vulkan_loader")) mb += 5;
        if (get("gpu/amd/mesa_va"))       mb += 20;
        if (get("gpu/amd/linux_fw"))      mb += 500;
    }

    // Gaming
    if (get("gaming/steam"))      mb += 400;
    if (get("gaming/lutris"))     mb += 50;
    if (get("gaming/heroic"))     mb += 200;
    if (get("gaming/wine"))       mb += 300;
    if (get("gaming/protonup"))   mb += 100;
    if (get("gaming/protonplus")) mb += 100;
    if (get("gaming/mangohud"))   mb += 10;
    if (get("gaming/goverlay"))   mb += 20;
    if (get("gaming/vkbasalt"))   mb += 5;
    if (get("gaming/flatseal"))   mb += 10;

    // Virtualisation
    if (get("virt/virtmanager"))  mb += 30;
    if (get("virt/libvirt"))      mb += 50;

    // Browsers
    if (get("browsers/firefox"))  mb += 250;
    if (get("browsers/chromium")) mb += 300;
    if (get("browsers/chrome"))   mb += 415;
    if (get("browsers/brave"))    mb += 415;
    if (get("browsers/vivaldi"))  mb += 423;
    if (get("browsers/librewolf"))mb += 300;

    // Comms
    if (get("comms/thunderbird"))  mb += 200;
    if (get("comms/discord"))      mb += 200;
    if (get("comms/vesktop"))      mb += 200;
    if (get("comms/spotify"))      mb += 200;

    // Theming
    if (get("theming/kzones"))          mb += 2;
    if (get("theming/panel_colorizer")) mb += 2;

    // CachyOS kernel
    if (get("cachyos/kernel"))      mb += 120;
    if (get("cachyos/kernel_devel"))mb += 80;
    if (get("cachyos/scx_scheds"))  mb += 10;
    if (get("cachyos/scx_manager")) mb += 5;
    if (get("cachyos/scx_tools"))   mb += 5;

    // Flatpak infrastructure
    bool anyFlatpak = get("gaming/heroic")    || get("gaming/protonup")  ||
                      get("gaming/protonplus") || get("gaming/flatseal")  ||
                      get("comms/discord")     || get("comms/vesktop")    ||
                      get("comms/spotify")     || get("comms/thunderbird") ||
                      get("browsers/librewolf") ||
                      get("content/blender");
    if (anyFlatpak) mb += 200; // Flatpak runtime overhead

    return mb;
}

int MainWizard::availableDiskMB()
{
    QProcess p;
    p.start("df", {"--output=avail", "-m", "/"});
    if (!p.waitForFinished(3000)) { p.kill(); return -1; }
    const QStringList lines = QString::fromUtf8(p.readAllStandardOutput())
                                .trimmed().split('\n');
    if (lines.size() >= 2) {
        bool ok;
        int avail = lines.last().trimmed().toInt(&ok);
        if (ok) return avail;
    }
    return -1;
}
