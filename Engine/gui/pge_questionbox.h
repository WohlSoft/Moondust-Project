#ifndef PGE_QUESTIONBOX_H
#define PGE_QUESTIONBOX_H

#include "pge_menuboxbase.h"

class PGE_QuestionBox : public PGE_MenuBoxBase
{
public:
    explicit PGE_QuestionBox(Scene * _parentScene = nullptr, std::string _title = "Menu is works!",
            msgType _type = msg_info, PGE_Point boxCenterPos = PGE_Point(-1,-1),
            double _padding = -1, std::string texture = "");
    PGE_QuestionBox(const PGE_QuestionBox &mb);
    ~PGE_QuestionBox() override = default;

    void onLeftButton() override;
    void onRightButton() override;
    void onJumpButton() override;
    void onAltJumpButton() override;
    void onRunButton() override;
    void onAltRunButton() override;
    void onStartButton() override;
    void onDropButton() override;
};

#endif // PGE_QUESTIONBOX_H
