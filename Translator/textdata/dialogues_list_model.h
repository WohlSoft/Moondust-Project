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


#ifndef DIALOGUESLISTMODEL_H
#define DIALOGUESLISTMODEL_H

#include <QAbstractTableModel>
#include "textdata.h"

class DialoguesListModel : public QAbstractTableModel
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    TranslationData_Level *m_level = nullptr;
    QString m_levelKey;
    QString m_recentLang;
    QVector<TranslationData_Dialogue> *m_dialogues = nullptr;

    struct TrView
    {
        int i;
        int count;
        QString note;
    };

    typedef QVector<TrView> TrViewList;
    TrViewList m_view;

public:
    explicit DialoguesListModel(TranslateProject *project, QObject *parent = nullptr);

    enum Columns
    {
        C_INDEX = 0,
        C_NUMBER_ITEMS,
        C_TITLE,
        C_COUNT
    };

    void setData(const QString &lang, const QString &level);
    void clear();

    TranslationData_Level &level() const;
    QString levelKey() const;

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
};

#endif // DIALOGUESLISTMODEL_H
