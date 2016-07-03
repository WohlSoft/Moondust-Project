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

    void loadPluginsInDir(const QDir& dir);

    bool hasPlugins() const;
    unsigned int getPluginCount() const;
    const PGE_EditorPluginItem* getPluginInfo(unsigned int index) const;

private:
    QList<PGE_EditorPluginItem*> m_plugins; // TODO: Own class for failed plugins?
    PGE_JsEngine m_engine;
};

#endif // PGE_EDITORPLUGINMANAGER_H
