#include <QPalette>
#include "gamingpage.h"
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
#include <QGridLayout>

// ==========================================================
// Seção Recolhível (Collapsible Section)
// ==========================================================
CollapsibleSection::CollapsibleSection(const QString &title, QWidget *parent) : QWidget(parent) {
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 10);
    mainLayout->setSpacing(5);
    
    auto *headerBtn = new QPushButton("▼  " + title);
    headerBtn->setCursor(Qt::PointingHandCursor);
    headerBtn->setStyleSheet(
        "QPushButton { text-align: left; padding: 12px; font-size: 15px; font-weight: bold; "
        "background-color: rgba(255, 255, 255, 0.08); border: none; border-radius: 8px; color: #ffffff; }"
        "QPushButton:hover { background-color: rgba(255, 255, 255, 0.15); }"
    );
    
    m_contentContainer = new QFrame;
    m_contentLayout = new QGridLayout(m_contentContainer);
    m_contentLayout->setContentsMargins(10, 10, 10, 10);
    m_contentLayout->setSpacing(15);
    
    mainLayout->addWidget(headerBtn);
    mainLayout->addWidget(m_contentContainer);
    
    connect(headerBtn, &QPushButton::clicked, this, [=]() {
        setExpanded(!m_isExpanded);
        headerBtn->setText((m_isExpanded ? "▼  " : "▶  ") + title);
    });
}

void CollapsibleSection::setExpanded(bool expand) {
    m_isExpanded = expand;
    m_contentContainer->setVisible(expand);
}

bool CollapsibleSection::matchesFilter(const QString &filter) {
    bool hasMatch = false;
    for (int i = 0; i < m_contentLayout->count(); ++i) {
        QWidget *widget = m_contentLayout->itemAt(i)->widget();
        if (widget) {
            QString cardText = widget->property("searchText").toString().toLower();
            if (filter.isEmpty() || cardText.contains(filter)) {
                widget->setVisible(true);
                hasMatch = true;
            } else {
                widget->setVisible(false);
            }
        }
    }
    // Expand open dynamically if a search query is typed and matched
    if (!filter.isEmpty() && hasMatch && !m_isExpanded) {
        setExpanded(true);
    }
    return hasMatch || filter.isEmpty();
}


// ==========================================================
// Página Principal de Gaming
// ==========================================================
GamingPage::GamingPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Ecossistema Gaming"));
    setSubTitle(tr("Plataformas, camadas de compatibilidade Windows, overlays de desempenho e suporte a controles."));
}

void GamingPage::addToolCard(CollapsibleSection *section, QString key, QString name, QString desc, bool installed, const QString &iconName, int row, int col)
{
    auto *card = new QFrame;
    card->setObjectName("ToolCard_" + key);
    card->setProperty("searchText", name + " " + desc); // Utilizado pelo QSearchPath
    card->setStyleSheet(
        "QFrame[objectName^=\"ToolCard_\"] { "
        "  background-color: #2c2c2c;"
        "  border: 1px solid rgba(255, 255, 255, 0.08);"
        "  border-radius: 12px;"
        "}"
        "QFrame[objectName^=\"ToolCard_\"]:hover { "
        "  background-color: #383838;"
        "  border: 1px solid rgba(255, 255, 255, 0.2);"
        "}"
    );
    
    // Altura mínima para o grid se ajustar de forma harmoniosa
    card->setMinimumHeight(80);

    auto *cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(12, 12, 12, 12);
    cardLayout->setSpacing(12);
    
    auto *checkbox = new QCheckBox;
    checkbox->setCursor(Qt::PointingHandCursor);
    checkbox->setStyleSheet("QCheckBox::indicator { width: 22px; height: 22px; }"); 
    checkbox->setChecked(installed);
    m_boxes[key] = checkbox;

    auto *iconLabel = new QLabel;
    QIcon devIcon = QIcon::fromTheme(iconName);
    if(devIcon.isNull()) devIcon = QIcon::fromTheme("applications-games");
    iconLabel->setPixmap(devIcon.pixmap(36, 36));
    
    auto *textLayout = new QVBoxLayout;
    textLayout->setSpacing(2);
    
    auto *titleLabel = new QLabel(QString("<b>%1</b>").arg(name));
    titleLabel->setStyleSheet("font-size: 14px; color: #ffffff;");
    
    auto *descLabel = new QLabel(desc);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #a0a0a0; font-size: 12px; line-height: 1.2;");
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(descLabel);
    textLayout->addStretch();
    
    auto *badgeLabel = new QLabel(tr("[Checando...]"));
    badgeLabel->setObjectName("badge_" + key);
    // Pills style nativa e centralizada a direita
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
    
    m_cards[key] = card;
    section->contentLayout()->addWidget(card, row, col);
}

void GamingPage::filterCards(const QString &text) {
    QString filter = text.toLower();
    for (auto *section : m_sections) {
        bool sectionHasMatch = section->matchesFilter(filter);
        section->setVisible(sectionHasMatch);
    }
}

void GamingPage::initializePage()
{
    if (layout()) {
        QLayoutItem *i; while ((i = layout()->takeAt(0))) { if (i->widget()) i->widget()->deleteLater(); delete i; }
        delete layout();
    }
    m_boxes.clear();
    m_badges.clear();
    m_cards.clear();
    m_sections.clear();

    auto *outer = new QVBoxLayout(this);
    outer->setSpacing(15);
    outer->setContentsMargins(10, 10, 10, 10);

    // ==========================================
    // Top Bar (Buttons and Search)
    // ==========================================
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

    // Barra de Pesquisa Vibecoding
    m_searchBar = new QLineEdit;
    m_searchBar->setPlaceholderText(tr("🔍 Buscar pacotes (ex: Lutris, Proton)..."));
    m_searchBar->setStyleSheet("QLineEdit { background-color: rgba(0,0,0,0.3); border: 1px solid #444; border-radius: 6px; padding: 6px; color: #fff; font-size: 13px; }");
    m_searchBar->setMinimumWidth(250);
    connect(m_searchBar, &QLineEdit::textChanged, this, &GamingPage::filterCards);

    connect(allBtn,  &QPushButton::clicked, this, [this]{ for (auto *cb : m_boxes) cb->setChecked(true); });
    connect(noneBtn, &QPushButton::clicked, this, [this]{ for (auto *cb : m_boxes) cb->setChecked(false); });
    connect(refreshBtn, &QPushButton::clicked, this, [this] { initializePage(); });

    toolbar->addWidget(m_searchBar);
    toolbar->addStretch();
    toolbar->addWidget(m_checkingLabel);
    toolbar->addSpacing(15);
    toolbar->addWidget(refreshBtn);
    toolbar->addWidget(allBtn);
    toolbar->addWidget(noneBtn);
    outer->addWidget(toolbarWidget);

    // ==========================================
    // Core Layout
    // ==========================================
    auto *scroll = new SmoothScrollArea; 
    scroll->setWidgetResizable(true); 
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(
        "QScrollArea { background-color: transparent; } "
        "QWidget#CardsContainer { background-color: transparent; } "
        "QScrollBar:vertical { width: 6px; background: transparent; } "
        "QScrollBar::handle:vertical { background: #555555; border-radius: 3px; min-height: 20px; } "
        "QScrollBar::handle:vertical:hover { background: #888888; } "
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
    );
    
    auto *inner = new QWidget; 
    inner->setObjectName("CardsContainer");
    auto *innerLayout = new QVBoxLayout(inner); 
    innerLayout->setSpacing(10);
    innerLayout->setContentsMargins(0, 0, 10, 0);

    // Seção 1: Controller Support
    auto *secControllers = new CollapsibleSection(tr("Controller Support"));
    m_sections.append(secControllers);
    innerLayout->addWidget(secControllers);
    addToolCard(secControllers, "kernel_modules_extra", tr("Kernel Modules Extra"), tr("Recomendado caso o seu controle (ex: Xbox, DualShock) não seja detectado nativamente. Provém drivers faltantes ao Kernel base."), false, "input-gaming", 0, 0);

    // Seção 2: Game Launchers
    auto *secLaunchers = new CollapsibleSection(tr("Game Launchers"));
    m_sections.append(secLaunchers);
    innerLayout->addWidget(secLaunchers);
    addToolCard(secLaunchers, "steam", tr("Steam"), tr("Maior loja digital do mundo, nativa com a maravilhosa integração do Valve Proton embutida para abrir games de Windows."), false, "steam", 0, 0);
    addToolCard(secLaunchers, "lutris", tr("Lutris"), tr("Lançador multifacetado de código aberto perfeito para jogos da GOG, EA, Amazon, emuladores variados e binários offline."), false, "lutris", 0, 1);
    addToolCard(secLaunchers, "heroic", tr("Heroic Games Launcher (Flatpak)"), tr("Cliente open source focado exclusivamente no Epic Games, GOG e Amazon Prime de forma hiper otimizada."), false, "games-app", 1, 0);

    // Seção 3: Windows Compatibility
    auto *secWin = new CollapsibleSection(tr("Windows Compatibility"));
    m_sections.append(secWin);
    innerLayout->addWidget(secWin);
    addToolCard(secWin, "wine", tr("Wine (Multilib)"), tr("Camada de tradução de Windows vital para o linux. O Rapidora compilará Wine x64 e x32 automaticamente simultaneamente."), false, "wine", 0, 0);
    addToolCard(secWin, "protontricks", tr("Protontricks"), tr("Script automatizado que injeta vcrun, directx ou media foundation dentro de garrafas de jogos específicos da Steam local."), false, "utilities-terminal", 0, 1);

    // Seção 4: Proton & Compatibility Tools
    auto *secProton = new CollapsibleSection(tr("Proton & Compatibility Tools"));
    m_sections.append(secProton);
    innerLayout->addWidget(secProton);
    addToolCard(secProton, "protonup", tr("ProtonUp-Qt (Flatpak)"), tr("Utilitário GUI essencial que instala a última versão experimental do Proton-GE, Luxtorpeda e Wine-GE com poucos cliques."), false, "package-x-generic", 0, 0);
    addToolCard(secProton, "protonplus", tr("ProtonPlus (Flatpak)"), tr("Alternativa fluída do ProtonUp escrita puramente em Vala e GTK4, muito polida para quem prefere designs orgânicos."), false, "package-x-generic", 0, 1);

    // Seção 5: Performance & Monitoring
    auto *secPerf = new CollapsibleSection(tr("Performance & Monitoring"));
    m_sections.append(secPerf);
    innerLayout->addWidget(secPerf);
    addToolCard(secPerf, "mangohud", tr("MangoHud"), tr("O MSI Afterburner do Linux. Permite travar quadros e injetar métricas completas e belíssimas de GPU/CPU nos seus jogos."), false, "utilities-system-monitor", 0, 0);
    addToolCard(secPerf, "goverlay", tr("GOverlay"), tr("A interface gráfica necessária para configurar a fonte, cor e posicionamento do MangoHud sem precisar encostar em arquivos de texto."), false, "preferences-system", 0, 1);

    innerLayout->addStretch();
    scroll->setWidget(inner);
    outer->addWidget(scroll);

    // ==========================================
    // Back-end Checkers Automáticos
    // ==========================================
    QList<QPair<QString, std::function<bool()>>> _checks;
    
    // Checagens RPM normais
    _checks.append({"steam", []{ return isDnfInstalled("steam"); }});
    _checks.append({"lutris", []{ return isDnfInstalled("lutris"); }});
    _checks.append({"wine", []{ return isDnfInstalled("wine"); }});
    _checks.append({"protontricks", []{ return isDnfInstalled("protontricks"); }});
    _checks.append({"mangohud", []{ return isDnfInstalled("mangohud"); }});
    _checks.append({"goverlay", []{ return isDnfInstalled("goverlay"); }});

    // Checagens Flatpak
    _checks.append({"heroic", []{ return isFlatpakInstalled("com.heroicgameslauncher.hgl"); }});
    _checks.append({"protonup", []{ return isFlatpakInstalled("net.davidotek.pupgui2"); }});
    _checks.append({"protonplus", []{ return isFlatpakInstalled("com.vysp3r.ProtonPlus"); }});
    
    // Checagem Híbrida do Kernel Módulo! (RPM + Modinfo)
    _checks.append({"kernel_modules_extra", []{ 
        return isDnfInstalled("kernel-modules-extra") || (system("modinfo xpad >/dev/null 2>&1") == 0); 
    }});
    
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

bool GamingPage::validatePage()
{
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it)
        m_wiz->setOpt(QString("gaming/%1").arg(it.key()), it.value()->isChecked());
    return true;
}
