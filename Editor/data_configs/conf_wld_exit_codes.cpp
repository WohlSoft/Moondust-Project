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

#include <QFile>
#include "data_configs.h"

void DataConfig::loadExitCodes()
{
    size_t total = 0;
    meta_exit_codes.clear();

    QString exitcodes_ini = config_dir + "wld_exit_types.ini";

    if(!QFile::exists(exitcodes_ini))
    {
        LogDebug("Custom exit codes wasn't load: wld_exit_types.ini doesn't exist");
        return;
    }

    IniProcessing ec_set(exitcodes_ini);

    ec_set.beginGroup("main");
    ec_set.read("total", total, 0);
    ec_set.endGroup();

    for(size_t i = 1; i <= total; ++i)
    {
        meta_wld_exitcodes ec;
        ec.code = (int)i;
        auto group = QString("exit-%1").arg(i).toStdString();

        if(!ec_set.contains(group))
        {
            ec.built_in = true;
            meta_exit_codes.push_back(ec);
            continue;
        }

        ec_set.beginGroup(group);
        ec_set.read("built-in", ec.built_in, false);
        ec_set.read("disabled", ec.disabled, false);
        ec_set.read("name", ec.name, QString());
        auto k = ec_set.allKeys();
        for(auto &kk : k)
        {
            auto ks = QString::fromStdString(kk);
            if(!ks.startsWith("name-"))
                continue;
            auto lang = ks.mid(5);
            ec_set.read(kk.c_str(), ec.name_lang[lang], QString());
        }
        ec_set.endGroup();

        meta_exit_codes.push_back(ec);
    }
}
