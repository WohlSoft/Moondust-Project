#ifndef TOOLBOXES_PROTOS_H
#define TOOLBOXES_PROTOS_H

class TilesetItemBox;

class LvlItemProperties;
class LvlWarpBox;
class LvlSectionProps;
class LevelItemBox;
class LvlSearchBox;
class LvlLayersBox;

class WorldItemBox;
class WLD_ItemProps;
class WldSearchBox;

#define MainWindowFriends \
    friend class TilesetItemBox;\
    friend class LvlItemProperties;\
    friend class LvlWarpBox;\
    friend class LvlSectionProps;\
    friend class LevelItemBox;\
    friend class LvlSearchBox;\
    friend class LvlLayersBox;\
    \
    friend class WorldItemBox;\
    friend class WLD_ItemProps;\
    friend class WldSearchBox;


#endif // TOOLBOXES_PROTOS_H
