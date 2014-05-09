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

#include <QFile>
#include <QTextStream>
#include <QApplication>

QString debugLogFile = "PGE_debug_log.txt" ;

void WriteToLog(QtMsgType type, QString msg)
    {
        QString txt;
        switch (type) {
        case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
        case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
        case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
        case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
    }

    QFile outFile(debugLogFile);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}
