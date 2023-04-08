#ifndef TEXTDATAPROCESSOR_H
#define TEXTDATAPROCESSOR_H

#include "textdata.h"

class TextDataProcessor
{
public:
    TextDataProcessor();
    bool loadProject(const QString &directory, TranslateProject &proj);
    bool saveProject(const QString &directory, TranslateProject &proj);
    bool loadProjectLevel(const QString &file, TranslateProject &proj);
    bool saveProjectLevel(const QString &file, TranslateProject &proj);
    bool scanEpisode(const QString &directory, TranslateProject &proj);
    bool createTranslation(TranslateProject &proj, const QString &lang);

private:
    void importLevel(TranslationData &origin, const QString &path, const QString &shortPath);
    void importWorld(TranslationData &origin, const QString &path, const QString &shortPath);
    void importScript(TranslationData &origin, const QString &path, const QString &shortPath);
    bool saveJSONs(const QString &directory, TranslateProject &proj);
    void updateTranslation(TranslateProject &proj, const QString &trName);
    void loadTranslation(TranslateProject &proj, const QString &trName, const QString &filePath);
};

#endif // TEXTDATAPROCESSOR_H
