/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Foundation/Foundation.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_filesystem.h>

#include "app_path_macosx.h"

char * getAppSupportDir(void)
{ @autoreleasepool
{
    char *retval = NULL;

    NSArray *array = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);

    if ([array count] > 0)
    {  /* we only want the first item in the list. */
        NSString *str = [array objectAtIndex:0];
        const char *base = [str fileSystemRepresentation];
        if (base) {
            const size_t len = SDL_strlen(base) + 4;
            retval = (char *)SDL_malloc(len);
            if (retval == NULL) {
                SDL_OutOfMemory();
            } else {
                SDL_snprintf(retval, len, "%s", base);
            }
        }
    }

    return retval;
}}

char * getScreenCaptureDir(void)
{ @autoreleasepool
{
    char *retval = NULL;

    // Get current screencapture location
    NSUserDefaults *appUserDefaults = [[NSUserDefaults alloc] init];
        [appUserDefaults addSuiteNamed:@"com.apple.screencapture"];
    NSDictionary *prefsDict = [appUserDefaults dictionaryRepresentation];

    NSString *str = [prefsDict valueForKey:@"location"];
    const char *base = [str fileSystemRepresentation];
    if (base) {
        const size_t len = SDL_strlen(base) + 4;
        retval = (char *)SDL_malloc(len);
        if (retval == NULL) {
            SDL_OutOfMemory();
        } else {
            SDL_snprintf(retval, len, "%s", base);
        }
    }

    return retval;
}}
