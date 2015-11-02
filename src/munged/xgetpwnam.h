/*****************************************************************************
 *  Written by Chris Dunlap <cdunlap@llnl.gov>.
 *  Copyright (C) 2007-2015 Lawrence Livermore National Security, LLC.
 *  Copyright (C) 2002-2007 The Regents of the University of California.
 *  UCRL-CODE-155910.
 *
 *  This file is part of the MUNGE Uid 'N' Gid Emporium (MUNGE).
 *  For details, see <https://dun.github.io/munge/>.
 *
 *  MUNGE is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.  Additionally for the MUNGE library (libmunge), you
 *  can redistribute it and/or modify it under the terms of the GNU Lesser
 *  General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *
 *  MUNGE is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  and GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  and GNU Lesser General Public License along with MUNGE.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *****************************************************************************/


#ifndef XGETPWNAM_H
#define XGETPWNAM_H

#include <pwd.h>
#include <stddef.h>


/*****************************************************************************
 *  Data Types
 *****************************************************************************/

typedef struct xpwbuf_t * xpwbuf_p;


/*****************************************************************************
 *  Functions
 *****************************************************************************/

xpwbuf_p xgetpwnam_buf_create (size_t len);

void xgetpwnam_buf_destroy (xpwbuf_p pwbufp);

size_t xgetpwnam_buf_get_len (xpwbuf_p pwbufp);

int xgetpwnam (const char *user, struct passwd *pwp, xpwbuf_p pwbufp);


#endif /* !XGETPWNAM_H */