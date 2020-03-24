/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef RUNTIMEENGINE_H
#define RUNTIMEENGINE_H

#include <QObject>
#include <QMenu>
#include <PGE_File_Formats/lvl_filedata.h>
#include <PGE_File_Formats/wld_filedata.h>

class AbstractRuntimeEngine : public QObject
{
    Q_OBJECT

public:
    explicit AbstractRuntimeEngine(QObject *parent = nullptr);

    virtual ~AbstractRuntimeEngine();

    static bool checkIsEngineRunning(AbstractRuntimeEngine *e, QWidget *p);

    static void rejectUntitled(QWidget *p);

    virtual void init();
    virtual void unInit();

    virtual void initMenu(QMenu *destmenu);

    virtual bool doTestLevelIPC(const LevelData &d);
    virtual bool doTestLevelFile(const QString &levelFile);

    virtual bool doTestWorldIPC(const WorldData &d);
    virtual bool doTestWorldFile(const QString &worldFile);

    virtual bool runNormalGame();

    virtual void terminate();

    virtual bool isRunning();

    QString getError();

    enum
    {
        //! Supported level testing with IPC (possible to test untitled/scratch or modified levels without saving)
        CAP_LEVEL_IPC  = 0x01,
        //! Supported level testing by file path (file saving is required)
        CAP_LEVEL_FILE = 0x02,
        //! Supported world map testing with IPC (possible to test untitled/scratch or modified worlds without saving)
        CAP_WORLD_IPC  = 0x04,
        //! Supported world map testing by file path (file saving is required)
        CAP_WORLD_FILE = 0x08,
        //! It's possible to just start a playing of a normal game
        CAP_RUN_GAME = 0x10,
        //! Game keeps working instance in background
        CAP_DAEMON = 0x20,
        //! Provide an extra menu
        CAP_HAS_MENU = 0x40,
    };

    virtual int capabilities();

    bool hasCapability(int cap);

signals:
    void workFinished();

protected:
    QString m_errorString;
};

#endif // RUNTIMEENGINE_H
