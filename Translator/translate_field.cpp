/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2024 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QLocale>

#include "translate_field.h"
#include "ui_translate_field.h"

#include "syntax_msgbox_macros.h"


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
        if(m_tr)
            m_tr->text = ui->translationText->toPlainText();
        emit textChanged(ui->translationText->toPlainText());
        emit itemActivated(m_lang);
    });

    QObject::connect(ui->note,
                     &QLineEdit::textChanged,
                     this,
                     [this](const QString &e)->void
    {
        if(m_tr)
            m_tr->note = e;
    });

    QObject::connect(ui->trIsComplete,
                     static_cast<void(QCheckBox::*)(bool)>(&QCheckBox::toggled),
                     this,
                     [this](bool b)->void
    {
        if(m_tr)
            m_tr->unfinished = !b;
    });

    m_highLighter = s_makeMsgBoxMacrosHighlighter(ui->translationText->document());

    ui->translationText->installEventFilter(this);
    ui->note->installEventFilter(this);

    ui->translationText->setEnabled(false);
    ui->note->setEnabled(false);
    ui->trIsComplete->setEnabled(false);
}

TranslateField::~TranslateField()
{
    delete m_highLighter;
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
    ui->trIsComplete->blockSignals(true);
    ui->trIsComplete->setChecked(false);
    ui->trIsComplete->blockSignals(false);
    ui->trIsComplete->setEnabled(false);
    ui->note->blockSignals(true);
    ui->note->clear();
    ui->note->blockSignals(false);
    ui->note->setEnabled(false);
}

void TranslateField::setText(const QString &text, const QString &note, bool finished)
{
    ui->translationText->blockSignals(true);
    ui->translationText->setPlainText(text);
    ui->translationText->blockSignals(false);
    ui->translationText->setEnabled(true);
    ui->note->blockSignals(true);
    ui->note->setText(note);
    ui->note->blockSignals(false);
    ui->note->setEnabled(true);
    ui->trIsComplete->blockSignals(true);
    ui->trIsComplete->setChecked(finished);
    ui->trIsComplete->blockSignals(false);
    ui->trIsComplete->setEnabled(true);
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
    Q_ASSERT(m_tr);
    return m_tr->text;
}

void TranslateField::reloadEntry()
{
    if(m_group < 0 || m_type < 0 || m_root.isEmpty())
        return;

    m_tr = nullptr;

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
            m_tr = &e.title;
            setText(e.title.text, e.title.note, !e.title.unfinished);
            break;
        }
        case TextTypes::WDT_TITLE:
        {
            m_tr = &l.title;
            setText(l.title.text, l.title.note, !l.title.unfinished);
            break;
        }
        case TextTypes::WDT_CREDITS:
            m_tr = &l.credits;
            setText(l.credits.text, l.credits.note, !l.credits.unfinished);
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
            m_tr = &e.talk;
            setText(e.talk.text, e.talk.note, !e.talk.unfinished);
            break;
        }
        case TextTypes::LDT_EVENT:
        {
            if(m_key < 0)
                return;
            auto &e = l.events[m_key];
            m_tr = &e.message;
            setText(e.message.text, e.message.note, !e.message.unfinished);
            break;
        }
        case TextTypes::LDT_TITLE:
            m_tr = &l.title;
            setText(l.title.text, l.title.note, !l.title.unfinished);
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
            m_tr = &e.translation;
            setText(e.translation.text, e.translation.note, !e.translation.unfinished);
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
