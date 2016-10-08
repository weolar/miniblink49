/***
*sys/locking.h - flags for locking() function
*
*       Copyright (c) 1985-2001, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the flags for the locking() function.
*       [System V]
*
*       [Public]
*
****/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_LOCKING
#define _INC_LOCKING

#if !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#define _LK_UNLCK       0       /* unlock the file region */
#define _LK_LOCK        1       /* lock the file region */
#define _LK_NBLCK       2       /* non-blocking lock */
#define _LK_RLCK        3       /* lock for writing */
#define _LK_NBRLCK      4       /* non-blocking lock for writing */

#if !__STDC__
/* Non-ANSI names for compatibility */
#define LK_UNLCK        _LK_UNLCK
#define LK_LOCK         _LK_LOCK
#define LK_NBLCK        _LK_NBLCK
#define LK_RLCK         _LK_RLCK
#define LK_NBRLCK       _LK_NBRLCK
#endif

#endif  /* _INC_LOCKING */

