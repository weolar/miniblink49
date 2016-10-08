/***
* wcstr.h - declarations for wide character string manipulation functions
*
*       Copyright (c) 1985-1992, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the function declarations for the string
*       manipulation functions.
*       [OBSOLETE][UNICODE/ISO]
*
*	WARNING: OBSOLETE FILE: use wchar.h/stdlib.h instead!!!
*	This file provided only for short-term compatibility.  It will
*	disappear in the near future after a brief transitional period.
*
****/

#ifndef _INC_WCSTR

#include <wchar.h>
#define wcswcs wcsstr

#define _wcscmpi _wcsicmp
#if !__STDC__
#define wcscmpi	_wcsicmp
#endif

/* from stdlib.h */
int    __cdecl wctomb(char *, wchar_t);
size_t __cdecl wcstombs(char*, const wchar_t *, size_t);

#define _INC_WCSTR
#endif  /* _INC_WCSTR */

