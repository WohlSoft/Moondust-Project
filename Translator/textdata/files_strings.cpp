#include <QColor>
#include <QPalette>
#include <QApplication>

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
    case TextTypes::S_WORLD:
        m_world = &(*m_project)[lang].worlds[key];
        for(auto w = m_world->level_titles.begin() ; w != m_world->level_titles.end(); ++w)
        {
            TrView e;
            e.source = s;
            e.type = TextTypes::WDT_LEVEL;
            e.title = w->title.text;
            e.tr_note = w->title.note;
            e.root = key;
            e.key = w.key();
            if(e.title.isEmpty())
                e.state = TextTypes::ST_BLANK;
            else if(w->title.unfinished && !e.title.isEmpty())
                e.state = TextTypes::ST_UNFINISHED;
            else if(w->title.vanished && !e.title.isEmpty())
                e.state = TextTypes::ST_VANISHED;
            else
                e.state = TextTypes::ST_FINISHED;
            e.note = w->filename;
            m_view.push_back(e);
        }

        if(!m_world->title.text.isEmpty())
        {
            TrView e;
            e.source = s;
            e.type = TextTypes::WDT_TITLE;
            e.title = m_world->title.text;
            e.tr_note = m_world->title.note;
            e.root = key;
            e.key = 0;
            if(e.title.isEmpty())
                e.state = TextTypes::ST_BLANK;
            else if(m_world->title.unfinished && !e.title.isEmpty())
                e.state = TextTypes::ST_UNFINISHED;
            else
                e.state = TextTypes::ST_FINISHED;
            e.note = tr("Episode title");
            m_view.push_back(e);
        }

        if(!m_world->credits.text.isEmpty())
        {
            TrView e;
            e.source = s;
            e.type = TextTypes::WDT_CREDITS;
            e.title = m_world->credits.text;
            e.tr_note = m_world->credits.note;
            e.root = key;
            e.key = 0;
            if(e.title.isEmpty())
                e.state = TextTypes::ST_BLANK;
            else if(m_world->credits.unfinished && !e.title.isEmpty())
                e.state = TextTypes::ST_UNFINISHED;
            else
                e.state = TextTypes::ST_FINISHED;
            e.note = tr("Episode credits");
            m_view.push_back(e);
        }

        std::sort(m_view.begin(), m_view.end(),
                  [](const TrView&o1, const TrView&o2)->bool
        {
            return o1.title.compare(o2.title, Qt::CaseInsensitive) < 0;
        });

        break;

    case TextTypes::S_LEVEL:
        m_level = &(*m_project)[lang].levels[key];
        for(auto w = m_level->events.begin() ; w != m_level->events.end(); ++w)
        {
            TrView e;
            e.source = s;
            e.type = TextTypes::LDT_EVENT;
            e.title = w->message.text;
            e.tr_note = w->message.note;
            e.root = key;
            e.key = w.key();
            if(e.title.isEmpty())
                e.state = TextTypes::ST_BLANK;
            else if(w->message.unfinished && !e.title.isEmpty())
                e.state = TextTypes::ST_UNFINISHED;
            else if(w->message.vanished && !e.title.isEmpty())
                e.state = TextTypes::ST_VANISHED;
            else
                e.state = TextTypes::ST_FINISHED;
            e.note = QString::number(w->event_index);
            e.note = QString("Event-%1").arg(w->event_index);
            m_view.push_back(e);
        }

        for(auto w = m_level->npc.begin() ; w != m_level->npc.end(); ++w)
        {
            TrView e;
            e.source = s;
            e.type = TextTypes::LDT_NPC;
            e.title = w->talk.text;
            e.root = key;
            e.tr_note = w->talk.note;
            e.key = w.key();
            if(e.title.isEmpty())
                e.state = TextTypes::ST_BLANK;
            else if(w->talk.unfinished && !e.title.isEmpty())
                e.state = TextTypes::ST_UNFINISHED;
            else if(w->talk.vanished && !e.title.isEmpty())
                e.state = TextTypes::ST_VANISHED;
            else
                e.state = TextTypes::ST_FINISHED;
            e.note = QString("NPC-%1 [idx=%2]").arg(w->npc_id).arg(w->npc_index);
            m_view.push_back(e);
        }

        if(!m_level->title.text.isEmpty())
        {
            TrView e;
            e.source = s;
            e.type = TextTypes::LDT_TITLE;
            e.title = m_level->title.text;
            e.tr_note = m_level->title.note;
            e.root = key;
            e.key = 0;
            if(e.title.isEmpty())
                e.state = TextTypes::ST_BLANK;
            else if(m_level->title.unfinished && !e.title.isEmpty())
                e.state = TextTypes::ST_UNFINISHED;
            else
                e.state = TextTypes::ST_FINISHED;
            e.note = tr("Level title");
            m_view.push_back(e);
        }

        std::sort(m_view.begin(), m_view.end(),
                  [](const TrView&o1, const TrView&o2)->bool
        {
            return o1.title.compare(o2.title, Qt::CaseInsensitive) < 0;
        });

        break;

    case TextTypes::S_SCRIPT:
        m_script = &(*m_project)[lang].scripts[key];
        for(auto w = m_script->lines.begin() ; w != m_script->lines.end(); ++w)
        {
            TrView e;
            e.source = s;
            e.type = TextTypes::SDT_LINE;
            e.title = w->translation.text;
            e.tr_note = w->translation.note;
            e.root = key;
            e.key = w.key();
            if(e.title.isEmpty())
                e.state = TextTypes::ST_BLANK;
            else if(w->translation.unfinished && !e.title.isEmpty())
                e.state = TextTypes::ST_UNFINISHED;
            else if(w->translation.vanished && !e.title.isEmpty())
                e.state = TextTypes::ST_VANISHED;
            else
                e.state = TextTypes::ST_FINISHED;
            e.note = e.note = QString("Line %1").arg(w->line);
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
    m_view.clear();
    endResetModel();
}

QVariant FilesStringsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role != Qt::DisplayRole && role != Qt::DecorationRole)
        return QVariant();

    switch(role)
    {
    case Qt::DecorationRole:
        switch(section)
        {
        case C_TYPE:
            return tr("Type");
        case C_STATE:
            return QIcon(":/images/s_check_on.png");
        case C_TITLE:
            return tr("Title");
        case C_NOTE:
            return tr("Note");
        }
        break;
    case Qt::DisplayRole:
        switch(section)
        {
        case C_TYPE:
            return tr("Type");
//        case C_STATE:
//            return tr("Satate");
        case C_TITLE:
            return tr("Title");
        case C_NOTE:
            return tr("Note");
        }
        break;
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
                if(o1.source == TextTypes::S_SCRIPT)
                    return o1.key < o2.key;
                else
                    return o1.note.compare(o2.note, Qt::CaseInsensitive) < 0;
            });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const TrView&o1, const TrView&o2)->bool
            {
                if(o1.source == TextTypes::S_SCRIPT)
                    return o1.key > o2.key;
                else
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
            case TextTypes::S_WORLD:
                switch(it.type)
                {
                case TextTypes::WDT_LEVEL:
                    return tr("L");
                case TextTypes::WDT_TITLE:
                    return tr("T");
                case TextTypes::WDT_CREDITS:
                    return tr("C");
                }
                break;
            case TextTypes::S_LEVEL:
                switch(it.type)
                {
                case TextTypes::LDT_EVENT:
                    return tr("E");
                case TextTypes::LDT_NPC:
                    return tr("N");
                case TextTypes::LDT_TITLE:
                    return tr("T");
                }
                break;
            case TextTypes::S_SCRIPT:
                switch(it.type)
                {
                case TextTypes::SDT_LINE:
                    return tr("L");
                }
                break;
            }
            break;

        case C_TITLE:
            return it.title;

        case C_NOTE:
            switch(it.source)
            {
            case TextTypes::S_WORLD:
                return it.note.isEmpty() ? tr("<No filename>") : it.note;
            default:
                return it.note;
            }
        }

        break;

    case Qt::DecorationRole:
        switch(index.column())
        {
        case C_STATE:
            switch(it.state)
            {
            case TextTypes::ST_BLANK:
                return QIcon(":/images/s_check_empty.png");
            case TextTypes::ST_UNFINISHED:
                return QIcon(":/images/s_check_off.png");
            case TextTypes::ST_FINISHED:
                return QIcon(":/images/s_check_on.png");
            case TextTypes::ST_VANISHED:
                return QIcon(":/images/s_check_obsolete.png");
            }
            break;
        }

        break;

    case Qt::TextColorRole:
        switch(index.column())
        {
        case C_NOTE:
            return it.note.isEmpty() ? QColor(Qt::gray) : QApplication::palette().text().color();
        }
        break;

    case R_NOTE:
        return it.tr_note;

    case R_GROUP:
        return it.source;
    case R_ROOT:
        return it.root;
    case R_TYPE:
        return it.type;
    case R_KEY:
        return it.key;
    }

    // FIXME: Implement me!
    return QVariant();
}
