/***
*locale.h - definitions/declarations for localization routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the structures, values, macros, and functions
*       used by the localization routines.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_LOCALE
#define _INC_LOCALE

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

/* define NULL pointer value */

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

/* Locale categories */

#define LC_ALL          0
#define LC_COLLATE      1
#define LC_CTYPE        2
#define LC_MONETARY     3
#define LC_NUMERIC      4
#define LC_TIME         5

#define LC_MIN          LC_ALL
#define LC_MAX          LC_TIME

/* Locale convention structure */

#ifndef _LCONV_DEFINED
struct lconv {
        char *decimal_point;
        char *thousands_sep;
        char *grouping;
        char *int_curr_symbol;
        char *currency_symbol;
        char *mon_decimal_point;
        char *mon_thousands_sep;
        char *mon_grouping;
        char *positive_sign;
        char *negative_sign;
        char int_frac_digits;
        char frac_digits;
        char p_cs_precedes;
        char p_sep_by_space;
        char n_cs_precedes;
        char n_sep_by_space;
        char p_sign_posn;
        char n_sign_posn;
        };
#define _LCONV_DEFINED
#endif

/* ANSI: char lconv members default is CHAR_MAX which is compile time
   dependent. Defining and using _charmax here causes CRT startup code
   to initialize lconv members properly */

#ifdef  _CHAR_UNSIGNED
extern int _charmax;
extern __inline int __dummy() { return _charmax; }
#endif

/* function prototypes */

#ifndef _CONFIG_LOCALE_SWT
#define _ENABLE_PER_THREAD_LOCALE           0x1
#define _DISABLE_PER_THREAD_LOCALE          0x2
#define _ENABLE_PER_THREAD_LOCALE_GLOBAL    0x10
#define _DISABLE_PER_THREAD_LOCALE_GLOBAL   0x20
#define _ENABLE_PER_THREAD_LOCALE_NEW       0x100
#define _DISABLE_PER_THREAD_LOCALE_NEW      0x200
#define _CONFIG_LOCALE_SWT
#endif

_CRTIMP int __cdecl _configthreadlocale(int i);
_CRTIMP char * __cdecl setlocale(int, const char *);
_CRTIMP struct lconv * __cdecl localeconv(void);

#ifndef _WLOCALE_DEFINED

/* wide function prototypes, also declared in wchar.h  */

_CRTIMP wchar_t * __cdecl _wsetlocale(int, const wchar_t *);

#define _WLOCALE_DEFINED
#endif

#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_LOCALE */

