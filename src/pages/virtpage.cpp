#include <QPalette>
#include "virtpage.h"
#include "../mainwizard.h"
#include "../pagehelpers.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>
#include <QApplication>
#include <QIcon>
#include <QVariant>
#include <QProcess>

VirtPage::VirtPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Virtualização de Sistemas"));
    setSubTitle(tr("Ferramentas nativas do kernel KVM/QEMU para execução de Máquinas Virtuais no nível do hardware."));
}

bool VirtPage::checkHardwareVirtualization() {
    QProcess p;
    p.start("bash", {"-c", "grep -E -c '(vmx|svm)' /proc/cpuinfo"});
    p.waitForFinished(2000);
    QString out = QString::fromUtf8(p.readAllStandardOutput()).trimmed();
    return out.toInt() > 0;
}

void VirtPage::addToolCard(QString key, QString name, QString desc, bool installed, const QString &iconName)
{
    auto *card = new QFrame;
    card->setObjectName("ToolCard_" + key);
    card->setStyleSheet(
        "QFrame[objectName^=\"ToolCard_\"] { "
        "  background-color: rgba(255, 255, 255, 0.03);"
        "  border: 1px solid rgba(255, 255, 255, 0.05);"
        "  border-radius: 12px;"
        "}"
        "QFrame[objectName^=\"ToolCard_\"]:hover { "
        "  background-color: rgba(255, 255, 255, 0.07);"
        "  border: 1px solid rgba(255, 255, 255, 0.15);"
        "}"
    );

    auto *cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(10, 10, 10, 10);
    cardLayout->setSpacing(12);
    
    auto *checkbox = new QCheckBox;
    checkbox->setCursor(Qt::PointingHandCursor);
    checkbox->setStyleSheet("QCheckBox::indicator { width: 22px; height: 22px; }"); 
    checkbox->setChecked(installed);
    m_boxes[key] = checkbox;

    auto *iconLabel = new QLabel;
    QIcon devIcon = QIcon::fromTheme(iconName);
    if(devIcon.isNull()) devIcon = QIcon::fromTheme("computer");
    iconLabel->setPixmap(devIcon.pixmap(32, 32));
    
    auto *textLayout = new QVBoxLayout;
    textLayout->setSpacing(2);
    
    auto *titleLabel = new QLabel(QString("<b>%1</b>").arg(name));
    titleLabel->setStyleSheet("font-size: 14px; color: #ffffff;");
    
    auto *descLabel = new QLabel(desc);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #999999; font-size: 12px; line-height: 1.2;");
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(descLabel);
    
    auto *badgeLabel = new QLabel(tr("[Checando...]"));
    badgeLabel->setObjectName("badge_" + key);
    badgeLabel->setFixedWidth(130);
    badgeLabel->setStyleSheet("color: #777777; font-weight: bold; background: transparent; padding: 4px 6px; border-radius: 10px;");
    badgeLabel->setAlignment(Qt::AlignCenter);
    badgeLabel->setWordWrap(false);
    badgeLabel->setProperty("isInstalled", false);
    m_badges[key] = badgeLabel;
    
    connect(checkbox, &QCheckBox::toggled, this, [badgeLabel](bool checked) {
        bool actuallyInstalled = badgeLabel->property("isInstalled").toBool();
        if (actuallyInstalled) {
            if (checked) {
                badgeLabel->setText(tr("[REINSTALAR]"));
                badgeLabel->setStyleSheet("background-color: rgba(53, 132, 228, 0.85); color: #ffffff; padding: 4px 12px; border-radius: 10px; font-weight: bold; font-size: 11px;");
            } else {
                badgeLabel->setText(tr("[INSTALADO]"));
                badgeLabel->setStyleSheet("background-color: rgba(43, 122, 66, 0.85); color: #ffffff; padding: 4px 12px; border-radius: 10px; font-weight: bold; font-size: 11px;");
            }
        }
    });

    cardLayout->addWidget(checkbox);
    cardLayout->addWidget(iconLabel);
    cardLayout->addLayout(textLayout, 1);
    cardLayout->addWidget(badgeLabel);
    
    m_cardsLayout->addWidget(card);
}

void VirtPage::initializePage()
{
    if (layout()) {
        QLayoutItem *i; while ((i = layout()->takeAt(0))) { if (i->widget()) i->widget()->deleteLater(); delete i; }
        delete layout();
    }
    m_boxes.clear();
    m_badges.clear();

    auto *outer = new QVBoxLayout(this);
    outer->setSpacing(15);
    outer->setContentsMargins(10, 10, 10, 10);

    // Botões Superiores Globais
    auto *toolbarWidget = new QWidget;
    auto *toolbar = new QHBoxLayout(toolbarWidget);
    toolbar->setContentsMargins(0,0,0,0);
    
    m_checkingLabel = new QLabel(tr("⏳ Varrendo pacotes KVM Locais..."));
    m_checkingLabel->setStyleSheet("color: #3584e4; font-weight: bold;");
    m_checkingLabel->setVisible(true);

    QString btnStyle = 
        "QPushButton { "
        "  background-color: rgba(255,255,255,0.05); color: #dddddd; "
        "  border: 1px solid rgba(255,255,255,0.1); border-radius: 6px; "
        "  padding: 6px 12px; font-weight: bold;"
        "} "
        "QPushButton:hover { background-color: rgba(255,255,255,0.1); color: #ffffff; border-color: rgba(255,255,255,0.2); }";

    auto *allBtn = new QPushButton(tr("☑ Selecionar Todos"));
    auto *noneBtn = new QPushButton(tr("☐ Nenhum"));

    allBtn->setStyleSheet(btnStyle); allBtn->setCursor(Qt::PointingHandCursor);
    noneBtn->setStyleSheet(btnStyle); noneBtn->setCursor(Qt::PointingHandCursor);

    connect(allBtn,  &QPushButton::clicked, this, [this]{ for (auto *cb : m_boxes) cb->setChecked(true); });
    connect(noneBtn, &QPushButton::clicked, this, [this]{ for (auto *cb : m_boxes) cb->setChecked(false); });

    toolbar->addStretch();
    toolbar->addWidget(m_checkingLabel);
    toolbar->addSpacing(15);
    toolbar->addWidget(allBtn);
    toolbar->addWidget(noneBtn);
    outer->addWidget(toolbarWidget);

    auto *scroll = new SmoothScrollArea; 
    scroll->setWidgetResizable(true); 
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background-color: transparent; } QWidget#CardsContainer { background-color: transparent; }");
    
    auto *inner = new QWidget; 
    inner->setObjectName("CardsContainer");
    m_cardsLayout = new QVBoxLayout(inner); 
    m_cardsLayout->setSpacing(10);
    m_cardsLayout->setContentsMargins(0, 0, 10, 0);

    // Renderiza Cards
    addToolCard("virtmanager", tr("Virt-Manager"), tr("Interface gráfica amigável de desktop para gerenciar redes virtuais e hospedar Máquinas Virtuais em KVM/QEMU."), false, "virt-manager");
    addToolCard("libvirt", tr("Libvirt API Daemon"), tr("Controlador daemon vital que comanda os módulos do hypervisor. Habilitado automaticamente no boot."), false, "application-x-executable");
    addToolCard("virt_install", tr("Virt-Install"), tr("Utilitário avançado de linha de comando usado para programar criação massificada de VMs de forma automatizada."), false, "utilities-terminal");
    addToolCard("virt_viewer", tr("Virt-Viewer"), tr("Visualizador de baixissima latência que injeta SPICE e VNC para exibição robusta remota da tela das máquinas virtuais."), false, "computer");

    // ==========================================
    // Módulo Informativo (Dica Vibecoding: Hardware Intel VT-x/AMD-V)
    // ==========================================
    m_cardsLayout->addSpacing(15);
    
    bool vtSupported = checkHardwareVirtualization();
    
    auto *alertFrame = new QFrame;
    alertFrame->setStyleSheet(vtSupported 
        ? "background-color: rgba(53, 132, 228, 0.1); border: 1px solid rgba(53, 132, 228, 0.3); border-radius: 8px; padding: 12px;"
        : "background-color: rgba(220, 50, 50, 0.1); border: 1px solid rgba(220, 50, 50, 0.4); border-radius: 8px; padding: 12px;"
    );

    auto *alertLayout = new QHBoxLayout(alertFrame);
    alertLayout->setContentsMargins(5, 5, 5, 5);
    alertLayout->setSpacing(15);

    auto *infoIcon = new QLabel("ℹ");
    infoIcon->setStyleSheet(vtSupported ? "font-size: 28px; color: #3584e4; font-weight: bold;" : "font-size: 28px; color: #ff5555; font-weight: bold;");
    
    auto *infoText = new QLabel;
    infoText->setWordWrap(true);
    
    QString user = m_wiz->targetUser();
    QString vtStatus = vtSupported 
        ? tr("<font color='#4cd14c'><b>[OK] Suporte de hardware VT-x/AMD-V habilitado na BIOS.</b></font><br>")
        : tr("<font color='#ff5555'><b>[FALHA] A Virtualização está DESABILITADA na BIOS ou seu CPU não suporta!</b> As Máquinas Virtuais não poderão iniciar até você corrigir isto, mesmo que instale os pacotes.</font><br><br>");

    infoText->setText(
        vtStatus +
        tr("<b>Automação de Grupo Libvirtd:</b><br>"
           "Se virtualizadores forem selecionados, o sistema aplicará de forma automatizada:<br>"
           "&bull; <tt>sudo systemctl enable --now libvirtd</tt><br>"
           "&bull; <tt>O usuário base '%1' será vinculado nativamente ao grupo 'libvirt'.</tt><br><br>"
           "<i>Será indispensável realizar Logout/Reboot da sessão após a conclusão geral do Rapidora. Componentes extras como swtpm para Windows 11 serão inclusos transparentemente.</i>").arg(user)
    );
    infoText->setStyleSheet("color: #b0b0b0; font-size: 13px; line-height: 1.4;");

    alertLayout->addWidget(infoIcon);
    alertLayout->addWidget(infoText, 1);
    
    m_cardsLayout->addWidget(alertFrame);
    
    m_cardsLayout->addStretch();
    scroll->setWidget(inner);
    outer->addWidget(scroll);

    // Validador Paralelo Async
    QList<QPair<QString, std::function<bool()>>> _checks;
    _checks.append({"virtmanager", []{ return isDnfInstalled("virt-manager"); }});
    _checks.append({"libvirt", []{ return isDnfInstalled("libvirt"); }});
    _checks.append({"virt_install", []{ return isDnfInstalled("virt-install"); }});
    _checks.append({"virt_viewer", []{ return isDnfInstalled("virt-viewer"); }});
    
    runChecksAsync(this, _checks, [this](QMap<QString,bool> results) {
        for (auto it = results.constBegin(); it != results.constEnd(); ++it) {
            if (!m_badges.contains(it.key())) continue;
            auto *badge = m_badges[it.key()];
            
            badge->setProperty("isInstalled", it.value());

            if (it.value()) {
                badge->setText(tr("[INSTALADO]"));
                badge->setStyleSheet("background-color: rgba(43, 122, 66, 0.85); color: #ffffff; padding: 4px 12px; border-radius: 10px; font-weight: bold; font-size: 11px;");
            } else {
                badge->setText(tr("[NÃO INSTALADO]"));
                badge->setStyleSheet("background-color: rgba(186, 102, 0, 0.85); color: #ffffff; padding: 4px 12px; border-radius: 10px; font-weight: bold; font-size: 11px;");
            }
        }
        if (m_checkingLabel) m_checkingLabel->setVisible(false);
    });
}

bool VirtPage::validatePage()
{
    // Transforma seleção visual em chaves para mainwizard processar nas suas branches!
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it)
        m_wiz->setOpt(QString("virt/%1").arg(it.key()), it.value()->isChecked());
    return true;
}
