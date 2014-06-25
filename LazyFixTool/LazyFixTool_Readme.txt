LazyFixTool - this is a free console utility, what fixing lazily-made image masks
--------------------------------------------------------------------------------
This is a part of Platformer Game Engine by Wohlstand
--------------------------------------------------------------------------------
This programm distributed under GNU GPLv3
================================================================================
--------------------------------------------------------------------------------
Usage:
--------------------------------------------------------------------------------
Syntax:

   LazyFixTool [--help] /path/to/folder [-O/path/to/out]

 --help              - Display this help
 /path/to/folder     - path to directory with pair of GIF files
 -O/path/to/out      - path to directory where will be saved PNG images

if -O will not be defined, PNG images will be saves in same folder, where placed GIF images.
--------------------------------------------------------------------------------
Examples:

On Windows:
LazyFixTool C:\SMBX\graphics\npc "-OC:\SMBX GFX PNG\npc"
LazyFixTool D:\XXX\graphics\block

On Linux/MacOS X
LazyFixTool ~/Wine/SMBX/graphics/npc "-O~/Graphics/PNG/npc"
LazyFixTool /home/vasya/Images/xxx/gif
LazyFixTool . ../npc_png

--------------------------------------------------------------------------------
