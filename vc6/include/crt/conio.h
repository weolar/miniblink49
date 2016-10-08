/***
*conio.h - console and port I/O declarations
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This include file contains the function declarations for
*       the MS C V2.03 compatible console I/O routines.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_CONIO
#define _INC_CONIO

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef __cplusplus
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

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _WCTYPE_T_DEFINED
typedef unsigned short wint_t;
typedef unsigned short wctype_t;
#define _WCTYPE_T_DEFINED
#endif

/* Define __cdecl for non-Microsoft compilers */

#if     ( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif

/* Function prototypes */

_CRTIMP char * __cdecl _cgets(char *);
_CRTIMP int __cdecl _cprintf(const char *, ...);
_CRTIMP int __cdecl _cputs(const char *);
_CRTIMP int __cdecl _cscanf(const char *, ...);
_CRTIMP int __cdecl _getch(void);
_CRTIMP int __cdecl _getche(void);
#ifdef  _M_IX86
int __cdecl _inp(unsigned short);
unsigned short __cdecl _inpw(unsigned short);
unsigned long __cdecl _inpd(unsigned short);
#endif  /* _M_IX86 */
_CRTIMP int __cdecl _kbhit(void);
#ifdef  _M_IX86
int __cdecl _outp(unsigned short, int);
unsigned short __cdecl _outpw(unsigned short, unsigned short);
unsigned long __cdecl _outpd(unsigned short, unsigned long);
#endif  /* _M_IX86 */
_CRTIMP int __cdecl _putch(int);
_CRTIMP int __cdecl _ungetch(int);


#ifndef _WCONIO_DEFINED

/* wide function prototypes, also declared in wchar.h */

#ifndef WEOF
#define WEOF (wint_t)(0xFFFF)
#endif

_CRTIMP wchar_t * __cdecl _cgetws(wchar_t *);
_CRTIMP wint_t __cdecl _getwch(void);
_CRTIMP wint_t __cdecl _getwche(void);
_CRTIMP wint_t __cdecl _putwch(wchar_t);
_CRTIMP wint_t __cdecl _ungetwch(wint_t);
_CRTIMP int __cdecl _cputws(const wchar_t *);
_CRTIMP int __cdecl _cwprintf(const wchar_t *, ...);
_CRTIMP int __cdecl _cwscanf(const wchar_t *, ...);


#define _WCONIO_DEFINED
#endif  /* _WCONIO_DEFINED */

#if     !__STDC__

/* Non-ANSI names for compatibility */

_CRTIMP char * __cdecl cgets(char *);
_CRTIMP int __cdecl cprintf(const char *, ...);
_CRTIMP int __cdecl cputs(const char *);
_CRTIMP int __cdecl cscanf(const char *, ...);
#ifdef  _M_IX86
int __cdecl inp(unsigned short);
unsigned short __cdecl inpw(unsigned short);
#endif  /* _M_IX86 */
_CRTIMP int __cdecl getch(void);
_CRTIMP int __cdecl getche(void);
_CRTIMP int __cdecl kbhit(void);
#ifdef  _M_IX86
int __cdecl outp(unsigned short, int);
unsigned short __cdecl outpw(unsigned short, unsigned short);
#endif  /* _M_IX86 */
_CRTIMP int __cdecl putch(int);
_CRTIMP int __cdecl ungetch(int);

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#endif  /* _INC_CONIO */

