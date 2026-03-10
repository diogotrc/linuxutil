#include "updatepage.h"
#include "../mainwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QFrame>
#include <QProcess>
#include <QScrollBar>
#include <QApplication>
#include <QMessageBox>

UpdatePage::UpdatePage(MainWizard *wizard) : QWizardPage(wizard), m_wiz(wizard)
{
    setTitle("System Update");
    setSubTitle("It is strongly recommended to fully update your system before installing new software.");
}

UpdatePage::~UpdatePage()
{
    cleanupProc();
}

void UpdatePage::cleanupProc()
{
    if (!m_proc) return;
    // Disconnect all signals first to prevent onReadyRead/onFinished firing
    // against a partially-torn-down page state during cleanup.
    m_proc->disconnect();
    if (m_proc->state() != QProcess::NotRunning) {
        m_proc->kill();
        m_proc->waitForFinished(3000);
    }
    m_proc->deleteLater();
    // QPointer automatically becomes null after deleteLater fires.
    // We don't null it manually — QPointer handles that safely.
}

void UpdatePage::initializePage()
{
    // Clean up any previous run before rebuilding the layout.
    // This handles the back-navigation re-entry case.
    cleanupProc();
    m_done    = false;
    m_skipped = false;

    if (layout()) {
        QLayoutItem *i;
        while ((i = layout()->takeAt(0))) { if (i->widget()) i->widget()->deleteLater(); delete i; }
        delete layout();
    }

    auto *outer = new QVBoxLayout(this);
    outer->setSpacing(10);

    // Warning box
    auto *warnBox = new QFrame;
    warnBox->setFrameShape(QFrame::StyledPanel);
    auto *warnLayout = new QVBoxLayout(warnBox);
    auto *warnLabel = new QLabel(
        "<b>Why update first?</b><br>"
        "Installing software on top of an outdated system can cause package conflicts, "
        "dependency errors, and instability. Running a full update ensures your system "
        "is in a clean, consistent state before we add anything new.<br><br>"
        "Click <b>Update Now</b> to run <tt>dnf upgrade --refresh</tt>. "
        "This may take several minutes depending on your connection speed and how many updates are available."
    );
    warnLabel->setWordWrap(true);
    warnLayout->addWidget(warnLabel);
    outer->addWidget(warnBox);

    // Button row
    auto *btnWidget = new QWidget;
    auto *btnLayout = new QHBoxLayout(btnWidget);
    btnLayout->setContentsMargins(0, 0, 0, 0);

    m_updateBtn = new QPushButton("Update Now");
    m_updateBtn->setFixedHeight(34);
    m_skipBtn   = new QPushButton("Skip (not recommended)");
    m_skipBtn->setFixedHeight(34);

    connect(m_updateBtn, &QPushButton::clicked, this, &UpdatePage::runUpdate);
    connect(m_skipBtn,   &QPushButton::clicked, this, [this] {
        m_skipped = true;
        m_done    = true;
        m_updateBtn->setEnabled(false);
        m_skipBtn->setEnabled(false);
        m_statusLabel->setText("<span style='color:#cc7700;'>⚠ Update skipped. Proceed at your own risk.</span>");
        emit completeChanged();
    });

    btnLayout->addWidget(m_updateBtn);
    btnLayout->addWidget(m_skipBtn);
    btnLayout->addStretch();
    outer->addWidget(btnWidget);

    // Status label
    m_statusLabel = new QLabel;
    m_statusLabel->setWordWrap(true);
    outer->addWidget(m_statusLabel);

    // Kernel update warning box (hidden initially — shown above log when kernel update detected)
    m_kernelBox = new QFrame;
    m_kernelBox->setFrameShape(QFrame::StyledPanel);
    m_kernelBox->setStyleSheet("QFrame { border: 2px solid #cc7700; border-radius: 4px; }");
    auto *kernelLayout = new QVBoxLayout(m_kernelBox);
    auto *kernelLabel = new QLabel(
        "<b>⚠ Kernel update detected</b><br><br>"
        "A new kernel was installed as part of this update. It is strongly recommended to reboot "
        "before continuing, so that the new kernel is loaded and all modules are in sync.<br><br>"
        "Please reboot now and rerun <b>LGL System Loadout</b> once you are back."
    );
    kernelLabel->setWordWrap(true);
    kernelLayout->addWidget(kernelLabel);

    m_rebootBtn = new QPushButton("Reboot Now");
    m_rebootBtn->setFixedHeight(34);
    connect(m_rebootBtn, &QPushButton::clicked, this, &UpdatePage::reboot);

    auto *kernelBtnLayout = new QHBoxLayout;
    kernelBtnLayout->addWidget(m_rebootBtn);
    auto *continueAnywayBtn = new QPushButton("Continue Anyway");
    continueAnywayBtn->setFixedHeight(34);
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

    // Log output (stretch=1 so it fills the remaining space below the kernel box)
    m_log = new QTextEdit;
    m_log->setReadOnly(true);
    m_log->setFont(QFont("monospace"));
    m_log->setMinimumHeight(200);
    outer->addWidget(m_log, 1);
}

void UpdatePage::runUpdate()
{
    // Guard against double invocation (e.g. button clicked twice quickly).
    if (m_proc && m_proc->state() != QProcess::NotRunning) return;

    m_updateBtn->setEnabled(false);
    m_skipBtn->setEnabled(false);
    m_log->clear();
    m_statusLabel->setText("<i>Checking current kernel list...</i>");
    m_kernelsBefore.clear();

    // Take the before-snapshot asynchronously so we never block the UI thread.
    // The actual dnf upgrade is started from the snapshot's finished handler.
    // snap has no parent — it self-deletes via finished -> deleteLater.
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
    m_statusLabel->setText("<i>Running dnf upgrade --refresh ...</i>");

    // QProcess has NO parent — its lifetime is managed by cleanupProc() and
    // the finished -> deleteLater connection. A parented QProcess would be
    // destroyed by Qt's layout teardown at unpredictable times.
    auto *proc = new QProcess;
    proc->setProcessChannelMode(QProcess::MergedChannels);

    connect(proc, &QProcess::readyRead, this, &UpdatePage::onReadyRead);
    connect(proc, &QProcess::finished,  this, [this](int code, QProcess::ExitStatus st) {
        onFinished(code, st);

        // Take the after-snapshot asynchronously — never block inside a signal handler.
        // snap2 has no parent — self-deletes via finished -> deleteLater.
        auto *snap2 = new QProcess;
        connect(snap2, &QProcess::finished, this, [this, snap2](int, QProcess::ExitStatus) {
            const QString kernelsAfter = QString::fromUtf8(snap2->readAllStandardOutput()).trimmed();
            snap2->deleteLater();

            if (kernelsAfter != m_kernelsBefore) {
                m_kernelBox->setVisible(true);
                // Don't set m_done yet — user must choose reboot or continue
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
        m_statusLabel->setText("<span style='color:#3db03d;'>✓ System update complete.</span>");
    } else {
        m_statusLabel->setText(
            QString("<span style='color:#cc0000;'>✗ Update finished with exit code %1. "
                    "Check the log above. You can still continue.</span>").arg(exitCode)
        );
        m_done = true;
        emit completeChanged();
    }
}

void UpdatePage::reboot()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Reboot",
        "Reboot now?\n\n"
        "Make sure you have saved any open files or work in other applications before rebooting. "
        "LGL System Loadout will need to be run again after the reboot.",
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
