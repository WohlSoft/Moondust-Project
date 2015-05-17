#ifndef GLCOLOR_H
#define GLCOLOR_H

#include <SDL2/SDL_opengl.h>
#include <QString>

class GlColor
{
public:
    GlColor();
    GlColor(const GlColor &clr);
    GlColor(GLuint rgba);
    GlColor(QString rgba);
    ~GlColor();
    void setRgba(GLuint rgba);
    void setRgba(QString rgba);
    void setRed(float _r);
    void setGreen(float _g);
    void setBlue(float _b);
    void setAlpha(float _a);
    void setRedI(float _r);
    void setGreenI(float _g);
    void setBlueI(float _b);
    void setAlphaI(float _a);
    float Red();
    float Green();
    float Blue();
    float Alpha();
private:
    float r;
    float g;
    float b;
    float a;
};

#endif // GLCOLOR_H
