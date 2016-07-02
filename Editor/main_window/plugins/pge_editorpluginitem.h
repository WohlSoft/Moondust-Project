#ifndef PGE_EDITORPLUGINITEM_H
#define PGE_EDITORPLUGINITEM_H


#include <QString>

class PGE_EditorPluginItem
{
public:
    PGE_EditorPluginItem(const QString& pluginName, const QString& authorName, const QString& description, int pluginVersionNumber);

    QString pluginName() const;
    QString authorName() const;
    int pluginVersionNum() const;
    QString description() const;

private:
    QString m_pluginName;
    QString m_authorName;
    QString m_description;
    int m_pluginVersionNum;
};

#endif // PGE_EDITORPLUGINITEM_H
