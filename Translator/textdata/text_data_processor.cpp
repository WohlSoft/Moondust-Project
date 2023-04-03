#include <cstdlib>
#include <cstring>

#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QFileInfo>
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

bool TextDataProcessor::loadProject(const QString &directory, TranslateProject &proj)
{
    // Close current project
    proj.clear();

    QDirIterator dit(directory,
                     {"translation_*.json"},
                     QDir::Files,
                     QDirIterator::NoIteratorFlags);
    bool hasTranslations = false;

    while(dit.hasNext())
    {
        auto f = dit.next();
        QFileInfo fi(f);
        QRegExp fe("translation_(\\w*)\\.json", Qt::CaseInsensitive, QRegExp::RegExp2);
        if(fe.exactMatch(fi.fileName()))
        {
            loadTranslation(proj, fe.cap(1), f);
            hasTranslations = true;
        }
    }

    if(!hasTranslations)
        return scanEpisode(directory, proj);

    return true;
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
                     QDirIterator::Subdirectories);

    // Fill the original language
    auto &origin = proj["origin"];

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

    updateTranslation(proj, "en");
    saveJSONs(directory, proj);

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
            continue;

        hasStrings |= true;

        e.event_index = i;
        e.message = ed.msg;
        if(!ed.trigger.isEmpty()) // Find index of the trigger
            e.trigger_next = find_event_index(l.events, ed.trigger);
        tr.events.insert(i, e);
        qDebug() << "Event" << i << "message: [" << e.message << "]" << "Trigger" << e.trigger_next;
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
        qDebug() << "NPC-" << n.npc_id << "idx" << i << "talk: [" << n.talk << "]" << "Trigger" << n.talk_trigger;
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

        if(n.talk_trigger >= 0 && tr.events.contains(n.talk_trigger))
        {
            QSet<int> triggered;
            int next_trigger = n.talk_trigger;
            do
            {
                auto &e = tr.events[next_trigger];
                TranslationData_DialogueNode de;
                de.type = TranslationData_DialogueNode::T_EVENT_MSG;
                de.item_index = e.event_index;
                if(!e.message.isEmpty())
                    d.push_back(de);
                triggered.insert(next_trigger);
                next_trigger = e.trigger_next;
            } while(next_trigger >= 0 && !triggered.contains(next_trigger));
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

        if(e.trigger_next >= 0 && tr.events.contains(e.trigger_next))
        {
            QSet<int> triggered;
            int next_trigger = e.trigger_next;
            do
            {
                auto &se = tr.events[next_trigger];
                TranslationData_DialogueNode de;
                de.type = TranslationData_DialogueNode::T_EVENT_MSG;
                de.item_index = se.event_index;
                if(!se.message.isEmpty())
                    d.push_back(de);
                triggered.insert(next_trigger);
                next_trigger = se.trigger_next;
            } while(next_trigger >= 0 && !triggered.contains(next_trigger));
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

void TextDataProcessor::saveJSONs(const QString &directory, TranslateProject &proj)
{
    auto &origin = proj["origin"];

    for(auto it = proj.begin(); it != proj.end(); ++it)
    {
        QJsonObject o;

        bool isOrigin = (it.key() == "origin");

        for(auto l = it->levels.begin(); l != it->levels.end(); ++l)
        {
            QJsonObject lo;
            auto &la = l.value();

            if(!la.title.isEmpty() || (!isOrigin && !origin.levels[l.key()].title.isEmpty()))
            {
                lo["title"] = la.title;
                if(!isOrigin)
                    lo["title-orig"] = origin.levels[l.key()].title;
            }

            if(!la.npc.isEmpty())
            {
                QJsonArray npc_a;

                for(auto &n : la.npc)
                {
                    QJsonObject entry;
                    entry["i"] = n.npc_index;
                    entry["talk"] = n.talk;

                    if(!isOrigin)
                    {
                        entry["orig"] = origin.levels[l.key()].npc[n.npc_index].talk;

                        if(n.unfinished)
                            entry["u"] = true;

                        if(n.vanished)
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
                    entry["i"] = e.event_index;
                    entry["msg"] = e.message;

                    if(!isOrigin)
                    {
                        entry["orig"] = origin.levels[l.key()].events[e.event_index].message;

                        if(e.unfinished)
                            entry["u"] = true;

                        if(e.vanished)
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

            if(!la.glossary.isEmpty())
            {
                QJsonObject g;
                for(auto gli = la.glossary.begin(); gli != la.glossary.end(); ++gli)
                    g[gli.key()] = gli.value();

                lo["glossary"] = g;
            }

            if(!la.chains.isEmpty())
            {
                QJsonArray cho;

                for(auto &e : la.chains)
                {
                    QJsonArray se;

                    for(auto &ie : e)
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

                    cho.append(se);
                }

                lo["chains"] = cho;
            }

            o[l.key()] = lo;
        }

        for(auto w = it->worlds.begin(); w != it->worlds.end(); ++w)
        {
            QJsonObject wo;
            auto &wa = w.value();

            if(!wa.title.isEmpty() || (!isOrigin && !origin.worlds[w.key()].title.isEmpty()))
            {
                wo["title"] = wa.title;
                if(!isOrigin)
                    wo["title-orig"] = origin.worlds[w.key()].title;
            }

            if(!wa.credits.isEmpty() || (!isOrigin && !origin.worlds[w.key()].credits.isEmpty()))
            {
                wo["credits"] = wa.credits;
                if(!isOrigin)
                    wo["credits-orig"] = origin.worlds[w.key()].credits;
            }

            if(!wa.level_titles.isEmpty())
            {
                QJsonArray level_a;

                for(auto &l : wa.level_titles)
                {
                    QJsonObject entry;
                    entry["i"] = l.level_index;
                    entry["tit"] = l.title;

                    if(!isOrigin)
                    {
                        entry["orig"] = origin.worlds[w.key()].level_titles[l.level_index].title;

                        if(l.unfinished)
                            entry["u"] = true;

                        if(l.vanished)
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
                    entry["i"] = i.line;
                    entry["src"] = i.source;

                    if(!isOrigin)
                    {
                        entry["tr"] = i.translation;

                        if(i.unfinished)
                            entry["u"] = true;

                        if(i.vanished)
                            entry["v"] = true;
                    }

                    lio.append(entry);
                }

                so["lines"] = lio;
            }

            o[s.key()] = so;
        }

        QFile f(QString("%1/translation_%2.json").arg(directory).arg(it.key()));
        if(f.open(QIODevice::WriteOnly))
        {
            QJsonDocument d;
            d.setObject(o);
            f.write(d.toJson(QJsonDocument::Indented));
            f.close();
        }
    }
}

void TextDataProcessor::updateTranslation(TranslateProject &proj, const QString &trName)
{
    auto &origin = proj["origin"];
    auto &tr = proj[trName];


    // Add missing entries at translation

    for(auto it = origin.levels.begin(); it != origin.levels.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = tr.levels[it.key()];

        for(auto nit = ls.npc.begin(); nit != ls.npc.end(); ++nit)
        {
            if(!ld.npc.contains(nit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ld.npc[nit.key()];
                n.unfinished = true;
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
                n.unfinished = true;
                n.event_index = eit->event_index;
                n.trigger_next = eit->trigger_next;
            }
        }
    }

    for(auto it = origin.worlds.begin(); it != origin.worlds.end(); ++it)
    {
        auto &ls = it.value();
        auto &ld = tr.worlds[it.key()];

        for(auto eit = ls.level_titles.begin(); eit != ls.level_titles.end(); ++eit)
        {
            if(!ld.level_titles.contains(eit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ld.level_titles[eit.key()];
                n.unfinished = true;
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
                n.unfinished = true;
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
                n.vanished = true;
            }
        }

        for(auto eit = ls.events.begin(); eit != ls.events.end(); ++eit)
        {
            if(!ld.events.contains(eit.key())) // Create empty entry with an "unfinished" entry
            {
                auto &n = ls.events[eit.key()];
                n.vanished = true;
            }
        }
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
                n.vanished = true;
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
                n.vanished = true;
            }
        }
    }
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

    auto &tr = proj[trName];

    for(auto &k : r.keys())
    {
        QJsonObject entry = r[k].toObject();
        if(k.endsWith(".lvl", Qt::CaseInsensitive) || k.endsWith(".lvlx", Qt::CaseInsensitive))
        {
            auto &lvl = tr.levels[k];
            lvl.title = entry.value("title").toString();

            if(entry.contains("events"))
            {
                QJsonArray arr = entry["events"].toArray();
                for(const auto &it : arr)
                {
                    auto ito = it.toObject();
                    int i;
                    TranslationData_EVENT et;
                    i = ito["i"].toInt(-1);
                    et.event_index = i;
                    et.message = ito["msg"].toString();
                    et.unfinished = ito["u"].toBool();
                    et.vanished = ito["v"].toBool();

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
                            et.triggered_by_npc.push_back(be.toInt(-1));
                    }

                    lvl.events.insert(i, et);
                }
            }

            if(entry.contains("npc"))
            {
                QJsonArray arr = entry["npc"].toArray();
                for(const auto &it : arr)
                {
                    auto ito = it.toObject();
                    int i;
                    TranslationData_NPC et;
                    i = ito["i"].toInt(-1);
                    et.npc_index = i;
                    et.npc_id = ito["t"].toInt();
                    et.talk= ito["talk"].toString();
                    et.unfinished = ito["u"].toBool();
                    et.vanished = ito["v"].toBool();
                    lvl.npc.insert(i, et);
                }
            }

            if(entry.contains("chains"))
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
                        QRegExp r_npc("npc-(\\d*)\\-talk", Qt::CaseInsensitive, QRegExp::RegExp2);
                        QRegExp r_event("event-(\\d*)\\-msg", Qt::CaseInsensitive, QRegExp::RegExp2);

                        if(r_npc.exactMatch(s))
                        {
                            n.item_index = r_npc.cap(1).toInt();
                            n.type = TranslationData_DialogueNode::T_NPC_TALK;
                            d.push_back(n);
                        }
                        else if(r_event.exactMatch(s))
                        {
                            n.item_index = r_event.cap(1).toInt();
                            n.type = TranslationData_DialogueNode::T_EVENT_MSG;
                            d.push_back(n);
                        }
                    }

                    TranslationData_DialogueNode n;
                    n.type = TranslationData_DialogueNode::T_END;
                    d.push_back(n);
                    lvl.chains.push_back(d);
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
            wld.title = entry.value("title").toString();
            wld.credits = entry.value("credits").toString();

            if(entry.contains("levels"))
            {
                QJsonArray arr = entry["levels"].toArray();
                for(const auto &it : arr)
                {
                    auto ito = it.toObject();
                    int i;
                    TranslationData_LEVEL et;
                    i = ito["i"].toInt(-1);
                    et.level_index = i;
                    et.title = ito["tit"].toString();
                    et.filename = ito["f"].toString("");
                    et.unfinished = ito["u"].toBool(false);
                    et.vanished = ito["v"].toBool(false);
                    wld.level_titles.insert(i, et);
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
                    l.source = ito["src"].toString();
                    l.translation = ito["tr"].toString();
                    l.unfinished = ito["u"].toBool(false);
                    l.vanished = ito["v"].toBool(false);
                    script.lines[l.line] = l;
                }
            }
        }
    }
}
