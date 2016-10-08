/***
*signal.h - defines signal values and routines
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the signal values and declares the signal functions.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_SIGNAL
#define _INC_SIGNAL

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


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

#ifndef _SIG_ATOMIC_T_DEFINED
typedef int sig_atomic_t;
#define _SIG_ATOMIC_T_DEFINED
#endif

#define NSIG 23     /* maximum signal number + 1 */


/* Signal types */

#define SIGINT          2       /* interrupt */
#define SIGILL          4       /* illegal instruction - invalid function image */
#define SIGFPE          8       /* floating point exception */
#define SIGSEGV         11      /* segment violation */
#define SIGTERM         15      /* Software termination signal from kill */
#define SIGBREAK        21      /* Ctrl-Break sequence */
#define SIGABRT         22      /* abnormal termination triggered by abort call */


/* signal action codes */

#define SIG_DFL (void (__cdecl *)(int))0           /* default signal action */
#define SIG_IGN (void (__cdecl *)(int))1           /* ignore signal */
#define SIG_SGE (void (__cdecl *)(int))3           /* signal gets error */
#define SIG_ACK (void (__cdecl *)(int))4           /* acknowledge */


/* signal error value (returned by signal call on error) */

#define SIG_ERR (void (__cdecl *)(int))-1          /* signal error value */


/* pointer to exception information pointers structure */

#if     defined(_MT) || defined(_DLL)
extern void * * __cdecl __pxcptinfoptrs(void);
#define _pxcptinfoptrs  (*__pxcptinfoptrs())
#else   /* ndef _MT && ndef _DLL */
extern void * _pxcptinfoptrs;
#endif  /* _MT || _DLL */


/* Function prototypes */

_CRTIMP void (__cdecl * __cdecl signal(int, void (__cdecl *)(int)))(int);
_CRTIMP int __cdecl raise(int);


#ifdef  __cplusplus
}
#endif

#endif  /* _INC_SIGNAL */

