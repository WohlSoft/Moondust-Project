#include "files_strings.h"

FilesStringsModel::FilesStringsModel(TranslateProject *project, QObject *parent)
    : QAbstractTableModel(parent)
    , m_project(project)
{}

void FilesStringsModel::setData(const QString &lang, int s, const QString &key)
{
    beginResetModel();

    m_level = nullptr;
    m_world = nullptr;
    m_script = nullptr;
    m_view.clear();

    switch(s)
    {
    case S_WORLD:
        m_world = &(*m_project)[lang].worlds[key];
        for(auto w = m_world->level_titles.begin() ; w != m_world->level_titles.end(); ++w)
        {
            TrView e;
            e.source = s;
            e.type = WDT_LEVEL;
            e.title = w->title;
            e.key = w.key();
            if(e.title.isEmpty())
                e.state = ST_BLANK;
            else if(w->unfinished && !e.title.isEmpty())
                e.state = ST_UNFINISHED;
            else if(w->vanished && !e.title.isEmpty())
                e.state = ST_VANISHED;
            else
                e.state = ST_FINISHED;
            e.note = w->filename;
            m_view.push_back(e);
        }

        if(!m_world->title.isEmpty())
        {
            TrView e;
            e.source = s;
            e.type = WDT_TITLE;
            e.title = m_world->title;
            e.key = 0;
            if(e.title.isEmpty())
                e.state = ST_BLANK;
            else if(m_world->title_unfinished && !e.title.isEmpty())
                e.state = ST_UNFINISHED;
            else
                e.state = ST_FINISHED;
            e.note = tr("Episode title");
            m_view.push_back(e);
        }

        if(!m_world->credits.isEmpty())
        {
            TrView e;
            e.source = s;
            e.type = WDT_TITLE;
            e.title = m_world->credits;
            e.key = 0;
            if(e.title.isEmpty())
                e.state = ST_BLANK;
            else if(m_world->credits_unfinished && !e.title.isEmpty())
                e.state = ST_UNFINISHED;
            else
                e.state = ST_FINISHED;
            e.note = tr("Episode credits");
            m_view.push_back(e);
        }

        std::sort(m_view.begin(), m_view.end(),
                  [](const TrView&o1, const TrView&o2)->bool
        {
            return o1.title.compare(o2.title, Qt::CaseInsensitive) < 0;
        });

        break;

    case S_LEVEL:
        m_level = &(*m_project)[lang].levels[key];
        for(auto w = m_level->events.begin() ; w != m_level->events.end(); ++w)
        {
            TrView e;
            e.source = s;
            e.type = LDT_EVENT;
            e.title = w->message;
            e.key = w.key();
            if(e.title.isEmpty())
                e.state = ST_BLANK;
            else if(w->unfinished && !e.title.isEmpty())
                e.state = ST_UNFINISHED;
            else if(w->vanished && !e.title.isEmpty())
                e.state = ST_VANISHED;
            else
                e.state = ST_FINISHED;
            e.note = QString::number(w->event_index);
            m_view.push_back(e);
        }

        for(auto w = m_level->npc.begin() ; w != m_level->npc.end(); ++w)
        {
            TrView e;
            e.source = s;
            e.type = LDT_NPC;
            e.title = w->talk;
            e.key = w.key();
            if(e.title.isEmpty())
                e.state = ST_BLANK;
            else if(w->unfinished && !e.title.isEmpty())
                e.state = ST_UNFINISHED;
            else if(w->vanished && !e.title.isEmpty())
                e.state = ST_VANISHED;
            else
                e.state = ST_FINISHED;
            e.note = QString("NPC-%1 [idx=%2]").arg(w->npc_id).arg(w->npc_index);
            m_view.push_back(e);
        }

        if(!m_level->title.isEmpty())
        {
            TrView e;
            e.source = s;
            e.type = LDT_TITLE;
            e.title = m_level->title;
            e.key = 0;
            if(e.title.isEmpty())
                e.state = ST_BLANK;
            else if(m_level->title_unfinished && !e.title.isEmpty())
                e.state = ST_UNFINISHED;
            else
                e.state = ST_FINISHED;
            e.note = tr("Level title");
            m_view.push_back(e);
        }

        std::sort(m_view.begin(), m_view.end(),
                  [](const TrView&o1, const TrView&o2)->bool
        {
            return o1.title.compare(o2.title, Qt::CaseInsensitive) < 0;
        });

        break;

    case S_SCRIPT:
        m_script = &(*m_project)[lang].scripts[key];
        for(auto w = m_script->lines.begin() ; w != m_script->lines.end(); ++w)
        {
            TrView e;
            e.source = s;
            e.type = SDT_LINE;
            e.title = w->source;
            e.key = w.key();
            if(e.title.isEmpty())
                e.state = ST_BLANK;
            else if(w->unfinished && !e.title.isEmpty())
                e.state = ST_UNFINISHED;
            else if(w->vanished && !e.title.isEmpty())
                e.state = ST_VANISHED;
            else
                e.state = ST_FINISHED;
            e.note = QString::number(w->line);
            m_view.push_back(e);
        }

        std::sort(m_view.begin(), m_view.end(),
                  [](const TrView&o1, const TrView&o2)->bool
        {
            return o1.title.compare(o2.title, Qt::CaseInsensitive) < 0;
        });

        break;
    }

    endResetModel();
}

void FilesStringsModel::clear()
{
    beginResetModel();
    m_level = nullptr;
    m_world = nullptr;
    m_script = nullptr;
    endResetModel();
}

QVariant FilesStringsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
    case C_TYPE:
        return tr("Type");
    case C_STATE:
        return tr("Satate");
    case C_TITLE:
        return tr("Title");
    case C_NOTE:
        return tr("Note");
    }

    return QVariant();
}

void FilesStringsModel::sort(int column, Qt::SortOrder order)
{
    switch(column)
    {
    case C_TYPE:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                return o1.type > o2.type;
            });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                return o1.type < o2.type;
            });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;

    case C_STATE:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                return o1.state > o2.state;
            });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                return o1.state < o2.state;
            });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;

    case C_TITLE:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                return o1.title.compare(o2.title, Qt::CaseInsensitive) < 0;
            });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                return o1.title.compare(o2.title, Qt::CaseInsensitive) > 0;
            });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;

    case C_NOTE:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                return o1.note.compare(o2.note, Qt::CaseInsensitive) < 0;
            });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                return o1.note.compare(o2.note, Qt::CaseInsensitive) > 0;
            });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;
    }
}

int FilesStringsModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_view.size();
}

int FilesStringsModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return C_COUNT;
}

QVariant FilesStringsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= m_view.size())
        return QVariant();

    auto &it = m_view[index.row()];

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case C_TYPE:
            switch(it.source)
            {
            case S_WORLD:
                switch(it.type)
                {
                case WDT_LEVEL:
                    return tr("L");
                case WDT_TITLE:
                    return tr("T");
                case WDT_CREDITS:
                    return tr("C");
                }
                break;
            case S_LEVEL:
                switch(it.type)
                {
                case LDT_EVENT:
                    return tr("E");
                case LDT_NPC:
                    return tr("N");
                case LDT_TITLE:
                    return tr("T");
                }
                break;
            case S_SCRIPT:
                switch(it.type)
                {
                case SDT_LINE:
                    return tr("L");
                }
                break;
            }
            break;

        case C_STATE:
            switch(it.state)
            {
            case ST_BLANK:
                return tr("B");
            case ST_UNFINISHED:
                return tr("U");
            case ST_FINISHED:
                return tr("F");
            case ST_VANISHED:
                return tr("V");
            }
            break;

        case C_TITLE:
            return it.title;

        case C_NOTE:
            return it.note;
        }


        break;
    }

    // FIXME: Implement me!
    return QVariant();
}
