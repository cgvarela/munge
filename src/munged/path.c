/*****************************************************************************
 *  $Id$
 *****************************************************************************
 *  Copyright (C) 2002-2006 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory.
 *  Written by Chris Dunlap <cdunlap@llnl.gov>.
 *  UCRL-CODE-155910.
 *
 *  This file is part of the MUNGE Uid 'N' Gid Emporium (MUNGE).
 *  For details, see <http://home.gna.org/munge/>.
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.
 *****************************************************************************
 *  Refer to "path.h" for documentation on public functions.
 *****************************************************************************/


#if HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "path.h"
#include "strlcpy.h"

#ifndef PATH_MAX
#  define PATH_MAX 4096
#endif /* !PATH_MAX */


/*****************************************************************************
 *  Internal Prototypes
 *****************************************************************************/

static int _path_set_err (int rc, char *buf, size_t buflen,
    const char *format, ...);


/*****************************************************************************
 *  External Functions
 *****************************************************************************/

int
path_canonicalize (const char *src, char *dst, int dstlen)
{
    char buf [PATH_MAX];
    int  n = 0;

    if (!src || !*src) {
        errno = EINVAL;
        return (-1);
    }
    if (!realpath (src, buf)) {
        return (-1);
    }
    if (buf[0] != '/') {
        errno = EINVAL;
        return (-1);
    }
    if ((dst != NULL) && (dstlen > 0)) {
        n = strlcpy (dst, buf, dstlen);
    }
    return (n);
}


int
path_dirname (const char *src, char *dst, size_t dstlen)
{
    char *p = NULL;
    enum { start, last_slash, last_word, prev_slash } state = start;

    if ((dst == NULL) || (dstlen <= 1)) {
        errno = EINVAL;
        return (-1);
    }
    if (src != NULL) {
        if (strlcpy (dst, src, dstlen) >= dstlen) {
            errno = ENAMETOOLONG;
            return (-1);
        }
        for (p = dst + strlen (dst) - 1; p >= dst; p--) {
            if (state == start) {
                state = (*p == '/') ? last_slash : last_word;
            }
            else if (state == last_slash) {
                if (*p != '/') state = last_word;
            }
            else if (state == last_word) {
                if (*p == '/') state = prev_slash;
            }
            else if (state == prev_slash) {
                if (*p != '/') break;
            }
            *p = '\0';
        }
    }
    if (p < dst) {
        dst[0] = (state == prev_slash || state == last_slash) ? '/' : '.';
        dst[1] = '\0';
    }
    return (0);
}


int
path_is_accessible (const char *path, char *errbuf, size_t errbuflen)
{
    int          n;
    char         buf [PATH_MAX];
    struct stat  st;
    char        *p;

    n = path_canonicalize (path, buf, sizeof (buf));
    if (n < 0) {
        return (_path_set_err (-1, errbuf, errbuflen,
            "cannot canonicalize \"%s\": %s", path, strerror (errno)));
    }
    if (n >= sizeof (buf)) {
        errno = ENAMETOOLONG;
        return (_path_set_err (-1, errbuf, errbuflen,
            "cannot canonicalize \"%s\": exceeded max path length", path));
    }
    if (lstat (buf, &st) < 0) {
        return (_path_set_err (-1, errbuf, errbuflen,
            "cannot check \"%s\": %s", buf, strerror (errno)));
    }
    if (!S_ISDIR (st.st_mode)) {
        if ((p = strrchr (buf, '/'))) {
            *p = '\0';
        }
    }
    while (buf[0] != '\0') {
        if (lstat (buf, &st) < 0) {
            return (_path_set_err (-1, errbuf, errbuflen,
                "cannot check \"%s\": %s", buf, strerror (errno)));
        }
        if (!S_ISDIR (st.st_mode)) {
            errno = EINVAL;
            return (_path_set_err (-1, errbuf, errbuflen,
                "cannot check \"%s\": unexpected file type", buf));
        }
        if ((st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
                != (S_IXUSR | S_IXGRP | S_IXOTH)) {
            return (_path_set_err (0, errbuf, errbuflen,
                "execute permissions for all required on \"%s\"", buf));
        }
        if (!(p = strrchr (buf, '/'))) {
            errno = EINVAL;
            return (_path_set_err (-1, errbuf, errbuflen,
                "cannot check \"%s\": internal error", buf));
        }
        if ((p == buf) && (buf[1] != '\0')) {
            p++;
        }
        *p = '\0';
    }
    return (1);
}


int
path_is_secure (const char *path, char *errbuf, size_t errbuflen)
{
    int          n;
    char         buf [PATH_MAX];
    struct stat  st;
    char        *p;
    uid_t        euid;
    gid_t        egid;

    n = path_canonicalize (path, buf, sizeof (buf));
    if (n < 0) {
        return (_path_set_err (-1, errbuf, errbuflen,
            "cannot canonicalize \"%s\": %s", path, strerror (errno)));
    }
    if (n >= sizeof (buf)) {
        errno = ENAMETOOLONG;
        return (_path_set_err (-1, errbuf, errbuflen,
            "cannot canonicalize \"%s\": exceeded max path length", path));
    }
    if (lstat (buf, &st) < 0) {
        return (_path_set_err (-1, errbuf, errbuflen,
            "cannot check \"%s\": %s", buf, strerror (errno)));
    }
    if (!S_ISDIR (st.st_mode)) {
        if ((p = strrchr (buf, '/'))) {
            *p = '\0';
        }
    }
    euid = geteuid ();
    egid = getegid ();

    while (buf[0] != '\0') {
        if (lstat (buf, &st) < 0) {
            return (_path_set_err (-1, errbuf, errbuflen,
                "cannot check \"%s\": %s", buf, strerror (errno)));
        }
        if (!S_ISDIR (st.st_mode)) {
            errno = EINVAL;
            return (_path_set_err (-1, errbuf, errbuflen,
                "cannot check \"%s\": unexpected file type", buf));
        }
        if ((st.st_uid != 0) && (st.st_uid != euid)) {
            return (_path_set_err (0, errbuf, errbuflen,
                "invalid ownership of \"%s\"", buf));
        }
        if ((st.st_mode & S_IWGRP) && !(st.st_mode & S_ISVTX)) {
            return (_path_set_err (0, errbuf, errbuflen,
                "group-writable permissions set on \"%s\"", buf));
        }
        if ((st.st_mode & S_IWOTH) && !(st.st_mode & S_ISVTX)) {
            return (_path_set_err (0, errbuf, errbuflen,
                "world-writable permissions set on \"%s\"", buf));
        }
        if (!(p = strrchr (buf, '/'))) {
            errno = EINVAL;
            return (_path_set_err (-1, errbuf, errbuflen,
                "cannot check \"%s\": internal error", buf));
        }
        if ((p == buf) && (buf[1] != '\0')) {
            p++;
        }
        *p = '\0';
    }
    return (1);
}


/*****************************************************************************
 *  Internal Functions
 *****************************************************************************/

static int
_path_set_err (int rc, char *buf, size_t buflen, const char *format, ...)
{
/*  Sets an error condition to be returned to the caller.
 *  If [buf] is non-NULL, the [format] string will be expanded and written
 *    to the buffer [buf] of length [buflen].
 *  Returns [rc].
 */
    va_list vargs;

    if ((buf != NULL) && (buflen > 0)) {
        va_start (vargs, format);
        (void) vsnprintf (buf, buflen, format, vargs);
        buf [buflen - 1] = '\0';
        va_end (vargs);
    }
    return (rc);
}