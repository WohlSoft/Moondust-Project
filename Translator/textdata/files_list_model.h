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

#ifndef FILESLISTMODEL_H
#define FILESLISTMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QIcon>
#include "textdata.h"


class FilesListModel : public QAbstractItemModel
{
    Q_OBJECT

    TranslateProject *m_project = nullptr;
    struct TrView
    {
        int type = -1;
        QString key;
        QString title;
        QString dir;
        QString path;
        QVector<TrView> children;
        QVector<TrView> *p;
        int parent = -1;
        int row = -1;
    };

    typedef QVector<TrView> TrViewList;
    TrViewList m_view;
    TrViewList m_viewOrphans;
    QString m_path;
    QString m_title;
    QIcon m_rootIcon;

    static void buildRelationsRekurs(TrView &it, TrViewList *parent, int parentIndex);
    void addToView(TrView &it, const QString &path);
    bool addChild(TrView &it, bool isOrphan = false);
    void buildRelations();

public:
    explicit FilesListModel(TranslateProject *project, QObject *parent = nullptr);

    enum Type
    {
        T_WORLD = 0,
        T_LEVEL,
        T_SCRIPT,
        T_DIR
    };

    enum Roles
    {
        R_TYPE = Qt::UserRole,
        R_KEY
    };

    void rebuildView(const QString &path);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // FILESLISTMODEL_H
