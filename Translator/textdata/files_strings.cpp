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

#include <QColor>
#include <QPalette>
#include <QApplication>

#include "files_strings.h"

int FilesStringsModel::stateFromTrLine(const TrLine &line)
{
    if(line.text.isEmpty())
        return TextTypes::ST_BLANK;
    else if(line.unfinished && !line.text.isEmpty())
        return TextTypes::ST_UNFINISHED;
    else if(line.vanished && !line.text.isEmpty())
        return TextTypes::ST_VANISHED;
    else
        return TextTypes::ST_FINISHED;
}

FilesStringsModel::FilesStringsModel(TranslateProject *project, QObject *parent)
    : QAbstractTableModel(parent)
    , m_project(project)
{}

void FilesStringsModel::setFileData(const QString &lang, int s, const QString &key)
{
    beginResetModel();

    m_level = nullptr;
    m_world = nullptr;
    m_script = nullptr;
    m_recentSection = -1;
    m_recentKey.clear();
    m_view.clear();

    TranslationData &origin = (*m_project)["metadata"];
    TranslationData &cur = (*m_project)[lang];
    const TranslationData_Level *cur_level = nullptr;
    const TranslationData_World *cur_world = nullptr;
    const TranslationData_Script *cur_script = nullptr;

    switch(s)
    {
    case TextTypes::S_WORLD:
        m_recentSection = s;
        m_recentKey = key;
        m_world = &origin.worlds[key];
        cur_world = cur.worlds.contains(key) ? &cur.worlds[key] : &origin.worlds[key];

        for(auto w = m_world->level_titles.cbegin() ; w != m_world->level_titles.cend(); ++w)
        {
            TrView e;
            const auto &cur_item = cur_world->level_titles.contains(w.key()) ?
                                       cur_world->level_titles[w.key()] :
                                       w.value();
            e.source = s;
            e.type = TextTypes::WDT_LEVEL;
            e.title = w->title.text;
            e.tr_note = w->title.note;
            e.root = key;
            e.key = w.key();
            e.state = stateFromTrLine(cur_item.title);
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
            e.state = stateFromTrLine(cur_world->title);
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
            e.state = stateFromTrLine(cur_world->credits);
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
        m_recentSection = s;
        m_recentKey = key;
        m_level = &origin.levels[key];
        cur_level = cur.levels.contains(key) ? &cur.levels[key] : &origin.levels[key];

        for(auto w = m_level->events.cbegin() ; w != m_level->events.cend(); ++w)
        {
            TrView e;
            const auto &cur_item = cur_level->events.contains(w.key()) ?
                                       cur_level->events[w.key()] :
                                       w.value();

            e.source = s;
            e.type = TextTypes::LDT_EVENT;
            e.title = w->message.text;
            e.tr_note = w->message.note;
            e.root = key;
            e.key = w.key();
            e.state = stateFromTrLine(cur_item.message);
            e.note = QString::number(w->event_index);
            e.note = QString("Event-%1").arg(w->event_index);
            m_view.push_back(e);
        }

        for(auto w = m_level->npc.begin() ; w != m_level->npc.end(); ++w)
        {
            TrView e;
            const auto &cur_item = cur_level->npc.contains(w.key()) ?
                                       cur_level->npc[w.key()] :
                                       w.value();

            e.source = s;
            e.type = TextTypes::LDT_NPC;
            e.title = w->talk.text;
            e.root = key;
            e.tr_note = w->talk.note;
            e.key = w.key();
            e.state = stateFromTrLine(cur_item.talk);
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
            e.state = stateFromTrLine(cur_level->title);
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
        m_recentSection = s;
        m_recentKey = key;
        m_script = &origin.scripts[key];
        cur_script = cur.scripts.contains(key) ? &cur.scripts[key] : &origin.scripts[key];

        for(auto w = m_script->lines.cbegin() ; w != m_script->lines.cend(); ++w)
        {
            TrView e;
            const auto &cur_item = cur_script->lines.contains(w.key()) ?
                                       cur_script->lines[w.key()] :
                                       w.value();

            e.source = s;
            e.type = TextTypes::SDT_LINE;
            e.title = w->translation.text;
            e.tr_note = w->translation.note;
            e.root = key;
            e.key = w.key();
            e.state = stateFromTrLine(cur_item.translation);
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

void FilesStringsModel::updateStatus(const QString &lang)
{
    if(m_recentSection < 0 || m_view.isEmpty())
        return;

    const QString &key = m_recentKey;
    TranslationData &origin = (*m_project)["metadata"];
    TranslationData &cur = (*m_project)[lang];
    const TranslationData_Level *cur_level = nullptr;
    const TranslationData_World *cur_world = nullptr;
    const TranslationData_Script *cur_script = nullptr;
    int i;

    switch(m_recentSection)
    {
    case TextTypes::S_WORLD:
        cur_world = cur.worlds.contains(key) ? &cur.worlds[key] : &origin.worlds[key];
        i = 0;

        for(auto it = m_view.begin(); it != m_view.end(); ++it, ++i)
        {
            TrView &o = *it;
            switch(o.type)
            {
            case TextTypes::WDT_LEVEL:
                if(!cur_world->level_titles.contains(o.key))
                    continue;
                o.state = stateFromTrLine(cur_world->level_titles[o.key].title);
                emit dataChanged(index(i, C_STATE), index(i, C_STATE));
                break;

            case TextTypes::WDT_TITLE:
                o.state = stateFromTrLine(cur_world->title);
                emit dataChanged(index(i, C_STATE), index(i, C_STATE));
                break;

            case TextTypes::WDT_CREDITS:
                o.state = stateFromTrLine(cur_world->credits);
                emit dataChanged(index(i, C_STATE), index(i, C_STATE));
                break;
            }
        }
        break;

    case TextTypes::S_LEVEL:
        cur_level = cur.levels.contains(key) ? &cur.levels[key] : &origin.levels[key];
        i = 0;

        for(auto it = m_view.begin(); it != m_view.end(); ++it, ++i)
        {
            TrView &o = *it;
            switch(o.type)
            {
            case TextTypes::LDT_EVENT:
                if(!cur_level->events.contains(o.key))
                    continue;
                o.state = stateFromTrLine(cur_level->events[o.key].message);
                emit dataChanged(index(i, C_STATE), index(i, C_STATE));
                break;

            case TextTypes::LDT_NPC:
                if(!cur_level->npc.contains(o.key))
                    continue;
                o.state = stateFromTrLine(cur_level->npc[o.key].talk);
                emit dataChanged(index(i, C_STATE), index(i, C_STATE));
                break;

            case TextTypes::LDT_TITLE:
                o.state = stateFromTrLine(cur_level->title);
                emit dataChanged(index(i, C_STATE), index(i, C_STATE));
                break;
            }
        }
        break;

    case TextTypes::S_SCRIPT:
        cur_script = cur.scripts.contains(key) ? &cur.scripts[key] : &origin.scripts[key];
        i = 0;

        for(auto it = m_view.begin(); it != m_view.end(); ++it, ++i)
        {
            TrView &o = *it;
            switch(o.type)
            {
            case TextTypes::SDT_LINE:
                if(!cur_script->lines.contains(o.key))
                    continue;
                o.state = stateFromTrLine(cur_script->lines[o.key].translation);
                emit dataChanged(index(i, C_STATE), index(i, C_STATE));
                break;
            }
        }

        break;
    }
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
//            return tr("State");
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

    case Qt::ToolTipRole:
        switch(index.column())
        {
        case C_TYPE:
            switch(it.source)
            {
            case TextTypes::S_WORLD:
                switch(it.type)
                {
                case TextTypes::WDT_LEVEL:
                    return tr("Level title");
                case TextTypes::WDT_TITLE:
                    return tr("Episode title");
                case TextTypes::WDT_CREDITS:
                    return tr("Episode credits");
                }
                break;
            case TextTypes::S_LEVEL:
                switch(it.type)
                {
                case TextTypes::LDT_EVENT:
                    return tr("Event's message box");
                case TextTypes::LDT_NPC:
                    return tr("NPC's dialogue message");
                case TextTypes::LDT_TITLE:
                    return tr("Level file's title");
                }
                break;
            case TextTypes::S_SCRIPT:
                switch(it.type)
                {
                case TextTypes::SDT_LINE:
                    return tr("Textual string");
                }
                break;
            }
            break;
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

    case Qt::ForegroundRole:
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
