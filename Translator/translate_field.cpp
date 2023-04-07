#include <QLocale>

#include "translate_field.h"
#include "ui_translate_field.h"


TranslateField::TranslateField(TranslateProject *project, QWidget *parent) :
    QFrame(parent)
    , m_project(project)
    , ui(new Ui::TranslateField)
{
    Q_ASSERT(m_project);
    ui->setupUi(this);
    QObject::connect(ui->translationText,
                     &QPlainTextEdit::textChanged,
                     this,
                     [this]()->void
    {
        if(m_string)
            *m_string = ui->translationText->toPlainText();
        emit textChanged(ui->translationText->toPlainText());
        emit itemActivated(m_lang);
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

    ui->translationText->setDisabled(false);
    ui->note->setDisabled(false);
}

TranslateField::~TranslateField()
{
    delete ui;
}

void TranslateField::setItem(int group, const QString &root, int type, int key)
{
    m_group = group;
    m_type = type;
    m_key = key;
    m_root = root;

    reloadEntry();
}

void TranslateField::clearItem()
{
    ui->translationText->blockSignals(true);
    ui->translationText->clear();
    ui->translationText->blockSignals(false);
    ui->translationText->setEnabled(false);
    ui->note->blockSignals(true);
    ui->note->clear();
    ui->note->blockSignals(false);
    ui->note->setEnabled(false);
}

void TranslateField::setText(const QString &text, const QString &note)
{
    ui->translationText->blockSignals(true);
    ui->translationText->setPlainText(text);
    ui->translationText->blockSignals(false);
    ui->translationText->setEnabled(true);
    ui->note->blockSignals(true);
    ui->note->setText(note);
    ui->note->blockSignals(false);
    ui->note->setEnabled(true);
}

void TranslateField::setLang(const QString &lang)
{
    m_lang = lang;
    QLocale loc(lang);
    ui->translationLabel->setText(tr("Translation for: %1 (%2)")
                                  .arg(loc.nativeLanguageName())
                                  .arg(loc.nativeCountryName()));
}

const QString &TranslateField::getLang() const
{
    return m_lang;
}

const QString &TranslateField::getText() const
{
    Q_ASSERT(m_string);
    return *m_string;
}

void TranslateField::reloadEntry()
{
    if(m_group < 0 || m_type < 0 || m_root.isEmpty())
        return;

    m_string = nullptr;
    m_note = nullptr;

    switch(m_group)
    {
    case TextTypes::S_WORLD:
    {
        auto &l = (*m_project)[m_lang].worlds[m_root];
        switch(m_type)
        {
        case TextTypes::WDT_LEVEL:
        {
            if(m_key < 0)
                return;
            auto &e = l.level_titles[m_key];
            m_string = &e.title.text;
            m_note = &e.title.note;
            setText(e.title.text, e.title.note);
            break;
        }
        case TextTypes::WDT_TITLE:
        {
            m_string = &l.title.text;
            m_note = &l.title.note;
            setText(l.title.text, l.title.note);
            break;
        }
        case TextTypes::WDT_CREDITS:
            m_string = &l.credits.text;
            m_note = &l.credits.note;
            setText(l.credits.text, l.credits.note);
            break;
        }
        break;
    }

    case TextTypes::S_LEVEL:
    {
        auto &l = (*m_project)[m_lang].levels[m_root];
        switch(m_type)
        {
        case TextTypes::LDT_NPC:
        {
            if(m_key < 0)
                return;
            auto &e = l.npc[m_key];
            m_string = &e.talk.text;
            m_note = &e.talk.note;
            setText(e.talk.text, e.talk.note);
            break;
        }
        case TextTypes::LDT_EVENT:
        {
            if(m_key < 0)
                return;
            auto &e = l.events[m_key];
            m_string = &e.message.text;
            m_note = &e.message.note;
            setText(e.message.text, e.message.note);
            break;
        }
        case TextTypes::LDT_TITLE:
            m_string = &l.title.text;
            m_note = &l.title.note;
            setText(l.title.text, l.title.note);
            break;
        }

        break;
    }

    case TextTypes::S_SCRIPT:
        auto &l = (*m_project)[m_lang].scripts[m_root];
        switch(m_type)
        {
        case TextTypes::SDT_LINE:
        {
            if(m_key < 0)
                return;
            auto &e = l.lines[m_key];
            m_string = &e.translation.text;
            m_note = &e.translation.note;
            setText(e.translation.text, e.translation.note);
            break;
        }
        }
        break;
    }
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
        {
            emit textChanged(ui->translationText->toPlainText());
            emit itemActivated(m_lang);
        }
    }

    return false;
}
