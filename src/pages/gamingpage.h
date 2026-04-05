#pragma once
#include <QWizardPage>
#include <QLabel>
#include <QCheckBox>
#include <QMap>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QLineEdit>

class MainWizard;

// The Collapsible Section class
class CollapsibleSection : public QWidget {
    Q_OBJECT
public:
    explicit CollapsibleSection(const QString &title, QWidget *parent = nullptr);
    QGridLayout* contentLayout() { return m_contentLayout; }
    void setExpanded(bool expand);
    bool matchesFilter(const QString &filter);

private:
    QFrame *m_header;
    QLabel *m_titleLabel;
    QLabel *m_toggleIcon;
    QFrame *m_contentContainer;
    QGridLayout *m_contentLayout;
    bool m_isExpanded = true;
};

class GamingPage : public QWizardPage {
    Q_OBJECT
public:
    explicit GamingPage(MainWizard *wizard);
    void initializePage() override;
    bool validatePage() override;

private:
    void addToolCard(CollapsibleSection *section, QString key, QString name, QString desc, bool installed, const QString &iconName, int row, int col);
    void filterCards(const QString &text);

    MainWizard               *m_wiz;
    QMap<QString, QCheckBox*> m_boxes;
    QMap<QString, QLabel*>    m_badges;
    QMap<QString, QFrame*>    m_cards;
    QList<CollapsibleSection*> m_sections;
    
    QLabel *m_checkingLabel = nullptr;
    QLineEdit *m_searchBar = nullptr;
};
