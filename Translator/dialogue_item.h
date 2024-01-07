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

#ifndef DIALOGUE_ITEM_H
#define DIALOGUE_ITEM_H

#include <QFrame>
#include "textdata/textdata.h"

namespace Ui {
class DialogueItem;
}

class DialogueItem : public QFrame
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    QString m_lang;
    QString m_level;
    QString m_text;
    QString m_display;
    int m_type = -1;
    int m_key = -1;
    TranslationData_NPC *m_npc = nullptr;
    TranslationData_EVENT *m_evt = nullptr;

public:
    enum Type
    {
        T_NPC,
        T_EVENT
    };

    explicit DialogueItem(TranslateProject *project,
                          const QString &lang,
                          const QString &level,
                          int type,
                          int key,
                          QWidget *parent = nullptr);
    ~DialogueItem();

    void setLang(const QString &lang);
    QString getText() const;

signals:
    void clicked();

protected:
    void changeEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);

private:
    void updateLabel();
    Ui::DialogueItem *ui;
};

#endif // DIALOGUE_ITEM_H
