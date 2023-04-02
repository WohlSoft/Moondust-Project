#include <cstdlib>
#include <cstring>

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtDebug>
#include <QFileInfo>
#include <PGE_File_Formats/file_formats.h>

#include "text_data_processor.h"

TextDataProcessor::TextDataProcessor()
{}

static int find_event_index(PGELIST<LevelSMBX64Event > &events, const QString &trigger)
{
    for(int j = 0; j < events.size(); ++j)
    {
        if(events[j].name == trigger)
            return j;
    }

    return -1;
}

bool TextDataProcessor::loadProject(const QString &directory, TranslateProject &proj)
{
    // Close current project
    proj.clear();

    QDir d(directory);
    QDirIterator dit(directory,
                     {
                         "*.lvl", "*.lvlx",
                         "*.wld", "*.wldx",
                         "lunadll.txt", "lunaworld.txt"
                     },
                     QDir::Files,
                     QDirIterator::Subdirectories);

    // Fill the original language
    auto &origin = proj["_origin"];

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

    return true;
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

    tr.title = l.LevelName;
    hasStrings |= !tr.title.isEmpty();

    if(!tr.title.isEmpty())
        qDebug() << "Level Title:" << tr.title;

    for(int i = 0; i < l.events.size(); ++i)
    {
        auto &ed = l.events[i];
        TranslationData_EVENT e;

        if(ed.msg.isEmpty())
            continue; // Don't include events with no messages

        hasStrings |= true;

        e.event_index = i;
        e.message = ed.msg;
        if(!ed.trigger.isEmpty()) // Find index of the trigger
            e.trigger_next = find_event_index(l.events, ed.trigger);
        tr.events.insert(i, e);
        qDebug() << "Event" << i << "message: [" << e.message << "]";
    }

    for(int i = 0; i < l.npc.size(); ++i)
    {
        auto &nd = l.npc[i];
        TranslationData_NPC n;

        if(nd.msg.isEmpty())
            continue; // Don't include NPCs with no messages

        hasStrings |= true;

        n.npc_index = i;
        n.npc_id = (int)nd.id;
        n.talk = nd.msg;
        if(!nd.event_talk.isEmpty())
            n.talk_trigger = find_event_index(l.events, nd.event_talk);

        tr.npc.insert(i, n);
        qDebug() << "NPC-" << n.npc_id << "idx" << i << "talk: [" << n.talk << "]";
    }

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

    // Build dialogues
    for(auto &n : tr.npc)
    {
        TranslationData_Dialogue d;
        TranslationData_DialogueNode dn;
        dn.type = TranslationData_DialogueNode::T_NPC_TALK;
        dn.item_index = n.npc_index;

        if(!n.talk.isEmpty())
            d.push_back(dn);

        if(n.talk_trigger >= 0)
        {
            int next_trigger = n.talk_trigger;
            do
            {
                auto &e = tr.events[next_trigger];
                TranslationData_DialogueNode de;
                de.type = TranslationData_DialogueNode::T_EVENT_MSG;
                de.item_index = e.event_index;
                if(!e.message.isEmpty())
                    d.push_back(de);
                next_trigger = e.trigger_next;
            } while(next_trigger >= 0);
        }

        if(!d.isEmpty())
        {
            dn.type = TranslationData_DialogueNode::T_END;
            dn.item_index = -1;
            d.push_back(dn);
            tr.chains.append(d);
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

        if(!e.message.isEmpty())
            d.push_back(dn);

        if(e.trigger_next >= 0)
        {
            int next_trigger = e.trigger_next;
            do
            {
                auto &se = tr.events[next_trigger];
                TranslationData_DialogueNode de;
                de.type = TranslationData_DialogueNode::T_EVENT_MSG;
                de.item_index = se.event_index;
                if(!se.message.isEmpty())
                    d.push_back(de);
                next_trigger = se.trigger_next;
            } while(next_trigger >= 0);
        }

        if(!d.isEmpty())
        {
            dn.type = TranslationData_DialogueNode::T_END;
            dn.item_index = -1;
            d.push_back(dn);
            tr.chains.append(d);
        }
    }

    if(!tr.chains.isEmpty())
    {
        qDebug() << "\n=========== Dialogues: ===========\n";
        for(auto &c : tr.chains)
        {
            qDebug() << "-------------------------------------";
            for(auto &m : c)
            {
                switch(m.type)
                {
                case TranslationData_DialogueNode::T_EVENT_MSG:
                    qDebug() << "-" << "EVENT" << tr.events[m.item_index].message;
                    break;
                case TranslationData_DialogueNode::T_NPC_TALK:
                    qDebug() << "-" << "NPC" << tr.npc[m.item_index].talk;
                    break;
                case TranslationData_DialogueNode::T_END:
                    qDebug() << "-------------------------------------\n";
                    break;
                default:
                    break;
                }
            }
        }
    }

    if(hasStrings)
        origin.levels.insert(shortPath, tr);
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

    tr.title = w.EpisodeTitle;
    hasStrings |= !tr.title.isEmpty();

    if(!tr.title.isEmpty())
        qDebug() << "World Title:" << tr.title;

    tr.credits = w.authors;
    hasStrings |= !tr.credits.isEmpty();

    if(!tr.credits.isEmpty())
        qDebug() << "Credits:" << tr.credits;

    for(int i = 0; i < w.levels.size(); ++i)
    {
        auto &l = w.levels[i];
        TranslationData_LEVEL ld;

        if(l.title.isEmpty())
            continue; // Skip blank level entries

        ld.level_index = i;
        ld.title = l.title;
        ld.filename = l.lvlfile;

        tr.level_titles.insert(i, ld);
        hasStrings |= true;
        qDebug() << "Level" << i << ld.title;
    }

    if(hasStrings)
        origin.worlds.insert(shortPath, tr);
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
