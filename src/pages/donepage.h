#pragma once
#include <QWizardPage>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QVBoxLayout>

class MainWizard;

class DonePage : public QWizardPage {
    Q_OBJECT
public:
    explicit DonePage(MainWizard *wizard);
    void initializePage() override;

private slots:
    void copyErrorsToClipboard();
    void copyFullLogToClipboard();
    void animateIn();

private:
    bool isRebootRequired() const;
    void buildInstalledBadges(QVBoxLayout *targetLayout);

    MainWizard     *m_wiz;

    // Hero section
    QLabel         *m_heroIcon        = nullptr;
    QLabel         *m_heroTitle       = nullptr;
    QLabel         *m_heroSubtitle    = nullptr;
    QFrame         *m_badgesContainer = nullptr;
    QVBoxLayout    *m_badgesLayout    = nullptr;

    // Status / disk bar
    QLabel         *m_statusBadge     = nullptr;

    // Reboot recommendation
    QFrame         *m_rebootFrame     = nullptr;

    // Failed steps section
    QFrame         *m_errorFrame      = nullptr;
    QPlainTextEdit *m_errorDetail     = nullptr;
    QPushButton    *m_copyErrorsBtn   = nullptr;

    // Action buttons
    QPushButton    *m_rebootBtn       = nullptr;
    QPushButton    *m_doneBtn         = nullptr;
    QPushButton    *m_copyLogBtn      = nullptr;

    // Entrance animation
    QGraphicsOpacityEffect *m_opacityEffect = nullptr;
};
