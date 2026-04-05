#include <QPalette>
#include "systemtoolspage.h"
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

SystemToolsPage::SystemToolsPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Ferramentas e Ajustes de Sistema"));
    setSubTitle(tr("Utilitários essenciais de manutenção e otimização da máquina."));
}

void SystemToolsPage::addToolCard(QString key, QString name, QString desc, bool installed, const QString &iconName)
{
    auto *card = new QFrame;
    card->setObjectName("ToolCard_" + key);
    // Aplicação QSS Exata (Cards Arredondados com hover suave e padding 8px)
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
    cardLayout->setContentsMargins(8, 8, 8, 8); // Padding 8px solicitado
    cardLayout->setSpacing(12);
    
    // 1) QCheckBox à Esquerda
    auto *checkbox = new QCheckBox;
    checkbox->setCursor(Qt::PointingHandCursor);
    checkbox->setStyleSheet("QCheckBox::indicator { width: 22px; height: 22px; }"); 
    checkbox->setChecked(installed);
    m_boxes[key] = checkbox;

    // 2) Ícone representativo pequeno
    auto *iconLabel = new QLabel;
    QIcon devIcon = QIcon::fromTheme(iconName);
    if(devIcon.isNull()) devIcon = QIcon::fromTheme("utilities-terminal");
    iconLabel->setPixmap(devIcon.pixmap(32, 32));
    
    // 3) QVBoxLayout Interno para Título e Descrição
    auto *textLayout = new QVBoxLayout;
    textLayout->setSpacing(2);
    
    auto *titleLabel = new QLabel(QString("<b>%1</b>").arg(name));
    titleLabel->setStyleSheet("font-size: 14px; color: #ffffff;");
    
    auto *descLabel = new QLabel(desc);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #999999; font-size: 12px; line-height: 1.2;");
    
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(descLabel);
    
    // 4) Badge à Direita
    auto *badgeLabel = new QLabel(tr("[Checando...]"));
    badgeLabel->setObjectName("badge_" + key);
    badgeLabel->setFixedWidth(130);
    badgeLabel->setStyleSheet("color: #777777; font-weight: bold; background: transparent; padding: 4px 6px; border-radius: 6px;");
    badgeLabel->setAlignment(Qt::AlignCenter);
    badgeLabel->setWordWrap(false);
    badgeLabel->setProperty("isInstalled", false); // Armazena bool de estado para lógica de hover/check
    m_badges[key] = badgeLabel;
    
    // Lógica Dinâmica do Badge: Se marcar (reinstalar), avisa.
    connect(checkbox, &QCheckBox::toggled, this, [badgeLabel](bool checked) {
        bool isActullyInstalled = badgeLabel->property("isInstalled").toBool();
        if (isActullyInstalled) {
            if (checked) {
                badgeLabel->setText(tr("[REINSTALAR]"));
                badgeLabel->setStyleSheet("background-color: rgba(53, 132, 228, 0.85); color: #ffffff; padding: 4px 8px; border-radius: 6px; font-weight: bold; font-size: 11px;");
            } else {
                badgeLabel->setText(tr("[INSTALADO]"));
                badgeLabel->setStyleSheet("background-color: rgba(43, 122, 66, 0.85); color: #ffffff; padding: 4px 8px; border-radius: 6px; font-weight: bold; font-size: 11px;");
            }
        }
    });

    // Paking no Card
    cardLayout->addWidget(checkbox);
    cardLayout->addWidget(iconLabel);
    cardLayout->addLayout(textLayout, 1);
    cardLayout->addWidget(badgeLabel);
    
    m_cardsLayout->addWidget(card);
}

void SystemToolsPage::initializePage()
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

    // Botões de Ação Global (Topo)
    auto *toolbarWidget = new QWidget;
    auto *toolbar = new QHBoxLayout(toolbarWidget);
    toolbar->setContentsMargins(0,0,0,0);
    
    m_checkingLabel = new QLabel(tr("⏳ Verificando integridade..."));
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

    // Container de Scrollbar fina/estilizada
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
    m_cardsLayout = new QVBoxLayout(inner); 
    m_cardsLayout->setSpacing(10);
    m_cardsLayout->setContentsMargins(0, 0, 10, 0);

    // -- SEÇÃO 1: Ferramentas e Utilitários --
    auto *grp1Label = new QLabel(tr("<b>Ferramentas do Sistema & Utilitários</b>"));
    grp1Label->setStyleSheet("font-size: 15px; color: #4cd14c; margin-top: 10px;");
    m_cardsLayout->addWidget(grp1Label);

    // XRDP foi removido
    const QList<std::tuple<QString,QString,QString>> items = {
        {"fastfetch",     tr("Fastfetch"), tr("Ferramenta veloz de informações de host de terminal.")},
        {"btop",          tr("BTop"),      tr("Monitor de recursos com uma bela UI colorida nos terminas de shell.")},
        {"htop",          tr("HTop"),      tr("Visualizador simples interativo baseado em textos (clássico).")},
        {"distrobox",     tr("Distrobox"), tr("Sub-instale ArchLinux, Debian e outras distros no mesmo terminal.")},
        {"timeshift",     tr("Timeshift"), tr("Proteção de sistema via Snapshots incríveis de estado completo via BTRFS.")},
    };

    for (const auto &[key, name, desc] : items) {
        addToolCard(key, name, desc, false, "utilities-terminal");
    }

    // -- SEÇÃO 2: Ajustes Modulares do OS --
    auto *grp2Label = new QLabel(tr("<b>Ajustes do Sistema</b>"));
    grp2Label->setStyleSheet("font-size: 15px; color: #4cd14c; margin-top: 20px;");
    m_cardsLayout->addWidget(grp2Label);

    // Nova Integração solicitada
    addToolCard("fix_kde_google", tr("Corrigir integração KDE Google"), 
                tr("Substitui o provedor de accounts KDE por um script que restabelece com perfeição o login do Google Drive e YouTube nas contas online."), 
                false, "applications-system");

    addToolCard("nm_wait_online", tr("Desabilitar NetworkManager-wait-online"), 
                tr("Otimiza tempo de boot em quase 20s desativando uma service ociosa."), 
                false, "preferences-system-network");

    addToolCard("clean_cache", tr("Limpeza de DNF Residual"), 
                tr("Após todas as dezenas de instalações do Rapidora, faz um flush de gigabytes que não seriam mais usados."), 
                false, "edit-clear-all");

    m_cardsLayout->addStretch();
    scroll->setWidget(inner);
    outer->addWidget(scroll);

    // Engine de Checagem
    QList<QPair<QString, std::function<bool()>>> _checks;
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it) {
        QString key = it.key();
        
        if (key == "fix_kde_google") {
            _checks.append({key, []{ return false; }}); // Sempre será false (script corretivo manual)
        } else if (key == "nm_wait_online" || key == "clean_cache") {
            _checks.append({key, []{ return false; }});
        } else {
            _checks.append({key, [key]{ return isDnfInstalled(key); }});
        }
    }
    
    runChecksAsync(this, _checks, [this](QMap<QString,bool> results) {
        for (auto it = results.constBegin(); it != results.constEnd(); ++it) {
            if (!m_badges.contains(it.key())) continue;
            auto *badge = m_badges[it.key()];
            
            badge->setProperty("isInstalled", it.value()); // Salva metadados pro hover do checkbox

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

bool SystemToolsPage::validatePage()
{
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it)
        m_wiz->setOpt(QString("systools/%1").arg(it.key()), it.value()->isChecked());
    return true;
}
