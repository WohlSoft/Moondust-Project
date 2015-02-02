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
#include <main_window/global_settings.h>
#include <dev_console/devconsole.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>
#include <networking/engine_intproc.h>

#ifdef Q_OS_WIN
#include <windows.h>
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

    if(!QFileInfo(command).exists())
    {
        QMessageBox::warning(this, tr("Engine is not found"),
                             tr("Can't start testing, engine is not found: \n%1\nPlease, check the application directory.")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

    if(IntEngine::isWorking())
    {
        if(QMessageBox::warning(this, tr("Engine already runned"),
                             tr("Engine is already testing another level.\n"
                                "Do you want to abort current testing process?"),
                             QMessageBox::Abort|QMessageBox::Cancel)==QMessageBox::Abort)
        {
            IntEngine::quit();
        }
        return;
    }

    if(activeChildWindow()==1)
    {
        //if(activeLvlEditWin()->isUntitled) return;

        QStringList args;
        args << "--debug";
        args << "--config=\""+QDir(configs.config_dir).dirName()+"\"";
        args << "--interprocessing";//activeLvlEditWin()->curFile;

        qDebug() << "Executing engine..." << command;
        QProcess::startDetached(command, args);

        QElapsedTimer t; t.start();
        while(t.elapsed()<500);

        qDebug() << "Installing interprocessing...";
        if(!IntEngine::isWorking()) IntEngine::init();
        IntEngine::setTestLvlBuffer(activeLvlEditWin()->LvlData);
        qDebug() << "Started";
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


    if(!QFileInfo(command).exists())
    {
        QMessageBox::warning(this, tr("Engine is not found"),
                             tr("Can't start testing, engine is not found: \n%1\nPlease, check the application directory.")
                             .arg(command),
                             QMessageBox::Ok);
        return;
    }

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

        QStringList args;
        args << "--debug";
        args << "--config=\""+QDir(configs.config_dir).dirName()+"\"";
        args << activeLvlEditWin()->curFile;

        QProcess::startDetached(command, args);
    }
}



void MainWindow::on_action_testSettings_triggered()
{

    QMessageBox::information(this, tr("Dummy"),
    tr("Sorry, the testing feature is under construction.\nSettings will available soon."),
    QMessageBox::Ok);

}


#ifdef Q_OS_WIN
static HRESULT _C2W(const char * pszChar, wchar_t **ppszwChar)
{
    HRESULT hr = S_OK;
    Q_UNUSED(hr);
    unsigned int iRetVal = 0;
    iRetVal = MultiByteToWideChar(CP_ACP,0,pszChar,-1,NULL,0);
    if(iRetVal == 0){(*ppszwChar) = NULL; return E_FAIL;}
    *ppszwChar = (wchar_t *) new wchar_t[iRetVal];
    if((*ppszwChar) == NULL) return E_FAIL;
    iRetVal = MultiByteToWideChar(CP_ACP,0,pszChar,-1,*ppszwChar,iRetVal);
    return iRetVal;
}
#endif

void MainWindow::on_actionRunTestSMBX_triggered()
{
 #ifdef Q_OS_WIN
    if(activeChildWindow()==1)
    {
        if(activeLvlEditWin()->isUntitled)
        {
            QMessageBox::warning(this, tr("Save file first"),
            tr("To run testing via SMBX file must be saved into disk first!"),
            QMessageBox::Ok);
            return;
        }

        COPYDATASTRUCT* cds = new COPYDATASTRUCT;
        cds->cbData = 1;
        cds->dwData = (ULONG_PTR)0xDEADC0DE;
        cds->lpData = NULL;

        HWND smbxWind = FindWindowA("ThunderRT6MDIForm", NULL);
        if(smbxWind)
        {
            QString fullPathToLevel= activeLvlEditWin()->curFile;
            if(activeLvlEditWin()->isModified)
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
            TCHAR szName[]=TEXT("Global\\LunaDLL_LevelFileName_834727238");
            HANDLE hMapFile;
            LPCTSTR pBuf;
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

            pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                                    FILE_MAP_ALL_ACCESS, // read/write permission
                                    0,
                                    0,
                                    15360);

            if(pBuf == NULL)
            {
                QMessageBox::warning(this, tr("Error"),
                tr("Could not map view of file (%1).").arg(GetLastError()),
                QMessageBox::Ok);
                CloseHandle(hMapFile);
                return;
            }

            TCHAR *wc = NULL;
            _C2W(fullPathToLevel.toLocal8Bit().data(),&wc);

            CopyMemory((PVOID)pBuf, wc, fullPathToLevel.toUtf8().size());
            UnmapViewOfFile(pBuf);
            CloseHandle(hMapFile);
            free(wc);
            /****************Write file path into shared memory****end*******************/


            if(DevConsole::isConsoleShown())
                DevConsole::log("Sent Message (Hopefully it worked)");
            //Minimize PGE Editor
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
#else
    log("Requires Windows OS!");
#endif

}


