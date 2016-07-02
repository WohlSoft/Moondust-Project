#ifndef PGE_EDITORPLUGINITEM_H
#define PGE_EDITORPLUGINITEM_H


#include <QString>

class PGE_EditorPluginItem
{
public:
    PGE_EditorPluginItem(const QString& pluginName, const QString& authorName, int pluginVersionNumber);

    QString pluginName() const;
    QString authorName() const;
    int pluginVersionNum() const;

private:
    QString m_pluginName;
    QString m_authorName;
    int m_pluginVersionNum;
};

#endif // PGE_EDITORPLUGINITEM_H
