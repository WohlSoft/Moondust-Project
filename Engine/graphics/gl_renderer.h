/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <string>
#include <common_features/rectf.h>
#include <common_features/point.h>
#include <common_features/pge_texture.h>

struct SDL_Thread;

class GlRenderer
{
public:
    /**
     * @brief Render engine type
     */
    enum RenderEngineType
    {
        //! Can't initialize any (or wanted) renderer engine
        RENDER_INVALID  = -1,
        //! Automatically choose the renderer engine on unit test probes
        RENDER_AUTO     = 0,
        //! OpenGL 2.1 renderer engine (with support for power-of-two textures only video cards)
        RENDER_OPENGL_2_1,
        //! OpenGL 3.1 renderer engine
        RENDER_OPENGL_3_1,
        //! Software renderer with using of SDL2 API
        RENDER_SW_SDL
    };

    /**
     * @brief Choose renderer and try it out if it works
     * @param rtype Wanted renderer engine
     * @return Obtined renderer engine
     */
    static RenderEngineType setRenderer(RenderEngineType rtype = RENDER_AUTO);

    /**
     * @brief Initialize OpenGL 2.1 renderer
     */
    static void setup_OpenGL21();

    /**
     * @brief Initialize OpenGL 3.1 (compatibility profile) renderer
     */
    static void setup_OpenGL31();

    /**
     * @brief Initialize Software-SDL renderer
     */
    static void setup_SW_SDL();

    /**
     * @brief Get renderer engine name
     * @return string with engine name
     */
    static std::string engineName();

    /**
     * @brief Get SDL Window init flags in condition of choosen renderer context
     * @return integer contains additional init flags to SDL rendering setup
     */
    static unsigned int SDL_InitFlags();

    /**
     * @brief Initialize renderer engine
     * @return true if everything is fine, false on any error
     */
    static bool init();
    /**
     * @brief DeInitialize renderer engine
     * @return true if everything is fine, false on any error
     */
    static bool uninit();

    /**
     * @brief Take current frame as screenshot
     */
    static void makeShot();

private:
    /**
     * @brief makeShot_action Asyncronious callback of screenshot capture
     * @param _pixels Raw pixels array taken from off current frame
     * @return Always 0
     */
    static int  makeShot_action(void *_pixels);

public:

    /**
     * @brief Is GIF/Video recorder turned on?
     * @return true if GIF/Video recorder is on
     */
    static bool recordInProcess();

    /**
     * @brief Toggle GIF/Video recorder
     */
    static void toggleRecorder();

    /**
     * @brief Record one frame (GIF/Video recorder)
     * @param ticktime Time delay of one frame
     */
    static void processRecorder(double ticktime);

private:
    static int  processRecorder_action(void *_pixels);

public:
    /**
     * @brief Is rendering engine ready?
     * @return true if rendering engine is ready
     */
    static bool ready();

    /**
     * @brief Flush current rendering frame buffer
     */
    static void flush();

    /**
     * @brief Repaint current frame buffer state
     */
    static void repaint();

    /*!
     * \brief Changed color to clear screen
     * \param r level of red (from 0.0 to 1.0)
     * \param g level of green (from 0.0 to 1.0)
     * \param b level of blue (from 0.0 to 1.0)
     * \param a level of alpha (from 0.0 to 1.0)
     */
    static void setClearColor(float r, float g, float b, float a);

    /*!
     * \brief Clear screen with pre-defined clear color
     */
    static void clearScreen();

    /**
     * @brief Render texture as-is
     * @param texture Texture context
     * @param x Screen position X
     * @param y Screen position Y
     */
    static void renderTexture(PGE_Texture *texture, float x, float y);

    /**
     * @brief Render matrix animation fragment
     * @param texture Texture context
     * @param x Screen position X
     * @param y Screen position Y
     * @param w Width of texture
     * @param h Height of texture
     * @param ani_top Top texture side position (between 0.0f and 1.0f)
     * @param ani_bottom Bottom texture side position (between 0.0f and 1.0f)
     * @param ani_left Left texture side position (between 0.0f and 1.0f)
     * @param ani_right Right texture side position (between 0.0f and 1.0f)
     */
    static void renderTexture(PGE_Texture *texture,
                              float x, float y, float w, float h,
                              float ani_top = 0.0f, float ani_bottom = 1.0f,
                              float ani_left = 0.0f, float ani_right = 1.0f);

    /**
     * @brief Draw color rectangle (using x-y positions and width-height sizes)
     * @param x Screen position X
     * @param y Screen position Y
     * @param w Width of rectangle
     * @param h Height of rectangle
     * @param red Red color level (Between 0.0f and 1.0f)
     * @param green Green color level (Between 0.0f and 1.0f)
     * @param blue Blue color level color level (Between 0.0f and 1.0f)
     * @param alpha Alpha-channel level (Between 0.0f and 1.0f)
     * @param filled Draw solid rectangle, otherwise, rectangular outline only will be drawn
     */
    static void renderRect(float x, float y,
                           float w, float h,
                           GLfloat red = 1.f, GLfloat green = 1.f, GLfloat blue = 1.f, GLfloat alpha = 1.f,
                           bool filled = true);

    /**
     * @brief Draw color rectangle (using left-top-right-bottom positions)
     * @param _left Screen position of left side
     * @param _top Screen position of top side
     * @param _right Screen position of right side
     * @param _bottom Screen position of bottom side
     * @param red Red color level (Between 0.0f and 1.0f)
     * @param green Green color level (Between 0.0f and 1.0f)
     * @param blue Blue color level color level (Between 0.0f and 1.0f)
     * @param alpha Alpha-channel level (Between 0.0f and 1.0f)
     */
    static void renderRectBR(double _left, double _top,
                             double _right, double _bottom,
                             GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

    /**
     * @brief Draw currently binded texture
     * @param x Screen position X
     * @param y Screen position Y
     * @param w Width of texture
     * @param h Height of texture
     * @param ani_top Top texture side position (between 0.0f and 1.0f)
     * @param ani_bottom Bottom texture side position (between 0.0f and 1.0f)
     * @param ani_left Left texture side position (between 0.0f and 1.0f)
     * @param ani_right Right texture side position (between 0.0f and 1.0f)
     */
    static void renderTextureCur(float x, float y,
                                 float w, float h,
                                 float ani_top=0.0f, float ani_bottom=1.0f,
                                 float ani_left=0.0f, float ani_right=1.0f);

    /**
     * @brief Bind the texture
     * @param texture Texture context
     */
    static void BindTexture(PGE_Texture *texture);

    /**
     * @brief Set the color of texture
     * @param red Red color level (Between 0.0f and 1.0f)
     * @param green Green color level (Between 0.0f and 1.0f)
     * @param blue Blue color level color level (Between 0.0f and 1.0f)
     * @param alpha Alpha-channel level (Between 0.0f and 1.0f)
     */
    static void setTextureColor(float Red, float Green, float Blue, float Alpha = 1.0f);

    /**
     * @brief Unbind the current texture
     */
    static void UnBindTexture();

    /**
     * @brief Helper function: Map the point of the physical window position into virtual surface
     * @param point Point in coordinates of physical window surface
     * @return Point in coordinates of virtual surface
     */
    static PGE_Point  MapToScr(PGE_Point point);

    /**
     * @brief Helper function: Map the point of the physical window position into virtual surface
     * @param x X position in coordinates of physical window surface
     * @param y Y position in coordinates of physical window surface
     * @return Point in coordinates of virtual surface
     */
    static PGE_Point  MapToScr(int x, int y);

    /**
     * @brief Helper function: Allign body horizontally to center of screen
     * @param x X position of body
     * @param w Width of body
     * @return Alligned X position of the body
     */
    static int  alignToCenterW(int x, int w);

    /**
     * @brief Helper function: Allign body vertically to center of screen
     * @param y Y position of body
     * @param h Height of body
     * @return Alligned Y position of the body
     */
    static int  alignToCenterH(int y, int h);

    /**
     * @brief Set the viewport rectangle
     * @param x X position of viewport rectangle
     * @param y Y position of viewport rectangle
     * @param w Width of viewport
     * @param h Height of viewport
     */
    static void setViewport(int x, int y, int w, int h);
    /**
     * @brief Reset viewport back to full screen
     */
    static void resetViewport();

    /**
     * @brief Set the size of viewport
     * @param w Width of viewport
     * @param h Height of viewport
     */
    static void setViewportSize(int w, int h);

    /**
     * @brief Set the size of virtual surface
     * @param w Width of virtual surface
     * @param h Height of virtual surface
     */
    static void setVirtualSurfaceSize(int w, int h);

    /**
     * @brief Load the texture from file
     * @param path Path to image file
     * @param maskPath Path to bitwise transparency mask (or keep empty)
     * @param maskFallbackPath Path to fallback (transparent PNG) to extract bitwise mask for given front (or keep empty)
     * @return Texture context
     */
    static PGE_Texture loadTexture(std::string path,
                                   std::string maskPath = std::string(),
                                   std::string maskFallbackPath = std::string());

    /**
     * @brief loadTextureP
     * @param target Destination texture context
     * @param path Path to image file
     * @param maskPath Path to bitwise transparency mask (or keep empty)
     * @param maskFallbackPath Path to fallback (transparent PNG) to extract bitwise mask for given front (or keep empty)
     */
    static void loadTextureP(PGE_Texture &target,
                             std::string path,
                             std::string maskPath = std::string(),
                             std::string maskFallbackPath = std::string());

    /**
     * @brief Load texture from raw pixels array
     * @param target Destination texture context
     * @param pixels Raw RGBA pixels array
     * @param width Width of image
     * @param height Height of image
     */
    static void loadRawTextureP(PGE_Texture &target, uint8_t *pixels, uint32_t width, uint32_t height);

    /**
     * @brief Unload the texture
     * @param tx Texture context with texture
     */
    static void deleteTexture(PGE_Texture &tx);

    /**
     * @brief Vertical direction of raw pixel data
     * @return true of pixel rows are ordered from top to bottom
     */
    static bool isTopDown();

    /**
     * @brief Get size of raw pixels data from the texture context
     * @param tx Texture context
     * @return size of texture raw pixels data in bytes
     */
    static int  getPixelDataSize(const PGE_Texture *tx);

    /**
     * @brief Get raw pixel data from off the texture context
     * @param tx Texture context
     * @param pixelData Pointer to array to write raw pixel data
     */
    static void getPixelData(const PGE_Texture *tx, unsigned char* pixelData);

private:
    //Virtual resolution of renderable zone
    //! Virtual resolution of renderable zone width
    static int m_viewport_w;
    //! Virtual resolution of renderable zone height
    static int m_viewport_h;

    //Scale of virtual and window resolutuins
    //! Horizontal scale factor between of virtual resolution and the window size
    static float m_scale_x;
    //! Vertical scale factor between of virtual resolution and the window size
    static float m_scale_y;

    //Side offsets to keep ratio
    //! Keep aspect ratio horizontal offset of virtual zone
    static float m_offset_x;
    //! Keep aspect ratio vertical offset of virtual zone
    static float m_offset_y;

    static SDL_Thread*  m_screenshot_thread;
    //! Is rendering engine is ready for work?
    static bool         m_isReady;
};

#endif // GL_RENDERER_H

