#pragma once
#include <QWizardPage>
#include <QFrame>

class MainWizard; // Forward declaration

/**
 * @brief WelcomePage - A primeira tela (após linguagens) informacional do Rapidora
 * 
 * Herda de QWizardPage para fácil integração com o QWizard principal do app.
 * Exibe um visual moderno com cards arredondados e explica a mecânica do SO.
 */
class WelcomePage : public QWizardPage {
    Q_OBJECT
public:
    explicit WelcomePage(MainWizard *wizard);
    
    // Método que garante que a página só pode avançar se certas condições forem atendidas (ex: sudo)
    bool isComplete() const override;
    
    // Hook chamado ao abrir a página - bom para ajustes do QWizard em tempo real
    void initializePage() override;

private:
    // Métodos para extração de informação solicitados no requisito:
    QString getSystemUser() const;
    QString getFedoraVersion() const;
    
    // Sub-componentes modulares de UI (Cards)
    QFrame* createSystemInfoCard();
    QFrame* createInstructionsCard();

    MainWizard* m_wizard; // Referência ao wizard principal para acesso global
};
