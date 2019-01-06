/*
 * iSL (Subsystem for Linux) for iOS & Android
 *
 * Copyright (C) 2018 Björn Rennfanz (bjoern@fam-rennfanz.de)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WIN32UNISTD_H
#define WIN32UNISTD_H

#if defined(_MSC_VER)
#   include <io.h>
#else
#   include <unistd.h>
#endif

#include <sys/types.h>
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#undef GNU_OUR_PAGESIZE
#if defined (HAVE_SYSCONF) && defined (HAVE_UNISTD_H)
# include <unistd.h>
# ifdef _SC_PAGESIZE
#  define GNU_OUR_PAGESIZE sysconf(_SC_PAGESIZE)
# endif
#endif

#ifndef GNU_OUR_PAGESIZE
# ifdef	PAGESIZE
#  define	GNU_OUR_PAGESIZE PAGESIZE
# else	/* no PAGESIZE */
#  ifdef	EXEC_PAGESIZE
#   define	GNU_OUR_PAGESIZE EXEC_PAGESIZE
#  else	/* no EXEC_PAGESIZE */
#   ifdef	NBPG
#    define	GNU_OUR_PAGESIZE (NBPG * CLSIZE)
#    ifndef	CLSIZE
#     define	CLSIZE 1
#    endif	/* CLSIZE */
#   else	/* no NBPG */
#    ifdef	NBPC
#     define	GNU_OUR_PAGESIZE NBPC
#    else	/* no NBPC */
#     define	GNU_OUR_PAGESIZE 4096	/* Just punt and use reasonable value */
#    endif /* NBPC */
#   endif /* NBPG */
#  endif /* EXEC_PAGESIZE */
# endif /* PAGESIZE */
#endif /* GNU_OUR_PAGESIZE */

static inline int getpagesize()
{
    return (GNU_OUR_PAGESIZE);
}

#endif // WIN32UNISTD_H
