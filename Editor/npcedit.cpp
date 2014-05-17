/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <QtWidgets>
#include "npcedit.h"
#include "ui_npcedit.h"
#include "file_formats/file_formats.h"

npcedit::npcedit(dataconfigs * configs, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::npcedit)
{
    pConfigs = configs;
    npc_id = 0;
    FileType = 2;
    isUntitled = true;
    isModyfied  = false;
    ui->setupUi(this);
}

npcedit::~npcedit()
{
    delete ui;
}


void npcedit::newFile(unsigned long npcID)
{
    npc_id = npcID;
    static int sequenceNumber = 1;
    ui->CurrentNPCID->setText( QString::number(npcID) );

    isUntitled = true;

    curFile = tr("npc-%1.txt").arg(npcID);

    setWindowTitle(curFile + QString(" [*] (%1)").arg(sequenceNumber++));

    setDefaultData(npcID);

    NpcData = DefaultNPCData; // create data templade
    StartNPCData = DefaultNPCData;
    setDataBoxes();

    documentWasModified();

    /*connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));*/
}

bool npcedit::loadFile(const QString &fileName, NPCConfigFile FileData)
{
    QFile file(fileName);
    NpcData = FileData;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Load file error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QFileInfo fileI(fileName);

    //Get NPC-ID from FileName
    QStringList tmp = fileI.baseName().split(QChar('-'));
    if(tmp.size()==2)
        if(!SMBX64::Int(tmp[1]))
        {
            setDefaultData( tmp[1].toInt() );
            ui->CurrentNPCID->setText( tmp[1] );
        }
        else
            setDefaultData(0);
    else
        setDefaultData(0);


    StartNPCData = NpcData; //Save current history for made reset
    setDataBoxes();


    setCurrentFile(fileName);
    documentNotModified();

    return true;
}

bool npcedit::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool npcedit::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
      curFile, tr("SMBX custom NPC config file (npc-*.txt)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool npcedit::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Write file error"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << WriteNPCTxtFile(NpcData);
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);

    documentNotModified();

    return true;
}



QString npcedit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void npcedit::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void npcedit::documentWasModified()
{
    QFont font;
    font.setWeight( QFont::Bold );
    isModyfied = true;
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
    ui->isModyfiedL->setText("Yes");
    ui->isModyfiedL->setFont( font );
}

void npcedit::documentNotModified()
{
    QFont font;
    font.setWeight( QFont::Normal );

    isModyfied = false;
    setWindowTitle(userFriendlyCurrentFile());
    ui->isModyfiedL->setText("No");
    ui->isModyfiedL->setFont( font );
}


bool npcedit::maybeSave()
{
    if (isModyfied) {
    QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, userFriendlyCurrentFile()+tr(" not saved"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
             | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }

    return true;
}

void npcedit::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    documentWasModified();
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString npcedit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


void npcedit::setDataBoxes()
{

    //Applay NPC data
    if(!StartNPCData.en_gfxoffsetx)
    {
        ui->en_GFXOffsetX->setChecked(false);
        ui->GFXOffSetX->setEnabled(false);
        ui->GFXOffSetX->setValue(DefaultNPCData.gfxoffsetx);
        NpcData.gfxoffsetx=DefaultNPCData.gfxoffsetx;
    }
    else
    {
        ui->en_GFXOffsetX->setChecked(true);
        ui->GFXOffSetX->setEnabled(true);
        ui->GFXOffSetX->setValue(StartNPCData.gfxoffsetx);
    }

    if(!StartNPCData.en_gfxoffsety)
    {
        ui->en_GFXOffsetY->setChecked(false);
        ui->GFXOffSetY->setEnabled(false);
        ui->GFXOffSetY->setValue(DefaultNPCData.gfxoffsety);
        NpcData.gfxoffsety=DefaultNPCData.gfxoffsety;
    }
    else
    {
        ui->en_GFXOffsetY->setChecked(true);
        ui->GFXOffSetY->setEnabled(true);
        ui->GFXOffSetY->setValue(StartNPCData.gfxoffsety);
    }

    if(!StartNPCData.en_foreground)
    {
        ui->En_IsForeground->setChecked(false);
        ui->IsForeground->setEnabled(false);
        ui->IsForeground->setChecked(DefaultNPCData.foreground);
        NpcData.foreground=DefaultNPCData.foreground;
    }
    else
    {
        ui->En_IsForeground->setChecked(true);
        ui->IsForeground->setEnabled(true);
        ui->IsForeground->setChecked(StartNPCData.foreground);
    }

    if(!StartNPCData.en_gfxheight)
    {
        ui->En_GFXh->setChecked(false);
        ui->GFXh->setEnabled(false);
        ui->GFXh->setValue(DefaultNPCData.gfxheight);
        NpcData.gfxheight=DefaultNPCData.gfxheight;
    }
    else
    {
        ui->En_GFXh->setChecked(true);
        ui->GFXh->setEnabled(true);
        ui->GFXh->setValue(StartNPCData.gfxheight);
    }

    if(!StartNPCData.en_gfxwidth)
    {
        ui->En_GFXw->setChecked(false);
        ui->GFXw->setEnabled(false);
        ui->GFXw->setValue(DefaultNPCData.gfxwidth);
        NpcData.gfxwidth=DefaultNPCData.gfxwidth;
    }
    else
    {
        ui->En_GFXw->setChecked(true);
        ui->GFXw->setEnabled(true);
        ui->GFXw->setValue(StartNPCData.gfxwidth);
    }

    if(!StartNPCData.en_framestyle)
    {
        ui->En_Framestyle->setChecked(false);
        ui->FrameStyle->setEnabled(false);
        ui->FrameStyle->setCurrentIndex(DefaultNPCData.framestyle);
        NpcData.framestyle=DefaultNPCData.framestyle;
    }
    else
    {
        ui->En_Framestyle->setChecked(true);
        ui->FrameStyle->setEnabled(true);
        ui->FrameStyle->setCurrentIndex(StartNPCData.framestyle);
    }

    if(!StartNPCData.en_framespeed)
    {
        ui->En_Framespeed->setChecked(false);
        ui->Framespeed->setEnabled(false);
        ui->Framespeed->setValue(DefaultNPCData.framespeed);
        NpcData.framespeed=DefaultNPCData.framespeed;
    }
    else
    {
        ui->En_Framespeed->setChecked(true);
        ui->Framespeed->setEnabled(true);
        ui->Framespeed->setValue(StartNPCData.framespeed);
    }

    if(!StartNPCData.en_frames)
    {
        ui->En_Frames->setChecked(false);
        ui->Frames->setEnabled(false);
        ui->Frames->setValue(DefaultNPCData.frames);
        NpcData.frames=DefaultNPCData.frames;
    }
    else
    {
        ui->En_Frames->setChecked(true);
        ui->Frames->setEnabled(true);
        ui->Frames->setValue(StartNPCData.frames);
    }

    if(!StartNPCData.en_score)
    {
        ui->En_Score->setChecked(false);
        ui->Score->setEnabled(false);
        ui->Score->setCurrentIndex(DefaultNPCData.score);
        NpcData.score=DefaultNPCData.score;
    }
    else
    {
        ui->En_Score->setChecked(true);
        ui->Score->setEnabled(true);
        ui->Score->setCurrentIndex(StartNPCData.score);
    }

    if(!StartNPCData.en_jumphurt)
    {
        ui->En_JumpHurt->setChecked(false);
        ui->JumpHurt->setEnabled(false);
        ui->JumpHurt->setChecked(DefaultNPCData.jumphurt);
        NpcData.jumphurt=DefaultNPCData.jumphurt;
    }
    else
    {
        ui->En_JumpHurt->setChecked(true);
        ui->JumpHurt->setEnabled(true);
        ui->JumpHurt->setChecked(StartNPCData.jumphurt);
    }

    if(!StartNPCData.en_grabtop)
    {
        ui->En_GrabTop->setChecked(false);
        ui->GrabTop->setEnabled(false);
        ui->GrabTop->setChecked(DefaultNPCData.grabtop);
        NpcData.grabtop=DefaultNPCData.grabtop;
    }
    else
    {
        ui->En_GrabTop->setChecked(true);
        ui->GrabTop->setEnabled(true);
        ui->GrabTop->setChecked(StartNPCData.grabtop);
    }

    if(!StartNPCData.en_grabside)
    {
        ui->En_GrabSide->setChecked(false);
        ui->GrabSide->setEnabled(false);
        ui->GrabSide->setChecked(DefaultNPCData.grabside);
        NpcData.grabside=DefaultNPCData.grabside;
    }
    else
    {
        ui->En_GrabSide->setChecked(true);
        ui->GrabSide->setEnabled(true);
        ui->GrabSide->setChecked(StartNPCData.grabside);
    }

    if(!StartNPCData.en_nofireball)
    {
        ui->En_NoFireball->setChecked(false);
        ui->NoFireball->setEnabled(false);
        ui->NoFireball->setChecked(DefaultNPCData.nofireball);
        NpcData.nofireball=DefaultNPCData.nofireball;
    }
    else
    {
        ui->En_NoFireball->setChecked(true);
        ui->NoFireball->setEnabled(true);
        ui->NoFireball->setChecked(StartNPCData.nofireball);
    }

    if(!StartNPCData.en_nohurt)
    {
        ui->En_DontHurt->setChecked(false);
        ui->DontHurt->setEnabled(false);
        ui->DontHurt->setChecked(DefaultNPCData.nohurt);
        NpcData.nohurt=DefaultNPCData.nohurt;
    }
    else
    {
        ui->En_DontHurt->setChecked(true);
        ui->DontHurt->setEnabled(true);
        ui->DontHurt->setChecked(StartNPCData.nohurt);
    }

    if(!StartNPCData.en_noyoshi)
    {
        ui->En_NoEat->setChecked(false);
        ui->NoEat->setEnabled(false);
        ui->NoEat->setChecked(DefaultNPCData.noyoshi);
        NpcData.noyoshi=DefaultNPCData.noyoshi;
    }
    else
    {
        ui->En_NoEat->setChecked(true);
        ui->NoEat->setEnabled(true);
        ui->NoEat->setChecked(StartNPCData.noyoshi);
    }

    if(!StartNPCData.en_noiceball)
    {
        ui->En_NoIceball->setChecked(false);
        ui->NoIceball->setEnabled(false);
        ui->NoIceball->setChecked(DefaultNPCData.noiceball);
        NpcData.noiceball=DefaultNPCData.noiceball;
    }
    else
    {
        ui->En_NoIceball->setChecked(true);
        ui->NoIceball->setEnabled(true);
        ui->NoIceball->setChecked(StartNPCData.noiceball);
    }

    if(!StartNPCData.en_height)
    {
        ui->En_Height->setChecked(false);
        ui->Height->setEnabled(false);
        ui->Height->setValue(DefaultNPCData.height);
        NpcData.height=DefaultNPCData.height;
    }
    else
    {
        ui->En_Height->setChecked(true);
        ui->Height->setEnabled(true);
        ui->Height->setValue(StartNPCData.height);
    }

    if(!StartNPCData.en_width)
    {
        ui->En_Width->setChecked(false);
        ui->Width->setEnabled(false);
        ui->Width->setValue(DefaultNPCData.width);
        NpcData.width=DefaultNPCData.width;
    }
    else
    {
        ui->En_Width->setChecked(true);
        ui->Width->setEnabled(true);
        ui->Width->setValue(StartNPCData.width);
    }

    if(!StartNPCData.en_npcblock)
    {
        ui->En_NPCBlock->setChecked(false);
        ui->NPCBlock->setEnabled(false);
        ui->NPCBlock->setChecked(DefaultNPCData.npcblock);
        NpcData.npcblock=DefaultNPCData.npcblock;
    }
    else
    {
        ui->En_NPCBlock->setChecked(true);
        ui->NPCBlock->setEnabled(true);
        ui->NPCBlock->setChecked(StartNPCData.npcblock);
    }

    if(!StartNPCData.en_npcblocktop)
    {
        ui->En_NPCBlockTop->setChecked(false);
        ui->NPCBlockTop->setEnabled(false);
        ui->NPCBlockTop->setChecked(DefaultNPCData.npcblocktop);
        NpcData.npcblocktop=DefaultNPCData.npcblocktop;
    }
    else
    {
        ui->En_NPCBlockTop->setChecked(true);
        ui->NPCBlockTop->setEnabled(true);
        ui->NPCBlockTop->setChecked(StartNPCData.npcblocktop);
    }

    if(!StartNPCData.en_speed)
    {
        ui->En_Speed->setChecked(false);
        ui->Speed->setEnabled(false);
        NpcData.speed=DefaultNPCData.speed;
        ui->Speed->setValue(DefaultNPCData.speed);
    }
    else
    {
        ui->En_Speed->setChecked(true);
        ui->Speed->setEnabled(true);
        ui->Speed->setValue(StartNPCData.speed);
    }

    if(!StartNPCData.en_playerblock)
    {
        ui->En_PlayerBlock->setChecked(false);
        ui->PlayerBlock->setEnabled(false);
        ui->PlayerBlock->setChecked(DefaultNPCData.playerblock);
        NpcData.playerblock=DefaultNPCData.playerblock;
    }
    else
    {
        ui->En_PlayerBlock->setChecked(true);
        ui->PlayerBlock->setEnabled(true);
        ui->PlayerBlock->setChecked(StartNPCData.playerblock);
    }

    if(!StartNPCData.en_playerblocktop)
    {
        ui->En_PlayerBlockTop->setChecked(false);
        ui->PlayerBlockTop->setEnabled(false);
        ui->PlayerBlockTop->setChecked(DefaultNPCData.playerblocktop);
        NpcData.playerblocktop=DefaultNPCData.playerblocktop;
    }
    else
    {
        ui->En_PlayerBlockTop->setChecked(true);
        ui->PlayerBlockTop->setEnabled(true);
        ui->PlayerBlockTop->setChecked(StartNPCData.playerblocktop);
    }

    if(!StartNPCData.en_noblockcollision)
    {
        ui->En_NoBlockCollision->setChecked(false);
        ui->NoBlockCollision->setEnabled(false);
        ui->NoBlockCollision->setChecked(DefaultNPCData.noblockcollision);
        NpcData.noblockcollision=DefaultNPCData.noblockcollision;
    }
    else
    {
        ui->En_NoBlockCollision->setChecked(true);
        ui->NoBlockCollision->setEnabled(true);
        ui->NoBlockCollision->setChecked(StartNPCData.noblockcollision);
    }

    if(!StartNPCData.en_nogravity)
    {
        ui->En_NoGravity->setChecked(false);
        ui->NoGravity->setEnabled(false);
        ui->NoGravity->setChecked(DefaultNPCData.nogravity);
        NpcData.nogravity=DefaultNPCData.nogravity;
    }
    else
    {
        ui->En_NoGravity->setChecked(true);
        ui->NoGravity->setEnabled(true);
        ui->NoGravity->setChecked(StartNPCData.nogravity);
    }

    if(!StartNPCData.en_cliffturn)
    {
        ui->En_TurnCliff->setChecked(false);
        ui->TurnCliff->setEnabled(false);
        ui->TurnCliff->setChecked(DefaultNPCData.cliffturn);
        NpcData.cliffturn=DefaultNPCData.cliffturn;
    }
    else
    {
        ui->En_TurnCliff->setChecked(true);
        ui->TurnCliff->setEnabled(true);
        ui->TurnCliff->setChecked(StartNPCData.cliffturn);
    }
    if(!StartNPCData.en_nohammer)
    {
        ui->En_NoHammer->setChecked(false);
        ui->NoHammer->setEnabled(false);
        ui->NoHammer->setChecked(DefaultNPCData.nohammer);
        NpcData.nohammer=DefaultNPCData.nohammer;
    }
    else
    {
        ui->En_NoHammer->setChecked(true);
        ui->NoHammer->setEnabled(true);
        ui->NoHammer->setChecked(StartNPCData.nohammer);
    }
}

void npcedit::setDefaultData(unsigned long npc_id)
{

    DefaultNPCData.en_gfxoffsetx=false;
    DefaultNPCData.en_gfxoffsety=false;
    DefaultNPCData.en_width=false;
    DefaultNPCData.en_height=false;
    DefaultNPCData.en_gfxwidth=false;
    DefaultNPCData.en_gfxheight=false;
    DefaultNPCData.en_score=false;
    DefaultNPCData.en_playerblock=false;
    DefaultNPCData.en_playerblocktop=false;
    DefaultNPCData.en_npcblock=false;
    DefaultNPCData.en_npcblocktop=false;
    DefaultNPCData.en_grabside=false;
    DefaultNPCData.en_grabtop=false;
    DefaultNPCData.en_jumphurt=false;
    DefaultNPCData.en_nohurt=false;
    DefaultNPCData.en_noblockcollision=false;
    DefaultNPCData.en_cliffturn=false;
    DefaultNPCData.en_noyoshi=false;
    DefaultNPCData.en_foreground=false;
    DefaultNPCData.en_speed=false;
    DefaultNPCData.en_nofireball=false;
    DefaultNPCData.en_nogravity=false;
    DefaultNPCData.en_frames=false;
    DefaultNPCData.en_framespeed=false;
    DefaultNPCData.en_framestyle=false;
    DefaultNPCData.en_noiceball=false;
    DefaultNPCData.en_nohammer=false;

    if((npc_id==0)||(npc_id > (unsigned long)pConfigs->main_npc.size()))
    {
        DefaultNPCData.gfxoffsetx=0;
        DefaultNPCData.gfxoffsety=0;
        DefaultNPCData.width=0;
        DefaultNPCData.height=0;
        DefaultNPCData.gfxwidth=0;
        DefaultNPCData.gfxheight=0;
        DefaultNPCData.score=0;
        DefaultNPCData.playerblock=0;
        DefaultNPCData.playerblocktop=0;
        DefaultNPCData.npcblock=0;
        DefaultNPCData.npcblocktop=0;
        DefaultNPCData.grabside=0;
        DefaultNPCData.grabtop=0;
        DefaultNPCData.jumphurt=0;
        DefaultNPCData.nohurt=0;
        DefaultNPCData.noblockcollision=0;
        DefaultNPCData.cliffturn=0;
        DefaultNPCData.noyoshi=0;
        DefaultNPCData.foreground=0;
        DefaultNPCData.speed=1;
        DefaultNPCData.nofireball=0;
        DefaultNPCData.nogravity=0;
        DefaultNPCData.frames=0;
        DefaultNPCData.framespeed=8;
        DefaultNPCData.framestyle=0;
        DefaultNPCData.noiceball=0;
        DefaultNPCData.nohammer=0;
    }
    else
    {
        bool found=false;
        int j;

        //Check Index exists
        if(npc_id < (unsigned int)pConfigs->index_npc.size())
        {
            j = pConfigs->index_npc[npc_id].i;
            if(pConfigs->main_npc[j].id == npc_id)
                found=true;
        }

        //if Index found
        if(!found)
        {
            for(j=0;j<pConfigs->main_npc.size();j++)
            {
                if(pConfigs->main_npc[j].id==npc_id)
                {
                   break;//Found!
                }
            }
        }

        DefaultNPCData.gfxoffsetx=pConfigs->main_npc[j].gfx_offset_x;
        DefaultNPCData.gfxoffsety=pConfigs->main_npc[j].gfx_offset_y;
        DefaultNPCData.width=pConfigs->main_npc[j].width;
        DefaultNPCData.height=pConfigs->main_npc[j].height;
        DefaultNPCData.gfxwidth=pConfigs->main_npc[j].gfx_w;
        DefaultNPCData.gfxheight=pConfigs->main_npc[j].gfx_h;
        DefaultNPCData.score=pConfigs->main_npc[j].score;
        DefaultNPCData.playerblock=(int)pConfigs->main_npc[j].block_player;
        DefaultNPCData.playerblocktop=(int)pConfigs->main_npc[j].block_player_top;
        DefaultNPCData.npcblock=(int)pConfigs->main_npc[j].block_npc;
        DefaultNPCData.npcblocktop=(int)pConfigs->main_npc[j].block_player_top;
        DefaultNPCData.grabside=(int)pConfigs->main_npc[j].grab_side;
        DefaultNPCData.grabtop=(int)pConfigs->main_npc[j].grab_top;
        DefaultNPCData.jumphurt=(int)(
                    (pConfigs->main_npc[j].hurt_player)
                                      &&
                                      (!pConfigs->main_npc[j].kill_on_jump));
        DefaultNPCData.nohurt=(int)(!pConfigs->main_npc[j].hurt_player);
        DefaultNPCData.noblockcollision=(int)(!pConfigs->main_npc[j].collision_with_blocks);
        DefaultNPCData.cliffturn=(int)pConfigs->main_npc[j].turn_on_cliff_detect;
        DefaultNPCData.noyoshi=(int)(!pConfigs->main_npc[j].can_be_eaten);
        DefaultNPCData.foreground=(int)pConfigs->main_npc[j].foreground;
        DefaultNPCData.speed=1;
        DefaultNPCData.nofireball=(int)(!pConfigs->main_npc[j].kill_by_fireball);
        DefaultNPCData.nogravity=(int)(!pConfigs->main_npc[j].gravity);
        DefaultNPCData.frames=pConfigs->main_npc[j].frames;
        DefaultNPCData.framespeed=8;
        DefaultNPCData.framestyle=(int)(!pConfigs->main_npc[j].framestyle);
        DefaultNPCData.noiceball=(int)(!pConfigs->main_npc[j].freeze_by_iceball);
        DefaultNPCData.nohammer=(int)(!pConfigs->main_npc[j].kill_hammer);
    }
}






////////////////////////Slots///////////////////////////////


void npcedit::on_ResetNPCData_clicked()
{
    NpcData = StartNPCData; //Restore first version
    setDataBoxes();
    documentNotModified();
}


void npcedit::on_en_GFXOffsetX_clicked()
{
    if(ui->en_GFXOffsetX->isChecked())
    {
        ui->GFXOffSetX->setEnabled(true);
        NpcData.en_gfxoffsetx=true;
    }
    else
    {
        ui->GFXOffSetX->setEnabled(false);
        NpcData.en_gfxoffsetx=false;
    }
    documentWasModified();
}

void npcedit::on_en_GFXOffsetY_clicked()
{
    if(ui->en_GFXOffsetY->isChecked())
    {
        ui->GFXOffSetY->setEnabled(true);
        NpcData.en_gfxoffsety=true;
    }
    else
    {
        ui->GFXOffSetY->setEnabled(false);
        NpcData.en_gfxoffsety=false;
    }
    documentWasModified();
}

void npcedit::on_En_GFXw_clicked()
{
    if(ui->En_GFXw->isChecked())
    {
        ui->GFXw->setEnabled(true);
        NpcData.en_gfxwidth=true;
    }
    else
    {
        ui->GFXw->setEnabled(false);
        NpcData.en_gfxwidth=false;
    }
    documentWasModified();
}

void npcedit::on_En_GFXh_clicked()
{
    if(ui->En_GFXh->isChecked())
    {
        ui->GFXh->setEnabled(true);
        NpcData.en_gfxheight=true;
    }
    else
    {
        ui->GFXh->setEnabled(false);
        NpcData.en_gfxheight=false;
    }
    documentWasModified();
}

void npcedit::on_En_Frames_clicked()
{
    if(ui->En_Frames->isChecked())
    {
        ui->Frames->setEnabled(true);
        NpcData.en_frames=true;
    }
    else
    {
        ui->Frames->setEnabled(false);
        NpcData.en_frames=false;
    }
    documentWasModified();
}

void npcedit::on_En_Framespeed_clicked()
{
    if(ui->En_Framespeed->isChecked())
    {
        ui->Framespeed->setEnabled(true);
        NpcData.en_framespeed=true;
    }
    else
    {
        ui->Framespeed->setEnabled(false);
        NpcData.en_framespeed=false;
    }
    documentWasModified();
}

void npcedit::on_En_Framestyle_clicked()
{
    if(ui->En_Framestyle->isChecked())
    {
        ui->FrameStyle->setEnabled(true);
        NpcData.en_framestyle=true;
    }
    else
    {
        ui->FrameStyle->setEnabled(false);
        NpcData.en_framestyle=false;
    }
    documentWasModified();
}

void npcedit::on_En_IsForeground_clicked()
{
    if(ui->En_IsForeground->isChecked())
    {
        ui->IsForeground->setEnabled(true);
        NpcData.en_foreground=true;
    }
    else
    {
        ui->IsForeground->setEnabled(false);
        NpcData.en_foreground=false;
    }
    documentWasModified();
}

void npcedit::on_En_GrabSide_clicked()
{
    if(ui->En_GrabSide->isChecked())
    {
        ui->GrabSide->setEnabled(true);
        NpcData.en_grabside=true;
    }
    else
    {
        ui->GrabSide->setEnabled(false);
        NpcData.en_grabside=false;
    }
    documentWasModified();
}

void npcedit::on_En_GrabTop_clicked()
{
    if(ui->En_GrabTop->isChecked())
    {
        ui->GrabTop->setEnabled(true);
        NpcData.en_grabtop=true;
    }
    else
    {
        ui->GrabTop->setEnabled(false);
        NpcData.en_grabtop=false;
    }
    documentWasModified();
}

void npcedit::on_En_JumpHurt_clicked()
{
    if(ui->En_JumpHurt->isChecked())
    {
        ui->JumpHurt->setEnabled(true);
        NpcData.en_jumphurt=true;
    }
    else
    {
        ui->JumpHurt->setEnabled(false);
        NpcData.en_jumphurt=false;
    }
    documentWasModified();
}

void npcedit::on_En_DontHurt_clicked()
{
    if(ui->En_DontHurt->isChecked())
    {
        ui->DontHurt->setEnabled(true);
        NpcData.en_nohurt=true;
    }
    else
    {
        ui->DontHurt->setEnabled(false);
        NpcData.en_nohurt=false;
    }
    documentWasModified();
}

void npcedit::on_En_Score_clicked()
{
    if(ui->En_Score->isChecked())
    {
        ui->Score->setEnabled(true);
        NpcData.en_score=true;
    }
    else
    {
        ui->Score->setEnabled(false);
        NpcData.en_score=false;
    }
    documentWasModified();
}

void npcedit::on_En_NoEat_clicked()
{
    if(ui->En_NoEat->isChecked())
    {
        ui->NoEat->setEnabled(true);
        NpcData.en_noyoshi=true;
    }
    else
    {
        ui->NoEat->setEnabled(false);
        NpcData.en_noyoshi=false;
    }
    documentWasModified();
}

void npcedit::on_En_NoFireball_clicked()
{
    if(ui->En_NoFireball->isChecked())
    {
        ui->NoFireball->setEnabled(true);
        NpcData.en_nofireball=true;
    }
    else
    {
        ui->NoFireball->setEnabled(false);
        NpcData.en_nofireball=false;
    }
    documentWasModified();
}

void npcedit::on_En_NoIceball_clicked()
{
    if(ui->En_NoIceball->isChecked())
    {
        ui->NoIceball->setEnabled(true);
        NpcData.en_noiceball=true;
    }
    else
    {
        ui->NoIceball->setEnabled(false);
        NpcData.en_noiceball=false;
    }
    documentWasModified();
}

void npcedit::on_En_Width_clicked()
{
    if(ui->En_Width->isChecked())
    {
        ui->Width->setEnabled(true);
        NpcData.en_width=true;
    }
    else
    {
        ui->Width->setEnabled(false);
        NpcData.en_width=false;
    }
    documentWasModified();
}

void npcedit::on_En_Height_clicked()
{
    if(ui->En_Height->isChecked())
    {
        ui->Height->setEnabled(true);
        NpcData.en_height=true;
    }
    else
    {
        ui->Height->setEnabled(false);
        NpcData.en_height=false;
    }
    documentWasModified();
}

void npcedit::on_En_Speed_clicked()
{
    if(ui->En_Speed->isChecked())
    {
        ui->Speed->setEnabled(true);
        NpcData.en_speed=true;
    }
    else
    {
        ui->Speed->setEnabled(false);
        NpcData.en_speed=false;
    }
    documentWasModified();
}

void npcedit::on_En_PlayerBlock_clicked()
{
    if(ui->En_PlayerBlock->isChecked())
    {
        ui->PlayerBlock->setEnabled(true);
        NpcData.en_playerblock=true;
    }
    else
    {
        ui->PlayerBlock->setEnabled(false);
        NpcData.en_playerblock=false;
    }
    documentWasModified();
}

void npcedit::on_En_PlayerBlockTop_clicked()
{
    if(ui->En_PlayerBlockTop->isChecked())
    {
        ui->PlayerBlockTop->setEnabled(true);
        NpcData.en_playerblocktop=true;
    }
    else
    {
        ui->PlayerBlockTop->setEnabled(false);
        NpcData.en_playerblocktop=false;
    }
    documentWasModified();
}

void npcedit::on_En_NPCBlock_clicked()
{
    if(ui->En_NPCBlock->isChecked())
    {
        ui->NPCBlock->setEnabled(true);
        NpcData.en_npcblock=true;
    }
    else
    {
        ui->NPCBlock->setEnabled(false);
        NpcData.en_npcblock=false;
    }
    documentWasModified();
}

void npcedit::on_En_NPCBlockTop_clicked()
{
    if(ui->En_NPCBlockTop->isChecked())
    {
        ui->NPCBlockTop->setEnabled(true);
        NpcData.en_npcblocktop=true;
    }
    else
    {
        ui->NPCBlockTop->setEnabled(false);
        NpcData.en_npcblocktop=false;
    }
    documentWasModified();
}

void npcedit::on_En_NoBlockCollision_clicked()
{
    if(ui->En_NoBlockCollision->isChecked())
    {
        ui->NoBlockCollision->setEnabled(true);
        NpcData.en_noblockcollision=true;
    }
    else
    {
        ui->NoBlockCollision->setEnabled(false);
        NpcData.en_noblockcollision=false;
    }
    documentWasModified();
}

void npcedit::on_En_NoGravity_clicked()
{
    if(ui->En_NoGravity->isChecked())
    {
        ui->NoGravity->setEnabled(true);
        NpcData.en_nogravity=true;
    }
    else
    {
        ui->NoGravity->setEnabled(false);
        NpcData.en_nogravity=false;
    }
    documentWasModified();
}

void npcedit::on_En_TurnCliff_clicked()
{
    if(ui->En_TurnCliff->isChecked())
    {
        ui->TurnCliff->setEnabled(true);
        NpcData.en_cliffturn=true;
    }
    else
    {
        ui->TurnCliff->setEnabled(false);
        NpcData.en_cliffturn=false;
    }
    documentWasModified();
}


void npcedit::on_En_NoHammer_clicked()
{
    if(ui->En_NoHammer->isChecked())
    {
        ui->NoHammer->setEnabled(true);
        NpcData.en_nohammer=true;
    }
    else
    {
        ui->NoHammer->setEnabled(false);
        NpcData.en_nohammer=false;
    }
    documentWasModified();
}








//Changed boxes
void npcedit::on_GFXOffSetX_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.gfxoffsetx = arg1;
}

void npcedit::on_GFXOffSetY_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.gfxoffsety = arg1;
}

void npcedit::on_GFXw_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.gfxwidth = arg1;
}

void npcedit::on_GFXh_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.gfxheight = arg1;
}

void npcedit::on_Frames_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.frames = arg1;
}

void npcedit::on_Framespeed_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.framespeed=arg1;
}

void npcedit::on_FrameStyle_currentIndexChanged(int index)
{
    documentWasModified();
    NpcData.framestyle=index;
}

void npcedit::on_IsForeground_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.foreground=arg1;
}

void npcedit::on_GrabSide_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.grabside=arg1;
}

void npcedit::on_GrabTop_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.grabtop=arg1;
}

void npcedit::on_JumpHurt_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.jumphurt=arg1;
}

void npcedit::on_DontHurt_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.nohurt=arg1;
}

void npcedit::on_Score_currentIndexChanged(int index)
{
    documentWasModified();
    NpcData.score=index;
}

void npcedit::on_NoEat_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.noyoshi=arg1;
}

void npcedit::on_NoFireball_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.nofireball=arg1;
}

void npcedit::on_NoIceball_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.noiceball=arg1;
}

void npcedit::on_Width_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.width=arg1;
}

void npcedit::on_Height_valueChanged(int arg1)
{
    documentWasModified();
    NpcData.height=arg1;
}

void npcedit::on_Speed_valueChanged(double arg1)
{
    documentWasModified();
    NpcData.speed = arg1;
}

void npcedit::on_PlayerBlock_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.playerblock=arg1;
}

void npcedit::on_PlayerBlockTop_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.playerblocktop=arg1;
}

void npcedit::on_NPCBlock_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.npcblock=arg1;
}

void npcedit::on_NPCBlockTop_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.npcblocktop=arg1;
}

void npcedit::on_NoBlockCollision_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.noblockcollision=arg1;
}

void npcedit::on_NoGravity_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.nogravity=arg1;
}

void npcedit::on_TurnCliff_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.cliffturn=arg1;
}

void npcedit::on_NoHammer_stateChanged(int arg1)
{
    documentWasModified();
    NpcData.nohammer=arg1;
}
