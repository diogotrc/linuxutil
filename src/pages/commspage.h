#pragma once
#include <QLabel>
#include <QWizardPage>
#include <QCheckBox>
#include <QMap>
#include <QVBoxLayout>
#include <QGridLayout>

class MainWizard;

class CommsPage : public QWizardPage {
    Q_OBJECT
public:
    explicit CommsPage(MainWizard *wizard);
    void initializePage() override;
    bool validatePage() override;

private:
    void addToolCard(QLayout *layout, QString key, QString name, QString desc, bool installed, const QString &iconName, int row = -1, int col = -1);

    MainWizard               *m_wiz;
    QMap<QString, QCheckBox*> m_boxes;
    QMap<QString, QLabel*>    m_badges;
    QLabel *m_checkingLabel = nullptr;
};
