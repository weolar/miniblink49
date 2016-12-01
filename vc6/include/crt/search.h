/***
*search.h - declarations for searcing/sorting routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the declarations for the sorting and
*       searching routines.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_SEARCH
#define _INC_SEARCH

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  __cplusplus
extern "C" {
#endif


#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

/* Define _CRTIMP */

#ifndef _CRTIMP
#ifdef  _DLL
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */

/* Define __cdecl for non-Microsoft compilers */

#if     ( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif


#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif


/* Function prototypes */

_CRTIMP void * __cdecl bsearch(const void *, const void *, size_t, size_t,
        int (__cdecl *)(const void *, const void *));
_CRTIMP void * __cdecl _lfind(const void *, const void *, unsigned int *, unsigned int,
        int (__cdecl *)(const void *, const void *));
_CRTIMP void * __cdecl _lsearch(const void *, void  *, unsigned int *, unsigned int,
        int (__cdecl *)(const void *, const void *));
_CRTIMP void __cdecl qsort(void *, size_t, size_t, int (__cdecl *)(const void *,
        const void *));


#if     !__STDC__
/* Non-ANSI names for compatibility */
_CRTIMP void * __cdecl lfind(const void *, const void *, unsigned int *, unsigned int,
        int (__cdecl *)(const void *, const void *));
_CRTIMP void * __cdecl lsearch(const void *, void  *, unsigned int *, unsigned int,
        int (__cdecl *)(const void *, const void *));
#endif  /* __STDC__ */


#ifdef  __cplusplus
}
#endif

#endif  /* _INC_SEARCH */

