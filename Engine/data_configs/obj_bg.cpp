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

#include "config_manager.h"
#include "config_manager_private.h"
#include <common_features/number_limiter.h>
#include <common_features/util.h>
#include <fmt/fmt_format.h>
#include <Utils/files.h>

/*****Level BG************/
PGE_DataArray<obj_BG>           ConfigManager::lvl_bg_indexes;
CustomDirManager                ConfigManager::Dir_BG;
ConfigManager::AnimatorsArray   ConfigManager::Animator_BG;
/*****Level BG************/

bool ConfigManager::loadLevelBackground(obj_BG &sbg, std::string section, obj_BG *merge_with, std::string iniFile, IniProcessing *setup)
{
    #define pMerge(param, def) (merge_with ? (merge_with->param) : (def))
    bool valid = true;
    bool internal = !setup;

    std::unique_ptr<IniProcessing> ptr_guard;
    if(internal)
    {
        setup = new IniProcessing(iniFile);
        ptr_guard.reset(setup);
    }

    setup->beginGroup(section);
    {
        setup->read("name", sbg.name, pMerge(name, ""));
        if(sbg.name.empty())
        {
            addError(fmt::format("{0} Item name isn't defined", section));
            return false;
        }
    }

    setup->readEnum("type", sbg.type,
                            pMerge(type, 0),
                            {
                                {"single-row", 0},
                                {"double-row", 1},
                                {"tiled", 2}
    });

    sbg.repeat_h = setup->value("repeat-h", (merge_with ? merge_with->repeat_h : 2.0)).toDouble();
    NumberLimiter::applyD(sbg.repeat_h, 1.0, 0.0);

    setup->readEnum("repeat-v", sbg.repead_v,
                            pMerge(repead_v, 0),
                            {
                                {"NR", 0},
                                {"ZR", 1},
                                {"RP", 2},
                                {"RZ", 3}
    });

    setup->read("image", sbg.image_n, pMerge(image_n, ""));
    setup->readEnum("attached", sbg.attached,
                                        pMerge(attached, 0),
                            {
                                {"bottom", 0},
                                {"top", 1}
    });
    setup->read("tiled-in-editor", sbg.editing_tiled, pMerge(editing_tiled, false));
    setup->read("magic", sbg.magic, pMerge(magic, false));

    setup->read("magic-strips", sbg.magic_strips, pMerge(magic_strips, 1));
    NumberLimiter::applyD(sbg.magic_strips, 1u, 1u);
    setup->read("magic-splits", sbg.magic_splits_i, pMerge(magic_splits_i, std::vector<int>()));
    setup->read("magic-speeds", sbg.magic_speeds_i, pMerge(magic_speeds_i, std::vector<double>()));

    setup->read("animated", sbg.animated, pMerge(animated, false)); //animated

    setup->read("frames", sbg.frames, pMerge(frames, 1));
    NumberLimiter::apply(sbg.frames, 1u);

    setup->read("framespeed", sbg.framespeed, pMerge(framespeed, 128));
    NumberLimiter::apply(sbg.framespeed, 1u);

    setup->read("display-frame", sbg.display_frame, pMerge(display_frame, 0));
    NumberLimiter::apply(sbg.display_frame, 0u);

    //frames
    if(sbg.type == 1)
    {
        setup->read("second-image", sbg.second_image_n, pMerge(second_image_n, ""));
        setup->read("second-repeat-h", sbg.second_repeat_h, pMerge(second_repeat_h, 2.0));
        NumberLimiter::applyD(sbg.second_repeat_h, 1.0, 0.0);

        setup->readEnum("second-repeat-v",
                                    sbg.second_repeat_v,
                                    pMerge(second_repeat_v, 0u),
                                    {
                                        {"NR", 0},
                                        {"ZR", 1},
                                        {"RP", 2},
                                        {"RZ", 3}
        });

        setup->readEnum("second-attached",
                                    sbg.second_attached,
                                    pMerge(second_attached, 0),
                                    {
                                        {"overfirst", 0},
                                        {"bottom", 1},
                                        {"top", 2}
        });
    }
    setup->endGroup();

    return valid;
    #undef pMerge
}

bool ConfigManager::loadLevelBackG()
{
    unsigned int i;
    obj_BG sbg;
    unsigned long bg_total = 0;
    bool useDirectory = false;
    std::string bg_ini = config_dirSTD + "lvl_bkgrd.ini";
    std::string nestDir = "";

    if(!Files::fileExists(bg_ini))
    {
        addError("ERROR LOADING lvl_bkgrd.ini: file does not exist");
        return false;
    }

    IniProcessing bgset(bg_ini);
    lvl_bg_indexes.clear();//Clear old
    bgset.beginGroup("background2-main");
    bg_total =  bgset.value("total", 0).toULongLong();
    nestDir =   bgset.value("config-dir", "").toString();

    if(!nestDir.empty())
    {
        nestDir = config_dirSTD + nestDir;
        useDirectory = true;
    }

    bgset.endGroup();
    lvl_bg_indexes.allocateSlots(bg_total);

    for(i = 1; i <= bg_total; i++)
    {
        sbg.isInit = false;
        sbg.image = NULL;
        sbg.textureArrayId = 0;
        sbg.animator_ID = 0;
        sbg.second_isInit = false;
        sbg.second_image = NULL;
        sbg.second_textureArrayId = 0;
        sbg.second_animator_ID = 0;
        bool valid = false;

        if(useDirectory)
            valid = loadLevelBackground(sbg, "background2", nullptr, fmt::format("{0}/background2-{1}.ini", nestDir, i));
        else
            valid = loadLevelBackground(sbg, std::string("background2-{0}", i), 0, "", &bgset);

        if(valid)
        {
            sbg.id = i;
            lvl_bg_indexes.storeElement(sbg.id, sbg);
            //Load custom config if possible
            loadCustomConfig<obj_BG>(lvl_bg_indexes, i, Dir_BG, "background2", "background2", &loadLevelBackground);
        }

        if(bgset.lastError() != IniProcessing::ERR_OK)
            addError(fmt::format("ERROR LOADING lvl_bgrnd.ini N:{0} (background2-{1})", bgset.lastError(), i));
    }

    return true;
}
