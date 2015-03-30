/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef SCENE_WORLD_H
#define SCENE_WORLD_H

#include <QString>
#include <QList>
#include <QRect>
#include "scene.h"
#include <controls/controller_keyboard.h>
#include <PGE_File_Formats/wld_filedata.h>
#include <common_features/pge_texture.h>
#include <data_configs/config_manager.h>

class WorldScene : public Scene
{
public:
    WorldScene();
    ~WorldScene();

    bool init();
    bool isInit;

    enum ExitWorldCodes
    {
        EXIT_close=-2,
        EXIT_error=-1,
        EXIT_exitWithSave,
        EXIT_exitNoSave,
        EXIT_beginLevel=0
    };
    int exitWorldDelay;
    int exitWorldCode;
    QString targetLevel;

    KeyboardController keyboard1;

    bool worldIsContinues;
    bool doExit;
    bool isPauseMenu;

    void update();
    void render();
    int exec();
    bool isExit();
    void setExiting(int delay, int reason);

    bool loadFile(QString filePath);

    QString getLastError();

    WorldData data;

    QRect viewportRect;

    long posX;
    long posY;
    QString currentMusicFile;

    QString levelTitle;
    long health;
    long stars;
    long points;
    long coins;

    int i;
    int delayToEnter;
    Uint32 lastTicks;
    bool debug_player_jumping;
    bool debug_player_onground;
    int  debug_player_foots;
    int  debug_render_delay;
    int  debug_phys_delay;
    int  debug_event_delay;
    int  uTick;

private:
    QString errorMsg;
    WorldMapData common_setup;

    PGE_Texture backgroundTex;
    QList<PGE_Texture > textures_bank;
};

#endif // SCENE_WORLD_H
