#ifndef PGE_EDITORPLUGININFO_H
#define PGE_EDITORPLUGININFO_H

#include <QDialog>
#include "pge_editorpluginmanager.h"


namespace Ui {
class PGE_EditorPluginInfo;
}

class PGE_EditorPluginInfo : public QDialog
{
    Q_OBJECT

public:
    constexpr static int UserRolePluginIndex = Qt::UserRole + 1;

    explicit PGE_EditorPluginInfo(PGE_EditorPluginManager* pluginManager, QWidget *parent = 0);
    ~PGE_EditorPluginInfo();

private:
    PGE_EditorPluginManager* m_pluginManager;
    Ui::PGE_EditorPluginInfo *ui;
};

#endif // PGE_EDITORPLUGININFO_H
