<?php


function needQuotes($value)
{
    if($value == (string)intval($value))
        return $value;
    if($value == (string)doubleval($value))
        return $value;
    if($value == "true")
        return $value;
    if($value == "false")
        return $value;
    return "\"".$value."\"";
}

function EndsWith($Haystack, $Needle){
    // Recommended version, using strpos
    return strrpos($Haystack, $Needle) === strlen($Haystack)-strlen($Needle);
}

function splitIni($iniFile, $targetDirectory, $sectionName)
{
    if(!is_dir($targetDirectory))
        mkdir($targetDirectory);
    $ini_array = parse_ini_file($iniFile, true, INI_SCANNER_RAW);
    foreach($ini_array as $key => &$section)
    {
        //echo $key;
        if(!EndsWith($key, "-main"))
            $out_data = "[".$sectionName."]\n";
        else
            $out_data = "[".$key."]\n";
        foreach($section as $param => &$value)
        {
            $out_data .= $param . " = " . needQuotes($value) . "\n";
        }
        if(!EndsWith($key, "-main"))
            file_put_contents($targetDirectory . "/" . $key . ".ini", $out_data);
        else
        {
            $out_data .= "config-dir = " . needQuotes($targetDirectory) . "\n";
            file_put_contents($iniFile, $out_data);
        }
    }
}

echo "\n";

if(!is_dir("items"))
    mkdir("items");

splitIni("lvl_blocks.ini", "items/blocks", "block");
splitIni("lvl_bgo.ini", "items/bgo", "background");
splitIni("lvl_npc.ini", "items/npc", "npc");
splitIni("lvl_bkgrd.ini", "items/backgrounds", "background2");

splitIni("wld_tiles.ini", "items/terrain", "tile");
splitIni("wld_scenery.ini", "items/scenery", "scenery");
splitIni("wld_paths.ini", "items/paths", "path");
splitIni("wld_levels.ini", "items/levels", "level");

echo "\n";

