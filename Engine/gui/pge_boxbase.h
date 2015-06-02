#ifndef PGE_BOXBASE_H
#define PGE_BOXBASE_H

#include "../scenes/scene.h"
#include <QString>

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
    PGE_BoxBase();
    PGE_BoxBase(Scene *_parentScene = 0);
    virtual ~PGE_BoxBase();

    virtual void exec();

    /**************Fader**************/
    float fader_opacity;
    float target_opacity;
    float fade_step;
    int fadeSpeed;
    float manual_ticks;
    void setFade(int speed, float target, float step);
    bool tickFader(float ticks);
    void fadeStep();
    /**************Fader**************/

    void loadTexture(QString path);

    void updateTickValue();

protected:
    Scene * parentScene;
    void construct(Scene *_parentScene = 0);
    float uTickf;
    int   uTick;

    void drawTexture(int left, int top, int right, int bottom, int border=32);
    void drawTexture(PGE_Rect _rect, int border=32);
    bool _textureUsed;
    void drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture);
    PGE_Texture styleTexture;
};

#endif // PGE_BOXBASE_H
