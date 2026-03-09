#pragma once
#include <QWizardPage>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QProcess>

class MainWizard;

class UpdatePage : public QWizardPage
{
    Q_OBJECT
public:
    explicit UpdatePage(MainWizard *wizard);
    void initializePage() override;
    bool isComplete() const override;

private slots:
    void runUpdate();
    void onReadyRead();
    void onFinished(int exitCode, QProcess::ExitStatus status);
    void reboot();

private:
    MainWizard  *m_wiz;
    QTextEdit   *m_log        = nullptr;
    QLabel      *m_statusLabel= nullptr;
    QPushButton *m_updateBtn  = nullptr;
    QPushButton *m_skipBtn    = nullptr;
    QPushButton *m_rebootBtn  = nullptr;
    QFrame      *m_kernelBox  = nullptr;
    QProcess    *m_proc       = nullptr;
    bool         m_done       = false;
    bool         m_skipped    = false;

    bool kernelWasUpdated();
};
