#include "LanguageSelectionWindow.hpp"
#include <libintl.h>
#include <locale.h>
#include <iostream>

#define _(String) gettext (String)

LanguageSelectionWindow::LanguageSelectionWindow() {
    set_title("Rapidora Setup");
    set_default_size(650, 450);
    set_resizable(false);

    set_dark_mode();
    setup_ui();
    
    // Default language fallback (en_US)
    m_selected_lang = "en_US";
    update_ui_text();
}

void LanguageSelectionWindow::set_dark_mode() {
    auto settings = Gtk::Settings::get_default();
    if (settings) {
        settings->property_gtk_application_prefer_dark_theme() = true;
    }
}

void LanguageSelectionWindow::setup_ui() {
    // Main Container Styling
    set_child(m_main_box);
    m_main_box.set_margin_start(40);
    m_main_box.set_margin_end(40);
    m_main_box.set_margin_top(40);
    m_main_box.set_margin_bottom(30);
    m_main_box.set_spacing(30);

    // Title label
    m_title_label.set_halign(Gtk::Align::CENTER);
    m_title_label.set_justify(Gtk::Justification::CENTER);
    m_main_box.append(m_title_label);

    // Language Cards Section
    m_cards_box.set_halign(Gtk::Align::CENTER);
    m_cards_box.set_valign(Gtk::Align::CENTER);
    m_cards_box.set_vexpand(true); // Pushes branding and action boxes to the bottom
    
    auto en_card = create_language_card("English", "🇺🇸", "en_US");
    auto pt_card = create_language_card("Português", "🇧🇷", "pt_BR");
    
    m_cards_box.append(*en_card);
    m_cards_box.append(*pt_card);
    m_main_box.append(m_cards_box);

    // Branding Label
    m_branding_label.set_halign(Gtk::Align::CENTER);
    m_branding_label.add_css_class("dim-label"); // Built-in GTK class for fading labels
    m_main_box.append(m_branding_label);

    // Actions Bar (Next / Cancel)
    m_action_box.set_halign(Gtk::Align::END);
    m_action_box.set_margin_top(10);
    
    m_btn_cancel.signal_clicked().connect(sigc::mem_fun(*this, &LanguageSelectionWindow::on_cancel_clicked));
    m_action_box.append(m_btn_cancel);
    
    m_btn_next.add_css_class("suggested-action"); // Style for primary action, blue in adwaita
    m_btn_next.signal_clicked().connect(sigc::mem_fun(*this, &LanguageSelectionWindow::on_next_clicked));
    m_action_box.append(m_btn_next);

    m_main_box.append(m_action_box);
}

void LanguageSelectionWindow::update_ui_text() {
    // Dynamic text changing via i18n / gettext
    m_title_label.set_markup("<span size='xx-large' weight='bold'>" + 
                             Glib::ustring(_("Welcome to Rapidora\nPlease select your language")) + 
                             "</span>");
                             
    m_branding_label.set_text("Rapidora - An update to diogotrc/linuxutil");
    
    m_btn_cancel.set_label(_("Cancel"));
    m_btn_next.set_label(_("Next"));
}

Gtk::Button* LanguageSelectionWindow::create_language_card(const Glib::ustring& name, const Glib::ustring& emoji, const std::string& lang_code) {
    auto button = Gtk::make_managed<Gtk::Button>();
    
    // Internal Box to structure Emoji + Name
    auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 15);
    box->set_margin_start(30);
    box->set_margin_end(30);
    box->set_margin_top(25);
    box->set_margin_bottom(25);
    box->set_size_request(160, 160); // Squared aspect for "card" feeling
    box->set_halign(Gtk::Align::CENTER);
    box->set_valign(Gtk::Align::CENTER);

    auto emoji_label = Gtk::make_managed<Gtk::Label>();
    emoji_label->set_markup("<span size='35000'>" + emoji + "</span>"); // Large emoji
    
    auto name_label = Gtk::make_managed<Gtk::Label>();
    name_label->set_markup("<span weight='bold'>" + name + "</span>");
    
    box->append(*emoji_label);
    box->append(*name_label);
    
    button->set_child(*box);

    button->signal_clicked().connect([this, lang_code]() {
        on_language_selected(lang_code);
    });

    return button;
}

void LanguageSelectionWindow::on_language_selected(const std::string& lang_code) {
    m_selected_lang = lang_code;
    
    // Dynamic Locale Setting
    std::string locale_str = lang_code + ".UTF-8";
    setlocale(LC_ALL, locale_str.c_str());
    setenv("LANGUAGE", lang_code.c_str(), 1); // Forces LANGUAGE env for some gettext environments
    
    // Reload local domains for texts compilation 
    // Usually resides in /usr/share/locale/<domain>/LC_MESSAGES/rapidora.mo
    bindtextdomain("rapidora", "/usr/share/locale"); 
    textdomain("rapidora");

    // Instantly refresh UI components with new language
    update_ui_text();
}

void LanguageSelectionWindow::on_next_clicked() {
    std::cout << "[INFO] Proceeding to next screen with lang: " << m_selected_lang << std::endl;
    // Emitting a logic signal or instantiating your next component here!
}

void LanguageSelectionWindow::on_cancel_clicked() {
    std::cout << "[INFO] Setup aborted." << std::endl;
    hide(); // Default closing hook
}
