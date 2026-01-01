/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lang_select.h"
#include "ui_lang_select.h"
#include <QLocale>
#include <QtDebug>


void LangSelect::initList()
{
    QList<QLocale> allLocales = QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);

    ui->language->clear();

    QMap<QString, QStringList> langs;

    for(const QLocale &locale : allLocales)
    {
        if(locale.name() == "C")
            continue; // unnecessary

        QStringList l = locale.name().split("_");

        QString lang = l[0].toLower();
        QString country = l.size() > 1 ? l[1].toLower() : QString();

        if(!langs.contains(lang))
        {
            QStringList x = {QString()};
            langs.insert(lang, x);
        }

        langs[lang].push_back(country);
    }

    for(auto i = langs.begin(); i != langs.end(); ++i)
    {
        for(auto j = i.value().begin(); j != i.value().end(); ++j)
        {
            QString code;
            QString stdCode;
            QString title;
            bool shortCode;

            if(j->isEmpty())
            {
                shortCode = true;
                code = i.key();
                stdCode = i.key();
            }
            else
            {
                shortCode = false;
                code = QString("%1-%2").arg(i.key()).arg(*j);
                stdCode = QString("%1_%2").arg(i.key()).arg(j->toUpper());
            }

            if(code == "en")
                title = "en - English";
            else
            {
                QLocale loc(stdCode);
                if(shortCode)
                    title = QString("%1 - %2")
                                .arg(code)
                                .arg(QLocale::languageToString(loc.language()));
                else
                    title = QString("%1 - %2 (%3)")
                                .arg(code)
                                .arg(QLocale::languageToString(loc.language()))
                                .arg(QLocale::countryToString(loc.country()));
            }

            ui->language->addItem(title, code);
        }
    }
}

LangSelect::LangSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LangSelect)
{
    ui->setupUi(this);
    initList();
}

LangSelect::~LangSelect()
{
    delete ui;
}

void LangSelect::setData(const QString &title, const QString &label)
{
    setWindowTitle(title);
    ui->intro->setText(label);
}

QString LangSelect::currentLanguageCode()
{
    return m_currentLangCode;
}

void LangSelect::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LangSelect::on_langSearch_textChanged(const QString &arg1)
{
    if(arg1.isEmpty())
        return;

    for(int i = 0; i < ui->language->count(); ++i)
    {
        QString text = ui->language->itemText(i);

        if(text.indexOf(arg1, 0, Qt::CaseInsensitive) >= 0)
        {
            ui->language->setCurrentIndex(i);
            return;
        }
    }
}

void LangSelect::on_language_currentIndexChanged(int index)
{
    if(index < 0)
        return;

    m_currentLangCode = ui->language->itemData(index).toString();
}
