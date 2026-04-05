#pragma once
#include <QWizardPage>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QPointer>
#include <QProcess>
#include <QProgressBar>

class MainWizard;

class UpdatePage : public QWizardPage
{
    Q_OBJECT
public:
    explicit UpdatePage(MainWizard *wizard);
    ~UpdatePage() override;
    void initializePage() override;
    bool isComplete() const override;

private slots:
    void runUpdate();
    void startDnfUpgrade();
    void onReadyRead();
    void onFinished(int exitCode, QProcess::ExitStatus status);
    void reboot();

private:
    // Finaliza com segurança os processos assíncronos
    void cleanupProc();

    MainWizard     *m_wiz;
    QPlainTextEdit *m_log         = nullptr;
    QProgressBar   *m_progressBar = nullptr;
    QLabel         *m_statusLabel = nullptr;
    QPushButton    *m_updateBtn   = nullptr;
    QPushButton    *m_skipBtn     = nullptr;
    QPushButton    *m_rebootBtn   = nullptr;
    QFrame         *m_kernelBox   = nullptr;

    // Gerencia o ponteiro para processo sem travar o teardown caso removido da tela
    QPointer<QProcess> m_proc;

    // Histórico de kernels para validar se atualizou algo crítico e pedir reboot
    QString            m_kernelsBefore;

    bool         m_done        = false;
    bool         m_skipped     = false;
};
