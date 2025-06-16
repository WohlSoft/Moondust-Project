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

#ifndef TEXTDATA_H
#define TEXTDATA_H

/*
    Translation data structure, designed according by plan:
    https://github.com/Wohlstand/TheXTech/discussions/340
*/

#include <QString>
#include <QMap>
#include <QSet>
#include <QVector>


struct TrLine
{
    QString text;
    QString note;
    bool unfinished = false;
    bool vanished = false;
};

struct TranslationData_NPC
{
    int npc_index = -1;
    int talk_trigger = -1;
    int npc_id = -1;
    QString talk_tr_id;
    TrLine talk;
};

struct TranslationData_EVENT
{
    int event_index = -1;
    int trigger_next = -1;
    QVector<int> triggered_by_npc;
    QVector<int> triggered_by_event;
    QString message_tr_id;
    TrLine message;
};

struct TranslationData_DialogueNode
{
    enum Type
    {
        T_END = -1,
        T_NPC_TALK = 0,
        T_EVENT_MSG
    } type = T_END;

    int item_index = -1;
};

struct TranslationData_Dialogue
{
    QString note;
    QVector<TranslationData_DialogueNode> messages;
};

struct TranslationData_Level
{
    TrLine title;
    //! Text data stored at NPC entries
    QMap<int, TranslationData_NPC> npc;
    //! Text data stored at event entries
    QMap<int, TranslationData_EVENT> events;
    //! Dialogue sequences (used to make an order and see the order of messages)
    QVector<TranslationData_Dialogue> dialogues;
    //! Stoirage of useful translations
    QMap<QString, QString> glossary;
    //! Total number of strings in the translation
    int t_strings = 0;
    //! Total number of translated strings
    int t_translated = 0;
};

struct TranslationData_LEVEL
{
    int level_index = -1;
    QString title_tr_id;
    TrLine title;
    QString filename;
};

struct TranslationData_World
{
    TrLine title;
    TrLine credits;
    QMap<int, TranslationData_LEVEL> level_titles;
    //! Stoirage of useful translations
    QMap<QString, QString> glossary;
    //! Total number of strings in the translation
    int t_strings = 0;
    //! Total number of translated strings
    int t_translated = 0;
};

struct TranslationData_ScriptLine
{
    int line = -1;
    QString line_tr_id;
    QString source;
    TrLine translation;
};

struct TranslationData_Script
{
    QString title;
    QMap<int, TranslationData_ScriptLine> lines;
    //! Total number of strings in the translation
    int t_strings = 0;
    //! Total number of translated strings
    int t_translated = 0;
};

struct TranslationData
{
    //! Translation will prefer the TrId system (special keywords). If set to FALSE, the original string will work as a keyword.
    bool useTrId = false;
    //! Will language being visible in the list?
    bool langVisible = true;
    //! Total number of strings in the translation
    int t_strings = 0;
    //! Total number of translated strings
    int t_translated = 0;
    QSet<QString> directories;
    QMap<QString, TranslationData_Level> levels;
    QMap<QString, TranslationData_World> worlds;
    QMap<QString, TranslationData_Script> scripts;

    // Meta-file only: TrId maps to inten indexes
    QMap<QString, int> trId_map_lvl_npc;
    QMap<QString, int> trId_map_lvl_events;
    QMap<QString, int> trId_map_wld_levels;
    QMap<QString, int> trId_map_script_lines;
};

#define TRANSLATE_METADATA "metadata"

typedef QMap<QString, TranslationData> TranslateProject;

#endif // TEXTDATA_H
