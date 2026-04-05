#pragma once
#include <QLabel>
#include <QWizardPage>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QProcess>
#include <QFrame>
#include <QMouseEvent>

class MainWizard;

class GpuCard : public QFrame {
    Q_OBJECT
public:
    GpuCard(const QString &id, const QString &title, const QString &desc, const QString &iconName, QWidget *parent = nullptr);
    
    QString id() const { return m_id; }
    bool isChecked() const { return m_checked; }
    void setChecked(bool checked);
    void addBadge(const QString &text);

signals:
    void clicked();
    void toggled(QString id);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void updateStyle();

    QString m_id;
    bool m_checked;
    QLabel *m_checkIcon;
    QVBoxLayout *m_vbox;
};

class GpuPage : public QWizardPage {
    Q_OBJECT
public:
    explicit GpuPage(MainWizard *wizard);
    void initializePage() override;
    bool validatePage() override;

private:
    void autoDetectGPU();
    void selectCard(const QString &id);

    MainWizard   *m_wiz = nullptr;
    
    GpuCard *m_cardAmd = nullptr;
    GpuCard *m_cardNvidia = nullptr;
    GpuCard *m_cardSkip = nullptr;
    
    QLabel *m_detectedLabel = nullptr;
};
