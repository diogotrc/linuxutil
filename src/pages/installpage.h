#pragma once
#include <QWizardPage>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QLabel>
#include <QThread>
#include <QPointer>
#include <QMap>
#include <QSplitter>
#include <QFrame>
#include <QPushButton>
#include <QEvent>
#include "../installworker.h"

class MainWizard;

class InstallPage : public QWizardPage {
    Q_OBJECT
public:
    explicit InstallPage(MainWizard *wizard);
    ~InstallPage() override;

    void initializePage() override;
    bool isComplete()     const override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onStepStarted(const QString &id, const QString &description);
    void onStepFinished(const QString &id, bool success, int exitCode);
    void onStepSkipped(const QString &id, const QString &description);
    void onLogLine(const QString &line);
    void onAllDone(int errorCount);
    void onStepClicked(QListWidgetItem *item);
    void toggleCompactMode();

private:
    void shutdownWorkerSync();

    MainWizard     *m_wiz;
    QListWidget    *m_stepList        = nullptr;
    QPlainTextEdit *m_stepDetail      = nullptr;
    QProgressBar   *m_progress        = nullptr;
    QLabel         *m_statusLabel     = nullptr;
    QSplitter      *m_mainSplitter    = nullptr;
    QWidget        *m_logContainer    = nullptr;
    QPushButton    *m_toggleViewBtn   = nullptr;

    QPointer<QThread>        m_thread;
    QPointer<InstallWorker>  m_worker;

    bool            m_done            = false;
    bool            m_compactMode     = false;
    int             m_totalSteps      = 0;
    int             m_doneSteps       = 0;

    QString         m_currentStepId;
    QMap<QString, QListWidgetItem*> m_stepItems;
    QMap<QString, QString>          m_stepLogs;
};
