#include <QLocale>

#include "translate_field.h"
#include "ui_translate_field.h"


TranslateField::TranslateField(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::TranslateField)
{
    ui->setupUi(this);
    QObject::connect(ui->translationText,
                     &QPlainTextEdit::textChanged,
                     this,
                     [this]()->void
    {
        if(m_string)
            *m_string = ui->translationText->toPlainText();
        emit TranslateField::textChanged(ui->translationText->toPlainText());
    });

    QObject::connect(ui->note,
                     &QLineEdit::textChanged,
                     this,
                     [this](const QString &e)->void
    {
        if(m_note)
            *m_note = e;
    });

    ui->translationText->installEventFilter(this);
    ui->note->installEventFilter(this);
}

TranslateField::~TranslateField()
{
    delete ui;
}

void TranslateField::setItem(int group, const QString &level, int type, int key)
{
    switch(group)
    {
    case TextTypes::S_WORLD:
    {
        break;
    }

    case TextTypes::S_LEVEL:
    {
        auto &l = (*m_project)[m_lang].levels[level];
        switch(type)
        {
        case TextTypes::LDT_NPC:
        {
            auto &e = l.npc[key];
            setText(e.talk, QString());
            break;
        }
        case TextTypes::LDT_EVENT:
        {
            auto &e = l.events[key];
            setText(e.message, QString());
            break;
        }
        case TextTypes::LDT_TITLE:
            setText(l.title, QString());
            break;
        }

        break;
    }

    case TextTypes::S_SCRIPT:
        break;
    }
}

void TranslateField::setText(const QString &text, const QString &note)
{
    ui->translationText->blockSignals(true);
    ui->translationText->setPlainText(text);
    ui->translationText->blockSignals(false);
    ui->note->blockSignals(true);
    ui->note->setText(note);
    ui->note->blockSignals(false);
}

void TranslateField::setLang(const QString &lang)
{
    m_lang = lang;
    QLocale loc(lang);
    ui->translationLabel->setText(tr("Translation for: %1 (%2)")
                                  .arg(loc.nativeLanguageName())
                                  .arg(loc.nativeCountryName()));
}

void TranslateField::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

bool TranslateField::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::FocusIn)
    {
        if(object == ui->translationText || object == ui->note)
            emit TranslateField::textChanged(ui->translationText->toPlainText());
    }

    return false;
}
