/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <memory>

#include <QAbstractListModel>
#include <QString>
#include <QVector>


class QFileIconProvider;

class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit FileListModel(QObject *parent = nullptr);
    virtual ~FileListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * @brief Set filter string to show objects that contains a string fragment
     * @param filter Content of the filter line
     */
    void setFilter(const QString filter);

    void clearList();
    void addEntry(const QString &fileName);

    void sortData();
    void setDirectory(const QString &dir);

private:
    QString m_filter;
    QString m_rootDirectory;
    QVector<QString> m_entries;
    QVector<int> m_entriesMap;
    std::unique_ptr<QFileIconProvider> m_iconProv;

    void updateVisibility();
    bool filterMatch(const QString &file) const;
};

#endif // FILELISTMODEL_H
