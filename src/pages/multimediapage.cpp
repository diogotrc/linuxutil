#include <QPalette>
#include "multimediapage.h"
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

MultimediaPage::MultimediaPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Multimídia & Codecs"));
    setSubTitle(tr("Suporte a vídeo, áudio e codecs. O repositório RPM Fusion é estritamente exigido a seguir."));
}

void MultimediaPage::addToolCard(QString key, QString name, QString desc, bool installed, const QString &iconName)
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
    if(devIcon.isNull()) devIcon = QIcon::fromTheme("applications-multimedia");
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

void MultimediaPage::initializePage()
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

    // -- Segurança Vibrante (Lógica rpmfusion) --
    bool fusionFree = isDnfInstalled("rpmfusion-free-release") || m_wiz->getOpt("repos/rpmfusion_free").toBool();
    bool fusionNonfree = isDnfInstalled("rpmfusion-nonfree-release") || m_wiz->getOpt("repos/rpmfusion_nonfree").toBool();

    if (!fusionFree || !fusionNonfree) {
        auto *warningFrame = new QFrame;
        warningFrame->setStyleSheet("background-color: rgba(220, 50, 50, 0.15); border: 1px solid rgba(220, 50, 50, 0.3); border-radius: 8px; padding: 6px 12px;");
        auto *warnLayout = new QHBoxLayout(warningFrame);
        auto *warnIcon = new QLabel("⚠️");
        warnIcon->setStyleSheet("font-size: 20px;");
        
        auto *warnText = new QLabel(tr("<b>Atenção:</b> Os repositórios do RPM Fusion parecem estar ausentes. Volte na Etapa de Repositórios e certifique-se de habilitá-los, ou essas instalações de Codecs irão falhar silenciosamente."));
        warnText->setWordWrap(true);
        warnText->setStyleSheet("color: #ffb3b3; font-size: 12px; line-height: 1.2;");
        
        warnLayout->addWidget(warnIcon);
        warnLayout->addSpacing(8);
        warnLayout->addWidget(warnText, 1);
        
        outer->addWidget(warningFrame);
    }

    // Botões Fixos Superiores
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

    // Instanciamento das Ferramentas
    addToolCard("ffmpeg", tr("FFmpeg (Core swap)"), 
                tr("Substitui o pacote ffmpeg-free castrado do Fedora pelo pacote original completo contendo todos os codificadores modernos H264 e H265 ativados."), 
                false, "video-x-generic");

    addToolCard("gst_bad_free_extras", tr("GStreamer Bad-Free Extras"), 
                tr("Sub-plugins adicionais de extensão base para leitura em matrizes de players nativos."), 
                false, "application-x-addon");

    addToolCard("gst_bad_nonfree", tr("GStreamer Ugly Plugins"), 
                tr("Camadas vitais pesadas e não-livres, injetando suporte robusto retroativo a formatos MP3, leitura de DVDs e muito mais ao Totem e afins."), 
                false, "media-optical");

    addToolCard("vlc", tr("VLC Media Player"), 
                tr("Player hiper robusto famoso por conter seus próprios codecs auto-suficientes, abrindo de tudo e um pouco mais sem dependências externas."), 
                false, "vlc");

    m_cardsLayout->addStretch();
    scroll->setWidget(inner);
    outer->addWidget(scroll);

    // Async Check
    QList<QPair<QString, std::function<bool()>>> _checks;
    _checks.append({"ffmpeg", []{ return isDnfInstalled("ffmpeg"); }});
    _checks.append({"gst_bad_free_extras", []{ return isDnfInstalled("gstreamer1-plugins-bad-free-extras"); }});
    _checks.append({"gst_bad_nonfree", []{ return isDnfInstalled("gstreamer1-plugins-ugly"); }});
    _checks.append({"vlc", []{ return isDnfInstalled("vlc"); }});
    
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

bool MultimediaPage::validatePage()
{
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it)
        m_wiz->setOpt(QString("media/%1").arg(it.key()), it.value()->isChecked());
    return true;
}
