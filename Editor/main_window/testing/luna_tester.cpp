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
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QtConcurrentRun>
#include "luna_tester.h"

#include <PGE_File_Formats/file_formats.h>
#include <data_functions/smbx64_validation_messages.h>
#include <common_features/app_path.h>
#include <common_features/logger.h>
#include <dev_console/devconsole.h>
#include <main_window/global_settings.h>

#ifdef _WIN64
#define USE_LUNAHEXER
#endif

class QThreadPointNuller
{
    QThread **pointer;
public:
    QThreadPointNuller(QThread **ptr) : pointer(ptr) {}
    ~QThreadPointNuller()
    {
        *pointer = nullptr;
    }
};

static std::string ReadMsgString(HANDLE hInputRead);

LunaTester::LunaTester() :
    QObject(nullptr),
    m_mw(nullptr),
    m_pi{0, 0, 0, 0},
    m_ipc_pipe_out(0),
    m_ipc_pipe_out_i(0),
    m_ipc_pipe_in(0),
    m_ipc_pipe_in_o(0),
    m_helperThread(0),
    m_noGL(false)
{}

LunaTester::~LunaTester()
{
    DWORD lpExitCode = 0;
    if(GetExitCodeProcess(m_pi.hProcess, &lpExitCode))
    {
        if(lpExitCode == STILL_ACTIVE)
        {
            WaitForSingleObject(m_pi.hProcess, 100);
            TerminateProcess(m_pi.hProcess, lpExitCode);
            CloseHandle(m_pi.hProcess);
        }
    }
    if(m_ipc_pipe_out)
    {
        CloseHandle(m_ipc_pipe_out);
        m_ipc_pipe_out = 0;
    }
    if(m_ipc_pipe_in)
    {
        CloseHandle(m_ipc_pipe_in);
        m_ipc_pipe_in = 0;
    }
}

void LunaTester::initLunaMenu(MainWindow *mw,
                              QMenu *mainmenu,
                              QAction *insert_before,
                              QAction *defaultTestAction,
                              QAction *secondaryTestAction,
                              QAction *startEngineAction)
{
    m_mw = mw;
    QIcon lunaIcon(":/lunalua.ico");
    QMenu *lunaMenu = mainmenu->addMenu(lunaIcon, "LunaTester");
    mainmenu->insertMenu(insert_before, lunaMenu);

    size_t menuItemId = 0;
    QAction *RunLunaTest;
    {
        RunLunaTest = lunaMenu->addAction("runTesting");
        mw->connect(RunLunaTest,    &QAction::triggered,
                    this,           &LunaTester::startLunaTester,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = RunLunaTest;
    }
    QAction *ResetCheckPoints = lunaMenu->addAction("resetCheckpoints");
    {
        mw->connect(ResetCheckPoints,   &QAction::triggered,
                    this,               &LunaTester::resetCheckPoints,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = ResetCheckPoints;
    }
    {
        QAction *disableOpenGL = lunaMenu->addAction("Disable OpenGL");
        disableOpenGL->setCheckable(true);
        disableOpenGL->setChecked(m_noGL);
        mw->connect(disableOpenGL,   &QAction::toggled,
                    [this](bool state)
        {
            m_noGL = state;
        });
        m_menuItems[menuItemId++] = disableOpenGL;
    }
    QAction *enableKeepingInBackground;
    {
        enableKeepingInBackground = lunaMenu->addAction("enableKeepingInBackground");
        enableKeepingInBackground->setCheckable(true);
        enableKeepingInBackground->setChecked(!m_killPreviousSession);
        mw->connect(enableKeepingInBackground,   &QAction::toggled,
                    [this](bool state)
        {
            m_killPreviousSession = !state;
        });
        m_menuItems[menuItemId++] = enableKeepingInBackground;
    }
    {
        QAction *KillFrozenThread = lunaMenu->addAction("Termitate frozen loader");
        mw->connect(KillFrozenThread,   &QAction::triggered,
                    this,               &LunaTester::killFrozenThread,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = KillFrozenThread;
    }
    {
        lunaMenu->addSeparator();
        QAction *runLegacyEngine = lunaMenu->addAction("startLegacyEngine");
        mw->connect(runLegacyEngine,   &QAction::triggered,
                    this,              &LunaTester::lunaRunGame,
                    Qt::QueuedConnection);
        m_menuItems[menuItemId++] = runLegacyEngine;
    }

    QAction *sep = lunaMenu->addSeparator();
    mainmenu->insertAction(insert_before, sep);

    retranslateMenu();
    connect(mw, &MainWindow::languageSwitched, this, &LunaTester::retranslateMenu);

    if(ConfStatus::SmbxTest_By_Default)
    {
        defaultTestAction->setShortcut(QStringLiteral(""));
        defaultTestAction->setShortcutContext(Qt::WindowShortcut);
        QIcon pgeEngine;
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_16.png"));
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_32.png"));
        pgeEngine.addPixmap(QPixmap(":/images/cat/cat_48.png"));

        mainmenu->insertAction(defaultTestAction, RunLunaTest);

        QMenu *pgeEngineMenu = mainmenu->addMenu(pgeEngine, "PGE Engine");
        mainmenu->insertMenu(lunaMenu->menuAction(), pgeEngineMenu);
        mainmenu->removeAction(defaultTestAction);
        mainmenu->removeAction(secondaryTestAction);
        mainmenu->removeAction(startEngineAction);

        pgeEngineMenu->insertAction(nullptr, defaultTestAction);
        pgeEngineMenu->insertAction(nullptr, secondaryTestAction);
        pgeEngineMenu->addSeparator();
        pgeEngineMenu->insertAction(nullptr, startEngineAction);

        RunLunaTest->setShortcut(QStringLiteral("F5"));
        RunLunaTest->setShortcutContext(Qt::WindowShortcut);
        RunLunaTest->setIcon(lunaIcon);

        ResetCheckPoints->setShortcut(QStringLiteral("Ctrl+F5"));
        ResetCheckPoints->setShortcutContext(Qt::WindowShortcut);
    }
}

void LunaTester::retranslateMenu()
{
    size_t menuItemId = 0;
    {
        QAction *RunLunaTest = m_menuItems[menuItemId++];
        RunLunaTest->setText(tr("Test level",
                                "Run the LunaTester based level testing."));
        RunLunaTest->setToolTip(tr("Starts level testing in the legacy engine.\n"
                                   "To have this feature work, latest LunaLUA must be installed.\n"
                                   "Otherwise, it will be very limited."));
    }
    {
        QAction *ResetCheckPoints = m_menuItems[menuItemId++];
        ResetCheckPoints->setText(tr("Reset checkpoints"));
        ResetCheckPoints->setToolTip(tr("Reset all checkpoint states to initial state."));
    }
    {
        QAction *disableOpenGL = m_menuItems[menuItemId++];
        disableOpenGL->setText(tr("Disable OpenGL", "Disable OpenGL on LunaTester side"));
        disableOpenGL->setToolTip(tr("Disable OpenGL rendering engine and use the GDI. "
                                     "Useful if your video card does not support OpenGL or "
                                     "LunaLua is crashing on the attempt to use it."));
    }
    {
        QAction *enableKeepingInBackground = m_menuItems[menuItemId++];
        enableKeepingInBackground->setText(tr("Keep running in background",
                                              "Keep Legacy Engine be running in background to speed-up testing starts after first launch."));
        enableKeepingInBackground->setToolTip(tr("Allows to start level testing very fast after first launch.\n"
                                                 "Requires powerful computer, otherwise engine will freeze on next test launch.\n"
                                                 "Suggested to disable this feature on slow machines or if any troubles are happens while attempts to run a testing."));
    }
    {
        QAction *KillFrozenThread = m_menuItems[menuItemId++];
        KillFrozenThread->setText(tr("Termitate frozen loader",
                                     "Terminite frozen LunaTester on the attempt to send any command to LunaLua."));
        KillFrozenThread->setToolTip(tr("Termiates frozen thread to allow you to run a test again."));
    }
    {
        QAction *runLegacyEngine = m_menuItems[menuItemId++];
        runLegacyEngine->setText(tr("Start Legacy Engine",
                                    "Launch legacy engine in game mode"));
        runLegacyEngine->setToolTip(tr("Launch legacy engine in game mode."));
    }
}

void LunaTester::killEngine()
{
    DWORD lpExitCode = 0;

    //Abort engine staying in background
    if(GetExitCodeProcess(this->m_pi.hProcess, &lpExitCode))
    {
        if(lpExitCode == STILL_ACTIVE)
        {
            WaitForSingleObject(this->m_pi.hProcess, 100);
            TerminateProcess(this->m_pi.hProcess, lpExitCode);
            CloseHandle(this->m_pi.hProcess);
            this->m_pi.hProcess = 0;
        }
    }

    // Make sure any old pipe handle is closed
    if(this->m_ipc_pipe_out)
    {
        CloseHandle(this->m_ipc_pipe_out);
        this->m_ipc_pipe_out = 0;
    }
    if(this->m_ipc_pipe_in)
    {
        CloseHandle(this->m_ipc_pipe_in);
        this->m_ipc_pipe_in = 0;
    }
}

/*****************************************Menu slots*************************************************/

inline void busyThreadBox(MainWindow *mw)
{
    QMessageBox::warning(mw,
                         "LunaTester",
                         LunaTester::tr("LunaLUA test loader thread is busy, try again or try termiate frozen loader!"),
                         QMessageBox::Ok);
}

/*!
 * \brief Starts testing in the hacked with LunaLUA-SMBX Engine if possible (Only for Windows builds)
 */
void LunaTester::startLunaTester()
{
    if(m_helper.isRunning())
        busyThreadBox(m_mw);
    else
    {
        if(m_mw->activeChildWindow() == MainWindow::WND_Level)
        {
            LevelEdit *lvl = m_mw->activeLvlEditWin();
            if(lvl)
            {
                if(m_killPreviousSession) killEngine();
                m_helper = QtConcurrent::run(this,
                                             &LunaTester::lunaRunnerThread,
                                             lvl->LvlData,
                                             lvl->curFile,
                                             lvl->isUntitled);
            }
        }
        else
        {
            QMessageBox::information(m_mw,
                                     "LunaTester",
                                     LunaTester::tr("LunaTester can't be used on world map because the legacy engine "
                                                    "doesn't provide ability to run testing of world maps. "
                                                    "Instead, you can use PGE Engine (\"Test\" -> \"Test saved file\" "
                                                    "menu item) to test your world map file in action without episode running."),
                                     QMessageBox::Ok);
        }
    }
}

void LunaTester::resetCheckPoints()
{
    if(m_helper.isRunning())
        busyThreadBox(m_mw);
    else
        m_helper = QtConcurrent::run(this, &LunaTester::lunaChkResetThread);
}

void LunaTester::killFrozenThread()
{
    if(m_helper.isRunning())
    {
        QMessageBox::StandardButton reply = QMessageBox::warning(m_mw,
                                            "LunaTester",
                                            tr("Are you really want to termitate loader thread?"),
                                            QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes)
        {
            DWORD lpExitCode = 0;
            if(GetExitCodeProcess(m_pi.hProcess, &lpExitCode))
            {
                if(lpExitCode == STILL_ACTIVE)
                {
                    WaitForSingleObject(m_pi.hProcess, 0);
                    TerminateProcess(m_pi.hProcess, lpExitCode);
                    CloseHandle(m_pi.hProcess);
                }
            }
        }
    }
    else
    {
        QMessageBox::information(m_mw,
                                 "LunaTester",
                                 tr("Loader thread is not running.",
                                    "LunaTester thread now doing nothing.\n"
                                    "This message is shown on request to terminate "
                                    "frozen LunaTester while it is not "
                                    "actually frozen and responds."),
                                 QMessageBox::Ok);
    }
}



/*****************************************Private functions*************************************************/

static std::string ReadMsgString(HANDLE hInputRead)
{
    // Note: This is not written to be particularly efficient right now. Just
    //       readable enough and safe.
    if(hInputRead == 0)
        return "";
    char c;
    std::vector<char> data;
    while(1)
    {
        data.clear();
        // Read until : delimiter
        bool err = false;
        while(true)
        {
            DWORD bytesRead;
            ReadFile(hInputRead, &c, 1, &bytesRead, NULL);

            if(bytesRead != 1)
                return "";

            if(c == ':')
                break;

            if((c > '9') || (c < '0'))
            {
                err = true;
                break;
            }

            data.push_back(c);
        }

        if(err)
            continue;

        std::string byteCountStr = std::string(&data[0], data.size());
        data.clear();
        // Interpret as number
        int byteCount = std::stoi(byteCountStr);
        if(byteCount <= 0) continue;
        int byteCursor = 0;
        data.resize(byteCount);
        while(byteCursor < byteCount)
        {
            DWORD bytesRead;
            ReadFile(hInputRead, &data[byteCursor], 1, &bytesRead, NULL);
            if(bytesRead == 0)
                return "";
            byteCursor += bytesRead;
        }
        // Get following comma
        {
            DWORD bytesRead;
            ReadFile(hInputRead, &c, 1, &bytesRead, NULL);

            if(bytesRead != 1)
                return "";

            if(c != ',')
                continue;
        }
        return std::string(&data[0], data.size());
    }
}


static void patch(FILE *f, unsigned int at, void *data, unsigned int size)
{
    fseek(f, at, SEEK_SET);
    fwrite(data, 1, size, f);
}

static void patchAStr(FILE *f, unsigned int at, char *str, unsigned int maxlen)
{
    char *data = (char*)malloc(maxlen);
    memset(data, 0, maxlen);
    unsigned int i;
    unsigned int len = strlen(str);
    for(i = 0; (i < len) && (i < maxlen - 1); i++)
        data[i] = str[i];
    fseek(f, at, SEEK_SET);
    fwrite(data, 1, maxlen, f);
    free(data);
}

static void patchUStr(FILE *f, unsigned int at, char *str, unsigned int maxlen)
{
    char *data = (char*)malloc(maxlen);
    memset(data, 0, maxlen);
    unsigned int i, j;
    unsigned int len = strlen(str);
    for(i = 0, j = 0; (i < len) && (j < maxlen); i++, j += 2)
    {
        data[j] = str[i];
        data[j + 1] = 0;
    }
    fseek(f, at, SEEK_SET);
    fwrite(data, 1, maxlen, f);
    free(data);
}

static unsigned char lunaPatch[132] =
{
    0x1C, 0x40, 0x72, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x70, 0x42, 0x72, 0x00, 0x00, 0x10, 0x00, 0x00,
    0x79, 0x60, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x3C, 0x60, 0x74, 0x00, 0x69, 0x60, 0x74, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x4C, 0x75, 0x6E, 0x61, 0x44, 0x6C, 0x6C, 0x2E, 0x64, 0x6C,
    0x6C, 0x00, 0x00, 0x00, 0x48, 0x55, 0x44, 0x48, 0x6F, 0x6F,
    0x6B, 0x00, 0x00, 0x00, 0x4F, 0x6E, 0x4C, 0x76, 0x6C, 0x4C,
    0x6F, 0x61, 0x64, 0x00, 0x00, 0x00, 0x54, 0x65, 0x73, 0x74,
    0x46, 0x75, 0x6E, 0x63, 0x00, 0x48, 0x60, 0x74, 0x00, 0x52,
    0x60, 0x74, 0x00, 0x5E, 0x60, 0x74, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x48, 0x60, 0x74, 0x00, 0x52, 0x60, 0x74, 0x00, 0x5E,
    0x60, 0x74
};

#ifdef _WIN32
typedef wchar_t LUNACHAR;
#define LunaFOPEN _wfopen
#define LunaR L"rb"
#define LunaW L"wb"
#else
typedef char LUNACHAR;
#define LunaFOPEN fopen
#define LunaR "rb"
#define LunaW "wb"
#endif

static bool isPatched(const LUNACHAR *dstexe)
{
    FILE *test = LunaFOPEN(dstexe, LunaR);
    if(test)
    {
        fclose(test);
        return true;
    }
    return false;
}

static int patchSMBX(const LUNACHAR *srcexe, const LUNACHAR *dstexe)
{
    if(isPatched(dstexe))
        return 0; //Already patched!

    char ch = 0;
    char null[4096];
    memset(null, 0, 4096);
    FILE *src = LunaFOPEN(srcexe, LunaR);
    FILE *dst = LunaFOPEN(dstexe, LunaW);
    if(src == NULL)
        return 1;
    if(dst == NULL)
        return 2;
    //Copy original SMBX.EXE into the new file
    while(fread(&ch, 1, 1, src) == 1)
        fwrite(&ch, 1, 1, dst);
    fclose(src);

    patch(dst, 0xBE, (void *)"\x04", 1);
    patch(dst, 0x109, (void *)"\x70", 1);
    patch(dst, 0x138, (void *)"\x00\x60\x74\x00\x3C", 5);
    patch(dst, 0x188, (void *)"\x00\x00\x00\x00\x00", 5);
    patch(dst, 0x1D7, (void *)"\xE0", 1);
    patch(dst, 0x228, (void *)"\x2E\x4E\x65\x77\x49\x54\x00\x00\x89", 9);
    patch(dst, 0x235, (void *)"\x60\x74\x00\x00\x10\x00\x00\x00\xD0\x72\x00\x00\x00\x00\x00", 15);
    patch(dst, 0x24F, (void *)"\xC0", 1);
    patchUStr(dst, 0x27614, (char *)"LunaLUA-SMBX Version 1.3.0.2 http://wohlsoft.ru", 124);
    patchAStr(dst, 0x67F6A, (char *)"LunaLUA-SMBX Version 1.3.0.2 http://wohlsoft.ru", 63);
    patchAStr(dst, 0xA1FE3, (char *)"LunaLUA-SMBX Version 1.3.0.2 http://wohlsoft.ru", 78);
    patchAStr(dst, 0xC9FC0, (char *)"LunaLUA-SMBX Version 1.3.0.2 http://wohlsoft.ru", 65);

    patchUStr(dst, 0x31A34, (char *)"about:blank", 82); //Kill annoying web viewer!

    #ifdef OVERRIDE_VERSIONINFO_1301 //in Redigit's 1.3 exe version info offset is different
    patchUStr(dst, 0x72C584, (char *)"Hacked with LunaLUA", 46); //Comment
    patchUStr(dst, 0x72C5D4, (char *)"WohlSoft Team", 46); //Company
    patchUStr(dst, 0x72C62C, (char *)"www.wohlsoft.ru", 46); //File description
    patchUStr(dst, 0x72C680, (char *)"sucks!", 54); //Copyright
    patchUStr(dst, 0x72C6E4, (char *)"triple sucks!", 54); //Trade marks
    patchUStr(dst, 0x72C740, (char *)"LunaLUA-SMBX", 38); //Product name
    patchUStr(dst, 0x72C788, (char *)"1.3.0.2", 14); //Version 1
    patchUStr(dst, 0x72C7BC, (char *)"1.3.0.2", 14); //Version 2
    #endif
    patch(dst, 0x4CA23B, (void *)"\xFF\x15\x71\x60\xB4\x00", 6);
    patch(dst, 0x4D9446, (void *)"\xFF\x15\x6D\x60\xB4\x00\x90", 7);
    patch(dst, 0x56C030, (void *)"\xFF\x15\x69\x60\xB4\x00", 6);
    patch(dst, 0x72D000, null, 4096);
    patch(dst, 0x72D000, lunaPatch, 132);
    fclose(dst);

    return 0;
}


LunaTester::LunaLoaderResult LunaTester::LunaHexerRun(
    const wchar_t *pathToLegacyEngine,
    const wchar_t *cmdLineArgs,
    const wchar_t *workingDir)
{
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    memset(&m_pi, 0, sizeof(m_pi));
    {
        SECURITY_ATTRIBUTES sa;
        // Set up the security attributes struct.
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        if(! CreatePipe(&m_ipc_pipe_out_i, &m_ipc_pipe_out, &sa, 0))
        {
            m_ipc_pipe_out = 0;
            m_ipc_pipe_out_i = 0;
        }
        if(! CreatePipe(&m_ipc_pipe_in,  &m_ipc_pipe_in_o, &sa, 0))
        {
            m_ipc_pipe_in = 0;
            m_ipc_pipe_in_o = 0;
        }
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput   = m_ipc_pipe_out_i;
        si.hStdOutput  = m_ipc_pipe_in_o;
        si.cb = sizeof(si);
        // Don't let child process take the write handle here
        SetHandleInformation(m_ipc_pipe_out, HANDLE_FLAG_INHERIT, 0);
        // Don't let child process take the read handle here
        SetHandleInformation(m_ipc_pipe_in, HANDLE_FLAG_INHERIT, 0);
    }

    std::wstring newPath(pathToLegacyEngine);
    newPath.append(L".hexed");
    int err = patchSMBX(pathToLegacyEngine, newPath.c_str());

    if(err != 0)
    {
        CloseHandle(m_ipc_pipe_out_i);
        CloseHandle(m_ipc_pipe_in_o);
        return LUNALOADER_PATCH_FAIL;
    }

    // Prepare command line
    size_t pos = 0;
    std::wstring quotedPathToSMBX(newPath.c_str());
    while((pos = quotedPathToSMBX.find(L"\"", pos)) != std::string::npos)
    {
        quotedPathToSMBX.replace(pos, 1, L"\\\"");
        pos += 2;
    }
    std::wstring strCmdLine = (
                                  std::wstring(L"\"") + quotedPathToSMBX + std::wstring(L"\" ") +
                                  std::wstring(cmdLineArgs)
                              );
    uint32_t cmdLineMemoryLen = sizeof(wchar_t) * (strCmdLine.length() + 1); // Include null terminator
    wchar_t *cmdLine = (wchar_t *)malloc(cmdLineMemoryLen);
    std::memcpy(cmdLine, strCmdLine.c_str(), cmdLineMemoryLen);

    // Create process
    if(!CreateProcessW(newPath.c_str(),  // Launch legacy engine executable
                       cmdLine,          // Command line
                       NULL,             // Process handle not inheritable
                       NULL,             // Thread handle not inheritable
                       TRUE,             // Set handle inheritance to FALSE
                       0, //No flags
                       NULL,             // Use parent's environment block
                       workingDir,       // Use parent's starting directory
                       &si,              // Pointer to STARTUPINFO structure
                       &m_pi)              // Pointer to PROCESS_INFORMATION structure
      )
    {
        free(cmdLine);
        cmdLine = NULL;
        return LUNALOADER_CREATEPROCESS_FAIL;
    }
    free(cmdLine);
    cmdLine = NULL;

    // Close handles
    CloseHandle(m_ipc_pipe_out_i);
    CloseHandle(m_ipc_pipe_in_o);
    CloseHandle(m_pi.hThread);
    m_ipc_pipe_out_i = 0;
    m_ipc_pipe_in_o  = 0;

    return LUNALOADER_OK;
}



static inline void setJmpAddr(uint8_t *patch, DWORD patchAddr, DWORD patchOffset, intptr_t target)
{
    DWORD *dwordAddr = (DWORD *)&patch[patchOffset + 1];
    *dwordAddr = (DWORD)target - (DWORD)(patchAddr + patchOffset + 5);
}

LunaTester::LunaLoaderResult LunaTester::LunaLoaderRun(
    const wchar_t *pathToLegacyEngine,
    const wchar_t *cmdLineArgs,
    const wchar_t *workingDir)
{
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    memset(&m_pi, 0, sizeof(m_pi));

    {
        SECURITY_ATTRIBUTES sa;
        // Set up the security attributes struct.
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;

        if(! CreatePipe(&m_ipc_pipe_out_i, &m_ipc_pipe_out, &sa, 0))
        {
            m_ipc_pipe_out = 0;
            m_ipc_pipe_out_i = 0;
        }
        if(! CreatePipe(&m_ipc_pipe_in,  &m_ipc_pipe_in_o, &sa, 0))
        {
            m_ipc_pipe_in = 0;
            m_ipc_pipe_in_o = 0;
        }
        si.dwFlags = STARTF_USESTDHANDLES;
        si.hStdInput   = m_ipc_pipe_out_i;
        si.hStdOutput  = m_ipc_pipe_in_o;
        si.cb = sizeof(si);
        // Don't let child process take the write handle here
        SetHandleInformation(m_ipc_pipe_out, HANDLE_FLAG_INHERIT, 0);
        // Don't let child process take the read handle here
        SetHandleInformation(m_ipc_pipe_in, HANDLE_FLAG_INHERIT, 0);
    }

    // Prepare command line
    size_t pos = 0;
    std::wstring quotedPathToSMBX(pathToLegacyEngine);

    while((pos = quotedPathToSMBX.find(L"\"", pos)) != std::string::npos)
    {
        quotedPathToSMBX.replace(pos, 1, L"\\\"");
        pos += 2;
    }

    std::wstring strCmdLine = (
                                  std::wstring(L"\"") + quotedPathToSMBX + std::wstring(L"\" ") +
                                  std::wstring(cmdLineArgs)
                              );

    uint32_t cmdLineMemoryLen = sizeof(wchar_t) * (strCmdLine.length() + 1); // Include null terminator
    wchar_t *cmdLine = (wchar_t *)malloc(cmdLineMemoryLen);
    std::memcpy(cmdLine, strCmdLine.c_str(), cmdLineMemoryLen);

    // Create process
    if(!CreateProcessW(pathToLegacyEngine, // Launch legacy engine executable
                       cmdLine,          // Command line
                       NULL,             // Process handle not inheritable
                       NULL,             // Thread handle not inheritable
                       TRUE,             // Set handle inheritance to FALSE
                       CREATE_SUSPENDED, // Create in suspended state
                       NULL,             // Use parent's environment block
                       workingDir,       // Use parent's starting directory
                       &si,              // Pointer to STARTUPINFO structure
                       &m_pi)              // Pointer to PROCESS_INFORMATION structure
      )
    {
        free(cmdLine);
        cmdLine = NULL;
        return LUNALOADER_CREATEPROCESS_FAIL;
    }
    free(cmdLine);
    cmdLine = NULL;

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
    intptr_t LoaderPatchAddr2 = (intptr_t)VirtualAllocEx(
                                 m_pi.hProcess,         // Target process
                                 NULL,                  // Don't request any particular address
                                 sizeof(LoaderPatch2),  // Length of Patch 2
                                 MEM_COMMIT,            // Type of memory allocation
                                 PAGE_READWRITE         // Memory protection type
                             );
    if(LoaderPatchAddr2 == (intptr_t)NULL)
        return LUNALOADER_PATCH_FAIL;

    // Set Patch1 Addresses
    setJmpAddr(LoaderPatch1, LoaderPatchAddr1, 0x00, LoaderPatchAddr2);

    // Set Patch2 Addresses
    setJmpAddr(LoaderPatch2, LoaderPatchAddr2, 0x10, (intptr_t)&LoadLibraryA);
    setJmpAddr(LoaderPatch2, LoaderPatchAddr2, 0x1D, LoaderPatchAddr1 + 5);

    // Patch the entry point...
    if(WriteProcessMemory(m_pi.hProcess, (void *)LoaderPatchAddr1, LoaderPatch1, sizeof(LoaderPatch1), NULL) == 0 ||
       WriteProcessMemory(m_pi.hProcess, (void *)LoaderPatchAddr2, LoaderPatch2, sizeof(LoaderPatch2), NULL) == 0)
        return LUNALOADER_PATCH_FAIL;

    // Change Patch2 memory protection type
    DWORD TmpDword = 0;
    if(VirtualProtectEx(
           m_pi.hProcess,
           (void *)LoaderPatchAddr2,
           sizeof(LoaderPatch2),
           PAGE_EXECUTE,
           &TmpDword
       ) == 0)
        return LUNALOADER_PATCH_FAIL;

    // Resume the main program thread
    ResumeThread(m_pi.hThread);

    // Close handles
    CloseHandle(m_ipc_pipe_out_i);
    CloseHandle(m_ipc_pipe_in_o);
    CloseHandle(m_pi.hThread);
    m_ipc_pipe_out_i = 0;
    m_ipc_pipe_in_o  = 0;

    return LUNALOADER_OK;
}


bool LunaTester::sendLevelData(LevelData &lvl, QString levelPath, bool isUntitled)
{
    //{"jsonrpc": "2.0", "method": "testLevel", "params":
    //   {"filename": "myEpisode/test.lvl", "leveldata": "<RAW SMBX64 LEVEL DATA>", "players": [{"character": 1}], "godMode": false, "showFPS": false },
    //"id": 3}

    //"players": [ { "character": 1, "powerup": 1, "mountType": 1, "mountColor": 1 }, { "character": 2, "powerup": 1, "mountType": 1, "mountColor": 1 } ]

    if((m_ipc_pipe_out == 0))
        return false;

    QJsonDocument jsonOut;
    QJsonObject jsonObj;
    jsonObj["jsonrpc"] = "2.0";
    jsonObj["method"] = "testLevel";

    QJsonObject JSONparams;

    if(!isUntitled)
    {
        if(levelPath.endsWith(".lvlx", Qt::CaseInsensitive))
            levelPath.remove(levelPath.size() - 1, 1);
        JSONparams["filename"] = levelPath;
    }
    else
        JSONparams["filename"] = ApplicationPath + "/worlds/untitled.lvl";

    QJsonArray JSONPlayers;
    QJsonObject JSONPlayer1, JSONPlayer2;
    SETTINGS_TestSettings  t = GlobalSettings::testing;
    JSONPlayer1["character"]  = t.p1_char;
    JSONPlayer1["powerup"]    = t.p1_state;
    JSONPlayer1["mountType"]  = t.p1_vehicleID;
    JSONPlayer1["mountColor"] = t.p1_vehicleType;
    JSONPlayer2["character"]  = t.p2_char;
    JSONPlayer2["powerup"]    = t.p2_state;
    JSONPlayer2["mountType"]  = t.p2_vehicleID;
    JSONPlayer2["mountColor"] = t.p2_vehicleType;
    //mountType

    JSONPlayers.push_back(JSONPlayer1);

    if(t.numOfPlayers > 1)
        JSONPlayers.push_back(JSONPlayer2);

    JSONparams["players"] = JSONPlayers;

    // Extra flags
    JSONparams["godMode"] = t.xtra_god;
    JSONparams["showFPS"] = t.xtra_showFPS;

    QString LVLRawData;
    //Generate actual SMBX64 Level file data
    FileFormats::WriteSMBX64LvlFileRaw(lvl, LVLRawData, 64);
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

    DWORD writtenBytes = 0;

    //Send data to SMBX
    if(WriteFile(m_ipc_pipe_out,
                 (LPCVOID)dataToSend.c_str(),
                 (DWORD)dataToSend.size(),
                 &writtenBytes, NULL) == TRUE)
    {
        //Read result from level testing run
        std::string resultMsg = ReadMsgString(m_ipc_pipe_in);
        LogDebugQD(QString("Received from SMBX JSON message: %1").arg(QString::fromStdString(resultMsg)));
        return true;
    }
    return false;
}

void LunaTester::lunaChkResetThread()
{
    QMutexLocker mlocker(&this->m_engine_mutex);
    Q_UNUSED(mlocker);
    QThreadPointNuller tlocker(&this->m_helperThread);
    Q_UNUSED(tlocker);
    SafeMsgBoxInterface msg(&m_mw->m_messageBoxer);
    this->m_helperThread = QThread::currentThread();
    DWORD lpExitCode = 0;
    if(GetExitCodeProcess(this->m_pi.hProcess, &lpExitCode))
    {
        if(lpExitCode == STILL_ACTIVE)
        {
            if((m_ipc_pipe_out == 0) || (m_ipc_pipe_in == 0))
                goto failed;
            QJsonDocument jsonOut;
            QJsonObject jsonObj;
            jsonObj["jsonrpc"]  = "2.0";
            jsonObj["method"]   = "resetCheckPoints";
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

            DWORD writtenBytes = 0;
            //Send data to SMBX
            if(WriteFile(m_ipc_pipe_out,
                         (LPCVOID)dataToSend.c_str(),
                         (DWORD)dataToSend.size(),
                         &writtenBytes, NULL) == TRUE)
            {
                std::string inMessage = ReadMsgString(m_ipc_pipe_in);
                LogDebugQD(QString("Received from SMBX JSON message: %1").arg(QString::fromStdString(inMessage)));
                QByteArray jsonData(inMessage.c_str(), inMessage.size());
                QJsonParseError err;
                QJsonDocument jsonOut = QJsonDocument::fromJson(jsonData, &err);
                if(err.error == QJsonParseError::NoError)
                {
                    QJsonObject obj = jsonOut.object();
                    if(obj["error"].isNull())
                    {
                        msg.info("LunaTester",
                                 tr("Checkpoints successfully reseted!"),
                                 QMessageBox::Ok);
                    }
                    else
                    {
                        msg.warning("LunaTester",
                                    QString::fromStdString(inMessage),
                                    QMessageBox::Ok);
                    }
                }
                else
                {
                    msg.warning(tr("LunaTester error!"),
                                err.errorString() +
                                "\n\nData:\n" +
                                QString::fromStdString(inMessage),
                                QMessageBox::Warning);
                }
            }
        }
    }
    else
        goto failed;
    return;
failed:
    msg.warning("LunaTester", tr("LunaLUA tester is not started!"), QMessageBox::Ok);
}

bool LunaTester::closeSmbxWindow(SafeMsgBoxInterface &msg)
{
    if(m_killPreviousSession || (m_ipc_pipe_out == 0) || (m_ipc_pipe_in == 0))
        return false;

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

    DWORD writtenBytes = 0;
    //Send data to SMBX
    if(WriteFile(m_ipc_pipe_out,
                 (LPCVOID)dataToSend.c_str(),
                 (DWORD)dataToSend.size(),
                 &writtenBytes, NULL) == TRUE)
    {
        std::string inMessage = ReadMsgString(m_ipc_pipe_in);
        LogDebugQD(QString("Received from SMBX JSON message: %1").arg(QString::fromStdString(inMessage)));
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
                //Close SMBX's window
                PostMessage(wSmbx, WM_CLOSE, 0, 0);
                //Wait half of second to avoid racings
                Sleep(500);
            }
            else
                msg.warning("LunaTester (closeSmbxWindow, obj[\"error\"])",
                            "LunaLua returned error message:\n" +
                            QString::fromStdString(inMessage),
                            QMessageBox::Ok);
        }
        else
        {
            msg.warning("LunaTester (closeSmbxWindow)",
                        "Error of parsing input data from LunaLua:\n" +
                        err.errorString() +
                        "\n\nData:\n" + QString::fromStdString(inMessage),
                        QMessageBox::Warning);
        }
        return true;
    }
    return false;
}

bool LunaTester::switchToSmbxWindow(SafeMsgBoxInterface &msg)
{
    if((m_ipc_pipe_out == 0) || (m_ipc_pipe_in == 0))
        return false;

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

    DWORD writtenBytes = 0;
    //Send data to SMBX
    if(WriteFile(m_ipc_pipe_out,
                 (LPCVOID)dataToSend.c_str(),
                 (DWORD)dataToSend.size(),
                 &writtenBytes, NULL) == TRUE)
    {
        std::string inMessage = ReadMsgString(m_ipc_pipe_in);
        LogDebugQD(QString("Received from SMBX JSON message: %1").arg(QString::fromStdString(inMessage)));
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
            }
            else
                msg.warning("LunaTester (switchToSmbxWindow, obj[\"error\"])",
                            "LunaLua returned error message:\n" +
                            QString::fromStdString(inMessage),
                            QMessageBox::Ok);
        }
        else
        {
            msg.warning("LunaTester (switchToSmbxWindow)",
                        "Error of parsing input data from LunaLua:\n" +
                        err.errorString() +
                        "\n\nData:\n" + QString::fromStdString(inMessage),
                        QMessageBox::Warning);
        }
        return true;
    }
    return false;
}

void LunaTester::lunaRunnerThread(LevelData in_levelData, QString levelPath, bool isUntitled)
{
    QMutexLocker mlocker(&this->m_engine_mutex);
    Q_UNUSED(mlocker);
    QThreadPointNuller tlocker(&this->m_helperThread);
    Q_UNUSED(tlocker);

    SafeMsgBoxInterface msg(&m_mw->m_messageBoxer);

    this->m_helperThread = QThread::currentThread();

    // Prepare level file for SMBX-64 format
    FileFormats::smbx64LevelPrepare(in_levelData);

    int smbx64limits = FileFormats::smbx64LevelCheckLimits(in_levelData);
    if(smbx64limits != FileFormats::SMBX64_FINE)
    {
        int reply = msg.warning(tr("SMBX64 limits are excited!"),
                                tr("Violation of SMBX64 standard has beeen found!\n"
                                   "%1\n"
                                   ", legacy engine may crash!\n"
                                   "Suggested to remove all excess elements.\n"
                                   "Are you want continue process?")
                                .arg(smbx64ErrMsgs(in_levelData, smbx64limits)),
                                QMessageBox::Yes | QMessageBox::No);
        if(reply != QMessageBox::Yes)
            return;
    }

    // Launch LunaLoader
    {
        QString smbxPath = ConfStatus::configDataPath;
        if(!QFile(smbxPath + ConfStatus::SmbxEXE_Name).exists())
        {
            msg.warning(LunaTester::tr("SMBX Directory wasn't configured right"),
                        LunaTester::tr("%1 not found!\nTo run testing via SMBX you should have right SMBX Integration configuration package!")
                        .arg(smbxPath + ConfStatus::SmbxEXE_Name),
                        QMessageBox::Ok);
            return;
        }

        DWORD lpExitCode = 0;
        if(GetExitCodeProcess(this->m_pi.hProcess, &lpExitCode))
        {
            if(lpExitCode == STILL_ACTIVE)
            {
                if(m_ipc_pipe_out != 0)
                {
                    //Workaround to avoid weird crash
                    closeSmbxWindow(msg);
                    //Then send level data to SMBX Engine
                    if(sendLevelData(in_levelData, levelPath, isUntitled))
                    {
                        //Stop music playback in the PGE Editor!
                        QMetaObject::invokeMethod(m_mw, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
                        // not sure how efficient it is
                        QMetaObject::invokeMethod(m_mw, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
                        //Attempt to switch SMBX Window
                        switchToSmbxWindow(msg);
                    }
                }
                else if(msg.warning(LunaTester::tr("SMBX Test is already runned"),
                                    LunaTester::tr("SMBX Engine is already testing another level.\n"
                                                   "Do you want to abort current testing process?"),
                                    QMessageBox::Abort | QMessageBox::Cancel) == QMessageBox::Abort)
                {
                    WaitForSingleObject(this->m_pi.hProcess, 100);
                    TerminateProcess(this->m_pi.hProcess, lpExitCode);
                    CloseHandle(this->m_pi.hProcess);
                }
                return;
            }
        }

        QString command = smbxPath + ConfStatus::SmbxEXE_Name;
        QStringList params;

        if(!QFile(smbxPath + "LunaDll.dll").exists())
        {
            /**********************************************
             *****Do Vanilla test with dummy episode!******
             **********************************************/
            {
                int msgRet = msg.richBox(LunaTester::tr("Vanilla SMBX detected!"),
                                         LunaTester::tr("%2 not found!\nYou have a Vanilla SMBX!<br>\n"
                                                        "That means, impossible to launch level testing automatically. "
                                                        "To launch a level testing, will be generated a dummy episode which you can "
                                                        "start and select manually.<br>\n<br>\n"
                                                        "Name of episode to generate: %1<br>\n<br>\n"
                                                        "Are you still want to launch a test?<br>\n<br>\n"
                                                        "If you want to have a full featured level testing, you need to get a LunaLUA here:<br>\n"
                                                        "%3")
                                         .arg("\"_temp_episode_pge\"")
                                         .arg(smbxPath + "LunaDll.dll")
                                         .arg("<a href=\"http://wohlsoft.ru/LunaLua/\">http://wohlsoft.ru/LunaLua/</a>"),
                                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Warning);

                if(msgRet != QMessageBox::Yes)
                    return;
            }

            QString tempPath        = smbxPath + "/worlds/";
            QString tempName        = "tmp.PgeWorld.DeleteMe";
            QString dst_Episode     = tempPath + tempName + "/";
            QString src_episodePath = in_levelData.meta.path;
            QString src_customPath  = in_levelData.meta.path + "/" + in_levelData.meta.filename;

            QDir dummyWorld(dst_Episode);
            if(dummyWorld.exists(dst_Episode))
            {
                QString testFolder = dst_Episode + "templevel/";
                RemoveDirectoryW(testFolder.toStdWString().c_str());//Remove symblic link if possible
                //Clean-up old stuff
                dummyWorld.removeRecursively();
            }
            dummyWorld.mkpath(dst_Episode);

            FileFormats::WriteSMBX64LvlFileF(dst_Episode + "/templevel.lvl", in_levelData, 64);

            if(!isUntitled)
            {
                //Copy available custom stuff into temp directory
                QDir episodeDir(src_episodePath);
                QDir customDir(src_customPath);

                QStringList fileters;
                fileters << "*.txt" << "*.ini" << "*.lua" << "*.gif" << "*.png" << "*.bmp";
                QStringList fileters_cdir;
                fileters_cdir << "*.txt" << "*.ini" << "*.lua" << "*.gif" << "*.png" << "*.mp3"
                              //OGG Vorbis and FLAC (LibOGG, LibVorbis, LibFLAC)
                              << "*.ogg" << "*.flac"
                              //Uncompressed audio data
                              << "*.wav" << "*.voc" << "*.aiff"
                              //MIDI
                              << "*.mid" << "*.cmf"
                              //MikMod (Modules)
                              << "*.mod" << "*.it" << "*.s3m" << "*.669" << "*.med" << "*.xm" << "*.amf"
                              << "*.apun" << "*.dsm" << "*.far" << "*.gdm" << "*.imf" << "*.mtm"
                              << "*.okt" << "*.stm" << "*.stx" << "*.ult" << "*.uni"
                              //GAME EMU
                              << "*.ay" << "*.gbs" << "*.gym" << "*.hes" << "*.kss" << "*.nsf" << "*.nsfe" << "*.sap" << "*.spc" << "*.vgm" << "*.vgz"
                              //Rockythechao's extensions
                              << "*.anim";
                episodeDir.setNameFilters(fileters);
                customDir.setNameFilters(fileters_cdir);

                //***********************Attempt to make symbolic link*******************************/
                bool needToCopyEverything = true;
                typedef BOOL *(WINAPI * FUNK_OF_SYMLINKS)(TCHAR * linkFileName, TCHAR * existingFileName, DWORD flags);
                HMODULE hKernel32 = NULL;
                FUNK_OF_SYMLINKS fCreateSymbolicLink = NULL;
                hKernel32 = LoadLibraryW(L"KERNEL32.DLL");
                if(hKernel32)
                {
                    fCreateSymbolicLink = (FUNK_OF_SYMLINKS)GetProcAddress(hKernel32, "CreateSymbolicLinkW");
                    if(fCreateSymbolicLink) //Try to make a symblic link
                    {
                        QString newPath = dst_Episode + "templevel/";
                        if(fCreateSymbolicLink((TCHAR *)newPath.toStdWString().c_str(),
                                               (TCHAR *)src_customPath.toStdWString().c_str(), 0x1))
                            needToCopyEverything = false;
                    }
                }
                //************Copy images and scripts from episode folder**********************/
                QStringList files = episodeDir.entryList(QDir::Files);
                foreach(QString filex, files)
                    QFile::copy(src_episodePath + "/" + filex, dst_Episode + filex);

                //********************Copy images and scripts from custom folder*****************/
                //***************for case where impossible to make a symbolic link***************/
                if(needToCopyEverything)
                {
                    customDir.setSorting(QDir::NoSort);
                    QDirIterator dirsList(src_customPath, fileters_cdir,
                                          QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                                          QDirIterator::Subdirectories);
                    while(dirsList.hasNext())
                    {
                        dirsList.next();
                        QString relativeDir = customDir.relativeFilePath(dirsList.fileInfo().absoluteDir().absolutePath());
                        QString filex = dirsList.fileName();//customDir.relativeFilePath(dirsList.filePath());
                        QString relNewPath = dst_Episode + "templevel/" + relativeDir;
                        QDir newRelDir(relNewPath);
                        if(!newRelDir.exists())
                            newRelDir.mkpath(relNewPath);
                        QFile::copy(src_customPath + "/" + relativeDir + "/" + filex, relNewPath + "/" + filex);
                    }
                }
                /*********************************************************************************/

                //Copy custom musics if possible
                foreach(LevelSection sec, in_levelData.sections)
                {
                    if(sec.music_file.isEmpty())
                        continue;

                    QString musFile = sec.music_file;
                    for(int i = 0; i < musFile.size(); i++)
                    {
                        if(musFile[i] == '|')
                            musFile.remove(i, musFile.size() - i);
                    }
                    QString MusicFileName = src_episodePath + "/" + musFile;
                    QString MusicNewPath = dst_Episode + musFile;

                    QFile mus(MusicFileName);
                    if(!mus.exists(MusicFileName))
                        continue;

                    QFileInfo inf(MusicNewPath);
                    if(!needToCopyEverything)
                    {
                        if(inf.absoluteDir().absolutePath().contains((dst_Episode + "templevel/"), Qt::CaseInsensitive))
                            continue; //Don't copy musics to the same directory!
                    }
                    if(!inf.absoluteDir().exists())
                        inf.absoluteDir().mkpath(inf.absoluteDir().absolutePath());
                    if(!needToCopyEverything)
                    {
                        if(fCreateSymbolicLink((TCHAR *)MusicNewPath.toStdWString().c_str(),
                                               (TCHAR *)MusicFileName.toStdWString().c_str(), 0x0) == 0)
                        {
                            mus.copy(MusicNewPath);//Copy file if impossible to make symbolic link to it
                        }
                    }
                    else
                        mus.copy(MusicNewPath);
                }
                if(hKernel32)
                    FreeLibrary(hKernel32);
            }

            WorldData worldmap;
            FileFormats::CreateWorldData(worldmap);
            worldmap.EpisodeTitle = "_temp_episode_pge";
            worldmap.IntroLevel_file = "templevel.lvl";
            worldmap.restartlevel = true;

            if(!FileFormats::WriteSMBX64WldFileF(dst_Episode + "/tempworld.wld", worldmap, 64))
            {
                msg.warning(LunaTester::tr("File save error"),
                            LunaTester::tr("Cannot save file %1:\n%2.")
                            .arg(dst_Episode + "/tempworld.wld")
                            .arg(FileFormats::errorString),
                            QMessageBox::Ok);
                return;
            }

            QProcess::startDetached(command, params, smbxPath);
            //Stop music playback in the PGE Editor!
            QMetaObject::invokeMethod(m_mw, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
            // not sure how efficient it is
            QMetaObject::invokeMethod(m_mw, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
            return;

        }
        else
        {
            /**********************************************
             **********Do LunaLUA testing launch!**********
             **********************************************/
            params << (m_killPreviousSession ? "--patch" : "--hideOnCloseIPC");

            if(m_noGL)
                params << "--nogl";

            QString argString;
            for(int i = 0; i < params.length(); i++)
            {
                if(i > 0)
                    argString += " ";
                argString += params[i];
            }

            // Make sure any old pipe handle is closed
            if(this->m_ipc_pipe_out)
            {
                CloseHandle(this->m_ipc_pipe_out);
                this->m_ipc_pipe_out = 0;
            }
            if(this->m_ipc_pipe_in)
            {
                CloseHandle(this->m_ipc_pipe_in);
                this->m_ipc_pipe_in = 0;
            }

            #ifdef USE_LUNAHEXER //Hexes legacy SMBX.exe and starts it as regular exe
            LunaLoaderResult res = LunaHexerRun(command.toStdWString().c_str(),
                                                argString.toStdWString().c_str(),
                                                smbxPath.toStdWString().c_str());
            #else
            LunaLoaderResult res = LunaLoaderRun(command.toStdWString().c_str(),
                                                 argString.toStdWString().c_str(),
                                                 smbxPath.toStdWString().c_str());
            #endif

            if(res == LUNALOADER_OK)
            {
                if(sendLevelData(in_levelData, levelPath, isUntitled))
                {
                    //GlobalSettings::recentMusicPlayingState = ui->actionPlayMusic->isChecked();
                    //Stop music playback in the PGE Editor!
                    QMetaObject::invokeMethod(m_mw, "setMusicButton", Qt::QueuedConnection, Q_ARG(bool, false));
                    QMetaObject::invokeMethod(m_mw, "on_actionPlayMusic_triggered", Qt::QueuedConnection, Q_ARG(bool, false));
                }
                else
                {
                    msg.warning(LunaTester::tr("LunaTester error"),
                                LunaTester::tr("Failed to send level into LunaLUA-SMBX!"),
                                QMessageBox::Ok);
                }
            }
            else
            {
                QString luna_error = "Unknown error";

                switch(res)
                {
                case LUNALOADER_CREATEPROCESS_FAIL:
                    luna_error = LunaTester::tr("process execution is failed.");
                    break;
                case LUNALOADER_PATCH_FAIL:
                    luna_error = LunaTester::tr("patching has failed.");
                    break;
                default:
                    break;
                }

                msg.warning(LunaTester::tr("LunaTester error"),
                            LunaTester::tr("Impossible to launch SMBX Engine, because %1").arg(luna_error),
                            QMessageBox::Ok);
            }
        }//Do LunaLUA direct testing launch
    }
}

void LunaTester::lunaRunGame()
{
    QString smbxPath = ConfStatus::configDataPath;
    QString command = smbxPath + ConfStatus::SmbxEXE_Name;

    if(!QFile(smbxPath + ConfStatus::SmbxEXE_Name).exists())
    {
        QMessageBox::warning(m_mw,
                             LunaTester::tr("Directory of Legacy Engine wasn't configured right"),
                             LunaTester::tr("%1 not found!\nTo use LunaTester you should have right "
                                            "Integration configuration package!")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

    QStringList params;
    params << "--patch";
    params << "--game";

    if(m_noGL)
        params << "--nogl";

    if(!QFile(smbxPath + "LunaDll.dll").exists())
        QProcess::startDetached(command, params, smbxPath);
    else
    {
        QString argString;
        for(int i = 0; i < params.length(); i++)
        {
            if(i > 0)
                argString += " ";
            argString += params[i];
        }

        DWORD lpExitCode = 0;
        //Abort engine staying in background
        if(GetExitCodeProcess(this->m_pi.hProcess, &lpExitCode))
        {
            if(lpExitCode == STILL_ACTIVE)
            {
                WaitForSingleObject(this->m_pi.hProcess, 100);
                TerminateProcess(this->m_pi.hProcess, lpExitCode);
                CloseHandle(this->m_pi.hProcess);
            }
        }

        // Make sure any old pipe handle is closed
        if(this->m_ipc_pipe_out)
        {
            CloseHandle(this->m_ipc_pipe_out);
            this->m_ipc_pipe_out = 0;
        }
        if(this->m_ipc_pipe_in)
        {
            CloseHandle(this->m_ipc_pipe_in);
            this->m_ipc_pipe_in = 0;
        }

        #ifdef USE_LUNAHEXER //Hexes legacy SMBX.exe and starts it as regular exe
        LunaLoaderResult res = LunaHexerRun(command.toStdWString().c_str(),
                                            argString.toStdWString().c_str(),
                                            smbxPath.toStdWString().c_str());
        #else
        LunaLoaderResult res = LunaLoaderRun(command.toStdWString().c_str(),
                                             argString.toStdWString().c_str(),
                                             smbxPath.toStdWString().c_str());
        #endif
        if(res != LUNALOADER_OK)
        {
            QString luna_error = "Unknown error";

            switch(res)
            {
            case LUNALOADER_CREATEPROCESS_FAIL:
                luna_error = LunaTester::tr("process execution is failed.");
                break;
            case LUNALOADER_PATCH_FAIL:
                luna_error = LunaTester::tr("patching has failed.");
                break;
            default:
                break;
            }

            QMessageBox::warning(m_mw,
                                 LunaTester::tr("LunaTester error"),
                                 LunaTester::tr("Impossible to launch Legacy Engine, because %1")
                                 .arg(luna_error),
                                 QMessageBox::Ok);
            return;
        }
    }

    //Stop music playback in the PGE Editor!
    QMetaObject::invokeMethod(m_mw,
                              "setMusicButton",
                              Qt::QueuedConnection,
                              Q_ARG(bool, false));

    // not sure how efficient it is
    QMetaObject::invokeMethod(m_mw,
                              "on_actionPlayMusic_triggered",
                              Qt::QueuedConnection,
                              Q_ARG(bool, false));
}

#endif //_WIN32
