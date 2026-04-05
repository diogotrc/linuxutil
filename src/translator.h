#pragma once
#include <QTranslator>
#include <QMap>
#include <QString>

#include <QObject>

class RapidoraTranslator : public QTranslator {
public:
    explicit RapidoraTranslator(const QString& lang, QObject *parent = nullptr) 
        : QTranslator(parent), m_lang(lang) {
        
        if (m_lang == "en_US") {
            // Language Dialog & Window
            m_en["Rapidora — Selecione o Idioma"] = "Rapidora — Select Language";
            m_en["Próximo  →"] = "Next  →";
            m_en["Rapidora"] = "Rapidora";
            m_en["<span style='font-size:28px; font-weight:700; color:#ffffff;'>Rapidora</span><br/><span style='font-size:14px; color:#8899aa;'>Selecione o idioma para continuar</span>"] = "<span style='font-size:28px; font-weight:700; color:#ffffff;'>Rapidora</span><br/><span style='font-size:14px; color:#8899aa;'>Select language to continue</span>";
            
            // Welcome Page
            m_en["Bem-vindo ao Rapidora"] = "Welcome to Rapidora";
            m_en["Este assistente configurará o seu sistema Fedora para jogos, criação de conteúdo, desenvolvimento e muito mais."] = "This wizard will configure your Fedora system for gaming, content creation, development, and more.";
            m_en["<b style='color: #ff6666;'>Aviso: Esta aplicação deve ser executada como root.</b><br><span style='color: #ffcccc;'>Por favor, reinicie com: <tt>pkexec rapidora</tt></span>"] = "<b style='color: #ff6666;'>Warning: This application must be run as root.</b><br><span style='color: #ffcccc;'>Please restart with: <tt>pkexec rapidora</tt></span>";
            m_en["<p>Este assistente permite escolher exatamente o que instalar. Nada é selecionado por padrão, a escolha é toda sua.</p><p>Você pode regressar e alterar as seleções antes que a instalação comece.</p><p><b>É necessária uma conexão ativa com a internet.</b></p>"] = "<p>This wizard lets you choose exactly what to install. Nothing is selected by default, the choice is entirely yours.</p><p>You can go back and change your selections before the installation begins.</p><p><b>An active internet connection is required.</b></p>";
            m_en["Informações do Sistema"] = "System Information";
            m_en["Versão do Fedora: "] = "Fedora Version: ";
            m_en["Usuário: "] = "User: ";
            m_en["Desconhecida"] = "Unknown";
            m_en["Guia das Caixas de Seleção"] = "Checkbox Guide";
            m_en["Instalado"] = "Installed";
            m_en["Se marcado, o pacote garante atualização/reinstalação."] = "If checked, the package will be updated/reinstalled.";
            m_en["Não Instalado"] = "Not Installed";
            m_en["Se marcado, indica que será baixado e instalado em sua máquina."] = "If checked, the package will be downloaded and installed.";
            m_en["<i>Itens não selecionados permanecerão inalterados pelo sistema.</i>"] = "<i>Unselected items will remain unchanged.</i>";

            // Update Page
            m_en["Atualização do Sistema"] = "System Update";
            m_en["Manter os pacotes de base do Fedora atualizados é essencial antes de efetuarmos injeções profundas de novos programas."] = "Keeping Fedora base packages updated is essential before installing new software.";
            m_en["<b>Por que atualizar agora?</b><br>Instalar novos softwares com bases desatualizadas pode gerar conflitos terríveis de dependências. A garantia de sucesso da engine do Rapidora começa por um sistema em estado impecavelmente limpo e linear ao que a RedHat e a comunidade distribuem hoje."] = "<b>Why update now?</b><br>Installing new software on outdated bases can cause dependency conflicts. Rapidora ensures success by starting with a clean, fully updated system.";
            m_en[" Atualizar Agora "] = " Update Now ";
            m_en[" Pular "] = " Skip ";
            m_en["<span style='color:#cc7700;'>⚠ Execução pulada pelo usuário. Prossiga sob sua própria responsabilidade.</span>"] = "<span style='color:#cc7700;'>⚠ Execution skipped by user. Proceed at your own risk.</span>";
            m_en["<b>⚠ Atualização de Kernel Detectada</b><br><br>Uma parte importantíssima do cérebro do sistema (o Kernel) sofreu alterações. Se formos instalar pacotes base nível-hardware adiante (como drivers GPU Virtuais ou MesaVulkan), eles precisam rastrear os headers deste kernel atualizado ao invés do atual.<br><br>Sugerimos <b>Reiniciar Imediatamente</b> e reabrir o Rapidora."] = "<b>⚠ Kernel Update Detected</b><br><br>The system kernel was updated. Hardware-level packages need to build against the new headers.<br><br>We recommend you <b>Reboot Immediately</b> and reopen Rapidora.";
            m_en["Reiniciar Imediatamente"] = "Reboot Immediately";
            m_en["Ignorar Risco e Continuar"] = "Ignore Risk and Continue";
            m_en["<i>Validando ramificações de Kernel do Fedora antes do procedimento...</i>"] = "<i>Validating Fedora Kernel branches before the procedure...</i>";
            m_en["<i>Negociando com servidores online... Baixando via <tt>dnf upgrade --refresh</tt>.</i>"] = "<i>Negotiating with online servers... Downloading via <tt>dnf upgrade --refresh</tt>.</i>";
            m_en["<span style='color:#3db03d;'>✓ Integrações validadas e Sistema Atualizado de ponta a ponta com sucesso.</span>"] = "<span style='color:#3db03d;'>✓ System updated successfully from end to end.</span>";
            m_en["Confirmação de Reinício"] = "Reboot Confirmation";
            m_en["Reiniciar o Servidor Gráfico e a Máquina agora?\n\nGaranta que trabalhos essenciais foram salvos em outras janelas antes de prosseguir. O assistente do Rapidora precisará ser lançado novamente desde o menu pelo seu usuário após reiniciar o sistema."] = "Reboot the Graphics Server and Machine now?\n\nEnsure work is saved in other windows. You will need to launch Rapidora again after rebooting.";

            // Titles
            m_en["Virtualização de Sistemas"] = "System Virtualization";
            m_en["Repositórios do Sistema"] = "System Repositories";
            m_en["Ferramentas do Sistema"] = "System Tools";
            m_en["Multimídia & Codecs"] = "Multimedia & Codecs";
            m_en["Criação de Conteúdo"] = "Content Creation";
            m_en["Drivers de Vídeo (GPU)"] = "Video Drivers (GPU)";
            m_en["Ecossistema Gamer"] = "Gaming Ecosystem";
            m_en["Navegadores de Internet"] = "Web Browsers";
            m_en["Comunicação & Produtividade"] = "Communication & Productivity";
            m_en["Revisão Final"] = "Final Review";
            m_en["Instalando o Sistema"] = "Installing System";
            m_en["Tudo Pronto!"] = "All Done!";
            
            // Shared Components
            m_en["[Checando...]"] = "[Checking...]";
            m_en["[REINSTALAR]"] = "[REINSTALL]";
            m_en["[INSTALADO]"] = "[INSTALLED]";
            m_en["[NÃO INSTALADO]"] = "[NOT INSTALLED]";
            m_en["☑ Selecionar Todos"] = "☑ Select All";
            m_en["☐ Nenhum"] = "☐ None";
            m_en["Anotações de Erro"] = "Error Logs";
            m_en["Sem erros."] = "No errors.";

            // Programmatically generated additional translations
            m_en["  Reiniciar Agora"] = "  Reboot Now";
            m_en[" Atualizar Agora "] = " Update Now ";
            m_en[" Pular "] = " Skip ";
            m_en["%1 etapa(s) encontraram erros. Verifique o log abaixo."] = "%1 step(s) encountered errors. Check the log below.";
            m_en["(O console DNF emitirá a stream aqui assim que o passo iniciar...)"] = "(The DNF console will feed the stream here as the step starts...)";
            m_en["(Sem detalhes disponíveis)"] = "(No details available)";
            m_en["<b style='color:#ff6b6b;'>Etapas com Erro</b>"] = "<b style='color:#ff6b6b;'>Steps with Errors</b>";
            m_en["<b>Ajustes do Sistema</b>"] = "<b>System Tweaks</b>";
            m_en["<b>Atenção:</b> Os repositórios do RPM Fusion parecem estar ausentes. Volte na Etapa de Repositórios e certifique-se de habilitá-los, ou essas instalações de Codecs irão falhar silenciosamente."] = "<b>Attention:</b> RPM Fusion repositories seem missing. Go back to Repositories and ensure they are enabled.";
            m_en["<b>Auditoria de Armazenamento OK</b><br>Carga Baixada: ~%1 GB &nbsp;|&nbsp; Espaço Livre: %2 GB"] = "<b>Storage Audit OK</b><br>Download Load: ~%1 GB &nbsp;|&nbsp; Free Space: %2 GB";
            m_en["<b>Ferramentas do Sistema & Utilitários</b>"] = "<b>System Tools & Utilities</b>";
            m_en["<b>Paciência é uma virtude! ⏳</b>"] = "<b>Patience is a virtue! ⏳</b>";
            m_en["<b>Pacotes Nativos do Sistema (RPM)</b>"] = "<b>Native System Packages (RPM)</b>";
            m_en["<b>Tamanho Estimado do Pacote: ~%1 GB</b><br>Não foi possível aferir o espaço alocado no disco físico."] = "<b>Estimated Size: ~%1 GB</b><br>Unable to measure physical disk space.";
            m_en["<b>⚠️ Espaço Crítico Insuficiente</b><br>Requeridos: <b>~%1 GB</b> &nbsp;|&nbsp; Disponíveis: <b>%2 GB</b>"] = "<b>⚠️ Insufficient Critical Space</b><br>Required: <b>~%1 GB</b> &nbsp;|&nbsp; Available: <b>%2 GB</b>";
            m_en["<br><br><font color='#ff5555'><b>Ignorando Auditoria: Prosseguindo instalação de forma insegura.</b></font>"] = "<br><br><font color='#ff5555'><b>Ignoring Audit: Proceeding unsafely.</b></font>";
            m_en["<font color='#4cd14c'><b>[OK] Suporte de hardware VT-x/AMD-V habilitado na BIOS.</b></font><br>"] = "<font color='#4cd14c'><b>[OK] VT-x/AMD-V hardware support enabled in BIOS.</b></font><br>";
            m_en["<font color='#ff5555'><b>[FALHA] A Virtualização está DESABILITADA na BIOS ou seu CPU não suporta!</b> As Máquinas Virtuais não poderão iniciar até você corrigir isto, mesmo que instale os pacotes.</font><br><br>"] = "<font color='#ff5555'><b>[FAILURE] Virtualization is DISABLED in BIOS.</b> VMs won't start until fixed.</font><br><br>";
            m_en["<i>Itens não selecionados permanecerão inalterados pelo sistema.</i>"] = "<i>Unselected items will remain unchanged.</i>";
            m_en["<i>Nada selecionado além das ferramentas base.<br>Retorne e selecione itens para construir seu sistema.</i>"] = "<i>Nothing selected besides base tools.<br>Return and select items to build your system.</i>";
            m_en["<i>Negociando com servidores online... Baixando via <tt>dnf upgrade --refresh</tt>.</i>"] = "<i>Negotiating with servers... Downloading via <tt>dnf upgrade --refresh</tt>.</i>";
            m_en["<i>Os seguintes softwares serão empacotados pela plataforma <b>Flatpak via Flathub</b> em caixas de areia blindadas. Nosso instalador conectará e criará as rotas de Flathub automaticamente por você caso não estejam configuradas no sistema.</i>"] = "<i>The following software will be packaged by <b>Flatpak via Flathub</b> in shielded sandboxes.</i>";
            m_en["<i>Validando ramificações de Kernel do Fedora antes do procedimento...</i>"] = "<i>Validating Fedora Kernel branches...</i>";
            m_en["<span style='color:#3db03d;'>✓ Integrações validadas e Sistema Atualizado de ponta a ponta com sucesso.</span>"] = "<span style='color:#3db03d;'>✓ System Updated end-to-end successfully.</span>";
            m_en["<span style='color:#cc7700;'>⚠ Execução pulada pelo usuário. Prossiga sob sua própria responsabilidade.</span>"] = "<span style='color:#cc7700;'>⚠ Execution skipped by user. Proceed at your own risk.</span>";
            m_en["A interface gráfica necessária para configurar a fonte, cor e posicionamento do MangoHud sem precisar encostar em arquivos de texto."] = "The GUI needed to configure your MangoHud without touching text files.";
            m_en["Acelerador Gráfico NVIDIA detectado na barramento físico!"] = "NVIDIA Graphics Accelerator detected on the physical bus!";
            m_en["Aceleração Gráfica"] = "Graphics Acceleration";
            m_en["Aceleração Gráfica e GPU"] = "Graphics Acceleration and GPU";
            m_en["Aguardando inicialização do despachante..."] = "Waiting for dispatcher initialization...";
            m_en["Alternativa fluída do ProtonUp escrita puramente em Vala e GTK4, muito polida para quem prefere designs orgânicos."] = "Fluid ProtonUp alternative written in Vala and GTK4, very polished.";
            m_en["Aplicativos criativos base para vídeos, áudio, vetores e design tridimensional."] = "Base creative applications for videos, audio, vectors, and 3D design.";
            m_en["As operações automáticas foram delegadas aos Workers no background."] = "Automatic operations delegated to background Workers.";
            m_en["Atualização de Base"] = "Base Update";
            m_en["Atualização do Sistema"] = "System Update";
            m_en["Audacity"] = "Audacity";
            m_en["BTop"] = "BTop";
            m_en["Bazaar Center"] = "Bazaar Center";
            m_en["Blender 3D (Flatpak)"] = "Blender 3D";
            m_en["Brave Browser"] = "Brave Browser";
            m_en["Camadas vitais pesadas e não-livres, injetando suporte robusto retroativo a formatos MP3, leitura de DVDs e muito mais ao Totem e afins."] = "Vital heavy non-free layers, injecting robust support for MP3 formats, DVD reading, and more.";
            m_en["Chrome com anabolizantes. Escudos fortes, bloqueio absoluto de anúncios, telemetrias e rastreadores desde o primeiro bip."] = "Chrome on steroids. Strong shields, absolute ad-block, and trackers blocked.";
            m_en["Chromium Base"] = "Chromium Base";
            m_en["Cliente de streaming musical e podcasts oficial da Spotify."] = "Official Spotify music streaming client.";
            m_en["Cliente open source focado exclusivamente no Epic Games, GOG e Amazon Prime de forma hiper otimizada."] = "Open source client tailored for Epic Games, GOG, and Amazon Prime.";
            m_en["Codecs Nativos"] = "Native Codecs";
            m_en["Comunicação & Produtividade"] = "Communication & Productivity";
            m_en["Configuração Concluída"] = "Setup Completed";
            m_en["Configuração de Inicialização"] = "Boot Configuration";
            m_en["Configurações Complementares"] = "Complementary Settings";
            m_en["Confirmação de Reinício"] = "Reboot Confirmation";
            m_en["Construa de uma nova maneira usando IA. Um ecossistema completo provido pelos repositórios globais da Google de forma nativa."] = "Build in a new way using AI. Complete native Google ecosystem.";
            m_en["Construindo o Sistema"] = "Building the System";
            m_en["Controlador daemon vital que comanda os módulos do hypervisor. Habilitado automaticamente no boot."] = "Vital daemon controller for the hypervisor. Enabled automatically at boot.";
            m_en["Controller Support"] = "Controller Support";
            m_en["Corrigir integração KDE Google"] = "Fix KDE Google integration";
            m_en["Criação de Conteúdo"] = "Content Creation";
            m_en["Desabilitar NetworkManager-wait-online"] = "Disable NetworkManager-wait-online";
            m_en["Desconhecida"] = "Unknown";
            m_en["Descubra os astros e trace planetas em tempo real direto da sua máquina."] = "Discover stars and trace planets in real-time.";
            m_en["Disponibiliza software de código aberto isento das restrições proprietárias mas não unificado no repositório Fedora oficial (Ex: codecs de vídeo ffmpeg, h264, h265, reprodutor nativo VLC Media)."] = "Provides open source software free of proprietary restrictions not unified in Fedora.";
            m_en["Distrobox"] = "Distrobox";
            m_en["Driver Proprietário (Código Fechado). Nosso sistema montará o kernel do Akmod-Nvidia utilizando as assinaturas do RPM Fusion NonFree de forma totalmente silenciosa e automática."] = "Proprietary Driver. The system will build Akmod-Nvidia silently.";
            m_en["Drivers AMD MESA"] = "AMD MESA Drivers";
            m_en["Drivers NVIDIA"] = "NVIDIA Drivers";
            m_en["Ecossistema Gaming"] = "Gaming Ecosystem";
            m_en["Encontre complementos incriveis, módulos extras e extensores robustos."] = "Find incredible add-ons, extra modules, and robust extenders.";
            m_en["FFmpeg (Core swap)"] = "FFmpeg (Core swap)";
            m_en["Fastfetch"] = "Fastfetch";
            m_en["Fechar"] = "Close";
            m_en["Ferramenta de estúdio de mesa multipista de rápido processamento para manipulação de áudio."] = "Fast-processing multipack desktop studio tool for audio.";
            m_en["Ferramenta veloz de informações de host de terminal."] = "Fast terminal host information tool.";
            m_en["Ferramentas de Gaming"] = "Gaming Tools";
            m_en["Ferramentas de Sistema"] = "System Tools";
            m_en["Ferramentas de chat, ambientes de construção em IA e suíte de edição."] = "Chat tools, AI environments, and editing suites.";
            m_en["Ferramentas e Ajustes de Sistema"] = "System Tools";
            m_en["Ferramentas nativas do kernel KVM/QEMU para execução de Máquinas Virtuais no nível do hardware."] = "Native KVM/QEMU kernel tools for running VMs.";
            m_en["Finalizar >>"] = "Finish >>";
            m_en["Framework de IA (Fedora RPM)"] = "AI Framework (Fedora RPM)";
            m_en["Framework e Flatpaks"] = "Framework & Flatpaks";
            m_en["From Fedora Repos"] = "From Fedora Repos";
            m_en["GIMP"] = "GIMP";
            m_en["GOverlay"] = "GOverlay";
            m_en["GStreamer Bad-Free Extras"] = "GStreamer Bad-Free Extras";
            m_en["GStreamer Ugly Plugins"] = "GStreamer Ugly Plugins";
            m_en["Game Launchers"] = "Game Launchers";
            m_en["Google Antigravity"] = "Google Antigravity";
            m_en["Google Chrome"] = "Google Chrome";
            m_en["Gráficos Intel Arc/Integrado detectados."] = "Intel Arc/Integrated Graphics detected.";
            m_en["Guia das Caixas de Seleção"] = "Checkbox Guide";
            m_en["HTop"] = "HTop";
            m_en["Habilite grandes fontes de software de terceiros. O RPM Fusion é um pré-requisito estrito para multimídia avançada, jogos fluídos e pacotes fechados."] = "Enable massive third-party software sources like RPM Fusion.";
            m_en["Heroic Games Launcher (Flatpak)"] = "Heroic Games Launcher";
            m_en["INSTALADO"] = "INSTALLED";
            m_en["Identifique sua placa de vídeo principal. Isso garantirá os drivers corretos e aceleradores no kernel do Linux e RPM Fusion."] = "Identify your main graphics card to ensure proper drivers.";
            m_en["Ignorar Risco e Continuar"] = "Ignore Risk and Continue";
            m_en["Informações do Sistema"] = "System Info";
            m_en["Iniciar Instalação"] = "Start Setup";
            m_en["Inkscape"] = "Inkscape";
            m_en["Instalado"] = "Installed";
            m_en["Instalação com Avisos"] = "Setup completed with Warnings";
            m_en["Intel / Máquina Virtual"] = "Intel / Virtual Machine";
            m_en["Interface gráfica amigável de desktop para gerenciar redes virtuais e hospedar Máquinas Virtuais em KVM/QEMU."] = "Friendly GUI to manage virtual networks and VMs in KVM/QEMU.";
            m_en["Kdenlive"] = "Kdenlive";
            m_en["Kernel & Escaladores"] = "Kernel & Scalers";
            m_en["Kernel Modules Extra"] = "Kernel Modules Extra";
            m_en["Lançador multifacetado de código aberto perfeito para jogos da GOG, EA, Amazon, emuladores variados e binários offline."] = "Open-source launcher perfect for GOG, EA, Amazon, and emulators.";
            m_en["LibreWolf (Flatpak)"] = "LibreWolf";
            m_en["Libvirt API Daemon"] = "Libvirt API Daemon";
            m_en["Limpeza de DNF Residual"] = "DNF Residual Cleanup";
            m_en["Log vazio."] = "Empty log.";
            m_en["Lutris"] = "Lutris";
            m_en["Maior loja digital do mundo, nativa com a maravilhosa integração do Valve Proton embutida para abrir games de Windows."] = "World's largest digital store natively integrated with Valve Proton.";
            m_en["MangoHud"] = "MangoHud";
            m_en["Manter os pacotes de base do Fedora atualizados é essencial antes de efetuarmos injeções profundas de novos programas."] = "Keeping Fedora's core packages updated is essential.";
            m_en["Mensageiro rápido nativizado para nuvem via API aberta Telegram."] = "Fast native cloud messenger via Telegram API.";
            m_en["Monitor de recursos com uma bela UI colorida nos terminas de shell."] = "Resource monitor with a beautiful colorful UI.";
            m_en["Mozilla Firefox"] = "Mozilla Firefox";
            m_en["Multimídia & Codecs"] = "Multimedia & Codecs";
            m_en["Navegador moderno de código aberto guiado à liberdade de dados. Ele já acompanha o Fedora Core por natureza."] = "Modern open source browser guided by data freedom.";
            m_en["Navegadores"] = "Web Browsers";
            m_en["Navegadores & Web"] = "Web & Browsers";
            m_en["Nenhum erro registrado."] = "No errors recorded.";
            m_en["Nenhuma GPU dedicada detectada (Fallback / Intel)."] = "No dedicated GPU detected (Fallback).";
            m_en["Nenhuma categoria selecionada."] = "No category selected.";
            m_en["NÃO INSTALADO"] = "NOT INSTALLED";
            m_en["Não Instalado"] = "Not Installed";
            m_en["O MSI Afterburner do Linux. Permite travar quadros e injetar métricas completas e belíssimas de GPU/CPU nos seus jogos."] = "The MSI Afterburner of Linux. FPS locking and hardware metrics.";
            m_en["O eterno e clássico editor de gráficos raster recheado de recursos para edição de fotos."] = "The classic raster graphics editor loaded with photo editing features.";
            m_en["O fluxo STDOUT dos contêineres e logs DNF será alimentado em tempo-real aqui.\\nClique em uma Categoria à esquerda para auditar..."] = "STDOUT flow containers and DNF logs will feed in real-time here.";
            m_en["OBS Studio"] = "OBS Studio";
            m_en["OnlyOffice"] = "OnlyOffice";
            m_en["Otimiza tempo de boot em quase 20s desativando uma service ociosa."] = "Optimizes boot time by disabling an idle service.";
            m_en["Overload: Falta %1 MB"] = "Overload: Missing %1 MB";
            m_en["Padrão ouro da indústria para gravação de tela, composição de cenas e streaming ao vivo."] = "Golden industry standard for screen recording and live streaming.";
            m_en["Performance & Monitoring"] = "Performance & Monitoring";
            m_en["Placa de Vídeo / APU AMD detectada na barramento físico!"] = "AMD Graphics Card / APU detected!";
            m_en["Placas gráficas híbridas, APUs estritas Intel ou Máquinas Virtuais. O Mesa nativo padrão do Fedora será unicamente o suficiente."] = "Hybrid graphics cards, Intel APUs, or VMs. The default Mesa will suffice.";
            m_en["Plano de comissionamento de sistema gerado. Revise todas as automações alocadas abaixo e clique em iniciar."] = "System commissioning plan generated. Review automations below.";
            m_en["Plataforma profissional de manipulação de vetores SVG perfeitamente escaláveis."] = "Professional SVG vector manipulation platform.";
            m_en["Plataformas, camadas de compatibilidade Windows, overlays de desempenho e suporte a controles."] = "Platforms, Windows compatibility layers, and overlays.";
            m_en["Player hiper robusto famoso por conter seus próprios codecs auto-suficientes, abrindo de tudo e um pouco mais sem dependências externas."] = "Hyper robust player containing self-sufficient codecs.";
            m_en["Poderosa suíte corporativa, 100% compatível localmente com planilhas e docs Word XSLX."] = "Powerful enterprise suite, 100% compatible locally with Word XSLX.";
            m_en["Poderoso editor de vídeo não-linear mantido livre sob o guarda-chuva do projeto KDE."] = "Powerful non-linear video editor.";
            m_en["Portais principais da Web. Navegadores de terceiros terão seus repositórios isolados adicionados automaticamente via dnf."] = "Main Web portals and third-party browsers.";
            m_en["Portal chave para pacotes comerciais/fechados vitais para certas rotinas do Linux Gamer, autorizando injeção limpa de ferramentas como o App Steam, Drivers proprietários da NVIDIA e componentes corporativos atrelados."] = "Key portal for vital commercial packages for gaming.";
            m_en["Privacy-focused (Flatpak from Flathub)"] = "Privacy-focused";
            m_en["Procedendo e consolidando %1 pacote(s) ou comandos da categoria."] = "Proceeding with %1 packages/commands.";
            m_en["Projetado para Power Users. Inúmeras funções densas de customização lateral e divisões de janelas baseadas em Chromium."] = "Designed for Power Users. Dense customization features.";
            m_en["Prosseguir de qualquer forma (Sob Próprio Risco)"] = "Proceed anyway (At Own Risk)";
            m_en["Proteção de sistema via Snapshots incríveis de estado completo via BTRFS."] = "System protection via full-state Snapshots.";
            m_en["Proton & Compatibility Tools"] = "Proton & Compatibility Tools";
            m_en["ProtonPlus (Flatpak)"] = "ProtonPlus";
            m_en["ProtonUp-Qt (Flatpak)"] = "ProtonUp-Qt";
            m_en["Protontricks"] = "Protontricks";
            m_en["Próximo  →"] = "Next  →";
            m_en["Recomendado caso o seu controle (ex: Xbox, DualShock) não seja detectado nativamente. Provém drivers faltantes ao Kernel base."] = "Recommended if your controller is not natively detected.";
            m_en["Redes & Comunicação (Flatpaks Isolados)"] = "Network & Communication";
            m_en["Refaz varredura DNF de instalação no cache local"] = "Remakes DNF installation scan in local cache";
            m_en["Reiniciar Imediatamente"] = "Reboot Immediately";
            m_en["Repositórios RPM Fusion"] = "RPM Fusion Repositories";
            m_en["Repositórios de Software"] = "Software Repositories";
            m_en["Repositórios do Sistema"] = "System Repositories";
            m_en["Script automatizado que injeta vcrun, directx ou media foundation dentro de garrafas de jogos específicos da Steam local."] = "Automated script that injects dependencies inside Steam bottles.";
            m_en["Se marcado, indica que será baixado e instalado em sua máquina."] = "If checked, it will be downloaded and installed.";
            m_en["Se marcado, o pacote garante atualização/reinstalação."] = "If checked, it guarantees update/reinstallation.";
            m_en["Seu sistema foi otimizado com sucesso."] = "Your system has been successfully optimized.";
            m_en["Spotify App"] = "Spotify App";
            m_en["Steam"] = "Steam";
            m_en["Stellarium Planetarium"] = "Stellarium Planetarium";
            m_en["Sub-instale ArchLinux, Debian e outras distros no mesmo terminal."] = "Sub-install ArchLinux, Debian, etc. in the same terminal.";
            m_en["Sub-plugins adicionais de extensão base para leitura em matrizes de players nativos."] = "Additional sub-plugins for extension matrix reading.";
            m_en["Sub-virtualização"] = "Sub-virtualization";
            m_en["Substitui o pacote ffmpeg-free castrado do Fedora pelo pacote original completo contendo todos os codificadores modernos H264 e H265 ativados."] = "Replaces ffmpeg-free with fully featured ffmpeg with H264 and H265.";
            m_en["Substitui o provedor de accounts KDE por um script que restabelece com perfeição o login do Google Drive e YouTube nas contas online."] = "Fixes KDE online accounts for Google Drive and YouTube.";
            m_en["Sumário Executivo"] = "Executive Summary";
            m_en["Suporte a vídeo, áudio e codecs. O repositório RPM Fusion é estritamente exigido a seguir."] = "Video, audio, and codecs support.";
            m_en["Suíte Open Source Integrada diretamente e ativamente ao Kernel. Instala extensões RADV, Mesa-DRI e Vulkan em massa."] = "Installs RADV, Mesa-DRI, and Vulkan extensions en masse.";
            m_en["Suíte líder de mercado, de código aberto para modelagem 3D, animação, rig de texturas e até simulação real-time."] = "Open-source suite for 3D modeling, animation, and texture rigging.";
            m_en["Telegram Desktop"] = "Telegram Desktop";
            m_en["Third-Party (Repo added automatically)"] = "Third-Party";
            m_en["Timeshift"] = "Timeshift";
            m_en["Tudo pronto!"] = "All set!";
            m_en["Tudo pronto! Seu sistema Fedora foi otimizado com sucesso."] = "All set! Your Fedora system was optimized successfully.";
            m_en["Um grande fork do Firefox hiper-focado em privacidade absoluta em tempo real. Empacotado sob segurança sandbox do Flathub."] = "A major Firefox fork focused on absolute privacy via sandbox.";
            m_en["Usuário: "] = "User: ";
            m_en["Utilitário GUI essencial que instala a última versão experimental do Proton-GE, Luxtorpeda e Wine-GE com poucos cliques."] = "Essential GUI utility that installs experimental Proton versions.";
            m_en["Utilitário avançado de linha de comando usado para programar criação massificada de VMs de forma automatizada."] = "Advanced CLI utility to map automated mass creation of VMs.";
            m_en["Utilitários essenciais de manutenção e otimização da máquina."] = "Essential maintenance and optimization machine utilities.";
            m_en["VLC Media Player"] = "VLC Media Player";
            m_en["Versão do Fedora: "] = "Fedora Version: ";
            m_en["Versão open source despida sem as telemetrias e bloqueios da Google. Nenhuma conta atrelada."] = "Open-source version without telemetry and Google blocks.";
            m_en["Virt-Install"] = "Virt-Install";
            m_en["Virt-Manager"] = "Virt-Manager";
            m_en["Virt-Viewer"] = "Virt-Viewer";
            m_en["Virtualização (KVM/QEMU)"] = "Virtualization (KVM/QEMU)";
            m_en["Virtualização de Sistemas"] = "System Virtualization";
            m_en["Visualizador de baixissima latência que injeta SPICE e VNC para exibição robusta remota da tela das máquinas virtuais."] = "Low latency viewer using SPICE and VNC for VMs.";
            m_en["Visualizador simples interativo baseado em textos (clássico)."] = "Simple text-based interactive viewer.";
            m_en["Vivaldi"] = "Vivaldi";
            m_en["Windows Compatibility"] = "Windows Compatibility";
            m_en["Wine (Multilib)"] = "Wine (Multilib)";
            m_en["Wrapper otimizadíssimo de WhatsApp Web feito nativamente em C++ WebKit."] = "Highly-optimized WhatsApp Web wrapper locally in WebKit.";
            m_en["ZapZap (WhatsApp)"] = "ZapZap (WhatsApp)";
            m_en["[Checando DNF...]"] = "[Checking DNF...]";
            m_en["[Checando...]"] = "[Checking...]";
            m_en["[INSTALADO]"] = "[INSTALLED]";
            m_en["[NÃO INSTALADO]"] = "[NOT INSTALLED]";
            m_en["[REINSTALAR]"] = "[REINSTALL]";
            m_en["↻ Atualizar"] = "↻ Update";
            m_en["⏳ Interrogando o sistema anfitrião..."] = "⏳ Interrogating host...";
            m_en["⏳ Rastreador em andamento..."] = "⏳ Scanning...";
            m_en["⏳ Validando mirrors locais..."] = "⏳ Validating mirrors...";
            m_en["⏳ Varrendo pacotes KVM Locais..."] = "⏳ Scanning KVM packages...";
            m_en["⏳ Verificando integridade..."] = "⏳ Checking integrity...";
            m_en["★ RECOMENDADO MÁQUINA"] = "★ MACHINE RECOMMENDED";
            m_en["☐ Nenhum"] = "☐ None";
            m_en["☑ Selecionar Todos"] = "☑ Select All";
            m_en["☑ Todos"] = "☑ All";
            m_en["⚠  %1 etapa(s) com erro"] = "⚠ %1 step(s) with error";
            m_en["⚠️ Construção Concluída: Detectados %1 erro(s). Reveja o Log."] = "⚠️ Done: %1 error(s) detected. Review the Log.";
            m_en["✔  0 erros encontrados — Instalação perfeita!"] = "✔ 0 errors found — Flawless installation!";
            m_en["🏁 Construção Concluída — Nenhuma advertência pendente."] = "🏁 Success — No pending warnings.";
            m_en["🐧 Sistema Alvo: Fedora %1   |   👤 Usuário: %2"] = "🐧 System: Fedora %1   |   👤 User: %2";
            m_en["👁 Esconder Metadados (Modo Compacto)"] = "👁 Hide Metadata";
            m_en["👁 Exibir Interações (Modo Completo)"] = "👁 Show Output";
            m_en["📋  Copiar Log Completo"] = "📋 Copy Full Log";
            m_en["📋  Copiar Log de Erros"] = "📋 Copy Error Log";
            m_en["🔍 Buscar pacotes (ex: Lutris, Proton)..."] = "🔍 Search packages...";
            m_en["🔎 Sondando PCI Local em busca de GPUs..."] = "🔎 Probing PCI for GPUs...";
            m_en["🟢 Aterrisando: "] = "🟢 Proceeding: ";

        }
    }

    bool isEmpty() const override { return false; }
    
    QString translate(const char *context, const char *sourceText, const char *disambiguation = nullptr, int n = -1) const override {
        Q_UNUSED(context);
        Q_UNUSED(disambiguation);
        Q_UNUSED(n);
        
        if (m_lang == "en_US") {
            QString src = QString::fromUtf8(sourceText);
            if (m_en.contains(src)) {
                return m_en.value(src);
            }
        }
        return QString();
    }

private:
    QString m_lang;
    QMap<QString, QString> m_en;
};
