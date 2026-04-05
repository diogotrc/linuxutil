#include "languagedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QFont>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

LanguageDialog::LanguageDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(tr("Rapidora — Selecione o Idioma"));
    setFixedSize(560, 380);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    buildUi();
}

void LanguageDialog::buildUi()
{
    // ── Outer container com visual dark-card ────────────────────────────
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(20, 20, 20, 20);

    auto *card = new QFrame(this);
    card->setObjectName("LangCard");
    card->setStyleSheet(
        "QFrame#LangCard {"
        "  background-color: #1c1f26;"
        "  border: 1px solid rgba(255,255,255,0.10);"
        "  border-radius: 18px;"
        "}"
    );
    auto *glow = new QGraphicsDropShadowEffect(card);
    glow->setBlurRadius(50);
    glow->setColor(QColor(53, 132, 228, 80));
    glow->setOffset(0, 8);
    card->setGraphicsEffect(glow);

    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 36, 40, 32);
    cardLayout->setSpacing(28);
    outerLayout->addWidget(card);

    // ── Logo / Título ──────────────────────────────────────────────────
    auto *titleLabel = new QLabel(
        tr("<span style='font-size:28px; font-weight:700; color:#ffffff;'>Rapidora</span>"
           "<br/><span style='font-size:14px; color:#8899aa;'>Selecione o idioma para continuar</span>")
    );
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setTextFormat(Qt::RichText);
    cardLayout->addWidget(titleLabel);

    // ── Botões de Idioma ───────────────────────────────────────────────
    auto *btnRow = new QHBoxLayout;
    btnRow->setSpacing(20);

    auto makeLangBtn = [&](const QString &emoji, const QString &name, const QString &sub, const QString &code) {
        auto *btn = new QPushButton;
        btn->setCursor(Qt::PointingHandCursor);
        btn->setCheckable(true);
        btn->setFixedSize(200, 120);

        // Internal layout via rich text label inside the button is not ideal;
        // use a QFrame-overlay trick: set button text to empty and add child widget.
        auto *inner = new QVBoxLayout(btn);
        inner->setContentsMargins(0, 12, 0, 12);
        inner->setSpacing(4);

        auto *emojiLbl = new QLabel(emoji);
        emojiLbl->setAlignment(Qt::AlignCenter);
        emojiLbl->setStyleSheet("font-size: 32px; background: transparent; border: none;");
        emojiLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

        auto *nameLbl = new QLabel(QString("<b style='font-size:16px;'>%1</b>").arg(name));
        nameLbl->setAlignment(Qt::AlignCenter);
        nameLbl->setTextFormat(Qt::RichText);
        nameLbl->setStyleSheet("color: #ffffff; background: transparent; border: none;");
        nameLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

        auto *subLbl = new QLabel(sub);
        subLbl->setAlignment(Qt::AlignCenter);
        subLbl->setStyleSheet("font-size: 11px; color: #778899; background: transparent; border: none;");
        subLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

        inner->addWidget(emojiLbl);
        inner->addWidget(nameLbl);
        inner->addWidget(subLbl);

        const QString baseStyle =
            "QPushButton {"
            "  background-color: rgba(255,255,255,0.04);"
            "  border: 1.5px solid rgba(255,255,255,0.10);"
            "  border-radius: 14px;"
            "}"
            "QPushButton:hover {"
            "  background-color: rgba(255,255,255,0.10);"
            "  border-color: rgba(255,255,255,0.25);"
            "}"
            "QPushButton:checked {"
            "  background-color: rgba(53, 132, 228, 0.20);"
            "  border: 2px solid #3584e4;"
            "}";
        btn->setStyleSheet(baseStyle);

        connect(btn, &QPushButton::clicked, this, [this, code, btn, btnRow]() {
            // Uncheck sibling buttons
            for (int i = 0; i < btnRow->count(); ++i) {
                if (auto *w = btnRow->itemAt(i)->widget())
                    if (auto *b = qobject_cast<QPushButton*>(w))
                        b->setChecked(false);
            }
            btn->setChecked(true);
            selectLanguage(code);
        });

        // Default selection
        if (code == m_selectedLang) btn->setChecked(true);

        btnRow->addWidget(btn);
        return btn;
    };

    makeLangBtn(QStringLiteral("🇧🇷"), QStringLiteral("Português"), QStringLiteral("Brasil"), QStringLiteral("pt_BR"));
    makeLangBtn(QStringLiteral("🇺🇸"), QStringLiteral("English"),   QStringLiteral("United States"), QStringLiteral("en_US"));

    cardLayout->addLayout(btnRow);

    // ── Divisor + Botão Próximo ────────────────────────────────────────
    cardLayout->addStretch();

    auto *nextBtn = new QPushButton(tr("Próximo  →"));
    nextBtn->setCursor(Qt::PointingHandCursor);
    nextBtn->setFixedHeight(44);
    nextBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #3584e4;"
        "  color: #ffffff;"
        "  font-size: 15px;"
        "  font-weight: bold;"
        "  border: none;"
        "  border-radius: 10px;"
        "}"
        "QPushButton:hover { background-color: #4a9cf5; }"
        "QPushButton:pressed { background-color: #2a6bc4; }"
    );
    connect(nextBtn, &QPushButton::clicked, this, &QDialog::accept);
    cardLayout->addWidget(nextBtn);
}

void LanguageDialog::selectLanguage(const QString &lang)
{
    m_selectedLang = lang;

    // Update button label depending on selected language
    // Find the next button child and update its text
    if (auto *nextBtn = findChild<QPushButton*>(QString(), Qt::FindDirectChildrenOnly)) {
        Q_UNUSED(nextBtn);
    }
    // Update Próximo / Next label based on language
    const auto buttons = findChildren<QPushButton*>();
    for (auto *btn : buttons) {
        if (!btn->isCheckable()) {
            // This is the "Next" button
            btn->setText(lang == QLatin1String("pt_BR")
                         ? QStringLiteral("Próximo  →")
                         : QStringLiteral("Next  →"));
        }
    }
}
