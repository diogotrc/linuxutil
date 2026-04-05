#pragma once
#include <QDialog>
#include <QString>

/**
 * @brief LanguageDialog — Tela de seleção de idioma Qt6 nativa.
 *
 * Exibida uma única vez antes do wizard principal. O idioma selecionado
 * é armazenado em m_selectedLang e pode ser consultado após o exec().
 */
class LanguageDialog : public QDialog {
    Q_OBJECT
public:
    explicit LanguageDialog(QWidget *parent = nullptr);

    /** Retorna o código de idioma selecionado (ex: "pt_BR", "en_US"). */
    QString selectedLang() const { return m_selectedLang; }

private:
    void buildUi();
    void selectLanguage(const QString &lang);

    QString m_selectedLang = QStringLiteral("pt_BR");
};
