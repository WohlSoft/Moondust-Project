LazyFixTool is a free console utility for fixing lazily-made image masks
and converting all BMPs into GIFs
--------------------------------------------------------------------------------
This is a part of the Platformer Game Engine by Wohlstand
--------------------------------------------------------------------------------
This program is distributed under the GNU GPLv3 license
================================================================================
This utility will fix lazily-made image masks:
============================================================================

USAGE:

   ./LazyFixTool  [-O </path/to/output/directory/>] [-w] [-d] [-n] [--]
                  [--version] [-h] <Input file path(s)> ...


Where:

   -O </path/to/output/directory/>,  --output </path/to/output/directory/>
     path to a directory where the fixed images will be saved

   -w,  --dig-recursive-old
     Look for images in subdirectories [deprecated]

   -d,  --dig-recursive
     Look for images in subdirectories

   -n,  --no-backup
     Don't create backup

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.

   <Input file path(s)>  (accepted multiple times)
     (required)  Input GIF file(s)


if -O will not be defined, the new images will be saved in the same folder as where the GIF images are placed.
--------------------------------------------------------------------------------
Examples:

On Windows:
LazyFixTool C:\SMBX\graphics\npc -O "C:\SMBX GFX PNG\npc"
LazyFixTool D:\XXX\graphics\block

On Linux/MacOS X
LazyFixTool ~/Wine/SMBX/graphics/npc -O "~/Graphics/PNG/npc"
LazyFixTool /home/vasya/Images/xxx/gif
LazyFixTool . ../npc_png

--------------------------------------------------------------------------------

Changelog:
3.0.0
- Reworked algorithm to be more accurate and optimized
- Using TClap for command line parsing
- No more Qt dependency, instead of QImage/QPixmap now is using a libFreeImage

2.0.0
-Implemented new better algorithm of fixing graphics which can mask more accurate mask

1.3.0
-Fixed backup directory creating
-Added support for single-fine conversion

