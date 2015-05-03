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

#include "config_manager.h"
#include "../gui/pge_msgbox.h"

/*****Level BGO************/
QList<obj_effect >     ConfigManager::lvl_effects;
QMap<long, obj_effect*>   ConfigManager::lvl_effects_indexes;
CustomDirManager ConfigManager::Dir_EFFECT;
QList<SimpleAnimator > ConfigManager::Animator_EFFECT;
/*****Level BGO************/


bool ConfigManager::loadLevelEffects()
{
    unsigned int i;

    obj_effect sbgo;
    unsigned long effects_total=0;
    QString effects_ini = config_dir + "lvl_effects.ini";

    if(!QFile::exists(effects_ini))
    {
        addError(QString("ERROR LOADING lvl_effects.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox msgBox(NULL, QString("ERROR LOADING lvl_bgo.ini: file does not exist"),
                          PGE_MsgBox::msg_fatal);
        msgBox.exec();
        return false;
    }

    QSettings effectset(effects_ini, QSettings::IniFormat);
    effectset.setIniCodec("UTF-8");

    lvl_effects.clear();   //Clear old
    lvl_effects_indexes.clear();

    effectset.beginGroup("effects-main");
        effects_total = effectset.value("total", "0").toInt();
        total_data +=effects_total;
    effectset.endGroup();


    for(i=1; i<=effects_total; i++)
    {

        sbgo.isInit = false;
        sbgo.image = NULL;
        sbgo.textureArrayId = 0;
        sbgo.animator_ID = 0;

        effectset.beginGroup( QString("effect-"+QString::number(i)) );

            sbgo.name = effectset.value("name", "").toString();

                if(sbgo.name=="")
                {
                    addError(QString("EFFECT-%1 Item name isn't defined").arg(i));
                    goto skipEffect;
                }

            imgFile = effectset.value("image", "").toString();
            sbgo.image_n = imgFile;
            if( (imgFile!="") )
            {
                tmp = imgFile.split(".", QString::SkipEmptyParts);
                if(tmp.size()==2)
                    imgFileM = tmp[0] + "m." + tmp[1];
                else
                    imgFileM = "";
                sbgo.mask_n = imgFileM;
            }
            else
            {
                addError(QString("EFFECT-%1 Image filename isn't defined").arg(i));
                goto skipEffect;
            }
            sbgo.frames = effectset.value("frames", "1").toInt();
            sbgo.animated = (sbgo.frames>1);
            sbgo.framestyle = effectset.value("frame-style", "125").toInt();
            sbgo.framespeed = effectset.value("frame-speed", "125").toInt();

            sbgo.frame_h = 0;//(sbgo.animated? qRound(qreal(sbgo.image.height())/sbgo.frames) : sbgo.image.height());
            sbgo.id = i;
            lvl_effects.push_back(sbgo);

            //Add to Index
            lvl_effects_indexes[lvl_effects.last().id] = &lvl_effects.last();

        skipEffect:
        effectset.endGroup();

        if( effectset.status() != QSettings::NoError )
        {
            addError(QString("ERROR LOADING lvl_bgo.ini N:%1 (bgo-%2)").arg(effectset.status()).arg(i), QtCriticalMsg);
        }
    }

    if((unsigned int)lvl_effects.size()<effects_total)
    {
        addError(QString("Not all Effects loaded! Total: %1, Loaded: %2").arg(effects_total).arg(lvl_effects.size()));
        PGE_MsgBox msgBox(NULL, QString("Not all Effectss loaded! Total: %1, Loaded: %2").arg(effects_total).arg(lvl_effects.size()),
                          PGE_MsgBox::msg_error);
        msgBox.exec();
    }
    return true;
}

