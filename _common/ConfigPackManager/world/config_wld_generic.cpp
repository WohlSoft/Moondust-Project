/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "config_wld_generic.h"

#include <IniProcessor/ini_processing.h>
#include <Utils/maths.h>
#include "../../number_limiter.h"

#include <assert.h>

WldGenericSetup::WldGenericSetup() : ConfigBaseSetup() {}

bool WldGenericSetup::parse(IniProcessing *setup,
                            PGEString imgPath,
                            uint32_t defaultGrid,
                            const WldGenericSetup *merge_with,
                            PGEString *error)
{
#define pMerge(param, def) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(def))
#define pMergeMe(param) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(param))
#define pAlias(paramName, destValue) setup->read(paramName, destValue, destValue)

    /*************Buffers*********************/
    uint32_t    w = 0, h = 0;
    /*************Buffers*********************/

    if(!parseBase(setup, imgPath, defaultGrid, w, h, merge_with, error))
        return false;

    setup->read("frames",       frames,     pMerge(frames, 1u));//Real
    pAlias("framecount",        frames);//Alias
    pAlias("frame-count",       frames);//Alias
#ifdef PGE_EDITOR // alternative animation for Editor
    pAlias("editor-frames",   frames);
#endif
    NumberLimiter::apply(frames, uint32_t(1u));
    animated = (frames > 1u);
    setup->read("frame-delay", framespeed, pMerge(framespeed, 125));//Real
    pAlias("frame-speed", framespeed);//Alias
    if(setup->hasKey("framespeed"))
    {
        pAlias("framespeed",  framespeed);//Alias
        framespeed = (framespeed * 1000u) / 65u;//Convert 1/65'th into milliseconds
    }
    NumberLimiter::apply(framespeed, uint32_t(1u));

    frame_sequence.clear();
    setup->read("frame-sequence", frame_sequence, pMergeMe(frame_sequence));
#ifdef PGE_EDITOR // alternative animation for Editor
    pAlias("editor-frame-sequence",   frame_sequence);
#endif

    frame_h = animated ? Maths::uRound(double(h) / double(frames)) : h;
    NumberLimiter::apply(frame_h, uint32_t(0u));

    setup->read("display-frame",    display_frame,      pMerge(display_frame, 0));
    setup->read("map3d-vertical",   map3d_vertical,     pMerge(map3d_vertical, false));
    setup->read("map3d-stackables", map3d_stackables,   pMerge(map3d_stackables, map3d_stackables));

    //Rows and cols for table-like element sets
    setup->read("row", row, pMerge(row, 0));
    setup->read("col", col, pMerge(col, 0));

#undef pMerge
#undef pMergeMe
#undef pAlias
    return true;
}
