/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "translator_main.h"
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <common_features/logger.h>
#include <common_features/app_path.h>
#include <Utf8Main/utf8main.h>
#include "version.h"


int main(int argc, char *argv[])
{
    QApplication::addLibraryPath( "." );
    QApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );
    QStringList args;

    QApplication::setOrganizationName(V_COMPANY);
    QApplication::setOrganizationDomain(V_PGE_URL);
    QApplication::setApplicationName("Moondust Translator");

    QApplication a(argc, argv);
#ifdef _WIN32
    for(int i = 0; i < argc; i++)
        args.push_back(QString::fromUtf8(argv[i]));
#else
    args    = a.arguments();
#endif

    TranslatorMain w;

    w.show();

    return a.exec();
}
