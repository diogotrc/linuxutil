#pragma once
#include <QWizardPage>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>

class MainWizard;

class ReviewPage : public QWizardPage {
    Q_OBJECT
public:
    explicit ReviewPage(MainWizard *wizard);
    void initializePage() override;
    bool isComplete() const override;

private slots:
    void onProceedAnyway();

private:
    MainWizard  *m_wiz;
    QProgressBar *m_diskBar;
    QLabel      *m_diskLabel;
    QLabel      *m_targetBadge;
    QPushButton *m_proceedBtn;
    QVBoxLayout *m_timelineLayout;
    QWidget     *m_emptyStateWidget;
    bool         m_diskOk       = true;
    bool         m_proceedForced = false;
};
