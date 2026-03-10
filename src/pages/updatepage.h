#pragma once
#include <QWizardPage>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QPointer>
#include <QProcess>

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
    // Kills and deletes any running m_proc. Safe to call if m_proc is null.
    void cleanupProc();

    MainWizard  *m_wiz;
    QTextEdit   *m_log         = nullptr;
    QLabel      *m_statusLabel = nullptr;
    QPushButton *m_updateBtn   = nullptr;
    QPushButton *m_skipBtn     = nullptr;
    QPushButton *m_rebootBtn   = nullptr;
    QFrame      *m_kernelBox   = nullptr;

    // QPointer so stale-pointer checks are safe after async deletion.
    // m_proc has no parent — lifetime managed via cleanupProc() and deleteLater().
    QPointer<QProcess> m_proc;

    // Kernel list snapshot taken before dnf upgrade, populated asynchronously.
    // Compared against a post-upgrade snapshot to detect new kernel installs.
    QString            m_kernelsBefore;

    bool         m_done        = false;
    bool         m_skipped     = false;
};
