#include "reviewpage.h"
#include "../mainwizard.h"
#include "../pagehelpers.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QAbstractButton>
#include <QScrollArea>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QMap>

ReviewPage::ReviewPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Sumário Executivo"));
    setSubTitle(tr("Plano de comissionamento de sistema gerado. Revise todas as automações alocadas abaixo e clique em iniciar."));

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(10, 10, 10, 10);

    // ==========================================
    // System Badge Horizontal
    // ==========================================
    m_targetBadge = new QLabel;
    m_targetBadge->setAlignment(Qt::AlignCenter);
    m_targetBadge->setStyleSheet("background-color: rgba(53, 132, 228, 0.15); color: #8cbcf8; font-weight: bold; border-radius: 8px; padding: 6px; font-size: 13px;");
    layout->addWidget(m_targetBadge);

    // ==========================================
    // Disk Frame e Progress Bar
    // ==========================================
    auto *diskFrame = new QFrame;
    diskFrame->setStyleSheet("background-color: rgba(255, 255, 255, 0.04); border: 1px solid rgba(255, 255, 255, 0.08); border-radius: 10px;");
    auto *diskLayout = new QVBoxLayout(diskFrame);
    
    m_diskLabel = new QLabel;
    m_diskLabel->setWordWrap(true);
    m_diskLabel->setStyleSheet("border: none; background: transparent; font-size: 13px; color: #dddddd;");
    
    m_diskBar = new QProgressBar;
    m_diskBar->setTextVisible(true);
    m_diskBar->setAlignment(Qt::AlignCenter);
    m_diskBar->setFixedHeight(20);
    // Base style that we'll alter dynamically depending on state
    
    diskLayout->addWidget(m_diskLabel);
    diskLayout->addWidget(m_diskBar);
    
    layout->addWidget(diskFrame);

    // Proceed anyway button (only visible when disk is low)
    m_proceedBtn = new QPushButton(tr("Prosseguir de qualquer forma (Sob Próprio Risco)"));
    m_proceedBtn->setStyleSheet("QPushButton { background-color: #7a0000; color: white; font-weight: bold; padding: 8px; border-radius: 6px; } QPushButton:hover { background-color: #9a0000; }");
    m_proceedBtn->setCursor(Qt::PointingHandCursor);
    m_proceedBtn->setVisible(false);
    connect(m_proceedBtn, &QPushButton::clicked, this, &ReviewPage::onProceedAnyway);
    layout->addWidget(m_proceedBtn);

    // ==========================================
    // Timeline ScrollArea
    // ==========================================
    auto *scroll = new SmoothScrollArea; 
    scroll->setWidgetResizable(true); 
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background-color: transparent; } QWidget#TimelineContainer { background-color: transparent; }");
    
    auto *inner = new QWidget; 
    inner->setObjectName("TimelineContainer");
    m_timelineLayout = new QVBoxLayout(inner); 
    m_timelineLayout->setSpacing(8);
    m_timelineLayout->setContentsMargins(0, 5, 10, 5);
    
    m_emptyStateWidget = new QWidget;
    auto *emptyLayout = new QVBoxLayout(m_emptyStateWidget);
    auto *emptyLabel = new QLabel(tr("<i>Nada selecionado além das ferramentas base.<br>Retorne e selecione itens para construir seu sistema.</i>"));
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet("color: #777777; background-color: rgba(255,255,255,0.03); padding: 20px; border-radius: 12px; font-size: 14px;");
    emptyLayout->addWidget(emptyLabel);
    m_timelineLayout->addWidget(m_emptyStateWidget);
    
    m_timelineLayout->addStretch();
    scroll->setWidget(inner);
    layout->addWidget(scroll);
}

void ReviewPage::initializePage()
{
    m_proceedForced = false;

    // Reset Next Button Text and Effects
    auto *nextBtn = wizard()->button(QWizard::NextButton);
    if (nextBtn) {
        nextBtn->setText(tr("Iniciar Instalação"));
        nextBtn->setGraphicsEffect(nullptr); // Apaga glow anterior caso usuário fique voltando página
    }

    // Set System Badge Dynamic Text
    m_targetBadge->setText(QString(tr("🐧 Sistema Alvo: Fedora %1   |   👤 Usuário: %2"))
        .arg(m_wiz->fedoraVersion())
        .arg(m_wiz->targetUser()));

    // ==========================================
    // Calcula Metadados e Gráfico de Disco
    // ==========================================
    const QList<InstallStep> steps = m_wiz->buildSteps();
    const int neededMB   = m_wiz->estimateDiskMB();
    const int availMB    = MainWizard::availableDiskMB();

    // Add 15% buffer
    const int neededWithBuffer = neededMB + (neededMB * 15 / 100) + 500; 

    // Helper p/ conversão em GB pra UI
    double dNeeded = neededMB / 1024.0;
    double dAvail  = availMB / 1024.0;

    if (availMB < 0) {
        m_diskLabel->setText(QString(tr("<b>Tamanho Estimado do Pacote: ~%1 GB</b><br>Não foi possível aferir o espaço alocado no disco físico.")).arg(dNeeded, 0, 'f', 1));
        m_diskBar->setVisible(false);
        m_diskOk = true; 
        m_proceedBtn->setVisible(false);
    } else if (availMB < neededWithBuffer) {
        m_diskLabel->setText(QString(tr("<b>⚠️ Espaço Crítico Insuficiente</b><br>Requeridos: <b>~%1 GB</b> &nbsp;|&nbsp; Disponíveis: <b>%2 GB</b>")).arg(neededWithBuffer / 1024.0, 0, 'f', 1).arg(dAvail, 0, 'f', 1));
        m_diskBar->setVisible(true);
        m_diskBar->setRange(0, 100);
        m_diskBar->setValue(100); // Superlotado
        m_diskBar->setFormat(tr("Overload: Falta %1 MB").arg(neededWithBuffer - availMB));
        m_diskBar->setStyleSheet("QProgressBar { background-color: rgba(255, 255, 255, 0.1); border-radius: 4px;text-align: center; color: white; font-weight: bold; } QProgressBar::chunk { background-color: #d14c4c; border-radius: 4px; }");
        
        m_diskOk = false;
        m_proceedBtn->setVisible(true);
    } else {
        m_diskLabel->setText(QString(tr("<b>Auditoria de Armazenamento OK</b><br>Carga Baixada: ~%1 GB &nbsp;|&nbsp; Espaço Livre: %2 GB")).arg(dNeeded, 0, 'f', 1).arg(dAvail, 0, 'f', 1));
        m_diskBar->setVisible(true);
        m_diskBar->setRange(0, availMB);
        m_diskBar->setValue(neededMB);
        m_diskBar->setFormat(QString("%1 GB Requeridos").arg(dNeeded, 0, 'f', 1));
        
        // Estilo Verde Suave Moderno
        m_diskBar->setStyleSheet("QProgressBar { background-color: rgba(255, 255, 255, 0.08); border-radius: 6px;text-align: center; color: #ffffff; font-weight: bold; } QProgressBar::chunk { background-color: #4cd14c; border-radius: 6px; }");
        
        m_diskOk = true;
        m_proceedBtn->setVisible(false);

        // ==========================================
        // Dica Vibecoding: Brilho/Glow de Reforço Positivo
        // ==========================================
        if (nextBtn) {
            auto *glow = new QGraphicsDropShadowEffect(nextBtn);
            glow->setBlurRadius(25);
            glow->setColor(QColor(76, 209, 76, 180)); // Verde Brilhante
            glow->setOffset(0, 0);
            nextBtn->setGraphicsEffect(glow);
        }
    }

    emit completeChanged();

    // ==========================================
    // Timeline Construction!
    // ==========================================
    // Remove widgets antigos (exceto o stretch e o emptyStateWidget que mantemos ponteiros)
    QLayoutItem *child;
    while ((child = m_timelineLayout->takeAt(0)) != nullptr) {
        if (child->widget() && child->widget() != m_emptyStateWidget) {
            child->widget()->deleteLater();
        }
        delete child;
    }

    if (steps.size() <= 1) {
        m_timelineLayout->addWidget(m_emptyStateWidget);
        m_emptyStateWidget->setVisible(true);
        m_timelineLayout->addStretch();
        return;
    }
    m_emptyStateWidget->setVisible(false);

    // Agrupamento semântico
    struct CatGroup { QString title; QString icon; int count = 0; };
    QMap<QString, CatGroup> groups;
    QStringList order = {"update", "repos", "sys", "gpu", "media", "gaming", "web", "virt", "flatpak", "kernel", "misc"};

    for (const InstallStep &s : steps) {
        QString id = s.id;
        if (id == "update_sys") {
            groups["update"].title = tr("Atualização de Base"); groups["update"].icon = "system-software-update"; groups["update"].count++;
        } else if (id.startsWith("rpmfusion") || id.contains("flathub") || id.contains("repo")) {
            groups["repos"].title = tr("Repositórios de Software"); groups["repos"].icon = "emblem-system"; groups["repos"].count++;
        } else if (id.startsWith("sys_") || id == "fastestmirror" || id == "max_parallel" || id.contains("autostart")) {
            groups["sys"].title = tr("Ferramentas de Sistema"); groups["sys"].icon = "preferences-system"; groups["sys"].count++;
        } else if (id.startsWith("gpu_") || id.startsWith("nvidia") || id.startsWith("amd")) {
            groups["gpu"].title = tr("Aceleração Gráfica"); groups["gpu"].icon = "video-display"; groups["gpu"].count++;
        } else if (id.startsWith("gaming_") || id == "steam" || id == "wine" || id.startsWith("lutris")) {
            groups["gaming"].title = tr("Ecossistema Gaming"); groups["gaming"].icon = "input-gaming"; groups["gaming"].count++;
        } else if (id.startsWith("virt_") || id.startsWith("libvirt")) {
            groups["virt"].title = tr("Sub-virtualização"); groups["virt"].icon = "computer"; groups["virt"].count++;
        } else if (id.startsWith("chrome") || id.startsWith("brave") || id.startsWith("firefox") || id.startsWith("vivaldi") || id.startsWith("librewolf")) {
            groups["web"].title = tr("Navegadores & Web"); groups["web"].icon = "web-browser"; groups["web"].count++;
        } else if (id.startsWith("flatpak_")) {
            groups["flatpak"].title = tr("Framework e Flatpaks"); groups["flatpak"].icon = "package-x-generic"; groups["flatpak"].count++;
        } else if (id.startsWith("cachyos") || id == "scx_scheds") {
            groups["kernel"].title = tr("Kernel & Escaladores"); groups["kernel"].icon = "applications-engineering"; groups["kernel"].count++;
        } else if (id.startsWith("media_") || id.contains("codec")) {
            groups["media"].title = tr("Codecs Nativos"); groups["media"].icon = "applications-multimedia"; groups["media"].count++;
        } else {
            groups["misc"].title = tr("Configurações Complementares"); groups["misc"].icon = "applications-utilities"; groups["misc"].count++;
        }
    }

    int stepNum = 1;
    for (const QString &k : order) {
        if (!groups.contains(k)) continue;
        
        auto &g = groups[k];
        
        auto *card = new QFrame;
        card->setStyleSheet("QFrame { background-color: rgba(255,255,255,0.03); border: 1px solid rgba(255,255,255,0.06); border-radius: 10px; } QFrame:hover { background-color: rgba(255,255,255,0.06); border-color: rgba(255,255,255,0.1); }");
        auto *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(15, 10, 15, 10);
        cardLayout->setSpacing(15);

        // Círculo Númerico
        auto *numLbl = new QLabel(QString::number(stepNum++));
        numLbl->setAlignment(Qt::AlignCenter);
        numLbl->setStyleSheet("background-color: #444444; color: white; font-weight: bold; border-radius: 12px; font-size: 13px;");
        numLbl->setFixedSize(24, 24);

        // Ícone Categoria
        auto *iconLbl = new QLabel;
        QIcon devIcon = QIcon::fromTheme(g.icon, QIcon::fromTheme("applications-other"));
        iconLbl->setPixmap(devIcon.pixmap(28, 28));
        iconLbl->setStyleSheet("background: transparent; border: none;");

        // Elementos de Texto
        auto *textLayout = new QVBoxLayout;
        auto *catLbl = new QLabel(QString("<b>%1</b>").arg(g.title));
        catLbl->setStyleSheet("font-size: 14px; color: #ffffff; background: transparent; border: none;");
        
        auto *descLbl = new QLabel(tr("Procedendo e consolidando %1 pacote(s) ou comandos da categoria.").arg(g.count));
        descLbl->setStyleSheet("font-size: 12px; color: #aaaaaa; background: transparent; border: none;");
        
        textLayout->addWidget(catLbl);
        textLayout->addWidget(descLbl);

        cardLayout->addWidget(numLbl);
        cardLayout->addWidget(iconLbl);
        cardLayout->addLayout(textLayout, 1);

        m_timelineLayout->addWidget(card);
    }

    // Oculto EmptyState mas insere de backup
    m_timelineLayout->addWidget(m_emptyStateWidget);
    m_timelineLayout->addStretch();
}

void ReviewPage::onProceedAnyway()
{
    m_proceedForced = true;
    m_proceedBtn->setVisible(false);
    m_diskLabel->setText(m_diskLabel->text() +
        tr("<br><br><font color='#ff5555'><b>Ignorando Auditoria: Prosseguindo instalação de forma insegura.</b></font>"));
    // Modifica a barra para refletir o erro manual
    m_diskBar->setStyleSheet("QProgressBar { background-color: rgba(255, 255, 255, 0.1); border-radius: 4px;text-align: center; color: white; font-weight: bold; } QProgressBar::chunk { background-color: #880000; border-radius: 4px; }");

    emit completeChanged();
    
    // Enable Next button forcefully
    if (auto *nextBtn = wizard()->button(QWizard::NextButton)) {
        nextBtn->setEnabled(true);
    }
}

bool ReviewPage::isComplete() const
{
    return m_diskOk || m_proceedForced;
}
