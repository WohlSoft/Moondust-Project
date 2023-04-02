#ifndef TEXTDATAPROCESSOR_H
#define TEXTDATAPROCESSOR_H

#include "textdata.h"

class TextDataProcessor
{
public:
    TextDataProcessor();
    bool loadProject(const QString &directory, TranslateProject &proj);

private:
    void importLevel(TranslationData &origin, const QString &path, const QString &shortPath);
    void importWorld(TranslationData &origin, const QString &path, const QString &shortPath);
    void importScript(TranslationData &origin, const QString &path, const QString &shortPath);
    void saveJSONs(const QString &directory, TranslateProject &proj);
};

#endif // TEXTDATAPROCESSOR_H
