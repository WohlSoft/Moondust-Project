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

#include "controller_joystick.h"
#include <QtDebug>


JoystickController::JoystickController() :
    Controller(),
    joystickController(0)
{
//    qDebug() << "Num of joysticks: " << SDL_NumJoysticks();
//    if(SDL_NumJoysticks() > 0){
//        //TODO: Select which controller you want to use.
//        joystickController =  SDL_JoystickOpen(0);
//        qDebug() << "Opened Controller \"" << SDL_JoystickName(joystickController) << "\"";
//    }

    kmap.jump       = 1;
    kmap.jump_alt   = 3;
    kmap.run        = 0;
    kmap.run_alt    = 2;
    kmap.drop       = 8;
    kmap.start      = 9;
    kmap.left       = 10;
    kmap.right      = 11;
    kmap.up         = 12;
    kmap.down       = 13;
}

JoystickController::~JoystickController()
{}

void JoystickController::setJoystickDevice(SDL_Joystick *jctrl)
{
    joystickController=jctrl;
}

void JoystickController::setJoyCtrlMap(KeyMapJoyCtrls ids, KeyMapJoyCtrls types)
{
    _ctrls_ids=ids;
    _ctrls_types=types;
}

SDL_Joystick *JoystickController::getJoystickDevice() const
{
    return joystickController;
}

void JoystickController::updateKey(bool &key, int &keyValue, int &keyID, int &keyType)
{
    int val=0, dx=0, dy=0;
    switch(keyType)
    {
    case KeyMapJoyCtrls::JoyAxis:
        val=SDL_JoystickGetAxis(joystickController, keyID);
            if(keyValue>0)
                key=(val>0);
            else if(keyValue<0)
                key=(val<0);
            else key=false;
        break;
    case KeyMapJoyCtrls::JoyBallX:
        SDL_JoystickGetBall(joystickController, keyID, &dx, &dy);
        if(keyID>0)
            key=(dx>0);
        else if(keyID<0)
            key=(dx<0);
        else key=false;
        break;
    case KeyMapJoyCtrls::JoyBallY:
        SDL_JoystickGetBall(joystickController, keyID, &dx, &dy);
        if(keyID>0)
            key=(dy>0);
        else if(keyID<0)
            key=(dy<0);
        else key=false;
        break;
    case KeyMapJoyCtrls::JoyHat:
        val=SDL_JoystickGetHat(joystickController, keyID);
        key = (val==keyValue);
        break;
    case KeyMapJoyCtrls::JoyButton:
        key = SDL_JoystickGetButton(joystickController, keyID);
        break;
    default:
        key=false;
        break;
    }
}

void JoystickController::update()
{
    if(!joystickController)
        return;

    SDL_PumpEvents();
    SDL_JoystickUpdate();

    updateKey(keys.jump, kmap.jump,   _ctrls_ids.jump,  _ctrls_types.jump);
    updateKey(keys.alt_jump, kmap.jump_alt, _ctrls_ids.jump_alt, _ctrls_types.jump_alt);

    updateKey(keys.run, kmap.run,     _ctrls_ids.run,   _ctrls_types.run);
    updateKey(keys.alt_run, kmap.run_alt, _ctrls_ids.run_alt, _ctrls_types.run_alt);

    updateKey(keys.right, kmap.right, _ctrls_ids.right, _ctrls_types.right);
    updateKey(keys.left, kmap.left,   _ctrls_ids.left,  _ctrls_types.left);
    updateKey(keys.up, kmap.up,       _ctrls_ids.up,    _ctrls_types.up);
    updateKey(keys.down, kmap.down,   _ctrls_ids.down,  _ctrls_types.down);

    updateKey(keys.drop, kmap.drop,   _ctrls_ids.drop,  _ctrls_types.drop);
    updateKey(keys.start, kmap.start, _ctrls_ids.start, _ctrls_types.start);
}



bool JoystickController::bindJoystickKey(SDL_Joystick *joy, int &keyValue, int &keyID, int &keyType)
{
    int val=0, dx=0, dy=0;
    //SDL_PumpEvents();
    SDL_JoystickUpdate();

    for(int i=0; i<SDL_JoystickNumAxes(joy);i++)
    {
        val=0;
        val=SDL_JoystickGetAxis(joy, i);
        if(val!=0)
        {
            keyValue=val;
            keyID=i;
            keyType=KeyMapJoyCtrls::JoyAxis;
            return true;
        }
    }

    for(int i=0; i<SDL_JoystickNumBalls(joy);i++)
    {
        dx=0; dy=0;
        SDL_JoystickGetBall(joy, i, &dx, &dy);
        if(dx!=0)
        {
            keyValue=dx;
            keyID=i;
            keyType=(int)KeyMapJoyCtrls::JoyBallX;
            return true;
        }else if(dy!=0)
        {
            keyValue=dy;
            keyID=i;
            keyType=(int)KeyMapJoyCtrls::JoyBallY;
            return true;
        }
    }

    for(int i=0; i<SDL_JoystickNumHats(joy);i++)
    {
        val=0;
        val=SDL_JoystickGetHat(joy, i);
        if(val!=0)
        {
            keyValue=val;
            keyID=i;
            keyType=(int)KeyMapJoyCtrls::JoyHat;
            return true;
        }
    }
    for(int i=0; i<SDL_JoystickNumButtons(joy);i++)
    {
        val=0;
        val=SDL_JoystickGetButton(joy, i);
        if(val==1)
        {
            keyValue=val;
            keyID=i;
            keyType=(int)KeyMapJoyCtrls::JoyButton;
            return true;
        }
    }

    keyValue=0;
    keyID=0;
    keyType=(int)KeyMapJoyCtrls::NoControl;
    return false;
}
