#include <QIcon>
#include <QFileInfo>
#include <QDir>
#include "files_list_model.h"


void FilesListModel::rebuildView(const QString &path)
{
    const TranslationData &origin = m_project->value("origin");
    m_path = path;
    QFileInfo p(path);
    m_title = p.completeBaseName();

    beginResetModel();

    m_view.clear();

    for(auto w = origin.worlds.begin() ; w != origin.worlds.end(); ++w)
    {
        TrView e;
        e.type = T_WORLD;
        e.key = w.key();
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

    for(auto l = origin.levels.begin() ; l != origin.levels.end(); ++l)
    {
        TrView e;
        e.type = T_LEVEL;
        e.key = l.key();
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

    for(auto s = origin.scripts.begin() ; s != origin.scripts.end(); ++s)
    {
        TrView e;
        e.type = T_SCRIPT;
        e.key = s.key();
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

    std::sort(m_view.begin(), m_view.end(),
              [](const TrView&o1, const TrView&o2)->bool
    {
        return o1.key.compare(o2.key, Qt::CaseInsensitive) < 0;
    });

    for(int i = 0; i < m_view.size(); ++i)
        m_view[i].row = i;

    buildRelations();

    endResetModel();
}

void FilesListModel::addChild(TrView &it)
{
    for(auto &i : m_view)
    {
        if(i.dir == it.path)
        {
            i.children.push_back(it);
            break;
        }
    }
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
