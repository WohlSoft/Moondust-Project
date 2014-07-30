#ifndef EDITOR_VERSION_H
#define EDITOR_VERSION_H

//Version of whole project
#define _VERSION "0.1.2.3-dev"
#define _RELEASE " Alpha" //Developing state (for release this field must be empty)

//Version of this program
#define _FILE_VERSION "0.1.2.3-dev"
#define _FILE_RELEASE " Alpha"

#define _VF1 0
#define _VF2 1
#define _VF3 2
#define _VF4 3

#define _VP1 0
#define _VP2 1
#define _VP3 2
#define _VP4 3

#define _COPYRIGHT "2014 by Wohlstand"

#define _COMPANY "Wohlhabend Network System"
#define _FILE_DESC "Platformer Game Engine - Editor"
#define _PRODUCT_NAME "Platformer Game Engine"

#define _INTERNAL_NAME "pge_editor"

#ifdef _WIN32
	#define _ORIGINAL_NAME "pge_editor.exe" // for Windows platforms
#else
	#define _ORIGINAL_NAME "pge_editor" // for any other platforms
#endif

//Uncomment this for enable detal logging
//#define DEBUG


#ifdef _WIN64
	#define OPERATION_SYSTEM "Windows"
#elif  _WIN32
	#define OPERATION_SYSTEM "Windows x64"
#elif defined __APPLE__
	#define OPERATION_SYSTEM "MacOSX"
#elif defined __FreeBSD__
	#define OPERATION_SYSTEM "FreeBSD"
#elif defined __gnu_linux__
	#define OPERATION_SYSTEM "GNU/Linux"
#else
	#define OPERATION_SYSTEM "Other"
#endif


#endif
