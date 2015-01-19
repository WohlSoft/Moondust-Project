/*
 * madplay - MPEG audio decoder and player
 * Copyright (C) 2000-2004 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: resample.c,v 1.11 2004/01/23 09:41:32 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <string.h>
# include <mad.h>

# include "mad_resample.h"
#include "audio.h"

/*
 * NAME:	resample_init()
 * DESCRIPTION:	initialize resampling state
 */
int mad_resample_init(struct resample_state *state,
		  unsigned int oldrate, unsigned int newrate)
{
  mad_fixed_t ratio;

  if (newrate == 0)
    return -1;

  ratio = mad_f_div(oldrate, newrate);
  if ((ratio <= 0) || (ratio > MAX_RESAMPLEFACTOR * MAD_F_ONE))
    return -1;

  state->ratio = ratio;

  state->step = 0;
  state->last = 0;

  return 0;
}

/*
 * NAME:	resample_block()
 * DESCRIPTION:	algorithmically change the sampling rate of a PCM sample block
 */
unsigned int
mad_resample_block(struct resample_state *state,
                            unsigned int nsamples, mad_fixed_t const *old,
                mad_fixed_t *newdata)
{
  mad_fixed_t const *end, *begin;

  /*
   * This resampling algorithm is based on a linear interpolation, which is
   * not at all the best sounding but is relatively fast and efficient.
   *
   * A better algorithm would be one that implements a bandlimited
   * interpolation.
   */

  if (state->ratio == MAD_F_ONE) {
    memcpy(newdata, old, nsamples * sizeof(mad_fixed_t));
    return nsamples;
  }

  end   = old + nsamples;
  begin = newdata;

  if (state->step < 0)
  {
    state->step = mad_f_fracpart(-state->step);

    while (state->step < MAD_F_ONE) {
      *newdata++ = state->step ?
	state->last + mad_f_mul(*old - state->last, state->step) : state->last;

      state->step += state->ratio;
      if (((state->step + 0x00000080L) & 0x0fffff00L) == 0)
	state->step = (state->step + 0x00000080L) & ~0x0fffffffL;
    }

    state->step -= MAD_F_ONE;
  }

  while (end - old > 1 + mad_f_intpart(state->step))
  {
    old        += mad_f_intpart(state->step);
    state->step = mad_f_fracpart(state->step);

    *newdata++ = state->step ?
      *old + mad_f_mul(old[1] - old[0], state->step) : *old;

    state->step += state->ratio;
    if (((state->step + 0x00000080L) & 0x0fffff00L) == 0)
      state->step = (state->step + 0x00000080L) & ~0x0fffffffL;
  }

  if (end - old == 1 + mad_f_intpart(state->step)) {
    state->last = end[-1];
    state->step = -state->step;
  }
  else
    state->step -= mad_f_fromint(end - old);

  return newdata - begin;
}
