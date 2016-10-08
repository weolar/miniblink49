/***
*memory.h - declarations for buffer (memory) manipulation routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This include file contains the function declarations for the
*       buffer (memory) manipulation routines.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_MEMORY
#define _INC_MEMORY

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

_CRTIMP void *  __cdecl _memccpy(void *, const void *, int, size_t);
_CRTIMP void *  __cdecl memchr(const void *, int, size_t);
_CRTIMP int     __cdecl _memicmp(const void *, const void *, size_t);
        int     __cdecl memcmp(const void *, const void *, size_t);
#ifndef _MEMCPY_INLINE_
        void *  __cdecl memcpy(void *, const void *, size_t);
#endif
        void *  __cdecl memset(void *, int, size_t);

#if _DBG_MEMCPY_INLINE_ && !defined(MIDL_PASS) && !defined(_MEMCPY_INLINE_) && !defined(_CRTBLD)
#define _MEMCPY_INLINE_
__forceinline
void *
__cdecl 
memcpy_inline (
    void *dst,
    const void *src,
    size_t size
    )
{
    //
    // Make sure the source and destination do not overlap such that the
    // move destroys the destination.
    //
    if (((char *)dst > (char *)src) &&
        ((char *)dst < ((char *)src + size))) {
        __debugbreak();
    }
    return memcpy(dst, src, size);
}
#define memcpy memcpy_inline
#endif

#if     !__STDC__

/* Non-ANSI names for compatibility */

_CRTIMP void * __cdecl memccpy(void *, const void *, int, size_t);
_CRTIMP int __cdecl memicmp(const void *, const void *, size_t);

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#endif  /* _INC_MEMORY */

