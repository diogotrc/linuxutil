#include <QApplication>
#include <QFont>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include "mainwizard.h"
#include "languagedialog.h"
#include "translator.h"
#include <unistd.h>
#include <pwd.h>
#include <cstdlib>
#include <cerrno>
#include <climits>

int main(int argc, char *argv[])
{
    // Restore the real user's environment before QApplication is created,
    // so the platform theme (KDE/Breeze) loads correctly.
    // pkexec sets PKEXEC_UID; sudo -E sets SUDO_USER.
    struct passwd *pw = nullptr;

    const char *pkexecUid = std::getenv("PKEXEC_UID");
    const char *sudoUser  = std::getenv("SUDO_USER");

    if (pkexecUid && geteuid() == 0) {
        // Validate PKEXEC_UID before use. atoi() is unsafe — a non-numeric or
        // empty value returns 0, which resolves to root via getpwuid(0).
        // strtoul with explicit error checking prevents that.
        char *endptr = nullptr;
        errno = 0;
        const unsigned long rawUid = std::strtoul(pkexecUid, &endptr, 10);
        const bool validUid = (errno == 0 && endptr != pkexecUid && *endptr == '\0'
                               && rawUid <= static_cast<unsigned long>(UINT_MAX));
        if (validUid) {
            pw = getpwuid(static_cast<uid_t>(rawUid));
        }
    } else if (sudoUser && geteuid() == 0) {
        pw = getpwnam(sudoUser);
    }

    if (pw) {
        uid_t uid = pw->pw_uid;
        qputenv("HOME",                 pw->pw_dir);
        qputenv("XDG_RUNTIME_DIR",      QString("/run/user/%1").arg(uid).toUtf8());
        qputenv("DBUS_SESSION_BUS_ADDRESS",
                 QString("unix:path=/run/user/%1/bus").arg(uid).toUtf8());
        qputenv("XDG_CONFIG_HOME",
                 QString("%1/.config").arg(pw->pw_dir).toUtf8());
        qputenv("XDG_DATA_HOME",
                 QString("%1/.local/share").arg(pw->pw_dir).toUtf8());
    }

    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME"))
        qputenv("QT_QPA_PLATFORMTHEME", "kde");

    QApplication app(argc, argv);
    app.setApplicationName("Rapidora");
    app.setApplicationVersion("1.0.6");
    app.setOrganizationName("LinuxGamerLife");
    app.setWindowIcon(QIcon::fromTheme("rapidora", QIcon("/usr/share/pixmaps/rapidora.png")));

    // Increase base font size by 2pt for readability
    QFont appFont = app.font();
    appFont.setPointSize(appFont.pointSize() + 2);
    app.setFont(appFont);

    // Load global QSS stylesheet — search in standard locations
    {
        const QStringList candidates = {
            QDir::currentPath()        + "/src/style.qss",           // dev build
            QApplication::applicationDirPath() + "/style.qss",       // beside binary
            "/usr/share/rapidora/style.qss",                          // installed
        };
        for (const QString &path : candidates) {
            QFile f(path);
            if (f.open(QFile::ReadOnly | QFile::Text)) {
                app.setStyleSheet(QString::fromUtf8(f.readAll()));
                break;
            }
        }
    }

    if (geteuid() != 0) {
        QMessageBox::warning(
            nullptr,
            "Administrator privileges required",
            "Rapidora requires administrator privileges to install packages.\n\n"
            "Please launch it from your application menu, or run:\n"
            "  pkexec rapidora"
        );
    }

    // Show language selection dialog before the main wizard
    LanguageDialog langDlg;
    if (langDlg.exec() != QDialog::Accepted)
        return 0;
    
    RapidoraTranslator *translator = new RapidoraTranslator(langDlg.selectedLang(), &app);
    app.installTranslator(translator);

    MainWizard wizard;
    wizard.show();

    return app.exec();
}
