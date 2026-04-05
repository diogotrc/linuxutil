#include "updatepage.h"
#include "../mainwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFrame>
#include <QProcess>
#include <QScrollBar>
#include <QApplication>
#include <QMessageBox>

UpdatePage::UpdatePage(MainWizard *wizard) 
    : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Atualização do Sistema"));
    setSubTitle(tr("Manter os pacotes de base do Fedora atualizados é essencial antes de efetuarmos injeções profundas de novos programas."));
}

UpdatePage::~UpdatePage()
{
    cleanupProc();
}

void UpdatePage::cleanupProc()
{
    if (!m_proc) return;
    m_proc->disconnect();
    if (m_proc->state() != QProcess::NotRunning) {
        m_proc->kill();
        m_proc->waitForFinished(3000);
    }
    m_proc->deleteLater();
}

void UpdatePage::initializePage()
{
    cleanupProc();
    m_done    = false;
    m_skipped = false;

    if (layout()) {
        QLayoutItem *i;
        while ((i = layout()->takeAt(0))) { if (i->widget()) i->widget()->deleteLater(); delete i; }
        delete layout();
    }

    auto *outer = new QVBoxLayout(this);
    outer->setSpacing(12);
    outer->setContentsMargins(10, 10, 10, 10);

    // 1. Info Box (Card de Alerta)
    auto *warnBox = new QFrame;
    warnBox->setObjectName("InfoBox");
    warnBox->setStyleSheet(
        "QFrame#InfoBox { "
        "  background-color: rgba(60, 140, 200, 0.15);" // Fundo levemente azulado/info
        "  border: 1px solid rgba(60, 140, 200, 0.4);"
        "  border-radius: 8px; "
        "}"
    );
    
    auto *warnLayout = new QVBoxLayout(warnBox);
    auto *warnLabel = new QLabel(
        tr("<b>Por que atualizar agora?</b><br>"
           "Instalar novos softwares com bases desatualizadas pode gerar conflitos terríveis de dependências. "
           "A garantia de sucesso da engine do Rapidora começa por um sistema em estado impecavelmente limpo "
           "e linear ao que a RedHat e a comunidade distribuem hoje.")
    );
    warnLabel->setWordWrap(true);
    // Tom amigável e legível
    warnLabel->setStyleSheet("color: #e0e0e0; font-size: 13px; margin: 5px;");
    warnLayout->addWidget(warnLabel);
    outer->addWidget(warnBox);

    // 2. Comandos (Top level da área de Log)
    auto *btnWidget = new QWidget;
    auto *btnLayout = new QHBoxLayout(btnWidget);
    btnLayout->setContentsMargins(0, 5, 0, 0);

    m_updateBtn = new QPushButton(tr(" Atualizar Agora "));
    m_updateBtn->setFixedHeight(36);
    m_updateBtn->setCursor(Qt::PointingHandCursor);
    m_updateBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #3db03d;" // Destaque para ação recomendada (Verde)
        "  color: white;"
        "  font-weight: bold;"
        "  border-radius: 6px;"
        "  padding: 0 20px;"
        "}"
        "QPushButton:hover { background-color: #4cd14c; }"
        "QPushButton:disabled { background-color: #3b3b3b; color: #777777; }"
    );

    m_skipBtn = new QPushButton(tr(" Pular "));
    m_skipBtn->setFixedHeight(36);
    m_skipBtn->setCursor(Qt::PointingHandCursor);
    m_skipBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: transparent;" // Estilo Secundário Limpo
        "  color: #aaaaaa;"
        "  border: 1px solid #777777;"
        "  font-weight: bold;"
        "  border-radius: 6px;"
        "  padding: 0 15px;"
        "}"
        "QPushButton:hover { background-color: rgba(255,255,255,0.05); color: #ffffff; }"
        "QPushButton:disabled { border: 1px solid #333333; color: #444444; }"
    );

    connect(m_updateBtn, &QPushButton::clicked, this, &UpdatePage::runUpdate);
    connect(m_skipBtn,   &QPushButton::clicked, this, [this] {
        m_skipped = true;
        m_done    = true;
        m_updateBtn->setEnabled(false);
        m_skipBtn->setEnabled(false);
        m_statusLabel->setText(tr("<span style='color:#cc7700;'>⚠ Execução pulada pelo usuário. Prossiga sob sua própria responsabilidade.</span>"));
        emit completeChanged();
    });

    btnLayout->addWidget(m_updateBtn);
    btnLayout->addWidget(m_skipBtn);
    btnLayout->addStretch();
    outer->addWidget(btnWidget);

    // 3. Label flutuante de Status
    m_statusLabel = new QLabel;
    m_statusLabel->setWordWrap(true);
    outer->addWidget(m_statusLabel);

    // 4. Progress Bar (Sutil)
    m_progressBar = new QProgressBar;
    m_progressBar->setTextVisible(false);
    m_progressBar->setFixedHeight(4); // Fina e Indiscreta
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "  border: none;"
        "  background: #2a2a2a;"
        "  border-radius: 2px;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #3584e4;" // Azul Loader Destaque
        "  border-radius: 2px;"
        "}"
    );
    m_progressBar->hide(); // Oculta até a operação iniciar
    outer->addWidget(m_progressBar);

    // 5. Caixa de Alerta p/ Kernel Novo 
    m_kernelBox = new QFrame;
    m_kernelBox->setObjectName("KernelBox");
    m_kernelBox->setStyleSheet("QFrame#KernelBox { border: 2px solid #cc7700; border-radius: 8px; background-color: rgba(200, 100, 0, 0.1); }");
    
    auto *kernelLayout = new QVBoxLayout(m_kernelBox);
    auto *kernelLabel = new QLabel(
        tr("<b>⚠ Atualização de Kernel Detectada</b><br><br>"
           "Uma parte importantíssima do cérebro do sistema (o Kernel) sofreu alterações. Se formos instalar pacotes base nível-hardware adiante (como drivers GPU Virtuais ou MesaVulkan), eles precisam rastrear os headers deste kernel atualizado ao invés do atual.<br><br>"
           "Sugerimos <b>Reiniciar Imediatamente</b> e reabrir o Rapidora.")
    );
    kernelLabel->setWordWrap(true);
    kernelLayout->addWidget(kernelLabel);

    m_rebootBtn = new QPushButton(tr("Reiniciar Imediatamente"));
    m_rebootBtn->setFixedHeight(36);
    m_rebootBtn->setStyleSheet("background-color: #cc7700; color: white; font-weight: bold; border-radius: 6px; padding: 0 15px;");
    connect(m_rebootBtn, &QPushButton::clicked, this, &UpdatePage::reboot);

    auto *kernelBtnLayout = new QHBoxLayout;
    kernelBtnLayout->addWidget(m_rebootBtn);
    
    auto *continueAnywayBtn = new QPushButton(tr("Ignorar Risco e Continuar"));
    continueAnywayBtn->setFixedHeight(36);
    connect(continueAnywayBtn, &QPushButton::clicked, this, [this] {
        m_kernelBox->setVisible(false);
        m_done = true;
        emit completeChanged();
    });
    kernelBtnLayout->addWidget(continueAnywayBtn);
    kernelBtnLayout->addStretch();
    
    kernelLayout->addLayout(kernelBtnLayout);
    m_kernelBox->setVisible(false);
    outer->addWidget(m_kernelBox);

    // 6. Output Terminal (Log da operação dnf)
    m_log = new QPlainTextEdit;
    m_log->setReadOnly(true);
    // Fonte Monospace Fixa
    QFont monoFont("monospace");
    monoFont.setStyleHint(QFont::Monospace);
    monoFont.setPointSize(10);
    m_log->setFont(monoFont);
    
    m_log->setMinimumHeight(220);
    m_log->setObjectName("LogTerminal");
    m_log->setStyleSheet(
        "QPlainTextEdit#LogTerminal {"
        "  background-color: #121212;" // Superfície dark total
        "  color: #00ff00;" // Texto Estilo Terminal Matrix
        "  border: 1px solid #333333;"
        "  border-radius: 10px;"
        "  padding: 8px;"
        "}"
    );
    // Fill remaining layout space pushing UI to top
    outer->addWidget(m_log, 1);
}

void UpdatePage::runUpdate()
{
    if (m_proc && m_proc->state() != QProcess::NotRunning) return;

    m_updateBtn->setEnabled(false);
    m_skipBtn->setEnabled(false);
    m_log->clear();
    
    // Mostra progress bar no modo 'Indeterminado/Loading' pulsante
    m_progressBar->setRange(0, 0); 
    m_progressBar->show();

    m_statusLabel->setText(tr("<i>Validando ramificações de Kernel do Fedora antes do procedimento...</i>"));
    m_kernelsBefore.clear();

    // Rotina assíncrona p/ ler pacotes RPM
    auto *snap = new QProcess;
    connect(snap, &QProcess::finished, this, [this, snap](int, QProcess::ExitStatus) {
        m_kernelsBefore = QString::fromUtf8(snap->readAllStandardOutput()).trimmed();
        snap->deleteLater();
        startDnfUpgrade();
    });
    snap->start("rpm", {"-q", "kernel"});
}

void UpdatePage::startDnfUpgrade()
{
    m_statusLabel->setText(tr("<i>Negociando com servidores online... Baixando via <tt>dnf upgrade --refresh</tt>.</i>"));

    auto *proc = new QProcess;
    proc->setProcessChannelMode(QProcess::MergedChannels);

    connect(proc, &QProcess::readyRead, this, &UpdatePage::onReadyRead);
    connect(proc, &QProcess::finished,  this, [this](int code, QProcess::ExitStatus st) {
        onFinished(code, st);

        // Desliga/conclui progress bar 
        m_progressBar->setRange(0, 100);
        m_progressBar->setValue(100);

        auto *snap2 = new QProcess;
        connect(snap2, &QProcess::finished, this, [this, snap2](int, QProcess::ExitStatus) {
            const QString kernelsAfter = QString::fromUtf8(snap2->readAllStandardOutput()).trimmed();
            snap2->deleteLater();

            if (kernelsAfter != m_kernelsBefore) {
                m_kernelBox->setVisible(true);
            } else {
                m_done = true;
                emit completeChanged();
            }
        });
        snap2->start("rpm", {"-q", "kernel"});
    });

    m_proc = proc;
    proc->start("dnf", {"-y", "upgrade", "--refresh"});
}

void UpdatePage::onReadyRead()
{
    if (!m_proc) return;
    const QString text = QString::fromUtf8(m_proc->readAll());
    m_log->moveCursor(QTextCursor::End);
    m_log->insertPlainText(text);
    m_log->verticalScrollBar()->setValue(m_log->verticalScrollBar()->maximum());
}

void UpdatePage::onFinished(int exitCode, QProcess::ExitStatus)
{
    if (exitCode == 0) {
        m_statusLabel->setText(tr("<span style='color:#3db03d;'>✓ Integrações validadas e Sistema Atualizado de ponta a ponta com sucesso.</span>"));
    } else {
        m_statusLabel->setText(
            QString(tr("<span style='color:#cc0000;'>✗ A atualização finalizou com um código anômalo estrutural da RedHat (%1). "
                       "Analise os logs no terminal virtual abaixo. Você ainda pode forçar a continuação se julgar seguro.</span>")).arg(exitCode)
        );
        m_done = true; // Liberamos para avançar (ignorar)
        emit completeChanged();
    }
}

void UpdatePage::reboot()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Confirmação de Reinício"),
        tr("Reiniciar o Servidor Gráfico e a Máquina agora?\n\n"
           "Garanta que trabalhos essenciais foram salvos em outras janelas antes de prosseguir. "
           "O assistente do Rapidora precisará ser lançado novamente desde o menu pelo seu usuário após reiniciar o sistema."),
        QMessageBox::Yes | QMessageBox::No
    );
    if (reply == QMessageBox::Yes) {
        QProcess::startDetached("systemctl", {"reboot"});
    }
}

bool UpdatePage::isComplete() const
{
    return m_done;
}
