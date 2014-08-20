PNG2GIFs - this is a free console utility, what convert PNG images
into SMBX GIF format with using masks.
--------------------------------------------------------------------------------
This is a part of the Platformer Game Engine by Wohlstand
--------------------------------------------------------------------------------
This program is distributed under the GNU GPLv3 license 
================================================================================
--------------------------------------------------------------------------------
Usage:
--------------------------------------------------------------------------------
Syntax:

   PNG2GIFs [--help] [-R]  file1.png [file2.png] [...] [-O/path/to/out]
   PNG2GIFs [--help] [-R] [-W] /path/to/folder [-O/path/to/out]

 --help              - Display this help
 /path/to/folder     - path to a directory with pairs of GIF files
 -O/path/to/out      - path to a directory where the GIF images will be saved
 -R                  - Remove source images after succesfull converting
 -W                  - Walk in subdirectores

if -O will not be defined, GIF images will be saves in same folder, where placed GIF images.
--------------------------------------------------------------------------------
Examples:

On Windows:
PNG2GIFs C:\SMBX\graphics\npc -O"C:\SMBX GFX PNG\npc"
PNG2GIFs D:\XXX\graphics\block
PNG2GIFs block-45.png

On Linux/MacOS X
PNG2GIFs ~/Wine/SMBX/graphics/npc -O"~/Graphics/PNG/npc"
PNG2GIFs /home/vasya/Images/xxx/gif
PNG2GIFs . ../npc_png

--------------------------------------------------------------------------------


Change Log:

1.0.0
-First version