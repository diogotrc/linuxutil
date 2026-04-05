#include "welcomepage.h"
#include "../mainwizard.h"
#include <QAbstractButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QFile>
#include <QTextStream>
#include <QIcon>
#include <unistd.h>
#include <pwd.h>

WelcomePage::WelcomePage(MainWizard *wizard) 
    : QWizardPage(wizard), m_wizard(wizard)
{
    // Cabeçalho Principal (Título e Subtítulo)
    setTitle(tr("Bem-vindo ao Rapidora"));
    setSubTitle(tr("Este assistente configurará o seu sistema Fedora para jogos, "
                   "criação de conteúdo, desenvolvimento e muito mais."));

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(20);
    layout->setContentsMargins(10, 10, 10, 10);

    // 1. Mensagem de Aviso caso não esteja rodando como root
    if (geteuid() != 0) {
        auto *warnFrame = new QFrame;
        warnFrame->setFrameShape(QFrame::StyledPanel);
        warnFrame->setStyleSheet(
            "QFrame { "
            "  background-color: #551010; "
            "  border: 1px solid #ff4d4d; "
            "  border-radius: 8px; "
            "  padding: 10px; "
            "}"
        );
        auto *wl = new QVBoxLayout(warnFrame);
        auto *warnLabel = new QLabel(
            tr("<b style='color: #ff6666;'>Aviso: Esta aplicação deve ser executada como root.</b><br>"
               "<span style='color: #ffcccc;'>Por favor, reinicie com: <tt>pkexec rapidora</tt></span>")
        );
        warnLabel->setWordWrap(true);
        wl->addWidget(warnLabel);
        layout->addWidget(warnFrame);
    }

    // 2. Quadro agrupado (Card) de Informações do Sistema
    layout->addWidget(createSystemInfoCard());

    // 3. Descrição sutil em Cinza
    auto *descLabel = new QLabel(
        tr("<p>Este assistente permite escolher exatamente o que instalar. "
           "Nada é selecionado por padrão, a escolha é toda sua.</p>"
           "<p>Você pode regressar e alterar as seleções antes que a instalação comece.</p>"
           "<p><b>É necessária uma conexão ativa com a internet.</b></p>")
    );
    descLabel->setWordWrap(true);
    // Tom de cinza sutil como solicitado
    descLabel->setStyleSheet("color: #aaaaaa; font-size: 14px;"); 
    layout->addWidget(descLabel);

    // 4. Quadro agrupado (Card) de Instruções e Guias
    layout->addWidget(createInstructionsCard());

    layout->addStretch();
}

bool WelcomePage::isComplete() const
{
    // Exige que apenas se rode este Wizard como SuperUsuário para avançar
    return geteuid() == 0;
}

void WelcomePage::initializePage()
{
    // Destaque visual especial no botão Próximo (Next) gerenciado pelo Wizard
    if (wizard()) {
        auto *nextBtn = wizard()->button(QWizard::NextButton);
        if (nextBtn) {
            nextBtn->setStyleSheet(
                "QPushButton {"
                "  background-color: #3584e4;" // Azul System / Destaque
                "  color: white;"
                "  border-radius: 6px;"
                "  padding: 6px 16px;" // Padding maior pros lados
                "  font-weight: bold;"
                "}"
                "QPushButton:hover:!pressed { background-color: #4a90e2; }" // Hover claro
                "QPushButton:disabled { background-color: #3b3b3b; color: #777777; }" // Hover Escuro
            );
        }
    }
}

QString WelcomePage::getSystemUser() const
{
    // Busca do OS quem executou o `sudo` ou o desktop original
    const char* sudoUser = getenv("SUDO_USER");
    if (sudoUser) {
        return QString(sudoUser);
    }
    const char* pkexecUser = getenv("PKEXEC_UID");
    if (pkexecUser) {
        struct passwd *pw = getpwuid(atoi(pkexecUser));
        if (pw) return QString(pw->pw_name);
    }
    // Fallback normal
    return QString(getenv("USER"));
}

QString WelcomePage::getFedoraVersion() const
{
    // Lógica pura de leitura do /etc/os-release (Parse seguro)
    QFile file("/etc/os-release");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("VERSION_ID=")) {
                return line.split('=').last().remove('"');
            }
        }
    }
    return tr("Desconhecida");
}

QFrame* WelcomePage::createSystemInfoCard()
{
    auto *frame = new QFrame;
    frame->setObjectName("SystemInfoCard");
    // Estilo moderno tipo 'Card' arredondado
    frame->setStyleSheet(
        "QFrame#SystemInfoCard {"
        "  background-color: rgba(255, 255, 255, 0.05);" // Fundo translúcido (destaca contra o form pai) 
        "  border: 1px solid rgba(255, 255, 255, 0.1);"  
        "  border-radius: 10px;"
        "}"
    );

    auto *hl = new QHBoxLayout(frame);
    hl->setContentsMargins(20, 20, 20, 20);

    // Ícone de Sistema
    auto *iconLabel = new QLabel;
    QIcon devIcon = QIcon::fromTheme("computer");
    // Valida fallback
    if(devIcon.isNull()) devIcon = QIcon::fromTheme("system-run");
    iconLabel->setPixmap(devIcon.pixmap(48, 48));
    
    // Layout de Textos do Card
    auto *textLayout = new QVBoxLayout;
    
    auto *title = new QLabel("<b>" + tr("Informações do Sistema") + "</b>");
    title->setStyleSheet("font-size: 16px; color: #ffffff;");
    
    QString fVer = getFedoraVersion();
    QString user = getSystemUser();
    
    auto *osLabel = new QLabel(tr("Versão do Fedora: ") + "<b>" + fVer + "</b>");
    auto *userLabel = new QLabel(tr("Usuário: ") + "<b>" + user + "</b>");
    
    textLayout->addWidget(title);
    textLayout->addWidget(osLabel);
    textLayout->addWidget(userLabel);
    
    hl->addWidget(iconLabel);
    hl->addSpacing(15);
    hl->addLayout(textLayout);
    hl->addStretch(); 
    
    return frame;
}

QFrame* WelcomePage::createInstructionsCard()
{
    auto *frame = new QFrame;
    frame->setObjectName("InstrCard");
    frame->setStyleSheet(
        "QFrame#InstrCard {"
        "  background-color: rgba(255, 255, 255, 0.05);"
        "  border: 1px solid rgba(255, 255, 255, 0.1);"
        "  border-radius: 10px;"
        "}"
    );

    auto *vl = new QVBoxLayout(frame);
    vl->setContentsMargins(20, 20, 20, 20);
    vl->setSpacing(15);

    auto *title = new QLabel("<b>" + tr("Guia das Caixas de Seleção") + "</b>");
    title->setStyleSheet("font-size: 15px; color: #ffffff; margin-bottom: 5px;");
    vl->addWidget(title);

    // Como criar as divisórias das listas:
    
    // -- Item 1 [Instalado]
    auto *item1 = new QHBoxLayout;
    
    auto *badge1 = new QLabel(tr("Instalado"));
    badge1->setAlignment(Qt::AlignCenter);
    badge1->setStyleSheet(
        "background-color: rgba(46, 139, 87, 0.9);" // Verde (SeaGreen)
        "color: white;"
        "padding: 4px 10px;"
        "border-radius: 6px;"
        "font-weight: bold;"
    );
    
    badge1->setFixedWidth(110);
    
    auto *desc1 = new QLabel(tr("Se marcado, o pacote garante atualização/reinstalação."));
    desc1->setWordWrap(true);
    
    item1->addWidget(badge1);
    item1->addSpacing(15);
    item1->addWidget(desc1);
    item1->addStretch();

    // -- Item 2 [Não Instalado]
    auto *item2 = new QHBoxLayout;
    
    auto *badge2 = new QLabel(tr("Não Instalado"));
    badge2->setAlignment(Qt::AlignCenter);
    badge2->setStyleSheet(
        "background-color: rgba(210, 105, 30, 0.9);" // Laranja escuro / Chocolate
        "color: white;"
        "padding: 4px 10px;"
        "border-radius: 6px;"
        "font-weight: bold;"
    );
    
    badge2->setFixedWidth(110);
    
    auto *desc2 = new QLabel(tr("Se marcado, indica que será baixado e instalado em sua máquina."));
    desc2->setWordWrap(true);
    
    item2->addWidget(badge2);
    item2->addSpacing(15);
    item2->addWidget(desc2);
    item2->addStretch();
    
    // -- Descrição Adicional
    auto *infoLabel = new QLabel(tr("<i>Itens não selecionados permanecerão inalterados pelo sistema.</i>"));
    infoLabel->setStyleSheet("color: #888888; font-size: 13px; margin-top: 5px;");

    vl->addLayout(item1);
    vl->addLayout(item2);
    vl->addWidget(infoLabel);

    return frame;
}
