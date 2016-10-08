/***
*fpieee.h - Definitions for floating point IEEE exception handling
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains constant and type definitions for handling
*       floating point exceptions [ANSI/IEEE std. 754]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_FPIEEE
#define _INC_FPIEEE

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifndef __assembler     /* MIPS ONLY: Protect from assembler */

#ifdef  _MSC_VER
/*
 * Currently, all MS C compilers for Win32 platforms default to 8 byte
 * alignment.
 */
#pragma pack(push,8)

/* Disable C4324: structure was padded due to __declspec(align()) */
#pragma warning(push)
#pragma warning(disable: 4324)
#endif  /* _MSC_VER */

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


/* Define __cdecl for non-Microsoft compilers */

#if     ( !defined(_MSC_VER) && !defined(__cdecl) )
#define __cdecl
#endif

/*
 * Define floating point IEEE compare result values.
 */

typedef enum {
    _FpCompareEqual,
    _FpCompareGreater,
    _FpCompareLess,
    _FpCompareUnordered
} _FPIEEE_COMPARE_RESULT;

/*
 * Define floating point format and result precision values.
 */

typedef enum {
    _FpFormatFp32,
    _FpFormatFp64,
    _FpFormatFp80,
    _FpFormatFp128,
    _FpFormatI16,
    _FpFormatI32,
    _FpFormatI64,
    _FpFormatU16,
    _FpFormatU32,
    _FpFormatU64,
    _FpFormatBcd80,
    _FpFormatCompare,
    _FpFormatString,
#if defined(_M_IA64)
    _FpFormatFp82
#endif
} _FPIEEE_FORMAT;

/*
 * Define operation code values.
 */

typedef enum {
    _FpCodeUnspecified,
    _FpCodeAdd,
    _FpCodeSubtract,
    _FpCodeMultiply,
    _FpCodeDivide,
    _FpCodeSquareRoot,
    _FpCodeRemainder,
    _FpCodeCompare,
    _FpCodeConvert,
    _FpCodeRound,
    _FpCodeTruncate,
    _FpCodeFloor,
    _FpCodeCeil,
    _FpCodeAcos,
    _FpCodeAsin,
    _FpCodeAtan,
    _FpCodeAtan2,
    _FpCodeCabs,
    _FpCodeCos,
    _FpCodeCosh,
    _FpCodeExp,
    _FpCodeFabs,
    _FpCodeFmod,
    _FpCodeFrexp,
    _FpCodeHypot,
    _FpCodeLdexp,
    _FpCodeLog,
    _FpCodeLog10,
    _FpCodeModf,
    _FpCodePow,
    _FpCodeSin,
    _FpCodeSinh,
    _FpCodeTan,
    _FpCodeTanh,
    _FpCodeY0,
    _FpCodeY1,
    _FpCodeYn,
    _FpCodeLogb,
    _FpCodeNextafter,
    _FpCodeNegate, 
    _FpCodeFmin,         /* XMMI */
    _FpCodeFmax,         /* XMMI */
    _FpCodeConvertTrunc, /* XMMI */
    _XMMIAddps,          /* XMMI */
    _XMMIAddss,
    _XMMISubps,
    _XMMISubss,
    _XMMIMulps,
    _XMMIMulss,
    _XMMIDivps,
    _XMMIDivss,
    _XMMISqrtps,
    _XMMISqrtss,
    _XMMIMaxps,
    _XMMIMaxss,
    _XMMIMinps,
    _XMMIMinss,
    _XMMICmpps,
    _XMMICmpss,
    _XMMIComiss,
    _XMMIUComiss,
    _XMMICvtpi2ps,
    _XMMICvtsi2ss,
    _XMMICvtps2pi,
    _XMMICvtss2si,
    _XMMICvttps2pi,
    _XMMICvttss2si,
    _XMMI2Addpd,         /* XMMI2 */
    _XMMI2Addsd,
    _XMMI2Subpd,
    _XMMI2Subsd,
    _XMMI2Mulpd,
    _XMMI2Mulsd,
    _XMMI2Divpd,
    _XMMI2Divsd,
    _XMMI2Sqrtpd,
    _XMMI2Sqrtsd,
    _XMMI2Maxpd,
    _XMMI2Maxsd,
    _XMMI2Minpd,
    _XMMI2Minsd,
    _XMMI2Cmppd,
    _XMMI2Cmpsd,
    _XMMI2Comisd,
    _XMMI2UComisd,
    _XMMI2Cvtpd2pi,   /* 66 2D    */
    _XMMI2Cvtsd2si,   /* F2       */
    _XMMI2Cvttpd2pi,  /* 66 2C    */
    _XMMI2Cvttsd2si,  /* F2       */
    _XMMI2Cvtps2pd,   /* 0F 5A    */
    _XMMI2Cvtss2sd,   /* F3       */
    _XMMI2Cvtpd2ps,   /* 66       */
    _XMMI2Cvtsd2ss,   /* F2       */
    _XMMI2Cvtdq2ps,   /* 0F 5B    */
    _XMMI2Cvttps2dq,  /* F3       */
    _XMMI2Cvtps2dq,   /* 66       */
    _XMMI2Cvttpd2dq,  /* 66 0F E6 */
    _XMMI2Cvtpd2dq,   /* F2       */
#if defined(_M_IA64)
    _FpCodeFma,
    _FpCodeFmaSingle,
    _FpCodeFmaDouble,
    _FpCodeFms,
    _FpCodeFmsSingle,
    _FpCodeFmsDouble,
    _FpCodeFnma,
    _FpCodeFnmaSingle,
    _FpCodeFnmaDouble,
    _FpCodeFamin,
    _FpCodeFamax
#endif
} _FP_OPERATION_CODE;

#endif  /* #ifndef __assembler */

/*
 * Define rounding modes.
 */

#ifndef __assembler     /* MIPS ONLY: Protect from assembler */

typedef enum {
    _FpRoundNearest,
    _FpRoundMinusInfinity,
    _FpRoundPlusInfinity,
    _FpRoundChopped
} _FPIEEE_ROUNDING_MODE;

typedef enum {
    _FpPrecisionFull,
    _FpPrecision53,
    _FpPrecision24,
#if defined(_M_IA64)
    _FpPrecision64,
    _FpPrecision113
#endif
} _FPIEEE_PRECISION;


/*
 * Define floating point context record
 */

typedef float           _FP32;
typedef double          _FP64;
typedef short           _I16;
typedef int             _I32;
typedef unsigned short  _U16;
typedef unsigned int    _U32;
typedef __int64         _Q64;


typedef struct
#if defined(_M_IA64)
    __declspec(align(16))
#endif
{
    unsigned short W[5];
} _FP80;

typedef struct __declspec(align(16))
{
    unsigned long W[4];
} _FP128;

typedef struct __declspec(align(8))
{
    unsigned long W[2];
} _I64;

typedef struct __declspec(align(8))
{
    unsigned long W[2];
} _U64;

typedef struct
#if defined(_M_IA64)
    __declspec(align(16))
#endif
{
    unsigned short W[5];
} _BCD80;

typedef struct __declspec(align(16))
{
    _Q64 W[2];
} _FPQ64;

typedef struct {
    union {
        _FP32        Fp32Value;
        _FP64        Fp64Value;
        _FP80        Fp80Value;
        _FP128       Fp128Value;
        _I16         I16Value;
        _I32         I32Value;
        _I64         I64Value;
        _U16         U16Value;
        _U32         U32Value;
        _U64         U64Value;
        _BCD80       Bcd80Value;
        char         *StringValue;
        int          CompareValue;
        _Q64         Q64Value;
        _FPQ64       Fpq64Value;
    } Value;

    unsigned int OperandValid : 1;
    unsigned int Format : 4;

} _FPIEEE_VALUE;


typedef struct {
    unsigned int Inexact : 1;
    unsigned int Underflow : 1;
    unsigned int Overflow : 1;
    unsigned int ZeroDivide : 1;
    unsigned int InvalidOperation : 1;
} _FPIEEE_EXCEPTION_FLAGS;


typedef struct {
    unsigned int RoundingMode : 2;
    unsigned int Precision : 3;
    unsigned int Operation :12;
    _FPIEEE_EXCEPTION_FLAGS Cause;
    _FPIEEE_EXCEPTION_FLAGS Enable;
    _FPIEEE_EXCEPTION_FLAGS Status;
    _FPIEEE_VALUE Operand1;
    _FPIEEE_VALUE Operand2;
    _FPIEEE_VALUE Result;
#if defined(_M_IA64)
    _FPIEEE_VALUE Operand3;
#endif
} _FPIEEE_RECORD, *_PFPIEEE_RECORD;


struct _EXCEPTION_POINTERS;

/*
 * Floating point IEEE exception filter routine
 */

_CRTIMP int __cdecl _fpieee_flt(
        unsigned long,
        struct _EXCEPTION_POINTERS *,
        int (__cdecl *)(_FPIEEE_RECORD *)
        );

#ifdef  __cplusplus
}
#endif

#ifdef  _MSC_VER
#pragma warning(pop)
#pragma pack(pop)
#endif  /* _MSC_VER */
#endif  /* #ifndef __assembler */

#endif  /* _INC_FPIEEE */

