<?php

$tr = simplexml_load_file("editor_en.ts");

$foundCrap = array();

foreach($tr->context as $ctx)
{
    foreach($ctx->message as $msg)
    {
        if(strcmp($msg->source, $msg->translation) != 0)
        {
            if(isset($msg->translation->attributes()["type"]))
            {
                //echo "THIS CRAP IS " . $msg->translation->attributes()["type"] . "\n";
                //Ignore vanished shit
                continue;
            }

            $foundCrap[(string)$msg->source] = $msg->translation;
        }
    }
}

$count = 0;

foreach($foundCrap as $msg => $trans)
{
            echo ++$count . ")\n====================\n" .
            $msg .
            "\n--------------------\n".
            $trans .
            "\n====================\n";
}

