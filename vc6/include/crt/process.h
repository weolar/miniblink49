/***
*process.h - definition and declarations for process control functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file defines the modeflag values for spawnxx calls.
*       Also contains the function argument declarations for all
*       process control related routines.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_PROCESS
#define _INC_PROCESS

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifndef _POSIX_

#ifdef __cplusplus
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

#ifndef _INTPTR_T_DEFINED
#ifdef  _WIN64
typedef __int64             intptr_t;
#else
typedef _W64 int            intptr_t;
#endif
#define _INTPTR_T_DEFINED
#endif

#ifndef _UINTPTR_T_DEFINED
#ifdef  _WIN64
typedef unsigned __int64    uintptr_t;
#else
typedef _W64 unsigned int   uintptr_t;
#endif
#define _UINTPTR_T_DEFINED
#endif

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif


/* modeflag values for _spawnxx routines */

#define _P_WAIT         0
#define _P_NOWAIT       1
#define _OLD_P_OVERLAY  2
#define _P_NOWAITO      3
#define _P_DETACH       4

#ifdef  _MT
#define _P_OVERLAY      2
#else
extern int _p_overlay;
#define _P_OVERLAY      _p_overlay
#endif  /* _MT */

/* Action codes for _cwait(). The action code argument to _cwait is ignored
   on Win32 though it is accepted for compatibilty with old MS CRT libs */
#define _WAIT_CHILD      0
#define _WAIT_GRANDCHILD 1


/* function prototypes */

#ifdef  _MT
_CRTIMP uintptr_t __cdecl _beginthread (void (__cdecl *) (void *),
        unsigned, void *);
_CRTIMP void __cdecl _endthread(void);
_CRTIMP uintptr_t __cdecl _beginthreadex(void *, unsigned,
        unsigned (__stdcall *) (void *), void *, unsigned, unsigned *);
_CRTIMP void __cdecl _endthreadex(unsigned);
#endif

#if     _MSC_VER >= 1200
_CRTIMP __declspec(noreturn) void __cdecl abort(void);
_CRTIMP __declspec(noreturn) void __cdecl exit(int);
_CRTIMP __declspec(noreturn) void __cdecl _exit(int);
#else
_CRTIMP void __cdecl abort(void);
_CRTIMP void __cdecl exit(int);
_CRTIMP void __cdecl _exit(int);
#endif
_CRTIMP void __cdecl _cexit(void);
_CRTIMP void __cdecl _c_exit(void);
_CRTIMP int __cdecl _getpid(void);

_CRTIMP intptr_t __cdecl _cwait(int *, intptr_t, int);
_CRTIMP intptr_t __cdecl _execl(const char *, const char *, ...);
_CRTIMP intptr_t __cdecl _execle(const char *, const char *, ...);
_CRTIMP intptr_t __cdecl _execlp(const char *, const char *, ...);
_CRTIMP intptr_t __cdecl _execlpe(const char *, const char *, ...);
_CRTIMP intptr_t __cdecl _execv(const char *, const char * const *);
_CRTIMP intptr_t __cdecl _execve(const char *, const char * const *, const char * const *);
_CRTIMP intptr_t __cdecl _execvp(const char *, const char * const *);
_CRTIMP intptr_t __cdecl _execvpe(const char *, const char * const *, const char * const *);
_CRTIMP intptr_t __cdecl _spawnl(int, const char *, const char *, ...);
_CRTIMP intptr_t __cdecl _spawnle(int, const char *, const char *, ...);
_CRTIMP intptr_t __cdecl _spawnlp(int, const char *, const char *, ...);
_CRTIMP intptr_t __cdecl _spawnlpe(int, const char *, const char *, ...);
_CRTIMP intptr_t __cdecl _spawnv(int, const char *, const char * const *);
_CRTIMP intptr_t __cdecl _spawnve(int, const char *, const char * const *,
        const char * const *);
_CRTIMP intptr_t __cdecl _spawnvp(int, const char *, const char * const *);
_CRTIMP intptr_t __cdecl _spawnvpe(int, const char *, const char * const *,
        const char * const *);
_CRTIMP int __cdecl system(const char *);

#ifndef _WPROCESS_DEFINED
/* wide function prototypes, also declared in wchar.h  */
_CRTIMP intptr_t __cdecl _wexecl(const wchar_t *, const wchar_t *, ...);
_CRTIMP intptr_t __cdecl _wexecle(const wchar_t *, const wchar_t *, ...);
_CRTIMP intptr_t __cdecl _wexeclp(const wchar_t *, const wchar_t *, ...);
_CRTIMP intptr_t __cdecl _wexeclpe(const wchar_t *, const wchar_t *, ...);
_CRTIMP intptr_t __cdecl _wexecv(const wchar_t *, const wchar_t * const *);
_CRTIMP intptr_t __cdecl _wexecve(const wchar_t *, const wchar_t * const *, const wchar_t * const *);
_CRTIMP intptr_t __cdecl _wexecvp(const wchar_t *, const wchar_t * const *);
_CRTIMP intptr_t __cdecl _wexecvpe(const wchar_t *, const wchar_t * const *, const wchar_t * const *);
_CRTIMP intptr_t __cdecl _wspawnl(int, const wchar_t *, const wchar_t *, ...);
_CRTIMP intptr_t __cdecl _wspawnle(int, const wchar_t *, const wchar_t *, ...);
_CRTIMP intptr_t __cdecl _wspawnlp(int, const wchar_t *, const wchar_t *, ...);
_CRTIMP intptr_t __cdecl _wspawnlpe(int, const wchar_t *, const wchar_t *, ...);
_CRTIMP intptr_t __cdecl _wspawnv(int, const wchar_t *, const wchar_t * const *);
_CRTIMP intptr_t __cdecl _wspawnve(int, const wchar_t *, const wchar_t * const *,
        const wchar_t * const *);
_CRTIMP intptr_t __cdecl _wspawnvp(int, const wchar_t *, const wchar_t * const *);
_CRTIMP intptr_t __cdecl _wspawnvpe(int, const wchar_t *, const wchar_t * const *,
        const wchar_t * const *);
_CRTIMP int __cdecl _wsystem(const wchar_t *);

#define _WPROCESS_DEFINED
#endif

/* --------- The following functions are OBSOLETE --------- */
/*
 * The Win32 API LoadLibrary, FreeLibrary and GetProcAddress should be used
 * instead.
 */
intptr_t __cdecl _loaddll(char *);
int __cdecl _unloaddll(intptr_t);
int (__cdecl * __cdecl _getdllprocaddr(intptr_t, char *, intptr_t))(void);
/* --------- The preceding functions are OBSOLETE --------- */


#ifdef  _DECL_DLLMAIN
/*
 * Declare DLL notification (initialization/termination) routines
 *      The preferred method is for the user to provide DllMain() which will
 *      be called automatically by the DLL entry point defined by the C run-
 *      time library code.  If the user wants to define the DLL entry point
 *      routine, the user's entry point must call _CRT_INIT on all types of
 *      notifications, as the very first thing on attach notifications and
 *      as the very last thing on detach notifications.
 */
#ifdef  _WINDOWS_       /* Use types from WINDOWS.H */
BOOL WINAPI DllMain(HANDLE, DWORD, LPVOID);
BOOL WINAPI _CRT_INIT(HANDLE, DWORD, LPVOID);
BOOL WINAPI _wCRT_INIT(HANDLE, DWORD, LPVOID);
extern BOOL (WINAPI *_pRawDllMain)(HANDLE, DWORD, LPVOID);
#else
int __stdcall DllMain(void *, unsigned, void *);
int __stdcall _CRT_INIT(void *, unsigned, void *);
int __stdcall _wCRT_INIT(void *, unsigned, void *);
extern int (__stdcall *_pRawDllMain)(void *, unsigned, void *);
#endif  /* _WINDOWS_ */
#endif

#if     !__STDC__

/* Non-ANSI names for compatibility */

#define P_WAIT          _P_WAIT
#define P_NOWAIT        _P_NOWAIT
#define P_OVERLAY       _P_OVERLAY
#define OLD_P_OVERLAY   _OLD_P_OVERLAY
#define P_NOWAITO       _P_NOWAITO
#define P_DETACH        _P_DETACH
#define WAIT_CHILD      _WAIT_CHILD
#define WAIT_GRANDCHILD _WAIT_GRANDCHILD

/* current declarations */
_CRTIMP intptr_t __cdecl cwait(int *, intptr_t, int);
_CRTIMP intptr_t __cdecl execl(const char *, const char *, ...);
_CRTIMP intptr_t __cdecl execle(const char *, const char *, ...);
_CRTIMP intptr_t __cdecl execlp(const char *, const char *, ...);
_CRTIMP intptr_t __cdecl execlpe(const char *, const char *, ...);
_CRTIMP intptr_t __cdecl execv(const char *, const char * const *);
_CRTIMP intptr_t __cdecl execve(const char *, const char * const *, const char * const *);
_CRTIMP intptr_t __cdecl execvp(const char *, const char * const *);
_CRTIMP intptr_t __cdecl execvpe(const char *, const char * const *, const char * const *);
_CRTIMP intptr_t __cdecl spawnl(int, const char *, const char *, ...);
_CRTIMP intptr_t __cdecl spawnle(int, const char *, const char *, ...);
_CRTIMP intptr_t __cdecl spawnlp(int, const char *, const char *, ...);
_CRTIMP intptr_t __cdecl spawnlpe(int, const char *, const char *, ...);
_CRTIMP intptr_t __cdecl spawnv(int, const char *, const char * const *);
_CRTIMP intptr_t __cdecl spawnve(int, const char *, const char * const *,
        const char * const *);
_CRTIMP intptr_t __cdecl spawnvp(int, const char *, const char * const *);
_CRTIMP intptr_t __cdecl spawnvpe(int, const char *, const char * const *,
        const char * const *);

_CRTIMP int __cdecl getpid(void);

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#endif  /* _POSIX_ */

#endif  /* _INC_PROCESS */

