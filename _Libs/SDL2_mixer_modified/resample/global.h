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
 * $Id: global.h,v 1.13 2004/01/23 09:41:31 rob Exp $
 */

# ifndef GLOBAL_H
# define GLOBAL_H

/* conditional debugging */

# if defined(DEBUG) && defined(NDEBUG)
#  error "cannot define both DEBUG and NDEBUG"
# endif

# if defined(DEBUG)
#  include <stdio.h>
# endif

/* conditional features */

# if defined(HAVE_MMAP) && !defined(USE_MMAP)
#  undef HAVE_MMAP
# endif

# if defined(HAVE_TERMIOS_H) || defined(_WIN32)
#  define USE_TTY
# endif

# if !defined(HAVE_UNISTD_H)
signed long read(int, void *, unsigned long count);
signed long lseek(int, signed long, int);
int open(char const *, int, ...);
int close(int);

#  if 0
#   define isatty(x)	((x) == STDIN_FILENO)
#  endif

#  define S_ISREG(x)	(((x) & S_IFREG) == S_IFREG)

#  define STDIN_FILENO	0
#  define STDOUT_FILENO	1

#  define SEEK_CUR	1
# endif

# if !defined(HAVE_ASSERT_H)
#  if defined(NDEBUG)
#   define assert(x)	/* nothing */
#  else
#   define assert(x)	do { if (!(x)) abort(); } while (0)
#  endif
# endif

/* conditional replacement function prototypes */

# if !defined(HAVE_STRCASECMP)
int strcasecmp(char const *, char const *);
# endif

# if !defined(HAVE_STRNCASECMP)
int strncasecmp(char const *, char const *, unsigned long);
# endif

/* natural language support */

# define  _(text)	gettext(text)
# define N_(text)	gettext_noop(text)

# endif
