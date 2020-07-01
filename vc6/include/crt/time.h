/***
*time.h - definitions/declarations for time routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file has declarations of time routines and defines
*       the structure returned by the localtime and gmtime routines and
*       used by asctime.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_TIME
#define _INC_TIME

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,8)
#endif  /* _MSC_VER */

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

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

/* Define the implementation defined time type */

#ifndef _TIME_T_DEFINED
#if 1 // def  _WIN64            /* fix vc6 time bug */
typedef __int64   time_t;       /* time value */
#else
typedef _W64 long time_t;       /* time value */
#endif
#define _TIME_T_DEFINED         /* avoid multiple def's of time_t */
#endif

#ifndef _TIME64_T_DEFINED
#if     _INTEGRAL_MAX_BITS >= 64
typedef __int64 __time64_t;     /* 64-bit time value */
#endif
#define _TIME64_T_DEFINED
#endif

#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif

#ifndef _SIZE_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    size_t;
#else
typedef _W64 unsigned int   size_t;
#endif
#define _SIZE_T_DEFINED
#endif


/* Define NULL pointer value */

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


#ifndef _TM_DEFINED
struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };
#define _TM_DEFINED
#endif


/* Clock ticks macro - ANSI version */

#define CLOCKS_PER_SEC  1000


/* Extern declarations for the global variables used by the ctime family of
 * routines.
 */

/* non-zero if daylight savings time is used */
_CRTIMP extern int _daylight;

/* offset for Daylight Saving Time */
_CRTIMP extern long _dstbias;

/* difference in seconds between GMT and local time */
_CRTIMP extern long _timezone;

/* standard/daylight savings time zone names */
_CRTIMP extern char * _tzname[2];


/* Function prototypes */

// _CRTIMP char * __cdecl asctime(const struct tm *);
// _CRTIMP char * __cdecl ctime(const time_t *);
// _CRTIMP clock_t __cdecl clock(void);
// _CRTIMP double __cdecl difftime(time_t, time_t);
// _CRTIMP struct tm * __cdecl gmtime(const time_t *);
// _CRTIMP struct tm * __cdecl localtime(const time_t *);
// _CRTIMP time_t __cdecl mktime(struct tm *);
// _CRTIMP time_t __cdecl _mkgmtime(struct tm *);
_CRTIMP size_t __cdecl strftime(char *, size_t, const char *, const struct tm *);
// _CRTIMP char * __cdecl _strdate(char *);
// _CRTIMP char * __cdecl _strtime(char *);
//_CRTIMP time_t __cdecl time(time_t *);

time_t time_64(time_t *pt);
struct tm* gmtime_64(const time_t *);
struct tm* localtime_64(const __int64 *pt);
time_t mktime_64(struct tm *);

__inline __int64 __cdecl time(__int64* t)
{
    return time_64(t);
}

__inline struct tm* __cdecl gmtime(const time_t* t)
{
    return gmtime_64(t);
}

__inline time_t __cdecl mktime(struct tm* t)
{
    return mktime_64(t);
}

__inline struct tm* localtime(const time_t* t)
{
    return localtime_64(t);
}

#ifdef  _POSIX_
_CRTIMP void __cdecl tzset(void);
#else
_CRTIMP void __cdecl _tzset(void);
#endif

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP char * __cdecl _ctime64(const __time64_t *);
_CRTIMP struct tm * __cdecl _gmtime64(const __time64_t *);
_CRTIMP struct tm * __cdecl _localtime64(const __time64_t *);
_CRTIMP __time64_t __cdecl _mktime64(struct tm *);
_CRTIMP __time64_t __cdecl _mkgmtime64(struct tm *);
_CRTIMP __time64_t __cdecl _time64(__time64_t *);
#endif

/* --------- The following functions are OBSOLETE --------- */
/* The Win32 API GetLocalTime and SetLocalTime should be used instead. */
// unsigned __cdecl _getsystime(struct tm *);
// unsigned __cdecl _setsystime(struct tm *, unsigned);
/* --------- The preceding functions are OBSOLETE --------- */


#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef _WTIME_DEFINED

/* wide function prototypes, also declared in wchar.h */
 
_CRTIMP wchar_t * __cdecl _wasctime(const struct tm *);
// _CRTIMP wchar_t * __cdecl _wctime(const time_t *);
// _CRTIMP size_t __cdecl wcsftime(wchar_t *, size_t, const wchar_t *,
//         const struct tm *);
// _CRTIMP wchar_t * __cdecl _wstrdate(wchar_t *);
// _CRTIMP wchar_t * __cdecl _wstrtime(wchar_t *);

#if     _INTEGRAL_MAX_BITS >= 64
_CRTIMP wchar_t * __cdecl _wctime64(const __time64_t *);
#endif

#define _WTIME_DEFINED
#endif


#if     !__STDC__ || defined(_POSIX_)

/* Non-ANSI names for compatibility */

#define CLK_TCK  CLOCKS_PER_SEC

_CRTIMP extern int daylight;
_CRTIMP extern long timezone;
_CRTIMP extern char * tzname[2];

_CRTIMP void __cdecl tzset(void);

#endif  /* __STDC__ */


#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_TIME */

