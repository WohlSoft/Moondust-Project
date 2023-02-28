#ifndef TEXTDATA_H
#define TEXTDATA_H

/*
    Translation data structure, designed according by plan:
    https://github.com/Wohlstand/TheXTech/discussions/340
*/

#include <QString>
#include <QMap>
#include <QVector>


struct TranslationData_NPC
{
    int npc_index = -1;
    int talk_trigger = -1;
    QString talk;
};

struct TranslationData_EVENT
{
    int event_index = -1;
    int trigger_next = -1;
    QString message;
};

struct TranslationData_DialogueNode
{
    enum Type
    {
        T_END = -1,
        T_NPC_TALK = 0,
        T_EVENT_MSG
    } type = T_END;

    int item_id = -1;
};

struct TranslationData_Level
{
    QString title;
    //! Text data stored at NPC entries
    QMap<int, TranslationData_NPC> npc;
    //! Text data stored at event entries
    QMap<int, TranslationData_EVENT> events;
    //! Dialogue sequences (used to make an order and see the order of messages)
    QVector<QVector<TranslationData_DialogueNode> > chains;
    //! Stoirage of useful translations
    QMap<QString, QString> glossary;
};

struct TranslationData_World
{
    QString title;
    QVector<QString > credits;
    QMap<int, QString> level_titles;
    //! Stoirage of useful translations
    QMap<QString, QString> glossary;
};

struct TranslationData
{
    QMap<QString, TranslationData_Level> levels;
    QMap<QString, TranslationData_World> worlds;
};

#endif // TEXTDATA_H
