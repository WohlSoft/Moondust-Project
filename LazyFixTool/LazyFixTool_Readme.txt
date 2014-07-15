LazyFixTool is a free console utility for fixing lazily-made image masks
and converting all BMPs into GIFs
--------------------------------------------------------------------------------
This is a part of the Platformer Game Engine by Wohlstand
--------------------------------------------------------------------------------
This program is distributed under the GNU GPLv3 license 
================================================================================
This utility will fix lazily-made image masks:
============================================================================
Syntax:

   LazyFixTool [--help] /path/to/folder [-O/path/to/out] [-W] [-N] [-G]

 --help              - Display this help
 /path/to/folder     - path to a directory with a pair of GIF files
 -O/path/to/out      - path to a directory where the new images will be saved
                       Note: (with -W flag will be ignored)
 -W                  - Walk in subdirectores
 -N                  - Don't create backup
 -G                  - Make gray shades on masks is more dark



if -O will not be defined, the PNG images will be saved in the same folder, where the GIF images are placed.
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