          PGE File Library v 0.3.1.7
==================================================
This library is a part of PGE Project.
==================================================
            Supported file formats:
==================================================
*.lvl   SMBX 1...64 Level File         -  Read/Write
*.wld   SMBX 1...64 World File         -  Read/Write
*.sav   SMBX 1...64 Game save File     -  Read only
*.dat   SMBX 1...64 Game config File   -  Read/Write
*.txt   SMBX64 NPC Custom config       -  Read/Write
*.lvlx  PGE-X Level File               -  Read/Write
*.wldx  PGE-X World File               -  Read/Write
*.savx  PGE-X Game save File           -  Read/Write
*.meta  PGE-X non-SMBX64 Meta File     -  Read/Write

==================================================
Use library with this header:

#include "file_formats.h"

==================================================

Library parses and generates RAW text string into internal.
You must read entire file data into std::string before parse,
or use openLevelFile() or openWorldFile() functions to read file
by the file path.

When you want to save file, you must open file stream like std::fout
and send generated data into it, or save it into std::string variable.

==================================================
SMBX files notes:
==================================================
1) You should save a text file with CRLF, or file will be not readable by SMBX Engine.
You can write a file like binary, but when you catching '\n', write a CRLF bytes yourself!

2) When you saving a level file (World file is not requires that), you should prepare data structure before saving it:

//Call of this function will update star counters and will set BGO's order priorities values
FileFormats::smbx64LevelPrepare(YourLevelData);
FileFormats::smbx64LevelSortBlocks(YourLevelData);  //Order blocks
FileFormats::smbx64LevelSortBGOs(YourLevelData);    //Order BGO's


