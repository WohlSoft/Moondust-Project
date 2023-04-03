#include "files_list_model.h"


void FilesListModel::rebuildView()
{
    const TranslationData &origin = m_project->value("origin");

    beginResetModel();

    m_view.clear();

    for(auto w = origin.worlds.begin() ; w != origin.worlds.end(); ++w)
    {
        TrView e;
        e.type = T_WORLD;
        e.title = w.key();
        m_view.push_back(e);
    }

    for(auto l = origin.levels.begin() ; l != origin.levels.end(); ++l)
    {
        TrView e;
        e.type = T_LEVEL;
        e.title = l.key();
        m_view.push_back(e);
    }

    for(auto s = origin.scripts.begin() ; s != origin.scripts.end(); ++s)
    {
        TrView e;
        e.type = T_SCRIPT;
        e.title = s.key();
        m_view.push_back(e);
    }

    std::sort(m_view.begin(), m_view.end(),
              [](const TrView&o1, const TrView&o2)->bool
    {
        return o1.title.compare(o2.title, Qt::CaseInsensitive) < 0;
    });

    endResetModel();
}

FilesListModel::FilesListModel(TranslateProject *project, QObject *parent)
    : QAbstractTableModel(parent)
    , m_project(project)
{}

QVariant FilesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
    case 0: // type
        return tr("Type");
    case 1: // type
        return tr("Title");
    }

    return QVariant();
}

int FilesListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_view.size();
}

int FilesListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant FilesListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    auto r = index.row();

    switch(index.column())
    {
    case 0: // type
        if(role == Qt::DisplayRole)
        {
            switch(m_view[r].type)
            {
            case T_WORLD:
                return tr("World");
            case T_LEVEL:
                return tr("Level");
            case T_SCRIPT:
                return tr("Script");
            }
        }
        else if(role == R_TYPE)
            return m_view[r].type;

        break;

    case 1: // title
        if(role == Qt::DisplayRole)
            return m_view[r].title;
        break;
    }

    // FIXME: Implement me!
    return QVariant();
}
