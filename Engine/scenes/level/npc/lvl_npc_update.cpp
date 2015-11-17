/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../lvl_npc.h"
#include "../lvl_section.h"
#include "../lvl_scene_ptr.h"

void LVL_Npc::update(float tickTime)
{
    float accelCof=tickTime/1000.0f;
    if(killed) return;
    if(wasDeactivated) return;

    _onGround = !collided_bottom.isEmpty();

    if(isGenerator) {
        updateCollisions();
        activationTimeout-=tickTime;
        updateGenerator(tickTime);
        return;
    }

    event_queue.processEvents(tickTime);

    animator.manualTick(tickTime);

    if(warpSpawing) {
        setSpeed(0.0, 0.0);
        return;
    }

    PGE_Phys_Object::update(tickTime);
    if(deActivatable) activationTimeout-=tickTime;

    if(motionSpeed!=0) {
        if(!collided_left.isEmpty())
            setDirection(1);
        else
        if(!collided_right.isEmpty())
            setDirection(-1);
        else
        if(setup->turn_on_cliff_detect && cliffDetected)
        {
            setDirection(_direction*-1);
            cliffDetected=false;
        }

        setSpeedX((motionSpeed*accelCof)*_direction);
    }

    if(not_movable()) {
        detector_player_pos.processDetector();
        if(detector_player_pos.directedTo()!=0)
            setDirection(detector_player_pos.directedTo());
    }

    LVL_Section *section=sct();
    PGE_RectF sBox = section->sectionRect();

    if(offSectionDeactivate) {
        if(!sBox.collideRect(posRect)) {
            if(activationTimeout>100)
                activationTimeout=100;
            //Iterate activation timeout if deactivation disabled by default
            if(!deActivatable) activationTimeout-=tickTime;
        }
    }

    if(section->isWrapH()) {
        if(posX()<sBox.left()-_width-1 )
            setPosX(sBox.right()-1);
        else
        if(posX()>sBox.right() + 1 )
            setPosX(sBox.left()-_width+1);
    }

    if(section->isWrapV()) {
        if(posY()<sBox.top()-_height-1 )
            setPosY(sBox.bottom()-1);
        else
        if(posY()>sBox.bottom() + 1 )
            setPosY(sBox.top()-_height+1);
    } else if((setup->kill_on_pit_fall) && (posY() > sBox.bottom()+_height) ) {
        kill(DAMAGE_PITFALL);
    }

    for(int i=0; i<detectors.size(); i++)
        detectors[i]->processDetector();

    try{
        lua_onLoop(tickTime);
    } catch (luabind::error& e) {
        LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
    }
}
