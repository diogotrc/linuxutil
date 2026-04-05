#ifndef RAPIDORA_LANGUAGE_SELECTION_WINDOW_HPP
#define RAPIDORA_LANGUAGE_SELECTION_WINDOW_HPP

#include <gtkmm.h>
#include <string>

class LanguageSelectionWindow : public Gtk::Window {
public:
    LanguageSelectionWindow();
    virtual ~LanguageSelectionWindow() = default;

private:
    // Configuração de Interface
    void setup_ui();
    void set_dark_mode();
    
    // Atualização de Textos (i18n)
    void update_ui_text();
    
    // Handlers de sinais
    void on_language_selected(const std::string& lang_code);
    void on_next_clicked();
    void on_cancel_clicked();

    // Helper para criar os botões/cards de idioma
    Gtk::Button* create_language_card(const Glib::ustring& name, const Glib::ustring& emoji, const std::string& lang_code);

    // Contêineres Principais
    Gtk::Box m_main_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::Box m_cards_box{Gtk::Orientation::HORIZONTAL, 20};
    Gtk::Box m_action_box{Gtk::Orientation::HORIZONTAL, 10};

    // Widgets de Interface
    Gtk::Label m_title_label;
    Gtk::Label m_branding_label;
    Gtk::Button m_btn_next;
    Gtk::Button m_btn_cancel;

    // Estado Global (Exemplo)
    std::string m_selected_lang;
};

#endif // RAPIDORA_LANGUAGE_SELECTION_WINDOW_HPP
