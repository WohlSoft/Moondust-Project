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
    $ini_array = parse_ini_file($iniFile, true);
    foreach($ini_array as $key => &$section)
    {
        //echo $key;
        $out_data = "[".$sectionName."]\n";
        foreach($section as $param => &$value)
        {
            $out_data .= $param . " = " . needQuotes($value) . "\n";
        }
        if(!EndsWith($key, "-main"))
            file_put_contents($targetDirectory . $key . ".ini", $out_data);
    }
}


