/*****************************************************************************
 *  $Id: str.h,v 1.2 2003/04/18 23:14:39 dun Exp $
 *****************************************************************************
 *  This file is part of the Munge Uid 'N' Gid Emporium (MUNGE).
 *  For details, see <http://www.llnl.gov/linux/munge/>.
 *  UCRL-CODE-2003-???.
 *
 *  Copyright (C) 2001-2003 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Chris Dunlap <cdunlap@llnl.gov>.
 *
 *  This is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License;
 *  if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 *  Suite 330, Boston, MA  02111-1307  USA.
 *****************************************************************************/


#ifndef STR_H
#define STR_H


#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>


char * strdupf (const char *fmt, ...);
/*
 *  Duplicates the string specified by the format-string [fmt].
 *  Returns the new string, or NULL if out of memory.
 *  The caller is responsible for freeing this new string.
 */

size_t strcatf (char *dst, size_t size, const char *fmt, ...);
/*
 *  Concatenates the string specified by the format-string [fmt] to
 *    the NUL-terminated string [dst] within a buffer of size [size].
 *    Note that [size] is the full size of [dst], not the space remaining.
 *  Returns the new length of the NUL-terminated string [dst],
 *    or -1 if truncation occurred.  The string in [dst] is
 *    guaranteed to be NUL-terminated.
 */

void strdump (const char *prefix, void *x, int n);
/*
 *  Debugging aid for dumping the byte array [x] of length [n] to stdout,
 *    prefixing the output with the [prefix] string.
 */


#endif /* !STR_H */