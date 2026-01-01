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

#include <QIcon>
#include <QFileInfo>
#include <QDir>
#include <QFileIconProvider>
#include <QtDebug>
#include "files_list_model.h"


static QString t2str(int t)
{
    switch(t)
    {
    case FilesListModel::T_WORLD:
        return "World";
    case FilesListModel::T_LEVEL:
        return "Level";
    case FilesListModel::T_SCRIPT:
        return "Script";
    case FilesListModel::T_DIR:
        return "Directory";
    default:
        return "Unknown";
    }
}


void FilesListModel::rebuildView(const QString &path)
{
    const TranslationData &origin = m_project->value("metadata");
    m_path = path;
    QFileInfo p(path);
    m_title = p.completeBaseName();

    qDebug() << "FilesListModel rebuild: Begin";

    beginResetModel();

    m_view.clear();
    m_viewOrphans.clear();

    for(auto w = origin.worlds.begin() ; w != origin.worlds.end(); ++w)
    {
        TrView e;
        e.type = T_WORLD;
        e.key = w.key();
        addToView(e, path);
    }

    for(auto l = origin.levels.begin() ; l != origin.levels.end(); ++l)
    {
        TrView e;
        e.type = T_LEVEL;
        e.key = l.key();
        addToView(e, path);
    }

    for(auto s = origin.scripts.begin() ; s != origin.scripts.end(); ++s)
    {
        TrView e;
        e.type = T_SCRIPT;
        e.key = s.key();
        addToView(e, path);
    }

    for(auto &s : origin.directories)
    {
        TrView e;
        e.type = T_DIR;
        e.key = s;
        addToView(e, path);
    }

    // Bring orphan objects to their parents
    if(!m_viewOrphans.isEmpty())
    {
        qDebug() << "Found" << m_viewOrphans.size() << "orphans... (Checking view objects, quantity" << m_view.size() << ")";
        for(auto &s : m_viewOrphans)
        {
            qDebug() << "Checking orphan for " << t2str(s.type) << s.title << "that supposed to be in path" << s.path;
            bool ret = addChild(s, true);
            Q_ASSERT(ret);
            // true orphan objects are invalid
        }
        m_viewOrphans.clear();
    }

    std::sort(m_view.begin(), m_view.end(),
              [](const TrView&o1, const TrView&o2)->bool
    {
        return o1.key.compare(o2.key, Qt::CaseInsensitive) < 0;
    });

    for(int i = 0; i < m_view.size(); ++i)
        m_view[i].row = i;

    buildRelations();

    endResetModel();

    qDebug() << "FilesListModel rebuild: End";
}

void FilesListModel::addToView(TrView &e, const QString &path)
{
    e.parent = -1;
    e.p = nullptr;
    QFileInfo i(path + "/" + e.key);
    e.title = i.fileName();
    e.dir = i.completeBaseName();
    e.path = i.path();
    e.path.remove(m_path);
    if(e.path.startsWith('/'))
        e.path.remove(0, 1);

    if(!e.path.isEmpty())
        addChild(e);
    else
        m_view.push_back(e);
}

bool FilesListModel::addChild(TrView &it, bool isOrphan)
{
    bool found = false;

    for(auto &i : m_view)
    {
        if(it.path.compare(i.dir, Qt::CaseInsensitive) == 0)
        {
            if(isOrphan)
                qDebug() << "Found child for orphan" << t2str(it.type) << it.title << "at the " << t2str(i.type) << i.title << "in dir" << i.dir;
            i.children.push_back(it);
            found = true;
            break;
        }
    }

    if(!found)
    {
        qDebug() << "Object" << t2str(it.type) << it.title << "is orphan, as no parent was found";

        if(isOrphan)
        {
            if(it.type == T_LEVEL)
            {
                qDebug() << "Adding orphan" << t2str(it.type) << it.title << "as a directory...";
                TrView e;
                e.type = T_DIR;
                e.key = it.dir;
                addToView(e, m_path);
                return true;
            }
            else
            {
                qDebug() << "Orphan" << t2str(it.type) << it.title << "has missing parent, supposed to be in path" << it.path;
            }
        }
        else // Add later, when required branch will be added soon
        {
            qDebug() << "Found orphan" << t2str(it.type) << it.title << "that supposed to be child of not yet in list" << it.path;
            m_viewOrphans.push_back(it);
        }
    }

    return found;
}

void FilesListModel::buildRelationsRekurs(FilesListModel::TrView &it, TrViewList *parent, int parentIndex)
{
    for(int i = 0; i < it.children.size(); ++i)
    {
        auto &sit = it.children[i];
        sit.row = i;
        sit.parent = parentIndex;
        sit.p = parent;
        buildRelationsRekurs(sit, &(it.children), i);
    }
}

void FilesListModel::buildRelations()
{
    for(int i = 0; i < m_view.size(); ++i)
    {
        auto &it = m_view[i];
        buildRelationsRekurs(it, &m_view, i);
    }
}

FilesListModel::FilesListModel(TranslateProject *project, QObject *parent)
    : QAbstractItemModel(parent)
    , m_project(project)
{
    m_rootIcon.addFile(":/images/file_wldx_16.png");
    m_rootIcon.addFile(":/images/file_wldx_32.png");
    m_rootIcon.addFile(":/images/file_wldx_48.png");
    m_rootIcon.addFile(":/images/file_wldx_256.png");
}

QVariant FilesListModel::headerData(int, Qt::Orientation, int) const
{
    return QVariant();
}

QModelIndex FilesListModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    const TrView *parentItem = nullptr;
    const TrView *childItem = nullptr;

    if(parent.isValid())
    {
        parentItem = static_cast<TrView*>(parent.internalPointer());
        Q_ASSERT(parentItem);
        childItem = &parentItem->children[row];
    }
    else
        childItem = &m_view[row];

    if(childItem)
        return createIndex(row, column, (void*)(childItem));

    return QModelIndex();
}

QModelIndex FilesListModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    const TrView *childItem = static_cast<TrView*>(index.internalPointer());
    const TrView *parentItem = (childItem->parent >= 0 ? &(*childItem->p)[childItem->parent] : nullptr);

    if(parentItem == nullptr)
        return QModelIndex();

    Q_ASSERT(parentItem->row >= 0);

    return createIndex(parentItem->row, 0, (void*)parentItem);
}

int FilesListModel::rowCount(const QModelIndex &parent) const
{
    TrView *parentItem = nullptr;

    if(parent.column() > 0)
        return 0;

    if(!parent.isValid())
        return m_view.size();
    else
        parentItem = static_cast<TrView*>(parent.internalPointer());

    return parentItem->children.size();
}

int FilesListModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 1;
    return 1;
}

Qt::ItemFlags FilesListModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

QVariant FilesListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    TrView *it = static_cast<TrView*>(index.internalPointer());

    if(it == nullptr)
    {
        if(role == Qt::DecorationRole)
            return m_rootIcon;
        else if(role == Qt::DisplayRole)
            return m_title;

        return QVariant();
    }

    switch(index.column())
    {
    case 0: // type
        if(role == Qt::ToolTipRole)
        {
            switch(it->type)
            {
            case T_WORLD:
                return tr("World");
            case T_LEVEL:
                return tr("Level");
            case T_SCRIPT:
                return tr("Script");
            case T_DIR:
                return tr("Directory");
            }
        }
        else if(role == Qt::DecorationRole)
        {
            switch(it->type)
            {
            case T_WORLD:
            {
                QIcon i(":/images/world.png");
                i.addFile(":/images/world.png");
                return i;
            }
            case T_LEVEL:
            {
                QIcon i(":/images/level.png");
                i.addFile(":/images/level16.png");
                return i;
            }
            case T_SCRIPT:
                return QIcon(":/images/scripts.png");
            case T_DIR:
            {
                QFileIconProvider i;
                return i.icon(QFileIconProvider::Folder);
            }
            }
        }
        else if(role == Qt::DisplayRole)
            return it->title;
        else if(role == R_TYPE)
            return it->type;
        else if(role == R_KEY)
            return it->key;

        break;
    }

    // FIXME: Implement me!
    return QVariant();
}
