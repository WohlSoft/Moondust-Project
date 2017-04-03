PNG2GIFs - this is a free console utility, which converts PNG images
into the SMBX GIF format with masks.
--------------------------------------------------------------------------------
This is a part of the Platformer Game Engine by Wohlstand
--------------------------------------------------------------------------------
This program is distributed under the GNU GPLv3 license
================================================================================
GitHub Repo:
https://github.com/Wohlhabend-Networks/PlatGEnWohl
--------------------------------------------------------------------------------
Usage:
--------------------------------------------------------------------------------
To use tool you need simply run it without any arguments, but will be opened
GUI window which giving able to drag&drop files into them.

You can use tool for batch conversion if you will use it with arguments:
--------------------------------------------------------------------------------
USAGE:

   ./PNG2GIFs  [-O </path/to/output/directory/>] [-w] [-d] [-r] [--]
               [--version] [-h] <Input file path(s)> ...


Where:

   -O </path/to/output/directory/>,  --output </path/to/output/directory/>
     path to a directory where the converted images will be saved

   -w,  --dig-recursive-old
     Look for images in subdirectories [deprecated]

   -d,  --dig-recursive
     Look for images in subdirectories

   -r,  --remove
     Remove source images after successful conversion

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.

   <Input file path(s)>  (accepted multiple times)
     (required)  Input GIF file(s)

if -O is not specified, GIF images will be saved in the same folder as where you placed the PNG images.
--------------------------------------------------------------------------------
Examples:

On Windows:
PNG2GIFs "C:\SMBX GFX PNG\npc" -O "C:\SMBX\graphics\npc"
PNG2GIFs D:\XXX\graphics\block
PNG2GIFs block-45.png

On Linux/Mac OS X
PNG2GIFs "~/Graphics/PNG/npc" -O "~/Wine/SMBX/graphics/npc"
PNG2GIFs /home/vasya/Images/xxx/gif
PNG2GIFs . ../npc_png

--------------------------------------------------------------------------------


Change Log:

2.0.0
- Reworked algorithm to be more accurate and optimized
- Using TClap for command line parsing
- No more Qt dependency, instead of QImage/QPixmap now is using a libFreeImage

1.1.0
-Added User-Friendly GUI

1.0.0
-First version

