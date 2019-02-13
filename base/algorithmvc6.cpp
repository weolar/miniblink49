
#if USING_VC6RT == 1

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <limits>
#include <limits.h>
#include <windows.h>

extern "C" double fmod(double _Xx, double _Yx);

#if !defined(_WIN64)
void __cdecl operator delete(void* pv, const std::nothrow_t&)
{
    ::free(pv);
}
#endif

namespace std {

int fpclassify(double x)
{
    union { double d; unsigned __int64 u; }u = { x };

    unsigned __int32 exp = (unsigned __int32)((u.u & 0x7fffffffffffffffULL) >> 52);

    if (0 == exp) {
        if (u.u & 0x000fffffffffffffULL)
            return FP_SUBNORMAL;

        return FP_ZERO;
    }

    if (0x7ff == exp) {
        if (u.u & 0x000fffffffffffffULL)
            return FP_NAN;

        return FP_INFINITE;
    }

    return FP_NORMAL;
}

int fpclassify(float x)
{
    return fpclassify((double)x);
}

float fmod(float _Xx, float _Yx)
{
    return ::fmod((double)_Xx, (double)_Yx);
}

double fmod(double _Xx, double _Yx)
{
    return ::fmod(_Xx, _Yx);
}

double asin(double x)
{
    return ::asin(x);
}

bool isFloatIEEE754Negative(float f)
{
    float d = f;
    if (sizeof(float) == sizeof(unsigned short int)) {
        return (*(unsigned short int *)(&d) >> (sizeof(unsigned short int)*CHAR_BIT - 1) == 1);
    }
    else if (sizeof(float) == sizeof(unsigned int)) {
        return (*(unsigned int *)(&d) >> (sizeof(unsigned int)*CHAR_BIT - 1) == 1);
    }
    else if (sizeof(float) == sizeof(unsigned long)) {
        return (*(unsigned long *)(&d) >> (sizeof(unsigned long)*CHAR_BIT - 1) == 1);
    }
    else if (sizeof(float) == sizeof(unsigned char)) {
        return (*(unsigned char *)(&d) >> (sizeof(unsigned char)*CHAR_BIT - 1) == 1);
    }
    else if (sizeof(float) == sizeof(unsigned long long)) {
        return (*(unsigned long long *)(&d) >> (sizeof(unsigned long long)*CHAR_BIT - 1) == 1);
    }
    return false; // Should never get here if you've covered all the potential types!
}

bool signbit(float x)
{
    return isFloatIEEE754Negative(x);
}

int _dsign(double x)
{
    enum { double_per_long = sizeof(double) / sizeof(long) };
    enum { long_msb = sizeof(long) * CHAR_BIT - 1 };
    union { double d; unsigned long i[double_per_long]; } u;
    unsigned long l;

    u.d = x;
#ifdef WORDS_BIGENDIAN
    l = u.i[0];
#else
    l = u.i[double_per_long - 1];
#endif
    return (int)(l >> long_msb);
}

bool signbit(double x)
{
    return _dsign(x) != 0;
}

double atan(double x)
{
    return ::atan(x);
}

double atan2(double x, double y)
{
    return ::atan2(x, y);
}

bool isless(double x, double y)
{
    if (!signbit(x) && signbit(y)) // + -
        return false;
    if (signbit(x) && !signbit(y)) // - +
        return true;
    return x < y;
}

double exp(double x)
{
    return ::exp(x);
}

int isinf(double d)
{
    return fpclassify(d) == FP_INFINITE;
}

int isnan(double d)
{
    return fpclassify(d) == FP_NAN;
}

double log(double val)
{
    return ::log(val);
}

float pow(float base, int exp) { return ::pow(base, exp); }
float pow(float base, float exp) { return ::pow(base, exp); }
double pow(int base, int exp) { return ::pow(base, exp); }
double pow(double base, int exp) { return ::pow(base, exp); }
double pow(double base, double exp) { return ::pow(base, exp); }

double pow(int base, double exp)
{
    return ::pow((double)base, exp);
}

double __cdecl copysign(double x, double y)
{
    union { double f; unsigned __int64 u; } ux, uy;

    ux.f = x;
    uy.f = y;

    ux.u &= 0x7fffffffffffffffULL;
    ux.u |= uy.u & 0x8000000000000000ULL;

    return ux.f;
}

__int64 abs(__int64 val)
{
    return (val > 0 ? val : -val);
}

unsigned __int64 abs(unsigned __int64 val)
{
    return val;
}

int abs(int val)
{
    return (val > 0 ? val : -val);
}

unsigned int abs(unsigned int val)
{
    return val;
}

double abs(double val) {
    return (val > 0 ? val : -val);
}

float abs(float val) {
    return (val > 0 ? val : -val);
}


double fabs(double val)
{
    return (val > 0 ? val : -val);
}

double floor(double x)
{
    return ::floor(x);
}

float ceil(float x)
{
    return ::ceil((double)x);
}

double ceil(double x)
{
    return ::ceil(x);
}

double sqrt(double val)
{
    return ::sqrt(val);
}

double acos(double val)
{
    return ::acos(val);
}

double sin(double val)
{
    return ::sin(val);
}

double cos(double val)
{
    return ::cos(val);
}

double tan(double val)
{
    return ::tan(val);
}

double ldexp(double x, int exponent)
{
    return ::ldexp(x, exponent);
}

bool isfinite(__int64 arg)
{
    return /*arg == arg &&*/ arg != std::numeric_limits<__int64>::infinity() && arg != -std::numeric_limits<__int64>::infinity();
}

bool isfinite(int arg)
{
    return /*arg == arg &&*/ arg != std::numeric_limits<int>::infinity() && arg != -std::numeric_limits<int>::infinity();
}

bool isfinite(unsigned int arg)
{
    return /*arg == arg &&*/ arg != std::numeric_limits<int>::infinity() && arg != -std::numeric_limits<int>::infinity();
}

bool isfinite(double arg)
{
    return arg == arg && arg != std::numeric_limits<double>::infinity() && arg != -std::numeric_limits<double>::infinity();
}

double nearbyint(double x)
{
  return ::nearbyint(x);
}

template<>
bool __cdecl operator != <char, char_traits<char>, allocator<char> >(basic_string<char, char_traits<char>, allocator<char> > const & a, char const * b)
{
    string bCopy(b);
    return a != bCopy;
}

template<>
basic_string<char, char_traits<char>, allocator<char> > __cdecl operator + <char, char_traits<char>, allocator<char> >(
    char const * a, basic_string<char, char_traits<char>, allocator<char> > const & b)
{
    string aCopy(a);
    aCopy += b;
    return aCopy;
}

template<>
basic_string<char, char_traits<char>, allocator<char> > __cdecl operator + <char, char_traits<char>, allocator<char> >(
    basic_string<char, char_traits<char>, allocator<char> > const & a, 
    basic_string<char, char_traits<char>, allocator<char> > const & b)
{
    string result(a);
    result.append(b);
    return result;
}

template<>
bool __cdecl operator < <wchar_t, char_traits<wchar_t>, allocator<wchar_t> > (
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & a,
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & b)
{
    return (a.compare(b) < 0);
}

template<>
basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator + 
<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
    wchar_t const * a, 
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & b)
{
    wstring aCopy(a);
    aCopy.append(b);
    return aCopy;
}

template<>
bool __cdecl operator == <wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & a, 
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & b)
{
    return (a.compare(b) == 0);
}

template<>
bool __cdecl operator == <wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
    basic_string<wchar_t, std::char_traits<wchar_t>, allocator<wchar_t> > const & a, wchar_t const * b)
{
    wstring bCopy(b);
    return a.compare(bCopy) == 0;
}

template<>
bool __cdecl operator < <char, char_traits<char>, allocator<char> >(
    basic_string<char, char_traits<char>, allocator<char> > const & a,
    basic_string<char, char_traits<char>, allocator<char> > const & b)
{
    return a.compare(b) < 0;
}

template<>
bool __cdecl operator != <wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & a, 
    wchar_t const * b)
{
    return a.compare(b) != 0;
}

template<>
bool __cdecl operator == <char, char_traits<char>, allocator<char> >(
    basic_string<char, char_traits<char>, allocator<char> > const & a, 
    basic_string<char, char_traits<char>, allocator<char> > const & b)
{
    return a.compare(b) == 0;
}

template<>
bool __cdecl operator != <char, char_traits<char>, allocator<char> >(
    basic_string<char, char_traits<char>, allocator<char> > const & a,
    basic_string<char, char_traits<char>, allocator<char> > const & b)
{
    return a.compare(b) != 0;
}

template<>
bool __cdecl operator==<char, char_traits<char>, allocator<char> >(
    basic_string<char, char_traits<char>, allocator<char> > const & a, 
    char const * b)
{
    return a.compare(b) == 0;
}

template<>
bool __cdecl operator==<char, char_traits<char>, allocator<char> >(
	char const * a,
	basic_string<char, char_traits<char>, allocator<char> > const & b) 
{
	return b.compare(a) == 0;
}

template<>
bool __cdecl operator!=<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & a,
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & b)
{
    return a.compare(b) != 0;
}

template<>
basic_string<wchar_t, struct std::char_traits<wchar_t>, class std::allocator<wchar_t> > __cdecl 
    operator+<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & a,
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & b)
{
    wstring aCopy(a);
    aCopy.append(b);
    return aCopy;
}

template<>
basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl 
    operator+<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > const & a, wchar_t const * b)
{
    wstring aCopy(a);
    aCopy.append(b);
    return aCopy;
}

template<>
basic_string<char, char_traits<char>, class std::allocator<char> > __cdecl operator + <char, char_traits<char>, class std::allocator<char> >(
    basic_string<char, char_traits<char>, allocator<char> > const & a, char const * b)
{
    string aCopy(a);
    aCopy.append(b);
    return aCopy;
}

// template<>
// void std::basic_string<char, std::char_traits<char>, std::allocator<char> >::clear(void)
// {
//     _Eos(0);
// }

} // std

//////////////////////////////////////////////////////////////////////////

double copysign(double x, double y)
{
    return std::copysign(x, y);
}

float copysignf(float number, float sign)
{
    return std::copysign((double)number, (double)sign);
}

double rint(double x)
{
    union { double d; unsigned __int64 u; }u = { x };
    unsigned __int64 absux = u.u & 0x7fffffffffffffffULL;

    //special case code for large int, Inf, NaN, 0
    if (absux - 1LL >= 0x4330000000000000ULL - 1LL)
        return x;

    u.u = (u.u & 0x8000000000000000ULL) | 0x4330000000000000ULL;    //copysign( 0x1.0p23f, x )

    x += u.d;
    x -= u.d;

    return x;
}

double nearbyint(double x)
{
    return rint(x);
}

double droundd(double val, int places);

long lround(long val)
{
    return (long)droundd((double)val, 4);
}

long lroundf(float val)
{
    return (long)droundd(val, 4);
}

long roundf(float val)
{
    return droundd(val, 4);
}

double round(double val)
{
    return droundd(val, 4);
}

long lrint(double x)
{
    double rounded = round(x);
    long result = (long)rounded;

    if (abs(result - x) != 0.5)
        return result;
    else {
        // Exact halfway case
        if (result & 1L) {
            // If the trailing bit is set, we rounded the wrong way
            long step = (result >> 30) | 1L;// x < 0 ? -1 : 1
            return result - step;            // x < 0 ? result + 1 : result - 1
        }
        else {
            return result;
        }
    }
}

float nextafterf(float x, float y)
{
    union { float f; unsigned int u; } ux = { x };
    unsigned int step = 1;

    if (y != y || x != x)
        return x + y;

    if (y <= x) { // a subtraction here would risk Inf-Inf
        if (y == x)
            return y;    //make sure sign of 0 is correct
        step = -1;
    }

    //correct for the sign
    int signMask = (int)ux.u >> 31;
    step = (step ^ signMask) - signMask;

    unsigned int absux = ux.u & 0x7fffffffU;

    if (absux == 0) { // zero
        ux.f = y;
        ux.u = (ux.u & 0x80000000U) + 1U;
        return ux.f;
    }

    ux.u += step;
    return ux.f;
}

double nextafter(double x, double y)
{
    union { double d; unsigned __int64 u; } ux = { x };
    unsigned __int64    step = 1;

    if (y != y || x != x)
        return x + y;

    if (y <= x) { // a subtraction here would risk Inf-Inf
        if (y == x)
            return y;    //make sure sign of 0 is correct
        step = -1LL;
    }

    //correct for the sign
    __int64 signMask = (__int64)ux.u >> 63;
    step = (step ^ signMask) - signMask;

    unsigned __int64 absux = ux.u & 0x7fffffffffffffffULL;

    if (absux == 0ULL) { // zero
        ux.d = y;
        ux.u = (ux.u & 0x8000000000000000ULL) + 1U;
        return ux.d;
    }

    ux.u += step;
    return ux.d;
}

typedef union { unsigned __int64 u; double d; } du;

float hypotf(float x, float y)
{
    //return _hypotf(x, y);
    static const double inf = 0xffffff;// inf();
    du u[3];
    du *largeVal = u;
    du *smallVal = &u[1];

    u[0].d = fabs(x);
    u[1].d = fabs(y);

    // handle inf / NaN
    if (0x7ff0000000000000ULL == (u[0].u & 0x7ff0000000000000ULL) ||
        0x7ff0000000000000ULL == (u[1].u & 0x7ff0000000000000ULL)) {
        if (0x7ff0000000000000ULL == u[0].u || 0x7ff0000000000000ULL == u[1].u)
            return inf;

        return x + y;        // NaN
    }

    if (x == 0.0 || y == 0.0)
        return fabs(x + y);

    //fix pointers to large and small if necessary
    if (u[0].d < u[1].d) {
        largeVal = &u[1];
        smallVal = &u[0];
    }

    //break values up into exponent and mantissa
    __int64 largeExp = largeVal->u >> 52;
    __int64 smallExp = smallVal->u >> 52;
    __int64 diff = largeExp - smallExp;
    if (diff >= 55L)
        return largeVal->d + smallVal->d;

    largeVal->u &= 0x000fffffffffffffULL;
    smallVal->u &= 0x000fffffffffffffULL;
    largeVal->u |= 0x3ff0000000000000ULL;
    smallVal->u |= 0x3ff0000000000000ULL;

    //fix up denormals
    if (0 == smallExp) {
        if (0 == largeExp) {
            largeVal->d -= 1.0;
            largeExp = (largeVal->u >> 52) - (1022);
            largeVal->u &= 0x000fffffffffffffULL;
            largeVal->u |= 0x3ff0000000000000ULL;
        }
        smallVal->d -= 1.0;
        smallExp = (smallVal->u >> 52) - (1022);
        smallVal->u &= 0x000fffffffffffffULL;
        smallVal->u |= 0x3ff0000000000000ULL;
    }

    u[2].u = (1023ULL - largeExp + smallExp) << 52;
    smallVal->d *= u[2].d;

    double r = sqrt(largeVal->d * largeVal->d + smallVal->d * smallVal->d);

    if (largeExp < 0) {
        largeExp += 1022;
        //r *= 0x1.0p - 1022;
        r *= 1.0 - 1022;
    }

    u[2].u = largeExp << 52;
    r *= u[2].d;

    return r;
}

double droundd(double val, int places) {
    double t;
    double f = pow(10.0, (double)places);
    double x = val * f;

    if (std::isinf(x) || std::isnan(x)) {
        return val;
    }

    if (x >= 0.0) {
        t = ceil(x);
        if ((t - x) > 0.50000000001) {
            t -= 1.0;
        }
    }
    else {
        t = ceil(-x);
        if ((t + x) > 0.50000000001) {
            t -= 1.0;
        }
        t = -t;
    }
    x = t / f;

    return !std::isnan(x) ? x : t;
}

double trunc(double val)
{
    //double absVal = (val > 0 ? val : -val);
    double result = floor(val);
    if (val < 0)
        result = -result;
    return result;
}

float truncf(float val)
{
    return trunc((double)val);
}

// extern "C" void _ultod3()
// {
//     DebugBreak();
// }
// 
// extern "C" void _dtol3()
// {
//     DebugBreak();
// }
// 
// extern "C" void _ltod3()
// {
//     DebugBreak();
// }
// 

#undef and
#undef xor

extern "C" __declspec(naked) __int64 _ftol2_sse(double v)
{
    __asm {
        push        ebp
            mov         ebp, esp
            sub         esp, 20h
            and         esp, 0FFFFFFF0h
            fld         st(0)
            fst         dword ptr[esp + 18h]
            fistp       qword ptr[esp + 10h]
            fild        qword ptr[esp + 10h]
            mov         edx, dword ptr[esp + 18h]
            mov         eax, dword ptr[esp + 10h]
            test        eax, eax
            je          integer_QnaN_or_zero
            arg_is_not_integer_QnaN :
        fsubp       st(1), st
            test        edx, edx
            jns         positive
            fstp        dword ptr[esp]
            mov         ecx, dword ptr[esp]
            xor ecx, 80000000h
            add         ecx, 7FFFFFFFh
            adc         eax, 0
            mov         edx, dword ptr[esp + 14h]
            adc         edx, 0
            jmp         localexit

            positive :
        fstp        dword ptr[esp]
            mov         ecx, dword ptr[esp]
            add         ecx, 7FFFFFFFh
            sbb         eax, 0
            mov         edx, dword ptr[esp + 14h]
            sbb         edx, 0
            jmp         localexit
            integer_QnaN_or_zero :
        mov         edx, dword ptr[esp + 14h]
            test        edx, 7FFFFFFFh
            jne         arg_is_not_integer_QnaN
            fstp        dword ptr[esp + 18h]
            fstp        dword ptr[esp + 18h]
            localexit :
            leave
            ret
    }
}
// 
// extern "C" void _dtoui3()
// {
//     DebugBreak();
// }
// 
// extern "C" void _ftol3()
// {
//     DebugBreak();
// }
// 
// extern "C" void _dtoul3()
// {
//     DebugBreak();
// }
// 
// extern "C" void _ftoui3()
// {
//     DebugBreak();
// }
// 
// extern "C" void __vdecl_floor2()
// {
//     DebugBreak();
// }
// 
// extern "C" void __vdecl_pow2()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_acos_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_cos_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_log_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_pow_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_sin_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_sqrt_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_asin_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_atan_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_exp_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_tan_precise()
// {
//     DebugBreak();
// }
// 
// extern "C" void _libm_sse2_log10_precise()
// {
//     DebugBreak();
// }

#ifdef __clang__

extern "C" unsigned long  __cdecl _byteswap_ulong(unsigned long i)
{
    unsigned int j;
    j = (i << 24);
    j += (i << 8) & 0x00FF0000;
    j += (i >> 8) & 0x0000FF00;
    j += (i >> 24);
    return j;
}

#endif

extern "C" void* _alloca_probe_16(size_t length)
{
    DebugBreak();
    return nullptr;
}

extern "C" void __report_rangecheckfailure()
{
    DebugBreak();
}

extern "C" void __stdcall _imp__InitOnceExecuteOnce(int a1, int a2, int a3, int a4)
{
    DebugBreak();
}

extern "C" void __std_terminate()
{
    DebugBreak();
}

extern "C" int vsnprintf(char* const buffer, size_t const count, char const* const format, va_list args)
{
    return _vsnprintf(buffer, count, format, args);
}

extern "C" int snprintf(char* buffer, size_t count, const char* format, ...)
{
    int result;
    va_list args;
    va_start(args, format);
    result = _vsnprintf(buffer, count, format, args);
    va_end(args);

    // In the case where the string entirely filled the buffer, _vsnprintf will not
    // null-terminate it, but snprintf must.
    if (count > 0)
        buffer[count - 1] = '\0';

    return result;
}

extern __declspec(dllimport) double _HUGE;

float nearbyintf(float x) {
    return 0;
}

double __cdecl scalbn(double _X, int _Y) {
    return 0;
}
#endif // USING_VC6RT
