#include "b2d_render.h"
#include <graphics/gl_renderer.h>

void b2d_render::DrawBody(b2Body *body, float posX, float posY, float red, float green, float blue, float alpha, float width, bool filled)
{
    const b2Transform& xf = body->GetTransform();
    for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
    {
        switch (f->GetType())
        {
        case b2Shape::e_polygon:
        {
            b2PolygonShape* poly = (b2PolygonShape*) f->GetShape();
            int32 vertexCount = poly->GetVertexCount();
            b2Assert(vertexCount <= b2_maxPolygonVertices);
            glDisable(GL_TEXTURE_2D);
            glColor4f(red, green, blue, alpha);
            glBegin(filled ? GL_POLYGON : GL_LINE_LOOP );
            glLineWidth(width);
            for (int32 i = 0; i < vertexCount; ++i)
            {
                b2Vec2 vertice = b2Mul(xf, poly->GetVertex(i));
                PGE_PointF pt = GlRenderer::MapToGl(vertice.x*10.f-posX, vertice.y*10.f-posY);
                glVertex2f(pt.x(), pt.y());
            }
            glEnd();
        }
            break;
        default: break;
        }
    }
}
