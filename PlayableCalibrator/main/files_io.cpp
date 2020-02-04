/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../calibrationmain.h"
#include "ui_calibrationmain.h"
#include "globals.h"
#include "graphics.h"
#include "app_path.h"

void CalibrationMain::OpenFile(QString fileName)
{
    QString imgFileM;
    QFileInfo ourFile(fileName);
    g_currentFile = fileName;

    g_lastOpenDir = ourFile.absoluteDir().path() + "/";

    QString errString;
    if(!Graphics::loadMaskedImage(g_lastOpenDir, ourFile.fileName(), imgFileM, m_xImageSprite, &errString))
    {
        m_xImageSprite = QPixmap();
        QMessageBox::warning(this, "Image Loading error",
                             QString("Failed to load image file %1!\nError string: %2")
                             .arg(fileName)
                             .arg(errString),
                             QMessageBox::Ok);
        g_currentFile = "";
        return;
    }

    loadConfig(fileName);

    initScene();
    updateControls();
    updateScene();
}



//Made templates for test calibration
void CalibrationMain::on_MakeTemplateB_clicked()
{
    QImage output(1000, 1000, QImage::Format_ARGB32);
    output.fill(Qt::transparent);

    QPainter pa(&output);
    pa.setPen(QPen(Qt::gray, 1));
    for(int i=1; i<10;i++)
    {
        pa.drawLine(0, 100*i, 1000, 100*i );
        pa.drawLine(100*i, 0, 100*i, 1000 );
    }
    pa.setPen(QPen(Qt::yellow, 1));
    pa.setBrush(Qt::transparent);

    for(int i=0; i<10; i++)
        for(int j=0; j<10; j++)
        {
            if(g_framesX[i][j].used)
            {
                pa.drawRect(g_framesX[i][j].offsetX + 100*i, g_framesX[i][j].offsetY + 100 * j,
                            g_frameWidth-1, (g_framesX[i][j].isDuck?g_frameHeightDuck:g_frameHeight)-1);
            }
        }
    pa.end();

    QFileInfo ourFile(g_currentFile);
    QString targetFilePng =  ourFile.absoluteDir().absolutePath() + "/" + ourFile.baseName()+"_hitboxes.png";
    targetFilePng = QFileDialog::getSaveFileName(this, tr("Save hitbox map as image"), targetFilePng, "PNG Image (*.png)");
    if(targetFilePng.isEmpty()) return;
//    QString targetFileGif1    =  AppPathManager::userAppDir() + "/calibrator/templates/" + ourFile.baseName() + ".gif";
//    QString targetFileGifMask =  AppPathManager::userAppDir() + "/calibrator/templates/" + ourFile.baseName() + "m.gif";

    QApplication::setOverrideCursor(Qt::WaitCursor);

//    QImage targetGif = output;
//    QImage mask = targetGif.alphaChannel();
//    mask.invertPixels();
    QPixmap::fromImage(output).save(targetFilePng, "png");
//    Graphics::toGif(targetGif, targetFileGif1);
//    Graphics::toGif(mask, targetFileGifMask);

    QApplication::restoreOverrideCursor();

    QMessageBox::information(this, tr("Saved"), tr("Hitbox map has been saved!"));

}
