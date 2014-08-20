PNG2GIFs - this is a free console utility, which converts PNG images
into the SMBX GIF format with masks.
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
 /path/to/folder     - path to a directory with pairs of PNG files
 -O/path/to/out      - path to a directory where the GIF images will be saved
 -R                  - Remove source images after successful conversion
 -W                  - Also look for images in subdirectories

if -O is not specified, GIF images will be saved in the same folder as where you placed the PNG images.
--------------------------------------------------------------------------------
Examples:

On Windows:
PNG2GIFs "C:\SMBX GFX PNG\npc" -O C:\SMBX\graphics\npc 
PNG2GIFs D:\XXX\graphics\block
PNG2GIFs block-45.png

On Linux/Mac OS X
PNG2GIFs "~/Graphics/PNG/npc" -O ~/Wine/SMBX/graphics/npc
PNG2GIFs /home/vasya/Images/xxx/gif
PNG2GIFs . ../npc_png

--------------------------------------------------------------------------------


Change Log:

1.0.0
-First version
