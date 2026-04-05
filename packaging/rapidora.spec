Name:           rapidora
Version:        1.0.7
Release:        1%{?dist}
Summary:        Guided setup wizard for Fedora — gaming, content creation, and development

License:        MIT
URL:            https://github.com/linuxgamerlife/rapidora
Source0:        https://github.com/linuxgamerlife/rapidora/releases/download/v%{version}/rapidora-%{version}.zip

BuildRequires:  cmake >= 3.16
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  unzip

# Runtime dependencies
# qt6-qtbase is the only true library dependency — all other tools (dnf, curl,
# flatpak) are invoked as subprocesses and are already present on any Fedora 43
# system. Declaring them here causes unnecessary resolver conflicts (e.g. dnf vs dnf5).
Requires:       qt6-qtbase
Requires:       polkit

%description
Rapidora is a graphical wizard that gets a fresh Fedora install
ready in minutes. Choose exactly what to install from a curated list spanning
gaming, multimedia, content creation, development tools, browsers,
communication apps, GPU drivers and virtualisation.

Features:
  - No defaults — nothing is pre-selected
  - Every item shows its current installed state before you commit
  - All checks run concurrently so pages load instantly
  - Installs only — nothing is removed without your knowledge
  - Single polkit password prompt via pkexec
  - Native Qt6 language selection dialog
  - Visual celebration screen on completion with per-category badges

%prep
%setup -q -n rapidora-%{version}

%build
mkdir -p build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=%{_prefix}
make -j2

%install
# Binary
install -Dm755 build/rapidora \
    %{buildroot}%{_bindir}/rapidora

# Desktop entry
install -Dm644 packaging/com.linuxgamerlife.rapidora.desktop \
    %{buildroot}%{_datadir}/applications/com.linuxgamerlife.rapidora.desktop

# Polkit policy
install -Dm644 packaging/rapidora.policy \
    %{buildroot}%{_datadir}/polkit-1/actions/com.linuxgamerlife.rapidora.policy

# Icons — hicolor theme sizes
install -Dm644 packaging/rapidora-256.png \
    %{buildroot}%{_datadir}/icons/hicolor/256x256/apps/rapidora.png
install -Dm644 packaging/rapidora-128.png \
    %{buildroot}%{_datadir}/icons/hicolor/128x128/apps/rapidora.png
install -Dm644 packaging/rapidora-64.png \
    %{buildroot}%{_datadir}/icons/hicolor/64x64/apps/rapidora.png
install -Dm644 packaging/rapidora-48.png \
    %{buildroot}%{_datadir}/icons/hicolor/48x48/apps/rapidora.png

# Pixmaps fallback (some older launchers use this)
install -Dm644 packaging/rapidora-256.png \
    %{buildroot}%{_datadir}/pixmaps/rapidora.png

# AppStream metainfo (Discover uses this for version display and app info)
install -Dm644 packaging/com.linuxgamerlife.rapidora.metainfo.xml \
    %{buildroot}%{_datadir}/metainfo/com.linuxgamerlife.rapidora.metainfo.xml

# Global QSS stylesheet
install -Dm644 src/style.qss \
    %{buildroot}%{_datadir}/rapidora/style.qss

%post
if [ -x /usr/bin/gtk-update-icon-cache ]; then
    gtk-update-icon-cache -f -t %{_datadir}/icons/hicolor &>/dev/null || :
fi
if [ -x /usr/bin/update-desktop-database ]; then
    update-desktop-database -q %{_datadir}/applications &>/dev/null || :
fi
if [ -x /usr/bin/appstreamcli ]; then
    appstreamcli refresh --force &>/dev/null || :
fi

%postun
if [ -x /usr/bin/gtk-update-icon-cache ]; then
    gtk-update-icon-cache -f -t %{_datadir}/icons/hicolor &>/dev/null || :
fi
if [ -x /usr/bin/update-desktop-database ]; then
    update-desktop-database -q %{_datadir}/applications &>/dev/null || :
fi
if [ -x /usr/bin/appstreamcli ]; then
    appstreamcli refresh --force &>/dev/null || :
fi

%files
%license LICENSE
%{_bindir}/rapidora
%{_datadir}/applications/com.linuxgamerlife.rapidora.desktop
%{_datadir}/metainfo/com.linuxgamerlife.rapidora.metainfo.xml
%{_datadir}/polkit-1/actions/com.linuxgamerlife.rapidora.policy
%{_datadir}/icons/hicolor/256x256/apps/rapidora.png
%{_datadir}/icons/hicolor/128x128/apps/rapidora.png
%{_datadir}/icons/hicolor/64x64/apps/rapidora.png
%{_datadir}/icons/hicolor/48x48/apps/rapidora.png
%{_datadir}/pixmaps/rapidora.png
%{_datadir}/rapidora/style.qss

%changelog
* Fri Apr 04 2026 Diogo Travassos <diogo@rapidora> - 1.0.7-1
- Bump version to 1.0.7 para garantir o gatilho de atualização do DNF
- Resolução definitiva do bug de path de ícone (fallback p/ generic hicolor "rapidora")
- Implementação customizada In-Memory do QTranslator reduzindo overhead
- Remoção da necessidade de infraestrutura build-time LinguistTools
- Tradução completa para o Inglês: mais de 200 novas strings mapeadas no translator.h



* Fri Apr 04 2026 Diogo Travassos <diogo@rapidora> - 1.0.6-1
- Novo ícone oficial do aplicativo
- Correção de crash na seleção do idioma Inglês
- Padronização da largura dos badges na WelcomePage para melhor alinhamento
- Tela Final (DonePage) completamente reescrita: hero card com glow, fade-in animation,
  badges dinâmicos por categoria instalada, pill de status verde/vermelho
- isRebootRequired(): detecta GPU, Virt e kernel modules para exibir alerta de reboot
- Sistema de QSS global (style.qss): tokens unificados para wizard, cards, badges,
  checkboxes, botões, scrollbars, progress bar, log view e dialogs
- Removidas páginas CachyOS e Theming do wizard
- Badges de status com setFixedWidth(130) em todas as páginas para layout estável
- Ícones de navegadores com fallback progressivo em browserspage
- LanguageDialog Qt6 nativo integrado no fluxo de inicialização
- Corrigidas entidades HTML &nbsp; no badge do Sumário Executivo

* Fri Apr 04 2026 Diogo Travassos <diogo@rapidora> - 1.0.5-1
- Tela Final (DonePage) completamente reescrita: hero card com glow, fade-in animation,
  badges dinâmicos por categoria instalada, pill de status verde/vermelho
- isRebootRequired(): detecta GPU, Virt e kernel modules para exibir alerta de reboot
- Sistema de QSS global (style.qss): tokens unificados para wizard, cards, badges,
  checkboxes, botões, scrollbars, progress bar, log view e dialogs
- Removidas páginas CachyOS e Theming do wizard
- Badges de status com setFixedWidth(130) em todas as páginas para layout estável
- Ícones de navegadores com fallback progressivo em browserspage
- LanguageDialog Qt6 nativo integrado no fluxo de inicialização
- Corrigidas entidades HTML &nbsp; no badge do Sumário Executivo

* Tue Mar 10 2026 LinuxGamerLife <contact@linuxgamerlife.com> - 1.0.4-1
- Metainfo file renamed to reverse-DNS format to match component ID and desktop file

* Tue Mar 10 2026 LinuxGamerLife <contact@linuxgamerlife.com> - 1.0.3-1
- Desktop file renamed to reverse-DNS format (com.linuxgamerlife.rapidora.desktop)
- AppStream metadata now passes validation; app appears correctly in Discover

* Tue Mar 10 2026 LinuxGamerLife <contact@linuxgamerlife.com> - 1.0.2-1
- Fixed AppStream metadata: component ID corrected to match desktop file name
- Discover/GNOME Software can now find and display the application

* Tue Mar 10 2026 LinuxGamerLife <contact@linuxgamerlife.com> - 1.0.1-1
- Panel Colorizer download fixed: QTemporaryDir path contained spaces breaking bash variable expansion
- Panel Colorizer log output no longer shows garbled binary text from file(1) command
- KZones download path fixed: same space-in-path issue as Panel Colorizer
- Kernel warning box no longer overlaps log output on System Update page
- Reboot confirmation dialog clarified: explains unsaved work means other open applications
- Secure Boot warning added to CachyOS Kernel page
- Qt version compatibility note added to README
- Thread lifecycle, atomic cancel flag, QProcess timeout hardening
- PKEXEC_UID and target user input validation

* Mon Mar 09 2026 LinuxGamerLife <contact@linuxgamerlife.com> - 1.0.0-1
- Initial release
