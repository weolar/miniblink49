/***
*sys/utime.h - definitions/declarations for utime()
*
*       Copyright (c) 1985-2001, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the structure used by the utime routine to set
*       new file access and modification times.  NOTE - MS-DOS
*       does not recognize access time, so this field will
*       always be ignored and the modification time field will be
*       used to set the new time.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_UTIME
#define _INC_UTIME

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  _MSC_VER
#pragma pack(push,8)
#endif  /* _MSC_VER */

#ifdef  __cplusplus
extern "C" {
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

#if !defined(_WCHAR_T_DEFINED) && !defined(_NATIVE_WCHAR_T_DEFINED)
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#if 1 // ndef _TIME_T_DEFINED
#if 1 // def  _WIN64
typedef __int64 time_t;         /* time value */
#else
typedef long    time_t;         /* time value */
#endif
#if     _INTEGRAL_MAX_BITS >= 64
typedef __int64 __time64_t;
#endif
#define _TIME_T_DEFINED         /* avoid multiple def's of time_t */
#endif

/* define struct used by _utime() function */

#ifndef _UTIMBUF_DEFINED

struct _utimbuf {
        time_t actime;          /* access time */
        time_t modtime;         /* modification time */
        };

#if     !__STDC__
/* Non-ANSI name for compatibility */
struct utimbuf {
        time_t actime;          /* access time */
        time_t modtime;         /* modification time */
        };
#endif

#if     _INTEGRAL_MAX_BITS >= 64
struct __utimbuf64 {
        __time64_t actime;      /* access time */
        __time64_t modtime;     /* modification time */
        };
#endif

#define _UTIMBUF_DEFINED
#endif


/* Function Prototypes */

// _CRTIMP int __cdecl _utime(const char *, struct _utimbuf *);
// 
// _CRTIMP int __cdecl _futime(int, struct _utimbuf *);
// 
// /* Wide Function Prototypes */
// _CRTIMP int __cdecl _wutime(const wchar_t *, struct _utimbuf *);

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP int __cdecl _utime64(const char *, struct __utimbuf64 *);
_CRTIMP int __cdecl _futime64(int, struct __utimbuf64 *);
_CRTIMP int __cdecl _wutime64(const wchar_t *, struct __utimbuf64 *);
#endif

#if     !__STDC__
/* Non-ANSI name for compatibility */
//_CRTIMP int __cdecl utime(const char *, struct utimbuf *);
#endif

#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_UTIME */

