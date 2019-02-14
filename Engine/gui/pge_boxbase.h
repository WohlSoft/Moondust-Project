#ifndef PGE_BOXBASE_H
#define PGE_BOXBASE_H

#include "../scenes/scene.h"
#include <string>

#include <common_features/rect.h>
#include <common_features/rectf.h>
#include <common_features/pge_texture.h>

///
/// \brief The PGE_BoxBase class
///
/// This class provides the basis of message boxes, menus, questions, etc.

class PGE_BoxBase
{
public:
    enum msgType
    {
        msg_info=0,
        msg_info_light,
        msg_warn,
        msg_error,
        msg_fatal
    };

    explicit PGE_BoxBase(Scene *parentScene = nullptr);
    PGE_BoxBase(const PGE_BoxBase &bb);
    virtual ~PGE_BoxBase();

    virtual void setParentScene(Scene *_parentScene);

    virtual void exec();
    virtual void update(double tickTime);
    virtual void render();

    /**************Fader**************/
    double  m_faderOpacity;
    double  m_targetOpacity;
    double  m_fadeStep;
    int     m_fadeSpeed;
    double  m_manualTicks;
    void    setFade(int speed, double target, double step);
    bool    tickFader(double ticks);
    void    fadeStep();
    /**************Fader**************/

    void loadTexture(std::string path);

    void updateTickValue();

protected:
    Scene * m_parentScene;
    void construct(Scene *parentScene = nullptr);
    double m_uTickf;
    int    m_uTick;

    void drawTexture(int left, int top, int right, int bottom, int border=32, float opacity=1.0f);
    void drawTexture(PGE_Rect _rect, int border=32, float opacity=1.0f);
    bool m_textureUsed;
    void drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture);
    PGE_Texture m_styleTexture;
};

#endif // PGE_BOXBASE_H
