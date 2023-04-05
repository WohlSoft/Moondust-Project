#ifndef TEXT_TYPES_H
#define TEXT_TYPES_H

namespace TextTypes
{

enum SourceType
{
    S_WORLD = 0,
    S_LEVEL,
    S_SCRIPT
};

enum State
{
    ST_BLANK = 0,
    ST_UNFINISHED,
    ST_FINISHED,
    ST_VANISHED
};

enum L_DType
{
    LDT_EVENT = 0,
    LDT_NPC,
    LDT_TITLE
};

enum W_DType
{
    WDT_LEVEL = 0,
    WDT_TITLE,
    WDT_CREDITS
};

enum S_DType
{
    SDT_LINE = 0
};

}

#endif // TEXT_TYPES_H
