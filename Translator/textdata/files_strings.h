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

#ifndef FILESSTRINGS_H
#define FILESSTRINGS_H

#include <QAbstractTableModel>

#include "files_list_model.h"
#include "text_types.h"


class FilesStringsModel : public QAbstractTableModel
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    const TranslationData *m_language = nullptr;
    const TranslationData_Level *m_level = nullptr;
    const TranslationData_World *m_world = nullptr;
    const TranslationData_Script *m_script = nullptr;
    int m_recentSection = -1;
    QString m_recentKey;

    struct TrView
    {
        int source;
        int state;
        int type;
        int key;
        QString title;
        QString note;
        QString tr_note;
        QString root;
    };

    typedef QVector<TrView> TrViewList;
    TrViewList m_view;

    static int stateFromTrLine(const TrLine &line);

public:
    explicit FilesStringsModel(TranslateProject *project, QObject *parent = nullptr);

    enum Columns
    {
        C_TYPE = 0,
        C_STATE,
        C_TITLE,
        C_NOTE,
        C_COUNT
    };

    enum Roles
    {
        R_NOTE = Qt::UserRole,
        R_ROOT,
        R_GROUP,
        R_TYPE,
        R_KEY,
        R_NOTE_PTR
    };

    void setFileData(const QString &lang, int s, const QString &key);
    void clear();

    void updateStatus(const QString &lang);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // FILESSTRINGS_H
