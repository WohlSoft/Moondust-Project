#include <stdio.h>
#include <stdarg.h>

#include "phys_debug_draw.h"
#undef main
#include <SDL2/SDL_opengl.h>
#include <scenes/level/lvl_camera.h>
#include <fontman/font_manager.h>

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int32 i = 0; i < vertexCount; ++i)
    {
        glVertex2f(vertices[i].x*10 -c->posX(), vertices[i].y*10-c->posY());
    }
    glEnd();
}
void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    for (int32 i = 0; i < vertexCount; ++i)
    {
        glVertex2f(vertices[i].x*10-c->posX(), vertices[i].y*10 - c->posY());
    }
    glEnd();
    glDisable(GL_BLEND);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int32 i = 0; i < vertexCount; ++i)
    {
        glVertex2f(vertices[i].x*10-c->posX(), vertices[i].y*10-c->posY());
    }
    glEnd();
    glEnable(GL_BLEND);
}
void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    const float32 k_segments = 16.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertex2f(v.x, v.y);
        theta += k_increment;
    }
    glEnd();
}
void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    const float32 k_segments = 16.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 theta = 0.0f;
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertex2f(v.x, v.y);
        theta += k_increment;
    }
    glEnd();
    glDisable(GL_BLEND);
    theta = 0.0f;
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
        glVertex2f(v.x, v.y);
        theta += k_increment;
    }
    glEnd();
    b2Vec2 p = center + radius * axis;
    glBegin(GL_LINES);
    glVertex2f(center.x, center.y);
    glVertex2f(p.x, p.y);
    glEnd();
}
void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(p1.x*10-c->posX(), p1.y*10-c->posY());
    glVertex2f(p2.x*10-c->posX(), p2.y*10-c->posY());
    glEnd();
}
void DebugDraw::DrawTransform(const b2Transform& xf)
{
    b2Vec2 p1 = xf.p, p2;
    const float32 k_axisScale = 0.4f;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glVertex2f(p1.x*10-c->posX(), p1.y*10-c->posY());
    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    glVertex2f(p2.x*10-c->posX(), p2.y*10-c->posY());
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glVertex2f(p1.x*10-c->posX(), p1.y*10-c->posY());
    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    glVertex2f(p2.x*10-c->posX(), p2.y*10-c->posY());
    glEnd();
}
void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
    glDisable(GL_TEXTURE_2D);
    glPointSize(size);
    glBegin(GL_POINTS);
    glColor4f(color.r, color.g, color.b, 1.0f);
    glVertex2f(p.x*10-c->posX(), p.y*10-c->posY());
    glEnd();
    glPointSize(1.0f);
}
void DebugDraw::DrawString(int x, int y, const char *string, ...)
{
    char buffer[128];
    va_list arg;
    va_start(arg, string);
    vsprintf(buffer, string, arg);
    va_end(arg);
    FontManager::printText(buffer, x*10-c->posX(), y*10-c->posY());
    //imguiDrawText(x, h - y, IMGUI_ALIGN_LEFT, buffer, imguiRGBA(230, 153, 153, 255));
}

void DebugDraw::DrawString(const b2Vec2& p, const char *string, ...)
{
    char buffer[128];
    va_list arg;
    va_start(arg, string);
    vsprintf(buffer, string, arg);
    va_end(arg);
    FontManager::printText(buffer, (int)p.x*10-c->posX(), (int)p.y*10-c->posY());
    //imguiDrawText((int)p.x, h - (int)p.y, IMGUI_ALIGN_LEFT, buffer, imguiRGBA(230, 153, 153, 255));
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& cl)
{
    glDisable(GL_TEXTURE_2D);
    glColor4f(cl.r, cl.g, cl.b, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(aabb->lowerBound.x*10-c->posX(), aabb->lowerBound.y*10-c->posY());
    glVertex2f(aabb->upperBound.x*10-c->posX(), aabb->lowerBound.y*10-c->posY());
    glVertex2f(aabb->upperBound.x*10-c->posX(), aabb->upperBound.y*10-c->posY());
    glVertex2f(aabb->lowerBound.x*10-c->posX(), aabb->upperBound.y*10-c->posY());
    glEnd();
}


