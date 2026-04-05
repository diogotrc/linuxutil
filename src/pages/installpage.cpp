#include <QAbstractButton>
#include <QColor>
#include <QThread>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include "installpage.h"
#include "../mainwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFrame>
#include <QFont>
#include <QScrollBar>

InstallPage::InstallPage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle(tr("Construindo o Sistema"));
    setSubTitle(tr("As operações automáticas foram delegadas aos Workers no background."));

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(15, 15, 15, 15);

    // ==========================================
    // Localização: Mensagem de Paciência
    // ==========================================
    auto *patienceFrame = new QFrame;
    patienceFrame->setFrameShape(QFrame::StyledPanel);
    patienceFrame->setStyleSheet("QFrame { background-color: rgba(53, 132, 228, 0.1); border: 1px solid rgba(53, 132, 228, 0.3); border-radius: 8px; }");
    
    auto *patienceLayout = new QHBoxLayout(patienceFrame);
    patienceLayout->setContentsMargins(15, 12, 15, 12);
    patienceLayout->setSpacing(12);

    auto *patienceIcon = new QLabel("☕");
    patienceIcon->setStyleSheet("font-size: 28px; background: transparent; border: none;");
    
    auto *textLayout = new QVBoxLayout;
    
    auto *patienceTitle = new QLabel(tr("<b>Paciência é uma virtude! ⏳</b>"));
    patienceTitle->setStyleSheet("color: #8cbcf8; font-size: 15px; background: transparent; border: none;");

    auto *patienceLabel = new QLabel(tr(
        "Alguns pacotes podem demorar para baixar e instalar devido a espelhos gringos. "
        "Se parecer que travou, <b>provavelmente não travou</b>. O rastreador ainda roda na Engine C++ em background.<br>"
        "Seja paciente — ou vá tomar um café enquanto montamos sua fortaleza!"
    ));
    patienceLabel->setWordWrap(true);
    patienceLabel->setStyleSheet("color: #b0b0b0; font-size: 13px; background: transparent; border: none;");
    
    textLayout->addWidget(patienceTitle);
    textLayout->addWidget(patienceLabel);
    
    patienceLayout->addWidget(patienceIcon);
    patienceLayout->addLayout(textLayout, 1);
    
    layout->addWidget(patienceFrame);

    // ==========================================
    // Status e Toggle Vibecoding
    // ==========================================
    auto *statusHeaderLayout = new QHBoxLayout;
    
    m_statusLabel = new QLabel(tr("Aguardando inicialização do despachante..."));
    m_statusLabel->setStyleSheet("color: #ffffff; font-weight: bold; font-size: 14px;");
    
    m_toggleViewBtn = new QPushButton(tr("👁 Esconder Metadados (Modo Compacto)"));
    m_toggleViewBtn->setCursor(Qt::PointingHandCursor);
    m_toggleViewBtn->setStyleSheet("QPushButton { background-color: rgba(255,255,255,0.05); color: #8cbcf8; font-weight: bold; padding: 6px 12px; border-radius: 6px; border: 1px solid rgba(53, 132, 228, 0.4); } QPushButton:hover { background-color: rgba(53, 132, 228, 0.2); border-color: #3584e4; color: white;}");
    connect(m_toggleViewBtn, &QPushButton::clicked, this, &InstallPage::toggleCompactMode);

    statusHeaderLayout->addWidget(m_statusLabel);
    statusHeaderLayout->addStretch();
    statusHeaderLayout->addWidget(m_toggleViewBtn);

    layout->addLayout(statusHeaderLayout);

    // ==========================================
    // Progress Bar Personalizada
    // ==========================================
    m_progress = new QProgressBar;
    m_progress->setRange(0, 100);
    m_progress->setValue(0);
    // Design vibrante 20px de grossura e cantos redondos contínuos
    m_progress->setStyleSheet(
        "QProgressBar { background-color: rgba(255, 255, 255, 0.05); border: 1px solid rgba(255,255,255,0.1); border-radius: 8px; text-align: center; color: white; font-weight: bold; min-height: 22px; font-size: 13px; }"
        "QProgressBar::chunk { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2bba2b, stop:1 #4cd14c); border-radius: 8px; }"
    );
    layout->addWidget(m_progress);

    // ==========================================
    // Painel Duplo - GUI Async
    // ==========================================
    m_mainSplitter = new QSplitter(Qt::Horizontal);

    // Lista Esquerda
    m_stepList = new QListWidget;
    m_stepList->setMinimumWidth(280);
    m_stepList->setMaximumWidth(400);
    m_stepList->setFocusPolicy(Qt::NoFocus);
    m_stepList->setStyleSheet(
        "QListWidget { background-color: rgba(255,255,255,0.02); border: 1px solid rgba(255,255,255,0.06); border-radius: 10px; outline: none; padding: 6px; }"
        "QListWidget::item { padding: 8px; border-radius: 6px; margin-bottom: 3px; color: #bbbbbb; }"
        "QListWidget::item:selected { background-color: rgba(53, 132, 228, 0.25); color: white; font-weight: bold; border: 1px solid rgba(53,132,228,0.5); }"
        "QListWidget::item:hover { background-color: rgba(255,255,255,0.05); }"
    );
    m_mainSplitter->addWidget(m_stepList);

    // Log Direita
    m_stepDetail = new QPlainTextEdit;
    m_stepDetail->setReadOnly(true);
    QFont mono("Monospace");
    mono.setStyleHint(QFont::TypeWriter);
    mono.setPointSize(10);
    m_stepDetail->setFont(mono);
    // Coletor em background puro (preto 1a1a1a simulando Terminal Real)
    m_stepDetail->setStyleSheet("QPlainTextEdit { background-color: #111111; color: #a5d6a7; border: 1px solid rgba(255,255,255,0.06); border-radius: 10px; padding: 12px; }");
    m_stepDetail->setPlaceholderText(tr("O fluxo STDOUT dos contêineres e logs DNF será alimentado em tempo-real aqui.\nClique em uma Categoria à esquerda para auditar..."));
    
    // Captura Cliques brutos no QTextEdit
    m_stepDetail->viewport()->installEventFilter(this);

    m_mainSplitter->addWidget(m_stepDetail);

    // Pesos Mágicos p/ Distribuição
    m_mainSplitter->setStretchFactor(0, 1);
    m_mainSplitter->setStretchFactor(1, 4);

    layout->addWidget(m_mainSplitter, 1);

    connect(m_stepList, &QListWidget::itemClicked, this, &InstallPage::onStepClicked);
}

InstallPage::~InstallPage() { shutdownWorkerSync(); }

bool InstallPage::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_stepDetail->viewport() && event->type() == QEvent::MouseButtonRelease) {
        // Interatividade Expand Log
        emit m_toggleViewBtn->clicked();
        return true;
    }
    return QWizardPage::eventFilter(obj, event);
}

void InstallPage::toggleCompactMode() {
    m_compactMode = !m_compactMode;
    m_mainSplitter->setVisible(!m_compactMode);
    
    if (m_compactMode) {
        m_toggleViewBtn->setText(tr("👁 Exibir Interações (Modo Completo)"));
        m_toggleViewBtn->setStyleSheet("QPushButton { background-color: rgba(43, 122, 66, 0.4); color: white; font-weight: bold; padding: 6px 12px; border-radius: 6px; border: 1px solid rgba(43, 122, 66, 0.8); } QPushButton:hover { background-color: rgba(43, 122, 66, 0.6); }");
    } else {
        m_toggleViewBtn->setText(tr("👁 Esconder Metadados (Modo Compacto)"));
        m_toggleViewBtn->setStyleSheet("QPushButton { background-color: rgba(255,255,255,0.05); color: #8cbcf8; font-weight: bold; padding: 6px 12px; border-radius: 6px; border: 1px solid rgba(53, 132, 228, 0.4); } QPushButton:hover { background-color: rgba(53, 132, 228, 0.2); border-color: #3584e4; color: white;}");
    }
}

void InstallPage::shutdownWorkerSync()
{
    if (m_worker) {
        m_worker->cancel();
    }
    if (m_thread) {
        m_thread->quit();
        m_thread->wait(); 
    }
}

void InstallPage::initializePage()
{
    wizard()->button(QWizard::BackButton)->setEnabled(false);
    wizard()->button(QWizard::NextButton)->setEnabled(false);

    if (m_worker) m_worker->cancel();
    if (m_thread) m_thread->quit();

    m_stepList->clear();
    m_stepDetail->clear();
    m_stepItems.clear();
    m_stepLogs.clear();
    m_done = false;
    m_doneSteps = 0;
    m_currentStepId.clear();

    QList<InstallStep> steps = m_wiz->buildSteps();
    m_totalSteps = steps.size();
    m_progress->setRange(0, m_totalSteps);
    m_progress->setValue(0);

    for (const InstallStep &step : steps) {
        auto *item = new QListWidgetItem("⏳  " + step.description);
        item->setData(Qt::UserRole, step.id);
        item->setData(Qt::UserRole + 1, step.description); // Pbackup limpo da string crua!
        m_stepList->addItem(item);
        m_stepItems[step.id] = item;
        m_stepLogs[step.id]  = QString();
    }

    auto *thread = new QThread;
    auto *worker = new InstallWorker;
    worker->setSteps(steps);
    worker->moveToThread(thread);

    connect(thread, &QThread::finished,           worker, &QObject::deleteLater);
    connect(thread, &QThread::finished,           thread, &QThread::deleteLater);

    connect(thread, &QThread::started,            worker, &InstallWorker::run);
    connect(worker, &InstallWorker::stepStarted,  this,   &InstallPage::onStepStarted);
    connect(worker, &InstallWorker::stepFinished, this,   &InstallPage::onStepFinished);
    connect(worker, &InstallWorker::stepSkipped,  this,   &InstallPage::onStepSkipped);
    connect(worker, &InstallWorker::logLine,      this,   &InstallPage::onLogLine);
    connect(worker, &InstallWorker::allDone,      this,   &InstallPage::onAllDone);
    connect(worker, &InstallWorker::allDone,      thread, &QThread::quit);

    m_thread = thread;
    m_worker = worker;

    thread->start();
}

void InstallPage::onStepStarted(const QString &id, const QString &description)
{
    m_currentStepId = id;
    m_statusLabel->setText(tr("🟢 Aterrisando: ") + description + " ...");
    
    if (m_stepItems.contains(id)) {
        // Play na seta azul
        m_stepItems[id]->setText("▶  " + m_stepItems[id]->data(Qt::UserRole + 1).toString());
        m_stepItems[id]->setForeground(QColor(53, 132, 228)); 
        m_stepList->setCurrentItem(m_stepItems[id]); // Auto-follow view!
        m_stepList->scrollToItem(m_stepItems[id]);
    }
}

void InstallPage::onStepFinished(const QString &id, bool success, int exitCode)
{
    m_doneSteps++;
    m_progress->setValue(m_doneSteps);

    if (m_stepItems.contains(id)) {
        m_stepItems[id]->setText((success ? "✅  " : "❌  ") + m_stepItems[id]->data(Qt::UserRole + 1).toString());
        m_stepItems[id]->setForeground(success ? QColor(76, 209, 76) : QColor(255, 80, 80));
    }

    if (m_stepLogs.contains(id))
        m_stepLogs[id] += success
            ? QString("\n[Sistema: Exit 0 - Camada Acoplada com Sucesso]")
            : QString("\n[Sistema: Exit %1 - FALHA DE ATERRISAGEM]").arg(exitCode);

    QListWidgetItem *sel = m_stepList->currentItem();
    if (sel && sel->data(Qt::UserRole).toString() == id)
        onStepClicked(sel);
}

void InstallPage::onStepSkipped(const QString &id, const QString &description)
{
    Q_UNUSED(description);
    m_doneSteps++;
    m_progress->setValue(m_doneSteps);

    if (m_stepItems.contains(id)) {
        m_stepItems[id]->setText("⏭️  " + m_stepItems[id]->data(Qt::UserRole + 1).toString());
        m_stepItems[id]->setForeground(QColor(130, 130, 160));
    }
    if (m_stepLogs.contains(id))
        m_stepLogs[id] += "[Camada bypassada por já existir pre-instalada localmente.]\n";
}

void InstallPage::onLogLine(const QString &line)
{
    if (!m_currentStepId.isEmpty() && m_stepLogs.contains(m_currentStepId))
        m_stepLogs[m_currentStepId] += line + "\n";

    QListWidgetItem *sel = m_stepList->currentItem();
    if (sel && sel->data(Qt::UserRole).toString() == m_currentStepId) {
        m_stepDetail->setPlainText(m_stepLogs[m_currentStepId]);
        m_stepDetail->verticalScrollBar()->setValue(m_stepDetail->verticalScrollBar()->maximum());
    }
}

void InstallPage::onStepClicked(QListWidgetItem *item)
{
    if (!item) return;
    const QString id = item->data(Qt::UserRole).toString();

    if (m_stepLogs.contains(id) && !m_stepLogs[id].isEmpty()) {
        m_stepDetail->setPlainText(m_stepLogs[id]);
        m_stepDetail->verticalScrollBar()->setValue(m_stepDetail->verticalScrollBar()->maximum());
    } else {
        m_stepDetail->setPlainText(tr("(O console DNF emitirá a stream aqui assim que o passo iniciar...)"));
    }
}

void InstallPage::onAllDone(int errorCount)
{
    m_done = true;

    QStringList failedSteps;
    for (auto it = m_stepItems.constBegin(); it != m_stepItems.constEnd(); ++it) {
        if (it.value()->text().startsWith("❌")) {
            const QString id   = it.key();
            const QString desc = it.value()->data(Qt::UserRole + 1).toString();
            failedSteps << QString("Step: %1\n%2").arg(desc).arg(m_stepLogs.value(id).trimmed());
        }
    }
    
    // Coleta o Log global reconstruindo todos os passos em uma trilha (para repassar a página Final)
    QString globalLog;
    for (int i=0; i < m_stepList->count(); i++) {
        QString cId = m_stepList->item(i)->data(Qt::UserRole).toString();
        globalLog += QString("=== %1 ===\n%2\n\n").arg(m_stepList->item(i)->data(Qt::UserRole+1).toString()).arg(m_stepLogs[cId]);
    }

    m_wiz->setOpt("install/errorCount",  errorCount);
    m_wiz->setOpt("install/failedSteps", failedSteps.join("\n\n---\n\n"));
    m_wiz->setOpt("install/fullLog",     globalLog);

    if (errorCount == 0) {
        m_statusLabel->setText(tr("🏁 Construção Concluída — Nenhuma advertência pendente."));
        m_statusLabel->setStyleSheet("color: #4cd14c; font-weight: bold; font-size: 14px;");
    } else {
        m_statusLabel->setText(QString(tr("⚠️ Construção Concluída: Detectados %1 erro(s). Reveja o Log.")).arg(errorCount));
        m_statusLabel->setStyleSheet("color: #d14c4c; font-weight: bold; font-size: 14px;");
    }

    // Retorna NextButton e traduz para Finalizar! (O Wizard framework faz os botões em inglês base, vamos transpor localmente).
    wizard()->button(QWizard::NextButton)->setText(tr("Finalizar >>"));
    wizard()->button(QWizard::NextButton)->setEnabled(true);
    
    emit completeChanged();
}
bool InstallPage::isComplete() const { return m_done; }
