/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#include "lib.h"

#ifdef _WIN32
#undef realpath
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif


int main(int argc, char **argv)
{
	int seconds, limit, ITrows;
	char fn[PATH_MAX];
	fn[0] = 0;
	ITrows = 200; // Default 200 IT rows/pattern
	limit = 0;    // Will be set later

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
			switch (argv[i][1])
			{
			case 'r':
				i++;
				ITrows = atoi(argv[i]);
				break;
			case 't':
				i++;
				limit = atoi(argv[i]);
				break;
			default:
				printf("Warning: unrecognized option '-%c'\n", argv[i][1]);
			}
		else
			realpath(argv[i], fn);
	}
	if (fn[0] == 0)
	{
		printf(" SPC2IT - converts SPC700 sound files to the Impulse Tracker format\n\n");
		printf(" Usage:  spc2it [options] <filename>\n");
		printf(" Where <filename> is any .spc or .sp# file\n\n");
		printf(" Options: ");
		printf("-t x        Specify a time limit in seconds        [60 default]\n");
		printf("          -d xxxxxxxx Voices to disable (1-8)                [none default]\n");
		printf("          -r xxx      Specify IT rows per pattern            [200 default]\n");
		return(0);
	}
	printf("\n");
	printf("Filepath:            %s\n", fn);

	int load_ret = spc2it_load(fn, ITrows, limit, 1);

	int i;
	for (i = 0; i < PATH_MAX; i++)
		if (fn[i] == 0)
			break;

	for (; i > 0; i--)
		if (fn[i] == '.')
		{
			strcpy(&fn[i + 1], "it");
			break;
		}

	if (!load_ret)
		spc2it_save(fn, 1);

	spc2it_cleanup();
}
