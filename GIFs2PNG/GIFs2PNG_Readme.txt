GIFs2PNG - this is a free console utility, what merge GIF images wilth his mask
and save them into PNG format.
--------------------------------------------------------------------------------
This is a part of the Platformer Game Engine by Wohlstand
--------------------------------------------------------------------------------
This program is distributed under the GNU GPLv3 license 
================================================================================
--------------------------------------------------------------------------------
Usage:
--------------------------------------------------------------------------------
Syntax:

   GIFs2PNG [--help] /path/to/folder [-O/path/to/out]

 --help              - Display this help
 /path/to/folder     - path to a directory with pairs of GIF files
 -O/path/to/out      - path to a directory where the PNG images will be saved
 -R                  - Remove source images after succesfull converting

if -O will not be defined, PNG images will be saves in same folder, where placed GIF images.
--------------------------------------------------------------------------------
Examples:

On Windows:
GIFs2PNG C:\SMBX\graphics\npc "-OC:\SMBX GFX PNG\npc"
GIFs2PNG D:\XXX\graphics\block

On Linux/MacOS X
GIFs2PNG ~/Wine/SMBX/graphics/npc "-O~/Graphics/PNG/npc"
GIFs2PNG /home/vasya/Images/xxx/gif
GIFs2PNG . ../npc_png

--------------------------------------------------------------------------------
