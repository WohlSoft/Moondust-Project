#ifndef TOOLBOXES_PROTOS_H
#define TOOLBOXES_PROTOS_H

class TilesetItemBox;
class BookmarksBox;
class DebuggerBox;

class LvlItemProperties;
class LvlWarpBox;
class LvlSectionProps;
class LevelItemBox;
class LvlSearchBox;
class LvlLayersBox;

class WorldItemBox;
class WLD_ItemProps;
class WldSearchBox;
class DockVizibilityManager;

#define MainWindowFriends \
    friend class TilesetItemBox;\
    friend class BookmarksBox;\
    friend class DebuggerBox;\
    \
    friend class LevelItemBox;\
    friend class LvlItemProperties;\
    friend class LvlWarpBox;\
    friend class LvlSectionProps;\
    friend class LvlSearchBox;\
    friend class LvlLayersBox;\
    \
    friend class WorldItemBox;\
    friend class WLD_ItemProps;\
    friend class WldSearchBox;


#endif // TOOLBOXES_PROTOS_H
