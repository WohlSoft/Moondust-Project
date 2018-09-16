#pragma once
#ifndef HISTORY_CUSTOM_SETTING_HHHHH
#define HISTORY_CUSTOM_SETTING_HHHHH
#include <QString>

class QVariant;
class QGraphicsItem;
class HistoryElementCustomSetting
{
public:
    HistoryElementCustomSetting();
    virtual ~HistoryElementCustomSetting();
    virtual void undo(const void *sourceItem, QVariant *mod, QGraphicsItem *item) = 0;
    virtual void redo(const void *sourceItem, QVariant *mod, QGraphicsItem *item) = 0;
    virtual QString getHistoryName() = 0;
};

#endif //HISTORY_CUSTOM_SETTING_HHHHH
