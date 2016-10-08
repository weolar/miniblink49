/***
*string.h - declarations for string manipulation functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains the function declarations for the string
*       manipulation functions.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_STRING
#define _INC_STRING

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


#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _NLSCMP_DEFINED
#define _NLSCMPERROR    2147483647  /* currently == INT_MAX */
#define _NLSCMP_DEFINED
#endif

/* Define NULL pointer value */

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


/* Function prototypes */

#ifndef _MEMCPY_INLINE_
        void *  __cdecl memcpy(void *, const void *, size_t);
#endif
        int     __cdecl memcmp(const void *, const void *, size_t);
        void *  __cdecl memset(void *, int, size_t);
        char *  __cdecl _strset(char *, int);
        char *  __cdecl strcpy(char *, const char *);
        char *  __cdecl strcat(char *, const char *);
        int     __cdecl strcmp(const char *, const char *);
        size_t  __cdecl strlen(const char *);
_CRTIMP void *  __cdecl _memccpy(void *, const void *, int, size_t);
_CRTIMP void *  __cdecl memchr(const void *, int, size_t);
_CRTIMP int     __cdecl _memicmp(const void *, const void *, size_t);

#if defined(_M_AMD64) || defined(_M_IA64)
        void *  __cdecl memmove(void *, const void *, size_t);
#else
_CRTIMP void *  __cdecl memmove(void *, const void *, size_t);
#endif

_CRTIMP char *  __cdecl strchr(const char *, int);
_CRTIMP int     __cdecl _strcmpi(const char *, const char *);
_CRTIMP int     __cdecl _stricmp(const char *, const char *);
_CRTIMP int     __cdecl strcoll(const char *, const char *);
_CRTIMP int     __cdecl _stricoll(const char *, const char *);
_CRTIMP int     __cdecl _strncoll(const char *, const char *, size_t);
_CRTIMP int     __cdecl _strnicoll(const char *, const char *, size_t);
_CRTIMP size_t  __cdecl strcspn(const char *, const char *);
_CRTIMP char *  __cdecl _strdup(const char *);
_CRTIMP char *  __cdecl _strerror(const char *);
_CRTIMP char *  __cdecl strerror(int);
_CRTIMP char *  __cdecl _strlwr(char *);
_CRTIMP char *  __cdecl strncat(char *, const char *, size_t);
_CRTIMP int     __cdecl strncmp(const char *, const char *, size_t);
_CRTIMP int     __cdecl _strnicmp(const char *, const char *, size_t);
_CRTIMP char *  __cdecl strncpy(char *, const char *, size_t);
_CRTIMP char *  __cdecl _strnset(char *, int, size_t);
_CRTIMP char *  __cdecl strpbrk(const char *, const char *);
_CRTIMP char *  __cdecl strrchr(const char *, int);
_CRTIMP char *  __cdecl _strrev(char *);
_CRTIMP size_t  __cdecl strspn(const char *, const char *);
_CRTIMP char *  __cdecl strstr(const char *, const char *);
_CRTIMP char *  __cdecl strtok(char *, const char *);
_CRTIMP char *  __cdecl _strupr(char *);
_CRTIMP size_t  __cdecl strxfrm (char *, const char *, size_t);


#if     !__STDC__

/* prototypes for oldnames.lib functions */
_CRTIMP void * __cdecl memccpy(void *, const void *, int, size_t);
_CRTIMP int __cdecl memicmp(const void *, const void *, size_t);
_CRTIMP int __cdecl strcmpi(const char *, const char *);
_CRTIMP int __cdecl stricmp(const char *, const char *);
_CRTIMP char * __cdecl strdup(const char *);
_CRTIMP char * __cdecl strlwr(char *);
_CRTIMP int __cdecl strnicmp(const char *, const char *, size_t);
_CRTIMP char * __cdecl strnset(char *, int, size_t);
_CRTIMP char * __cdecl strrev(char *);
        char * __cdecl strset(char *, int);
_CRTIMP char * __cdecl strupr(char *);

#endif  /* !__STDC__ */


#ifndef _WSTRING_DEFINED

/* wide function prototypes, also declared in wchar.h  */

_CRTIMP wchar_t * __cdecl wcscat(wchar_t *, const wchar_t *);
_CRTIMP wchar_t * __cdecl wcschr(const wchar_t *, wchar_t);
_CRTIMP int __cdecl wcscmp(const wchar_t *, const wchar_t *);
_CRTIMP wchar_t * __cdecl wcscpy(wchar_t *, const wchar_t *);
_CRTIMP size_t __cdecl wcscspn(const wchar_t *, const wchar_t *);
_CRTIMP size_t __cdecl wcslen(const wchar_t *);
_CRTIMP wchar_t * __cdecl wcsncat(wchar_t *, const wchar_t *, size_t);
_CRTIMP int __cdecl wcsncmp(const wchar_t *, const wchar_t *, size_t);
_CRTIMP wchar_t * __cdecl wcsncpy(wchar_t *, const wchar_t *, size_t);
_CRTIMP wchar_t * __cdecl wcspbrk(const wchar_t *, const wchar_t *);
_CRTIMP wchar_t * __cdecl wcsrchr(const wchar_t *, wchar_t);
_CRTIMP size_t __cdecl wcsspn(const wchar_t *, const wchar_t *);
_CRTIMP wchar_t * __cdecl wcsstr(const wchar_t *, const wchar_t *);
_CRTIMP wchar_t * __cdecl wcstok(wchar_t *, const wchar_t *);
_CRTIMP wchar_t * __cdecl _wcserror(int);
_CRTIMP wchar_t * __cdecl __wcserror(const wchar_t *);

_CRTIMP wchar_t * __cdecl _wcsdup(const wchar_t *);
_CRTIMP int __cdecl _wcsicmp(const wchar_t *, const wchar_t *);
_CRTIMP int __cdecl _wcsnicmp(const wchar_t *, const wchar_t *, size_t);
_CRTIMP wchar_t * __cdecl _wcsnset(wchar_t *, wchar_t, size_t);
_CRTIMP wchar_t * __cdecl _wcsrev(wchar_t *);
_CRTIMP wchar_t * __cdecl _wcsset(wchar_t *, wchar_t);

_CRTIMP wchar_t * __cdecl _wcslwr(wchar_t *);
_CRTIMP wchar_t * __cdecl _wcsupr(wchar_t *);
_CRTIMP size_t __cdecl wcsxfrm(wchar_t *, const wchar_t *, size_t);
_CRTIMP int __cdecl wcscoll(const wchar_t *, const wchar_t *);
_CRTIMP int __cdecl _wcsicoll(const wchar_t *, const wchar_t *);
_CRTIMP int __cdecl _wcsncoll(const wchar_t *, const wchar_t *, size_t);
_CRTIMP int __cdecl _wcsnicoll(const wchar_t *, const wchar_t *, size_t);

#if     !__STDC__

/* old names */
#define wcswcs wcsstr

/* prototypes for oldnames.lib functions */
_CRTIMP wchar_t * __cdecl wcsdup(const wchar_t *);
_CRTIMP int __cdecl wcsicmp(const wchar_t *, const wchar_t *);
_CRTIMP int __cdecl wcsnicmp(const wchar_t *, const wchar_t *, size_t);
_CRTIMP wchar_t * __cdecl wcsnset(wchar_t *, wchar_t, size_t);
_CRTIMP wchar_t * __cdecl wcsrev(wchar_t *);
_CRTIMP wchar_t * __cdecl wcsset(wchar_t *, wchar_t);
_CRTIMP wchar_t * __cdecl wcslwr(wchar_t *);
_CRTIMP wchar_t * __cdecl wcsupr(wchar_t *);
_CRTIMP int __cdecl wcsicoll(const wchar_t *, const wchar_t *);

#endif  /* !__STDC__ */

#define _WSTRING_DEFINED
#endif

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

#ifdef  __cplusplus
}
#endif

#endif  /* _INC_STRING */

