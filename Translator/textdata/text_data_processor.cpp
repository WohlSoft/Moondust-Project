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

#include <cstdlib>
#include <cstring>

#include <QDir>
#include <QFile>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#   include <QRegExp>
#else
#   include <QRegularExpression>
#endif
#include <QFileInfo>
#include <QTextStream>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include <QFileInfo>
#include <PGE_File_Formats/file_formats.h>

#include "text_types.h"
#include "text_data_processor.h"


static QStringList s_getRegExMatches(const QString &pattern, const QString &input)
{
    QStringList ret;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp fe(pattern, Qt::CaseInsensitive, QRegExp::RegExp2);
    if(fe.exactMatch(input))
        ret = fe.capturedTexts();
#else
    QRegularExpression fe(pattern, QRegularExpression::CaseInsensitiveOption);
    auto m = fe.match(input);
    if(m.hasMatch())
        ret = m.capturedTexts();
#endif

    return ret;
}


/*!
 * \brief Follow the chain of event triggers and find a first event with a message text if possible
 * \param events Events list
 * \param trigger Triggered event name
 * \param p_triggered Registory of followed indeces used to break the loop
 * \param prev_j (recursive use only): the event value where recursive call was sent
 * \return Found index or -1 if found nothing
 */
static int find_event_index(PGELIST<LevelSMBX64Event > &events,
                            const QString &trigger,
                            QSet<int> *p_triggered = nullptr,
                            int prev_j = -1)
{
    QSet<int> triggered;

    if(!p_triggered)
        p_triggered = &triggered;

    for(int j = 0; j < events.size(); ++j)
    {
        auto &e = events[j];
        if(e.name.compare(trigger, Qt::CaseInsensitive) == 0)
        {
            if(p_triggered->contains(j))
                return prev_j; // Avoid infinite loops

            if(e.msg.isEmpty() && !e.trigger.isEmpty()) // If this event contains no message, follow the chain
            {
                p_triggered->insert(j);
                return find_event_index(events, e.trigger, p_triggered, j);
            }

            return j;
        }
    }

    return -1;
}

#ifdef DEBUG_BUILD
static bool validate(const QMap<int, TranslationData_EVENT> &d)
{
    bool ret = true;
    for(auto i = d.begin(); i != d.end(); ++i)
    {
        Q_ASSERT(i.key() >= 0);
        ret &= (i.key() == i.value().event_index);
        Q_ASSERT(ret);
        if(!ret)
            qWarning() << QString("Caught an event entry %1 with %1 index inside").arg(i.key()).arg(i.value().event_index);
    }

    if(ret)
        qDebug() << "Events array is valid";
    else
        qWarning() << "Events array is INVALID";

    return ret;
}

static bool validate(const QMap<int, TranslationData_NPC> &d)
{
    bool ret = true;
    for(auto i = d.begin(); i != d.end(); ++i)
    {
        Q_ASSERT(i.key() >= 0);
        ret &= (i.key() == i.value().npc_index);
        Q_ASSERT(ret);
        if(!ret)
            qWarning() << QString("Caught an NPC entry %1 with %1 index inside").arg(i.key()).arg(i.value().npc_index);
    }

    if(ret)
        qDebug() << "NPC array is valid";
    else
        qWarning() << "NPC array is INVALID";

    return ret;
}
#endif


TextDataProcessor::TextDataProcessor()
{}

void TextDataProcessor::toI18N(const QString &directory)
{
    QDir i18n(directory + "/i18n");
    if(i18n.exists())
        return;

    i18n.cdUp();
    i18n.mkdir("i18n");

    QDirIterator dit(directory,
                     {"translation_*.json"},
                     QDir::Files,
                     QDirIterator::FollowSymlinks);

    while(dit.hasNext())
    {
        auto f = dit.next();
        QFileInfo fi(f);
        QFile::rename(f, directory + "/i18n/" + fi.fileName());
    }
}

bool TextDataProcessor::loadProject(const QString &directory, TranslateProject &proj)
{
    // Close current project
    proj.clear();

    // Update the translation structure
    toI18N(directory);

    QDirIterator dit(directory + "/i18n",
                     {"translation_*.json"},
                     QDir::Files,
                     QDirIterator::FollowSymlinks);
    bool hasTranslations = false;

    QString meta_file = directory + "/i18n/translation_metadata.json";
    QString meta_file_legacy = directory + "/i18n/translation_origin.json";

    if(!QFile::exists(meta_file) && QFile::exists(meta_file_legacy))
        meta_file = meta_file_legacy; // Load legacy translation if presented

    if(QFile::exists(meta_file)) // Load metafile first
        loadTranslation(proj, "metadata", meta_file);

    while(dit.hasNext())
    {
        auto f = dit.next();
        QFileInfo fi(f);
        QStringList cap = s_getRegExMatches("translation_([a-zA-Z-]*)\\.json", fi.fileName());

        if(cap.size() >= 2 && cap[1] != "origin" && cap[1] != "metadata")
        {
            loadTranslation(proj, cap[1], f);
            hasTranslations = true;
        }
    }

    if(!hasTranslations)
        return scanEpisode(directory, proj);

    return true;
}

bool TextDataProcessor::saveProject(const QString &directory, TranslateProject &proj)
{
    return saveJSONs(directory, proj);
}

bool TextDataProcessor::loadProjectLevel(const QString &file, TranslateProject &proj)
{
    QFileInfo f(file);
    QDir f_d = f.absoluteDir();
    QString directory = f_d.absolutePath() + "/" + f.completeBaseName();

    // Close current project
    proj.clear();

    // Update the translation structure
    toI18N(directory);

    QDirIterator dit(directory + "/i18n",
                     {"translation_*.json"},
                     QDir::Files,
                     QDirIterator::FollowSymlinks);

    bool hasTranslations = false;

    QString meta_file = directory + "/i18n/translation_metadata.json";
    QString meta_file_legacy = directory + "/i18n/translation_origin.json";

    if(QFile::exists(meta_file)) // Load metafile first
        loadTranslation(proj, "metadata", meta_file);

    if(!QFile::exists(meta_file) && QFile::exists(meta_file_legacy))
        meta_file = meta_file_legacy; // Load legacy translation if presented

    while(dit.hasNext())
    {
        auto f = dit.next();
        QFileInfo fi(f);
        QStringList cap = s_getRegExMatches("translation_([a-zA-Z-]*)\\.json", fi.fileName());

        if(cap.size() >= 2 && cap[1] != "origin" && cap[1] != "metadata")
        {
            loadTranslation(proj, cap[1], f);
            hasTranslations = true;
        }
    }

    if(!hasTranslations)
        return scanSingleLevel(file, proj);

    return true;
}

bool TextDataProcessor::saveProjectLevel(const QString &file, TranslateProject &proj)
{
    QFileInfo f(file);
    QString directory = f.absoluteDir().absolutePath() + "/" + f.completeBaseName();
    return saveJSONs(directory, proj);
}

bool TextDataProcessor::scanEpisode(const QString &directory, TranslateProject &proj)
{
    QDir d(directory);
    QDirIterator dit(directory,
                     {
                         "*.lvl", "*.lvlx",
                         "*.wld", "*.wldx",
                         "lunadll.txt", "lunaworld.txt"
                     },
                     QDir::Files,
                     QDirIterator::Subdirectories|QDirIterator::FollowSymlinks);

    // Fill the original language
    auto &origin = proj["metadata"];

    while(dit.hasNext())
    {
        auto f = dit.next();
        auto rfp = d.relativeFilePath(f);

        qDebug() << "\n\n\n" << rfp;

        // Levels
        if(f.endsWith(".lvl", Qt::CaseInsensitive) || f.endsWith(".lvlx", Qt::CaseInsensitive))
            importLevel(origin, f, rfp);

        // World maps
        else if(f.endsWith(".wld", Qt::CaseInsensitive) || f.endsWith(".wldx", Qt::CaseInsensitive))
            importWorld(origin, f, rfp);

        // Scripts
        else if(f.endsWith(".txt", Qt::CaseInsensitive))
            importScript(origin, f, rfp);
    }

    // Update existing language entries
    for(auto l = proj.begin(); l != proj.end(); ++l)
    {
        if(l.key() == "metadata")
            continue;

        updateTranslation(proj, l.key());
    }

    saveJSONs(directory, proj);

    return true;
}

bool TextDataProcessor::scanSingleLevel(const QString &file, TranslateProject &proj)
{
    QFileInfo f(file);
    QDir f_d = f.absoluteDir();
    QString directory = f_d.absolutePath() + "/" + f.completeBaseName();

    // Fill the original language
    auto &origin = proj["metadata"];

    // Import level
    importLevel(origin, file, f_d.relativeFilePath(file));

    // Import lunadll script if presented
    QString lunaTxtPath = directory + "/lunadll.txt";
    if(QFile::exists(lunaTxtPath))
        importScript(origin, lunaTxtPath, f_d.relativeFilePath(lunaTxtPath));

    saveJSONs(directory, proj);

    return true;
}

bool TextDataProcessor::createTranslation(TranslateProject &proj, const QString &lang)
{
    updateTranslation(proj, lang);
    return true;
}

TrLine &TextDataProcessor::getItemRef(TranslateProject &proj, const QString &lang, const QString &root, int group, int type, int key, bool *ok)
{
    static TrLine errored;

    if(ok)
        *ok = true;

    try
    {
        if(!proj.contains(lang))
            throw std::logic_error("Language missing");

        auto &l = proj[lang];

        switch(group)
        {
        case TextTypes::S_WORLD:
        {
            if(!l.worlds.contains(root))
                throw std::logic_error("World entry (group) is missing");

            auto &r = l.worlds[root];

            switch(type)
            {
            case TextTypes::WDT_LEVEL:
                if(!r.level_titles.contains(key))
                    std::logic_error("Level key is not found");

                return r.level_titles[key].title;

            case TextTypes::WDT_TITLE:
                return r.title;

            case TextTypes::WDT_CREDITS:
                return r.credits;

            default:
                std::logic_error("Invalid world item type");
            }

            break;
        }
        case TextTypes::S_LEVEL:
        {
            if(!l.levels.contains(root))
                throw std::logic_error("Level entry (group) is missing");

            auto &r = l.levels[root];

            switch(type)
            {
            case TextTypes::LDT_EVENT:
                if(!r.events.contains(key))
                    std::logic_error("Event key is not found");

                return r.events[key].message;

            case TextTypes::LDT_NPC:
                if(!r.npc.contains(key))
                    std::logic_error("NPCkey is not found");

                return r.npc[key].talk;

            case TextTypes::LDT_TITLE:
                return r.title;

            default:
                std::logic_error("Invalid level item type");
            }
            break;
        }
        case TextTypes::S_SCRIPT:
        {
            if(!l.scripts.contains(root))
                throw std::logic_error("Script entry (group) is missing");

            auto &r = l.scripts[root];

            switch(type)
            {
            case TextTypes::SDT_LINE:
                if(!r.lines.contains(key))
                    std::logic_error("NPCkey is not found");

                return r.lines[key].translation;

            default:
                std::logic_error("Invalid level item type");
            }
            break;
        }
        default:
            throw std::logic_error("Invalid file group");
        }

    }
    catch(const std::exception &)
    {
        if(ok)
            *ok = false;
        return errored;
    }

    return errored;
}

void TextDataProcessor::importLevel(TranslationData &origin, const QString &path, const QString &shortPath)
{
    bool hasStrings = false;
    LevelData l;
    TranslationData_Level tr;

    if(!FileFormats::OpenLevelFile(path, l))
    {
        qWarning() << "Failed to load the level: " << l.meta.ERROR_info
                   << "at line" << l.meta.ERROR_linenum
                   << "contents: " << l.meta.ERROR_linedata;
        return;
    }

    tr.title.text = l.LevelName;
    hasStrings |= !tr.title.text.isEmpty();

    if(!tr.title.text.isEmpty())
        qDebug() << "Level Title:" << tr.title.text;

    for(int i = 0; i < l.events.size(); ++i)
    {
        auto &ed = l.events[i];
        TranslationData_EVENT e;

        if(ed.msg.isEmpty())
            continue;

        hasStrings |= true;

        e.event_index = i;
        e.message.text = ed.msg;
        if(!ed.trigger.isEmpty()) // Find index of the trigger
            e.trigger_next = find_event_index(l.events, ed.trigger);
        tr.events.insert(i, e);
        qDebug() << "Event" << i << "message: [" << e.message.text << "]" << "Trigger" << e.trigger_next;
    }

#ifdef DEBUG_BUILD
    validate(tr.events);
#endif

    for(int i = 0; i < l.npc.size(); ++i)
    {
        auto &nd = l.npc[i];
        TranslationData_NPC n;

        if(nd.msg.isEmpty())
            continue; // Don't include NPCs with no messages

        hasStrings |= true;

        n.npc_index = i;
        n.npc_id = (int)nd.id;
        n.talk.text = nd.msg;
        if(!nd.event_talk.isEmpty())
            n.talk_trigger = find_event_index(l.events, nd.event_talk);

        tr.npc.insert(i, n);
        qDebug() << "NPC-" << n.npc_id << "idx" << i << "talk: [" << n.talk.text << "]" << "Trigger" << n.talk_trigger;
    }

#ifdef DEBUG_BUILD
    validate(tr.npc);
#endif

    // Build relations
    for(auto &e : tr.events)
    {
        for(auto &sub_e : tr.events)
        {
            if(sub_e.trigger_next == e.event_index)
                e.triggered_by_event.push_back(sub_e.event_index);
        }

        for(auto &n : tr.npc)
        {
            if(n.talk_trigger == e.event_index)
                e.triggered_by_npc.push_back(n.npc_index);
        }
    }

#ifdef DEBUG_BUILD
    validate(tr.npc);
    validate(tr.events);
#endif

    // Build dialogues
    for(auto &n : tr.npc)
    {
        TranslationData_Dialogue d;
        TranslationData_DialogueNode dn;
        dn.type = TranslationData_DialogueNode::T_NPC_TALK;
        dn.item_index = n.npc_index;

        if(!n.talk.text.isEmpty())
            d.messages.push_back(dn);

        if(n.talk_trigger >= 0 && tr.events.contains(n.talk_trigger))
        {
            QSet<int> triggered;
            int next_trigger = n.talk_trigger;
            do
            {
                Q_ASSERT(next_trigger >= 0);
                auto &e = tr.events[next_trigger];

                TranslationData_DialogueNode de;
                de.type = TranslationData_DialogueNode::T_EVENT_MSG;
                de.item_index = e.event_index;
                if(!e.message.text.isEmpty())
                    d.messages.push_back(de);

                triggered.insert(next_trigger);
                if(e.trigger_next >= 0 && !tr.events.contains(e.trigger_next))
                    e.trigger_next = -1;
                next_trigger = e.trigger_next;
            } while(next_trigger >= 0 && !triggered.contains(next_trigger));
        }

#ifdef DEBUG_BUILD
        validate(tr.npc);
        validate(tr.events);
#endif

        if(!d.messages.isEmpty())
        {
            dn.type = TranslationData_DialogueNode::T_END;
            dn.item_index = -1;
            d.messages.push_back(dn);
            tr.dialogues.append(d);
        }
    }

    for(auto &e : tr.events)
    {
        if(!e.triggered_by_event.isEmpty() || !e.triggered_by_npc.isEmpty())
            continue; // Skip events that appears at the middle of dialogue

        TranslationData_Dialogue d;
        TranslationData_DialogueNode dn;
        dn.type = TranslationData_DialogueNode::T_EVENT_MSG;
        dn.item_index = e.event_index;

        if(!e.message.text.isEmpty())
            d.messages.push_back(dn);

        if(e.trigger_next >= 0 && tr.events.contains(e.trigger_next))
        {
            QSet<int> triggered;
            int next_trigger = e.trigger_next;
            do
            {
                Q_ASSERT(next_trigger >= 0);
                auto &se = tr.events[next_trigger];

                TranslationData_DialogueNode de;
                de.type = TranslationData_DialogueNode::T_EVENT_MSG;
                de.item_index = se.event_index;
                if(!se.message.text.isEmpty())
                    d.messages.push_back(de);

                triggered.insert(next_trigger);
                if(se.trigger_next >= 0 && !tr.events.contains(se.trigger_next))
                    se.trigger_next = -1;
                next_trigger = se.trigger_next;

            } while(next_trigger >= 0 && !triggered.contains(next_trigger));
        }

#ifdef DEBUG_BUILD
        validate(tr.npc);
        validate(tr.events);
#endif

        if(!d.messages.isEmpty())
        {
            dn.type = TranslationData_DialogueNode::T_END;
            dn.item_index = -1;
            d.messages.push_back(dn);
            tr.dialogues.append(d);
        }
    }

    if(!tr.dialogues.isEmpty())
    {
        qDebug() << "\n=========== Dialogues: ===========\n";
        for(auto &c : tr.dialogues)
        {
            qDebug() << "-------------------------------------";
            for(auto &m : c.messages)
            {
                switch(m.type)
                {
                case TranslationData_DialogueNode::T_EVENT_MSG:
                    qDebug() << "-" << "EVENT" << tr.events[m.item_index].message.text;
                    Q_ASSERT(m.item_index >= 0);
                    break;
                case TranslationData_DialogueNode::T_NPC_TALK:
                    qDebug() << "-" << "NPC" << tr.npc[m.item_index].talk.text;
                    break;
                case TranslationData_DialogueNode::T_END:
                    qDebug() << "-------------------------------------\n";
                    break;
                default:
                    break;
                }
            }
        }

#ifdef DEBUG_BUILD
        validate(tr.npc);
        validate(tr.events);
#endif
    }

    if(hasStrings)
        origin.levels.insert(shortPath, tr);
    else
    {
        QFileInfo f(path);
        QString fp = f.absoluteDir().absolutePath() + "/" + f.completeBaseName();
        QDir d(fp);
        if(d.exists())
            origin.directories.insert(shortPath);
    }
}

void TextDataProcessor::importWorld(TranslationData &origin, const QString &path, const QString &shortPath)
{
    bool hasStrings = false;
    WorldData w;
    TranslationData_World tr;

    if(!FileFormats::OpenWorldFile(path, w))
    {
        qWarning() << "Failed to load the world: " << w.meta.ERROR_info
                   << "at line" << w.meta.ERROR_linenum
                   << "contents: " << w.meta.ERROR_linedata;
        return;
    }

    tr.title.text = w.EpisodeTitle;
    hasStrings |= !tr.title.text.isEmpty();

    if(!tr.title.text.isEmpty())
        qDebug() << "World Title:" << tr.title.text;

    tr.credits.text = w.authors;
    hasStrings |= !tr.credits.text.isEmpty();

    if(!tr.credits.text.isEmpty())
        qDebug() << "Credits:" << tr.credits.text;

    for(int i = 0; i < w.levels.size(); ++i)
    {
        auto &l = w.levels[i];
        TranslationData_LEVEL ld;

        if(l.title.isEmpty())
            continue; // Skip blank level entries

        ld.level_index = i;
        ld.title.text = l.title;
        ld.filename = l.lvlfile;

        tr.level_titles.insert(i, ld);
        hasStrings |= true;
        qDebug() << "Level" << i << ld.title.text;
    }

    if(hasStrings)
        origin.worlds.insert(shortPath, tr);
    else
    {
        QFileInfo f(path);
        QString fp = f.absoluteDir().absolutePath() + "/" + f.completeBaseName();
        QDir d(fp);
        if(d.exists())
            origin.directories.insert(shortPath);
    }
}

void TextDataProcessor::importScript(TranslationData &origin, const QString &path, const QString &shortPath)
{
    bool hasStrings = false;
    TranslationData_Script tr;
    tr.title = shortPath;

    char wbuf[2000];
    char wmidbuf[2000];
    size_t wbuflen = 0;
    char combuf[150];
    int cur_section = 0;
    double target = 0;
    double param1 = 0;
    double param2 = 0;
    double param3 = 0;
    double length = 0;
    int btarget = 0;
    int bparam1 = 0;
    int bparam2 = 0;
    int bparam3 = 0;
    int blength = 0;

    char wstrbuf[1000];
    char wrefbuf[128];
    int lineNum = 0;

    std::memset(wbuf, 0, 2000);
    std::memset(combuf, 0, 150);
    std::memset(wstrbuf, 0, 1000);
    std::memset(wrefbuf, 0, 128);

    QFile file(path);
    if(!file.open(QIODevice::Text|QIODevice::ReadOnly))
    {
        qWarning() << "Can't open script file" << path << file.errorString();
        return;
    }

    // Skip BOM
    if(file.read(wbuf, 4) == 4)
    {
        // UTF-8 is only supported
        if(std::memcmp(wbuf, "\xEF\xBB\xBF", 3) == 0)
            file.seek(3);
        else
            file.seek(0);
    }

#define PARSE_FMT_STR       " %149[^,], %lf , %lf , %lf , %lf , %lf , %999[^\n]"
#define PARSE_FMT_STR_2     " %149[^,], %i , %i , %i , %i , %i , %999[^\n]"

    while(!file.atEnd())
    {
        std::memset(wbuf, 0, sizeof(wbuf));
        std::memset(wmidbuf, 0, sizeof(wmidbuf));
        std::memset(wstrbuf, 0, sizeof(wstrbuf));
        std::memset(wrefbuf, 0, sizeof(wrefbuf));
        std::memset(combuf, 0, sizeof(combuf));
        target = 0;
        param1 = 0;
        param2 = 0;
        param3 = 0;
        length = 0;
        btarget = 0;
        bparam1 = 0;
        bparam2 = 0;
        bparam3 = 0;
        blength = 0;

        auto wbuf_q = file.readLine(2000);
        if(wbuf_q.isEmpty())
            break; // File has been finished

        std::memcpy(wbuf, wbuf_q.data(), wbuf_q.size());
        lineNum++;

        // Is it a comment?
        char *commentLine = std::strstr(wbuf, "//");
        if(commentLine != nullptr)
            commentLine[0] = '\0'; // Cut the comment line at here

        // does this line contains anything useful?
        wbuflen = std::strlen(wbuf);
        while(wbuflen > 0)
        {
            auto c = wbuf[wbuflen - 1];
            if(c != '\n' && c != '\r' && c != '\t' && c != ' ')
                break;
            wbuf[wbuflen - 1] = '\0';
            wbuflen--;
        }

        // Is it an empty line?
        if(wbuflen <= 0)
            continue;

        // Is it a new section header?
        if(wbuf[0] == '#')
        {
            QString wbuf_q(wbuf + 1);
            // Is it the level load header?
            if(wbuf[1] == '-')
                cur_section = -1;
            else if(wbuf_q.compare("end", Qt::CaseInsensitive) == 0)
                continue; // "END" keyword, just do nothing
            else // Else, parse the section number
            {
                try
                {
                    cur_section = std::stoi(wbuf + 1);
                }
                catch (const std::exception &e)
                {
                    qWarning() << "Bad section format ({0})" << e.what();
                    // Keep section number unchanged
                }
            }
        }
        else   // Else, parse as a command
        {
            // Is there a variable reference marker?
            if(wbuf[0] == '$')
            {
                int i = 1;
                while(wbuf[i] != ',' && wbuf[i] != '\x0D' && wbuf[i] != '\x0A' && i < 126)
                    ++i;

                wbuf[i] = '\x00'; // Turn the comma into a null terminator
                std::memcpy(wrefbuf, &wbuf[1], sizeof(wrefbuf)); // Copy new string into wrefbuf
                wrefbuf[sizeof(wrefbuf) - 1] = '\0';
                std::memcpy(wmidbuf, wbuf, sizeof(wmidbuf));
                std::strcpy(wbuf, &wmidbuf[i + 1]); // The rest of the line minus the ref is the new wbuf
            }

            // Decimal pass
            int hits = std::sscanf(wbuf, PARSE_FMT_STR, combuf, &target, &param1, &param2, &param3, &length, wstrbuf);

            // Integer pass
            int bhits = std::sscanf(wbuf, PARSE_FMT_STR_2, combuf, &btarget, &bparam1, &bparam2, &bparam3, &blength, wstrbuf);

            // Check for formatting failure
            if(hits < 3 && bhits < 3)
            {
                qWarning() << wbuf << ": Bad line format" << lineNum << "section" << cur_section;
                continue;
            }

            // Register new autocode

            std::string ac_str = std::string(wstrbuf); // Get the string out of strbuf
            ac_str.erase(ac_str.find_last_not_of(" \n\r\t") + 1);

            QString command(combuf);

            if(command == "ShowText" || command == "ShowVar")
            {
                TranslationData_ScriptLine sline;
                sline.line = lineNum;
                sline.source = QString::fromStdString(ac_str);
                sline.translation.text = sline.source;
                tr.lines.insert(lineNum, sline);
                hasStrings |= true;
                qDebug() << "Command line" << command << lineNum << QString::fromStdString(ac_str);
            }
        }
    }

#undef PARSE_FMT_STR
#undef PARSE_FMT_STR_2

    if(hasStrings)
        origin.scripts.insert(shortPath, tr);
}

void TextDataProcessor::recountStats(TranslateProject &proj, TranslationData &tr, bool isOrigin)
{
    TranslationData &origin = proj["metadata"];

    tr.t_strings = isOrigin ? 0 : origin.t_strings;
    tr.t_translated = 0;

    for(auto li = tr.levels.begin(); li != tr.levels.end(); ++li)
    {
        auto &l = li.value();
        l.t_strings = isOrigin ? 0 : origin.levels[li.key()].t_strings;
        l.t_translated = 0;

        if(isOrigin)
        {
            if(!l.title.text.isEmpty())
            {
                ++l.t_strings;
                ++tr.t_strings;
            }
        }
        else
        {
            if(!l.title.unfinished && !l.title.text.isEmpty())
            {
                ++l.t_translated;
                ++tr.t_translated;
            }
        }

        for(auto &e : l.events)
        {
            if(isOrigin)
            {
                if(!e.message.text.isEmpty())
                {
                    ++l.t_strings;
                    ++tr.t_strings;
                }
            }
            else
            {
                if(!e.message.unfinished && !e.message.text.isEmpty())
                {
                    ++l.t_translated;
                    ++tr.t_translated;
                }
            }
        }

        for(auto &e : l.npc)
        {
            if(isOrigin)
            {
                if(!e.talk.text.isEmpty())
                {
                    ++l.t_strings;
                    ++tr.t_strings;
                }
            }
            else
            {
                if(!e.talk.unfinished && !e.talk.text.isEmpty())
                {
                    ++l.t_translated;
                    ++tr.t_translated;
                }
            }
        }
    }

    for(auto li = tr.worlds.begin(); li != tr.worlds.end(); ++li)
    {
        auto &l = li.value();
        l.t_strings = isOrigin ? 0 : origin.worlds[li.key()].t_strings;
        l.t_translated = 0;

        if(isOrigin)
        {
            if(!l.title.text.isEmpty())
            {
                ++l.t_strings;
                ++tr.t_strings;
            }

            if(!l.credits.text.isEmpty())
            {
                ++l.t_strings;
                ++tr.t_strings;
            }
        }
        else
        {
            if(!l.title.unfinished && !l.title.text.isEmpty())
            {
                ++l.t_translated;
                ++tr.t_translated;
            }

            if(!l.credits.unfinished && !l.credits.text.isEmpty())
            {
                ++l.t_translated;
                ++tr.t_translated;
            }
        }

        for(auto &e : l.level_titles)
        {
            if(isOrigin)
            {
                if(!e.title.text.isEmpty())
                {
                    ++l.t_strings;
                    ++tr.t_strings;
                }
            }
            else
            {
                if(!e.title.unfinished && !e.title.text.isEmpty())
                {
                    ++l.t_translated;
                    ++tr.t_translated;
                }
            }
        }
    }

    for(auto li = tr.scripts.begin(); li != tr.scripts.end(); ++li)
    {
        auto &l = li.value();
        l.t_strings = isOrigin ? 0 : origin.scripts[li.key()].t_strings;
        l.t_translated = 0;

        for(auto &e : l.lines)
        {
            if(isOrigin)
            {
                if(!e.translation.text.isEmpty())
                {
                    ++l.t_strings;
                    ++tr.t_strings;
                }
            }
            else
            {
                if(!e.translation.unfinished && !e.translation.text.isEmpty())
                {
                    ++l.t_translated;
                    ++tr.t_translated;
                }
            }
        }
    }

}

bool TextDataProcessor::saveJSONs(const QString &directory, const TranslateProject &proj)
{
    bool ret = true;
    const auto &origin = proj[TRANSLATE_METADATA];
    bool useTrId = origin.useTrId;

    for(auto it = proj.begin(); it != proj.end(); ++it)
    {
        QJsonObject o;

        bool isOrigin = (it.key() == TRANSLATE_METADATA);

        if(isOrigin)
            o["use-tr-id"] = it->useTrId;

        for(auto l = it->levels.begin(); l != it->levels.end(); ++l)
        {
            QJsonObject lo;
            auto &la = l.value();

            if(!la.title.text.isEmpty() || (!isOrigin && !origin.levels[l.key()].title.text.isEmpty()))
            {
                lo["title"] = la.title.text;
                if(!la.title.note.isEmpty())
                    lo["title-n"] = la.title.note;

                if(!isOrigin)
                {
                    if(la.title.unfinished)
                        lo["title-u"] = true;
                    lo["title-orig"] = origin.levels[l.key()].title.text;
                }
            }

            if(!la.npc.isEmpty())
            {
                QJsonArray npc_a;

                for(auto &n : la.npc)
                {
                    QJsonObject entry;
                    if(useTrId)
                    {
                        entry["tr-id"] = n.talk_tr_id;
                        if(isOrigin)
                            entry["i"] = n.npc_index;
                    }
                    else
                        entry["i"] = n.npc_index;

                    entry["talk"] = n.talk.text;
                    if(!n.talk.note.isEmpty())
                        entry["talk-n"] = n.talk.note;

                    if(!isOrigin)
                    {
                        entry["orig"] = origin.levels[l.key()].npc[n.npc_index].talk.text;

                        if(n.talk.unfinished)
                            entry["u"] = true;

                        if(n.talk.vanished)
                            entry["v"] = true;
                    }
                    else
                    {
                        entry["t"] = n.npc_id;
                        if(n.talk_trigger >= 0)
                            entry["trig"] = n.talk_trigger;
                    }

                    npc_a.append(entry);
                }

                lo["npc"] = npc_a;
            }

            if(!la.events.isEmpty())
            {
                QJsonArray event_a;

                for(auto &e : la.events)
                {
                    QJsonObject entry;

                    if(useTrId)
                    {
                        entry["tr-id"] = e.message_tr_id;
                        if(isOrigin)
                            entry["i"] = e.event_index;
                    }
                    else
                        entry["i"] = e.event_index;

                    entry["msg"] = e.message.text;
                    if(!e.message.note.isEmpty())
                        entry["msg-n"] = e.message.note;

                    if(!isOrigin)
                    {
                        Q_ASSERT(e.event_index >= 0);
                        entry["orig"] = origin.levels[l.key()].events[e.event_index].message.text;

                        if(e.message.unfinished)
                            entry["u"] = true;

                        if(e.message.vanished)
                            entry["v"] = true;
                    }
                    else
                    {
                        if(e.trigger_next >= 0)
                            entry["trig"] = e.trigger_next;

                        if(!e.triggered_by_npc.isEmpty())
                        {
                            QJsonArray by_npc;
                            for(auto &q : e.triggered_by_npc)
                                by_npc.append(q);
                            entry["by-npc"] = by_npc;
                        }

                        if(!e.triggered_by_event.isEmpty())
                        {
                            QJsonArray by_event;
                            for(auto &q : e.triggered_by_event)
                                by_event.append(q);
                            entry["by-event"] = by_event;
                        }
                    }

                    event_a.append(entry);
                }

                lo["events"] = event_a;
            }

#ifdef DEBUG_BUILD
            validate(la.npc);
            validate(la.events);
#endif

            if(!la.glossary.isEmpty())
            {
                QJsonObject g;
                for(auto gli = la.glossary.begin(); gli != la.glossary.end(); ++gli)
                    g[gli.key()] = gli.value();

                lo["glossary"] = g;
            }

            if(!la.dialogues.isEmpty())
            {
                QJsonArray cho;

                for(auto &e : la.dialogues)
                {
                    QJsonObject dio;
                    QJsonArray se;

                    dio["note"] = e.note;

                    for(auto &ie : e.messages)
                    {
                        switch(ie.type)
                        {
                        case TranslationData_DialogueNode::T_EVENT_MSG:
                            se.push_back(QString("event-%1-msg").arg(ie.item_index));
                            break;
                        case TranslationData_DialogueNode::T_NPC_TALK:
                            se.push_back(QString("npc-%1-talk").arg(ie.item_index));
                            break;
                        default:
                        case TranslationData_DialogueNode::T_END:
                            break;
                        }
                    }

                    dio["messages"] = se;
                    cho.append(dio);
                }

                lo["dialogues"] = cho;
            }

            o[l.key()] = lo;
        }

        for(auto w = it->worlds.begin(); w != it->worlds.end(); ++w)
        {
            QJsonObject wo;
            auto &wa = w.value();

            if(!wa.title.text.isEmpty() || (!isOrigin && !origin.worlds[w.key()].title.text.isEmpty()))
            {
                wo["title"] = wa.title.text;
                o[" episode_title"] = QJsonValue(wa.title.text);
                o[" episode_world"] = QJsonValue(w.key());
                if(!wa.title.note.isEmpty())
                    wo["title-n"] = wa.title.note;

                if(!isOrigin)
                {
                    if(wa.title.unfinished)
                        wo["title-u"] = true;
                    wo["title-orig"] = origin.worlds[w.key()].title.text;
                }
            }

            if(!wa.credits.text.isEmpty() || (!isOrigin && !origin.worlds[w.key()].credits.text.isEmpty()))
            {
                wo["credits"] = wa.credits.text;
                if(!wa.credits.text.isEmpty())
                    wo["credits-n"] = wa.credits.note;

                if(!isOrigin)
                {
                    if(wa.credits.unfinished)
                        wo["credits-u"] = true;
                    wo["credits-orig"] = origin.worlds[w.key()].credits.text;
                }
            }

            if(!wa.level_titles.isEmpty())
            {
                QJsonArray level_a;

                for(auto &l : wa.level_titles)
                {
                    QJsonObject entry;

                    if(useTrId)
                    {
                        entry["tr-id"] = l.title_tr_id;
                        if(isOrigin)
                            entry["i"] = l.level_index;
                    }
                    else
                        entry["i"] = l.level_index;

                    entry["tit"] = l.title.text;
                    if(!l.title.note.isEmpty())
                        entry["tit-n"] = l.title.note;

                    if(!isOrigin)
                    {
                        entry["orig"] = origin.worlds[w.key()].level_titles[l.level_index].title.text;

                        if(l.title.unfinished)
                            entry["u"] = true;

                        if(l.title.vanished)
                            entry["v"] = true;
                    }
                    else
                    {
                        entry["f"] = l.filename;
                    }

                    level_a.append(entry);
                }

                wo["levels"] = level_a;
            }

            if(!wa.glossary.isEmpty())
            {
                QJsonObject g;
                for(auto gli = wa.glossary.begin(); gli != wa.glossary.end(); ++gli)
                    g[gli.key()] = gli.value();

                wo["glossary"] = g;
            }

            o[w.key()] = wo;
        }

        for(auto s = it->scripts.begin(); s != it->scripts.end(); ++s)
        {
            QJsonObject so;
            auto &sa = s.value();

            so["title"] = sa.title;

            if(!sa.lines.isEmpty())
            {
                QJsonArray lio;

                for(auto &i : sa.lines)
                {
                    QJsonObject entry;

                    if(useTrId)
                    {
                        entry["tr-id"] = i.line_tr_id;
                        if(isOrigin)
                            entry["i"] = i.line;
                    }
                    else
                    {
                        entry["i"] = i.line;
                        entry["src"] = i.source;
                    }

                    if(!isOrigin)
                    {
                        entry["tr"] = i.translation.text;
                        if(!i.translation.note.isEmpty())
                            entry["tr-n"] = i.translation.note;
                        if(i.translation.unfinished)
                            entry["u"] = true;
                        if(i.translation.vanished)
                            entry["v"] = true;
                    }
                    else if(!i.translation.note.isEmpty())
                        entry["src-n"] = i.translation.note;

                    lio.append(entry);
                }

                so["lines"] = lio;
            }

            o[s.key()] = so;
        }

        if(!it->directories.isEmpty())
        {
            for(auto &s : it->directories)
                o[s] = QJsonObject();
        }

        QDir i18n(directory);
        if(!i18n.exists("i18n"))
            i18n.mkdir("i18n");

        QFile f(QString("%1/i18n/translation_%2.json").arg(directory).arg(it.key()));
        if(f.open(QIODevice::WriteOnly))
        {
            QJsonDocument d;
            d.setObject(o);
            f.write(d.toJson(QJsonDocument::Indented));
            f.close();
            ret &= true;
        }
        else
            ret &= false;
    }

    // Remove legacy "origin" file as new "metadata" got been written instead of this
    QString legacy_meta = directory + "/i18n/translation_origin.json";
    if(QFile::exists(legacy_meta))
        QFile::remove(legacy_meta);

    return ret;
}

void TextDataProcessor::updateTranslation(TranslateProject &proj, const QString &trName)
{
    auto &origin = proj["metadata"];
    auto &tr = proj[trName];
    Q_ASSERT(trName != "origin" && trName != "metadata"); // Never call this function over the Meta File


    // Add missing entries at translation

    for(auto it = origin.levels.begin(); it != origin.levels.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = tr.levels[it.key()];

        if(ld.title.text.isEmpty())
            ld.title.unfinished = true;

        for(auto nit = ls.npc.begin(); nit != ls.npc.end(); ++nit)
        {
            if(!ld.npc.contains(nit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ld.npc[nit.key()];
                n.talk.unfinished = true;
                n.npc_id = nit->npc_id;
                n.npc_index = nit->npc_index;
                n.talk_trigger = nit->talk_trigger;
            }
        }

        for(auto eit = ls.events.begin(); eit != ls.events.end(); ++eit)
        {
            if(!ld.events.contains(eit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ld.events[eit.key()];
                n.message.unfinished = true;
                n.event_index = eit->event_index;
                n.trigger_next = eit->trigger_next;
            }
        }

#ifdef DEBUG_BUILD
        validate(ls.npc);
        validate(ls.events);
#endif
    }

    for(auto it = origin.worlds.begin(); it != origin.worlds.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = tr.worlds[it.key()];

        if(ld.title.text.isEmpty())
            ld.title.unfinished = true;

        if(ld.credits.text.isEmpty())
            ld.credits.unfinished = true;

        for(auto eit = ls.level_titles.begin(); eit != ls.level_titles.end(); ++eit)
        {
            if(!ld.level_titles.contains(eit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ld.level_titles[eit.key()];
                n.title.unfinished = true;
                n.level_index = eit->level_index;
            }
        }
    }

    for(auto it = origin.scripts.begin(); it != origin.scripts.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = tr.scripts[it.key()];

        for(auto lit = ls.lines.begin(); lit != ls.lines.end(); ++lit)
        {
            if(!ld.lines.contains(lit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ld.lines[lit.key()];
                n.translation.unfinished = true;
                n.source = lit->source;
                n.line = lit->line;
            }
        }
    }


    // Mark entries as vanished when origin is no longer exists

    for(auto it = tr.levels.begin(); it != tr.levels.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = origin.levels[it.key()];

        for(auto nit = ls.npc.begin(); nit != ls.npc.end(); ++nit)
        {
            if(!ld.npc.contains(nit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ls.npc[nit.key()];
                n.talk.vanished = true;
            }
        }

        for(auto eit = ls.events.begin(); eit != ls.events.end(); ++eit)
        {
            if(!ld.events.contains(eit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = eit.value();
                n.message.vanished = true;
            }
        }

#ifdef DEBUG_BUILD
        validate(ld.npc);
        validate(ld.events);
#endif
    }

    for(auto it = tr.worlds.begin(); it != tr.worlds.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = origin.worlds[it.key()];

        for(auto eit = ls.level_titles.begin(); eit != ls.level_titles.end(); ++eit)
        {
            if(!ld.level_titles.contains(eit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ls.level_titles[eit.key()];
                n.title.vanished = true;
            }
        }
    }

    for(auto it = tr.scripts.begin(); it != tr.scripts.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = origin.scripts[it.key()];

        for(auto lit = ls.lines.begin(); lit != ls.lines.end(); ++lit)
        {
            if(!ld.lines.contains(lit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ls.lines[lit.key()];
                n.translation.vanished = true;
            }
        }
    }


    // Delete empty entries marked as "vanished"

    for(auto it = tr.levels.begin(); it != tr.levels.end(); ++it)
    {
        auto &ls = it.value();

        for(auto nit = ls.npc.begin(); nit != ls.npc.end(); )
        {
            const auto &n = nit.value();
            if(n.talk.vanished && n.talk.text.isEmpty())
                nit = ls.npc.erase(nit);
            else
                ++nit;
        }

        for(auto eit = ls.events.begin(); eit != ls.events.end(); )
        {
            const auto &n = eit.value();
            if(n.message.vanished && n.message.text.isEmpty())
                eit = ls.events.erase(eit);
            else
                ++eit;
        }
    }

    for(auto it = tr.worlds.begin(); it != tr.worlds.end(); ++it)
    {
        auto &ls = it.value();
        for(auto eit = ls.level_titles.begin(); eit != ls.level_titles.end(); )
        {
            const auto &n = eit.value();
            if(n.title.vanished && n.title.text.isEmpty())
                eit = ls.level_titles.erase(eit);
            else
                ++eit;
        }
    }

    for(auto it = tr.scripts.begin(); it != tr.scripts.end(); ++it)
    {
        auto &ls = it.value();

        for(auto lit = ls.lines.begin(); lit != ls.lines.end(); )
        {
            const auto &n = lit.value();
            if(n.translation.vanished && n.translation.text.isEmpty())
                lit = ls.lines.erase(lit);
            else
                ++lit;
        }
    }


    // If content doesn't matches, update and clear it

    for(auto it = tr.scripts.begin(); it != tr.scripts.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = origin.scripts[it.key()];

        for(auto lit = ls.lines.begin(); lit != ls.lines.end(); ++lit)
        {
            auto &n = ls.lines[lit.key()];
            if(n.translation.vanished)
                continue; // Skip vanished translations

            auto &n_orig = ld.lines[lit.key()];

            if(n.source != n_orig.source)
            {
                n.source = n_orig.source;
                n.translation.text.clear();
                n.translation.unfinished = true;
            }
        }
    }


    recountStats(proj, tr, false);
}

void TextDataProcessor::loadTranslation(TranslateProject &proj, const QString &trName, const QString &filePath)
{
    QJsonParseError e;
    QJsonDocument d;

    QFile f(filePath);
    if(!f.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open file" << filePath;
        return;
    }

    auto inData = f.readAll();
    f.close();

    d = QJsonDocument::fromJson(inData, &e);
    if(d.isNull())
    {
        qWarning() << "Failed to parse the file" << filePath
                   << "error:" << e.errorString()
                   << "offset:" << e.offset;
        return;
    }

    QJsonObject r = d.object();
    bool isOrigin = (trName == "metadata");

    auto &tr = proj[trName];
    auto &tr_orig = proj["metadata"];

    if(isOrigin)
        tr.useTrId = r.value("use-tr-id").toBool(false);

    for(auto &k : r.keys())
    {
        auto val = r.value(k);

        if(!val.isObject())
            continue; // Skip non-objects

        QJsonObject entry = val.toObject();

        if(entry.isEmpty()) // If blank object, it's a "directory"
        {
            tr.directories.insert(k);
        }
        else if(k.endsWith(".lvl", Qt::CaseInsensitive) || k.endsWith(".lvlx", Qt::CaseInsensitive))
        {
            auto &lvl = tr.levels[k];
            lvl.title.text = entry.value("title").toString();
            lvl.title.note = entry.value("title-n").toString();
            lvl.title.unfinished = entry.value("title-u").toBool();

            if(entry.contains("events"))
            {
                QJsonArray arr = entry["events"].toArray();
                for(const auto &it : arr)
                {
                    auto ito = it.toObject();
                    TranslationData_EVENT et;
                    et.event_index = ito["i"].toInt(-1);
                    et.message_tr_id = ito["tr-id"].toString();

                    if(et.event_index < 0 && et.message_tr_id.isEmpty())
                        continue; // Skip invalid entries

                    if(tr.useTrId)
                    {
                        if(isOrigin)
                        {
                            Q_ASSERT(et.event_index >= 0);
                            Q_ASSERT(!et.message_tr_id.isEmpty());
                            tr.trId_map_lvl_events.insert(et.message_tr_id, et.event_index);
                        }
                        else
                        {
                            Q_ASSERT(tr_orig.trId_map_lvl_events.contains(et.message_tr_id));
                            et.event_index = tr_orig.trId_map_lvl_events[et.message_tr_id];
                        }
                    }

                    et.message.text = ito["msg"].toString();
                    et.message.note = ito["msg-n"].toString();
                    et.message.unfinished = ito["u"].toBool();
                    et.message.vanished = ito["v"].toBool();
                    et.trigger_next = ito["trig"].toInt(-1);

                    if(ito.contains("by-npc"))
                    {
                        auto byNpc = ito["by-npc"].toArray();
                        for(const auto &bn : byNpc)
                            et.triggered_by_npc.push_back(bn.toInt(-1));
                    }

                    if(ito.contains("by-event"))
                    {
                        auto byEvent = ito["by-event"].toArray();
                        for(const auto &be : byEvent)
                            et.triggered_by_event.push_back(be.toInt(-1));
                    }

                    lvl.events.insert(et.event_index, et);
                }
            }

            if(entry.contains("npc"))
            {
                QJsonArray arr = entry["npc"].toArray();
                for(const auto &it : arr)
                {
                    auto ito = it.toObject();
                    TranslationData_NPC et;
                    et.npc_index = ito["i"].toInt(-1);
                    et.talk_tr_id = ito["tr-id"].toString();

                    if(et.npc_index < 0 && et.talk_tr_id.isEmpty())
                        continue; // Skip invalid entries

                    if(tr.useTrId)
                    {
                        if(isOrigin)
                        {
                            Q_ASSERT(et.npc_index >= 0);
                            Q_ASSERT(!et.talk_tr_id.isEmpty());
                            tr.trId_map_lvl_npc.insert(et.talk_tr_id, et.npc_index);
                        }
                        else
                        {
                            Q_ASSERT(tr_orig.trId_map_lvl_npc.contains(et.talk_tr_id));
                            et.npc_index = tr_orig.trId_map_lvl_npc[et.talk_tr_id];
                        }
                    }

                    et.npc_id = ito["t"].toInt(-1);
                    et.talk.text = ito["talk"].toString();
                    et.talk.note = ito["talk-n"].toString();
                    et.talk.unfinished = ito["u"].toBool();
                    et.talk.vanished = ito["v"].toBool();
                    et.talk_trigger = ito["trig"].toInt(-1);
                    lvl.npc.insert(et.npc_index, et);
                }
            }

            // DEPRECATED fallback
            if(entry.contains("chains")) // Fallback
            {
                QJsonArray arr = entry["chains"].toArray();
                for(const auto &pit : arr)
                {
                    TranslationData_Dialogue d;
                    const auto &sarr = pit.toArray();

                    for(const auto &it : sarr)
                    {
                        auto s = it.toString();
                        TranslationData_DialogueNode n;
                        QStringList l_npc = s_getRegExMatches("npc-(\\d*)\\-talk", s);
                        QStringList l_event = s_getRegExMatches("event-(\\d*)\\-msg", s);

                        if(l_npc.size() >= 2)
                        {
                            n.item_index = l_npc[1].toInt();
                            n.type = TranslationData_DialogueNode::T_NPC_TALK;
                            d.messages.push_back(n);
                        }
                        else if(l_event.size() >= 2)
                        {
                            n.item_index = l_event[1].toInt();
                            n.type = TranslationData_DialogueNode::T_EVENT_MSG;
                            d.messages.push_back(n);
                        }
                    }

                    TranslationData_DialogueNode n;
                    n.type = TranslationData_DialogueNode::T_END;
                    d.messages.push_back(n);
                    lvl.dialogues.push_back(d);
                }
            }

            if(entry.contains("dialogues"))
            {
                QJsonArray arr = entry["dialogues"].toArray();
                for(const auto &pit : arr)
                {
                    TranslationData_Dialogue d;
                    auto dio = pit.toObject();
                    d.note = dio["note"].toString();
                    const auto &sarr = dio["messages"].toArray();

                    for(const auto &it : sarr)
                    {
                        auto s = it.toString();
                        TranslationData_DialogueNode n;
                        QStringList l_npc = s_getRegExMatches("npc-(\\d*)\\-talk", s);
                        QStringList l_event = s_getRegExMatches("event-(\\d*)\\-msg", s);

                        if(l_npc.size() >= 2)
                        {
                            n.item_index = l_npc[1].toInt();
                            n.type = TranslationData_DialogueNode::T_NPC_TALK;
                            d.messages.push_back(n);
                        }
                        else if(l_event.size() >= 2)
                        {
                            n.item_index = l_event[1].toInt();
                            n.type = TranslationData_DialogueNode::T_EVENT_MSG;
                            d.messages.push_back(n);
                        }
                    }

                    TranslationData_DialogueNode n;
                    n.type = TranslationData_DialogueNode::T_END;
                    d.messages.push_back(n);
                    lvl.dialogues.push_back(d);
                }
            }

            if(entry.contains("glossary"))
            {
                QJsonObject obj = entry["glossary"].toObject();
                for(auto it : obj.keys())
                    lvl.glossary[it] = obj[it].toString();
            }
        }
        else if(k.endsWith(".wld", Qt::CaseInsensitive) || k.endsWith(".wldx", Qt::CaseInsensitive))
        {
            auto &wld = tr.worlds[k];
            wld.title.text = entry.value("title").toString();
            wld.title.note = entry.value("title-n").toString();
            wld.title.unfinished = entry.value("title-u").toBool();
            wld.credits.text = entry.value("credits").toString();
            wld.credits.note = entry.value("credits-n").toString();
            wld.credits.unfinished = entry.value("credits-u").toBool();

            if(entry.contains("levels"))
            {
                QJsonArray arr = entry["levels"].toArray();
                for(const auto &it : arr)
                {
                    auto ito = it.toObject();
                    TranslationData_LEVEL et;
                    et.level_index = ito["i"].toInt(-1);
                    et.title_tr_id = ito["tr-id"].toString();

                    if(et.level_index < 0 && et.title_tr_id.isEmpty())
                        continue; // Skip invalid entries

                    if(tr.useTrId)
                    {
                        if(isOrigin)
                        {
                            Q_ASSERT(et.level_index >= 0);
                            Q_ASSERT(!et.title_tr_id.isEmpty());
                            tr.trId_map_wld_levels.insert(et.title_tr_id, et.level_index);
                        }
                        else
                        {
                            Q_ASSERT(tr_orig.trId_map_wld_levels.contains(et.title_tr_id));
                            et.level_index = tr_orig.trId_map_wld_levels[et.title_tr_id];
                        }
                    }

                    et.title.text = ito["tit"].toString();
                    et.title.note = ito["tit-n"].toString();
                    et.title.unfinished = ito["u"].toBool(false);
                    et.title.vanished = ito["v"].toBool(false);
                    et.filename = ito["f"].toString("");
                    wld.level_titles.insert(et.level_index, et);
                }
            }

            if(entry.contains("glossary"))
            {
                QJsonObject obj = entry["glossary"].toObject();
                for(auto it : obj.keys())
                    wld.glossary[it] = obj[it].toString();
            }
        }
        else if(k.endsWith("lunadll.txt", Qt::CaseInsensitive) || k.endsWith("lunaworld.txt", Qt::CaseInsensitive))
        {
            auto &script = tr.scripts[k];
            script.title = entry.value("title").toString();

            if(entry.contains("lines"))
            {
                QJsonArray arr = entry["lines"].toArray();
                for(const auto &it : arr)
                {
                    TranslationData_ScriptLine l;
                    auto ito = it.toObject();
                    l.line = ito["i"].toInt(-1);
                    l.line_tr_id = ito["tr-id"].toString();

                    if(l.line < 0 && l.line_tr_id.isEmpty())
                        continue; // Skip invalid entries

                    if(tr.useTrId)
                    {
                        if(isOrigin)
                        {
                            Q_ASSERT(l.line >= 0);
                            Q_ASSERT(!l.line_tr_id.isEmpty());
                            tr.trId_map_script_lines.insert(l.line_tr_id, l.line);
                        }
                        else
                        {
                            Q_ASSERT(tr_orig.trId_map_script_lines.contains(l.line_tr_id));
                            l.line = tr_orig.trId_map_script_lines[l.line_tr_id];
                        }
                    }

                    l.source = ito["src"].toString();

                    if(isOrigin)
                    {
                        l.translation.text = l.source;
                        l.translation.note = ito["src-n"].toString();
                    }
                    else
                    {
                        l.translation.text = ito["tr"].toString();
                        l.translation.note = ito["tr-n"].toString();
                        l.translation.unfinished = ito["u"].toBool(false);
                        l.translation.vanished = ito["v"].toBool(false);
                    }

                    script.lines[l.line] = l;
                }
            }
        }
    }

    recountStats(proj, tr, isOrigin);
}
