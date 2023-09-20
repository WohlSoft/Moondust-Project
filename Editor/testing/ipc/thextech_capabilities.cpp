/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "thextech_capabilities.h"
#include <QProcess>
#include <QFileInfo>
#include <QVariant>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

static QString s_getExecRawOutput(const QString &exec, const QStringList &args)
{
    QString ret;
    QProcess p;

    p.setProgram(exec);
    p.setArguments(args);

    p.start();
    p.waitForStarted(5000);

    while(p.state() == QProcess::Running)
    {
        if(!p.waitForReadyRead(5000))
            break;
        ret += QString::fromUtf8(p.readAllStandardOutput());
    }

    p.waitForFinished(5000);

    return ret;
}

static void fillCapsLegacy(TheXTechCapabilities &caps)
{
    caps.type = "legacy fallback";
    caps.version = "legacy";

    caps.ipcProtocols << "moondust-stdinout" << "--END--";
    caps.arguments << "c"
                   << "u"
                   << "frameskip"
                   << "no-frameskip"
                   << "no-sound"
                   << "never-pause"
                   << "bg-input"
                   << "render"
                   << "leveltest"
                   << "num-players"
                   << "battle"
                   << "player1"
                   << "player2"
                   << "god-mode"
                   << "grab-all"
                   << "show-fps"
                   << "max-fps"
                   << "magic-hand"
                   << "editor"
                   << "interprocessing"
                   << "compat-level"
                   << "speed-run-mode"
                   << "speed-run-semitransparent"
                   << "speed-run-blink-mode"
                   << "show-controls"
                   << "show-battery-status"
                   << "verbose"
                   << "levelpath"
                   << "--END--";

    caps.renders << "sw" << "hw" << "vsync" << "--END--";
    caps.features << "stars-number" << "editor" << "test-level-file" << "test-level-ipc" << "--END--";

    caps.isCompatible = true;
}

static void fillCapsFromJson(TheXTechCapabilities &caps, const QString &rawJson)
{
    QJsonDocument q = QJsonDocument::fromJson(rawJson.toUtf8());
    auto root = q.object();
    auto rKeys = root.keys();

    caps.type = "--capabilities";

    if(rKeys.contains("version"))
        caps.version = root["version"].toString();

    if(rKeys.contains("ipc-protocols"))
    {
        auto features = root["ipc-protocols"].toArray();
        auto vl = features.toVariantList();
        for(auto &q : vl)
            caps.ipcProtocols.insert(q.toString());
    }

    if(rKeys.contains("arguments"))
    {
        auto features = root["arguments"].toArray();
        auto vl = features.toVariantList();
        for(auto &q : vl)
            caps.arguments.insert(q.toString());
    }

    if(rKeys.contains("renders"))
    {
        auto features = root["renders"].toArray();
        auto vl = features.toVariantList();
        for(auto &q : vl)
            caps.renders.insert(q.toString());
    }

    if(rKeys.contains("features"))
    {
        auto features = root["features"].toArray();
        auto vl = features.toVariantList();
        for(auto &q : vl)
            caps.features.insert(q.toString());
    }

    caps.isCompatible = true;
}

bool getTheXTechCapabilities(TheXTechCapabilities &caps, const QString &path)
{
    caps.clear();

    QFileInfo f(path);
    QString input;

    if(!f.exists()) // Selected file doesn't exists
        return false;

    input = s_getExecRawOutput(path, {"--help"});

    if(!input.contains("--capabilities")) // Fill with legacy preset
        fillCapsLegacy(caps);
    else // Parse from JSON
    {
        input = s_getExecRawOutput(path, {"--capabilities"});
        fillCapsFromJson(caps, input);
    }

    caps.loaded = true;
    return true;
}
