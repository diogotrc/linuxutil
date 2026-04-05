#include <QPalette>
#include "browserspage.h"
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
#include <QGraphicsDropShadowEffect>

BrowsersPage::BrowsersPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Navegadores"));
    setSubTitle(tr("Portais principais da Web. Navegadores de terceiros terão seus repositórios isolados adicionados automaticamente via dnf."));
}

void BrowsersPage::addToolCard(QVBoxLayout *layout, QString key, QString name, QString desc, bool installed, const QString &iconName)
{
    auto *card = new QFrame;
    card->setObjectName("ToolCard_" + key);
    // Baseline Style
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
    cardLayout->setContentsMargins(12, 12, 12, 12);
    cardLayout->setSpacing(15);
    
    auto *checkbox = new QCheckBox;
    checkbox->setCursor(Qt::PointingHandCursor);
    checkbox->setStyleSheet("QCheckBox::indicator { width: 22px; height: 22px; }"); 
    checkbox->setChecked(installed);
    m_boxes[key] = checkbox;

    auto *iconLabel = new QLabel;
    // Try progressively more generic icon names as fallbacks
    QIcon devIcon = QIcon::fromTheme(iconName);
    if (devIcon.isNull()) {
        const QStringList altNames = {"internet-web-browser", "web-browser", "browser",
                                      "applications-internet", "text-html"};
        for (const auto &alt : altNames) {
            devIcon = QIcon::fromTheme(alt);
            if (!devIcon.isNull()) break;
        }
    }
    iconLabel->setFixedSize(42, 42);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(devIcon.pixmap(38, 38));
    
    auto *textLayout = new QVBoxLayout;
    textLayout->setSpacing(2);
    
    auto *titleLabel = new QLabel(QString("<b>%1</b>").arg(name));
    titleLabel->setStyleSheet("font-size: 15px; color: #ffffff;");
    
    auto *descLabel = new QLabel(desc);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #a0a0a0; font-size: 12px; line-height: 1.2;");
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(descLabel);
    textLayout->addStretch();
    
    auto *badgeLabel = new QLabel(tr("[Checando...]"));
    badgeLabel->setObjectName("badge_" + key);
    badgeLabel->setFixedWidth(130);
    badgeLabel->setStyleSheet("color: #777777; font-weight: bold; background: transparent; padding: 4px 6px; border-radius: 10px;");
    badgeLabel->setAlignment(Qt::AlignCenter);
    badgeLabel->setWordWrap(false);
    badgeLabel->setProperty("isInstalled", false);
    m_badges[key] = badgeLabel;
    
    connect(checkbox, &QCheckBox::toggled, this, [badgeLabel, card](bool checked) {
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

        // ==========================================
        // Dica Vibecoding: Efeito Glow Tátil no Card Selecionado!
        // ==========================================
        if (checked) {
            auto *glow = new QGraphicsDropShadowEffect(card);
            glow->setBlurRadius(25);
            glow->setColor(QColor(53, 132, 228, 160)); // Azul Neon Radiante
            glow->setOffset(0, 0);
            card->setGraphicsEffect(glow);
            card->setStyleSheet(
                "QFrame[objectName^=\"ToolCard_\"] { "
                "  background-color: rgba(53, 132, 228, 0.12);"
                "  border: 1px solid #3584e4;"
                "  border-radius: 12px;"
                "}"
            );
        } else {
            card->setGraphicsEffect(nullptr); // Remove neon
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
        }
    });

    cardLayout->addWidget(checkbox);
    cardLayout->addWidget(iconLabel);
    cardLayout->addLayout(textLayout, 1);
    cardLayout->addWidget(badgeLabel);
    
    layout->addWidget(card);
}

void BrowsersPage::initializePage()
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

    // Botões Topo
    auto *toolbarWidget = new QWidget;
    auto *toolbar = new QHBoxLayout(toolbarWidget);
    toolbar->setContentsMargins(0,0,0,0);
    
    m_checkingLabel = new QLabel(tr("⏳ Rastreador em andamento..."));
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
    auto *refreshBtn = new QPushButton(tr("↻ Atualizar"));

    allBtn->setStyleSheet(btnStyle); allBtn->setCursor(Qt::PointingHandCursor);
    noneBtn->setStyleSheet(btnStyle); noneBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setStyleSheet(btnStyle); refreshBtn->setCursor(Qt::PointingHandCursor);

    connect(allBtn,  &QPushButton::clicked, this, [this]{ for (auto *cb : m_boxes) cb->setChecked(true); });
    connect(noneBtn, &QPushButton::clicked, this, [this]{ for (auto *cb : m_boxes) cb->setChecked(false); });
    connect(refreshBtn, &QPushButton::clicked, this, [this] { initializePage(); });

    toolbar->addStretch();
    toolbar->addWidget(m_checkingLabel);
    toolbar->addSpacing(15);
    toolbar->addWidget(refreshBtn);
    toolbar->addWidget(allBtn);
    toolbar->addWidget(noneBtn);
    outer->addWidget(toolbarWidget);

    auto *scroll = new SmoothScrollArea; 
    scroll->setWidgetResizable(true); 
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background-color: transparent; } QWidget#CardsContainer { background-color: transparent; }");
    
    auto *inner = new QWidget; 
    inner->setObjectName("CardsContainer");
    auto *innerLayout = new QVBoxLayout(inner); 
    innerLayout->setSpacing(10);
    innerLayout->setContentsMargins(0, 0, 10, 0);

    auto addSectionLabel = [&](const QString &title) {
        if(innerLayout->count() > 0) innerLayout->addSpacing(8);
        auto *lbl = new QLabel(QString("<b>%1</b>").arg(title));
        lbl->setStyleSheet("font-size: 15px; color: #4cd14c; margin-bottom: 2px;");
        innerLayout->addWidget(lbl);
    };

    // ==========================================
    // Renderização Dinâmica - Categorias
    // ==========================================
    
    addSectionLabel(tr("From Fedora Repos"));
    addToolCard(innerLayout, "firefox", tr("Mozilla Firefox"), tr("Navegador moderno de código aberto guiado à liberdade de dados. Ele já acompanha o Fedora Core por natureza."), false, "firefox");
    addToolCard(innerLayout, "chromium", tr("Chromium Base"), tr("Versão open source despida sem as telemetrias e bloqueios da Google. Nenhuma conta atrelada."), false, "chromium");

    addSectionLabel(tr("Third-Party (Repo added automatically)"));
    addToolCard(innerLayout, "chrome", tr("Google Chrome"), tr("Ecossistema pleno da Google. O Rapidora cuidará de injetar todas as chaves PGP seguras e criar o dnf.repo dinâmico no disco."), false, "google-chrome");
    addToolCard(innerLayout, "brave", tr("Brave Browser"), tr("Chrome com anabolizantes. Escudos fortes, bloqueio absoluto de anúncios, telemetrias e rastreadores desde o primeiro bip."), false, "brave-browser");
    addToolCard(innerLayout, "vivaldi", tr("Vivaldi"), tr("Projetado para Power Users. Inúmeras funções densas de customização lateral e divisões de janelas baseadas em Chromium."), false, "vivaldi");

    addSectionLabel(tr("Privacy-focused (Flatpak from Flathub)"));
    addToolCard(innerLayout, "librewolf", tr("LibreWolf (Flatpak)"), tr("Um grande fork do Firefox hiper-focado em privacidade absoluta em tempo real. Empacotado sob segurança sandbox do Flathub."), false, "librewolf");

    innerLayout->addStretch();
    scroll->setWidget(inner);
    outer->addWidget(scroll);

    // ==========================================
    // Verificação Base
    // ==========================================
    QList<QPair<QString, std::function<bool()>>> _checks;
    _checks.append({"firefox", []{ return isDnfInstalled("firefox"); }});
    _checks.append({"chromium", []{ return isDnfInstalled("chromium"); }});
    _checks.append({"chrome", []{ return isDnfInstalled("google-chrome-stable"); }});
    _checks.append({"brave", []{ return isDnfInstalled("brave-browser"); }});
    _checks.append({"vivaldi",   []{ return isDnfInstalled("vivaldi-stable"); }});
    _checks.append({"librewolf", []{ return isFlatpakInstalled("io.gitlab.librewolf-community"); }});
    
    runChecksAsync(this, _checks, [this](QMap<QString,bool> results) {
        for (auto it = results.constBegin(); it != results.constEnd(); ++it) {
            if (!m_boxes.contains(it.key())) continue;
            auto *cb = m_boxes[it.key()];
            auto *badge = m_badges[it.key()];
            
            badge->setProperty("isInstalled", it.value());

            if (it.value()) {
                badge->setText(tr("[INSTALADO]"));
                badge->setStyleSheet("background-color: rgba(43, 122, 66, 0.85); color: #ffffff; padding: 4px 12px; border-radius: 10px; font-weight: bold; font-size: 11px;");
            } else {
                badge->setText(tr("[NÃO INSTALADO]"));
                badge->setStyleSheet("background-color: rgba(186, 102, 0, 0.85); color: #ffffff; padding: 4px 12px; border-radius: 10px; font-weight: bold; font-size: 11px;");
            }

            // Ativa o toggle effect trigger
            emit cb->toggled(cb->isChecked());
        }
        if (m_checkingLabel) m_checkingLabel->setVisible(false);
    });
}

bool BrowsersPage::validatePage()
{
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it)
        m_wiz->setOpt(QString("browsers/%1").arg(it.key()), it.value()->isChecked());
    return true;
}
