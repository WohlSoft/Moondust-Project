LazyFixTool - this is a free console utility, for fix the lazily-made image masks
and also, convert all BMPs into GIF
--------------------------------------------------------------------------------
This is a part of Platformer Game Engine by Wohlstand
--------------------------------------------------------------------------------
This program distributed under GNU GPLv3
================================================================================
This utility will fix lazily-made image masks:
============================================================================
Syntax:

   LazyFixTool [--help] /path/to/folder [-O/path/to/out] [-W] [-N] [-G]

 --help              - Display this help
 /path/to/folder     - path to directory with pair of GIF files
 -O/path/to/out      - path to directory where will be saved new images
                       Note: (with -W flag will be ingored)
 -W                  - Walk in subdirectores
 -N                  - Don't create backup
 -G                  - Make gray shades on masks is more dark



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
