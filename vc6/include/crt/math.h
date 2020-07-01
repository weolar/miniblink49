/***
*math.h - definitions and declarations for math library
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file contains constant definitions and external subroutine
*       declarations for the math subroutine library.
*       [ANSI/System V]
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_MATH
#define _INC_MATH

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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __assembler /* Protect from assembler */

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


/* Definition of _exception struct - this struct is passed to the matherr
 * routine when a floating point exception is detected
 */

#ifndef _EXCEPTION_DEFINED
struct _exception {
        int type;       /* exception type - see below */
        char *name;     /* name of function where error occured */
        double arg1;    /* first argument to function */
        double arg2;    /* second argument (if any) to function */
        double retval;  /* value to be returned by function */
        } ;

#define _EXCEPTION_DEFINED
#endif


/* Definition of a _complex struct to be used by those who use cabs and
 * want type checking on their argument
 */

#ifndef _COMPLEX_DEFINED
struct _complex {
        double x,y; /* real and imaginary parts */
        } ;

#if     !__STDC__ && !defined (__cplusplus)
/* Non-ANSI name for compatibility */
#define complex _complex
#endif

#define _COMPLEX_DEFINED
#endif
#endif  /* __assembler */


/* Constant definitions for the exception type passed in the _exception struct
 */

#define _DOMAIN     1   /* argument domain error */
#define _SING       2   /* argument singularity */
#define _OVERFLOW   3   /* overflow range error */
#define _UNDERFLOW  4   /* underflow range error */
#define _TLOSS      5   /* total loss of precision */
#define _PLOSS      6   /* partial loss of precision */

#define EDOM        33
#define ERANGE      34


/* Definitions of _HUGE and HUGE_VAL - respectively the XENIX and ANSI names
 * for a value returned in case of error by a number of the floating point
 * math routines
 */
#ifndef __assembler /* Protect from assembler */
_CRTIMP extern double _HUGE;
#endif  /* __assembler */

#define HUGE_VAL _HUGE

#ifdef  _USE_MATH_DEFINES

/* Define _USE_MATH_DEFINES before including math.h to expose these macro
 * definitions for common math constants.  These are placed under an #ifdef
 * since these commonly-defined names are not part of the C/C++ standards.
 */

/* Definitions of useful mathematical constants
 * M_E        - e
 * M_LOG2E    - log2(e)
 * M_LOG10E   - log10(e)
 * M_LN2      - ln(2)
 * M_LN10     - ln(10)
 * M_PI       - pi
 * M_PI_2     - pi/2
 * M_PI_4     - pi/4
 * M_1_PI     - 1/pi
 * M_2_PI     - 2/pi
 * M_2_SQRTPI - 2/sqrt(pi)
 * M_SQRT2    - sqrt(2)
 * M_SQRT1_2  - 1/sqrt(2)
 */

#define M_E        2.71828182845904523536
#define M_LOG2E    1.44269504088896340736
#define M_LOG10E   0.434294481903251827651
#define M_LN2      0.693147180559945309417
#define M_LN10     2.30258509299404568402
#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616
#define M_1_PI     0.318309886183790671538
#define M_2_PI     0.636619772367581343076
#define M_2_SQRTPI 1.12837916709551257390
#define M_SQRT2    1.41421356237309504880
#define M_SQRT1_2  0.707106781186547524401

#endif  /* _USE_MATH_DEFINES */

/* Function prototypes */

#if     !defined(__assembler)   /* Protect from assembler */
        int     __cdecl abs(int);
        double  __cdecl acos(double);
        double  __cdecl asin(double);
        double  __cdecl atan(double);
        double  __cdecl atan2(double, double);
        double  __cdecl cos(double);
        double  __cdecl cosh(double);
        double  __cdecl exp(double);
        double  __cdecl fabs(double);
        double  __cdecl fmod(double, double);
        long    __cdecl labs(long);
        double  __cdecl log(double);
        double  __cdecl log10(double);
        double  __cdecl pow(double, double);
        double  __cdecl sin(double);
        double  __cdecl sinh(double);
        double  __cdecl tan(double);
        double  __cdecl tanh(double);
        double  __cdecl sqrt(double);
_CRTIMP double  __cdecl atof(const char *);
_CRTIMP double  __cdecl _cabs(struct _complex);
_CRTIMP double  __cdecl ceil(double);
_CRTIMP double  __cdecl floor(double);
_CRTIMP double  __cdecl frexp(double, int *);
_CRTIMP double  __cdecl _hypot(double, double);
_CRTIMP double  __cdecl _j0(double);
_CRTIMP double  __cdecl _j1(double);
_CRTIMP double  __cdecl _jn(int, double);
_CRTIMP double  __cdecl ldexp(double, int);
        int     __cdecl _matherr(struct _exception *);
_CRTIMP double  __cdecl modf(double, double *);
        float   __cdecl fmaxf(float _X, float _Y);
        float   __cdecl fmax(float _X, float _Y);

_CRTIMP double  __cdecl _y0(double);
_CRTIMP double  __cdecl _y1(double);
_CRTIMP double  __cdecl _yn(int, double);
/*_CRTIMP*/ double __cdecl scalbn( double _X,  int _Y);

#if     defined(_M_IX86)

_CRTIMP int     __cdecl _set_SSE2_enable(int);

#endif

#if defined(_M_IA64)

/* ANSI C, 4.5 Mathematics        */

/* 4.5.2 Trigonometric functions */

_CRTIMP float  __cdecl acosf( float );
_CRTIMP float  __cdecl asinf( float );
_CRTIMP float  __cdecl atanf( float );
_CRTIMP float  __cdecl atan2f( float , float );
_CRTIMP float  __cdecl cosf( float );
_CRTIMP float  __cdecl sinf( float );
_CRTIMP float  __cdecl tanf( float );

/* 4.5.3 Hyperbolic functions */
_CRTIMP float  __cdecl coshf( float );
_CRTIMP float  __cdecl sinhf( float );
_CRTIMP float  __cdecl tanhf( float );

/* 4.5.4 Exponential and logarithmic functions */
_CRTIMP float  __cdecl expf( float );
_CRTIMP float  __cdecl logf( float );
_CRTIMP float  __cdecl log10f( float );
_CRTIMP float  __cdecl modff( float , float* );

/* 4.5.5 Power functions */
_CRTIMP float  __cdecl powf( float , float );
_CRTIMP float  __cdecl sqrtf( float );

/* 4.5.6 Nearest integer, absolute value, and remainder functions */
_CRTIMP float  __cdecl ceilf( float );
_CRTIMP float  __cdecl fabsf( float );
_CRTIMP float  __cdecl floorf( float );
_CRTIMP float  __cdecl fmodf( float , float );

_CRTIMP float  __cdecl _hypotf(float, float);
_CRTIMP float  __cdecl ldexpf(float, int);

#endif /* _M_IA64 */

#if defined(_M_AMD64)

/* ANSI C, 4.5 Mathematics        */

/* 4.5.2 Trigonometric functions */

_CRTIMP float  __cdecl acosf( float );
_CRTIMP float  __cdecl asinf( float );
_CRTIMP float  __cdecl atanf( float );
_CRTIMP float  __cdecl atan2f( float , float );
_CRTIMP float  __cdecl cosf( float );
_CRTIMP float  __cdecl sinf( float );
_CRTIMP float  __cdecl tanf( float );

/* 4.5.3 Hyperbolic functions */
_CRTIMP float  __cdecl coshf( float );
_CRTIMP float  __cdecl sinhf( float );
_CRTIMP float  __cdecl tanhf( float );

/* 4.5.4 Exponential and logarithmic functions */
_CRTIMP float  __cdecl expf( float );
_CRTIMP float  __cdecl logf( float );
_CRTIMP float  __cdecl log10f( float );
_CRTIMP float  __cdecl modff( float , float* );

/* 4.5.5 Power functions */
_CRTIMP float  __cdecl powf( float , float );
_CRTIMP float  __cdecl sqrtf( float );

/* 4.5.6 Nearest integer, absolute value, and remainder functions */
_CRTIMP float  __cdecl ceilf( float );
_CRTIMP float  __cdecl floorf( float );
_CRTIMP float  __cdecl fmodf( float , float );

_CRTIMP float  __cdecl _hypotf(float, float);

_CRTIMP float __cdecl _copysignf (float, float);
_CRTIMP float __cdecl _chgsignf (float);
_CRTIMP float __cdecl _scalbf(float, int);
_CRTIMP float __cdecl _logbf(float);
_CRTIMP float __cdecl _nextafterf(float, float);
_CRTIMP int    __cdecl _finitef(float);
_CRTIMP int    __cdecl _isnanf(float);
_CRTIMP int    __cdecl _fpclassf(float);

#endif /* _M_AMD64 */


/* Macros defining long double functions to be their double counterparts
 * (long double is synonymous with double in this implementation).
 */

#ifndef __cplusplus
#define acosl(x)    ((long double)acos((double)(x)))
#define asinl(x)    ((long double)asin((double)(x)))
#define atanl(x)    ((long double)atan((double)(x)))
#define atan2l(x,y) ((long double)atan2((double)(x), (double)(y)))
#define _cabsl      _cabs
#define ceill(x)    ((long double)ceil((double)(x)))
#define cosl(x)     ((long double)cos((double)(x)))
#define coshl(x)    ((long double)cosh((double)(x)))
#define expl(x)     ((long double)exp((double)(x)))
#define fabsl(x)    ((long double)fabs((double)(x)))
#define floorl(x)   ((long double)floor((double)(x)))
#define fmodl(x,y)  ((long double)fmod((double)(x), (double)(y)))
#define frexpl(x,y) ((long double)frexp((double)(x), (y)))
#define _hypotl(x,y)    ((long double)_hypot((double)(x), (double)(y)))
#define ldexpl(x,y) ((long double)ldexp((double)(x), (y)))
#define logl(x)     ((long double)log((double)(x)))
#define log10l(x)   ((long double)log10((double)(x)))
#define _matherrl   _matherr
#define modfl(x,y)  ((long double)modf((double)(x), (double *)(y)))
#define powl(x,y)   ((long double)pow((double)(x), (double)(y)))
#define sinl(x)     ((long double)sin((double)(x)))
#define sinhl(x)    ((long double)sinh((double)(x)))
#define sqrtl(x)    ((long double)sqrt((double)(x)))
#define tanl(x)     ((long double)tan((double)(x)))
#define tanhl(x)    ((long double)tanh((double)(x)))
#else   /* __cplusplus */
inline long double acosl(long double _X)
        {return (acos((double)_X)); }
inline long double asinl(long double _X)
        {return (asin((double)_X)); }
inline long double atanl(long double _X)
        {return (atan((double)_X)); }
inline long double atan2l(long double _X, long double _Y)
        {return (atan2((double)_X, (double)_Y)); }
inline long double ceill(long double _X)
        {return (ceil((double)_X)); }
inline long double cosl(long double _X)
        {return (cos((double)_X)); }
inline long double coshl(long double _X)
        {return (cosh((double)_X)); }
inline long double expl(long double _X)
        {return (exp((double)_X)); }
inline long double fabsl(long double _X)
        {return (fabs((double)_X)); }
inline long double floorl(long double _X)
        {return (floor((double)_X)); }
inline long double fmodl(long double _X, long double _Y)
        {return (fmod((double)_X, (double)_Y)); }
inline long double frexpl(long double _X, int *_Y)
        {return (frexp((double)_X, _Y)); }
inline long double ldexpl(long double _X, int _Y)
        {return (ldexp((double)_X, _Y)); }
inline long double logl(long double _X)
        {return (log((double)_X)); }
inline long double log10l(long double _X)
        {return (log10((double)_X)); }
inline long double modfl(long double _X, long double *_Y)
        {double _Di, _Df = modf((double)_X, &_Di);
        *_Y = (long double)_Di;
        return (_Df); }
inline long double powl(long double _X, long double _Y)
        {return (pow((double)_X, (double)_Y)); }
inline long double sinl(long double _X)
        {return (sin((double)_X)); }
inline long double sinhl(long double _X)
        {return (sinh((double)_X)); }
inline long double sqrtl(long double _X)
        {return (sqrt((double)_X)); }
inline long double tanl(long double _X)
        {return (tan((double)_X)); }
inline long double tanhl(long double _X)
        {return (tanh((double)_X)); }
inline float frexpf(float _X, int *_Y)
        {return ((float)frexp((double)_X, _Y)); }

#if     !defined(_M_IA64) 
inline float fabsf(float _X)
        {return ((float)fabs((double)_X)); }
inline float ldexpf(float _X, int _Y)
        {return ((float)ldexp((double)_X, _Y)); }
#if     !defined(_M_AMD64)
inline float tanhf(float _X)
        {return ((float)tanh((double)_X)); }
inline float acosf(float _X)
        {return ((float)acos((double)_X)); }
inline float asinf(float _X)
        {return ((float)asin((double)_X)); }
inline float atanf(float _X)
        {return ((float)atan((double)_X)); }
inline float atan2f(float _X, float _Y)
        {return ((float)atan2((double)_X, (double)_Y)); }
inline float ceilf(float _X)
        {return ((float)ceil((double)_X)); }
inline float cosf(float _X)
        {return ((float)cos((double)_X)); }
inline float coshf(float _X)
        {return ((float)cosh((double)_X)); }
inline float expf(float _X)
        {return ((float)exp((double)_X)); }
inline float floorf(float _X)
        {return ((float)floor((double)_X)); }
inline float fmodf(float _X, float _Y)
        {return ((float)fmod((double)_X, (double)_Y)); }
inline float logf(float _X)
        {return ((float)log((double)_X)); }
inline float log10f(float _X)
        {return ((float)log10((double)_X)); }
inline float modff(float _X, float *_Y)
        { double _Di, _Df = modf((double)_X, &_Di);
        *_Y = (float)_Di;
        return ((float)_Df); }
inline float powf(float _X, float _Y)
        {return ((float)pow((double)_X, (double)_Y)); }
inline float sinf(float _X)
        {return ((float)sin((double)_X)); }
inline float sinhf(float _X)
        {return ((float)sinh((double)_X)); }
inline float sqrtf(float _X)
        {return ((float)sqrt((double)_X)); }
inline float tanf(float _X)
        {return ((float)tan((double)_X)); }
#endif  /* !defined(_M_AMD64) */
#endif  /* !defined(_M_IA64) */
#endif  /* __cplusplus */
#endif  /* __assembler */

#if     !__STDC__

/* Non-ANSI names for compatibility */

#define DOMAIN      _DOMAIN
#define SING        _SING
#define OVERFLOW    _OVERFLOW
#define UNDERFLOW   _UNDERFLOW
#define TLOSS       _TLOSS
#define PLOSS       _PLOSS

#define matherr     _matherr

#ifndef __assembler /* Protect from assembler */

_CRTIMP extern double HUGE;

_CRTIMP double  __cdecl cabs(struct _complex);
_CRTIMP double  __cdecl hypot(double, double);
_CRTIMP double  __cdecl j0(double);
_CRTIMP double  __cdecl j1(double);
_CRTIMP double  __cdecl jn(int, double);
        int     __cdecl matherr(struct _exception *);
_CRTIMP double  __cdecl y0(double);
_CRTIMP double  __cdecl y1(double);
_CRTIMP double  __cdecl yn(int, double);

#endif  /* __assembler */

#endif  /* __STDC__ */

#ifdef  __cplusplus
}

extern "C++" {

template<class _Ty> inline
        _Ty _Pow_int(_Ty _X, int _Y)
        {unsigned int _N;
        if (_Y >= 0)
                _N = _Y;
        else
                _N = -_Y;
        for (_Ty _Z = _Ty(1); ; _X *= _X)
                {if ((_N & 1) != 0)
                        _Z *= _X;
                if ((_N >>= 1) == 0)
                        return (_Y < 0 ? _Ty(1) / _Z : _Z); }}

inline long __cdecl abs(long _X)
        {return (labs(_X)); }
inline double __cdecl abs(double _X)
        {return (fabs(_X)); }
inline double __cdecl pow(double _X, int _Y)
        {return (_Pow_int(_X, _Y)); }
inline double __cdecl pow(int _X, int _Y)
        {return (_Pow_int(_X, _Y)); }
inline float __cdecl abs(float _X)
        {return (fabsf(_X)); }
inline float __cdecl acos(float _X)
        {return (acosf(_X)); }
inline float __cdecl asin(float _X)
        {return (asinf(_X)); }
inline float __cdecl atan(float _X)
        {return (atanf(_X)); }
inline float __cdecl atan2(float _Y, float _X)
        {return (atan2f(_Y, _X)); }
inline float __cdecl ceil(float _X)
        {return (ceilf(_X)); }
inline float __cdecl cos(float _X)
        {return (cosf(_X)); }
inline float __cdecl cosh(float _X)
        {return (coshf(_X)); }
inline float __cdecl exp(float _X)
        {return (expf(_X)); }
inline float __cdecl fabs(float _X)
        {return (fabsf(_X)); }
inline float __cdecl floor(float _X)
        {return (floorf(_X)); }
inline float __cdecl fmod(float _X, float _Y)
        {return (fmodf(_X, _Y)); }
inline float __cdecl frexp(float _X, int * _Y)
        {return (frexpf(_X, _Y)); }
inline float __cdecl ldexp(float _X, int _Y)
        {return (ldexpf(_X, _Y)); }
inline float __cdecl log(float _X)
        {return (logf(_X)); }
inline float __cdecl log10(float _X)
        {return (log10f(_X)); }
inline float __cdecl modf(float _X, float * _Y)
        {return (modff(_X, _Y)); }
inline float __cdecl pow(float _X, float _Y)
        {return (powf(_X, _Y)); }
inline float __cdecl pow(float _X, int _Y)
        {return (_Pow_int(_X, _Y)); }
inline float __cdecl sin(float _X)
        {return (sinf(_X)); }
inline float __cdecl sinh(float _X)
        {return (sinhf(_X)); }
inline float __cdecl sqrt(float _X)
        {return (sqrtf(_X)); }
inline float __cdecl tan(float _X)
        {return (tanf(_X)); }
inline float __cdecl tanh(float _X)
        {return (tanhf(_X)); }
inline long double __cdecl abs(long double _X)
        {return (fabsl(_X)); }
inline long double __cdecl acos(long double _X)
        {return (acosl(_X)); }
inline long double __cdecl asin(long double _X)
        {return (asinl(_X)); }
inline long double __cdecl atan(long double _X)
        {return (atanl(_X)); }
inline long double __cdecl atan2(long double _Y, long double _X)
        {return (atan2l(_Y, _X)); }
inline long double __cdecl ceil(long double _X)
        {return (ceill(_X)); }
inline long double __cdecl cos(long double _X)
        {return (cosl(_X)); }
inline long double __cdecl cosh(long double _X)
        {return (coshl(_X)); }
inline long double __cdecl exp(long double _X)
        {return (expl(_X)); }
inline long double __cdecl fabs(long double _X)
        {return (fabsl(_X)); }
inline long double __cdecl floor(long double _X)
        {return (floorl(_X)); }
inline long double __cdecl fmod(long double _X, long double _Y)
        {return (fmodl(_X, _Y)); }
inline long double __cdecl frexp(long double _X, int * _Y)
        {return (frexpl(_X, _Y)); }
inline long double __cdecl ldexp(long double _X, int _Y)
        {return (ldexpl(_X, _Y)); }
inline long double __cdecl log(long double _X)
        {return (logl(_X)); }
inline long double __cdecl log10(long double _X)
        {return (log10l(_X)); }
inline long double __cdecl modf(long double _X, long double * _Y)
        {return (modfl(_X, _Y)); }
inline long double __cdecl pow(long double _X, long double _Y)
        {return (powl(_X, _Y)); }
inline long double __cdecl pow(long double _X, int _Y)
        {return (_Pow_int(_X, _Y)); }
inline long double __cdecl sin(long double _X)
        {return (sinl(_X)); }
inline long double __cdecl sinh(long double _X)
        {return (sinhl(_X)); }
inline long double __cdecl sqrt(long double _X)
        {return (sqrtl(_X)); }
inline long double __cdecl tan(long double _X)
        {return (tanl(_X)); }
inline long double __cdecl tanh(long double _X)
        {return (tanhl(_X)); }

}
#endif  /* __cplusplus */

#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif  /* _INC_MATH */

