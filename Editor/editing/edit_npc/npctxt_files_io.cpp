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

#include <QtWidgets>

#include "npcedit.h"
#include "./ui_npcedit.h"
#include "./file_formats/file_formats.h"
#include "./main_window/global_settings.h"
#include "./common_features/mainwinconnect.h"


void npcedit::newFile(unsigned long npcID)
{
    npc_id = npcID;
    static int sequenceNumber = 1;
    ui->CurrentNPCID->setText( QString::number(npcID) );

    isUntitled = true;

    curFile = tr("npc-%1.txt").arg(npcID);

    setWindowTitle(curFile + QString(" [*] (%1)").arg(sequenceNumber++));

    setDefaultData(npcID);

    NpcData = DefaultNPCData; // create data template
    StartNPCData = DefaultNPCData;
    setDataBoxes();

    documentWasModified();

    loadPreview();
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
            npc_id = tmp[1].toInt();
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

    loadPreview();
    on_DirectLeft_clicked();

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
      (isUntitled)?GlobalSettings::savePath_npctxt+QString("/")+curFile:curFile, tr("SMBX custom NPC config file (npc-*.txt)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool npcedit::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("File save error"),
                             tr("Cannot save file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    GlobalSettings::savePath_npctxt = QFileInfo(fileName).path();

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << FileFormats::WriteNPCTxtFile(NpcData);
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);

    documentNotModified();

    refreshImageFile();
    updatePreview();

    MainWinConnect::pMainWin->AddToRecentFiles(fileName);
    MainWinConnect::pMainWin->SyncRecentFiles();

    return true;
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

void npcedit::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    documentWasModified();
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}



QString npcedit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}


QString npcedit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
