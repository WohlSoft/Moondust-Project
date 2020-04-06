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

#include "luna_capabilities.h"

#include <QFile>
#include <QByteArray>
#include <QFileInfo>
#include <QVariant>
#include <QDir>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

// Boundaries of built-in capatibilities JSON block
#define LUNALUA_CAPS_BEGIN  "STARTFEATUREJSON_Qf+OLC2Re05VTUIuDg99daqvfViNcGEwViqrEfXfJhg"
#define LUNALUA_CAPS_END    "ENDFEATUREJSON_Qf+OLC2Re05VTUIuDg99daqvfViNcGEwViqrEfXfJhg"

// Magic strings to heuristically recognize capatibilities by a version of LunaLua
#define DLLCHECK_SMBX2BETA3         "LUNALUA V0.7.3.1 BETA"
#define DLLCHECK_SMBX2MAGLX3        "LUNALUA 70d92f34"
#define DLLCHECK_SMBX2PAL           "LUNALUA 669b266"
#define DLLCHECK_SMBX2BETA4         "LUNALUA 915bbba"
#define DLLCHECK_SMBX2BETA4p1       "LUNALUA 82b1b88"

// Window titles defined in config/game.ini file to heuristically recognize capaitibilites
#define HEURISTIC_SMBX2BETA3        "Super Mario Bros. X - 2.0.0 (Beta 3)"
#define HEURISTIC_SMBX2MAGLX3       "Super Mario Bros. X2 - (2.0.0 bMAGLX3)"
#define HEURISTIC_SMBX2PAL          "Super Mario Bros. X2 - (2.0.0 PAL)"
#define HEURISTIC_SMBX2b4           "Super Mario Bros. X2 - (2.0.0 b4)"


/**
 * @brief Fills capatibilities of SMBX3-Beta3 LunaLua
 * @param caps Target capatibilities structure
 */
static void fillCapsBeta3(LunaLuaCapabilities &caps)
{
    caps.type = "heuristic BETA3";
    caps.ipcCommands << "echo" << "testLevel" <<
                        "getWindowHandle" << "resetCheckPoints";
    caps.args << "patch" << "game" << "leveleditor" << "noframeskip" <<
                 "nosound" << "debugger" << "logger" << "newlauncher" <<
                 "console" << "nogl" << "testLevel" << "waitForIPC" <<
                 "hideOnCloseIPC";
}

/**
 * @brief Fills capatibilities of SMBX3-MAGLX3 LunaLua
 * @param caps Target capatibilities structure
 */
static void fillCapsMAGLX3(LunaLuaCapabilities &caps)
{
    caps.type = "heuristic MAGLX3";
    caps.features << "LVLX";
    caps.ipcCommands << "echo" << "testLevel" <<
                        "getWindowHandle" << "resetCheckPoints" <<
                        "getSupportedFeatures";
    caps.args << "patch" << "game" << "leveleditor" << "noframeskip" <<
                 "nosound" << "debugger" << "logger" << "newlauncher" <<
                 "console" << "nogl" << "testLevel" << "waitForIPC" <<
                 "hideOnCloseIPC" << "oldLvlLoader";
}

/**
 * @brief Fills capatibilities of SMBX3-PAL LunaLua
 * @param caps Target capatibilities structure
 */
static void fillCapsPAL(LunaLuaCapabilities &caps)
{
    caps.type = "heuristic PAL";
    caps.features << "LVLX";
    caps.ipcCommands << "echo" << "testLevel" <<
                        "getWindowHandle" << "resetCheckPoints" <<
                        "getSupportedFeatures";
    caps.args << "patch" << "game" << "leveleditor" << "noframeskip" <<
                 "nosound" << "debugger" << "logger" << "newlauncher" <<
                 "console" << "nogl" << "testLevel" << "waitForIPC" <<
                 "hideOnCloseIPC" << "oldLvlLoader";
}

/**
 * @brief Fills capatibilities of SMBX3-Beta4 LunaLua
 * @param caps Target capatibilities structure
 */
static void fillCapsBeta4(LunaLuaCapabilities &caps)
{
    caps.type = "heuristic BETA4";
    caps.features << "LVLX";
    caps.ipcCommands << "echo" << "testLevel" <<
                        "getWindowHandle" << "resetCheckPoints" <<
                        "getSupportedFeatures";
    caps.args << "patch" << "game" << "leveleditor" << "noframeskip" <<
                 "nosound" << "debugger" << "logger" << "newlauncher" <<
                 "console" << "nogl" << "testLevel" << "waitForIPC" <<
                 "hideOnCloseIPC" << "oldLvlLoader" <<
                 "softGL" << "forceHardGL";
}

/**
 * @brief Fills capatibilities, captured from a built-in JSON string
 * @param caps Target capatibilities structure
 */
static void fillCapsFromJson(LunaLuaCapabilities &caps, const QByteArray &rawJson)
{
    QJsonDocument q = QJsonDocument::fromJson(rawJson);
    auto root = q.object();
    auto rKeys = root.keys();

    caps.type = "Built-in JSON";

    if(rKeys.contains("Features"))
    {
        auto features = root["Features"].toArray();
        auto vl = features.toVariantList();
        for(auto &q : vl)
            caps.features.insert(q.toString());
    }

    if(rKeys.contains("IPCCmds"))
    {
        auto features = root["IPCCmds"].toArray();
        auto vl = features.toVariantList();
        for(auto &q : vl)
            caps.ipcCommands.insert(q.toString());
    }

    if(rKeys.contains("Args"))
    {
        auto features = root["Args"].toArray();
        auto vl = features.toVariantList();
        for(auto &q : vl)
            caps.args.insert(q.toString());
    }
}

/**
 * @brief Retrieve a built-in JSON string or a magic word to recognize capatibilities of LunaDll.dll
 * @param caps Target output capatibilities structure
 * @param path Path to LunaDll.dll file
 * @return true when capatibilities was been recognized, false if not, therefore needos to perform a heuristic detection
 */
static bool getRawLunaCapabilities(LunaLuaCapabilities &caps, const QString &path)
{
    QByteArray a;
    QByteArray out;
    const QByteArray begin(LUNALUA_CAPS_BEGIN);
    const QByteArray end(LUNALUA_CAPS_END);
    qint64 got = 0;
    QFile q(path);
    if(!q.open(QIODevice::ReadOnly))
        return false;

    q.seek(q.size() - 2048);
    a.resize(4096);

    do
    {
        a.fill(0);
        got = q.read(a.data(), 2048);
        if(got < 0)
            break;

        int bPos = a.indexOf(begin);
        if(bPos < 0)
        {
            // Try to find any known LunaLua version marker
            if(a.indexOf(DLLCHECK_SMBX2BETA3) > 0)
            {
                fillCapsBeta3(caps);
                caps.type = "DLL-Check Beta3";
                return true;
            }
            else if(a.indexOf(DLLCHECK_SMBX2MAGLX3) > 0)
            {
                fillCapsMAGLX3(caps);
                caps.type = "DLL-Check MAGLX3";
                return true;
            }
            else if(a.indexOf(DLLCHECK_SMBX2PAL) > 0)
            {
                fillCapsPAL(caps);
                caps.type = "DLL-Check PAL";
                return true;
            }
            else if(a.indexOf(DLLCHECK_SMBX2BETA4) > 0)
            {
                fillCapsBeta4(caps);
                caps.type = "DLL-Check Beta4";
                return true;
            }
            else if(a.indexOf(DLLCHECK_SMBX2BETA4p1) > 0)
            {
                fillCapsBeta4(caps);
                caps.type = "DLL-Check Beta4 Patch 1";
                return true;
            }

            qint64 at = q.pos() - 2048;
            if(at > 500)
                q.seek(at - 500);
            else
                q.seek(0);
            continue;
        }

        int ePos = a.indexOf(end);
        if(ePos < 0)
            break; // Invalid file

        int dStart = bPos + begin.size();
        int dLen = ePos - dStart;

        out = a.mid(dStart, dLen).trimmed();
        fillCapsFromJson(caps, out);
        return true;

    } while(q.pos() > 0);

    return false;
}

/**
 * @brief Heuristically detect a version of SMBX2 to use certain capatibilities set
 * @param caps Output capatibilities container
 * @param path Path to LunaDll.dll file
 * @return true if known version was been found, false when if not
 */
static bool getLunaHeuristicCapabilities(LunaLuaCapabilities &caps, const QString &path)
{
    QFileInfo p(path);

    auto dir = p.absoluteDir();

    if(dir.exists("config/game.ini"))
    {
        auto gsIni = dir.absoluteFilePath("config/game.ini");
        QFile gs(gsIni);
        if(!gs.open(QIODevice::ReadOnly|QIODevice::Text))
            return false;

        auto gt = gs.read(1000);

        if(gt.indexOf(HEURISTIC_SMBX2BETA3) >= 0)
        {
            fillCapsBeta3(caps);
            return true;
        }
        else if(gt.indexOf(HEURISTIC_SMBX2MAGLX3) >= 0)
        {
            fillCapsMAGLX3(caps);
            return true;
        }
        else if(gt.indexOf(HEURISTIC_SMBX2PAL) >= 0)
        {
            fillCapsPAL(caps);
            return true;
        }
        else if(gt.indexOf(HEURISTIC_SMBX2b4) >= 0)
        {
            fillCapsBeta4(caps);
            return true;
        }

        // Unsupported LunaLua toolchain
        return false;
    }

    // missing config/game.ini for a heuristic detection of SMBX2 version
    return false;
}


bool getLunaCapabilities(LunaLuaCapabilities &caps, const QString &path)
{
    caps.clear();

    if(!getRawLunaCapabilities(caps, path))
        return getLunaHeuristicCapabilities(caps, path);

    return true;
}
