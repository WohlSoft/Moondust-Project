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
 * $Id: resample.h,v 1.9 2004/01/23 09:41:32 rob Exp $
 */

# ifndef RESAMPLE_H
# define RESAMPLE_H

# include <mad.h>

struct resample_state {
  mad_fixed_t ratio;

  mad_fixed_t step;
  mad_fixed_t last;
};

int mad_resample_init(struct resample_state *, unsigned int, unsigned int);

# define resample_finish(state)  /* nothing */

unsigned int mad_resample_block(struct resample_state *, unsigned int,
			    mad_fixed_t const *, mad_fixed_t *);

# endif
