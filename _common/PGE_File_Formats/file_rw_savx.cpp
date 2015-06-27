/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "file_formats.h"
#include "file_strlist.h"
#include "pge_x.h"

#ifdef PGE_FILES_QT
#include <QDir>
#include <QFileInfo>
#ifdef PGE_FILES_USE_MESSAGEBOXES
#include <QMessageBox>
#endif
#endif

GamesaveData FileFormats::ReadExtendedSaveFile(PGESTRING RawData, PGESTRING filePath, bool sielent)
{
    GamesaveData FileData = dummySaveDataArray();
    PGEFile pgeX_Data( RawData );
    PGESTRING line;

    saveCharacterState plr_state;
    visibleItem        vz_item;
    starOnLevel        star_level;

    int str_count=0;

    //Add path data
    if(!filePath.isEmpty())
    {
        QFileInfo in_1(filePath);
        FileData.filename = in_1.baseName();
        FileData.path = in_1.absoluteDir().absolutePath();
    }

    FileData.characterStates.clear();
    FileData.currentCharacter.clear();

    FileData.untitled = false;
    FileData.modified = false;

    ///////////////////////////////////////Begin file///////////////////////////////////////
    if( !pgeX_Data.buildTreeFromRaw() )
    {
        errorString = pgeX_Data.lastError();
        goto badfile;
    }


    for(int section=0; section<pgeX_Data.dataTree.size(); section++) //look sections
    {
        PGEFile::PGEX_Entry &f_section = pgeX_Data.dataTree[section];
        ///////////////////JOKES//////////////////////
        if(f_section.name=="JOKES")
        {
            #ifdef PGE_FILES_USE_MESSAGEBOXES
            if((!silentMode)&&(!f_section.data.isEmpty()))
                if(!f_section.data[0].values.isEmpty())
                    QMessageBox::information(nullptr, "Jokes",
                            f_section.data[0].values[0].value,
                            QMessageBox::Ok);
            #endif
        }//jokes
        ///////////////////HEADER//////////////////////
        else
        if(f_section.name=="SAVE_HEADER")
        {
            if(f_section.type!=PGEFile::PGEX_Struct)
            {
                errorString=PGESTRING("Wrong section data syntax:\nSection [%1]").arg(f_section.name);
                goto badfile;
            }

            for(int sdata=0;sdata<f_section.data.size();sdata++)
            {
                if(f_section.data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=PGESTRING("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(f_section.name)
                            .arg(sdata);
                    goto badfile;
                }

                PGEFile::PGEX_Item x = f_section.data[sdata];
                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=PGESTRING("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(f_section.name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="LV")
                    {
                        if(PGEFile::IsIntU(v.value))
                            FileData.lives= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="CN")
                    {
                        if(PGEFile::IsIntU(v.value))
                            FileData.coins= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="PT")
                    {
                        if(PGEFile::IsIntU(v.value))
                            FileData.points= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="TS")
                    {
                        if(PGEFile::IsIntU(v.value))
                            FileData.totalStars= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="WX")
                    {
                        if(PGEFile::IsIntS(v.value))
                            FileData.worldPosX= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="WY")
                    {
                        if(PGEFile::IsIntS(v.value))
                            FileData.worldPosY= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="HW")
                    {
                        if(PGEFile::IsIntS(v.value))
                            FileData.last_hub_warp= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MI")
                    {
                        if(PGEFile::IsIntU(v.value))
                            FileData.musicID= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MF")
                    {
                        if(PGEFile::IsQStr(v.value))
                            FileData.musicFile= PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="GC")
                    {
                        if(PGEFile::IsBool(v.value))
                            FileData.gameCompleted= (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                }
            }
        }//Header
        else ///////////////////CHARACTERS//////////////////////
        if(f_section.name=="CHARACTERS")
        {
            if(f_section.type!=PGEFile::PGEX_Struct)
            {
                errorString=PGESTRING("Wrong section data syntax:\nSection [%1]").arg(f_section.name);
                goto badfile;
            }

            for(int sdata=0;sdata<f_section.data.size();sdata++)
            {
                if(f_section.data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=PGESTRING("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(f_section.name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = f_section.data[sdata];

                plr_state = dummySavCharacterState();
                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=PGESTRING("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(f_section.name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID")
                    {
                        if(PGEFile::IsIntU(v.value))
                            plr_state.id = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="ST")
                    {
                        if(PGEFile::IsIntU(v.value))
                            plr_state.state = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="IT")
                    {
                        if(PGEFile::IsIntU(v.value))
                            plr_state.itemID = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MT")
                    {
                        if(PGEFile::IsIntU(v.value))
                            plr_state.mountType = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MI")
                    {
                        if(PGEFile::IsIntU(v.value))
                            plr_state.mountID= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="HL")
                    {
                        if(PGEFile::IsIntU(v.value))
                            plr_state.health= v.value.toInt();
                        else
                            goto badfile;
                    }
                }
                FileData.characterStates.push_back(plr_state);
            }
        }//CHARACTERS
        else ///////////////////CHARACTERS_PER_PLAYERS//////////////////////
        if(f_section.name=="CHARACTERS_PER_PLAYERS")
        {
            if(f_section.type!=PGEFile::PGEX_Struct)
            {
                errorString=PGESTRING("Wrong section data syntax:\nSection [%1]").arg(f_section.name);
                goto badfile;
            }

            for(int sdata=0;sdata<f_section.data.size();sdata++)
            {
                if(f_section.data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=PGESTRING("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(f_section.name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = f_section.data[sdata];

                int character=0;
                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=PGESTRING("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(f_section.name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID")
                    {
                        if(PGEFile::IsIntU(v.value))
                            character = v.value.toInt();
                        else
                            goto badfile;
                    }
                }
                FileData.currentCharacter.push_back(character);
            }
        }//CHARACTERS_PER_PLAYERS
        else ///////////////////VIZ_LEVELS//////////////////////
        if(f_section.name=="VIZ_LEVELS")
        {
            if(f_section.type!=PGEFile::PGEX_Struct)
            {
                errorString=PGESTRING("Wrong section data syntax:\nSection [%1]").arg(f_section.name);
                goto badfile;
            }

            for(int sdata=0;sdata<f_section.data.size();sdata++)
            {
                if(f_section.data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=PGESTRING("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(f_section.name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = f_section.data[sdata];

                vz_item.first=0;
                vz_item.second=false;
                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=PGESTRING("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(f_section.name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID")
                    {
                        if(PGEFile::IsIntU(v.value))
                            vz_item.first = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="V")
                    {
                        if(PGEFile::IsBool(v.value))
                            vz_item.second = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                }
                FileData.visibleLevels.push_back(vz_item);
            }
        }//VIZ_LEVELS
        else ///////////////////VIZ_PATHS//////////////////////
        if(f_section.name=="VIZ_PATHS")
        {
            if(f_section.type!=PGEFile::PGEX_Struct)
            {
                errorString=PGESTRING("Wrong section data syntax:\nSection [%1]").arg(f_section.name);
                goto badfile;
            }

            for(int sdata=0;sdata<f_section.data.size();sdata++)
            {
                if(f_section.data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=PGESTRING("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(f_section.name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = f_section.data[sdata];

                vz_item.first=0;
                vz_item.second=false;
                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=PGESTRING("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(f_section.name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID")
                    {
                        if(PGEFile::IsIntU(v.value))
                            vz_item.first = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="V")
                    {
                        if(PGEFile::IsBool(v.value))
                            vz_item.second = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                }
                FileData.visiblePaths.push_back(vz_item);
            }
        }//VIZ_PATHS
        else ///////////////////VIZ_SCENERY//////////////////////
        if(f_section.name=="VIZ_SCENERY")
        {
            if(f_section.type!=PGEFile::PGEX_Struct)
            {
                errorString=PGESTRING("Wrong section data syntax:\nSection [%1]").arg(f_section.name);
                goto badfile;
            }

            for(int sdata=0;sdata<f_section.data.size();sdata++)
            {
                if(f_section.data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=PGESTRING("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(f_section.name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = f_section.data[sdata];

                vz_item.first=0;
                vz_item.second=false;
                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=PGESTRING("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(f_section.name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID")
                    {
                        if(PGEFile::IsIntU(v.value))
                            vz_item.first = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="V")
                    {
                        if(PGEFile::IsBool(v.value))
                            vz_item.second = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                }
                FileData.visibleScenery.push_back(vz_item);
            }
        }//VIZ_SCENERY
        else ///////////////////STARS//////////////////////
        if(f_section.name=="STARS")
        {
            if(f_section.type!=PGEFile::PGEX_Struct)
            {
                errorString=PGESTRING("Wrong section data syntax:\nSection [%1]").arg(f_section.name);
                goto badfile;
            }

            for(int sdata=0;sdata<f_section.data.size();sdata++)
            {
                if(f_section.data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=PGESTRING("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(f_section.name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = f_section.data[sdata];

                star_level.first="";
                star_level.second=0;
                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=PGESTRING("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(f_section.name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="L")
                    {
                        if(PGEFile::IsQStr(v.value))
                            star_level.first = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="S")
                    {
                        if(PGEFile::IsIntU(v.value))
                            star_level.second = v.value.toInt();
                        else
                            goto badfile;
                    }
                }
                FileData.gottenStars.push_back(vz_item);
            }
        }//STARS
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////
    errorString.clear(); //If no errors, clear string;
    FileData.ReadFileValid=true;

    return FileData;

    badfile:    //If file format not corrects
    if(!sielent)
       BadFileMsg(FileData.path+"/"+FileData.filename+"\nError message: "+errorString, str_count, line);
        FileData.ReadFileValid=false;
    return FileData;
}



PGESTRING FileFormats::WriteExtendedSaveFile(GamesaveData &FileData)
{
    PGESTRING TextData;
    long i;

    TextData += "SAVE_HEADER\n";
    TextData += PGEFile::value("LV", PGEFile::IntS(FileData.lives));
    TextData += PGEFile::value("CN", PGEFile::IntS(FileData.coins));
    TextData += PGEFile::value("PT", PGEFile::IntS(FileData.points));
    TextData += PGEFile::value("TS", PGEFile::IntS(FileData.totalStars));
    TextData += PGEFile::value("WX", PGEFile::IntS(FileData.worldPosX));
    TextData += PGEFile::value("WY", PGEFile::IntS(FileData.worldPosY));
    TextData += PGEFile::value("HW", PGEFile::IntS(FileData.last_hub_warp));
    TextData += PGEFile::value("MI", PGEFile::IntS(FileData.musicID));
    TextData += PGEFile::value("MF", PGEFile::qStrS(FileData.musicFile));
    TextData += PGEFile::value("GC", PGEFile::BoolS(FileData.gameCompleted));
    TextData += "\n";
    TextData += "SAVE_HEADER_END\n";

    if(!FileData.characterStates.isEmpty())
    {
        TextData += "CHARACTERS\n";
        for(i=0;i< FileData.characterStates.size(); i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.characterStates[i].id));
            TextData += PGEFile::value("ST", PGEFile::IntS(FileData.characterStates[i].state));
            TextData += PGEFile::value("IT", PGEFile::IntS(FileData.characterStates[i].itemID));
            TextData += PGEFile::value("MT", PGEFile::IntS(FileData.characterStates[i].mountType));
            TextData += PGEFile::value("MI", PGEFile::IntS(FileData.characterStates[i].mountID));
            TextData += PGEFile::value("HL", PGEFile::IntS(FileData.characterStates[i].health));
            TextData += "\n";
        }
        TextData += "CHARACTERS_END\n";
    }

    if(!FileData.currentCharacter.isEmpty())
    {
        TextData += "CHARACTERS_PER_PLAYERS\n";
        for(i=0;i< FileData.currentCharacter.size(); i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.currentCharacter[i]));
            TextData += "\n";
        }
        TextData += "CHARACTERS_PER_PLAYERS_END\n";
    }

    if(!FileData.visibleLevels.isEmpty())
    {
        TextData += "VIZ_LEVELS\n";
        for(i=0;i< FileData.visibleLevels.size(); i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.visibleLevels[i].first));
            TextData += PGEFile::value("V", PGEFile::BoolS(FileData.visibleLevels[i].second));
            TextData += "\n";
        }
        TextData += "VIZ_LEVELS_END\n";
    }

    if(!FileData.visiblePaths.isEmpty())
    {
        TextData += "VIZ_PATHS\n";
        for(i=0;i< FileData.visiblePaths.size(); i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.visiblePaths[i].first));
            TextData += PGEFile::value("V", PGEFile::BoolS(FileData.visiblePaths[i].second));
            TextData += "\n";
        }
        TextData += "VIZ_PATHS_END\n";
    }

    if(!FileData.visibleScenery.isEmpty())
    {
        TextData += "VIZ_SCENERY\n";
        for(i=0;i< FileData.visibleScenery.size(); i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.visibleScenery[i].first));
            TextData += PGEFile::value("V", PGEFile::BoolS(FileData.visibleScenery[i].second));
            TextData += "\n";
        }
        TextData += "VIZ_SCENERY_END\n";
    }

    if(!FileData.gottenStars.isEmpty())
    {
        TextData += "STARS\n";
        for(i=0;i< FileData.gottenStars.size(); i++)
        {
            TextData += PGEFile::value("L", PGEFile::qStrS(FileData.gottenStars[i].first));
            TextData += PGEFile::value("S", PGEFile::IntS(FileData.gottenStars[i].second));
            TextData += "\n";
        }
        TextData += "STARS_END\n";
    }

    TextData += "\n";
    return TextData;
}

