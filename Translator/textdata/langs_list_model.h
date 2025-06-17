/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LANGSLISTMODEL_H
#define LANGSLISTMODEL_H

#include <QAbstractTableModel>
#include <QHash>
#include "textdata.h"

class QSettings;

class LangsListModel : public QAbstractTableModel
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    QSettings *m_setup = nullptr;

    struct LangsView
    {
        bool    vis;
        int     strings;
        int     translated;
        QString code;
        QString lang_name;
    };

    typedef QVector<LangsView> LangsViewList;
    LangsViewList m_view;
    QHash<QString, int> m_viewByLang;

    void rebuildMap();

signals:
    void visibilityChanged();

public:
    explicit LangsListModel(TranslateProject *project, QSettings *setup, QObject *parent = nullptr);

    enum Columns
    {
        C_VISIBLE = 0,
        C_TOTAL_STRINGS,
        C_TOTAL_TRANSLATED,
        C_LANG_CODE,
        C_LANG_NAME,

        P_COLUMNS_COUNT
    };

    bool langIsVisible(const QString &langCode);

    void refreshData();
    void clear();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
};

#endif // LANGSLISTMODEL_H
