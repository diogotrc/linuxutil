#include "gpupage.h"
#include "../mainwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QApplication>
#include <QProcess>

// =======================================================
// GpuCard Widget implementation
// =======================================================
GpuCard::GpuCard(const QString &id, const QString &title, const QString &desc, const QString &iconName, QWidget *parent)
    : QFrame(parent), m_id(id), m_checked(false)
{
    setFrameStyle(QFrame::StyledPanel);
    setCursor(Qt::PointingHandCursor);
    
    m_vbox = new QVBoxLayout(this);
    m_vbox->setAlignment(Qt::AlignCenter);
    
    auto *iconLabel = new QLabel;
    QIcon icn = QIcon::fromTheme(iconName);
    if (icn.isNull()) icn = QIcon::fromTheme("hardware-video-card", QIcon::fromTheme("video-display"));
    iconLabel->setPixmap(icn.pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignCenter);
    
    auto *titleLabel = new QLabel(QString("<b>%1</b>").arg(title));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 17px; color: #ffffff;");
    
    auto *descLabel = new QLabel(desc);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #b0b0b0; font-size: 13px; line-height: 1.3;");
    
    // Check symbol (hide it initially via transparency)
    m_checkIcon = new QLabel("✔");
    m_checkIcon->setStyleSheet("color: transparent; font-weight: bold; font-size: 20px;");
    m_checkIcon->setAlignment(Qt::AlignRight | Qt::AlignTop);
    
    auto *topLayout = new QHBoxLayout;
    topLayout->addStretch();
    topLayout->addWidget(m_checkIcon);
    
    m_vbox->addLayout(topLayout);
    m_vbox->addWidget(iconLabel);
    m_vbox->addSpacing(10);
    m_vbox->addWidget(titleLabel);
    m_vbox->addSpacing(5);
    m_vbox->addWidget(descLabel);
    m_vbox->addStretch();
    
    updateStyle();
}

void GpuCard::setChecked(bool checked) {
    if (m_checked != checked) {
        m_checked = checked;
        updateStyle();
        if (checked) emit toggled(m_id);
    }
}

void GpuCard::addBadge(const QString &text) {
    auto *badge = new QLabel(text);
    // Badge Dourado e destancante
    badge->setStyleSheet("background-color: #f7a000; color: #000000; padding: 4px 10px; border-radius: 8px; font-weight: bold; font-size: 11px;");
    badge->setAlignment(Qt::AlignCenter);
    // Inserindo antes do ícone para ficar de Título Superior
    m_vbox->insertWidget(0, badge, 0, Qt::AlignCenter);
}

void GpuCard::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QFrame::mouseReleaseEvent(event);
}

void GpuCard::updateStyle() {
    if (m_checked) {
        setStyleSheet(
            "GpuCard { "
            "  background-color: rgba(53, 132, 228, 0.15);"
            "  border: 2px solid #3584e4;"
            "  border-radius: 15px;"
            "}"
        );
        m_checkIcon->setStyleSheet("color: #3584e4; font-weight: bold; font-size: 20px;");
    } else {
        setStyleSheet(
            "GpuCard { "
            "  background-color: rgba(255, 255, 255, 0.05);"
            "  border: 2px solid rgba(255, 255, 255, 0.05);"
            "  border-radius: 15px;"
            "}"
            "GpuCard:hover { "
            "  background-color: rgba(255, 255, 255, 0.08);"
            "  border: 2px solid rgba(255, 255, 255, 0.2);"
            "}"
        );
        m_checkIcon->setStyleSheet("color: transparent; font-weight: bold; font-size: 20px;");
    }
}

// =======================================================
// GpuPage Logic implementation
// =======================================================
GpuPage::GpuPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Aceleração Gráfica e GPU"));
    setSubTitle(tr("Identifique sua placa de vídeo principal. Isso garantirá os drivers corretos e aceleradores no kernel do Linux e RPM Fusion."));
}

void GpuPage::initializePage()
{
    if (layout()) {
        QLayoutItem *i; while ((i = layout()->takeAt(0))) { if (i->widget()) i->widget()->deleteLater(); delete i; }
        delete layout();
    }

    auto *outer = new QVBoxLayout(this);
    outer->setSpacing(20);
    outer->setContentsMargins(10, 10, 10, 10);

    // Dica de UI "Vibecoding" - Feedback Visual Dinâmico
    m_detectedLabel = new QLabel(tr("🔎 Sondando PCI Local em busca de GPUs..."));
    m_detectedLabel->setStyleSheet("color: #3584e4; font-weight: bold; font-size: 14px;");
    m_detectedLabel->setAlignment(Qt::AlignCenter);
    outer->addWidget(m_detectedLabel);
    outer->addSpacing(15);

    // Layout de Cards (Cards grandes)
    auto *cardsLayout = new QHBoxLayout;
    cardsLayout->setSpacing(20);

    // Card AMD
    m_cardAmd = new GpuCard("amd", 
                            "Gráficos AMD", 
                            tr("Suíte Open Source Integrada diretamente e ativamente ao Kernel. Instala extensões RADV, Mesa-DRI e Vulkan em massa."), 
                            "video-display");
    
    // Card NVIDIA
    m_cardNvidia = new GpuCard("nvidia", 
                               "Gráficos NVIDIA", 
                               tr("Driver Proprietário (Código Fechado). Nosso sistema montará o kernel do Akmod-Nvidia utilizando as assinaturas do RPM Fusion NonFree de forma totalmente silenciosa e automática."), 
                               "video-display-3d");

    // Card Skip / Outro
    m_cardSkip = new GpuCard("skip", 
                             tr("Intel / Máquina Virtual"), 
                             tr("Placas gráficas híbridas, APUs estritas Intel ou Máquinas Virtuais. O Mesa nativo padrão do Fedora será unicamente o suficiente."), 
                             "dialog-question");

    cardsLayout->addWidget(m_cardAmd);
    cardsLayout->addWidget(m_cardNvidia);
    cardsLayout->addWidget(m_cardSkip);

    outer->addLayout(cardsLayout);
    outer->addStretch(); // Empurra pra cima de forma elegante

    // Lógica de Mutex (Somente 1 Selecionado por vez)
    auto handleSelection = [this](QString clickedId) {
        selectCard(clickedId);
    };

    connect(m_cardAmd, &GpuCard::clicked, this, [=]() { handleSelection("amd"); });
    connect(m_cardNvidia, &GpuCard::clicked, this, [=]() { handleSelection("nvidia"); });
    connect(m_cardSkip, &GpuCard::clicked, this, [=]() { handleSelection("skip"); });

    // Dispara a Mágica "Vibecoding" 
    autoDetectGPU();
}

void GpuPage::selectCard(const QString &id) {
    m_cardAmd->setChecked(id == "amd");
    m_cardNvidia->setChecked(id == "nvidia");
    m_cardSkip->setChecked(id == "skip");
}

void GpuPage::autoDetectGPU()
{
    QProcess p;
    // O lspci roda cruamente rapidíssimo
    p.start("lspci", QStringList{});
    p.waitForFinished(3000);
    QString out = QString::fromUtf8(p.readAllStandardOutput()).toLower();
    
    QString autoChoice = "skip";
    QString foundGPUName = tr("Nenhuma GPU dedicada detectada (Fallback / Intel).");

    // Procura por VGA ou 3D controller no log 
    if (out.contains("nvidia")) {
        autoChoice = "nvidia";
        m_cardNvidia->addBadge(tr("★ RECOMENDADO MÁQUINA"));
        foundGPUName = tr("Acelerador Gráfico NVIDIA detectado na barramento físico!");
    } else if (out.contains("amd") || out.contains("radeon")) {
        autoChoice = "amd";
        m_cardAmd->addBadge(tr("★ RECOMENDADO MÁQUINA"));
        foundGPUName = tr("Placa de Vídeo / APU AMD detectada na barramento físico!");
    } else if (out.contains("intel")) {
        m_cardSkip->addBadge(tr("★ RECOMENDADO MÁQUINA"));
        foundGPUName = tr("Gráficos Intel Arc/Integrado detectados.");
    }

    m_detectedLabel->setText(QString("🔎 <b>Concluído:</b> %1").arg(foundGPUName));
    m_detectedLabel->setStyleSheet("color: #4cd14c; font-weight: normal; font-size: 13px;");

    // Seleciona a carta dinamicamente
    selectCard(autoChoice);
}

bool GpuPage::validatePage()
{
    QString choice = "none";
    if (m_cardAmd->isChecked())         choice = "amd";
    else if (m_cardNvidia->isChecked()) choice = "nvidia";
    else if (m_cardSkip->isChecked())   choice = "skip";
    
    // Conforme refatorado no arquivo mainwizard.cpp, a engine cuida dos installSteps via esta flag universal invisível!
    m_wiz->setOpt("gpu/choice", choice);
    
    return true;
}
