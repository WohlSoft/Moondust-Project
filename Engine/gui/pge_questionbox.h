#ifndef PGE_QUESTIONBOX_H
#define PGE_QUESTIONBOX_H

#include "pge_menuboxbase.h"

class PGE_QuestionBox : public PGE_MenuBoxBase
{
public:
    PGE_QuestionBox(Scene * _parentScene=NULL, QString _title="Menu is works!",
               msgType _type=msg_info, PGE_Point boxCenterPos=PGE_Point(-1,-1), float _padding=-1, QString texture="");
    PGE_QuestionBox(const PGE_QuestionBox &mb);

    ~PGE_QuestionBox();

    void onLeftButton();
    void onRightButton();
    void onJumpButton();
    void onAltJumpButton();
    void onRunButton();
    void onAltRunButton();
    void onStartButton();
    void onDropButton();
};

#endif // PGE_QUESTIONBOX_H
