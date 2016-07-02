#ifndef PGE_EDITORPLUGINMANAGER_H
#define PGE_EDITORPLUGINMANAGER_H

#include <QObject>
#include <QDir>
#include <js_engine/pge_jsengine.h>
#include "pge_editorpluginitem.h"

class PGE_EditorPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PGE_EditorPluginManager(QObject *parent = 0);

    bool hasPlugins() const;
    void loadPluginsInDir(const QDir& dir);


private:
    QList<PGE_EditorPluginItem> m_loadedPlugins;
    PGE_JsEngine m_engine;
};

#endif // PGE_EDITORPLUGINMANAGER_H
