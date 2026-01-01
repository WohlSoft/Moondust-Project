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

#ifndef LANG_SELECT_H
#define LANG_SELECT_H

#include <QDialog>

namespace Ui {
class LangSelect;
}

class LangSelect : public QDialog
{
    Q_OBJECT

    void initList();

public:
    explicit LangSelect(QWidget *parent = nullptr);
    ~LangSelect();

    void setData(const QString &title, const QString &label);

    QString currentLanguageCode();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_langSearch_textChanged(const QString &arg1);
    void on_language_currentIndexChanged(int index);

private:
    Ui::LangSelect *ui;
    QString m_currentLangCode;
};

#endif // LANG_SELECT_H
