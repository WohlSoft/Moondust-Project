GIFs2PNG - this is a free console utility, which merges GIF images with their mask
and saves them into PNG format.
--------------------------------------------------------------------------------
This is a part of the Platformer Game Engine by Wohlstand
--------------------------------------------------------------------------------
This program is distributed under the GNU GPLv3 license
================================================================================
USAGE:

   ./GIFs2PNG  [-C </path/to/config/pack>] [-O
               </path/to/output/directory/>] [-w] [-d] [-b] [-r] [--]
               [--version] [-h] <Input file path(s)> ...


Where:

   -C </path/to/config/pack>,  --config </path/to/config/pack>
     Allow usage of default masks from specific PGE config pack (Useful for
     cases where the GFX designer didn't make a mask image)

   -O </path/to/output/directory/>,  --output </path/to/output/directory/>
     path to a directory where the PNG images will be saved

   -w,  --dig-recursive-old
     Look for images in subdirectories [deprecated]

   -d,  --dig-recursive
     Look for images in subdirectories

   -b,  --ingnore-bg
     Skip all "background2-*.gif" sprites (due a bug in the LunaLUA)

   -r,  --remove
     Remove source images after a succesful conversion

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.

   <Input file path(s)>  (accepted multiple times)
     (required)  Input GIF file(s)

--------------------------------------------------------------------------------
Examples:

On Windows:
GIFs2PNG C:\SMBX\graphics\npc -O "C:\SMBX GFX PNG\npc"
GIFs2PNG D:\XXX\graphics\block

On Linux/MacOS X
GIFs2PNG ~/Wine/SMBX/graphics/npc -O "~/Graphics/PNG/npc"
GIFs2PNG /home/vasya/Images/xxx/gif
GIFs2PNG . ../npc_png

--------------------------------------------------------------------------------

Change Log:

3.0.0
- Reworked algorithm to be more accurate and optimized
- Using TClap for command line parsing
- No more Qt dependency, instead of QImage/QPixmap now is using a libFreeImage

1.0.5
-Added support for conversion by filelist instead whole directories
-Fixed output path bug

1.0.1
-Added walking in subdirectories

1.0.0
-First version
