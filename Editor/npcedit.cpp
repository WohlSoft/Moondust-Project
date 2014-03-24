#include <QtGui>
#include "npcedit.h"
#include "ui_npcedit.h"

npcedit::npcedit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::npcedit)
{
    FileType = 2;
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    //setFixedSize( sizeHint() );
    setWindowFlags(Qt::Tool | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    ui->setupUi(this);
}

npcedit::~npcedit()
{
    delete ui;
}


void npcedit::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("npc-%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    /*connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));*/
}

void npcedit::setDataBoxes()
{

    //Applay NPC data

    if(!NpcData.en_gfxoffsetx)
    {
        ui->en_GFXOffsetX->setChecked(false);
        ui->GFXOffSetX->setEnabled(false);
    }
    else
    {
        ui->en_GFXOffsetX->setChecked(true);
        ui->GFXOffSetX->setEnabled(true);
        ui->GFXOffSetX->setValue(NpcData.gfxoffsetx);
    }

    if(!NpcData.en_gfxoffsety)
    {
        ui->en_GFXOffsetY->setChecked(false);
        ui->GFXOffSetY->setEnabled(false);
    }
    else
    {
        ui->en_GFXOffsetY->setChecked(true);
        ui->GFXOffSetY->setEnabled(true);
        ui->GFXOffSetY->setValue(NpcData.gfxoffsetx);
    }

    if(!NpcData.en_foreground)
    {
        ui->En_IsForeground->setChecked(false);
        ui->IsForeground->setEnabled(false);
    }
    else
    {
        ui->En_IsForeground->setChecked(true);
        ui->IsForeground->setEnabled(true);
        ui->IsForeground->setChecked(NpcData.foreground);
    }

    if(!NpcData.en_gfxheight)
    {
        ui->En_GFXh->setChecked(false);
        ui->GFXh->setEnabled(false);
    }
    else
    {
        ui->En_GFXh->setChecked(true);
        ui->GFXh->setEnabled(true);
        ui->GFXh->setValue(NpcData.gfxheight);
    }

    if(!NpcData.en_gfxwidth)
    {
        ui->En_GFXw->setChecked(false);
        ui->GFXw->setEnabled(false);
    }
    else
    {
        ui->En_GFXw->setChecked(true);
        ui->GFXw->setEnabled(true);
        ui->GFXw->setValue(NpcData.gfxwidth);
    }

    if(!NpcData.en_framestyle)
    {
        ui->En_Framestyle->setChecked(false);
        ui->FrameStyle->setEnabled(false);
    }
    else
    {
        ui->En_Framestyle->setChecked(true);
        ui->FrameStyle->setEnabled(true);
        ui->FrameStyle->setCurrentIndex(NpcData.framestyle);
    }

    if(!NpcData.en_framespeed)
    {
        ui->En_Framespeed->setChecked(false);
        ui->Framespeed->setEnabled(false);
    }
    else
    {
        ui->En_Framespeed->setChecked(true);
        ui->Framespeed->setEnabled(true);
        ui->Framespeed->setValue(NpcData.framespeed);
    }

    if(!NpcData.en_frames)
    {
        ui->En_Frames->setChecked(false);
        ui->Frames->setEnabled(false);
    }
    else
    {
        ui->En_Frames->setChecked(true);
        ui->Frames->setEnabled(true);
        ui->Frames->setValue(NpcData.frames);
    }

    if(!NpcData.en_score)
    {
        ui->En_Score->setChecked(false);
        ui->Score->setEnabled(false);
    }
    else
    {
        ui->En_Score->setChecked(true);
        ui->Score->setEnabled(true);
        ui->Score->setCurrentIndex(NpcData.score);
    }

    if(!NpcData.en_jumphurt)
    {
        ui->En_JumpHurt->setChecked(false);
        ui->JumpHurt->setEnabled(false);
    }
    else
    {
        ui->En_JumpHurt->setChecked(true);
        ui->JumpHurt->setEnabled(true);
        ui->JumpHurt->setChecked(NpcData.jumphurt);
    }

    if(!NpcData.en_grabtop)
    {
        ui->En_GrabTop->setChecked(false);
        ui->GrabTop->setEnabled(false);
    }
    else
    {
        ui->En_GrabTop->setChecked(true);
        ui->GrabTop->setEnabled(true);
        ui->GrabTop->setChecked(NpcData.grabtop);
    }

    if(!NpcData.en_grabside)
    {
        ui->En_GrabSide->setChecked(false);
        ui->GrabSide->setEnabled(false);
    }
    else
    {
        ui->En_GrabSide->setChecked(true);
        ui->GrabSide->setEnabled(true);
        ui->GrabSide->setChecked(NpcData.grabside);
    }

    if(!NpcData.en_nofireball)
    {
        ui->En_NoFireball->setChecked(false);
        ui->NoFireball->setEnabled(false);
    }
    else
    {
        ui->En_NoFireball->setChecked(true);
        ui->NoFireball->setEnabled(true);
        ui->NoFireball->setChecked(NpcData.nofireball);
    }

    if(!NpcData.en_nohurt)
    {
        ui->En_DontHurt->setChecked(false);
        ui->DontHurt->setEnabled(false);
    }
    else
    {
        ui->En_DontHurt->setChecked(true);
        ui->DontHurt->setEnabled(true);
        ui->DontHurt->setChecked(NpcData.nohurt);
    }

    if(!NpcData.en_noyoshi)
    {
        ui->En_NoEat->setChecked(false);
        ui->NoEat->setEnabled(false);
    }
    else
    {
        ui->En_NoEat->setChecked(true);
        ui->NoEat->setEnabled(true);
        ui->NoEat->setChecked(NpcData.noyoshi);
    }

    if(!NpcData.en_noiceball)
    {
        ui->En_NoIceball->setChecked(false);
        ui->NoIceball->setEnabled(false);
    }
    else
    {
        ui->En_NoIceball->setChecked(true);
        ui->NoIceball->setEnabled(true);
        ui->NoIceball->setChecked(NpcData.noiceball);
    }

    if(!NpcData.en_height)
    {
        ui->En_Height->setChecked(false);
        ui->Height->setEnabled(false);
    }
    else
    {
        ui->En_Height->setChecked(true);
        ui->Height->setEnabled(true);
        ui->Height->setValue(NpcData.height);
    }

    if(!NpcData.en_width)
    {
        ui->En_Width->setChecked(false);
        ui->Width->setEnabled(false);
    }
    else
    {
        ui->En_Width->setChecked(true);
        ui->Width->setEnabled(true);
        ui->Width->setValue(NpcData.width);
    }

    if(!NpcData.en_npcblock)
    {
        ui->En_NPCBlock->setChecked(false);
        ui->NPCBlock->setEnabled(false);
    }
    else
    {
        ui->En_NPCBlock->setChecked(true);
        ui->NPCBlock->setEnabled(true);
        ui->NPCBlock->setChecked(NpcData.npcblock);
    }

    if(!NpcData.en_npcblocktop)
    {
        ui->En_NPCBlockTop->setChecked(false);
        ui->NPCBlockTop->setEnabled(false);
    }
    else
    {
        ui->En_NPCBlockTop->setChecked(true);
        ui->NPCBlockTop->setEnabled(true);
        ui->NPCBlockTop->setChecked(NpcData.npcblocktop);
    }

    if(!NpcData.en_speed)
    {
        ui->En_Speed->setChecked(false);
        ui->Speed->setEnabled(false);
    }
    else
    {
        ui->En_Speed->setChecked(true);
        ui->Speed->setEnabled(true);
        ui->Speed->setValue(NpcData.speed);
    }

    if(!NpcData.en_playerblock)
    {
        ui->En_PlayerBlock->setChecked(false);
        ui->PlayerBlock->setEnabled(false);
    }
    else
    {
        ui->En_PlayerBlock->setChecked(true);
        ui->PlayerBlock->setEnabled(true);
        ui->PlayerBlock->setChecked(NpcData.playerblock);
    }

    if(!NpcData.en_playerblocktop)
    {
        ui->En_PlayerBlockTop->setChecked(false);
        ui->PlayerBlockTop->setEnabled(false);
    }
    else
    {
        ui->En_PlayerBlockTop->setChecked(true);
        ui->PlayerBlockTop->setEnabled(true);
        ui->PlayerBlockTop->setChecked(NpcData.playerblocktop);
    }

    if(!NpcData.en_noblockcollision)
    {
        ui->En_NoBlockCollision->setChecked(false);
        ui->NoBlockCollision->setEnabled(false);
    }
    else
    {
        ui->En_NoBlockCollision->setChecked(true);
        ui->NoBlockCollision->setEnabled(true);
        ui->NoBlockCollision->setChecked(NpcData.noblockcollision);
    }

    if(!NpcData.en_nogravity)
    {
        ui->En_NoGravity->setChecked(false);
        ui->NoGravity->setEnabled(false);
    }
    else
    {
        ui->En_NoGravity->setChecked(true);
        ui->NoGravity->setEnabled(true);
        ui->NoGravity->setChecked(NpcData.nogravity);
    }

    if(!NpcData.en_cliffturn)
    {
        ui->En_TurnCliff->setChecked(false);
        ui->TurnCliff->setEnabled(false);
    }
    else
    {
        ui->En_TurnCliff->setChecked(true);
        ui->TurnCliff->setEnabled(true);
        ui->TurnCliff->setChecked(NpcData.cliffturn);
    }

}

bool npcedit::loadFile(const QString &fileName, NPCConfigFile FileData)
{
    QFile file(fileName);
    NpcData = FileData;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartNPCData = NpcData; //Save current history for made reset
    setDataBoxes();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    //setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

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
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool npcedit::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    /*
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    */

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
    /*setWindowModified(document()->isModified());*/
}

bool npcedit::maybeSave()
{
    /*
    if (document()->isModified()) {
    QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
             | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }*/

    return true;
}

void npcedit::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString npcedit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
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
}

void npcedit::on_En_PlayerBlock_clicked()
{
    if(ui->En_PlayerBlock->isChecked())
    {
        ui->PlayerBlock->setEnabled(true);
        NpcData.playerblock=true;
    }
    else
    {
        ui->PlayerBlock->setEnabled(false);
        NpcData.playerblock=false;
    }
}

void npcedit::on_En_PlayerBlockTop_clicked()
{
    if(ui->En_PlayerBlockTop->isChecked())
    {
        ui->PlayerBlockTop->setEnabled(true);
        NpcData.playerblocktop=true;
    }
    else
    {
        ui->PlayerBlockTop->setEnabled(false);
        NpcData.playerblocktop=false;
    }
}

void npcedit::on_En_NPCBlock_clicked()
{
    if(ui->En_NPCBlock->isChecked())
    {
        ui->NPCBlock->setEnabled(true);
        NpcData.npcblock=true;
    }
    else
    {
        ui->NPCBlock->setEnabled(false);
        NpcData.npcblock=false;
    }
}

void npcedit::on_En_NPCBlockTop_clicked()
{
    if(ui->En_NPCBlockTop->isChecked())
    {
        ui->NPCBlockTop->setEnabled(true);
        NpcData.npcblocktop=true;
    }
    else
    {
        ui->NPCBlockTop->setEnabled(false);
        NpcData.npcblocktop=false;
    }
}

void npcedit::on_En_NoBlockCollision_clicked()
{
    if(ui->En_NoBlockCollision->isChecked())
    {
        ui->NoBlockCollision->setEnabled(true);
        NpcData.noblockcollision=true;
    }
    else
    {
        ui->NoBlockCollision->setEnabled(false);
        NpcData.noblockcollision=false;
    }
}

void npcedit::on_En_NoGravity_clicked()
{
    if(ui->En_NoGravity->isChecked())
    {
        ui->NoGravity->setEnabled(true);
        NpcData.nogravity=true;
    }
    else
    {
        ui->NoGravity->setEnabled(false);
        NpcData.nogravity=false;
    }
}

void npcedit::on_En_TurnCliff_clicked()
{
    if(ui->En_TurnCliff->isChecked())
    {
        ui->TurnCliff->setEnabled(true);
        NpcData.cliffturn=true;
    }
    else
    {
        ui->TurnCliff->setEnabled(false);
        NpcData.cliffturn=false;
    }
}

void npcedit::on_ResetNPCData_clicked()
{
    NpcData = StartNPCData; //Restore first version
    setDataBoxes();

}
