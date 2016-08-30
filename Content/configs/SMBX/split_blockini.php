<?php

echo "\n";
$ini_array = parse_ini_file("lvl_blocks.ini", true);
$targetDirectory = "items/blocks/";
$sectionName = "block";

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

foreach($ini_array as $key => &$section)
{
    echo $key;
    $out_data = "[".$sectionName."]\n";
    foreach($section as $param => &$value)
    {
        $out_data .= $param . " = " . needQuotes($value) . "\n";
    }
    file_put_contents($targetDirectory . $key . ".ini", $out_data);
}
echo "\n";

