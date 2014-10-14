#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <QPoint>
#include <QPointF>

class GlRenderer
{
public:
    static bool init();
    static bool uninit();
    static QPointF mapToOpengl(QPoint s);
};

#endif // GL_RENDERER_H
