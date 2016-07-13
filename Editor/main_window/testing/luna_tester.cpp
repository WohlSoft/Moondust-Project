/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

/*
    LunaTester provides ability to run level tests in the LunaLUA-SMBX.
    However it works on Windows OS only. To implement similar thing on other OS,
    communication with Wine is required.
*/

#ifdef _WIN32
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include "luna_tester.h"

#include <sstream>
#include <windows.h>
#include <QThread>
#include <cstring>
#include <QDesktopWidget>
#include <QDirIterator>
#include <QJsonObject>
#include <QMetaObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "luna_tester.h"

#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <PGE_File_Formats/file_formats.h>
#include <dev_console/devconsole.h>
#include <main_window/global_settings.h>

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
                                      LunaTester &luna)
{
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    memset(&luna.m_pi, 0, sizeof(luna.m_pi));

    {
        SECURITY_ATTRIBUTES sa;
        // Set up the security attributes struct.
        sa.nLength= sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        if( ! CreatePipe( &luna.m_ipc_pipe_out_i, &luna.m_ipc_pipe_out, &sa, 0))
        {
            luna.m_ipc_pipe_out=0;
            luna.m_ipc_pipe_out_i=0;
        }
        if( ! CreatePipe( &luna.m_ipc_pipe_in,  &luna.m_ipc_pipe_in_o, &sa, 0))
        {
            luna.m_ipc_pipe_in=0;
            luna.m_ipc_pipe_in_o=0;
        }
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput   = luna.m_ipc_pipe_out_i;
        si.hStdOutput  = luna.m_ipc_pipe_in_o;
        si.cb = sizeof(si);
        // Don't let child process take the write handle here
        SetHandleInformation(luna.m_ipc_pipe_out, HANDLE_FLAG_INHERIT, 0);
        // Don't let child process take the read handle here
        SetHandleInformation(luna.m_ipc_pipe_in, HANDLE_FLAG_INHERIT, 0);
    }

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
        TRUE,             // Set handle inheritance to FALSE
        CREATE_SUSPENDED, // Create in suspended state
        NULL,             // Use parent's environment block
        workingDir,       // Use parent's starting directory
        &si,              // Pointer to STARTUPINFO structure
        &luna.m_pi)              // Pointer to PROCESS_INFORMATION structure
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
        luna.m_pi.hProcess,           // Target process
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
    if (WriteProcessMemory(luna.m_pi.hProcess, (void*)LoaderPatchAddr1, LoaderPatch1, sizeof(LoaderPatch1), NULL) == 0 ||
        WriteProcessMemory(luna.m_pi.hProcess, (void*)LoaderPatchAddr2, LoaderPatch2, sizeof(LoaderPatch2), NULL) == 0)
    {
        return LUNALOADER_PATCH_FAIL;
    }

    // Change Patch2 memory protection type
    DWORD TmpDword = 0;
    if (VirtualProtectEx(
        luna.m_pi.hProcess,
        (void*)LoaderPatchAddr2,
        sizeof(LoaderPatch2),
        PAGE_EXECUTE,
        &TmpDword
    ) == 0) {
        return LUNALOADER_PATCH_FAIL;
    }

    // Resume the main program thread
    ResumeThread(luna.m_pi.hThread);

    // Close handles
    CloseHandle(luna.m_ipc_pipe_out_i);
    CloseHandle(luna.m_ipc_pipe_in_o);
    CloseHandle(luna.m_pi.hThread);
    luna.m_ipc_pipe_out_i = 0;
    luna.m_ipc_pipe_in_o  = 0;
    //CloseHandle(pi.hProcess); //Don't close it because needed to catch already-running SMBX Engine

    return LUNALOADER_OK;
}

static bool SMBXEditorIsStarted()
{
    HWND smbxWind = FindWindowA("ThunderRT6MDIForm", NULL);
    return (smbxWind);
}

static std::string ReadMsgString(HANDLE hInputRead)
{
    // Note: This is not written to be particularly efficient right now. Just
    //       readable enough and safe.
    if (hInputRead == 0)
        return "";
    char c;
    std::vector<char> data;
    while (1)
    {
        data.clear();
        // Read until : delimiter
        bool err = false;
        while (1)
        {
            DWORD bytesRead;
            ReadFile(hInputRead, &c, 1, &bytesRead, NULL);
            if (bytesRead != 1)
            {
                return "";
            }
            if (c == ':') break;
            if ((c > '9') || (c < '0'))
            {
                err = true;
                break;
            }
            data.push_back(c);
        }
        if (err) continue;
        std::string byteCountStr = std::string(&data[0], data.size());
        data.clear();
        // Interpret as number
        int byteCount = std::stoi(byteCountStr);
        if (byteCount <= 0) continue;
        int byteCursor = 0;
        data.resize(byteCount);
        while (byteCursor < byteCount)
        {
            DWORD bytesRead;
            ReadFile(hInputRead, &data[byteCursor], 1, &bytesRead, NULL);
            if (bytesRead == 0)
            {
                return "";
            }
            byteCursor += bytesRead;
        }
        // Get following comma
        {
            DWORD bytesRead;
            ReadFile(hInputRead, &c, 1, &bytesRead, NULL);
            if (bytesRead != 1) {
                return "";
            }
            if (c != ',')
            {
                continue;
            }
        }
        return std::string(&data[0], data.size());
    }
}

static bool SendLevelDataToLunaLuaSMBX(LevelEdit* ed, HANDLE hInputWrite, HANDLE hInputRead)
{
    //{"jsonrpc": "2.0", "method": "testLevel", "params":
    //   {"filename": "myEpisode/test.lvl", "leveldata": "<RAW SMBX64 LEVEL DATA>", "players": [{"character": 1}]},
    //"id": 3}

    //"players": [ { "character": 1, "powerup": 1, "mountType": 1, "mountColor": 1 }, { "character": 2, "powerup": 1, "mountType": 1, "mountColor": 1 } ]

    if( (hInputWrite == 0) || (ed == NULL) )
    {
        return false;
    }

    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"] = "2.0";
    jsonObj["method"] = "testLevel";

        QJsonObject JSONparams;

        if( !ed->isUntitled )
            JSONparams["filename"] = ed->curFile;
        else
            JSONparams["filename"] = ApplicationPath+"/worlds/untitled.lvl";

        QJsonArray JSONPlayers;
        QJsonObject JSONPlayer1, JSONPlayer2;
            SETTINGS_TestSettings  t = GlobalSettings::testing;
            JSONPlayer1["character"] = t.p1_char;
            JSONPlayer1["powerup"]   = t.p1_state;
            JSONPlayer2["character"] = t.p2_char;
            JSONPlayer2["powerup"]   = t.p2_state;

            JSONPlayers.push_back(JSONPlayer1);
        if(t.numOfPlayers>1)
            JSONPlayers.push_back(JSONPlayer2);

        JSONparams["players"] = JSONPlayers;

        QString LVLRawData;
        //To don't affect level data state, need to remember recently used file format and version identifier
        {
            int recentFormatBackup  = ed->LvlData.RecentFormat;
            int recentFormatVBackup = ed->LvlData.RecentFormatVersion;

            //Generate actual SMBX64 Level file data
            //TODO:  Maybe we should undo that earlier optimization, and copy LvlData after all?
            //       We do need to call smbx64LevelPrepare after all...
            //Reply: that is required because unordered blocks and BGOs are will produce very vired result:
            //       - unordered blocks in array are will cause NPCs fall through blocks like no blocks
            //       - unordered BGOs in array are will result clash
            //       And yea, only doing optimization on LunaLUA-side is right solution to don't do prepare data work here
            FileFormats::smbx64LevelPrepare(ed->LvlData);
            FileFormats::WriteSMBX64LvlFileRaw(ed->LvlData, LVLRawData, 64);
            ed->LvlData.RecentFormat = recentFormatBackup;
            ed->LvlData.RecentFormatVersion = recentFormatVBackup;
        }
        //Set CRLF
        LVLRawData.replace("\n", "\r\n");
        //Store data into JSON
        JSONparams["leveldata"] = LVLRawData;

    jsonObj["params"] = JSONparams;
    jsonObj["id"] = 3;
    jsonOut.setObject(jsonObj);
    QByteArray outputJSON = jsonOut.toJson();

    //Converting JSON data into net string
    std::string dataToSend = outputJSON.toStdString();
    int len = dataToSend.size();
    std::string len_std = std::to_string(len) + ":";
    dataToSend.insert(0, len_std.c_str(), len_std.size());
    dataToSend.push_back(',');

    DWORD writtenBytes=0;

    //Send data to SMBX
    if(WriteFile(hInputWrite,
              (LPCVOID)dataToSend.c_str(),
              (DWORD)dataToSend.size(),
              &writtenBytes, NULL)==TRUE)
    {
        //Read result from level testing run
        std::string resultMsg = ReadMsgString(hInputRead);
        LogDebugQD(QString("Received from SMBX JSON message: %1").arg(QString::fromStdString(resultMsg)));
        return true;
    }
    return false;
}

static bool switchToSmbxWindow(SafeMsgBoxInterface &msg, HANDLE hInputWrite, HANDLE hInputRead)
{
    if( (hInputWrite == 0) || (hInputRead == 0) )
    {
        return false;
    }

    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"]  = "2.0";
    jsonObj["method"]   = "getWindowHandle";
    QJsonObject JSONparams;
    JSONparams["xxx"] = 0;
    jsonObj["params"] = JSONparams;
    jsonObj["id"] = 3;
    jsonOut.setObject(jsonObj);
    QByteArray outputJSON = jsonOut.toJson();

    //Converting JSON data into net string
    std::string dataToSend = outputJSON.toStdString();
    int len = dataToSend.size();
    std::stringstream outString;
    outString << std::to_string(len) << ":" << dataToSend << ',';
    dataToSend = outString.str();

    DWORD writtenBytes=0;
    //Send data to SMBX
    if(WriteFile(hInputWrite,
              (LPCVOID)dataToSend.c_str(),
              (DWORD)dataToSend.size(),
              &writtenBytes, NULL)==TRUE)
    {
        std::string inMessage = ReadMsgString(hInputRead);
        QByteArray jsonData(inMessage.c_str(), inMessage.size());
        QJsonParseError err;
        QJsonDocument jsonOut = QJsonDocument::fromJson(jsonData, &err);
        if(err.error == QJsonParseError::NoError)
        {
            QJsonObject obj = jsonOut.object();
            if(obj["error"].isNull())
            {
                unsigned long smbxHwnd = ulong(obj["result"].toVariant().toULongLong());
                HWND wSmbx = HWND(smbxHwnd);
                //msg.warning("Test2", QString("Received: %1, from: %2").arg(smbxHwnd).arg(QString::fromStdString(inMessage)), QMessageBox::Ok);
                //wchar_t title[MAX_PATH];
                //GetWindowTextW(wSmbx, title, MAX_PATH);
                //msg.warning("Test2", QString("Window title is: %1").arg(QString::fromWCharArray(title)), QMessageBox::Ok);
                ShowWindow(wSmbx, SW_SHOW);
                SetForegroundWindow(wSmbx);
                SetActiveWindow(wSmbx);
                SetFocus(wSmbx);
            } else {
                msg.warning("Test2", QString::fromStdString(inMessage), QMessageBox::Ok);
            }
        } else {
            msg.warning("ERROR!", err.errorString()+"\n\nData:\n"+QString::fromStdString(inMessage), QMessageBox::Warning);
        }
        return true;
    }
    return false;
}

class QThreadPointNuller
{
    QThread**pointer;
public:
    QThreadPointNuller(QThread ** ptr) : pointer(ptr) {}
    ~QThreadPointNuller() {*pointer = nullptr;}
};

void MainWindow::_RunSmbxTestHelper()
{
    QMutexLocker mlocker(&engine_mutex);
    Q_UNUSED(mlocker);
    QThreadPointNuller tlocker(&m_luna->m_helperThread);
    Q_UNUSED(tlocker);

    SafeMsgBoxInterface msg(&m_messageBoxer);

    m_luna->m_helperThread = QThread::currentThread();

    if( activeChildWindow() == 1 )
    {
        // Is SMBX Editor already running. If running,
        // do old method - send path to already running editor
        if( !SMBXEditorIsStarted() )
        {
            QString smbxPath = ConfStatus::configDataPath;
            if(!QFile(smbxPath+ConfStatus::SmbxEXE_Name).exists())
            {
                msg.warning(tr("SMBX Directory wasn't configured right"),
                            tr("%1 not found!\nTo run testing via SMBX you should have right SMBX Integration configuration package!")
                                     .arg(smbxPath+ConfStatus::SmbxEXE_Name),
                QMessageBox::Ok);
                return;
            }

            DWORD lpExitCode = 0;
            if(GetExitCodeProcess(m_luna->m_pi.hProcess, &lpExitCode))
            {
                if(lpExitCode==STILL_ACTIVE)
                {
                    if( m_luna->m_ipc_pipe_out != 0 )
                    {
                        LevelEdit* ed = activeLvlEditWin();
                        if(!ed)
                        {
                            msg.critical("Internal error",
                                         "Can't start level testing, because activeLvlEditWin() returned NULL",
                                         QMessageBox::Ok);
                            return;
                        }

                        if ( SendLevelDataToLunaLuaSMBX(ed, m_luna->m_ipc_pipe_out, m_luna->m_ipc_pipe_in) )
                        {
                            //Stop music playback in the PGE Editor!
                            QMetaObject::invokeMethod(this, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
                            // not sure how efficient it is
                            QMetaObject::invokeMethod(this, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
                            //Attempt to switch SMBX Window
                            switchToSmbxWindow(msg, m_luna->m_ipc_pipe_out, m_luna->m_ipc_pipe_in);
                        }
                    }
                    else if( msg.warning(tr("SMBX Test is already runned"),
                             tr("SMBX Engine is already testing another level.\n"
                                "Do you want to abort current testing process?"),
                             QMessageBox::Abort|QMessageBox::Cancel) == QMessageBox::Abort)
                    {
                        WaitForSingleObject(m_luna->m_pi.hProcess, 100);
                        TerminateProcess(m_luna->m_pi.hProcess, lpExitCode);
                        CloseHandle(m_luna->m_pi.hProcess);
                    }
                    return;
                }
            }

            QString command = smbxPath+ConfStatus::SmbxEXE_Name;
            QStringList params;

            if( !QFile(smbxPath+"LunaDll.dll").exists() )
            {
                /**********************************************
                 *****Do Vanilla test with dummy episode!******
                 **********************************************/
                {
                    int msgRet = msg.richBox(tr("Vanilla SMBX detected!"),
                                tr("%2 not found!\nYou have a Vanilla SMBX!<br>\n"
                       "That means, impossible to launch level testing automatically. "
                       "To launch a level testing, will be generated a dummy episode which you can "
                       "start and select manually.<br>\n<br>\n"
                       "Name of episode to generate: %1<br>\n<br>\n"
                       "Are you still want to launch a test?<br>\n<br>\n"
                       "If you want to have a full featured level testing, you need to get a LunaLUA here:<br>\n"
                       "%3")
                             .arg("\"_temp_episode_pge\"")
                             .arg(smbxPath+"LunaDll.dll")
                             .arg("<a href=\"http://wohlsoft.ru/LunaLua/\">http://wohlsoft.ru/LunaLua/</a>"),
                                QMessageBox::Yes|QMessageBox::No, QMessageBox::Warning);

                    if(msgRet != QMessageBox::Yes )
                        return;
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

                WorldData worldmap;
                FileFormats::CreateWorldData(worldmap);
                worldmap.EpisodeTitle = "_temp_episode_pge";
                worldmap.IntroLevel_file = "templevel.lvl";
                worldmap.restartlevel = true;

                if(!FileFormats::WriteSMBX64WldFileF(newEpisode+"/tempworld.wld", worldmap, 64))
                {
                    msg.warning(tr("File save error"),
                                tr("Cannot save file %1:\n%2.")
                                 .arg(newEpisode+"/tempworld.wld")
                                 .arg(FileFormats::errorString),
                                QMessageBox::Ok);
                    return;
                }

                QProcess::startDetached(command, params, smbxPath);
                //Stop music playback in the PGE Editor!
                QMetaObject::invokeMethod(this, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
                // not sure how efficient it is
                QMetaObject::invokeMethod(this, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
                return;

            } else {
            /**********************************************
             **********Do LunaLUA testing launch!**********
             **********************************************/
                //params << "--patch";
                params << "--hideOnCloseIPC";

                LevelEdit* ed = activeLvlEditWin();
                if(!ed)
                {
                    msg.critical("Internal error",
                                 "Can't start level testing, because activeLvlEditWin() returned NULL",
                                 QMessageBox::Ok);
                    return;
                }

                QString argString;
                for (int i=0; i<params.length(); i++)
                {
                    if (i > 0)
                    {
                        argString += " ";
                    }
                    argString += params[i];
                }

                // Make sure any old pipe handle is closed
                if(m_luna->m_ipc_pipe_out)
                {
                    CloseHandle(m_luna->m_ipc_pipe_out);
                    m_luna->m_ipc_pipe_out = 0;
                }
                if(m_luna->m_ipc_pipe_in)
                {
                    CloseHandle(m_luna->m_ipc_pipe_in);
                    m_luna->m_ipc_pipe_in = 0;
                }

                LunaLoaderResult res = LunaLoaderRun(command.toStdWString().c_str(),
                                                     argString.toStdWString().c_str(),
                                                     smbxPath.toStdWString().c_str(),
                                                     *m_luna );

                if(res == LUNALOADER_OK)
                {
                    if(SendLevelDataToLunaLuaSMBX(ed, m_luna->m_ipc_pipe_out, m_luna->m_ipc_pipe_in))
                    {
                        GlobalSettings::recentMusicPlayingState = ui->actionPlayMusic->isChecked();
                        //Stop music playback in the PGE Editor!
                        QMetaObject::invokeMethod(this, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
                        QMetaObject::invokeMethod(this, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
                    } else {
                        msg.warning(tr("LunaTester error"),
                        tr("Failed to send level into LunaLUA-SMBX!"),
                        QMessageBox::Ok);
                    }
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

                    msg.warning(tr("LunaTester error"),
                    tr("Impossible to launch SMBX Engine, because %1").arg(luna_error),
                    QMessageBox::Ok);
                }
            }//Do LunaLUA direct testing launch
        }
        else //Attempt to attach into running SMBX Editor
        {
        /**********************************************
         ******Do LunaLUA in-editor test launch!*******
         **********************************************/
            if(activeLvlEditWin()->isUntitled)
            {
                msg.warning(tr("Save file first"),
                tr("To run testing via SMBX file must be saved into disk first!"),
                QMessageBox::Ok);
                return;
            }
            QString fullPathToLevel= activeLvlEditWin()->curFile;
            if(!activeLvlEditWin()->LvlData.smbx64strict)
            {
                int ret = msg.warning(tr("Incompatible file format"),
                            tr("To take able to test level in the SMBX, file should be saved into SMBX64 format!\n"
                               "Will be created a temporary file. Do you want to continue?"),
                                QMessageBox::Yes|QMessageBox::Abort);

                if( ret == QMessageBox::Abort )
                    return;

                //Double point will be unique and will don't overwrite your lvl file, but will use same custom folder
                QString newPath = activeLvlEditWin()->LvlData.path+"/"+activeLvlEditWin()->LvlData.filename+"..lvl";
                if(!activeLvlEditWin()->saveSMBX64LVL(newPath, true))
                {
                    msg.warning(tr("Error"),
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
                fullPathToLevel.replace('/', '\\');

                if(activeLvlEditWin()->LvlData.modified)
                {
                    int ret = msg.question(
                            tr("SMBX Level test"),
                            tr("Do you wanna to save file before start testing?\n"),
                            QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
                    if(ret==QMessageBox::Cancel)
                        return;
                    else
                    if(ret==QMessageBox::Yes)
                        QMetaObject::invokeMethod(activeLvlEditWin(), "doSave", Qt::QueuedConnection);
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
                        msg.warning(tr("Error"),
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
                        msg.warning(tr("Error"),
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
                    msg.warning(tr("Error"),
                    tr("Could not map view of file (%1).").arg(GetLastError()),
                    QMessageBox::Ok);
                    CloseHandle(hMapFile);
                    return;
                }

                if(fullPathToLevel.size()*sizeof(wchar_t) > 15360)
                {
                    msg.warning(tr("Error"),
                    tr("Too long path: ").arg(fullPathToLevel),
                    QMessageBox::Ok);
                    UnmapViewOfFile(pBuf);
                    CloseHandle(hMapFile);
                    return;
                }

                LogDebugQD("Attempt to start testing of " + fullPathToLevel + "via SMBX");

                std::wstring str = fullPathToLevel.toStdWString();
                ZeroMemory((PVOID)pBuf, 15360);
                CopyMemory((PVOID)pBuf, str.c_str(), str.size()*sizeof(wchar_t));

                UnmapViewOfFile(pBuf);
                CloseHandle(hMapFile);
                /****************Write file path into shared memory****end*******************/

                LogDebugQD("Sent Message (Hopefully it worked)");

                GlobalSettings::recentMusicPlayingState = ui->actionPlayMusic->isChecked();
                //Stop music playback in the PGE Editor!
                QMetaObject::invokeMethod(this, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
                QMetaObject::invokeMethod(this, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));

                //Minimize PGE Editor
                if(qApp->desktop()->screenCount()==1) // Minimize editor window if alone screen was found
                {
                    QMetaObject::invokeMethod(this, "showMinimized", Qt::QueuedConnection);
                }

                //Send command and restore window
                SetForegroundWindow(smbxWind);
                ShowWindow(smbxWind, SW_MAXIMIZE);
                SetFocus(smbxWind);
                SendMessageA(smbxWind, WM_COPYDATA, (WPARAM)this->winId(), (LPARAM)cds);
            }
            else
            {
                msg.warning(tr("Error"),
                            tr("Failed to find SMBX Window"),
                            QMessageBox::Ok);
            }
        }
    }//Is this a level edit window?
}

#endif // LUNA_TESTER_H
