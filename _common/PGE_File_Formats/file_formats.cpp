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

#include "pge_file_lib_sys.h"
#include "pge_file_lib_globs.h"

#include "file_formats.h"
#ifdef PGE_EDITOR
#include <common_features/themes.h>
#endif
#ifdef PGE_FILES_USE_MESSAGEBOXES
#include <QMessageBox>
#endif

PGESTRING FileFormats::errorString="";
bool FileFormats::silentMode=false;

void FileFormats::BadFileMsg(PGESTRING fileName_DATA, int str_count, PGESTRING line)
{
    #ifdef PGE_FILES_USE_MESSAGEBOXES
    if(!silentMode)
    {
        QMessageBox * box;
        box= new QMessageBox();
        box->setWindowTitle( QTranslator::tr("Bad File") );
        box->setWindowIcon( Themes::icon(Themes::debugger) );
        box->setText(
                    PGESTRING( QTranslator::tr("Bad file format\nFile: %1\n").arg(fileName_DATA)  //Print Bad data string
                              +QTranslator::tr("Line Number: %1\n").arg(str_count)         //Print Line With error
                               +QTranslator::tr("Line Data: %1").arg(line))
                    );
        box->setStandardButtons(QMessageBox::Ok);
        box->setIcon(QMessageBox::Warning);
        box->exec();
    }
    #endif
    #ifdef PGE_FILES_QT
    errorString = PGESTRING( QTranslator::tr("Bad file format\nFile: %1\n").arg(fileName_DATA)  //Print Bad data string
                           +QTranslator::tr("Line Number: %1\n").arg(str_count)         //Print Line With error
                            +QTranslator::tr("Line Data: %1").arg(line));
    #else
    std::ostringstream errMsg;
    errMsg<<"Bad file format\nFile:"<<fileName_DATA<<"\n"<<
            "Line Number: "<<str_count<<"\n"<<"Line Data: "<< line;
    errorString = errMsg.str();
    #endif
}


PGESTRING FileFormats::removeQuotes(PGESTRING str)
{
    PGESTRING target = str;
    #ifdef PGE_FILES_QT
    if(target.isEmpty())
        return target;
    if(target[0]==QChar('\"'))
        target.remove(0,1);
    if((!target.isEmpty()) && (target[target.size()-1]==QChar('\"')))
        target.remove(target.size()-1,1);
    #else
        if(target.empty())
            return target;
        if(target[0]=='\"')
            target.erase(target.begin()+0);
        if((!target.empty()) && (target[target.size()-1]=='\"'))
            target.erase(target.begin()+target.size()-1);
    #endif
    return target;
}

/***************************************************************************/
CrashData::CrashData() : used(false),untitled(false), modifyed(false) {}

CrashData::CrashData(const CrashData &_cd)
{
    this->used=_cd.used;
    this->untitled=_cd.untitled;
    this->modifyed=_cd.modifyed;
    this->fullPath=_cd.fullPath;
    this->filename=_cd.filename;
    this->path = _cd.path;
}

CrashData::CrashData(CrashData &_cd)
{
    this->used=_cd.used;
    this->untitled=_cd.untitled;
    this->modifyed=_cd.modifyed;
    this->fullPath=_cd.fullPath;
    this->filename=_cd.filename;
    this->path = _cd.path;
}

void CrashData::reset()
{
    used=false;
    untitled=false;
    modifyed=false;
    fullPath.clear();
    filename.clear();
    path.clear();
}

