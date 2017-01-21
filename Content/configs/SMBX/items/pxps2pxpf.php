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

    function saveIni($iniFile, $section)
    {
        $out_data = "[npc]\n";
        foreach($section as $param => &$value)
        {
            $out_data .= $param . " = " . needQuotes($value) . "\n";
        }
        file_put_contents($iniFile, $out_data);
    }

    for($npcID = 1; $npcID <=300; $npcID++)
    {
        //"SMBX-38a/items/npc/npc-".$npcID.".ini";
        $mainINI = parse_ini_file("./npc/npc-".$npcID.".ini", true, INI_SCANNER_RAW);
        if(isset($mainINI['npc']['speed']))
            $mainINI['npc']['speed'] = round(intval($mainINI['npc']['speed'])/65);
        saveIni("./npc/npc-".$npcID.".ini", $mainINI['npc']);
    }


