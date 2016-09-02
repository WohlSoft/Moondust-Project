<?php
/*
    A small script which renames everything from Mario universe
    into universe created by Talkhaus a free community of gamers and rom-hackers.
*/

$characters = [
    ["name=\"Peach\"", "name=\"Kood\""],
    ["name=\"Mario\"", "name=\"Demo\""],
    ["name=\"Luigi\"", "name=\"Iris\""],
    ["name=\"Toad\"", "name=\"Raocow\""],
    ["name=\"Link\"", "name=\"Steath\""]
];

$groups = [
    ["\"Super Mario Bros.\"",      "\"A2XT Set #1\""],
    ["\"Super Mario Bros. 2\"",    "\"A2XT Set #2\""],
    ["\"Super Mario Bros. 3\"",    "\"A2XT Set #3\""],
    ["\"Super Mario World\"",      "\"A2XT Set #4\""]
];

$furbas = [
    ["Goomba", "Furba"],
    ["goomba", "furba"],
    ["Gloomba", "Furba"],
    ["Galoomba Stomped", "Furba Stomped"],
    ["Galoomba", "Furba"],
    ["(SMW)", "Nut-Head"],
];

$goopas = [
    ["Koopa Troopa", "Goopa"],
    ["Koopa Para-Troopa", "Para-Goopa"],
    ["Beach Koopa", "Shelless Goopa"],
    ["SMW", "Small"],
];

$radish = [
    ["Super Mushroom", "Radish"],
    ["1-Up Mushroom", "1-Up Radish"]
];

$bots = [
    ["Bot", "Spring"],
    ["Bit", "Spring (Red)"]
];

$drillrozes = [
    ["Piranha Plant (Green)", "Drill Rose"],
    ["Piranha Plant (Red, Upside-Down)", "Drill Rose"],
    ["Piranha Plant (Red Horizontal)", "Drill Rose"]
];

$hands = [
    ["Big Piranha Plant (Upside-Down)", "Hand"],
    ["Big Piranha Plant", "Hand"]
];

$catnips = [
    ["Yoshi (Green)", "Catnip (Green)"],
    ["Yoshi (Blue)",  "Catnip (Blue)"],
    ["Yoshi (Red)",  "Catnip (Red)"],
    ["Yoshi (Yellow)",  "Catnip (Yellow)"],
    ["Yoshi (Pink)",  "Catnip (Khaki)"],
    ["Yoshi (Purple)","Catnip (Brown)"],
    ["Yoshi (Cyan)",  "Catnip (White)"],
    ["Yoshi (Black)", "Catnip (Raibow)"]
];

$sfxNames = [
    ["Yoshi", "Catnip"],
    ["Zelda", "Steath"],
    ["Got Star Exit", "Got a Leek Exit"],
    ["SMB3 Game Beat", "A2XT Victory"],
    ["Bowser Killed", "Boss Killed"],
    ["Birdo Spit", "Spit"],
    ["Birdo Hit", "Boss hit"],
    ["Birdo Beat", "Boss killed"],
    ["SMB2 Exit", "Mason jar exit"],
    ["SMB3 Exit", "Roulette exit"],
    ["Dungeon Clear", "Money bag exit"],
    ["Sonic Ring", "Golden Tooth"],
    ["Wart Bubbles", "Duck Quacking"],
    ["Wart Killed", "Duck defeat"],
    ["SMB2 Throw", "Throw"],
    ["SMB2 Hit", "Player hit"],
    ["SMW Exit", "Goal tape exit"],
    ["SMW Checkpoint", "Checkpoint tape"],
    ["Blaarg", "Sumbarine"],
    ["Racoon", "Transform"],
    ["Thwomp", "Crush"],
    ["SM ", ""]
];

function replaceByDictionary($file, $dict)
{
    $str = file_get_contents($file);
    for($i = 0; $i<count($dict); $i++)
    {
        //echo "Replace ".$dict[$i][0]." with ".$dict[$i][1]."\n";
        $str=str_replace($dict[$i][0], $dict[$i][1], $str);
    }
    file_put_contents($file, $str);
}

function replaceByWord($file, $word)
{
    $str = file_get_contents($file);
    //echo "Replace ".$word[0]." with ".$word[1]."\n";
    $str=str_replace($word[0], $word[1], $str);
    file_put_contents($file, $str);
}

function updateGroups($file)
{
    replaceByWord($file, ["\"Super Mario Bros.\"",      "\"A2XT Set #1\""]);
    replaceByWord($file, ["\"Super Mario Bros. 2\"",    "\"A2XT Set #2\""]);
    replaceByWord($file, ["\"Super Mario Bros. 3\"",    "\"A2XT Set #3\""]);
    replaceByWord($file, ["\"Super Mario World\"",      "\"A2XT Set #4\""]);
    replaceByWord($file, ["\"The Lakitus are throwing Lakitus!\"", "\"Misc. #2\""]);
}

function DoSmbx2Talkhaus($directory)
{
    global $characters, $furbas, $goopas, $radish, $bots, $drillrozes, $hands, $catnips, $sfxNames, $groups;
    replaceByDictionary("$directory/lvl_characters.ini", $characters);
    replaceByDictionary("$directory/sounds.ini", $sfxNames);

    replaceByWord("$directory/engine.ini", ["#008080", "#660f55"]);

    for($i=1; $i<=700; $i++)
    {
        switch($i)
        {
        case 622:case 626:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Mario", "Demo"]);break;
        case 623:case 627:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Luigi", "Iris"]);break;
        case 624:case 628:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Peach", "Kood"]);break;
        case 625:case 629:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Toad", "Raocow"]);break;
        case 631:case 632:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Link", "Steath"]);break;
        case 527:case 529:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Chozo", "Ampersand"]);break;
        case 505:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Bob-omb", "Canister"]);break;
        case 598:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Jelectro", "Spikeclop"]);break;
        case 615:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Reznor's platform", "Castle platform block"]);break;
        case 109:case 621:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Muncher", "Saw belt"]);break;
        case 111:case 112:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Bill Blaster", "Cannon"]);break;
        case 192:case 193:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["SMB ", ""]);break;
        case 386:case 387:case 390:case 393:case 394:case 395:case 396:case 401:case 402:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["SMB1 ", ""]);break;
        case 2:case 5:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["SMB3 ", ""]);break;
        case 88:case 89:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["SMW ", ""]);break;
        case 540:case 541:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Crateria", "Dark Castle"]);break;
        case 519:case 520:case 521:case 522:case 523:case 524:case 525:case 526:case 528:case 530:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Brinstar", "Dark Dungeon"]);break;
        case 534:case 535:case 536:case 537:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Metroid ", "Sewerage "]);break;
        case 544:case 545:case 546:case 547:
        case 548:case 549:case 550:case 551:case 552:case 553:case 554:case 555:case 556:case 557:
        case 558:case 559:case 560:case 561:case 562:case 563:case 564:case 565:case 566:case 567:
            replaceByWord("$directory/items/blocks/block-$i.ini", ["Tourian", "Techno"]);break;
        }
        updateGroups("$directory/items/blocks/block-$i.ini");
        replaceByWord("$directory/items/blocks/block-$i.ini", ["\"Metroid\"",      "\"Dark Castle\""]);
    }
    for($i=1; $i<=200; $i++)
    {
        switch($i)
        {
        case 150:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["Phanto", "Demo Face"]);break;
        case 88:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["Subcon Door", "Digital Door"]);break;
        case 13:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["SMB3 Goal", "Goal border"]);break;
        case 147:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["Bowser's ", ""]);break;
        case 100:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["SMB3 ", ""]);break;
        case 160:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["Star", "Leek"]);break;
        case 151:case 152:case 153:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["Crateria", "Dark"]);break;
        case 70:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["SMW ", ""]);break;
        case 154:case 155:case 156:case 157:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["Mother Brain's ", ""]);break;
        case 126:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["Sleeping Princess Zelda", "Queen Luna XIII"]);break;
        case 11:case 12:case 60:case 61:
            replaceByWord("$directory/items/bgo/background-$i.ini", ["SMB3 Goal", "Goal"]);break;
        }
        updateGroups("$directory/items/bgo/background-$i.ini");
    }

    for($i=1; $i<=292; $i++)
    {
        //replaceByWord("$directory/items/npc/npc-$i.ini", );
        //replaceByWord("$directory/items/npc/npc-$i.ini", );
        switch($i)
        {
        case 1: case 2: case 3: case 27: case 59: case 61: case 63:
        case 65: case 71: case 89: case 165: case 166: case 167:
        case 242: case 243: case 244:
            replaceByDictionary("$directory/items/npc/npc-$i.ini", $furbas);break;

        case 239:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Dynamite Plunger", "Switch can"]);break;

        case 154: case 155: case 156: case 157:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Mushroom Block", "Wooden brick"]);break;

        case 278: case 279:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Propeller Block", "Jet Pack"]);break;

        case 232: case 236:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Blurp", "Fishclop"]);
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Cheep Cheep (SMW)", "Fishclop"]);
            break;

        case 6:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Koopa Troopa (Red)", "Goopa (Violet)"]);break;
        case 4:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Koopa Troopa (Green)", "Goopa (Orange)"]);break;
        case 5:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Koopa Troopa Shell (Green)", "Goopa Shell (Orange)"]);break;
        case 7:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Koopa Troopa Shell (Red)", "Goopa Shell (Violet)"]);break;
        case 76:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Koopa Para-Troopa (Green)", "Para-Goopa (Orange)"]);break;
        case 161:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Koopa Para-Troopa (Red)", "Para-Goopa (Violet)"]);break;
        case 55: case 72: case 73: case 109:
        case 110: case 111: case 112: case 113: case 114: case 115: case 116: case 117: case 118:
        case 119: case 120: case 121: case 122: case 123: case 124: case 172: case 173:
        case 174: case 175: case 176: case 177:
            replaceByDictionary("$directory/items/npc/npc-$i.ini", $goopas);break;

        case 9: case 90: case 143: case 146: case 184: case 185: case 186: case 187:
            replaceByDictionary("$directory/items/npc/npc-$i.ini", $radish);break;

        case 249:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Mushroom", "Radish"]);break;

        case 96:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Yoshi Eggs", "Catnip Eggs"]);break;

        case 273:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["? Mushroom", "Tomato"]);break;

        case 275: case 276:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Volcano Lotus", "Love frog"]);break;
        
        case 47:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Lakitu (SMB3)", "Flying Bananasnake"]);break;

        case 284:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Lakitu (SMW)", "Butterfly"]);break;

        case 48:
        case 286:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Spiny Egg", "Caterpillar (curtailed)"]);break;
        
        case 36:
        case 285:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Spiny", "Caterpillar"]);break;

        case 284:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Lakitu (SMW)", "Butterfly"]);break;

        case 189:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Dry Bones", "Shaman"]);break;

        case 234:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Fish Bone", "Bonefish"]);break;

        case 29:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Hammer Bros.", "Shooting Bananasnake"]);break;

        case 125:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Tinsuit", "Ukrainian Dancer"]);break;

        case 271:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Swooper", "Goobat"]);break;

        case 77:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Ninji", "Fauriel"]);break;

        case 42:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Eerie", "Paralar"]);break;

        case 164:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Mega Mole", "Shopping Trolley"]);break;

        case 14:
        case 182:
        case 183:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Fire Flower", "Fire Cactus"]);break;

        case 179:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Grinder", "Fire Flower"]);break;

        case 207:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Spike Top", "Pickelhaube"]);break;

        case 28:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Cheep Cheep (Red)", "Catfish (Red)"]);break;

        case 229:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Cheep Cheep (Green)", "Catfish (Blue)"]);break;

        case 230:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Cheep Cheep (Red)", "Catfish (Yellow)"]);break;

        case 233:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Cheep Cheep (Green)", "Catfish (Green)"]);break;
        
        case 30:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Hammer", "Banana"]);break;

        case 231:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Blooper", "Squidclop"]);break;

        case 235:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Blooper", "Polyp"]);break;
        
        case 93:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Piranha Plant", "Venus Fly Head"]);break;

        case 270:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Jumping Piranha Plant", "Venus Fly Jump"]);break;

        case 97:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Power Star (Exit)", "Leek (Exit)"]);break;

        case 196:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Power Star (Collectable)", "Leek (Collectable)"]);break;

        case 196:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Power Star (Collectable)", "Leek (Collectable)"]);break;

        case 126:case 127:case 128:
            replaceByDictionary("$directory/items/npc/npc-$i.ini", $bots);break;

        case 19:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Shy Guy (Red)", "Alien (White)"]);break;

        case 20:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Shy Guy (Blue)", "Alien (Gray)"]);break;

        case 8:case 51:case 52:
            replaceByDictionary("$directory/items/npc/npc-$i.ini", $drillrozes);break;

        case 245: case 246:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Venus Fire Trap", "Snap Dragon"]);break;
        
        case 261:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Nipper Plant", "Teeth"]);break;

        case 162:case 163:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Rex", "Mr. Rabbit"]);break;

        case 211:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Rinka Block", "Donut Cannon"]);break;

        case 210:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Rinka", "Donut"]);break;

        case 204:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Rocket Ripper", "Genie"]);break;

        case 203:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Ripper", "Flying Fish"]);break;

        case 205:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Zoomer", "Octobro"]);break;

        case 168:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bully", "Sandcastle"]);break;

        case 74:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Piranhacus Giganticus", "Caterpillar Guy"]);break;

        case 256:case 257:
            replaceByDictionary("$directory/items/npc/npc-$i.ini", $hands);break;
        
        case 200:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bowser (SMB1)", "Bananasnake King"]);break;

        case 201:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Wart", "Duck"]);break;

        case 202:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Wart's Bubble", "Ducking"]);break;

        case 134:case 262:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Mouser", "Kitty"]);break;

        case 39:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Birdo", "Toaster Cannon"]);break;
        
        case 40:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Birdo's Egg", "Toast"]);break;

        case 15:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Boom Boom", "Some Nondescript Dude Thing"]);break;

        case 268:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Larry Koopa's Shell", "Penguin head"]);break;

        case 268:case 269:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Larry Koopa", "Penguin"]);break;

        case 86:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bowser (SMB3)", "Mr. Vipper"]);break;

        case 87:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bowser's Fireball", "Mr. Vipper's Fireball"]);break;

        case 280:case 281:case 282:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Ludwig von Koopa", "King Charles IV"]);break;

        case 209:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Mother Brain", "King Husc XII"]);break;

        case 102:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Link", "Feed"]);break;
        
        case 206:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Spark", "Goobal"]);break;

        case 272:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Hoopster", "Vinesaw"]);break;
        
        case 129:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Tweeter", "Twitter"]);break;

        case 135:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bob-Omb (SMB2)", "Canister"]);break;

        case 247:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Pokey", "Totem"]);break;

        case 37:case 180:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Thwomp", "Crusher"]);break;

        case 12:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Podoboo", "Podocat"]);break;

        case 17:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bullet Bill", "Bill"]);break;

        case 18:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Banzai Bill", "Big Bill"]);break;

        case 21:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bill Blaster", "Cannon"]);break;

        case 22:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Billy Gun", "Machine gun"]);break;

        case 136:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bob-Omb (SMB3)", "Bombu"]);break;

        case 137:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bob-Omb Ignited (SMB3)", "Bombu Ignited"]);break;

        case 53:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Sidestepper", "Crab"]);break;

        case 54:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Fighter Fly", "Ice Hopper"]);break;

        case 259:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Roto-Disc", "Disco Dan"]);break;

        case 152:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Gold Ring", "Gold Tooth"]);break;

        case 251:case 252:case 253:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Rupee", "Candy"]);break;

        case 274:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Dragon Coin", "Rao Coin"]);break;
        
        case 171:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Hammer (Player)", "Rotten fish"]);break;

        case 75:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Toad (Jumping)", "Poor Citizen"]);break;

        case 291:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Princess Peach's Bomb", "Kood's Bomb"]);break;

        case 292:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Toad's Boomerang", "Raocow's Gamepad"]);break;

        case 94:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Toad", "Busynessman"]);break;

        case 198:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Princess Peach", "Calleoca"]);break;

        case 158:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Mr. Saturn", "Demo's Pet Dog"]);break;

        case 16:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Magic Ball", "Money bag"]);break;

        case 41:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Crystal Ball", "Mason jar"]);break;

        case 11:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["SMB3 Card Roulette", "Card Roulette"]);break;
        
        case 95:case 96:case 98:case 99:case 100:case 148:case 149:case 150:case 228:
            replaceByDictionary("$directory/items/npc/npc-$i.ini", $catnips);break;

        case 108:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Yoshi", "Catnip"]);break;

        case 237:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Ice Cube (Cyan Yoshi)", "Frozen face"]);break;

        case 153:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Poison Mushroom", "Old cheese"]);break;

        case 23:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Buzzy Beetle", "Skull slime"]);break;
        
        case 24:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Buzzy Beetle Shell", "Skull"]);break;

        case 199:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Blargg", "Submarine"]);break;

        case 35:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Kuribo's Shoe", "Green sack"]);break;

        case 191:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Podoboo's Shoe", "Red sack"]);break;

        case 193:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Lakitu's Shoe", "Blue sack"]);break;

        case 84:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bowser Statue", "Frog Statue"]);break;

        case 85:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bowser Statue's Fireball", "Statue fireball"]);break;

        case 181:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Bowser Statue", "Catnip Statue"]);break;

        case 46:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Donut Block (Red)", "Unstable brick (Red)"]);break;

        case 212:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Donut Block (Brown)", "Unstable brick (Brown)"]);break;

        case 130:case 131:case 132:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Snifit", "Spitter"]);break;

        case 133:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Snifit Bullet", "Spittle"]);break;

        case 250:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Heart", "Carrot"]);break;

        case 194:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Kamikaze Koopa", "Crazy Goopa Shell"]);break;

        case 56:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Koopa Clown Car", "Clown Helicopter"]);break;

        case 34:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Super Leaf", "Salat Leaf"]);break;

        case 169:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Tanooki Suit", "Cabbage of the Witch"]);break;

        case 170:
            replaceByWord("$directory/items/npc/npc-$i.ini", ["Hammer Suit", "Super Pumpkin"]);break;
        }

        replaceByDictionary("$directory/items/npc/npc-$i.ini", $groups);

        updateGroups("$directory/items/npc/npc-$i.ini");
    }

    if(file_exists("$directory/script/npcs/para_goomba.lua"))
    {
        if(file_exists("$directory/script/npcs/para_furba.lua"))
            unlink("$directory/script/npcs/para_furba.lua");
        rename("$directory/script/npcs/para_goomba.lua", "$directory/script/npcs/para_furba.lua");
    }
}

DoSmbx2Talkhaus("./Raocow (A2MBXT)");


