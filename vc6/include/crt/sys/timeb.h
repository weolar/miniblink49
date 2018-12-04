/***
*sys/timeb.h - definition/declarations for _ftime()
*
*       Copyright (c) 1985-2001, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file define the _ftime() function and the types it uses.
*       [System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_TIMEB
#define _INC_TIMEB

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


#ifndef _TIME_T_DEFINED
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


/* Structure returned by _ftime system call */

#ifndef _TIMEB_DEFINED
struct _timeb {
        time_t time;
        unsigned short millitm;
        short timezone;
        short dstflag;
        };

#if     !__STDC__

/* Non-ANSI name for compatibility */

struct timeb {
        time_t time;
        unsigned short millitm;
        short timezone;
        short dstflag;
        };

#endif

#if     _INTEGRAL_MAX_BITS >= 64
struct __timeb64 {
        __time64_t time;
        unsigned short millitm;
        short timezone;
        short dstflag;
        };
#endif

#define _TIMEB_DEFINED
#endif


/* Function prototypes */

//_CRTIMP void __cdecl _ftime(struct _timeb *);
void __cdecl _ftime_64(struct _timeb *);

inline void __cdecl _ftime(struct _timeb* t)
{
    _ftime_64(t);
}

#if     !__STDC__

/* Non-ANSI name for compatibility */

//_CRTIMP void __cdecl ftime(struct timeb *);

#endif

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP void __cdecl _ftime64(struct __timeb64 *);
#endif

#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_TIMEB */

