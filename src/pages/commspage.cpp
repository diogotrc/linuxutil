#include <QPalette>
#include "commspage.h"
#include "../mainwizard.h"
#include "../pagehelpers.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>
#include <QApplication>
#include <QIcon>
#include <QVariant>
#include <QMessageBox>

CommsPage::CommsPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Comunicação & Produtividade"));
    setSubTitle(tr("Ferramentas de chat, ambientes de construção em IA e suíte de edição."));
}

void CommsPage::addToolCard(QLayout *layout, QString key, QString name, QString desc, bool installed, const QString &iconName, int row, int col)
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
    cardLayout->setContentsMargins(12, 12, 12, 12);
    cardLayout->setSpacing(12);
    
    auto *checkbox = new QCheckBox;
    checkbox->setCursor(Qt::PointingHandCursor);
    checkbox->setStyleSheet("QCheckBox::indicator { width: 22px; height: 22px; }"); 
    checkbox->setChecked(installed);
    m_boxes[key] = checkbox;

    auto *iconLabel = new QLabel;
    QIcon devIcon = QIcon::fromTheme(iconName);
    if(devIcon.isNull()) devIcon = QIcon::fromTheme("applications-internet");
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

        if (checked) {
            card->setStyleSheet(
                "QFrame[objectName^=\"ToolCard_\"] { "
                "  background-color: rgba(53, 132, 228, 0.15);"
                "  border: 1px solid #3584e4;"
                "  border-radius: 12px;"
                "}"
            );
        } else {
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
    
    if (auto *grid = qobject_cast<QGridLayout*>(layout)) {
        grid->addWidget(card, row, col);
    } else if (auto *vbox = qobject_cast<QVBoxLayout*>(layout)) {
        vbox->addWidget(card);
    }
}

void CommsPage::initializePage()
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

    // Toolbar superior
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
    // Renderização Dinâmica - Extremos
    // ==========================================
    
    addSectionLabel(tr("Framework de IA (Fedora RPM)"));
    addToolCard(innerLayout, "antigravity", tr("Google Antigravity"), tr("Construa de uma nova maneira usando IA. Um ecossistema completo provido pelos repositórios globais da Google de forma nativa."), false, "google");

    addSectionLabel(tr("Redes & Comunicação (Flatpaks Isolados)"));
    
    auto *gridContainer = new QWidget;
    auto *gridLayout = new QGridLayout(gridContainer);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(10);

    // Mapeamento Bidimensional usando QGridLayout via Cast Direto de Interface
    addToolCard(gridLayout, "telegram", tr("Telegram Desktop"), tr("Mensageiro rápido nativizado para nuvem via API aberta Telegram."), false, "telegram", 0, 0);
    addToolCard(gridLayout, "zapzap", tr("ZapZap (WhatsApp)"), tr("Wrapper otimizadíssimo de WhatsApp Web feito nativamente em C++ WebKit."), false, "whatsapp", 0, 1);
    
    addToolCard(gridLayout, "spotify", tr("Spotify App"), tr("Cliente de streaming musical e podcasts oficial da Spotify."), false, "spotify", 1, 0);
    addToolCard(gridLayout, "stellarium", tr("Stellarium Planetarium"), tr("Descubra os astros e trace planetas em tempo real direto da sua máquina."), false, "stellarium", 1, 1);
    
    addToolCard(gridLayout, "onlyoffice", tr("OnlyOffice"), tr("Poderosa suíte corporativa, 100% compatível localmente com planilhas e docs Word XSLX."), false, "onlyoffice-desktopeditors", 2, 0);
    addToolCard(gridLayout, "bazaar", tr("Bazaar Center"), tr("Encontre complementos incriveis, módulos extras e extensores robustos."), false, "bazaar", 2, 1);

    innerLayout->addWidget(gridContainer);
    innerLayout->addStretch();
    scroll->setWidget(inner);
    outer->addWidget(scroll);

    // ==========================================
    // Verificação de Sistema (RPM + OSTree)
    // ==========================================
    QList<QPair<QString, std::function<bool()>>> _checks;
    _checks.append({"antigravity", []{ return isDnfInstalled("google-antigravity"); }});
    _checks.append({"telegram", []{ return isFlatpakInstalled("org.telegram.desktop"); }});
    _checks.append({"zapzap", []{ return isFlatpakInstalled("com.rtosta.zapzap"); }});
    _checks.append({"spotify", []{ return isFlatpakInstalled("com.spotify.Client"); }});
    _checks.append({"stellarium", []{ return isFlatpakInstalled("org.stellarium.Stellarium"); }});
    _checks.append({"onlyoffice", []{ return isFlatpakInstalled("org.onlyoffice.desktopeditors"); }});
    _checks.append({"bazaar", []{ return isFlatpakInstalled("io.github.kolunmi.Bazaar"); }});
    
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

            emit cb->toggled(cb->isChecked()); // Trigger colors manually post-spawn
        }
        if (m_checkingLabel) m_checkingLabel->setVisible(false);
    });
}

bool CommsPage::validatePage()
{
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it) {
        m_wiz->setOpt(QString("comms/%1").arg(it.key()), it.value()->isChecked());
    }

    bool tg_checked = m_boxes.contains("telegram") && m_boxes["telegram"]->isChecked();
    bool zz_checked = m_boxes.contains("zapzap") && m_boxes["zapzap"]->isChecked();
    
    // Dica Vibecoding Elevada! Diálogo inteligente
    if (tg_checked || zz_checked) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, 
                                      tr("Configuração de Inicialização"), 
                                      tr("Habilitar Inicialização Automática?\n\n"
                                         "Você selecionou comunicadores (Telegram/ZapZap). Deseja adicioná-los para ligarem automaticamente junto com o sistema operacional?"),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if (tg_checked) m_wiz->setOpt("comms/autostart_chats_telegram", true);
            if (zz_checked) m_wiz->setOpt("comms/autostart_chats_zapzap", true);
        } else {
            m_wiz->setOpt("comms/autostart_chats_telegram", false);
            m_wiz->setOpt("comms/autostart_chats_zapzap", false);
        }
    }
    
    return true;
}
