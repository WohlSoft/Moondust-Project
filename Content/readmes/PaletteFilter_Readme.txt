PaletteFilter - this is a free console utility which can adapt given images into specific pallete

--------------------------------------------------------------------------------

USAGE:

   ./PaletteFilter  [-O </path/to/output/directory/>] -P <pallete.png> [-w]
                    [-d] [-n] [--] [--version] [-h] <Input file path(s)>
                    ...


Where:

   -O </path/to/output/directory/>,  --output </path/to/output/directory/>
     path to a directory where the fixed images will be saved

   -P <pallete.png>,  --pallete <pallete.png>
     (required)  Path to the reference PNG or GIF image file for a role of
     a pallete

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
     (required)  Input PNG file(s) (GIFs are not supported by this tool)

if -O is not specified, source images will be overriden.
--------------------------------------------------------------------------------
Examples:

On Windows:
PaletteFilter -P myPallete.png "C:\SMBX GFX PNG\npc" -O "C:\SMBX\graphics\npc"
PaletteFilter -P myPallete.png D:\XXX\graphics\block
PaletteFilter -P myPallete.png block-45.png

On Linux/Mac OS X
PaletteFilter -P myPallete.png "~/Graphics/PNG/npc" -O "~/Wine/SMBX/graphics/npc"
PaletteFilter -P myPallete.png /home/vasya/Images/xxx/gif
PaletteFilter -P myPallete.png . ../npc_png

--------------------------------------------------------------------------------


Change Log:
1.0.0
-First version

