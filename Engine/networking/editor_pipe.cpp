#include "editor_pipe.h"
#include <common_features/logger.h>
#include <common_features/util.h>

#include <QFile>
#include <QStringList>
#include <QtDebug>
#include <QElapsedTimer>

#include <iostream>
#include <cstdio>
#include "../common_features/app_path.h"
#include <networking/intproc.h>

/**************************************************************************************************************/

int EditorPipe::run(void *self)
{
    EditorPipe &me = *reinterpret_cast<EditorPipe *>(self);

    while(me.m_thread_isAlive)
    {
        std::string buffer;
        std::string out;
        std::cin >> buffer;

        if(me.m_thread_isAlive)
        {
            util::base64_decode(out, buffer);
            me.icomingData(out);
        }
    }

    return 0;
}

void EditorPipe::start()
{
    std::cin.sync_with_stdio(false);
    m_thread_isAlive = true;
    m_thread = SDL_CreateThread(&run, "EditorPipe_std", this);
}

void EditorPipe::stop()
{
    m_thread_isAlive = false;
    //int status = 0;
    //SDL_WaitThread(m_thread, &status);
}

void EditorPipe::sendMessage(const QString &msg)
{
    std::string in = msg.toStdString();
    std::string outO;
    util::base64_encode(outO, reinterpret_cast<const unsigned char *>(in.c_str()), in.size());
    std::fprintf(stdout, "%s\n", outO.c_str());
    std::fflush(stdout);
}

void EditorPipe::sendMessage(const std::string &in)
{
    std::string outO;
    util::base64_encode(outO, reinterpret_cast<const unsigned char *>(in.c_str()), in.size());
    std::fprintf(stdout, "%s\n", outO.c_str());
    std::fflush(stdout);
}

void EditorPipe::sendMessage(const char *in)
{
    std::string outO;
    util::base64_encode(outO, reinterpret_cast<const unsigned char *>(in), strlen(in));
    std::fprintf(stdout, "%s\n", outO.c_str());
    std::fflush(stdout);
}
/**************************************************************************************************************/


/**
 * @brief EditorPipe::LocalServer
 *  Constructor
 */

EditorPipe::EditorPipe():
    m_thread(nullptr),
    m_thread_isAlive(false),
    m_isWorking(false),
    m_doAcceptLevelData(false),
    m_doParseLevelData(false),
    m_levelAccepted(false)
{
    m_acceptedRawData.clear();
    FileFormats::CreateLevelData(m_acceptedLevel);
    m_acceptedLevel.meta.ReadFileValid = false;
    pLogDebug("Construct interprocess pipe...");
    start();
}

/**
 * @brief EditorPipe::~LocalServer
 *  Destructor
 */
EditorPipe::~EditorPipe()
{
    pLogDebug("Destroying interprocess pipe...");
    stop();
}

void EditorPipe::shut()
{
    sendMessage("CMD:ENGINE_CLOSED");
    stop();
}

bool EditorPipe::levelIsLoad()
{
    m_levelAccepted_lock.lock();
    bool state = m_levelAccepted;
    m_levelAccepted = false;
    m_levelAccepted_lock.unlock();
    return state;
}

void EditorPipe::icomingData(std::string &in)
{
    if(("PARSE_LVLX" == in) || ("OPEN_TEMP_LVLX" == in))
    {
        m_doAcceptLevelData = false;
        pLogDebug("LVLX accepting is done!");
        IntProc::setState("LVLX Accepted, do parsing of LVLX");
    }

    if(m_doAcceptLevelData)
    {
        m_acceptedRawData.append(in.c_str());
        pLogDebug("Append LVLX data...");
    }
    else if(in.compare(0, 11, "SEND_LVLX: ") == 0)
    {
        //Delete old cached stuff
        m_acceptedRawData.clear();
        D_pLogDebug("IN: >> %s",
                    (in.size() > 30 ?
                     in.substr(0, 30).c_str() :
                     in.c_str())
                   );
        m_accepted_lvl_path   = QString::fromUtf8(in.c_str() + 11, static_cast<int>(in.size() - 11));//skip "SEND_LVLX: "
        m_doAcceptLevelData  = true;
        IntProc::setState("Accepted SEND_LVLX");
        sendMessage("READY\n\n");
    }
    else if(in.compare("PARSE_LVLX") == 0)
    {
        pLogDebug("do Parse LVLX: PARSE_LVLX");
        m_doParseLevelData = true;
        FileFormats::ReadExtendedLvlFileRaw(m_acceptedRawData, m_accepted_lvl_path, m_acceptedLevel);
        IntProc::setState(QString("LVLX is valid: %1").arg(m_acceptedLevel.meta.ReadFileValid));
        pLogDebug("Level data parsed, Valid: %d", m_acceptedLevel.meta.ReadFileValid);

        if(!m_acceptedLevel.meta.ReadFileValid)
        {
            pLogDebug("Error reason:  %s", m_acceptedLevel.meta.ERROR_info.toStdString().c_str());
            pLogDebug("line number:   %d", m_acceptedLevel.meta.ERROR_linenum);
            pLogDebug("line contents: %s", m_acceptedLevel.meta.ERROR_linedata.toStdString().c_str());
            D_pLogDebug("Invalid File data BEGIN >>>>>>>>>>>\n"
                        "%s"
                        "\n<<<<<<<<<<<<INVALID File data END",
                        m_acceptedRawData.toStdString().c_str());
        }

        m_levelAccepted_lock.lock();
        m_levelAccepted = true;
        m_levelAccepted_lock.unlock();
    }
    else if(in.compare("OPEN_TEMP_LVLX") == 0)
    {
        pLogDebug("do Parse LVLX: >> OPEN_TEMP_LVLX");
        m_doParseLevelData = true;
        QFile temp(m_acceptedRawData.remove("\n"));

        //if( FileFormats::ReadExtendedLvlFileF(accepted_lvl_raw.remove("\n"), accepted_lvl) )
        if(temp.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream x(&temp);
            QString raw = x.readAll();
            FileFormats::ReadExtendedLvlFileRaw(raw, m_accepted_lvl_path, m_acceptedLevel);
            IntProc::setState(QString("LVLX is valid: %1").arg(m_acceptedLevel.meta.ReadFileValid));
            pLogDebug("Level data parsed, Valid: %d", m_acceptedLevel.meta.ReadFileValid);
        }
        else
        {
            IntProc::setState(QString("LVLX is valid: %1").arg(m_acceptedLevel.meta.ReadFileValid));
            pLogDebug("Level data parsed, Valid: %d", m_acceptedLevel.meta.ReadFileValid);
            pLogDebug("ERROR: Can't open temp file");
            m_acceptedLevel.meta.ReadFileValid = false;
        }

        temp.remove();
        m_levelAccepted_lock.lock();
        m_levelAccepted = true;
        m_levelAccepted_lock.unlock();
    }
    else if(in.compare(0, 11, "PLACEITEM: ") == 0)
    {
        D_pLogDebug("Accepted Placing item!");
        IntProc::storeCommand(in.c_str() + 11, in.size() - 11, IntProc::PlaceItem);
    }
    else if(in.compare(0, 8, "MSGBOX: ") == 0)
    {
        pLogDebug("Accepted Message box: %s", in.c_str());
        IntProc::storeCommand(in.c_str() + 8, in.size() - 8, IntProc::MsgBox);
    }
    else if(in.compare("CHEAT: ") == 0)
    {
        pLogDebug("Accepted cheat code: %s", in.c_str());
        IntProc::storeCommand(in.c_str() + 7, in.size() - 7, IntProc::Cheat);
    }
    else if(in.compare("PING") == 0)
    {
        D_pLogDebug("IN: >> PING");
        sendMessage("PONG\n\n");
        pLogDebug("Ping-Pong!");
    }
}
