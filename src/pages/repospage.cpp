#include <QCheckBox>
#include <QPalette>
#include "repospage.h"
#include "../mainwizard.h"
#include "../pagehelpers.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QApplication>
#include <QIcon>

ReposPage::ReposPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Repositórios do Sistema"));
    setSubTitle(tr("Habilite grandes fontes de software de terceiros. O RPM Fusion é um pré-requisito estrito para multimídia avançada, jogos fluídos e pacotes fechados."));
}

void ReposPage::initializePage()
{
    // Limpa a tela se o usuário der 'Back' e voltar
    if (layout()) {
        QLayoutItem *item;
        while ((item = layout()->takeAt(0))) { if (item->widget()) item->widget()->deleteLater(); delete item; }
        delete layout();
    }
    m_boxes.clear();

    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setSpacing(20);
    outerLayout->setContentsMargins(10, 10, 10, 10);

    // 1. Barra de Ferramentas / Controle Superior
    auto *toolbarWidget = new QWidget;
    auto *toolbar = new QHBoxLayout(toolbarWidget);
    toolbar->setContentsMargins(0,0,0,0);
    
    m_checkingLabel = new QLabel(tr("⏳ Validando mirrors locais..."));
    m_checkingLabel->setStyleSheet("color: #3584e4; font-weight: bold;");
    
    auto *refreshBtn = new QPushButton(tr("↻ Atualizar"));
    auto *allBtn     = new QPushButton(tr("☑ Todos"));
    auto *noneBtn    = new QPushButton(tr("☐ Nenhum"));
    
    // Qt Style Sheets p/ os botões Dark (Aprovado Requisito)
    QString btnStyle = 
        "QPushButton { "
        "  background-color: rgba(255,255,255,0.05); "
        "  color: #dddddd; "
        "  border: 1px solid rgba(255,255,255,0.1); "
        "  border-radius: 6px; "
        "  padding: 6px 12px; "
        "  font-weight: bold;"
        "} "
        "QPushButton:hover { background-color: rgba(255,255,255,0.1); color: #ffffff; border-color: rgba(255,255,255,0.2); }";

    refreshBtn->setStyleSheet(btnStyle);
    refreshBtn->setCursor(Qt::PointingHandCursor);
    refreshBtn->setToolTip(tr("Refaz varredura DNF de instalação no cache local"));
    
    allBtn->setStyleSheet(btnStyle);
    allBtn->setCursor(Qt::PointingHandCursor);
    
    noneBtn->setStyleSheet(btnStyle);
    noneBtn->setCursor(Qt::PointingHandCursor);

    connect(refreshBtn, &QPushButton::clicked, this, [this] { initializePage(); });
    connect(allBtn,  &QPushButton::clicked, this, [this]{ for (auto *cb : m_boxes) cb->setChecked(true); });
    connect(noneBtn, &QPushButton::clicked, this, [this]{ for (auto *cb : m_boxes) cb->setChecked(false); });

    toolbar->addStretch();
    // Botões alinhados à direita
    toolbar->addWidget(m_checkingLabel);
    toolbar->addSpacing(15);
    toolbar->addWidget(refreshBtn);
    toolbar->addWidget(allBtn);
    toolbar->addWidget(noneBtn);
    
    outerLayout->addWidget(toolbarWidget);

    // 2. Área Central - Geração de Cards
    auto *cardsLayout = new QVBoxLayout;
    cardsLayout->setSpacing(15);

    // Factory Method interno limpo
    auto createCard = [&](const QString &key, const QString &title, const QString &desc, const QString &iconName) {
        auto *card = new QFrame;
        card->setObjectName("RepoCard");
        // QSS dos cards: Cores de background relativas, cantos e interatividade hover
        card->setStyleSheet(
            "QFrame#RepoCard { "
            "  background-color: rgba(255, 255, 255, 0.04);" // Leve destaque do fundo
            "  border: 1px solid rgba(255, 255, 255, 0.08);"
            "  border-radius: 12px;" // Cantos super-arredondados (Requisito)
            "}"
            "QFrame#RepoCard:hover { "
            "  background-color: rgba(255, 255, 255, 0.08);" // Acende ao passar mouse
            "  border: 1px solid rgba(255, 255, 255, 0.2);"
            "}"
        );

        auto *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(20, 20, 20, 20);
        
        // Ícone visual
        auto *iconLabel = new QLabel;
        QIcon cIcon = QIcon::fromTheme(iconName);
        if(cIcon.isNull()) cIcon = QIcon::fromTheme("package-x-generic");
        iconLabel->setPixmap(cIcon.pixmap(56, 56)); // Renderiza largo p/ cards
        
        // Stack Vertical Interno de Textos
        auto *textLayout = new QVBoxLayout;
        textLayout->setSpacing(5);
        
        auto *headerLayout = new QHBoxLayout;
        auto *titleLabel = new QLabel(QString("<b>%1</b>").arg(title));
        titleLabel->setStyleSheet("font-size: 16px; color: #ffffff;");
        
        auto *badgeLabel = new QLabel(tr("[Checando DNF...]"));
        badgeLabel->setObjectName("badge_" + key); // ID Mestre para a async callback atuar
        badgeLabel->setStyleSheet("color: #666666; font-weight: bold; background: transparent;");
        badgeLabel->setAlignment(Qt::AlignCenter);
        
        headerLayout->addWidget(titleLabel);
        headerLayout->addSpacing(15);
        headerLayout->addWidget(badgeLabel);
        headerLayout->addStretch();
        
        auto *descLabel = new QLabel(desc);
        descLabel->setWordWrap(true);
        descLabel->setStyleSheet("color: #b0b0b0; font-size: 13px; line-height: 1.3;");
        
        textLayout->addLayout(headerLayout);
        textLayout->addWidget(descLabel);
        textLayout->addStretch();
        
        // Controle Checkbox Direita
        auto *checkbox = new QCheckBox;
        checkbox->setCursor(Qt::PointingHandCursor);
        checkbox->setStyleSheet("QCheckBox::indicator { width: 28px; height: 28px; }"); 
        
        m_boxes[key] = checkbox;

        // Assembly
        cardLayout->addWidget(iconLabel);
        cardLayout->addSpacing(20);
        cardLayout->addLayout(textLayout, 1);
        cardLayout->addWidget(checkbox);
        
        cardsLayout->addWidget(card);
    };

    // Card Livre
    createCard("rpmfusion_free", "RPM Fusion Free", 
               tr("Disponibiliza software de código aberto isento das restrições proprietárias mas não unificado no repositório Fedora oficial (Ex: codecs de vídeo ffmpeg, h264, h265, reprodutor nativo VLC Media)."),
               "software-store");

    // Card Proprietário
    createCard("rpmfusion_nonfree", "RPM Fusion NonFree", 
               tr("Portal chave para pacotes comerciais/fechados vitais para certas rotinas do Linux Gamer, autorizando injeção limpa de ferramentas como o App Steam, Drivers proprietários da NVIDIA e componentes corporativos atrelados."),
               "preferences-system-network"); // Um ícone estético alternativo 

    cardsLayout->addStretch();
    outerLayout->addLayout(cardsLayout);

    // 3. Executador Assíncrono para Injetar cores nos badges dinâmicos
    QList<QPair<QString, std::function<bool()>>> _checks;
    _checks.append({"rpmfusion_free",    []{ return isDnfInstalled("rpmfusion-free-release"); }});
    _checks.append({"rpmfusion_nonfree", []{ return isDnfInstalled("rpmfusion-nonfree-release"); }});
    
    runChecksAsync(this, _checks, [this](QMap<QString,bool> results) {
        for (auto it = results.constBegin(); it != results.constEnd(); ++it) {
            // Varre o DOM via ID rastreável
            auto *badge = this->findChild<QLabel*>("badge_" + it.key());
            if (!badge) continue;
            
            if (it.value()) {
                badge->setText(tr("INSTALADO"));
                // Altera QSS para a Pílula Verde Escura com Bordas
                badge->setStyleSheet(
                    "background-color: #2b7a42; color: #ffffff;" // Tom orgânico Dark-Green
                    "padding: 3px 10px; border-radius: 6px; font-weight: bold; font-size: 11px;"
                );
            } else {
                badge->setText(tr("NÃO INSTALADO"));
                // Altera QSS para Alerto Dourado/Laranja
                badge->setStyleSheet(
                    "background-color: #ba6600; color: #ffffff;"
                    "padding: 3px 10px; border-radius: 6px; font-weight: bold; font-size: 11px;"
                );
            }
        }
        if (m_checkingLabel) m_checkingLabel->setVisible(false);
    });
}

// Emite ao clique de Avanço no QWizard
bool ReposPage::validatePage()
{
    // Grava as caixas marcadas nas opções m_wiz globais
    for (auto it = m_boxes.constBegin(); it != m_boxes.constEnd(); ++it)
        m_wiz->setOpt(QString("repos/%1").arg(it.key()), it.value()->isChecked());
    return true;
}
