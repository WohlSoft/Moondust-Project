/****************************************************
*Part of SPC2IT, read readme.md for more information*
****************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

#include "sound.h"
#include "it.h"
#include "emu.h"
#include "sneese_spc.h"

_Static_assert(sizeof(u8) == 1, "Warning: wrong size u8");
_Static_assert(sizeof(u16) == 2, "Warning: wrong size u16");
_Static_assert(sizeof(u32) == 4, "Warning: wrong size u32");
_Static_assert(sizeof(u64) == 8, "Warning: wrong size u64");
_Static_assert(sizeof(s8) == 1, "Warning: wrong size s8");
_Static_assert(sizeof(s16) == 2, "Warning: wrong size s16");
_Static_assert(sizeof(s32) == 4, "Warning: wrong size s32");
_Static_assert(sizeof(s64) == 8, "Warning: wrong size s64");
_Static_assert(sizeof(ITFileHeader) == 192, "Warning: wrong size ITFileHeader");
_Static_assert(sizeof(ITFileSample) == 80, "Warning: wrong size ITFileSample");
_Static_assert(sizeof(ITFilePattern) == 8, "Warning: wrong size ITFilePattern");
_Static_assert(sizeof(SPCFile) == 65920, "Warning: wrong size SPCFile");

int spc2it_load(const char* src, int ITrows, int limit, int verbose)
{
    if (ITStart(ITrows))
    {
        // printf("Error: failed to initialize pattern buffers\n");
        return(1);
    }
    if (SPCInit(src)) // Reset SPC and load state
    {
        // printf("Error: failed to initialize emulation\n");
        return(1);
    }
    if (SNDInit())
    {
        // printf("Error: failed to initialize sound\n");
        return(1);
    }

    if ((!limit) && (SPCtime))
        limit = SPCtime;
    else if (!limit)
        limit = 60;

    if (verbose)
    {
        printf("Time (seconds):      %i\n", limit);

        printf("IT Parameters:\n");
        printf("    Rows/pattern:    %d\n", ITrows);

        printf("ID info:\n");
        printf("        Song:  %s\n", SPCInfo.SongTitle);
        printf("        Game:  %s\n", SPCInfo.GameTitle);
        printf("      Dumper:  %s\n", SPCInfo.DumperName);
        printf("    Comments:  %s\n", SPCInfo.Comment);
        printf("  Created on:  %s\n", SPCInfo.Date);

        printf("\n");

        fflush(stdout);
    }

    SNDNoteOn(SPC_DSP[0x4c]);

    int success = 1;
    int seconds = SNDratecnt = 0;
    while (true)
    {
        if (ITMix() || ITUpdate())
            return 1;

        SNDratecnt += 1;

        int ret = SPC_START(2048000 / (SPCUpdateRate * 2)); // emulate the SPC700

        if (ret)
            return 1;

        if (SNDratecnt >= SPCUpdateRate)
        {
            SNDratecnt -= SPCUpdateRate;
            seconds++; // count number of seconds

            if (verbose)
            {
                printf("Progress: %f%%\r", (((f64)seconds / limit) * 100));
                fflush(stdout);
            }

            if (seconds == limit)
                break;
        }
    }

    return 0;
}

int spc2it_save(const char* dest, int verbose)
{
    if (verbose)
        printf("\n\nSaving file...\n");

    if (ITWrite(dest))
    {
        if (verbose)
            printf("Error: failed to write %s.\n", dest);

        return 1;
    }

    if (verbose)
        printf("Wrote to %s successfully.\n", dest);

    return 0;
}

void spc2it_cleanup()
{
    ITCleanup();
    Reset_SPC();
}

int spc2it_convert(const char* src, const char* dest)
{
    int failure = spc2it_load(src, 200, 0, 0) || spc2it_save(dest, 0);

    spc2it_cleanup();

    return failure;
}
