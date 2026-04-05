#include "donepage.h"
#include "../mainwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPlainTextEdit>
#include <QProcess>
#include <QClipboard>
#include <QApplication>
#include <QFrame>
#include <QPushButton>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QFont>

// ─────────────────────────────────────────────────────────────────────────────
// DonePage Constructor — the UI skeleton is built once here. initializePage()
// populates the dynamic content each time the page is shown.
// ─────────────────────────────────────────────────────────────────────────────
DonePage::DonePage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Configuração Concluída"));
    setSubTitle(tr("Tudo pronto! Seu sistema Fedora foi otimizado com sucesso."));

    // ── Root scroll area so content doesn't clip on small windows ─────────
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto *scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background: transparent; }");
    outerLayout->addWidget(scroll);

    auto *content = new QWidget;
    content->setObjectName("DoneContent");
    scroll->setWidget(content);

    auto *root = new QVBoxLayout(content);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(16);

    // ── Hero Card ─────────────────────────────────────────────────────────
    auto *heroFrame = new QFrame;
    heroFrame->setObjectName("HeroCard");
    heroFrame->setStyleSheet(
        "QFrame#HeroCard {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "    stop:0 rgba(53,132,228,0.12), stop:1 rgba(36,190,140,0.10));"
        "  border: 1px solid rgba(53,132,228,0.3);"
        "  border-radius: 18px;"
        "}"
    );
    auto *heroGlow = new QGraphicsDropShadowEffect(heroFrame);
    heroGlow->setBlurRadius(40);
    heroGlow->setColor(QColor(53, 132, 228, 90));
    heroGlow->setOffset(0, 4);
    heroFrame->setGraphicsEffect(heroGlow);

    auto *heroLayout = new QVBoxLayout(heroFrame);
    heroLayout->setContentsMargins(30, 30, 30, 24);
    heroLayout->setSpacing(10);
    heroLayout->setAlignment(Qt::AlignCenter);

    // Icon
    m_heroIcon = new QLabel("✅");
    m_heroIcon->setAlignment(Qt::AlignCenter);
    m_heroIcon->setStyleSheet("font-size: 64px; background: transparent;");

    m_heroTitle = new QLabel(tr("Tudo pronto!"));
    m_heroTitle->setAlignment(Qt::AlignCenter);
    m_heroTitle->setStyleSheet(
        "font-size: 26px; font-weight: 700; color: #ffffff; background: transparent;");

    m_heroSubtitle = new QLabel(tr("Seu sistema foi otimizado com sucesso."));
    m_heroSubtitle->setAlignment(Qt::AlignCenter);
    m_heroSubtitle->setStyleSheet(
        "font-size: 14px; color: #8899aa; background: transparent;");

    heroLayout->addWidget(m_heroIcon);
    heroLayout->addWidget(m_heroTitle);
    heroLayout->addWidget(m_heroSubtitle);
    root->addWidget(heroFrame);

    // ── Installed Badges Row ──────────────────────────────────────────────
    m_badgesContainer = new QFrame;
    m_badgesContainer->setStyleSheet(
        "QFrame { background: rgba(255,255,255,0.03);"
        "  border: 1px solid rgba(255,255,255,0.07);"
        "  border-radius: 12px; }"
    );
    m_badgesLayout = new QVBoxLayout(m_badgesContainer);
    m_badgesLayout->setContentsMargins(16, 12, 16, 12);
    m_badgesLayout->setSpacing(8);
    root->addWidget(m_badgesContainer);

    // ── Status Pill ───────────────────────────────────────────────────────
    m_statusBadge = new QLabel;
    m_statusBadge->setAlignment(Qt::AlignCenter);
    m_statusBadge->setFixedHeight(36);
    m_statusBadge->setStyleSheet(
        "background-color: rgba(36, 190, 140, 0.20);"
        "color: #36be8c;"
        "font-size: 13px;"
        "font-weight: bold;"
        "border: 1px solid rgba(36,190,140,0.4);"
        "border-radius: 18px;"
        "padding: 0 20px;"
    );
    root->addWidget(m_statusBadge);

    // ── Reboot Recommendation ─────────────────────────────────────────────
    m_rebootFrame = new QFrame;
    m_rebootFrame->setStyleSheet(
        "QFrame {"
        "  background: rgba(255, 165, 0, 0.10);"
        "  border: 1px solid rgba(255,165,0,0.3);"
        "  border-radius: 10px;"
        "  padding: 6px 12px;"
        "}"
    );
    auto *rebootLayout = new QHBoxLayout(m_rebootFrame);
    rebootLayout->setContentsMargins(12, 10, 12, 10);
    rebootLayout->setSpacing(12);

    auto *rebootIcon = new QLabel("⚠️");
    rebootIcon->setStyleSheet("font-size: 20px; background: transparent;");
    auto *rebootText = new QLabel(
        tr("<b>Reinício Recomendado</b><br/>"
           "<span style='color:#bbbbbb; font-size:12px;'>"
           "Um reinício é necessário para que Kernel, GPU, Virtualização e permissões de grupo entrem totalmente em vigor."
           "</span>"));
    rebootText->setWordWrap(true);
    rebootText->setTextFormat(Qt::RichText);
    rebootText->setStyleSheet("color: #ffcc55; background: transparent;");

    rebootLayout->addWidget(rebootIcon);
    rebootLayout->addWidget(rebootText, 1);
    root->addWidget(m_rebootFrame);

    // ── Failed Steps Section (hidden by default) ──────────────────────────
    m_errorFrame = new QFrame;
    m_errorFrame->setStyleSheet(
        "QFrame { background: rgba(220,50,50,0.10);"
        "  border: 1px solid rgba(220,50,50,0.3);"
        "  border-radius: 10px; }"
    );
    auto *errorLayout = new QVBoxLayout(m_errorFrame);
    errorLayout->setContentsMargins(14, 12, 14, 12);
    errorLayout->setSpacing(8);

    auto *errorHeader = new QHBoxLayout;
    auto *errorIcon   = new QLabel("❌");
    errorIcon->setStyleSheet("font-size: 18px; background: transparent;");
    auto *errorTitle  = new QLabel(tr("<b style='color:#ff6b6b;'>Etapas com Erro</b>"));
    errorTitle->setTextFormat(Qt::RichText);
    errorTitle->setStyleSheet("background: transparent;");
    errorHeader->addWidget(errorIcon);
    errorHeader->addWidget(errorTitle, 1);

    m_errorDetail = new QPlainTextEdit;
    m_errorDetail->setReadOnly(true);
    m_errorDetail->setMaximumHeight(120);
    QFont mono("Monospace");
    mono.setStyleHint(QFont::TypeWriter);
    mono.setPointSize(9);
    m_errorDetail->setFont(mono);
    m_errorDetail->setStyleSheet(
        "QPlainTextEdit {"
        "  background: rgba(0,0,0,0.3);"
        "  color: #ff9999;"
        "  border: none;"
        "  border-radius: 6px;"
        "}"
    );

    m_copyErrorsBtn = new QPushButton(tr("📋  Copiar Log de Erros"));
    m_copyErrorsBtn->setCursor(Qt::PointingHandCursor);
    m_copyErrorsBtn->setStyleSheet(
        "QPushButton {"
        "  background: rgba(220,50,50,0.25); color: #ffbbbb;"
        "  border: 1px solid rgba(220,50,50,0.4);"
        "  border-radius: 8px; padding: 6px 16px; font-weight: bold;"
        "}"
        "QPushButton:hover { background: rgba(220,50,50,0.40); color: #ffffff; }"
    );
    connect(m_copyErrorsBtn, &QPushButton::clicked, this, &DonePage::copyErrorsToClipboard);

    errorLayout->addLayout(errorHeader);
    errorLayout->addWidget(m_errorDetail);
    errorLayout->addWidget(m_copyErrorsBtn, 0, Qt::AlignRight);
    m_errorFrame->setVisible(false);
    root->addWidget(m_errorFrame);

    root->addStretch();

    // ── Bottom Action Bar ──────────────────────────────────────────────────
    auto *actionBar = new QHBoxLayout;
    actionBar->setSpacing(10);

    m_copyLogBtn = new QPushButton(tr("📋  Copiar Log Completo"));
    m_copyLogBtn->setCursor(Qt::PointingHandCursor);
    m_copyLogBtn->setStyleSheet(
        "QPushButton {"
        "  background: rgba(255,255,255,0.07); color: #cccccc;"
        "  border: 1px solid rgba(255,255,255,0.12);"
        "  border-radius: 8px; padding: 8px 18px; font-weight: bold;"
        "}"
        "QPushButton:hover { background: rgba(255,255,255,0.14); color: #ffffff; }"
    );
    connect(m_copyLogBtn, &QPushButton::clicked, this, &DonePage::copyFullLogToClipboard);

    m_doneBtn = new QPushButton(tr("Fechar"));
    m_doneBtn->setCursor(Qt::PointingHandCursor);
    m_doneBtn->setStyleSheet(
        "QPushButton {"
        "  background: rgba(255,255,255,0.07); color: #cccccc;"
        "  border: 1px solid rgba(255,255,255,0.12);"
        "  border-radius: 8px; padding: 8px 22px; font-weight: bold;"
        "}"
        "QPushButton:hover { background: rgba(255,255,255,0.14); color: #ffffff; }"
    );
    connect(m_doneBtn, &QPushButton::clicked, []() {
        QApplication::quit();
    });

    m_rebootBtn = new QPushButton(tr("  Reiniciar Agora"));
    m_rebootBtn->setCursor(Qt::PointingHandCursor);
    m_rebootBtn->setStyleSheet(
        "QPushButton {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "    stop:0 #3584e4, stop:1 #4a9cf5);"
        "  color: #ffffff;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 9px 28px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "    stop:0 #4a9cf5, stop:1 #62aaff);"
        "}"
        "QPushButton:pressed { background: #2a6bc4; }"
    );
    connect(m_rebootBtn, &QPushButton::clicked, []() {
        QProcess::startDetached("systemctl", {"reboot"});
    });

    actionBar->addWidget(m_copyLogBtn);
    actionBar->addStretch();
    actionBar->addWidget(m_doneBtn);
    actionBar->addWidget(m_rebootBtn);
    root->addLayout(actionBar);

    // Opacity effect for entrance animation
    m_opacityEffect = new QGraphicsOpacityEffect(content);
    m_opacityEffect->setOpacity(0.0);
    content->setGraphicsEffect(m_opacityEffect);
}

// ─────────────────────────────────────────────────────────────────────────────
// initializePage() — called every time the page becomes visible
// ─────────────────────────────────────────────────────────────────────────────
void DonePage::initializePage()
{
    const int     errors      = m_wiz->getOpt("install/errorCount",  0).toInt();
    const QString failedSteps = m_wiz->getOpt("install/failedSteps", QString()).toString();

    // ── Clear previous badges ─────────────────────────────────────────────
    while (m_badgesLayout->count()) {
        auto *item = m_badgesLayout->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    buildInstalledBadges(m_badgesLayout);

    // ── Status Pill ───────────────────────────────────────────────────────
    if (errors == 0) {
        m_statusBadge->setText(tr("✔  0 erros encontrados — Instalação perfeita!"));
        m_statusBadge->setStyleSheet(
            "background-color: rgba(36,190,140,0.20); color: #36be8c;"
            "font-size: 13px; font-weight: bold;"
            "border: 1px solid rgba(36,190,140,0.4);"
            "border-radius: 18px; padding: 0 20px;"
        );
    } else {
        m_statusBadge->setText(tr("⚠  %1 etapa(s) com erro").arg(errors));
        m_statusBadge->setStyleSheet(
            "background-color: rgba(220,50,50,0.20); color: #ff8888;"
            "font-size: 13px; font-weight: bold;"
            "border: 1px solid rgba(220,50,50,0.4);"
            "border-radius: 18px; padding: 0 20px;"
        );
        // Adjust hero for partial success
        m_heroIcon->setText("⚠️");
        m_heroTitle->setText(tr("Instalação com Avisos"));
        m_heroSubtitle->setText(tr("%1 etapa(s) encontraram erros. Verifique o log abaixo.").arg(errors));
    }

    // ── Failed Steps ─────────────────────────────────────────────────────
    m_errorFrame->setVisible(errors > 0);
    if (errors > 0) {
        m_errorDetail->setPlainText(
            failedSteps.isEmpty() ? tr("(Sem detalhes disponíveis)") : failedSteps);
    }

    // ── Reboot Recommendation ─────────────────────────────────────────────
    m_rebootFrame->setVisible(isRebootRequired());

    // ── Reboot Button Focus ───────────────────────────────────────────────
    if (isRebootRequired()) {
        QTimer::singleShot(200, m_rebootBtn, [this]() {
            m_rebootBtn->setFocus();
        });
    }

    // ── Entrance fade-in animation ────────────────────────────────────────
    QTimer::singleShot(50, this, &DonePage::animateIn);
}

// ─────────────────────────────────────────────────────────────────────────────
// Builds one badge row per installed category detected from wizard options
// ─────────────────────────────────────────────────────────────────────────────
void DonePage::buildInstalledBadges(QVBoxLayout *targetLayout)
{
    auto addBadge = [&](const QString &name, const QString &emoji) {
        auto *row = new QHBoxLayout;
        row->setSpacing(10);

        auto *emojiLbl = new QLabel(emoji);
        emojiLbl->setStyleSheet("font-size: 18px; background: transparent;");
        emojiLbl->setFixedWidth(28);

        auto *nameLbl = new QLabel(QString("<b>%1</b>").arg(name));
        nameLbl->setTextFormat(Qt::RichText);
        nameLbl->setStyleSheet("color: #dddddd; font-size: 13px; background: transparent;");

        auto *pill = new QLabel(tr("INSTALADO"));
        pill->setFixedWidth(100);
        pill->setAlignment(Qt::AlignCenter);
        pill->setStyleSheet(
            "background-color: rgba(36,190,140,0.20);"
            "color: #36be8c;"
            "font-size: 11px; font-weight: bold;"
            "border: 1px solid rgba(36,190,140,0.4);"
            "border-radius: 10px; padding: 3px 10px;"
        );

        row->addWidget(emojiLbl);
        row->addWidget(nameLbl, 1);
        row->addWidget(pill);
        targetLayout->addLayout(row);
    };

    bool addedAny = false;

    // Check wizard options to show which categories actually ran
    const QString gpuChoice = m_wiz->getOpt("gpu/choice", "none").toString();

    if (m_wiz->getOpt("repos/rpmfusion_free").toBool() ||
        m_wiz->getOpt("repos/rpmfusion_nonfree").toBool()) {
        addBadge(tr("Repositórios RPM Fusion"), "📦"); addedAny = true;
    }
    if (gpuChoice == "nvidia" || gpuChoice == "amd") {
        const QString gpuName = (gpuChoice == "nvidia")
            ? tr("Drivers NVIDIA") : tr("Drivers AMD MESA");
        addBadge(gpuName, "🖥"); addedAny = true;
    }
    if (m_wiz->getOpt("gaming/steam").toBool()  ||
        m_wiz->getOpt("gaming/lutris").toBool() ||
        m_wiz->getOpt("gaming/heroic").toBool()) {
        addBadge(tr("Ferramentas de Gaming"), "🎮"); addedAny = true;
    }
    if (m_wiz->getOpt("virt/virtmanager").toBool() ||
        m_wiz->getOpt("virt/libvirt").toBool()) {
        addBadge(tr("Virtualização (KVM/QEMU)"), "⚙️"); addedAny = true;
    }
    if (m_wiz->getOpt("media/ffmpeg").toBool() ||
        m_wiz->getOpt("media/vlc").toBool()) {
        addBadge(tr("Multimídia & Codecs"), "🎬"); addedAny = true;
    }
    if (m_wiz->getOpt("browsers/firefox").toBool()  ||
        m_wiz->getOpt("browsers/chrome").toBool()   ||
        m_wiz->getOpt("browsers/brave").toBool()    ||
        m_wiz->getOpt("browsers/vivaldi").toBool()) {
        addBadge(tr("Navegadores"), "🌐"); addedAny = true;
    }
    if (m_wiz->getOpt("comms/antigravity").toBool() ||
        m_wiz->getOpt("comms/telegram").toBool()) {
        addBadge(tr("Comunicação & Produtividade"), "💬"); addedAny = true;
    }
    if (m_wiz->getOpt("content/obs").toBool()      ||
        m_wiz->getOpt("content/kdenlive").toBool() ||
        m_wiz->getOpt("content/gimp").toBool()) {
        addBadge(tr("Criação de Conteúdo"), "🎨"); addedAny = true;
    }
    if (m_wiz->getOpt("systools/btop").toBool()       ||
        m_wiz->getOpt("systools/distrobox").toBool()) {
        addBadge(tr("Ferramentas de Sistema"), "🛠"); addedAny = true;
    }

    if (!addedAny) {
        auto *placeholder = new QLabel(tr("Nenhuma categoria selecionada."));
        placeholder->setStyleSheet("color: #666666; font-style: italic; background: transparent;");
        placeholder->setAlignment(Qt::AlignCenter);
        targetLayout->addWidget(placeholder);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// isRebootRequired — returns true if any critical system component was changed
// ─────────────────────────────────────────────────────────────────────────────
bool DonePage::isRebootRequired() const
{
    const QString gpuChoice = m_wiz->getOpt("gpu/choice", "none").toString();
    // GPU driver install always needs reboot
    if (gpuChoice == "nvidia" || gpuChoice == "amd") return true;
    // Virtualisation needs usermod group changes to take effect
    if (m_wiz->getOpt("virt/libvirt").toBool())       return true;
    // Kernel modules extra
    if (m_wiz->getOpt("gaming/kernel_modules_extra").toBool()) return true;

    return false;
}

// ─────────────────────────────────────────────────────────────────────────────
// animateIn — gentle fade-in for the whole page content
// ─────────────────────────────────────────────────────────────────────────────
void DonePage::animateIn()
{
    if (!m_opacityEffect) return;
    auto *anim = new QPropertyAnimation(m_opacityEffect, "opacity", this);
    anim->setDuration(500);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

// ─────────────────────────────────────────────────────────────────────────────
// Clipboard helpers
// ─────────────────────────────────────────────────────────────────────────────
void DonePage::copyErrorsToClipboard()
{
    const QString text = m_wiz->getOpt("install/failedSteps", QString()).toString();
    QApplication::clipboard()->setText(text.isEmpty() ? tr("Nenhum erro registrado.") : text);
}

void DonePage::copyFullLogToClipboard()
{
    const QString text = m_wiz->getOpt("install/fullLog", QString()).toString();
    QApplication::clipboard()->setText(text.isEmpty() ? tr("Log vazio.") : text);
}
