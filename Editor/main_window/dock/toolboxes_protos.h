#pragma once
#ifndef TOOLBOXES_PROTOS_H
#define TOOLBOXES_PROTOS_H

class DockVizibilityManager;

class TilesetItemBox;
class BookmarksBox;
class DebuggerBox;
class VariablesBox;

class LvlItemProperties;
class LvlWarpBox;
class LvlSectionProps;
class LevelItemBox;
class LvlSearchBox;
class LvlLayersBox;
class LvlEventsBox;

class WorldSettingsBox;
class WorldItemBox;
class WorldMusicBoxItemBox;
class WLD_ItemProps;
class WldSearchBox;


#define MainWindowFriends \
    friend class TilesetItemBox;\
    friend class BookmarksBox;\
    friend class DebuggerBox;\
    friend class VariablesBox;\
    friend class ItemSelectDialog;\
    \
    friend class LevelItemBox;\
    friend class LvlItemProperties;\
    friend class LvlWarpBox;\
    friend class LvlSectionProps;\
    friend class LvlSearchBox;\
    friend class LvlLayersBox;\
    friend class LvlEventsBox;\
    \
    friend class WorldSettingsBox;\
    friend class WorldItemBox;\
    friend class WorldMusicBoxItemBox;\
    friend class WLD_ItemProps;\
    friend class WldSearchBox;

#endif // TOOLBOXES_PROTOS_H
