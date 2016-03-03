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

#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QProcess>

#include <common_features/app_path.h>
#include <common_features/logger_sets.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>
#include <dev_console/devconsole.h>
#include <audio/sdl_music_player.h>

#include "testing_settings.h"

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <networking/engine_intproc.h>
#include <PGE_File_Formats/file_formats.h>

#ifdef Q_OS_WIN
#include <windows.h>
#include <QDesktopWidget>
#include <cstring>
#include <QDirIterator>
#endif

void MainWindow::on_action_doTest_triggered()
{
    QString command;

    #ifdef _WIN32
    command = ApplicationPath+"/pge_engine.exe";
    #elif __APPLE__
    command = ApplicationPath+"/pge_engine.app/Contents/MacOS/pge_engine";
    #else
    command = ApplicationPath+"/pge_engine";
    #endif

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker);

    if(!QFileInfo(command).exists())
    {
        QMessageBox::warning(this, tr("Engine is not found"),
                             tr("Can't start testing, engine is not found: \n%1\nPlease, check the application directory.")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

    if(engine_proc.state()==QProcess::Running)
    {
        if(QMessageBox::warning(this, tr("Engine already runned"),
                             tr("Engine is already testing another level.\n"
                                "Do you want to abort current testing process?"),
                             QMessageBox::Abort|QMessageBox::Cancel)==QMessageBox::Abort) {
            engine_proc.terminate();
            engine_proc.close();
        }
        return;
    }

    if(activeChildWindow()==1)
    {
        IntEngine::init(&engine_proc);

        //if(activeLvlEditWin()->isUntitled) return;
        LevelEdit* edit = activeLvlEditWin();
        if(!edit) return;

        QStringList args;
        args << "--debug";
        args << "--config=\""+configs.config_dir+"\"";
        args << "--interprocessing";//activeLvlEditWin()->curFile;

        IntEngine::setTestLvlBuffer(edit->LvlData);

        qDebug() << "Executing engine..." << command;
        engine_proc.start(command, args);
        engine_proc.waitForStarted();
        qDebug() << "Started";

        //Stop music playback in the PGE!
        on_actionPlayMusic_triggered(false);
        setMusicButton(false);
        PGE_MusPlayer::MUS_stopMusic();
    }
    else
        return;
}


void MainWindow::on_action_doSafeTest_triggered()
{
    QString command;

    #ifdef _WIN32
    command = ApplicationPath+"/pge_engine.exe";
    #elif __APPLE__
    command = ApplicationPath+"/pge_engine.app/Contents/MacOS/pge_engine";
    #else
    command = ApplicationPath+"/pge_engine";
    #endif

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker);

    if(!QFileInfo(command).exists())
    {
        QMessageBox::warning(this, tr("Engine is not found"),
                             tr("Can't start testing, engine is not found: \n%1\nPlease, check the application directory.")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

    if(engine_proc.state()==QProcess::Running)
    {
        if(QMessageBox::warning(this, tr("Engine already runned"),
                             tr("Engine is already testing another level.\n"
                                "Do you want to abort current testing process?"),
                             QMessageBox::Abort|QMessageBox::Cancel)==QMessageBox::Abort) {
            engine_proc.terminate();
            engine_proc.close();
        }
        return;
    }

    QStringList args;
    args << "--debug";
    args << "--config=\""+configs.config_dir+"\"";

    if(activeChildWindow()==1)
    {
        if(activeLvlEditWin()->isUntitled)
        {
            QMessageBox::warning(this, tr("Save file first"),
            tr("To run testing of saved file, please save them into disk first!\n"
               "You can run testing without saving of file if you will use \"Run testing\" menu item."),
            QMessageBox::Ok);
            return;
        }
        args << activeLvlEditWin()->curFile;
    }
    else
    if(activeChildWindow()==3)
    {
        if(activeWldEditWin()->isUntitled)
        {
            QMessageBox::warning(this, tr("Save file first"),
            tr("To run testing of saved file, please save them into disk first!\n"
               "You can run testing without saving of file if you will use \"Run testing\" menu item."),
            QMessageBox::Ok);
            return;
        }
        args << activeWldEditWin()->curFile;
    }

    engine_proc.start(command, args);
    engine_proc.waitForStarted();

    //Stop music playback in the PGE Editor!
    on_actionPlayMusic_triggered(false);
    setMusicButton(false);
    PGE_MusPlayer::MUS_stopMusic();
}


void MainWindow::on_action_Start_Engine_triggered()
{
    QString command;

    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker);

    #ifdef _WIN32
    command = ApplicationPath+"/pge_engine.exe";
    #elif __APPLE__
    command = ApplicationPath+"/pge_engine.app/Contents/MacOS/pge_engine";
    #else
    command = ApplicationPath+"/pge_engine";
    #endif

    if(!QFileInfo(command).exists())
    {
        QMessageBox::warning(this, tr("Engine is not found"),
                             tr("Engine is not found: \n%1\nPlease, check the application directory.")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

    QStringList args;
    args << "--config=\""+configs.config_dir+"\"";

    QProcess::startDetached(command, args);
    //Stop music playback in the PGE Editor!
    on_actionPlayMusic_triggered(false);
    setMusicButton(false);
    PGE_MusPlayer::MUS_stopMusic();
}


void MainWindow::on_action_testSettings_triggered()
{
    TestingSettings testingSetup(this);
    util::DialogToCenter(&testingSetup, true);
    testingSetup.exec();
}


#define DUMMY_WORLD_GENERATION
//#define USE_DIRECT_LEVELTEST_LUNALUA_0_7_3 //Until Rednaxela will implement full level testing thing
#define SHARED_FILEPATH_SENDING
//#define DO_WINAPI_TRICKS //Just junk test with attempt to embedd SMBX Editor's window as built-in subwindow (works creepy and buggy, so, try on your own risk! xD)

#if defined(Q_OS_WIN) && defined(DO_WINAPI_TRICKS)
static QWidget *test=NULL;
#endif

#if defined(Q_OS_WIN) && defined(DUMMY_WORLD_GENERATION)
enum LunaLoaderResult {
    LUNALOADER_OK = 0,
    LUNALOADER_CREATEPROCESS_FAIL,
    LUNALOADER_PATCH_FAIL
};

static void setJmpAddr(uint8_t* patch, DWORD patchAddr, DWORD patchOffset, DWORD target) {
    DWORD* dwordAddr = (DWORD*)&patch[patchOffset+1];
    *dwordAddr = (DWORD)target - (DWORD)(patchAddr + patchOffset + 5);
}

static LunaLoaderResult LunaLoaderRun(const wchar_t *pathToSMBX,
                                      const wchar_t *cmdLineArgs,
                                      const wchar_t *workingDir,
                                      PROCESS_INFORMATION &pi) {
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));

    // Prepare command line
    size_t pos = 0;
    std::wstring quotedPathToSMBX(pathToSMBX);
    while ((pos = quotedPathToSMBX.find(L"\"", pos)) != std::string::npos) {
        quotedPathToSMBX.replace(pos, 1, L"\\\"");
        pos += 2;
    }
    std::wstring strCmdLine = (
        std::wstring(L"\"") + quotedPathToSMBX + std::wstring(L"\" ") +
        std::wstring(cmdLineArgs)
        );
    uint32_t cmdLineMemoryLen = sizeof(wchar_t) * (strCmdLine.length() + 1); // Include null terminator
    wchar_t* cmdLine = (wchar_t*)malloc(cmdLineMemoryLen);
    std::memcpy(cmdLine, strCmdLine.c_str(), cmdLineMemoryLen);

    // Create process
    if (!CreateProcessW(pathToSMBX, // Launch smbx.exe
        cmdLine,          // Command line
        NULL,             // Process handle not inheritable
        NULL,             // Thread handle not inheritable
        FALSE,            // Set handle inheritance to FALSE
        CREATE_SUSPENDED, // Create in suspended state
        NULL,             // Use parent's environment block
        workingDir,       // Use parent's starting directory
        &si,              // Pointer to STARTUPINFO structure
        &pi)              // Pointer to PROCESS_INFORMATION structure
        )
    {
        free(cmdLine); cmdLine = NULL;
        return LUNALOADER_CREATEPROCESS_FAIL;
    }
    free(cmdLine); cmdLine = NULL;

    // Patch 1 (jump to Patch 2)
    uintptr_t LoaderPatchAddr1 = 0x40BDD8;
    unsigned char LoaderPatch1[] =
    {
        0xE9, 0x00, 0x00, 0x00, 0x00  // 0x40BDD8 JMP <Patch2>
    };

    // Patch 2 (loads LunaDll.dll)
    unsigned char LoaderPatch2[] =
    {
        0x68, 0x64, 0x6C, 0x6C, 0x00, // 00 PUSH "dll\0"
        0x68, 0x44, 0x6C, 0x6C, 0x2E, // 05 PUSH "Dll."
        0x68, 0x4C, 0x75, 0x6E, 0x61, // 0A PUSH "Luna"
        0x54,                         // 0F PUSH ESP
        0xE8, 0x00, 0x00, 0x00, 0x00, // 10 CALL LoadLibraryA
        0x83, 0xC4, 0x0C,             // 15 ADD ESP, 0C
        0x68, 0x6C, 0xC1, 0x40, 0x00, // 18 PUSH 40C16C (this inst used to be at 0x40BDD8)
        0xE9, 0x00, 0x00, 0x00, 0x00, // 1D JMP 40BDDD
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
    };

    // Allocate space for Patch 2
    DWORD LoaderPatchAddr2 = (DWORD)VirtualAllocEx(
        pi.hProcess,           // Target process
        NULL,                  // Don't request any particular address
        sizeof(LoaderPatch2),  // Length of Patch 2
        MEM_COMMIT,            // Type of memory allocation
        PAGE_READWRITE         // Memory protection type
        );
    if (LoaderPatchAddr2 == (DWORD)NULL) {
        return LUNALOADER_PATCH_FAIL;
    }

    // Set Patch1 Addresses
    setJmpAddr(LoaderPatch1, LoaderPatchAddr1, 0x00, LoaderPatchAddr2);

    // Set Patch2 Addresses
    setJmpAddr(LoaderPatch2, LoaderPatchAddr2, 0x10, (DWORD)&LoadLibraryA);
    setJmpAddr(LoaderPatch2, LoaderPatchAddr2, 0x1D, LoaderPatchAddr1 + 5);

    // Patch the entry point...
    if (WriteProcessMemory(pi.hProcess, (void*)LoaderPatchAddr1, LoaderPatch1, sizeof(LoaderPatch1), NULL) == 0 ||
        WriteProcessMemory(pi.hProcess, (void*)LoaderPatchAddr2, LoaderPatch2, sizeof(LoaderPatch2), NULL) == 0)
    {
        return LUNALOADER_PATCH_FAIL;
    }

    // Change Patch2 memory protection type
    DWORD TmpDword = 0;
    if (VirtualProtectEx(
        pi.hProcess,
        (void*)LoaderPatchAddr2,
        sizeof(LoaderPatch2),
        PAGE_EXECUTE,
        &TmpDword
    ) == 0) {
        return LUNALOADER_PATCH_FAIL;
    }

    // Resume the main program thread
    ResumeThread(pi.hThread);

    // Close handles
    CloseHandle(pi.hThread);
    //CloseHandle(pi.hProcess); //Don't close it because needed to catch already-running SMBX Engine

    return LUNALOADER_OK;
}
#endif

#ifdef SHARED_FILEPATH_SENDING
static bool SMBXEditorIsStarted()
{
    HWND smbxWind = FindWindowA("ThunderRT6MDIForm", NULL);
    return (smbxWind);
}
#endif

void MainWindow::on_actionRunTestSMBX_triggered()
{
    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker);

 #ifdef Q_OS_WIN
    #ifdef DUMMY_WORLD_GENERATION
    if(activeChildWindow()==1)
    {
        #ifdef SHARED_FILEPATH_SENDING
        if(!SMBXEditorIsStarted())//Is SMBX Editor already running. If running, do old method - send path to already running editor
        {
        #endif //SHARED_FILEPATH_SENDING
            QString smbxPath = ConfStatus::configDataPath;
            if(!QFile(smbxPath+ConfStatus::SmbxEXE_Name).exists())
            {
                QMessageBox::warning(this, tr("SMBX Directory wasn't configured right"),
                tr("%1 not found!\nTo run testing via SMBX you should have right SMBX Integration configuration package!")
                                     .arg(smbxPath+ConfStatus::SmbxEXE_Name),
                QMessageBox::Ok);
                return;
            }

            DWORD lpExitCode=0;
            if(GetExitCodeProcess(m_luna_pi.hProcess, &lpExitCode))
            {
                if(lpExitCode==STILL_ACTIVE)
                {
                    if(QMessageBox::warning(this, tr("SMBX Test is already runned"),
                                         tr("SMBX Engine is already testing another level.\n"
                                            "Do you want to abort current testing process?"),
                                         QMessageBox::Abort|QMessageBox::Cancel)==QMessageBox::Abort) {
                        WaitForSingleObject(m_luna_pi.hProcess, 100);
                        TerminateProcess(m_luna_pi.hProcess, lpExitCode);
                        CloseHandle(m_luna_pi.hProcess);

                    }
                    return;
                }
            }

            QString tempPath = smbxPath+"/worlds/";
            QString tempName = "tmp.PgeWorld.DeleteMe";
            QString newEpisode = tempPath+tempName+"/";

            LevelEdit* ed = activeLvlEditWin();

            QDir dummyWorld(newEpisode);
            if(dummyWorld.exists(newEpisode))
            {
                QString testFolder=newEpisode+"templevel/";
                RemoveDirectoryW(testFolder.toStdWString().c_str());//Remove symblic link if possible

                //Clean-up old stuff
                dummyWorld.removeRecursively();
            }
            dummyWorld.mkpath(newEpisode);

            ed->saveSMBX64LVL(newEpisode+"/templevel.lvl", true);
            if(!ed->isUntitled)
            {
                QString episodePath = ed->LvlData.path;
                QString customPath  = ed->LvlData.path+"/"+ed->LvlData.filename;

                //Copy available custom stuff into temp directory
                QDir episodeDir(episodePath);
                QDir customDir(customPath);

                QStringList fileters;
                fileters << "*.txt" << "*.ini" << "*.lua" << "*.gif" << "*.png" << "*.bmp";
                QStringList fileters_cdir;
                fileters_cdir << "*.txt" << "*.ini" << "*.lua" << "*.gif" << "*.png" << "*.mp3"
                              //OGG Vorbis and FLAC (LibOGG, LibVorbis, LibFLAC)
                              << "*.ogg" << "*.flac"
                              //Uncompressed audio data
                              << "*.wav" << "*.voc" << "*.aiff"
                              //MIDI
                              << "*.mid"
                              //MikMod (Modules)
                              << "*.mod" << "*.it" << "*.s3m" << "*.669" << "*.med" << "*.xm" << "*.amf"
                              << "*.apun" << "*.dsm" << "*.far" << "*.gdm" << "*.imf" << "*.mtm"
                              << "*.okt" << "*.stm" << "*.stx" << "*.ult" << "*.uni"
                              //GAME EMU
                              << "*.ay" << "*.gbs"<<"*.gym"<<"*.hes"<<"*.kss"<<"*.nsf"<<"*.nsfe"<<"*.sap"<<"*.spc"<<"*.vgm"<<"*.vgz"
                              //Rockythechao's extensions
                              << "*.anim";
                episodeDir.setNameFilters(fileters);
                customDir.setNameFilters(fileters_cdir);

                //***********************Attempt to make symbolic link*******************************/
                bool needToCopyEverything=true;
                typedef BOOL *(WINAPI *FUNK_OF_SYMLINKS) (TCHAR * linkFileName, TCHAR * existingFileName, DWORD flags);
                HMODULE hKernel32 = NULL;
                FUNK_OF_SYMLINKS fCreateSymbolicLink = NULL;
                hKernel32 = LoadLibraryW(L"KERNEL32.DLL");
                if(hKernel32)
                {
                    fCreateSymbolicLink = (FUNK_OF_SYMLINKS)GetProcAddress(hKernel32, "CreateSymbolicLinkW");
                    if(fCreateSymbolicLink) //Try to make a symblic link
                    {
                        QString newPath=newEpisode+"templevel/";
                        if(fCreateSymbolicLink((TCHAR*)newPath.toStdWString().c_str(),
                                               (TCHAR*)customPath.toStdWString().c_str(), 0x1))
                            needToCopyEverything = false;
                    }
                }
                //************Copy images and scripts from episode folder**********************/
                QStringList files = episodeDir.entryList(QDir::Files);
                foreach(QString filex, files)
                {
                    QFile::copy(episodePath+"/"+filex, newEpisode+filex);
                }

                //********************Copy images and scripts from custom folder*****************/
                //***************for case where impossible to make a symbolic link***************/
                if(needToCopyEverything)
                {
                    customDir.setSorting(QDir::NoSort);
                    QDirIterator dirsList(customPath, fileters_cdir,
                                          QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,
                                          QDirIterator::Subdirectories);
                    while(dirsList.hasNext())
                    {
                        dirsList.next();
                        QString relativeDir = customDir.relativeFilePath(dirsList.fileInfo().absoluteDir().absolutePath());
                        QString filex = dirsList.fileName();//customDir.relativeFilePath(dirsList.filePath());
                        QString relNewPath = newEpisode+"templevel/"+relativeDir;
                        QDir newRelDir(relNewPath);
                        if(!newRelDir.exists())
                            newRelDir.mkpath(relNewPath);
                        QFile::copy(customPath+"/"+relativeDir+"/"+filex, relNewPath+"/"+filex);
                    }
                }
                /*********************************************************************************/

                //Copy custom musics if possible
                foreach(LevelSection sec, ed->LvlData.sections)
                {
                    if(sec.music_file.isEmpty())
                        continue;

                    QString musFile=sec.music_file;
                    for(int i=0;i<musFile.size();i++)
                    {
                        if(musFile[i]=='|')
                        {
                            musFile.remove(i, musFile.size()-i);
                        }
                    }
                    QString MusicFileName=episodePath+"/"+musFile;
                    QString MusicNewPath=newEpisode+musFile;

                    QFile mus(MusicFileName);
                    if(!mus.exists(MusicFileName))
                        continue;

                    QFileInfo inf(MusicNewPath);
                    if(!needToCopyEverything)
                    {
                        if(inf.absoluteDir().absolutePath().contains((newEpisode+"templevel/"), Qt::CaseInsensitive))
                            continue; //Don't copy musics to the same directory!
                    }
                    if(!inf.absoluteDir().exists())
                    {
                        inf.absoluteDir().mkpath(inf.absoluteDir().absolutePath());
                    }
                    if(!needToCopyEverything)
                    {
                       if(fCreateSymbolicLink((TCHAR*)MusicNewPath.toStdWString().c_str(),
                                              (TCHAR*)MusicFileName.toStdWString().c_str(), 0x0)==0)
                       {
                           mus.copy(MusicNewPath);//Copy file if impossible to make symbolic link to it
                       }
                    } else {
                        mus.copy(MusicNewPath);
                    }
                }
                if(hKernel32)
                {
                    FreeLibrary(hKernel32);
                }
            }

            WorldData worldmap = FileFormats::CreateWorldData();
            worldmap.EpisodeTitle = "_temp_episode_pge";
            worldmap.IntroLevel_file = "templevel.lvl";
            worldmap.restartlevel = true;

            QFile file(newEpisode+"/tempworld.wld");
            if(!file.open(QFile::WriteOnly))
            {
                QMessageBox::warning(this, tr("File save error"),
                                     tr("Cannot save file %1:\n%2.")
                                     .arg(newEpisode+"/tempworld.wld")
                                     .arg(file.errorString()));
                return;
            }

            QString raw = FileFormats::WriteSMBX64WldFile(worldmap, 64);
            for(int i=0; i<raw.size(); i++)
            {
                if(raw[i]=='\n')
                {
                    //Force writing CRLF to prevent fakse damage of file on SMBX in Windows
                    const char bytes[2] = {0x0D, 0x0A};
                    file.write((const char*)(&bytes), 2);
                }
                else
                {
                    const char byte[1] = {raw[i].toLatin1()};
                    file.write((const char*)(&byte), 1);
                }
            }
            file.close();

            QString command=smbxPath+ConfStatus::SmbxEXE_Name;
            QStringList params;

            if(!QFile(smbxPath+"LunaDll.dll").exists())
            {
                QMessageBox::warning(this, tr("Vanilla SMBX detected!"),
                tr("%2 not found!\nYou have a Vanilla SMBX!\n"
                   "That means, impossible to launch level testing automatically.\n"
                   "To launch a level testing, start a game and select playing of the %1 episode.")
                                     .arg("\"_temp_episode_pge\"")
                                     .arg(smbxPath+"LunaDll.dll"),
                QMessageBox::Ok);

                engine_proc.setWorkingDirectory(smbxPath);
                engine_proc.start(command, params);
                engine_proc.waitForStarted();

                return;
            }

            command.replace('/', '\\');

            QDir smbxConfig(smbxPath+"/config/");
            smbxConfig.mkpath(smbxPath+"/config/");

            #ifdef USE_DIRECT_LEVELTEST_LUNALUA_0_7_3
            if(ed->isUntitled || ed->curFile.endsWith(".lvlx"))
            {
            #endif
                params << "--patch";
                params << "--game";

                QFile autostart(smbxPath+"/config/autostart.ini");
                QString autostartTempEpisode = "[autostart]\n"
                        "do-autostart: true\n"
                        "episode-name: %1\n"
                        "singleplayer: true\n"
                        "# 1 = Mario/Demo\n"
                        "# 2 = Luigi/Iris\n"
                        "# 3 = Peach/Kood\n"
                        "# 4 = Toad/Raocow\n"
                        "# 5 = Link/Sheath\n"
                        "character-player1: 1\n"
                        "character-player2: 2\n"
                        "save-slot: 1\n";

                autostart.open(QIODevice::WriteOnly|QIODevice::Text);
                QTextStream autostart_out(&autostart);
                autostart_out << autostartTempEpisode.arg(worldmap.EpisodeTitle);
                autostart_out.flush();
                autostart.close();
            #ifdef USE_DIRECT_LEVELTEST_LUNALUA_0_7_3
            } else {
                params << "--testLevel=\""+ed->curFile+"\"";
            }
            #endif

            QString argString;
            for (int i=0; i<params.length(); i++) {
                if (i > 0) {
                    argString += " ";
                }
                argString += params[i];
            }

            LunaLoaderResult res = LunaLoaderRun(command.toStdWString().c_str(),
                                                 argString.toStdWString().c_str(),
                                                 smbxPath.toStdWString().c_str(),
                                                 m_luna_pi);

            if(res==LUNALOADER_OK)
            {
                //Stop music playback in the PGE!
                on_actionPlayMusic_triggered(false);
                setMusicButton(false);
                PGE_MusPlayer::MUS_stopMusic();
            } else {
                QString luna_error="Unknown error";

                switch(res)
                {
                case LUNALOADER_CREATEPROCESS_FAIL:
                    luna_error=tr("process execution is failed.");
                case LUNALOADER_PATCH_FAIL:
                    luna_error=tr("patching has failed.");
                default:
                    break;
                }

                QMessageBox::warning(this, tr("Failed to launch LunaLUA-SMBX!"),
                tr("Impossible to launch SMBX Engine, because %1").arg(luna_error),
                QMessageBox::Ok);
            }
        #ifdef SHARED_FILEPATH_SENDING
        }
        else //Attempt to attach into running SMBX Editor
        {
            if(activeLvlEditWin()->isUntitled)
            {
                QMessageBox::warning(this, tr("Save file first"),
                tr("To run testing via SMBX file must be saved into disk first!"),
                QMessageBox::Ok);
                return;
            }
            QString fullPathToLevel= activeLvlEditWin()->curFile;
            if(!activeLvlEditWin()->LvlData.smbx64strict)
            {
                QMessageBox::StandardButton ret=QMessageBox::warning(this, tr("Incompatible file format"),
                tr("To take able to test level in the SMBX, file should be saved into SMBX64 format!\n"
                   "Will be created a temporary file. Do you want to continue?"),
                QMessageBox::Yes|QMessageBox::Abort);

                if(ret==QMessageBox::Abort)
                    return;

                //Double point will be unique and will don't overwrite your lvl file, but will use same custom folder
                QString newPath = activeLvlEditWin()->LvlData.path+"/"+activeLvlEditWin()->LvlData.filename+"..lvl";
                if(!activeLvlEditWin()->saveSMBX64LVL(newPath, true))
                {
                    QMessageBox::warning(this, tr("Error"),
                    tr("Fail to create temp file %1").arg(newPath),
                    QMessageBox::Ok);
                    return;
                }
                fullPathToLevel = newPath;
            }


            COPYDATASTRUCT* cds = new COPYDATASTRUCT;
            cds->cbData = 1;
            cds->dwData = (ULONG_PTR)0xDEADC0DE;
            cds->lpData = NULL;

            HWND smbxWind = FindWindowA("ThunderRT6MDIForm", NULL);
            if(smbxWind)
            {
                #ifdef DO_WINAPI_TRICKS
                test = new QWidget();
                test->setAttribute(Qt::WA_DeleteOnClose, true);
                test->setWindowTitle("Embedded SMBX Editor box");
                ui->centralWidget->addSubWindow(test);
                test->show();
                test->update();
                qApp->processEvents();

                // Change the parent so the calc window belongs to our apps main window
                SetParent(smbxWind, (HWND)test->winId());
                // Update the style so the calc window is embedded in our main window
                LONG lExStyle = GetWindowLong(smbxWind, GWL_STYLE);
                //lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_CAPTION);
                SetWindowLong(smbxWind, GWL_STYLE, lExStyle | WS_CHILD);
        //        LONG lExStyle = GetWindowLong(calcHwnd, GWL_EXSTYLE);
        //        lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE | WS_CAPTION);
        //        SetWindowLong(calcHwnd, GWL_EXSTYLE, lExStyle);

                // We need to update the position as well since changing the parent does not
                // adjust it automatically.
                //SetWindowPos(smbxWind, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
                //SetWindowTheme(calcHwnd,L"",L"");
                #endif

                fullPathToLevel.replace('/', '\\');

                if(activeLvlEditWin()->LvlData.modified)
                {
                    QMessageBox::StandardButton ret = QMessageBox::question(this,
                            tr("SMBX Level test"),
                            tr("Do you wanna to save file before start testing?\n"),
                            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
                    if(ret==QMessageBox::Cancel)
                        return;
                    else
                    if(ret==QMessageBox::Yes)
                        activeLvlEditWin()->save();
                }

                /****************Write file path into shared memory**************************/

                //Connect to Shared memory and send data
                TCHAR szName[]=TEXT("LunaDLL_LevelFileName_834727238");
                HANDLE hMapFile;
                wchar_t *pBuf;
                hMapFile = OpenFileMapping(
                                   FILE_MAP_ALL_ACCESS,   // read/write access
                                   FALSE,                 // do not inherit the name
                                   szName);               // name of mapping object

                if(hMapFile == NULL)
                {
                    switch(GetLastError())
                    {
                    case ERROR_FILE_NOT_FOUND:
                        QMessageBox::warning(this, tr("Error"),
                        tr("SMBX with LunaDLL is not running!"),
                        QMessageBox::Ok);
                        break;
                    default:
                        DWORD   dwLastError = GetLastError();
                        TCHAR   lpBuffer[256] = L"?";
                        if(dwLastError != 0)    // Don't want to see a "operation done successfully" error ;-)
                            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,                 // It´s a system error
                                             NULL,                                      // No string to be formatted needed
                                             dwLastError,                               // Hey Windows: Please explain this error!
                                             MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),  // Do it in the standard language
                                             lpBuffer,              // Put the message here
                                             255,                     // Number of bytes to store the message
                                             NULL);
                        QMessageBox::warning(this, tr("Error"),
                        tr("Fail to send file patth into LunaDLL: (%1)").arg(QString::fromWCharArray(lpBuffer)),
                        QMessageBox::Ok);
                    }
                    return;
                }

                pBuf = (wchar_t*)MapViewOfFile(hMapFile,   // handle to map object
                                        FILE_MAP_ALL_ACCESS, // read/write permission
                                        0,
                                        0,
                                        15360 );

                if(pBuf == NULL)
                {
                    QMessageBox::warning(this, tr("Error"),
                    tr("Could not map view of file (%1).").arg(GetLastError()),
                    QMessageBox::Ok);
                    CloseHandle(hMapFile);
                    return;
                }

                if(fullPathToLevel.size()*sizeof(wchar_t) > 15360)
                {
                    QMessageBox::warning(this, tr("Error"),
                    tr("Too long path: ").arg(fullPathToLevel),
                    QMessageBox::Ok);
                    UnmapViewOfFile(pBuf);
                    CloseHandle(hMapFile);
                    return;
                }

                qDebug()<< "Attempt to start testing of " << fullPathToLevel << "via SMBX";

                std::wstring str = fullPathToLevel.toStdWString();
                ZeroMemory((PVOID)pBuf, 15360);
                CopyMemory((PVOID)pBuf, str.c_str(), str.size()*sizeof(wchar_t));

                UnmapViewOfFile(pBuf);
                CloseHandle(hMapFile);
                /****************Write file path into shared memory****end*******************/


                if(DevConsole::isConsoleShown())
                    DevConsole::log("Sent Message (Hopefully it worked)");

                //Stop music playback in the PGE!
                on_actionPlayMusic_triggered(false);
                setMusicButton(false);
                PGE_MusPlayer::MUS_stopMusic();

                //Minimize PGE Editor
                if(qApp->desktop()->screenCount()==1) // Minimize editor window if alone screen was found
                    this->showMinimized();

                //Send command and restore window
                SetForegroundWindow(smbxWind);
                ShowWindow(smbxWind, SW_MAXIMIZE);
                SetFocus(smbxWind);
                SendMessageA(smbxWind, WM_COPYDATA, (WPARAM)this->winId(), (LPARAM)cds);
            }
            else
            {
                if(DevConsole::isConsoleShown())
                    DevConsole::log(tr("Failed to find SMBX Window"));
                else
                    QMessageBox::warning(this, tr("Error"),
                    tr("Failed to find SMBX Window"),
                    QMessageBox::Ok);
            }
        }
        #endif//SHARED_FILEPATH_SENDING
    }
    #endif//DUMMY_WORLD_GENERATION
#else
    DevConsole::log("This feature requires Microsoft(R) Windows OS!");
#endif

}


