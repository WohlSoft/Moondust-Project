#include "dialogues_list_model.h"

DialoguesListModel::DialoguesListModel(TranslateProject *project, QObject *parent)
    : QAbstractTableModel(parent)
    , m_project(project)
{}

void DialoguesListModel::setData(const QString &lang, const QString &level)
{
    beginResetModel();
    m_levelKey = level;
    m_level = &(*m_project)[lang].levels[level];
    m_dialogues = &m_level->dialogues;
    m_view.clear();

    for(int i = 0; i < m_dialogues->size(); ++i)
    {
        auto &d = (*m_dialogues)[i];
        TrView e;
        e.i = i;
        e.note = d.note;
        m_view.push_back(e);
    }

    endResetModel();
}

void DialoguesListModel::clear()
{
    beginResetModel();
    m_levelKey.clear();
    m_level = nullptr;
    m_dialogues = nullptr;
    m_view.clear();
    endResetModel();
}

TranslationData_Level &DialoguesListModel::level() const
{
    return *m_level;
}

QString DialoguesListModel::levelKey() const
{
    return m_levelKey;
}

QVariant DialoguesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
    case C_INDEX:
        return tr("No");
    case C_TITLE:
        return tr("Title");
    }

    return QVariant();
}

int DialoguesListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_view.size();
}

int DialoguesListModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return C_COUNT;
}

QVariant DialoguesListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= m_view.size())
        return QVariant();

    auto &it = m_view[index.row()];

    switch(index.column())
    {
    case C_INDEX:
        if(role == Qt::DisplayRole)
            return it.i;
        break;
    case C_TITLE:
        if(role == Qt::DisplayRole || role == Qt::EditRole)
            return it.note;
        break;
    }

    return QVariant();
}

bool DialoguesListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(data(index, role) != value && index.column() == C_TITLE)
    {
        auto &d = m_view[index.row()];
        d.note = value.toString();
        (*m_dialogues)[d.i].note = d.note;
        emit dataChanged(index, index, {role});
        return true;
    }

    return false;
}

Qt::ItemFlags DialoguesListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    switch(index.column())
    {
    case C_TITLE:
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    default:
        return QAbstractItemModel::flags(index);
    }
}
