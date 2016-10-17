<?php

$page = "";

function retreivePageTitle($pagePath)
{
    global $page;
    $string = file_get_contents($pagePath);
    $xml = new DOMDocument;
    $xml->loadHTML($string);

    $head = $xml->getElementsByTagName ( "title" );
    $title = $head->item(0)->nodeValue;
    $page .= "<li><a href=\"". $pagePath ."\">". ($title != "" ? $title : $pagePath ) ."</a></li>\n";
}


function EndsWith($Haystack, $Needle){
    // Recommended version, using strpos
    return strrpos($Haystack, $Needle) === strlen($Haystack)-strlen($Needle);
}

function getDirContents($dir, &$results = array())
{
    global $page;
    $files = scandir($dir);
    foreach($files as $key => $value){
        $path = $dir. "/" .$value;
        if(!is_dir($path))
        {
            if(EndsWith($path, ".html"))
            {
                retreivePageTitle($path);
                $results[] = $path;
            }
        } 
        else if($value != "." && $value != "..") 
        {
            $page .= "<li><ul>";
            getDirContents($path, $results);
            $page .= "</ul></li>\n";
            $results[] = $path;
        }
    }

    return $results;
}

$page .= "<html><head><meta charset=\"utf-8\" /><title>Contents</title></head><body>\n";

$page .= "<ul>\n";

getDirContents(".");

$page .= "</ul>\n";

$page .= "</body></html>";

file_put_contents("auto-contents.htm", $page);

?>

