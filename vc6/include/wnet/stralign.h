/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    stralign.h

Abstract:

    This module contains macros and prototypes to expose the unaligned wide
    character interfaces.

    Public interfaces created or declared here include:

    ua_CharUpper()
    ua_CharUpperW()
    ua_lstrcmp()
    ua_lstrcmpW()
    ua_lstrcmpi()
    ua_lstrcmpiW()
    ua_lstrlen()
    ua_lstrlenW()
    ua_tcscpy()
    ua_tcscpy_s()
    ua_wcschr()
    ua_wcscpy()
    ua_wcscpy_s()
    ua_wcsicmp()
    ua_wcslen()
    ua_wcsrchr()

    STRUC_ALIGNED_STACK_COPY()
    TSTR_ALIGNED()
    TSTR_ALIGNED_STACK_COPY()
    WSTR_ALIGNED()
    WSTR_ALIGNED_STACK_COPY()

Author:


Revision History:

--*/

#if !defined(__STRALIGN_H_) && !defined(MIDL_PASS)
#define __STRALIGN_H_

#ifndef _STRALIGN_USE_SECURE_CRT
#if defined(__GOT_SECURE_LIB__) && __GOT_SECURE_LIB__ >= 200402L
#define _STRALIGN_USE_SECURE_CRT 1
#else
#define _STRALIGN_USE_SECURE_CRT 0
#endif
#endif

#if !defined(_WINDOWS_INSECURE_DEPRECATE)
#if defined(_WINDOWS_SECURE_NO_DEPRECATE) || !_STRALIGN_USE_SECURE_CRT
#define _WINDOWS_INSECURE_DEPRECATE
#else
#define _WINDOWS_INSECURE_DEPRECATE __declspec(deprecated)
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_X86_)

//
// Alignment of unicode strings is not necessary on X86.
//

#define WSTR_ALIGNED(s) TRUE

#define ua_CharUpperW CharUpperW
#define ua_lstrcmpiW  lstrcmpiW
#define ua_lstrcmpW   lstrcmpW
#define ua_lstrlenW   lstrlenW
#define ua_wcschr     wcschr
#define ua_wcsicmp    wcsicmp
#define ua_wcslen     wcslen
#define ua_wcsrchr    wcsrchr
#if _STRALIGN_USE_SECURE_CRT
#define ua_wcscpy_s   wcscpy_s
#endif


__inline
PUWSTR
static
_WINDOWS_INSECURE_DEPRECATE
ua_wcscpy(
    IN PUWSTR  Destination,
    IN PCUWSTR Source
    )
{
#pragma warning(push)
#pragma warning(disable:4996)
    return wcscpy(Destination, Source);
#pragma warning(pop)
}

#else

//
// The C runtime libraries expect aligned string pointers.  Following are the
// prototypes for our own, slower worker functions that accept unaligned
// UNICODE strings.
//

//
// Macro to determine whether a pointer to a unicode character is naturally
// aligned.
//

#define WSTR_ALIGNED(s) (((DWORD_PTR)(s) & (sizeof(WCHAR)-1)) == 0)

//
// Platform-specific prototypes for worker functions exported from kernel32.
// Do not call these directly, they do not exist on all platforms.  Instead
// use the equivalent ua_xxx() routines.
//

LPUWSTR
WINAPI
uaw_CharUpperW(
    IN OUT LPUWSTR String
    );

int
APIENTRY
uaw_lstrcmpW(
    IN PCUWSTR String1,
    IN PCUWSTR String2
    );

int
APIENTRY
uaw_lstrcmpiW(
    IN PCUWSTR String1,
    IN PCUWSTR String2
    );

int
WINAPI
uaw_lstrlenW(
    IN LPCUWSTR String
    );

PUWSTR
__cdecl
uaw_wcschr(
    IN PCUWSTR String,
    IN WCHAR   Character
    );

PUWSTR
_cdecl
uaw_wcscpy(
    IN PUWSTR Destination,
    IN PCUWSTR Source
    );

int
__cdecl
uaw_wcsicmp(
    IN PCUWSTR String1,
    IN PCUWSTR String2
    );

size_t
__cdecl
uaw_wcslen(
    IN PCUWSTR String
    );

PUWSTR
__cdecl
uaw_wcsrchr(
    IN PCUWSTR String,
    IN WCHAR   Character
    );

//
// Following are the inline wrappers that determine the optimal worker function
// to call based on the alignment of the UNICODE string arguments.  Their
// behavior is otherwise identical to the corresponding standard run-time
// routiunes.
//

#if defined(CharUpper)
__inline
LPUWSTR
static
ua_CharUpperW(
    LPUWSTR String
    )
{
    if (WSTR_ALIGNED(String)) {
        return CharUpperW( (PWSTR)String );
    } else {
        return uaw_CharUpperW( String );
    }
}
#endif

#if defined(lstrcmp)
__inline
int
static
ua_lstrcmpW(
    IN LPCUWSTR String1,
    IN LPCUWSTR String2
    )
{
    if (WSTR_ALIGNED(String1) && WSTR_ALIGNED(String2)) {
        return lstrcmpW( (LPCWSTR)String1, (LPCWSTR)String2);
    } else {
        return uaw_lstrcmpW( String1, String2 );
    }
}
#endif

#if defined(lstrcmpi)
__inline
int
static
ua_lstrcmpiW(
    IN LPCUWSTR String1,
    IN LPCUWSTR String2
    )
{
    if (WSTR_ALIGNED(String1) && WSTR_ALIGNED(String2)) {
        return lstrcmpiW( (LPCWSTR)String1, (LPCWSTR)String2 );
    } else {
        return uaw_lstrcmpiW( String1, String2 );
    }
}
#endif

#if defined(lstrlen)
__inline
int
static
ua_lstrlenW(
    IN LPCUWSTR String
    )
{
    if (WSTR_ALIGNED(String)) {
        return lstrlenW( (PCWSTR)String );
    } else {
        return uaw_lstrlenW( String );
    }
}
#endif

#if defined(_WSTRING_DEFINED)

//
// Certain run-time string functions are overloaded in C++, to avoid
// inadvertent stripping of the const attribute.
//
// The functions of interest here include: wcschr and wcsrchr.
//
// There are three flavors of these functions:
//
// Flavor  Returns    Parameter
//
// 1       PWSTR      PCWSTR
// 2       PCWSTR     PCWSTR
// 3       PWSTR      PWSTR
//
// string.h declares flavor 1 whether for C or C++.  This is the non-ANSI,
// backward compatible mode.
//
// wchar.h declares flavor 1 if C, or flavors 2 and 3 if C++.  This is the
// ANSI method.
//
// Our corresponding functions need to match what was declared.  The way
// we can tell is by looking at _WConst_return... if it is defined then
// we want to match the prototypes in wchar.h, otherwise we'll match
// the prototypes in string.h.
//

#if defined(_WConst_return) 
typedef _WConst_return WCHAR UNALIGNED *PUWSTR_C;
#else
typedef WCHAR UNALIGNED *PUWSTR_C;
#endif

//
// Here is flavor 1 or 2
//

__inline
PUWSTR_C
static
ua_wcschr(
    IN PCUWSTR String,
    IN WCHAR   Character
    )
{
    if (WSTR_ALIGNED(String)) {
        return wcschr((PCWSTR)String, Character);
    } else {
        return (PUWSTR_C)uaw_wcschr(String, Character);
    }
}

__inline
PUWSTR_C
static
ua_wcsrchr(
    IN PCUWSTR String,
    IN WCHAR   Character
    )
{
    if (WSTR_ALIGNED(String)) {
        return wcsrchr((PCWSTR)String, Character);
    } else {
        return (PUWSTR_C)uaw_wcsrchr(String, Character);
    }
}

#if defined(__cplusplus) && defined(_WConst_Return)

//
// Here is flavor 3
//

__inline
PUWSTR
static
_WINDOWS_INSECURE_DEPRECATE
ua_wcschr(
    IN PUWSTR String,
    IN WCHAR  Character
    )
{
    if (WSTR_ALIGNED(String)) {
#pragma warning(push)
#pragma warning(disable:4996)
        return wcscpy( (PWSTR)Destination, (PCWSTR)Source );
#pragma warning(pop)
    } else {
        return uaw_wcscpy( Destination, Source );
    }
}

__inline
PUWSTR
static
ua_wcscpy_s(
    IN PUWSTR  Destination,
    IN size_t  DestinationSize,
    IN PCUWSTR Source
    )
{
    if (WSTR_ALIGNED(Source) && WSTR_ALIGNED(Destination)) {
        return (wcscpy_s( (PWSTR)Destination, DestinationSize, (PCWSTR)Source ) == 0 ? Destination : NULL);
    } else {
        /* TODO (alecont): Need to reference uaw_wcscpy_s */
        return uaw_wcscpy((PCUWSTR)String, Character);
    }
}

__inline
PUWSTR
static
ua_wcsrchr(
    IN PUWSTR String,
    IN WCHAR  Character
    )
{
    if (WSTR_ALIGNED(String)) {
        return wcsrchr(String, Character);
    } else {
        return uaw_wcsrchr((PCUWSTR)String, Character);
    }
}

#endif  // __cplusplus && _WConst_Return

__inline
PUWSTR
static
_WINDOWS_INSECURE_DEPRECATE
ua_wcscpy(
    IN PUWSTR  Destination,
    IN PCUWSTR Source
    )
{
    if (WSTR_ALIGNED(Source) && WSTR_ALIGNED(Destination)) {
#pragma warning(push)
#pragma warning(disable:4996)
        return wcscpy( (PWSTR)Destination, (PCWSTR)Source );
#pragma warning(pop)	
    } else {
        return uaw_wcscpy( Destination, Source );
    }
}


#if _STRALIGN_USE_SECURE_CRT
__inline
PUWSTR
static
ua_wcscpy_s(
    IN PUWSTR  Destination,
    IN size_t  DestinationSize,
    IN PCUWSTR Source
    )
{
    if (WSTR_ALIGNED(Source) && WSTR_ALIGNED(Destination)) {
        return (wcscpy_s( (PWSTR)Destination, DestinationSize, (PCWSTR)Source ) == 0 ? Destination : NULL);
    } else {
        /* TODO: Need to reference uaw_wcscpy_s */
        return uaw_wcscpy( Destination, Source );
    }
}
#endif

__inline
size_t
static
ua_wcslen(
    IN PCUWSTR String
    )
{
    if (WSTR_ALIGNED(String)) {
        return wcslen( (PCWSTR)String );
    } else {
        return uaw_wcslen( String );
    }
}

#endif  // _WSTRING_DEFINED

__inline
int
static
ua_wcsicmp(
    IN LPCUWSTR String1,
    IN LPCUWSTR String2
    )
{
    if (WSTR_ALIGNED(String1) && WSTR_ALIGNED(String2)) {
        return _wcsicmp( (LPCWSTR)String1, (LPCWSTR)String2 );
    } else {
        return uaw_wcsicmp( String1, String2 );
    }
}

#endif  // _X86_

//++
//
// VOID
// WSTR_ALIGNED_STACK_COPY (
//    OUT PCWSTR *TargetString,
//    IN  PCUWSTR SourceString OPTIONAL
//    )
//
// VOID
// TSTR_ALIGNED_STACK_COPY (
//    OUT PCTSTR *TargetString,
//    IN  PCUTSTR SourceString OPTIONAL
//    )
//
// Routine Description:
//
//    These macros set TargetString to an aligned pointer to the string
//    represented by SourceString.  If necessary, an aligned copy of
//    SourceString is copied onto the stack.
//
// Arguments:
//
//    TargetString - Supplies a pointer to a pointer to the resultant
//                   string.  This may be the same as SourceString if
//                   that argument is aligned.
//
//    SourceString - Supplies a pointer to the possibly unaligned UNICODE
//                   string.
//
// Return Value:
//
//    None.
//
// Note:
//
//    These macros may allocate memory on the stack via the CRT function
//    _alloca().  This memory is "freed" when the calling function exits.
//    As a result, do not use these macros inside of a loop that may execute
//    a large number of times - instead, use a wrapper function, or use
//    an explicit buffer like this:
//
//    TCHAR AlignedStringBuffer[ MAX_FOOSTR_CHARS ];
//    PTSTR AlignedString;
//
//    while (a < b) {
//        ...
//        if (TSTR_ALIGNED(s) {
//            AlignedString = s;
//        } else {
//            AlignedString = (PTSTR)ua_tcscpy(AlignedStringBuffer,s);
//        }
//        SomeSystemFunction(AlignedString);
//        ...
//    }
//        
//        
//--

//
// __UA_WSTRSIZE returns the number of bytes required to store the
// supplied null-terminated UNICODE string.
//        
// __UA_LOCALCOPY accepts a pointer to unaligned data and a size.  It
// allocates an aligned buffer on the stack and copies the data into
// it, returning a pointer to the buffer.
//

#if !defined(__UA_WCSLEN)
#define __UA_WCSLEN ua_wcslen
#endif

#define __UA_WSTRSIZE(s)    ((__UA_WCSLEN(s)+1)*sizeof(WCHAR))
#define __UA_STACKCOPY(p,s) memcpy(_alloca(s),p,s)

//
// Note that NULL is aligned.
//

#if defined(_X86_)

#define WSTR_ALIGNED_STACK_COPY(d,s) (*(d) = (PCWSTR)(s))

#else

//
// Use of an inline function here is not possible, as the results of
// the _alloca() will not be preserved upon return from the function.
//

#define WSTR_ALIGNED_STACK_COPY(d,s)                                \
    {                                                               \
        PCUWSTR __ua_src;                                           \
        ULONG   __ua_size;                                          \
        PWSTR  __ua_dst;                                            \
                                                                    \
        __ua_src = (s);                                             \
        if (WSTR_ALIGNED(__ua_src)) {                               \
            __ua_dst = (PWSTR)__ua_src;                             \
        } else {                                                    \
            __ua_size = __UA_WSTRSIZE(__ua_src);                    \
            __ua_dst = (PWSTR)_alloca(__ua_size);                   \
            memcpy(__ua_dst,__ua_src,__ua_size);                    \
        }                                                           \
        *(d) = (PCWSTR)__ua_dst;                                    \
    }

#endif

#define ASTR_ALIGNED_STACK_COPY(d,s) (*(d) = (PCSTR)(s))

//++
//
// <type> CONST *
// STRUC_ALIGNED_STACK_COPY (
//     IN <type name>,
//     IN PVOID Struc OPTIONAL
//     )
// 
// Routine Description:
//
//    This macro returns an aligned pointer to Struc, creating a local
//    copy on the stack if necessary.
//
//    This should be used only for relatively small structures, and efforts
//    should be made to align the structure properly in the first place.  Use
//    this macro only as a last resort.
//
// Arguments:
//
//    <type> - The type specifier of Struc
//
//    Struc - Supplies a pointer to the structure in question.
//
// Return Value:
//
//    Returns a const pointer to Struc if it is properly aligned, or a pointer 
//    to a stack-allocated copy of Struc if it is not.
//
//--

#if !defined(_X86_)

#define __UA_STRUC_ALIGNED(t,s) \
    (((DWORD_PTR)(s) & (TYPE_ALIGNMENT(t)-1)) == 0)

#define STRUC_ALIGNED_STACK_COPY(t,s) \
    __UA_STRUC_ALIGNED(t,s) ?   \
        ((t const *)(s)) :      \
        ((t const *)__UA_STACKCOPY((s),sizeof(t)))

#else

#define STRUC_ALIGNED_STACK_COPY(t,s) ((CONST t *)(s))

#endif

#if defined(UNICODE)

#define TSTR_ALIGNED_STACK_COPY(d,s)    WSTR_ALIGNED_STACK_COPY(d,s)
#define TSTR_ALIGNED(x)                 WSTR_ALIGNED(x)
#define ua_CharUpper                    ua_CharUpperW
#define ua_lstrcmp                      ua_lstrcmpW
#define ua_lstrcmpi                     ua_lstrcmpiW
#define ua_lstrlen                      ua_lstrlenW
#define ua_tcscpy                       ua_wcscpy
#if _STRALIGN_USE_SECURE_CRT
#define ua_tcscpy_s                     ua_wcscpy_s
#endif

#else

#define TSTR_ALIGNED_STACK_COPY(d,s)    ASTR_ALIGNED_STACK_COPY(d,s)
#define TSTR_ALIGNED(x)                 TRUE
#define ua_CharUpper                    CharUpperA
#define ua_lstrcmp                      lstrcmpA
#define ua_lstrcmpi                     lstrcmpiA
#define ua_lstrlen                      lstrlenA
#define ua_tcscpy                       strcpy
#if _STRALIGN_USE_SECURE_CRT
#define ua_tcscpy_s                     strcpy_s
#endif

#endif  // UNICODE

#ifdef __cplusplus
}
#endif


#endif  // __STRALIGN_H_

    

                                         



                                                      

