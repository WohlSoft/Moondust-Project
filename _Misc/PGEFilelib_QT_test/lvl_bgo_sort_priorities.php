#!/usr/bin/php
<?php

$handle = fopen("BGO_Z-Order_calculation.csv", "r");
$bini = parse_ini_file("lvl_bgo.ini", true);

if($handle && $bini)
{
    $iniSets = array();

    function putVal($bgoID, $val)
    {
        global $bini;
        return (isset($bini["background-".$bgoID][$val]) ? $bini["background-".$bgoID][$val] : "");
    }

    $iniOut = "[background-main]\ntotal=190\n\n";

    /*
    [background-3]
    image=background-3.gif
    name=Hills 2
    group="Super Mario Bros. 3"
    category=Greens
    grid=32
    view=background
    climbing=0
    animated=0
    frames=1
    frame-speed=125
    smbx64-sort-priority=27
    */

    while(($line = fgets($handle)) !== false)
    {
        if($line=="") continue;

        $line = str_replace("\n", "", $line);
        $params = explode(",", $line);

        $bgoID = $params[1];
        $sp1x  = $params[0];
        $sp4x  = $params[2];
        $view  = $params[3];
        $zoffset  = $params[4];

        $iniSets[$bgoID] = array();
        $iniSets[$bgoID]['image'] = putVal($bgoID, "image");
        $iniSets[$bgoID]['name'] = putVal($bgoID, "name");
        $iniSets[$bgoID]['group'] = putVal($bgoID, "group");
        $iniSets[$bgoID]['category'] = putVal($bgoID, "category");
        $iniSets[$bgoID]['grid'] = putVal($bgoID, "grid");
        $iniSets[$bgoID]['offset-x'] = putVal($bgoID, "offset-x");
        $iniSets[$bgoID]['offset-y'] = putVal($bgoID, "offset-y");
        $iniSets[$bgoID]['view'] = $view;
        $iniSets[$bgoID]['z-offset'] = $zoffset;
        $iniSets[$bgoID]['climbing'] = $bini["background-".$bgoID]["climbing"];
        $iniSets[$bgoID]['animated'] = $bini["background-".$bgoID]["animated"];
        $iniSets[$bgoID]['frames'] = putVal($bgoID, "frames");
        $iniSets[$bgoID]['frame-speed'] = putVal($bgoID, "frame-speed");
        $iniSets[$bgoID]['smbx64-sort-priority'] = $sp4x;
        $iniSets[$bgoID]['display-frame'] = putVal($bgoID, "display-frame");
    }
    fclose($handle);

    //print_r($iniSets[$bgoID]);

    $out = fopen("lvl_bgo_out.ini", "w");

    fputs($out, $iniOut);

    echo "const int _smbx64_bgo_sort_priorities[190] = {\n";
    $bgosList = "    ";

    for($i = 1; $i<=190; $i++)
    {

        $iniEntry = "[background-".$i."]\n";
        $iniEntry .= "image=\"background-".$i.".gif\"\n";
        $iniEntry .= "name=\"".$iniSets[$i]['name']."\"\n";
        $iniEntry .= "group=\"".$iniSets[$i]['group']."\"\n";
        $iniEntry .= "category=\"".$iniSets[$i]['category']."\"\n";
        if($iniSets[$i]['grid'] != 32)
            $iniEntry .= "grid=".$iniSets[$i]['grid']."\n";
        if(!empty($iniSets[$i]['offset-x']))
            $iniEntry .= "offset-x=".$iniSets[$i]['offset-x']."\n";
        if(!empty($iniSets[$i]['offset-y']))
            $iniEntry .= "offset-y=".$iniSets[$i]['offset-y']."\n";
        $iniEntry .= "view=".$iniSets[$i]['view']."\n";
        if(!empty($iniSets[$i]['z-offset']))
            $iniEntry .= "z-offset=".$iniSets[$i]['z-offset']."\n";
        if(!empty($iniSets[$i]['climbing']))
            $iniEntry .= "climbing=".$iniSets[$i]['climbing']."\n";
        if($iniSets[$i]['animated'] != 0)
            $iniEntry .= "animated=".$iniSets[$i]['animated']."\n";
        if( $iniSets[$i]['frames'] > 1 )
            $iniEntry .= "frames=".$iniSets[$i]['frames']."\n";
        if($iniSets[$i]['animated'] != 0)
            $iniEntry .= "frame-speed=".$iniSets[$i]['frame-speed']."\n";
        $iniEntry .= "smbx64-sort-priority=".$iniSets[$i]['smbx64-sort-priority']."\n";
        if(!empty($iniSets[$bgoID]['display-frame']))
            $iniEntry .= "display-frame=".$iniSets[$i]['display-frame']."\n";

        $iniEntry .= "\n";

        fputs($out, $iniEntry);

        $bgosList .= $iniSets[$i]['smbx64-sort-priority'].( $i<190 ? "," : "\n" );
        if(strlen($bgosList)>84)
        {
            echo $bgosList."\n";
            $bgosList = "    ";
        }
    }

    echo $bgosList;

    echo "};\n";

    fclose($out);

}
else 
{
    echo "shit happens!\n";
} 


