#ifndef PGE_QUESTIONBOX_H
#define PGE_QUESTIONBOX_H

#include "pge_boxbase.h"


#include "pge_boxbase.h"
#include "../scenes/scene.h"

#include <graphics/gl_renderer.h>
#include <graphics/gl_color.h>
#include <common_features/rect.h>
#include <common_features/point.h>
#include <common_features/pointf.h>
#include <common_features/size.h>
#include <common_features/sizef.h>

#include <QColor>
#include <QString>
#include <QStringList>

#include "pge_menu.h"

class Controller;

class PGE_QuestionBox : public PGE_BoxBase
{
public:
    PGE_QuestionBox(Scene * _parentScene=NULL, QString _title="Menu is works!",
               msgType _type=msg_info, PGE_Point boxCenterPos=PGE_Point(-1,-1), float _padding=-1, QString texture="");
    PGE_QuestionBox(const PGE_QuestionBox &mb);

    void construct(QString _title="Menu is works!",
                    msgType _type=msg_info, PGE_Point pos=PGE_Point(-1,-1), float _padding=-1, QString texture="");

    ~PGE_QuestionBox();

    void setParentScene(Scene * _parentScene);
    void setType(msgType _type);
    void setTitleFont(QString fontName);
    void setTitleFontColor(GlColor color);
    void setTitleText(QString text);
    void setPadding(int _padding);

    void clearMenu();
    void addMenuItem(QString &menuitem);
    void addMenuItems(QStringList &menuitems);

    void setPos(float x, float y);
    void setMaxMenuItems(int items);
    void setBoxSize(float _Width, float _Height, float _padding);
    void update(float ticks);
    void render();
    void restart();
    bool isRunning();
    void exec();
    void setRejectSnd(long sndRole);
    int  answer();

    void reject();

    void processKeyEvent(SDL_Keycode &key);

    void processLoader(float ticks);
    void processBox(float);
    void processUnLoader(float ticks);

    static void info(QString msg);
    //static void info(std::string msg);
    static void warn(QString msg);
    //static void warn(std::string msg);
    static void error(QString msg);
    //static void error(std::string msg);
    static void fatal(QString msg);
    //static void fatal(std::string msg);

private:
    void updateSize();
    int     _page;
    bool    running;
    int     fontID;
    GlColor fontRgba;
    int     _answer_id;

    long    reject_snd;
    PGE_Point _pos;
    Controller *_ctrl1;
    Controller *_ctrl2;
    msgType type;

    PGE_Rect _sizeRect;
    QString  title;
    PGE_Size title_size;
    PGE_Menu _menu;

    float width;
    float height;
    float padding;
    QColor bg_color;
    void initControllers();
    void updateControllers();
};

#endif // PGE_QUESTIONBOX_H
