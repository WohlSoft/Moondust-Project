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

   LazyFixTool [--help] [-N] [-G] file1.gif [file2.gif] [...] [-O/path/to/out]
   LazyFixTool [--help] [-W] [-N] [-G] /path/to/folder [-O/path/to/out]

 --help              - Display this help
 /path/to/folder     - path to a directory with a pair of GIF files
 -O/path/to/out      - path to a directory where the new images will be saved
 -W                  - Also look for images in subdirectories
 -N                  - Don't create backup

 -G                  - Make gray shades on masks darker (available before v.2.0)



if -O will not be defined, the new images will be saved in the same folder as where the GIF images are placed.
--------------------------------------------------------------------------------
Examples:

On Windows:
LazyFixTool C:\SMBX\graphics\npc -O"C:\SMBX GFX PNG\npc"
LazyFixTool D:\XXX\graphics\block

On Linux/MacOS X
LazyFixTool ~/Wine/SMBX/graphics/npc -O~/Graphics/PNG/npc
LazyFixTool /home/vasya/Images/xxx/gif
LazyFixTool . ../npc_png

--------------------------------------------------------------------------------

Changelog:
2.0.0
-Implemented new better algorithm of fixing graphics which can mask more accurate mask

1.3.0
-Fixed backup directory creating
-Added support for single-fine conversion
