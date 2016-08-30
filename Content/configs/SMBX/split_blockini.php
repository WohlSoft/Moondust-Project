<?php

require_once("_ini_splitter.php");
echo "\n";

if(!is_dir("items"))
    mkdir("items");

splitIni("lvl_blocks.ini", "items/blocks/", "block");
splitIni("lvl_bgo.ini", "items/bgo/", "background");
splitIni("lvl_npc.ini", "items/npc/", "npc");
splitIni("lvl_bkgrd.ini", "items/backgrounds/", "background2");

splitIni("wld_tiles.ini", "items/terrain/", "tile");
splitIni("wld_scenery.ini", "items/scenery/", "scenery");
splitIni("wld_paths.ini", "items/scenery/", "path");
splitIni("wld_levels.ini", "items/levels/", "level");

echo "\n";

