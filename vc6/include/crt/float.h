/***
*float.h - constants for floating point values
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains defines for a number of implementation dependent
*       values which are commonly used by sophisticated numerical (floating
*       point) programs.
*       [ANSI]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_FLOAT
#define _INC_FLOAT

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

#define DBL_DIG         15                      /* # of decimal digits of precision */
#define DBL_EPSILON     2.2204460492503131e-016 /* smallest such that 1.0+DBL_EPSILON != 1.0 */
#define DBL_MANT_DIG    53                      /* # of bits in mantissa */
#define DBL_MAX         1.7976931348623158e+308 /* max value */
#define DBL_MAX_10_EXP  308                     /* max decimal exponent */
#define DBL_MAX_EXP     1024                    /* max binary exponent */
#define DBL_MIN         2.2250738585072014e-308 /* min positive value */
#define DBL_MIN_10_EXP  (-307)                  /* min decimal exponent */
#define DBL_MIN_EXP     (-1021)                 /* min binary exponent */
#define _DBL_RADIX      2                       /* exponent radix */
#define _DBL_ROUNDS     1                       /* addition rounding: near */

#define FLT_DIG         6                       /* # of decimal digits of precision */
#define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */
#define FLT_GUARD       0
#define FLT_MANT_DIG    24                      /* # of bits in mantissa */
#define FLT_MAX         3.402823466e+38F        /* max value */
#define FLT_MAX_10_EXP  38                      /* max decimal exponent */
#define FLT_MAX_EXP     128                     /* max binary exponent */
#define FLT_MIN         1.175494351e-38F        /* min positive value */
#define FLT_MIN_10_EXP  (-37)                   /* min decimal exponent */
#define FLT_MIN_EXP     (-125)                  /* min binary exponent */
#define FLT_NORMALIZE   0
#define FLT_RADIX       2                       /* exponent radix */
#define FLT_ROUNDS      1                       /* addition rounding: near */

#define LDBL_DIG        DBL_DIG                 /* # of decimal digits of precision */
#define LDBL_EPSILON    DBL_EPSILON             /* smallest such that 1.0+LDBL_EPSILON != 1.0 */
#define LDBL_MANT_DIG   DBL_MANT_DIG            /* # of bits in mantissa */
#define LDBL_MAX        DBL_MAX                 /* max value */
#define LDBL_MAX_10_EXP DBL_MAX_10_EXP          /* max decimal exponent */
#define LDBL_MAX_EXP    DBL_MAX_EXP             /* max binary exponent */
#define LDBL_MIN        DBL_MIN                 /* min positive value */
#define LDBL_MIN_10_EXP DBL_MIN_10_EXP          /* min decimal exponent */
#define LDBL_MIN_EXP    DBL_MIN_EXP             /* min binary exponent */
#define _LDBL_RADIX     DBL_RADIX               /* exponent radix */
#define _LDBL_ROUNDS    DBL_ROUNDS              /* addition rounding: near */

/* Function prototypes */

_CRTIMP unsigned int __cdecl _clearfp(void);
_CRTIMP unsigned int __cdecl _controlfp(unsigned int,unsigned int);
_CRTIMP unsigned int __cdecl _statusfp(void);
_CRTIMP void __cdecl _fpreset(void);

#if defined(_M_IX86)
_CRTIMP void _statusfp2(unsigned int *pX86, unsigned int *pSSE2);
#endif

#define _clear87        _clearfp
#define _status87       _statusfp

/*
 * Abstract User Status Word bit definitions
 */

#define _SW_INEXACT     0x00000001              /* inexact (precision) */
#define _SW_UNDERFLOW   0x00000002              /* underflow */
#define _SW_OVERFLOW    0x00000004              /* overflow */
#define _SW_ZERODIVIDE  0x00000008              /* zero divide */
#define _SW_INVALID     0x00000010              /* invalid */
#define _SW_DENORMAL    0x00080000              /* denormal status bit */

/*
 * New Control Bit that specifies the ambiguity in control word.
 */
#define _EM_AMBIGUIOUS  0x80000000
/*
 * Abstract User Control Word Mask and bit definitions
 */
#define _MCW_EM         0x0008001f              /* interrupt Exception Masks */
#define _EM_INEXACT     0x00000001              /*   inexact (precision) */
#define _EM_UNDERFLOW   0x00000002              /*   underflow */
#define _EM_OVERFLOW    0x00000004              /*   overflow */
#define _EM_ZERODIVIDE  0x00000008              /*   zero divide */
#define _EM_INVALID     0x00000010              /*   invalid */
#define _EM_DENORMAL    0x00080000              /* denormal exception mask (_control87 only) */

#define _MCW_RC         0x00000300              /* Rounding Control */
#define _RC_NEAR        0x00000000              /*   near */
#define _RC_DOWN        0x00000100              /*   down */
#define _RC_UP          0x00000200              /*   up */
#define _RC_CHOP        0x00000300              /*   chop */

/*
 * i386 specific definitions
 */
#define _MCW_PC         0x00030000              /* Precision Control */
#define _PC_64          0x00000000              /*    64 bits */
#define _PC_53          0x00010000              /*    53 bits */
#define _PC_24          0x00020000              /*    24 bits */

#define _MCW_IC         0x00040000              /* Infinity Control */
#define _IC_AFFINE      0x00040000              /*   affine */
#define _IC_PROJECTIVE  0x00000000              /*   projective */

/*
 * RISC specific definitions
 */

#define _MCW_DN         0x03000000              /* Denormal Control */
#define _DN_SAVE        0x00000000              /*   save denormal results and operands */
#define _DN_FLUSH       0x01000000              /*   flush denormal results and operands to zero */
#define _DN_FLUSH_OPERANDS_SAVE_RESULTS 0x02000000  /*   flush operands to zero and save results */
#define _DN_SAVE_OPERANDS_FLUSH_RESULTS 0x03000000  /*   save operands and flush results to zero */

/* initial Control Word value */

#if     defined(_M_IX86)

#define _CW_DEFAULT ( _RC_NEAR + _PC_53 + _EM_INVALID + _EM_ZERODIVIDE + _EM_OVERFLOW + _EM_UNDERFLOW + _EM_INEXACT + _EM_DENORMAL)

#elif   defined(_M_IA64)

#define _CW_DEFAULT ( _RC_NEAR + _PC_64 + _EM_INVALID + _EM_ZERODIVIDE + _EM_OVERFLOW + _EM_UNDERFLOW + _EM_INEXACT + _EM_DENORMAL)

#elif   defined(_M_AMD64)

#define _CW_DEFAULT ( _RC_NEAR + _EM_INVALID + _EM_ZERODIVIDE + _EM_OVERFLOW + _EM_UNDERFLOW + _EM_INEXACT + _EM_DENORMAL)

#endif

_CRTIMP unsigned int __cdecl _control87(unsigned int,unsigned int);
_CRTIMP int __control87_2(unsigned int newctrl, unsigned int mask,
                          unsigned int* x86_cw, unsigned int* sse2_cw);


/* Global variable holding floating point error code */

#if     defined(_MT) || defined(_DLL)
_CRTIMP extern int * __cdecl __fpecode(void);
#define _fpecode        (*__fpecode())
#else   /* ndef _MT && ndef _DLL */
extern int _fpecode;
#endif  /* _MT || _DLL */

/* invalid subconditions (_SW_INVALID also set) */

#define _SW_UNEMULATED          0x0040  /* unemulated instruction */
#define _SW_SQRTNEG             0x0080  /* square root of a neg number */
#define _SW_STACKOVERFLOW       0x0200  /* FP stack overflow */
#define _SW_STACKUNDERFLOW      0x0400  /* FP stack underflow */

/*  Floating point error signals and return codes */

#define _FPE_INVALID            0x81
#define _FPE_DENORMAL           0x82
#define _FPE_ZERODIVIDE         0x83
#define _FPE_OVERFLOW           0x84
#define _FPE_UNDERFLOW          0x85
#define _FPE_INEXACT            0x86

#define _FPE_UNEMULATED         0x87
#define _FPE_SQRTNEG            0x88
#define _FPE_STACKOVERFLOW      0x8a
#define _FPE_STACKUNDERFLOW     0x8b

#define _FPE_EXPLICITGEN        0x8c    /* raise( SIGFPE ); */


/* IEEE recommended functions */

_CRTIMP double __cdecl _copysign (double, double);
_CRTIMP double __cdecl _chgsign (double);
_CRTIMP double __cdecl _scalb(double, long);
_CRTIMP double __cdecl _logb(double);
_CRTIMP double __cdecl _nextafter(double, double);
_CRTIMP int    __cdecl _finite(double);
_CRTIMP int    __cdecl _isnan(double);
_CRTIMP int    __cdecl _fpclass(double);

#define _FPCLASS_SNAN   0x0001  /* signaling NaN */
#define _FPCLASS_QNAN   0x0002  /* quiet NaN */
#define _FPCLASS_NINF   0x0004  /* negative infinity */
#define _FPCLASS_NN     0x0008  /* negative normal */
#define _FPCLASS_ND     0x0010  /* negative denormal */
#define _FPCLASS_NZ     0x0020  /* -0 */
#define _FPCLASS_PZ     0x0040  /* +0 */
#define _FPCLASS_PD     0x0080  /* positive denormal */
#define _FPCLASS_PN     0x0100  /* positive normal */
#define _FPCLASS_PINF   0x0200  /* positive infinity */


#if     !__STDC__

/* Non-ANSI names for compatibility */

#define clear87         _clear87
#define status87        _status87
#define control87       _control87

_CRTIMP void __cdecl fpreset(void);

#define DBL_RADIX               _DBL_RADIX
#define DBL_ROUNDS              _DBL_ROUNDS

#define LDBL_RADIX              _LDBL_RADIX
#define LDBL_ROUNDS             _LDBL_ROUNDS

#define EM_AMBIGUIOUS           _EM_AMBIGUIOUS

#define MCW_EM                  _MCW_EM
#define EM_INVALID              _EM_INVALID
#define EM_DENORMAL             _EM_DENORMAL
#define EM_ZERODIVIDE           _EM_ZERODIVIDE
#define EM_OVERFLOW             _EM_OVERFLOW
#define EM_UNDERFLOW            _EM_UNDERFLOW
#define EM_INEXACT              _EM_INEXACT

#define MCW_IC                  _MCW_IC
#define IC_AFFINE               _IC_AFFINE
#define IC_PROJECTIVE           _IC_PROJECTIVE

#define MCW_RC                  _MCW_RC
#define RC_CHOP                 _RC_CHOP
#define RC_UP                   _RC_UP
#define RC_DOWN                 _RC_DOWN
#define RC_NEAR                 _RC_NEAR

#define MCW_PC                  _MCW_PC
#define PC_24                   _PC_24
#define PC_53                   _PC_53
#define PC_64                   _PC_64

#define CW_DEFAULT              _CW_DEFAULT

#define SW_INVALID              _SW_INVALID
#define SW_DENORMAL             _SW_DENORMAL
#define SW_ZERODIVIDE           _SW_ZERODIVIDE
#define SW_OVERFLOW             _SW_OVERFLOW
#define SW_UNDERFLOW            _SW_UNDERFLOW
#define SW_INEXACT              _SW_INEXACT

#define SW_UNEMULATED           _SW_UNEMULATED
#define SW_SQRTNEG              _SW_SQRTNEG
#define SW_STACKOVERFLOW        _SW_STACKOVERFLOW
#define SW_STACKUNDERFLOW       _SW_STACKUNDERFLOW

#define FPE_INVALID             _FPE_INVALID
#define FPE_DENORMAL            _FPE_DENORMAL
#define FPE_ZERODIVIDE          _FPE_ZERODIVIDE
#define FPE_OVERFLOW            _FPE_OVERFLOW
#define FPE_UNDERFLOW           _FPE_UNDERFLOW
#define FPE_INEXACT             _FPE_INEXACT

#define FPE_UNEMULATED          _FPE_UNEMULATED
#define FPE_SQRTNEG             _FPE_SQRTNEG
#define FPE_STACKOVERFLOW       _FPE_STACKOVERFLOW
#define FPE_STACKUNDERFLOW      _FPE_STACKUNDERFLOW

#define FPE_EXPLICITGEN         _FPE_EXPLICITGEN

#endif  /* __STDC__ */

#ifdef  __cplusplus
}
#endif

#endif  /* _INC_FLOAT */

