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

#ifndef TRANSLATE_FIELD_H
#define TRANSLATE_FIELD_H

#include <QFrame>
#include "textdata/textdata.h"
#include "textdata/text_types.h"


namespace Ui {
class TranslateField;
}

class SimpleHighlighter;

class TranslateField : public QFrame
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    QString m_lang;
    QString m_root;
    int m_group = -1;
    int m_type = -1;
    int m_key = -1;
    TrLine *m_tr = nullptr;
    bool m_prevEmpty = false;

    SimpleHighlighter *m_highLighter = nullptr;

public:
    explicit TranslateField(TranslateProject *project, QWidget *parent = nullptr);
    ~TranslateField();

    void setItem(int group, const QString &root, int type, int key);
    void clearItem();

    void setText(const QString &text, const QString &note, bool finished);
    void setLang(const QString &lang);

    const QString &getLang() const;
    const QString &getText() const;

    void reloadEntry();

signals:
    void textChanged(const QString &newText);
    void itemActivated(const QString &lang);
    void trStateUpdated(); // Call when between empty <> !empty, or when "finished" checked/unchecked

protected:
    void changeEvent(QEvent *e);
    bool eventFilter(QObject *object, QEvent *event);

private:
    Ui::TranslateField *ui;
};

#endif // TRANSLATE_FIELD_H
