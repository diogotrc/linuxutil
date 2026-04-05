#pragma once
#include <QLabel>
#include <QWizardPage>
#include <QCheckBox>
#include <QMap>
#include <QVBoxLayout>

class MainWizard;

class MultimediaPage : public QWizardPage {
    Q_OBJECT
public:
    explicit MultimediaPage(MainWizard *wizard);
    void initializePage() override;
    bool validatePage() override;

private:
    void addToolCard(QString key, QString name, QString desc, bool installed, const QString &iconName);

    MainWizard               *m_wiz;
    QMap<QString, QCheckBox*> m_boxes;
    QMap<QString, QLabel*>    m_badges;
    QLabel *m_checkingLabel = nullptr;
    QVBoxLayout *m_cardsLayout = nullptr;
};
