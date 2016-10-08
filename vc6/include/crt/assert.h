/***
*assert.h - define the assert macro
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Defines the assert(exp) macro.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
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

#undef  assert

#ifdef  NDEBUG

#define assert(exp)     ((void)0)

#else

#ifdef  __cplusplus
extern "C" {
#endif

_CRTIMP void __cdecl _assert(void *, void *, unsigned);

#ifdef  __cplusplus
}
#endif

#define assert(exp) (void)( (exp) || (_assert(#exp, __FILE__, __LINE__), 0) )

#endif  /* NDEBUG */

