#include <QPalette>
#include "contentpage.h"
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

ContentPage::ContentPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Criação de Conteúdo"));
    setSubTitle(tr("Aplicativos criativos base para vídeos, áudio, vetores e design tridimensional."));
}

void ContentPage::addToolCard(QString key, QString name, QString desc, bool installed, const QString &iconName)
{
    auto *card = new QFrame;
    card->setObjectName("ToolCard_" + key);
    card->setStyleSheet(
        "QFrame[objectName^=\"ToolCard_\"] { "
        "  background-color: rgba(255, 255, 255, 0.03);"
        "  border: 1px solid rgba(255, 255, 255, 0.05);"
        "  border-radius: 10px;"
        "}"
        "QFrame[objectName^=\"ToolCard_\"]:hover { "
        "  background-color: rgba(255, 255, 255, 0.07);"
        "  border: 1px solid rgba(255, 255, 255, 0.15);"
        "}"
    );

    auto *cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(8, 8, 8, 8);
    cardLayout->setSpacing(12);
    
    auto *checkbox = new QCheckBox;
    checkbox->setCursor(Qt::PointingHandCursor);
    checkbox->setStyleSheet("QCheckBox::indicator { width: 22px; height: 22px; }"); 
    checkbox->setChecked(installed);
    m_boxes[key] = checkbox;

    auto *iconLabel = new QLabel;
    QIcon devIcon = QIcon::fromTheme(iconName);
    if(devIcon.isNull()) devIcon = QIcon::fromTheme("applications-graphics"); // base genérico seguro
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
    badgeLabel->setStyleSheet("color: #777777; font-weight: bold; background: transparent; padding: 4px 6px; border-radius: 6px;");
    badgeLabel->setAlignment(Qt::AlignCenter);
    badgeLabel->setWordWrap(false);
    badgeLabel->setProperty("isInstalled", false);
    m_badges[key] = badgeLabel;
    
    connect(checkbox, &QCheckBox::toggled, this, [badgeLabel](bool checked) {
        bool actuallyInstalled = badgeLabel->property("isInstalled").toBool();
        if (actuallyInstalled) {
            if (checked) {
                badgeLabel->setText(tr("[REINSTALAR]"));
                badgeLabel->setStyleSheet("background-color: rgba(53, 132, 228, 0.85); color: #ffffff; padding: 4px 8px; border-radius: 6px; font-weight: bold; font-size: 11px;");
            } else {
                badgeLabel->setText(tr("[INSTALADO]"));
                badgeLabel->setStyleSheet("background-color: rgba(43, 122, 66, 0.85); color: #ffffff; padding: 4px 8px; border-radius: 6px; font-weight: bold; font-size: 11px;");
            }
        }
    });

    cardLayout->addWidget(checkbox);
    cardLayout->addWidget(iconLabel);
    cardLayout->addLayout(textLayout, 1);
    cardLayout->addWidget(badgeLabel);
    
    m_cardsLayout->addWidget(card);
}

void ContentPage::initializePage()
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

    // Botões Fixos Superiores
    auto *toolbarWidget = new QWidget;
    auto *toolbar = new QHBoxLayout(toolbarWidget);
    toolbar->setContentsMargins(0,0,0,0);
    
    m_checkingLabel = new QLabel(tr("⏳ Interrogando o sistema anfitrião..."));
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
    // Customização discreta de scroll para listas longas
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
    m_cardsLayout = new QVBoxLayout(inner); 
    m_cardsLayout->setSpacing(10);
    m_cardsLayout->setContentsMargins(0, 0, 10, 0);

    // -- LISTA RPM --
    auto *rpmHeader = new QLabel(tr("<b>Pacotes Nativos do Sistema (RPM)</b>"));
    rpmHeader->setStyleSheet("font-size: 15px; color: #4cd14c; margin-top: 5px;");
    m_cardsLayout->addWidget(rpmHeader);

    const QList<std::tuple<QString,QString,QString,QString>> rpmItems = {
        {"obs",      tr("OBS Studio"), tr("Padrão ouro da indústria para gravação de tela, composição de cenas e streaming ao vivo."), "camera-web"},
        {"kdenlive", tr("Kdenlive"),   tr("Poderoso editor de vídeo não-linear mantido livre sob o guarda-chuva do projeto KDE."), "kdenlive"},
        {"gimp",     tr("GIMP"),       tr("O eterno e clássico editor de gráficos raster recheado de recursos para edição de fotos."), "gimp"},
        {"inkscape", tr("Inkscape"),   tr("Plataforma profissional de manipulação de vetores SVG perfeitamente escaláveis."), "inkscape"},
        {"audacity", tr("Audacity"),   tr("Ferramenta de estúdio de mesa multipista de rápido processamento para manipulação de áudio."), "audacity"},
    };

    for (const auto &[key, name, desc, icon] : rpmItems) {
        addToolCard(key, name, desc, false, icon);
    }

    // -- SEÇÃO FLATPAK (Com a Verificação Vibecoding Nativa/Garantida Exibida) --
    m_cardsLayout->addSpacing(15);
    
    auto *flatpakContainer = new QFrame;
    flatpakContainer->setStyleSheet("background-color: rgba(74, 144, 226, 0.1); border-radius: 8px; padding: 6px;");
    auto *flatpakLayout = new QHBoxLayout(flatpakContainer);
    flatpakLayout->setContentsMargins(10, 5, 10, 5);
    
    auto *flatpakIcon = new QLabel;
    QIcon fIcon = QIcon::fromTheme("software-store");
    if(fIcon.isNull()) fIcon = QIcon::fromTheme("package-x-generic");
    flatpakIcon->setPixmap(fIcon.pixmap(24, 24));
    
    auto *flatpakText = new QLabel(tr("<i>Os seguintes softwares serão empacotados pela plataforma <b>Flatpak via Flathub</b> em caixas de areia blindadas. Nosso instalador conectará e criará as rotas de Flathub automaticamente por você caso não estejam configuradas no sistema.</i>"));
    flatpakText->setWordWrap(true);
    flatpakText->setStyleSheet("color: #92b8ea; font-size: 12px;");
    
    flatpakLayout->addWidget(flatpakIcon);
    flatpakLayout->addWidget(flatpakText, 1);
    
    m_cardsLayout->addWidget(flatpakContainer);

    addToolCard("blender", tr("Blender 3D (Flatpak)"), 
                tr("Suíte líder de mercado, de código aberto para modelagem 3D, animação, rig de texturas e até simulação real-time."), 
                false, "blender");

    m_cardsLayout->addStretch();
    scroll->setWidget(inner);
    outer->addWidget(scroll);

    // Motor de Validação via Threads Seguras
    QList<QPair<QString, std::function<bool()>>> _checks;
    
    _checks.append({"obs", []{ return isDnfInstalled("obs-studio"); }});
    _checks.append({"kdenlive", []{ return isDnfInstalled("kdenlive"); }});
    _checks.append({"gimp", []{ return isDnfInstalled("gimp"); }});
    _checks.append({"inkscape", []{ return isDnfInstalled("inkscape"); }});
    _checks.append({"audacity", []{ return isDnfInstalled("audacity"); }});
    
    // Checagem diferenciada p/ o flatpak
    _checks.append({"blender", []{ return isFlatpakInstalled("org.blender.Blender"); }});
    
    runChecksAsync(this, _checks, [this](QMap<QString,bool> results) {
        for (auto it = results.constBegin(); it != results.constEnd(); ++it) {
            if (!m_badges.contains(it.key())) continue;
            auto *badge = m_badges[it.key()];
            
            badge->setProperty("isInstalled", it.value());

            if (it.value()) {
                badge->setText(tr("[INSTALADO]"));
                badge->setStyleSheet("background-color: rgba(43, 122, 66, 0.85); color: #ffffff; padding: 4px 8px; border-radius: 6px; font-weight: bold; font-size: 11px;");
            } else {
                badge->setText(tr("[NÃO INSTALADO]"));
                badge->setStyleSheet("background-color: rgba(186, 102, 0, 0.85); color: #ffffff; padding: 4px 8px; border-radius: 6px; font-weight: bold; font-size: 11px;");
            }
        }
        if (m_checkingLabel) m_checkingLabel->setVisible(false);
    });
}

bool ContentPage::validatePage()
{
    // Todas as boxas desta aba alimentam o diretório `content/` principal do MainWizard, intersecionando c/ o pipeline final do dnf ou flatpak.
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it)
        m_wiz->setOpt(QString("content/%1").arg(it.key()), it.value()->isChecked());
    return true;
}
