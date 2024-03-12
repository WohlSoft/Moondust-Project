/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef TEXTDATAPROCESSOR_H
#define TEXTDATAPROCESSOR_H

#include "textdata.h"

class TextDataProcessor
{
public:
    TextDataProcessor();
    void toI18N(const QString &directory);
    bool loadProject(const QString &directory, TranslateProject &proj);
    bool saveProject(const QString &directory, TranslateProject &proj);
    bool loadProjectLevel(const QString &file, TranslateProject &proj);
    bool saveProjectLevel(const QString &file, TranslateProject &proj);
    bool scanEpisode(const QString &directory, TranslateProject &proj);
    bool scanSingleLevel(const QString &file, TranslateProject &proj);
    bool createTranslation(TranslateProject &proj, const QString &lang);

private:
    void importLevel(TranslationData &origin, const QString &path, const QString &shortPath);
    void importWorld(TranslationData &origin, const QString &path, const QString &shortPath);
    void importScript(TranslationData &origin, const QString &path, const QString &shortPath);
    void recountStats(TranslateProject &proj, TranslationData &tr, bool isOrigin);
    bool saveJSONs(const QString &directory, const TranslateProject &proj);
    void updateTranslation(TranslateProject &proj, const QString &trName);
    void loadTranslation(TranslateProject &proj, const QString &trName, const QString &filePath);
};

#endif // TEXTDATAPROCESSOR_H
