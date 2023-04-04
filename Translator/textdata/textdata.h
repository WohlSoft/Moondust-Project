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


struct TranslationData_NPC
{
    int npc_index = -1;
    int talk_trigger = -1;
    int npc_id = -1;
    QString talk;
    bool unfinished = false;
    bool vanished = false;
};

struct TranslationData_EVENT
{
    int event_index = -1;
    int trigger_next = -1;
    QVector<int> triggered_by_npc;
    QVector<int> triggered_by_event;
    QString message;
    bool unfinished = false;
    bool vanished = false;
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

typedef QVector<TranslationData_DialogueNode> TranslationData_Dialogue;

struct TranslationData_Level
{
    QString title;
    bool title_unfinished = false;
    //! Text data stored at NPC entries
    QMap<int, TranslationData_NPC> npc;
    //! Text data stored at event entries
    QMap<int, TranslationData_EVENT> events;
    //! Dialogue sequences (used to make an order and see the order of messages)
    QVector<TranslationData_Dialogue> chains;
    //! Stoirage of useful translations
    QMap<QString, QString> glossary;
};

struct TranslationData_LEVEL
{
    int level_index = -1;
    QString title;
    QString filename;
    bool unfinished = false;
    bool vanished = false;
};

struct TranslationData_World
{
    QString title;
    bool title_unfinished = false;
    QString credits;
    bool credits_unfinished = false;
    QMap<int, TranslationData_LEVEL> level_titles;
    //! Stoirage of useful translations
    QMap<QString, QString> glossary;
};

struct TranslationData_ScriptLine
{
    int line = -1;
    QString source;
    QString translation;
    bool unfinished = false;
    bool vanished = false;
};

struct TranslationData_Script
{
    QString title;
    QMap<int, TranslationData_ScriptLine> lines;
};

struct TranslationData
{
    QSet<QString> directories;
    QMap<QString, TranslationData_Level> levels;
    QMap<QString, TranslationData_World> worlds;
    QMap<QString, TranslationData_Script> scripts;
};

typedef QMap<QString, TranslationData> TranslateProject;

#endif // TEXTDATA_H
