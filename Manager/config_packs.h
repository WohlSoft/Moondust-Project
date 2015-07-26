#ifndef CONFIG_PACKS_H
#define CONFIG_PACKS_H

#define _DEF_CONFIG_REPO "http://download.gna.org/pgewohlstand/configs/"
#define _CPACK_INDEX_FILE "configs.index"

#include <QString>
#include <QList>

struct ConfigPackRepo{
    ConfigPackRepo();
    QString url;
    bool enabled;
};

struct ConfigPackInfo{
    QString name;
    QString url;
    long lastupdate;
};

extern QList<ConfigPackRepo > cpacks_reposList;
extern QList<ConfigPackInfo > cpacks_List;

#endif // CONFIG_PACKS_H
