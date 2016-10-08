/******************************************************************
*                                                                 *
*  intsafe.h -- This module defines helper functions to prevent   *
*               integer overflow bugs.                            *
*                                                                 *
*  Copyright (c) Microsoft Corp.  All rights reserved.            *
*                                                                 *
******************************************************************/
#ifndef _INTSAFE_H_INCLUDED_
#define _INTSAFE_H_INCLUDED_

#if (_MSC_VER > 1000)
#pragma once
#endif

#include <specstrings.h>    // for __in, etc.

// compiletime asserts (failure results in error C2118: negative subscript)
#ifndef C_ASSERT
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif

#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && (_MSC_VER >= 1300)
#define _W64 __w64
#else
#define _W64
#endif
#endif

//
// typedefs
//
typedef          char       CHAR;
typedef unsigned char       UCHAR;
typedef unsigned char       BYTE;
typedef          short      SHORT;
typedef unsigned short      USHORT;
typedef unsigned short      WORD;
typedef          int        INT;
typedef unsigned int        UINT;
typedef          long       LONG;
typedef unsigned long       ULONG;
typedef unsigned long       DWORD;
typedef          __int64    INT64;
typedef unsigned __int64    ULONGLONG;

#if (__midl > 501)
typedef [public]          __int3264 INT_PTR;
typedef [public] unsigned __int3264 UINT_PTR;
typedef [public]          __int3264 LONG_PTR;
typedef [public] unsigned __int3264 ULONG_PTR;
#else
#ifdef _WIN64
typedef          __int64    INT_PTR;
typedef unsigned __int64    UINT_PTR;
typedef          __int64    LONG_PTR;
typedef unsigned __int64    ULONG_PTR;
#else
typedef _W64 int            INT_PTR;
typedef _W64 unsigned int   UINT_PTR;
typedef _W64 long           LONG_PTR;
typedef _W64 unsigned long  ULONG_PTR;
#endif // WIN64
#endif // (__midl > 501)

#ifdef _WIN64
typedef          __int64    ptrdiff_t;
typedef unsigned __int64    size_t;
#else
typedef _W64 int            ptrdiff_t;
typedef _W64 unsigned int   size_t;
#endif

typedef ULONG_PTR   DWORD_PTR;
typedef LONG_PTR    SSIZE_T;
typedef ULONG_PTR   SIZE_T;



#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef long HRESULT;
#endif // !_HRESULT_DEFINED

#ifndef SUCCEEDED
#define SUCCEEDED(hr)  ((HRESULT)(hr) >= 0)
#endif

#ifndef FAILED
#define FAILED(hr)  ((HRESULT)(hr) < 0)
#endif

#ifndef S_OK
#define S_OK  ((HRESULT)0x00000000L)
#endif

#define INTSAFE_E_ARITHMETIC_OVERFLOW   ((HRESULT)0x80070216L)  // 0x216 = 534 = ERROR_ARITHMETIC_OVERFLOW

#ifndef LOWORD
#define LOWORD(_dw)     ((WORD)((DWORD_PTR)(_dw) & 0xffff))
#endif
#ifndef HIWORD
#define HIWORD(_dw)     ((WORD)(((DWORD_PTR)(_dw)) >> 16))
#endif
#ifndef LODWORD
#define LODWORD(_qw)    ((DWORD)((unsigned __int64)(_qw)))
#endif
#ifndef HIDWORD
#define HIDWORD(_qw)    ((DWORD)(((unsigned __int64)(_qw)) >> 32))
#endif

//
// AMD64 intrinsic UnsignedMultiply128 definition
//
#if defined(_AMD64_)
#ifdef __cplusplus
extern "C" {
#endif
#define UnsignedMultiply128 _umul128
ULONG64
UnsignedMultiply128 (
    __in ULONG64  Multiplier,
    __in ULONG64  Multiplicand,
    __out ULONG64 *HighProduct
    );
#pragma intrinsic(_umul128)
#ifdef __cplusplus
}
#endif
#endif // _AMD64_

//
// UInt32x32To64 macro
//
#ifndef UInt32x32To64
#if defined(MIDL_PASS) || defined(RC_INVOKED) || defined(_M_CEE_PURE) || defined(_68K_) || defined(_MPPC_) || defined(_M_IA64) || defined(_M_AMD64)
#define UInt32x32To64(a, b) (((unsigned __int64)((unsigned int)(a))) * ((unsigned __int64)((unsigned int)(b))))
#elif defined(_M_IX86)
#define UInt32x32To64(a, b) ((unsigned __int64)(((unsigned __int64)((unsigned int)(a))) * ((unsigned int)(b))))
#else
#error Must define a target architecture.
#endif
#endif // !UInt32x32To64


//
// Min/Max type values
//
#define INTSAFE_SHORT_MIN       (-32768)
#define INTSAFE_INT_MIN         (-2147483647 - 1)
#define INTSAFE_LONG_MIN        (-2147483647L - 1)
#define INTSAFE_INT64_MIN       (-9223372036854775807i64 - 1)

#ifdef _WIN64
#define INTSAFE_INT_PTR_MIN     INTSAFE_INT64_MIN
#define INTSAFE_LONG_PTR_MIN    INTSAFE_INT64_MIN
#define INTSAFE_ptrdiff_t_MIN   INTSAFE_INT64_MIN
#define INTSAFE_SSIZE_T_MIN     INTSAFE_INT64_MIN
#else
#define INTSAFE_INT_PTR_MIN     INTSAFE_INT_MIN
#define INTSAFE_LONG_PTR_MIN    INTSAFE_LONG_MIN
#define INTSAFE_ptrdiff_t_MIN   INTSAFE_INT_MIN
#define INTSAFE_SSIZE_T_MIN     INTSAFE_LONG_MIN
#endif

#define INTSAFE_BYTE_MAX        0xff
#define INTSAFE_SHORT_MAX       32767
#define INTSAFE_USHORT_MAX      0xffff
#define INTSAFE_WORD_MAX        0xffff
#define INTSAFE_INT_MAX         2147483647
#define INTSAFE_UINT_MAX        0xffffffff
#define INTSAFE_LONG_MAX        2147483647L
#define INTSAFE_ULONG_MAX       0xffffffffUL
#define INTSAFE_DWORD_MAX       0xffffffffUL
#define INTSAFE_INT64_MAX       9223372036854775807i64
#define INTSAFE_ULONGLONG_MAX   0xffffffffffffffffui64

#ifdef _WIN64
#define INTSAFE_INT_PTR_MAX     INTSAFE_INT64_MAX
#define INTSAFE_UINT_PTR_MAX    INTSAFE_ULONGLONG_MAX
#define INTSAFE_LONG_PTR_MAX    INTSAFE_INT64_MAX
#define INTSAFE_ULONG_PTR_MAX   INTSAFE_ULONGLONG_MAX
#define INTSAFE_DWORD_PTR_MAX   INTSAFE_ULONGLONG_MAX
#define INTSAFE_ptrdiff_t_MAX   INTSAFE_INT64_MAX
#define INTSAFE_size_t_MAX      INTSAFE_ULONGLONG_MAX
#define INTSAFE_SSIZE_T_MAX     INTSAFE_INT64_MAX
#define INTSAFE_SIZE_T_MAX      INTSAFE_ULONGLONG_MAX
#else
#define INTSAFE_INT_PTR_MAX     INTSAFE_INT_MAX 
#define INTSAFE_UINT_PTR_MAX    INTSAFE_UINT_MAX
#define INTSAFE_LONG_PTR_MAX    INTSAFE_LONG_MAX
#define INTSAFE_ULONG_PTR_MAX   INTSAFE_ULONG_MAX
#define INTSAFE_DWORD_PTR_MAX   INTSAFE_DWORD_MAX
#define INTSAFE_ptrdiff_t_MAX   INTSAFE_INT_MAX
#define INTSAFE_size_t_MAX      INTSAFE_UINT_MAX
#define INTSAFE_SSIZE_T_MAX     INTSAFE_LONG_MAX
#define INTSAFE_SIZE_T_MAX      INTSAFE_ULONG_MAX
#endif


//
// It is common for -1 to be used as an error value
//
#define BYTE_ERROR      0xff
#define SHORT_ERROR     (-1)
#define USHORT_ERROR    0xffff
#define WORD_ERROR      USHORT_ERROR
#define INT_ERROR       (-1)
#define UINT_ERROR      0xffffffff
#define LONG_ERROR      (-1L)
#define ULONG_ERROR     0xffffffffUL
#define DWORD_ERROR     ULONG_ERROR
#define INT64_ERROR     (-1i64)
#define ULONGLONG_ERROR (0xffffffffffffffffui64)

#ifdef _WIN64
#define INT_PTR_ERROR   INT64_ERROR
#define UINT_PTR_ERROR  ULONGLONG_ERROR
#define LONG_PTR_ERROR  INT64_ERROR
#define ULONG_PTR_ERROR ULONGLONG_ERROR
#define DWORD_PTR_ERROR ULONGLONG_ERROR
#define ptrdiff_t_ERROR INT64_ERROR
#define size_t_ERROR    ULONGLONG_ERROR
#define SSIZE_T_ERROR   INT64_ERROR
#define SIZE_T_ERROR    ULONGLONG_ERROR
#else
#define INT_PTR_ERROR   INT_ERROR 
#define UINT_PTR_ERROR  UINT_ERROR
#define LONG_PTR_ERROR  LONG_ERROR
#define ULONG_PTR_ERROR ULONG_ERROR
#define DWORD_PTR_ERROR DWORD_ERROR
#define ptrdiff_t_ERROR INT_ERROR
#define size_t_ERROR    UINT_ERROR
#define SSIZE_T_ERROR   LONG_ERROR
#define SIZE_T_ERROR    ULONG_ERROR
#endif


//
// We make some assumptions about the sizes of various types. Let's be
// explicit about those assumptions and check them.
//
C_ASSERT(sizeof(USHORT) == 2);
C_ASSERT(sizeof(INT) == 4);
C_ASSERT(sizeof(UINT) == 4);
C_ASSERT(sizeof(LONG) == 4);
C_ASSERT(sizeof(ULONG) == 4);
C_ASSERT(sizeof(UINT_PTR) == sizeof(ULONG_PTR));


// ============================================================================
// Conversion functions
//
// There are three reasons for having conversion functions:
//
// 1. We are converting from a signed type to an unsigned type of the same
//    size, or vice-versa
//
//    Since we only have unsigned math functions, this allows people to convert
//    to unsigned, do the math, and then convert back to signed
//
// 2. We are converting to a smaller type, and we could therefore possibly
//    overflow.
//
//    However, it makes no sense to have functions that convert from a
//    ULONGLONG -> BYTE (too big of a transition). If you want this then write
//    your own wrapper function that calls ULongLongToLong() and ULongToByte().
//
// 3. We are converting to a bigger type, and we are signed and the type we are
//    converting to is unsigned.
//
//=============================================================================


//
// SHORT -> UCHAR conversion
//
__inline
__checkReturn
HRESULT
ShortToUChar(
    __in SHORT sOperand,
    __out UCHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if ((sOperand >= 0) && (sOperand <= 255))
    {
        *pch = (UCHAR)sOperand;
        hr = S_OK;
    }

    return hr;
}

//
// SHORT -> CHAR conversion
//
#ifdef _CHAR_UNSIGNED
__forceinline
__checkReturn
HRESULT
ShortToChar(
    __in SHORT sOperand,
    __out CHAR* pch)
{
    return ShortToUChar(sOperand, (UCHAR*)pch);
}
#else
__inline
__checkReturn
HRESULT
ShortToChar(
    __in SHORT sOperand,
    __out CHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if ((sOperand >= -128) && (sOperand <= 127))
    {
        *pch = (CHAR)sOperand;
        hr = S_OK;
    }

    return hr;
}
#endif // _CHAR_UNSIGNED

//
// SHORT -> BYTE conversion
//
#define ShortToByte  ShortToUChar

//
// SHORT -> USHORT conversion
//
__inline
__checkReturn
HRESULT
ShortToUShort(
    __in SHORT sOperand,
    __out USHORT* pusResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pusResult = USHORT_ERROR;

    if (sOperand >= 0)
    {
        *pusResult = (USHORT)sOperand;
        hr = S_OK;
    }

    return hr;
}

//
// SHORT -> WORD conversion
//
#define ShortToWord ShortToUShort

//
// USHORT -> UCHAR conversion
//
__inline
__checkReturn
HRESULT
UShortToUChar(
    __in USHORT usOperand,
    __out UCHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if (usOperand <= 255)
    {
        *pch = (UCHAR)usOperand;
        hr = S_OK;
    }

    return hr;
}

//
// USHORT -> CHAR conversion
//
#ifdef _CHAR_UNSIGNED
__forceinline
__checkReturn
HRESULT
UShortToChar(
    __in USHORT usOperand,
    __out CHAR* pch)
{
    return UShortToUChar(usOperand, (UCHAR*)pch);
}
#else
__inline
__checkReturn
HRESULT
UShortToChar(
    __in USHORT usOperand,
    __out CHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if (usOperand <= 127)
    {
        *pch = (CHAR)usOperand;
        hr = S_OK;
    }

    return hr;
}
#endif // _CHAR_UNSIGNED

//
// USHORT -> BYTE conversion
//
#define UShortToByte    UShortToUChar

//
// USHORT -> SHORT conversion
//
__inline
__checkReturn
HRESULT
UShortToShort(
    __in USHORT usOperand,
    __out SHORT* psResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *psResult = SHORT_ERROR;

    if (usOperand <= INTSAFE_SHORT_MAX)
    {
        *psResult = (SHORT)usOperand;
        hr = S_OK;
    }

    return hr;
}

//
// WORD -> CHAR conversion
//
#define WordToChar  UShortToChar

//
// WORD -> UCHAR conversion
//
#define WordToUChar UShortToUChar

//
// WORD -> BYTE conversion
//
#define WordToByte  UShortToByte

//
// WORD -> SHORT conversion
//
#define WordToShort UShortToShort

//
// INT -> UCHAR conversion
//
__inline
__checkReturn
HRESULT
IntToUChar(
    __in INT iOperand,
    __out UCHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if ((iOperand >= 0) && (iOperand <= 255))
    {
        *pch = (UCHAR)iOperand;
        hr = S_OK;
    }

    return hr;
}

//
// INT -> CHAR conversion
//
#ifdef _CHAR_UNSIGNED
__forceinline
__checkReturn
HRESULT
IntToChar(
    __in INT iOperand,
    __out CHAR* pch)
{
    return IntToUChar(iOperand, (UCHAR*)pch);
}
#else
__inline
__checkReturn
HRESULT
IntToChar(
    __in INT iOperand,
    __out CHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if ((iOperand >= -128) && (iOperand <= 127))
    {
        *pch = (CHAR)iOperand;
        hr = S_OK;
    }

    return hr;
}
#endif // _CHAR_UNSIGNED

//
// INT -> BYTE conversion
//
#define IntToByte   IntToUChar

//
// INT -> SHORT conversion
//
__inline
__checkReturn
HRESULT
IntToShort(
    __in INT iOperand,
    __out SHORT* psResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *psResult = SHORT_ERROR;

    if ((iOperand >= INTSAFE_SHORT_MIN) && (iOperand <= INTSAFE_SHORT_MAX))
    {
        *psResult = (SHORT)iOperand;
        hr = S_OK;
    }

    return hr;
}

//
// INT -> USHORT conversion
//
__inline
__checkReturn
HRESULT
IntToUShort(
    __in INT iOperand,
    __out USHORT* pusResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pusResult = USHORT_ERROR;

    if ((iOperand >= 0) && (iOperand <= INTSAFE_USHORT_MAX))
    {
        *pusResult = (USHORT)iOperand;
        hr = S_OK;
    }

    return hr;
}

//
// INT -> WORD conversion
//
#define IntToWord   IntToUShort

//
// INT -> UINT conversion
//
__inline
__checkReturn
HRESULT
IntToUInt(
    __in INT iOperand,
    __out UINT* puResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *puResult = UINT_ERROR;

    if (iOperand >= 0)
    {
        *puResult = (UINT)iOperand;
        hr = S_OK;
    }

    return hr;
}

//
// INT -> UINT_PTR conversion
//
#ifdef _WIN64
#define IntToUIntPtr    IntToULongLong
#else
#define IntToUIntPtr    IntToUInt
#endif

//
// INT -> ULONG conversion
//
__inline
__checkReturn
HRESULT
IntToULong(
    __in INT iOperand,
    __out ULONG* pulResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pulResult = ULONG_ERROR;

    if (iOperand >= 0)
    {
        *pulResult = (ULONG)iOperand;
        hr = S_OK;
    }

    return hr;
}

//
// INT -> ULONG_PTR conversion
//
#ifdef _WIN64
#define IntToULongPtr   IntToULongLong
#else
#define IntToULongPtr   IntToULong
#endif

//
// INT -> DWORD conversion
//
#define IntToDWord  IntToULong

//
// INT -> DWORD_PTR conversion
//
#define IntToDWordPtr   IntToULongPtr

//
// INT -> ULONGLONG conversion
//
__inline
__checkReturn
HRESULT
IntToULongLong(
    __in INT iOperand,
    __out ULONGLONG* pullResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pullResult = ULONGLONG_ERROR;

    if (iOperand >= 0)
    {
        *pullResult = (ULONGLONG)iOperand;
        hr = S_OK;
    }

    return hr;
}

//
// INT -> size_t conversion
//
#define IntToSizeT  IntToUIntPtr

//
// INT -> SIZE_T conversion
//
#define IntToSIZET  IntToULongPtr

//
// INT_PTR -> INT conversion
//
#ifdef _WIN64
#define IntPtrToInt Int64ToInt
#else
__inline
__checkReturn
HRESULT
IntPtrToInt(
    __in INT_PTR iOperand,
    __out INT* piResult)
{
    *piResult = (INT)iOperand;
    return S_OK;
}
#endif

//
// INT_PTR -> UINT conversion
//
#ifdef _WIN64
#define IntPtrToUInt    Int64ToUInt
#else
#define IntPtrToUInt    IntToUInt
#endif

//
// INT_PTR -> UINT_PTR conversion
//
#ifdef _WIN64
#define IntPtrToUIntPtr Int64ToULongLong
#else
#define IntPtrToUIntPtr IntToUInt
#endif

//
// INT_PTR -> LONG conversion
//
#ifdef _WIN64
#define IntPtrToLong    Int64ToLong
#else
__inline
__checkReturn
HRESULT
IntPtrToLong(
    __in INT_PTR iOperand,
    __out LONG* plResult)
{
    *plResult = (LONG)iOperand;
    return S_OK;
}    
#endif

//
// INT_PTR -> ULONG conversion
//
#ifdef _WIN64
#define IntPtrToULong   Int64ToULong
#else
#define IntPtrToULong   IntToULong
#endif

//
// INT_PTR -> ULONG_PTR conversion
//
#ifdef _WIN64
#define IntPtrToULongPtr    Int64ToULongLong
#else
#define IntPtrToULongPtr    IntToULong
#endif

//
// INT_PTR -> DWORD conversion
//
#define IntPtrToDWord   IntPtrToULong

//    
// INT_PTR -> DWORD_PTR conversion
//
#define IntPtrToDWordPtr    IntPtrToULongPtr

//
// INT_PTR -> ULONGLONG conversion
//
#ifdef _WIN64
#define IntPtrToULongLong   Int64ToULongLong
#else
#define IntPtrToULongLong   IntToULongLong
#endif

//
// INT_PTR -> size_t conversion
//
#define IntPtrToSizeT   IntPtrToUIntPtr

//
// INT_PTR -> SIZE_T conversion
//
#define IntPtrToSIZET   IntPtrToULongPtr

//
// UINT -> UCHAR conversion
//
__inline
__checkReturn
HRESULT
UIntToUChar(
    __in UINT uOperand,
    __out UCHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if (uOperand <= 255)
    {
        *pch = (UCHAR)uOperand;
        hr = S_OK;
    }

    return hr;
}

//
// UINT -> CHAR conversion
//
#ifdef _CHAR_UNSIGNED
__forceinline
__checkReturn
HRESULT
UIntToChar(
    __in UINT uOperand,
    __out CHAR* pch)
{
    return UIntToUChar(uOperand, (UCHAR*)pch);
}
#else
__inline
__checkReturn
HRESULT
UIntToChar(
    __in UINT uOperand,
    __out CHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if (uOperand <= 127)
    {
        *pch = (CHAR)uOperand;
        hr = S_OK;
    }

    return hr;
}
#endif // _CHAR_UNSIGNED

//
// UINT -> BYTE conversion
//
#define UIntToByte   UIntToUChar

//
// UINT -> SHORT conversion
//
__inline
__checkReturn
HRESULT
UIntToShort(
    __in UINT uOperand,
    __out SHORT* psResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *psResult = SHORT_ERROR;

    if (uOperand <= INTSAFE_SHORT_MAX)
    {
        *psResult = (SHORT)uOperand;
        hr = S_OK;
    }

    return hr;
}

//
// UINT -> USHORT conversion
//
__inline
__checkReturn
HRESULT
UIntToUShort(
    __in UINT uOperand,
    __out USHORT* pusResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pusResult = USHORT_ERROR;

    if (uOperand <= INTSAFE_USHORT_MAX)
    {
        *pusResult = (USHORT)uOperand;
        hr = S_OK;
    }

    return hr;
}

//
// UINT -> WORD conversion
//
#define UIntToWord  UIntToUShort

//
// UINT -> INT conversion
//
__inline
__checkReturn
HRESULT
UIntToInt(
    __in UINT uOperand,
    __out INT* piResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *piResult = INT_ERROR;

    if (uOperand <= INTSAFE_INT_MAX)
    {
        *piResult = (INT)uOperand;
        hr = S_OK;
    }

    return hr;
}

//
// UINT -> INT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
UIntToIntPtr(
    __in UINT uOperand,
    __out INT_PTR* piResult)
{
    *piResult = (INT_PTR)uOperand;
    return S_OK;
}
#else
#define UIntToIntPtr    UIntToInt
#endif

//
// UINT -> LONG conversion
//
__inline
__checkReturn
HRESULT
UIntToLong(
    __in UINT uOperand,
    __out LONG* plResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *plResult = LONG_ERROR;

    if (uOperand <= INTSAFE_LONG_MAX)
    {
        *plResult = (LONG)uOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// UINT -> LONG_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
UIntToLongPtr(
    __in UINT uOperand,
    __out LONG_PTR* plResult)
{
    *plResult = (LONG_PTR)uOperand;
    return S_OK;
}
#else
#define UIntToLongPtr   UIntToLong
#endif

//
// UINT -> ptrdiff_t conversion
//
#define UIntToPtrdiffT  UIntToIntPtr

//
// UINT -> SSIZE_T conversion
//
#define UIntToSSIZET    UIntToLongPtr

//
// UINT_PTR -> INT conversion
//
#ifdef _WIN64
#define UIntPtrToInt    ULongLongToInt
#else
#define UIntPtrToInt    UIntToInt
#endif

//
// UINT_PTR -> INT_PTR conversion
//
#ifdef _WIN64
#define UIntPtrToIntPtr ULongLongToInt64
#else
#define UIntPtrToIntPtr UIntToInt
#endif

//
// UINT_PTR -> UINT conversion
//
#ifdef _WIN64
#define UIntPtrToUInt   ULongLongToUInt
#else
__inline
__checkReturn
HRESULT
UIntPtrToUInt(
    __in UINT_PTR uOperand,
    __out UINT* puResult)
{
    *puResult = (UINT)uOperand;
    return S_OK;
}
#endif

//
// UINT_PTR -> LONG conversion
//
#ifdef _WIN64
#define UIntPtrToLong   ULongLongToLong
#else
#define UIntPtrToLong   UIntToLong
#endif

//
// UINT_PTR -> LONG_PTR conversion
//
#ifdef _WIN64
#define UIntPtrToLongPtr    ULongLongToInt64
#else
#define UIntPtrToLongPtr    UIntToLong
#endif

//
// UINT_PTR -> ULONG conversion
//
#ifdef _WIN64
#define UIntPtrToULong  ULongLongToULong
#else
__inline
__checkReturn
HRESULT
UIntPtrToULong(
    __in UINT_PTR uOperand,
    __out ULONG* pulResult)
{
    *pulResult = (ULONG)uOperand;
    return S_OK;
}
#endif

//
// UINT_PTR -> DWORD conversion
//
#define UIntPtrToDWord  UIntPtrToULong

//
// UINT_PTR -> INT64 conversion
//
__inline
__checkReturn
HRESULT
UIntPtrToInt64(
    __in UINT_PTR uOperand,
    __out INT64* pi64Result)
{
#ifdef _WIN64
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pi64Result = INT64_ERROR;
    
    if (uOperand <= INTSAFE_INT64_MAX)
    {
        *pi64Result = (INT64)uOperand;
        hr = S_OK;
    }
    
    return hr;
#else
    *pi64Result = (INT64)uOperand;
    return S_OK;
#endif
}

//
// UINT_PTR -> ptrdiff_t conversion
//
#define UIntPtrToPtrdiffT   UIntPtrToIntPtr

//
// UINT_PTR -> SSIZE_T conversion
//
#define UIntPtrToSSIZET UIntPtrToLongPtr

//
// LONG -> UCHAR conversion
//
__inline
__checkReturn
HRESULT
LongToUChar(
    __in LONG lOperand,
    __out UCHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if ((lOperand >= 0) && (lOperand <= 255))
    {
        *pch = (UCHAR)lOperand;
        hr = S_OK;
    }

    return hr;
}

//
// LONG -> CHAR conversion
//
#ifdef _CHAR_UNSIGNED
__forceinline
__checkReturn
HRESULT
LongToChar(
    __in LONG lOperand,
    __out CHAR* pch)
{
    return LongToUChar(lOperand, (UCHAR*)pch);
}
#else
__inline
__checkReturn
HRESULT
LongToChar(
    __in LONG lOperand,
    __out CHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if ((lOperand >= -128) && (lOperand <= 127))
    {
        *pch = (CHAR)lOperand;
        hr = S_OK;
    }

    return hr;
}
#endif // _CHAR_UNSIGNED

//
// LONG -> BYTE conversion
//
#define LongToByte  LongToUChar

//
// LONG -> SHORT conversion
//
__inline
__checkReturn
HRESULT
LongToShort(
    __in LONG lOperand,
    __out SHORT* psResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *psResult = SHORT_ERROR;
     
    if ((lOperand >= INTSAFE_SHORT_MIN) && (lOperand <= INTSAFE_SHORT_MAX))
    {
       *psResult = (SHORT)lOperand;
       hr = S_OK;
    }
     
    return hr;
}

//
// LONG -> USHORT conversion
//
__inline
__checkReturn
HRESULT
LongToUShort(
    __in LONG lOperand,
    __out USHORT* pusResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pusResult = USHORT_ERROR;
    
    if ((lOperand >= 0) && (lOperand <= INTSAFE_USHORT_MAX))
    {
        *pusResult = (USHORT)lOperand;
        hr = S_OK;
    }
    
    return hr;
}

//   
// LONG -> WORD conversion
//
#define LongToWord  LongToUShort

//
// LONG -> INT conversion
//
__inline
__checkReturn
HRESULT
LongToInt(
    __in LONG lOperand,
    __out INT* piResult)
{
    // we have C_ASSERT's above that ensures that this assignment is ok
    *piResult = (INT)lOperand;
    return S_OK;
}

//
// LONG -> INT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
LongToIntPtr(
    __in LONG lOperand,
    __out INT_PTR* piResult)
{
    *piResult = (INT_PTR)lOperand;
    return S_OK;
}
#else
#define LongToIntPtr    LongToInt
#endif

//
// LONG -> UINT conversion
//
__inline
__checkReturn
HRESULT
LongToUInt(
    __in LONG lOperand,
    __out UINT* puResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *puResult = UINT_ERROR;
    
    if (lOperand >= 0)
    {
        *puResult = (UINT)lOperand;
        hr = S_OK;
    }
    
    return hr;
}   

//
// LONG -> UINT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
LongToUIntPtr(
    __in LONG lOperand,
    __out UINT_PTR* puResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *puResult = UINT_PTR_ERROR;
    
    if (lOperand >= 0)
    {
        *puResult = (UINT_PTR)lOperand;
        hr = S_OK;
    }
    
    return hr;
}
#else
#define LongToUIntPtr   LongToUInt
#endif

//
// LONG -> ULONG conversion
//
__inline
__checkReturn
HRESULT
LongToULong(
    __in LONG lOperand,
    __out ULONG* pulResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pulResult = ULONG_ERROR;
    
    if (lOperand >= 0)
    {
        *pulResult = (ULONG)lOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// LONG -> ULONG_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
LongToULongPtr(
    __in LONG lOperand,
    __out ULONG_PTR* pulResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pulResult = ULONG_PTR_ERROR;
    
    if (lOperand >= 0)
    {
        *pulResult = (ULONG_PTR)lOperand;
        hr = S_OK;
    }
    
    return hr;
}
#else
#define LongToULongPtr  LongToULong
#endif

//
// LONG -> DWORD conversion
//
#define LongToDWord LongToULong

//
// LONG -> DWORD_PTR conversion
//
#define LongToDWordPtr  LongToULongPtr

//
// LONG -> ULONGLONG conversion
//
__inline
__checkReturn
HRESULT
LongToULongLong(
    __in LONG lOperand,
    __out ULONGLONG* pullResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pullResult = ULONGLONG_ERROR;
    
    if (lOperand >= 0)
    {
        *pullResult = (ULONGLONG)lOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// LONG -> ptrdiff_t conversion
//
#define LongToPtrdiffT  LongToIntPtr

//
// LONG -> size_t conversion
//
#define LongToSizeT LongToUIntPtr

//
// LONG -> SIZE_T conversion
//
#define LongToSIZET LongToULongPtr

//
// LONG_PTR -> INT conversion
//
#ifdef _WIN64
#define LongPtrToInt    Int64ToInt
#else
#define LongPtrToInt    LongToInt
#endif

//
// LONG_PTR -> INT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
LongPtrToIntPtr(
    __in LONG_PTR lOperand,
    __out INT_PTR* piResult)
{
    *piResult = (INT_PTR)lOperand;
    return S_OK;
}
#else
#define LongPtrToIntPtr LongToInt
#endif

//
// LONG_PTR -> UINT conversion
//
#ifdef _WIN64
#define LongPtrToUInt   Int64ToUInt
#else
#define LongPtrToUInt   LongToUInt
#endif

//
// LONG_PTR -> UINT_PTR conversion
//
#ifdef _WIN64
#define LongPtrToUIntPtr    Int64ToULongLong
#else
#define LongPtrToUIntPtr    LongToUInt
#endif

//
// LONG_PTR -> LONG conversion
//
__inline
__checkReturn
HRESULT
LongPtrToLong(
    __in LONG_PTR lOperand,
    __out LONG* plResult)
{
#ifdef _WIN64
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *plResult = LONG_ERROR;
    
    if ((lOperand >= INTSAFE_LONG_MIN) && (lOperand <= INTSAFE_LONG_MAX))
    {
        *plResult = (LONG)lOperand;
        hr = S_OK;
    }
    
    return hr;
#else
    *plResult = (LONG)lOperand;
    return S_OK;
#endif
}

//    
// LONG_PTR -> ULONG conversion
//
#ifdef _WIN64
#define LongPtrToULong  Int64ToULong
#else
#define LongPtrToULong  LongToULong
#endif

//
// LONG_PTR -> ULONG_PTR conversion
//
#ifdef _WIN64
#define LongPtrToULongPtr   Int64ToULongLong
#else
#define LongPtrToULongPtr   LongToULong
#endif

//
// LONG_PTR -> DWORD conversion
//
#define LongPtrToDWord  LongPtrToULong

//
// LONG_PTR -> DWORD_PTR conversion
//
#define LongPtrToDWordPtr   LongPtrToULongPtr 

//
// LONG_PTR -> ULONGLONG conversion
//
#ifdef _WIN64
#define LongPtrToULongLong  Int64ToULongLong
#else
#define LongPtrToULongLong  LongToULongLong
#endif

//
// LONG_PTR -> size_t conversion
//
#define LongPtrToSizeT  LongPtrToUIntPtr

//
// LONG_PTR -> SIZE_T conversion
//
#define LongPtrToSIZET  LongPtrToULongPtr

//
// UINT -> UCHAR conversion
//
__inline
__checkReturn
HRESULT
ULongToUChar(
    __in ULONG ulOperand,
    __out UCHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if (ulOperand <= 255)
    {
        *pch = (UCHAR)ulOperand;
        hr = S_OK;
    }

    return hr;
}

//
// ULONG -> CHAR conversion
//
#ifdef _CHAR_UNSIGNED
__forceinline
__checkReturn
HRESULT
ULongToChar(
    __in ULONG ulOperand,
    __out CHAR* pch)
{
    return ULongToUChar(ulOperand, (UCHAR*)pch);
}
#else
__inline
__checkReturn
HRESULT
ULongToChar(
    __in ULONG ulOperand,
    __out CHAR* pch)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pch = '\0';

    if (ulOperand <= 127)
    {
        *pch = (CHAR)ulOperand;
        hr = S_OK;
    }

    return hr;
}
#endif // _CHAR_UNSIGNED

//
// ULONG -> BYTE conversion
//
#define ULongToByte ULongToUChar

//
// ULONG -> SHORT conversion
//
__inline
__checkReturn
HRESULT
ULongToShort(
    __in ULONG ulOperand,
    __out SHORT* psResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *psResult = SHORT_ERROR;

    if (ulOperand <= INTSAFE_SHORT_MAX)
    {
        *psResult = (SHORT)ulOperand;
        hr = S_OK;
    }

    return hr;
}

//
// ULONG -> USHORT conversion
//
__inline
__checkReturn
HRESULT
ULongToUShort(
    __in ULONG ulOperand,
    __out USHORT* pusResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pusResult = USHORT_ERROR;

    if (ulOperand <= INTSAFE_USHORT_MAX)
    {
        *pusResult = (USHORT)ulOperand;
        hr = S_OK;
    }

    return hr;
}

//
// ULONG -> WORD conversion
//
#define ULongToWord ULongToUShort

//
// ULONG -> INT conversion
//
__inline
__checkReturn
HRESULT
ULongToInt(
    __in ULONG ulOperand,
    __out INT* piResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *piResult = INT_ERROR;
    
    if (ulOperand <= INTSAFE_INT_MAX)
    {
        *piResult = (INT)ulOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONG -> INT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
ULongToIntPtr(
    __in ULONG ulOperand,
    __out INT_PTR* piResult)
{
    *piResult = (INT_PTR)ulOperand;
    return S_OK;
}
#else
#define ULongToIntPtr   ULongToInt
#endif

//
// ULONG -> UINT conversion
//
__inline
__checkReturn
HRESULT
ULongToUInt(
    __in ULONG ulOperand,
    __out UINT* puResult)
{
    // we have C_ASSERT's above that ensures that this assignment is ok    
    *puResult = (UINT)ulOperand;    
    return S_OK;
}

//
// ULONG -> UINT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
ULongToUIntPtr(
    __in LONG ulOperand,
    __out UINT_PTR* puiResult)
{
    *puiResult = (UINT_PTR)ulOperand;
    return S_OK;
}
#else
#define ULongToUIntPtr  ULongToUInt
#endif

//
// ULONG -> LONG conversion
//
__inline
__checkReturn
HRESULT
ULongToLong(
    __in ULONG ulOperand,
    __out LONG* plResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *plResult = LONG_ERROR;
    
    if (ulOperand <= INTSAFE_LONG_MAX)
    {
        *plResult = (LONG)ulOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONG -> LONG_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
ULongToLongPtr(
    __in ULONG ulOperand,
    __out LONG_PTR* plResult)
{
    *plResult = (LONG_PTR)ulOperand;
    return S_OK;
}
#else
#define ULongToLongPtr  ULongToLong
#endif

//
// ULONG -> ptrdiff_t conversion
//
#define ULongToPtrdiffT ULongToIntPtr

//
// ULONG -> SSIZE_T conversion
//
#define ULongToSSIZET   ULongToLongPtr

//
// ULONG_PTR -> INT conversion
//
#ifdef _WIN64
#define ULongPtrToInt   ULongLongToInt
#else
#define ULongPtrToInt   ULongToInt
#endif

//
// ULONG_PTR -> INT_PTR conversion
//
#ifdef _WIN64
#define ULongPtrToIntPtr    ULongLongToInt64
#else
#define ULongPtrToIntPtr    ULongToInt
#endif

//
// ULONG_PTR -> UINT conversion
//
#ifdef _WIN64
#define ULongPtrToUInt  ULongLongToUInt
#else
#define ULongPtrToUInt  ULongToUInt
#endif

//
// ULONG_PTR -> UINT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
ULongPtrToUIntPtr(
    __in ULONG_PTR ulOperand,
    __out UINT_PTR* puResult)
{
    *puResult = (UINT_PTR)ulOperand;
    return S_OK;
}
#else
#define ULongPtrToUIntPtr   ULongToUInt
#endif

//
// ULONG_PTR -> LONG conversion
//
#ifdef _WIN64
#define ULongPtrToLong  ULongLongToLong
#else
#define ULongPtrToLong  ULongToLong
#endif

//        
// ULONG_PTR -> LONG_PTR conversion
//
#ifdef _WIN64
#define ULongPtrToLongPtr   ULongLongToInt64
#else
#define ULongPtrToLongPtr   ULongToLong
#endif

//
// ULONG_PTR -> ULONG conversion
//
__inline
__checkReturn
HRESULT
ULongPtrToULong(
    __in ULONG_PTR ulOperand,
    __out ULONG* pulResult)
{
#ifdef _WIN64
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pulResult = ULONG_ERROR;

    if (ulOperand <= INTSAFE_ULONG_MAX)
    {
        *pulResult = (ULONG)ulOperand;
        hr = S_OK;
    }
    
    return hr;
#else
    *pulResult = (ULONG)ulOperand;
    return S_OK;
#endif    
}

//
// ULONG_PTR -> DWORD conversion
//
#define ULongPtrToDWord ULongPtrToULong

//
// ULONG_PTR -> INT64
//
__inline
__checkReturn
HRESULT
ULongPtrToInt64(
    __in ULONG_PTR ulOperand,
    __out INT64* pi64Result)
{
#ifdef _WIN64
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pi64Result = INT64_ERROR;
    
    if (ulOperand <= INTSAFE_INT64_MAX)
    {
        *pi64Result = (INT64)ulOperand;
        hr = S_OK;
    }
    
    return hr;
#else
    *pi64Result = (INT64)ulOperand;
    return S_OK;
#endif
}

//
// ULONG_PTR -> ptrdiff_t conversion
//
#define ULongPtrToPtrdiffT  ULongPtrToIntPtr

//
// ULONG_PTR -> SSIZE_T conversion
//
#define ULongPtrToSSIZET    ULongPtrToLongPtr

//
// DWORD -> CHAR conversion
//
#define DWordToChar ULongToChar

//
// DWORD -> UCHAR conversion
//
#define DWordToUChar    ULongToUChar

//
// DWORD -> BYTE conversion
//
#define DWordToByte ULongToUChar

//
// DWORD -> SHORT conversion
//
#define DWordToShort    ULongToShort

//
// DWORD -> USHORT conversion
//
#define DWordToUShort   ULongToUShort

//
// DWORD -> WORD conversion
//
#define DWordToWord ULongToUShort

//
// DWORD -> INT conversion
//
#define DWordToInt  ULongToInt

//
// DWORD -> INT_PTR conversion
//
#define DWordToIntPtr   ULongToIntPtr

//
// DWORD -> UINT conversion
//
#define DWordToUInt ULongToUInt

//
// DWORD -> UINT_PTR conversion
//
#define DWordToUIntPtr  ULongToUIntPtr

//
// DWORD -> LONG conversion
//
#define DWordToLong ULongToLong

//
// DWORD -> LONG_PTR conversion
//
#define DWordToLongPtr  ULongToLongPtr

//
// DWORD -> ptrdiff_t conversion
//
#define DWordToPtrdiffT ULongToIntPtr

//
// DWORD -> SSIZE_T conversion
//
#define DWordToSSIZET   ULongToLongPtr

//
// DWORD_PTR -> INT conversion
//
#define DWordPtrToInt   ULongPtrToInt

//
// DWORD_PTR -> INT_PTR conversion
//
#define DWordPtrToIntPtr    ULongPtrToIntPtr

//
// DWORD_PTR -> UINT conversion
//
#define DWordPtrToUInt  ULongPtrToUInt

//
// DWODR_PTR -> UINT_PTR conversion
//
#define DWordPtrToUIntPtr   ULongPtrToUIntPtr

//
// DWORD_PTR -> LONG conversion
//
#define DWordPtrToLong  ULongPtrToLong

//
// DWORD_PTR -> LONG_PTR conversion
//
#define DWordPtrToLongPtr   ULongPtrToLongPtr

//
// DWORD_PTR -> ULONG conversion
//
#define DWordPtrToULong ULongPtrToULong

//
// DWORD_PTR -> DWORD conversion
//
#define DWordPtrToDWord ULongPtrToULong

//
// DWORD_PTR -> INT64 conversion
//
#define DWordPtrToInt64 ULongPtrToInt64

//
// DWORD_PTR -> ptrdiff_t conversion
//
#define DWordPtrToPtrdiffT  ULongPtrToIntPtr

//
// DWORD_PTR -> SSIZE_T conversion
//
#define DWordPtrToSSIZET    ULongPtrToLongPtr

//
// INT64 -> INT conversion
//
__inline
__checkReturn
HRESULT
Int64ToInt(
    __in INT64 i64Operand,
    __out INT* piResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *piResult = INT_ERROR;
    
    if ((i64Operand >= INTSAFE_INT_MIN) && (i64Operand <= INTSAFE_INT_MAX))
    {
        *piResult = (INT)i64Operand;
        hr = S_OK;
    }
    
    return hr;
}

//
// INT64 -> INT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
Int64ToIntPtr(
    __in INT64 i64Operand,
    __out INT_PTR* piResult)
{
    *piResult = (INT_PTR)i64Operand;
    return S_OK;
}
#else
#define Int64ToIntPtr   Int64ToInt
#endif

//
// INT64 -> UINT conversion
//
__inline
__checkReturn
HRESULT
Int64ToUInt(
    __in INT64 i64Operand,
    __out UINT* puResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *puResult = UINT_ERROR;
    
    if ((i64Operand >= 0) && (i64Operand <= INTSAFE_UINT_MAX))
    {
        *puResult = (UINT)i64Operand;
        hr = S_OK;
    }
    
    return hr;    
}

//
// INT64 -> UINT_PTR conversion
//
#ifdef _WIN64
#define Int64ToUIntPtr  Int64ToULongLong
#else
#define Int64ToUIntPtr  Int64ToUInt
#endif

//
// INT64 -> LONG conversion
//
__inline
__checkReturn
HRESULT
Int64ToLong(
    __in INT64 i64Operand,
    __out LONG* plResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *plResult = LONG_ERROR;
    
    if ((i64Operand >= INTSAFE_LONG_MIN) && (i64Operand <= INTSAFE_LONG_MAX))
    {
        *plResult = (LONG)i64Operand;
        hr = S_OK;
    }
    
    return hr;    
}

//
// INT64 -> LONG_PTR conversion
//
#ifdef _WIN64
#define Int64ToLongPtr  Int64ToIntPtr
#else
#define Int64ToLongPtr  Int64ToLong
#endif

//
// INT64 -> ULONG conversion
//
__inline
__checkReturn
HRESULT
Int64ToULong(
    __in INT64 i64Operand,
    __out ULONG* pulResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pulResult = ULONG_ERROR;
    
    if ((i64Operand >= 0) && (i64Operand <= INTSAFE_ULONG_MAX))
    {
        *pulResult = (ULONG)i64Operand;
        hr = S_OK;
    }
    
    return hr;    
}

//
// INT64 -> ULONG_PTR conversion
//
#ifdef _WIN64
#define Int64ToULongPtr Int64ToULongLong
#else
#define Int64ToULongPtr Int64ToULong
#endif

//
// INT64 -> DWORD conversion
//
#define Int64ToDWord    Int64ToULong

//
// INT64 -> DWORD_PTR conversion
//
#define Int64ToDWordPtr Int64ToULongPtr

//
// INT64 -> ULONGLONG conversion
//
__inline
__checkReturn
HRESULT
Int64ToULongLong(
    __in INT64 i64Operand,
    __out ULONGLONG* pullResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pullResult = ULONGLONG_ERROR;
    
    if (i64Operand >= 0)
    {
        *pullResult = (ULONGLONG)i64Operand;
        hr = S_OK;
    }
    
    return hr; 
}

//
// INT64 -> ptrdiff_t conversion
//
#define Int64ToPtrdiffT Int64ToIntPtr

//
// INT64 -> size_t conversion
//
#define Int64ToSizeT    Int64ToUIntPtr

//
// INT64 -> SSIZE_T conversion
//
#define Int64ToSSIZET   Int64ToLongPtr

//
// INT64 -> SIZE_T conversion
//
#define Int64ToSIZET    Int64ToULongPtr

//
// ULONGLONG -> INT conversion
//
__inline
__checkReturn
HRESULT
ULongLongToInt(
    __in ULONGLONG ullOperand,
    __out INT* piResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *piResult = INT_ERROR;
    
    if (ullOperand <= INTSAFE_INT_MAX)
    {
        *piResult = (INT)ullOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONGLONG -> INT_PTR conversion
//
#ifdef _WIN64
#define ULongLongToIntPtr   ULongLongToInt64
#else
#define ULongLongToIntPtr   ULongLongToInt
#endif

//
// ULONGLONG -> UINT conversion
//
__inline
__checkReturn
HRESULT
ULongLongToUInt(
    __in ULONGLONG ullOperand,
    __out UINT* puResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *puResult = UINT_ERROR;
    
    if (ullOperand <= INTSAFE_UINT_MAX)
    {
        *puResult = (UINT)ullOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONGLONG -> UINT_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
ULongLongToUIntPtr(
    __in ULONGLONG ullOperand,
    __out UINT_PTR* puResult)
{
    *puResult = (UINT_PTR)ullOperand;
    return S_OK;
}
#else    
#define ULongLongToUIntPtr  ULongLongToUInt
#endif

//
// ULONGLONG -> LONG conversion
//
__inline
__checkReturn
HRESULT
ULongLongToLong(
    __in ULONGLONG ullOperand,
    __out LONG* plResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *plResult = LONG_ERROR;
    
    if (ullOperand <= INTSAFE_LONG_MAX)
    {
        *plResult = (LONG)ullOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONGLONG -> LONG_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
ULongLongToLongPtr(
    __in ULONGLONG ullOperand,
    __out LONG_PTR* plResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *plResult = LONG_PTR_ERROR;
    
    if (ullOperand <= INTSAFE_LONG_PTR_MAX)
    {
        *plResult = (LONG_PTR)ullOperand;
        hr = S_OK;
    }
    
    return hr;
}
#else
#define ULongLongToLongPtr  ULongLongToLong
#endif

//
// ULONGLONG -> ULONG conversion
//
__inline
__checkReturn
HRESULT
ULongLongToULong(
    __in ULONGLONG ullOperand,
    __out ULONG* pulResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pulResult = ULONG_ERROR;
    
    if (ullOperand <= INTSAFE_ULONG_MAX)
    {
        *pulResult = (ULONG)ullOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONGLONG -> ULONG_PTR conversion
//
#ifdef _WIN64
__inline
__checkReturn
HRESULT
ULongLongToULongPtr(
    __in ULONGLONG ullOperand,
    __out ULONG_PTR* pulResult)
{
    *pulResult = (ULONG_PTR)ullOperand;
    return S_OK;
}
#else
#define ULongLongToULongPtr ULongLongToULong
#endif

//
// ULONGLONG -> DWORD conversion
//
#define ULongLongToDWord    ULongLongToULong

//
// ULONGLONG -> DWORD_PTR conversion
//
#define ULongLongToDWordPtr ULongLongToULongPtr

//
// ULONGLONG -> INT64 conversion
//
__inline
__checkReturn
HRESULT
ULongLongToInt64(
    __in ULONGLONG ullOperand,
    __out INT64* pi64Result)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pi64Result = INT64_ERROR;
    
    if (ullOperand <= INTSAFE_INT64_MAX)
    {
        *pi64Result = (INT64)ullOperand;
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONGLONG -> ptrdiff_t conversion
//
#define ULongLongToPtrdiffT ULongLongToIntPtr

//
// ULONGLONG -> size_t conversion
//
#define ULongLongToSizeT    ULongLongToUIntPtr

//
// ULONGLONG -> SSIZE_T conversion
//
#define ULongLongToSSIZET   ULongLongToLongPtr

//
// ULONGLONG -> SIZE_T conversion
//
#define ULongLongToSIZET    ULongLongToULongPtr

//
// ptrdiff_t -> INT conversion
//
#define PtrdiffTToInt   IntPtrToInt

//
// ptrdiff_t -> UINT conversion
//
#define PtrdiffTToUInt  IntPtrToUInt

//
// ptrdiff_t -> UINT_PTR conversion
//
#define PtrdiffTToUIntPtr   IntPtrToUIntPtr

//
// ptrdiff_t -> LONG conversion
//
#define PtrdiffTToLong  IntPtrToLong

//
// ptrdiff_t -> ULONG conversion
//
#define PtrdiffTToULong IntPtrToULong

//
// ptrdiff_t -> ULONG_PTR conversion
//
#define PtrdiffTToULongPtr  IntPtrToULongPtr

//
// ptrdiff_t -> DWORD conversion
//
#define PtrdiffTToDWord IntPtrToULong

//
// ptrdiff_t -> DWORD_PTR conversion
//
#define PtrdiffTToDWordPtr  IntPtrToULongPtr

//
// ptrdiff_t -> size_t conversion
//
#define PtrdiffTToSizeT IntPtrToUIntPtr

//
// ptrdiff_t -> SIZE_T conversion
//
#define PtrdiffTToSIZET IntPtrToULongPtr

//
// size_t -> INT conversion
//
#define SizeTToInt  UIntPtrToInt

//
// size_t -> INT_PTR conversion
//
#define SizeTToIntPtr   UIntPtrToIntPtr

//
// size_t -> UINT conversion
//
#define SizeTToUInt UIntPtrToUInt

//
// size_t -> LONG conversion
//
#define SizeTToLong UIntPtrToLong

//
// size_t -> LONG_PTR conversion
//
#define SizeTToLongPtr  UIntPtrToLongPtr

//
// size_t -> ULONG conversion
//
#define SizeTToULong    UIntPtrToULong

//
// size_t -> DWORD conversion
//
#define SizeTToDWord    UIntPtrToULong

//
// size_t -> INT64
//
__inline
__checkReturn
HRESULT
SizeTToInt64(
    __in size_t Operand,
    __out INT64* pi64Result)
{
#ifdef _WIN64
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pi64Result = INT64_ERROR;
    
    if (Operand <= INTSAFE_INT64_MAX)
    {
        *pi64Result = (INT64)Operand;
        hr = S_OK;
    }
    
    return hr; 
#else
    *pi64Result = (INT64)Operand;
    return S_OK;
#endif
}

//   
// size_t -> ptrdiff_t conversion
//
#define SizeTToPtrdiffT UIntPtrToIntPtr

//
// size_t -> SSIZE_T conversion
//
#define SizeTToSSIZET   UIntPtrToLongPtr

//
// SSIZE_T -> INT conversion
//
#define SSIZETToInt LongPtrToInt

//
// SSIZE_T -> INT_PTR conversion
//
#define SSIZETToIntPtr  LongPtrToIntPtr

//
// SSIZE_T -> UINT conversion
//
#define SSIZETToUInt    LongPtrToUInt

//
// SSIZE_T -> UINT_PTR conversion
//
#define SSIZETToUIntPtr LongPtrToUIntPtr

//
// SSIZE_T -> LONG conversion
//
#define SSIZETToLong    LongPtrToLong

//
// SSIZE_T -> ULONG conversion
//
#define SSIZETToULong   LongPtrToULong

//
// SSIZE_T -> ULONG_PTR conversion
//
#define SSIZETToULongPtr    LongPtrToULongPtr

//
// SSIZE_T -> DWORD conversion
//
#define SSIZETToDWord   LongPtrToULong

//
// SSIZE_T -> DWORD_PTR conversion
//
#define SSIZETToDWordPtr    LongPtrToULongPtr

//
// SSIZE_T -> size_t conversion
//
#define SSIZETToSizeT   LongPtrToUIntPtr

//
// SSIZE_T -> SIZE_T conversion
//
#define SSIZETToSIZET   LongPtrToULongPtr

//
// SIZE_T -> INT conversion
//
#define SIZETToInt  ULongPtrToInt

//
// SIZE_T -> INT_PTR conversion
//
#define SIZETToIntPtr   ULongPtrToIntPtr

//
// SIZE_T -> UINT conversion
//
#define SIZETToUInt ULongPtrToUInt

//
// SIZE_T -> LONG conversion
//
#define SIZETToLong ULongPtrToLong

//
// SIZE_T -> LONG_PTR conversion
//
#define SIZETToLongPtr  ULongPtrToLongPtr

//
// SIZE_T -> ULONG conversion
//
#define SIZETToULong    ULongPtrToULong

//
// SIZE_T -> DWORD conversion
//
#define SIZETToDWord    ULongPtrToULong

//
// SIZE_T -> INT64
//
#define SIZETToInt64    ULongPtrToInt64

//
// SIZE_T -> ptrdiff_t conversion
//
#define SIZETToPtrdiffT ULongPtrToIntPtr

//
// SIZE_T -> SSIZE_T conversion
//
#define SIZETToSSIZET   ULongPtrToLongPtr


// ============================================================================
// Addition functions
//=============================================================================

//
// USHORT addition
//
__inline
__checkReturn
HRESULT
UShortAdd(
    __in USHORT usAugend,
    __in USHORT usAddend,
    __out USHORT* pusResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pusResult = USHORT_ERROR;

    if (((USHORT)(usAugend + usAddend)) >= usAugend)
    {
        *pusResult = (usAugend + usAddend);
        hr = S_OK;
    }
    
    return hr;
}

//
// WORD addtition
//
#define WordAdd     UShortAdd

//
// UINT addition
//
__inline
__checkReturn
HRESULT
UIntAdd(
    __in UINT uAugend,
    __in UINT uAddend,
    __out UINT* puResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *puResult = UINT_ERROR;

    if ((uAugend + uAddend) >= uAugend)
    {
        *puResult = (uAugend + uAddend);
        hr = S_OK;
    }
    
    return hr;
}

//
// UINT_PTR addition
//
#ifdef _WIN64
#define UIntPtrAdd      ULongLongAdd
#else
#define UIntPtrAdd      UIntAdd
#endif // _WIN64

//
// ULONG addition
//
__inline
__checkReturn
HRESULT
ULongAdd(
    __in ULONG ulAugend,
    __in ULONG ulAddend,
    __out ULONG* pulResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pulResult = ULONG_ERROR;

    if ((ulAugend + ulAddend) >= ulAugend)
    {
        *pulResult = (ulAugend + ulAddend);
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONG_PTR addition
//
#ifdef _WIN64
#define ULongPtrAdd     ULongLongAdd
#else
#define ULongPtrAdd     ULongAdd
#endif // _WIN64

//
// DWORD addition
//
#define DWordAdd        ULongAdd

//
// DWORD_PTR addition
//
#ifdef _WIN64
#define DWordPtrAdd     ULongLongAdd
#else
#define DWordPtrAdd     ULongAdd
#endif // _WIN64

//
// size_t addition
//
__inline
__checkReturn
HRESULT
SizeTAdd(
    __in size_t Augend,
    __in size_t Addend,
    __out size_t* pResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pResult = size_t_ERROR;

    if ((Augend + Addend) >= Augend)
    {
        *pResult = (Augend + Addend);
        hr = S_OK;
    }
    
    return hr;
}

//
// SIZE_T addition
//
#ifdef _WIN64
#define SIZETAdd      ULongLongAdd
#else
#define SIZETAdd      ULongAdd
#endif // _WIN64

//
// ULONGLONG addition
//
__inline
__checkReturn
HRESULT
ULongLongAdd(
    __in ULONGLONG ullAugend,
    __in ULONGLONG ullAddend,
    __out ULONGLONG* pullResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pullResult = ULONGLONG_ERROR;

    if ((ullAugend + ullAddend) >= ullAugend)
    {
        *pullResult = (ullAugend + ullAddend);
        hr = S_OK;
    }
    
    return hr;
}


// ============================================================================
// Subtraction functions
//=============================================================================

//
// USHORT subtraction
//
__inline
__checkReturn
HRESULT
UShortSub(
    __in USHORT usMinuend,
    __in USHORT usSubtrahend,
    __out USHORT* pusResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pusResult = USHORT_ERROR;

    if (usMinuend >= usSubtrahend)
    {
        *pusResult = (usMinuend - usSubtrahend);
        hr = S_OK;
    }
    
    return hr;
}

//
// WORD subtraction
//
#define WordSub     UShortSub


//
// UINT subtraction
//
__inline
__checkReturn
HRESULT
UIntSub(
    __in UINT uMinuend,
    __in UINT uSubtrahend,
    __out UINT* puResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *puResult = UINT_ERROR;

    if (uMinuend >= uSubtrahend)
    {
        *puResult = (uMinuend - uSubtrahend);
        hr = S_OK;
    }
    
    return hr;
}

//
// UINT_PTR subtraction
//
#ifdef _WIN64
#define UIntPtrSub  ULongLongSub
#else
#define UIntPtrSub  UIntSub
#endif // _WIN64

//
// ULONG subtraction
//
__inline
__checkReturn
HRESULT
ULongSub(
    __in ULONG ulMinuend,
    __in ULONG ulSubtrahend,
    __out ULONG* pulResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pulResult = ULONG_ERROR;

    if (ulMinuend >= ulSubtrahend)
    {
        *pulResult = (ulMinuend - ulSubtrahend);
        hr = S_OK;
    }
    
    return hr;
}

//
// ULONG_PTR subtraction
//
#ifdef _WIN64
#define ULongPtrSub ULongLongSub
#else
#define ULongPtrSub ULongSub
#endif // _WIN64


//
// DWORD subtraction
//
#define DWordSub        ULongSub

//
// DWORD_PTR subtraction
//
#ifdef _WIN64
#define DWordPtrSub     ULongLongSub
#else
#define DWordPtrSub     ULongSub
#endif // _WIN64

//
// size_t subtraction
//
__inline
__checkReturn
HRESULT
SizeTSub(
    __in size_t Minuend,
    __in size_t Subtrahend,
    __out size_t* pResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pResult = size_t_ERROR;

    if (Minuend >= Subtrahend)
    {
        *pResult = (Minuend - Subtrahend);
        hr = S_OK;
    }
    
    return hr;
}

//
// SIZE_T subtraction
//
#ifdef _WIN64
#define SIZETSub    ULongLongSub
#else
#define SIZETSub    ULongSub
#endif // _WIN64

//
// ULONGLONG subtraction
//
__inline
__checkReturn
HRESULT
ULongLongSub(
    __in ULONGLONG ullMinuend,
    __in ULONGLONG ullSubtrahend,
    __out ULONGLONG* pullResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
    *pullResult = ULONGLONG_ERROR;

    if (ullMinuend >= ullSubtrahend)
    {
        *pullResult = (ullMinuend - ullSubtrahend);
        hr = S_OK;
    }
    
    return hr;
}


// ============================================================================
// Multiplication functions
//=============================================================================

//
// USHORT multiplication
//
__inline
__checkReturn
HRESULT
UShortMult(
    __in USHORT usMultiplicand,
    __in USHORT usMultiplier,
    __out USHORT* pusResult)
{
    ULONG ulResult = ((ULONG)usMultiplicand) * (ULONG)usMultiplier;
    
    return ULongToUShort(ulResult, pusResult);
}

//
// WORD multiplication
//
#define WordMult      UShortMult

//
// UINT multiplication
//
__inline
__checkReturn
HRESULT
UIntMult(
    __in UINT uMultiplicand,
    __in UINT uMultiplier,
    __out UINT* puResult)
{
    ULONGLONG ull64Result = UInt32x32To64(uMultiplicand, uMultiplier);

    return ULongLongToUInt(ull64Result, puResult);
}

//
// UINT_PTR multiplication
//
#ifdef _WIN64
#define UIntPtrMult     ULongLongMult
#else
#define UIntPtrMult     UIntMult
#endif // _WIN64

//
// ULONG multiplication
//
__inline
__checkReturn
HRESULT
ULongMult(
    __in ULONG ulMultiplicand,
    __in ULONG ulMultiplier,
    __out ULONG* pulResult)
{
    ULONGLONG ull64Result = UInt32x32To64(ulMultiplicand, ulMultiplier);
    
    return ULongLongToULong(ull64Result, pulResult);
}

//
// ULONG_PTR multiplication
//
#ifdef _WIN64
#define ULongPtrMult    ULongLongMult
#else
#define ULongPtrMult    ULongMult
#endif // _WIN64


//
// DWORD multiplication
//
#define DWordMult       ULongMult

//
// DWORD_PTR multiplication
//
#ifdef _WIN64
#define DWordPtrMult    ULongLongMult
#else
#define DWordPtrMult    ULongMult
#endif // _WIN64

//
// size_t multiplication
//

#ifdef _WIN64
#define SizeTMult       ULongLongMult
#else
#define SizeTMult       UIntMult
#endif // _WIN64

//
// SIZE_T multiplication
//
#ifdef _WIN64
#define SIZETMult       ULongLongMult
#else
#define SIZETMult       ULongMult
#endif // _WIN64

//
// ULONGLONG multiplication
//
__inline
__checkReturn
HRESULT
ULongLongMult(
    __in ULONGLONG ullMultiplicand,
    __in ULONGLONG ullMultiplier,
    __out ULONGLONG* pullResult)
{
    HRESULT hr = INTSAFE_E_ARITHMETIC_OVERFLOW;
#ifdef _AMD64_
    ULONGLONG u64ResultHigh;
    ULONGLONG u64ResultLow;
    
    *pullResult = ULONGLONG_ERROR;
    
    u64ResultLow = UnsignedMultiply128(ullMultiplicand, ullMultiplier, &u64ResultHigh);
    if (u64ResultHigh == 0)
    {
        *pullResult = u64ResultLow;
        hr = S_OK;
    }
#else
    // 64x64 into 128 is like 32.32 x 32.32.
    //
    // a.b * c.d = a*(c.d) + .b*(c.d) = a*c + a*.d + .b*c + .b*.d
    // back in non-decimal notation where A=a*2^32 and C=c*2^32:  
    // A*C + A*d + b*C + b*d
    // So there are four components to add together.
    //   result = (a*c*2^64) + (a*d*2^32) + (b*c*2^32) + (b*d)
    //
    // a * c must be 0 or there would be bits in the high 64-bits
    // a * d must be less than 2^32 or there would be bits in the high 64-bits
    // b * c must be less than 2^32 or there would be bits in the high 64-bits
    // then there must be no overflow of the resulting values summed up.
    
    ULONG dw_a;
    ULONG dw_b;
    ULONG dw_c;
    ULONG dw_d;
    ULONGLONG ad = 0;
    ULONGLONG bc = 0;
    ULONGLONG bd = 0;
    ULONGLONG ullResult = 0;
    
    *pullResult = ULONGLONG_ERROR;

    dw_a = (ULONG)(ullMultiplicand >> 32);
    dw_c = (ULONG)(ullMultiplier >> 32);

    // common case -- if high dwords are both zero, no chance for overflow
    if ((dw_a == 0) && (dw_c == 0))
    {
        dw_b = (DWORD)ullMultiplicand;
        dw_d = (DWORD)ullMultiplier;

        *pullResult = (((ULONGLONG)dw_b) * (ULONGLONG)dw_d);
        hr = S_OK;
    }
    else
    {
        // a * c must be 0 or there would be bits set in the high 64-bits
        if ((dw_a == 0) ||
            (dw_c == 0))
        {
            dw_d = (DWORD)ullMultiplier;

            // a * d must be less than 2^32 or there would be bits set in the high 64-bits
            ad = (((ULONGLONG)dw_a) * (ULONGLONG)dw_d);
            if ((ad & 0xffffffff00000000) == 0)
            {
                dw_b = (DWORD)ullMultiplicand;

                // b * c must be less than 2^32 or there would be bits set in the high 64-bits
                bc = (((ULONGLONG)dw_b) * (ULONGLONG)dw_c);
                if ((bc & 0xffffffff00000000) == 0)
                {
                    // now sum them all up checking for overflow.
                    // shifting is safe because we already checked for overflow above
                    if (SUCCEEDED(ULongLongAdd(bc << 32, ad << 32, &ullResult)))                        
                    {
                        // b * d
                        bd = (((ULONGLONG)dw_b) * (ULONGLONG)dw_d);
                    
                        if (SUCCEEDED(ULongLongAdd(ullResult, bd, &ullResult)))
                        {
                            *pullResult = ullResult;
                            hr = S_OK;
                        }
                    }
                }
            }
        }
    }
#endif // _AMD64_  
    
    return hr;
}

#endif // _INTSAFE_H_INCLUDED_

