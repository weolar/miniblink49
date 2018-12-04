#ifndef base_algorithmvc6_h
#define base_algorithmvc6_h

#if USING_VC6RT == 1

#include <math.h>

#undef min
#undef max

#define _INFCODE   1
#define _NANCODE   2

#define _DENORM    (-2)

#define FP_INFINITE  _INFCODE
#define FP_NAN       _NANCODE
#define FP_NORMAL    (-1)
#define FP_SUBNORMAL _DENORM
#define FP_ZERO      0

namespace std {

#ifdef _WIN64
    typedef __int64             ptrdiff_t;
#else
    typedef int            ptrdiff_t;
#endif

template<class _Ty> inline const _Ty min(const _Ty& _Left, const _Ty& _Right)
{
    return ((_Right < _Left) ? _Right : _Left);
}

template<class _Ty> inline const _Ty max(const _Ty& _Left, const _Ty& _Right)
{
    return ((_Right > _Left) ? _Right : _Left);
}

inline const float max(const float& _Left, const long& _Right)
{
    return ((_Right > _Left) ? _Right : _Left);
}

template<> inline const float max(const float& _Left, const float& _Right)
{
    return ((_Right > _Left) ? _Right : _Left);
}

template<> inline const float min(const float& _Left, const float& _Right)
{
    return ((_Right < _Left) ? _Right : _Left);
}

double log(double val);

float pow(float base, int exp);
float pow(float base, float exp);
double pow(int base, int exp);
double pow(int base, double exp);
double pow(double base, int exp);
double pow(double base, double exp);

__int64 abs(__int64 val);
unsigned __int64 abs(unsigned __int64 val);
int abs(int val);
unsigned int abs(unsigned int val);
double abs(double val);
float abs(float val);

__int64 abs(__int64 val);
double fabs(double val);
double floor(double x);
float ceil(float x);
double ceil(double x);

//////////////////////////////////////////////////////////////////////////

double ldexp(double x, int exponent);

float fmod(float _Xx, float _Yx);
double fmod(double _Xx, double _Yx);

bool signbit(float x);
bool signbit(double x);

bool isfinite(__int64 arg);
bool isfinite(int arg);
bool isfinite(unsigned int arg);
bool isfinite(double arg);

int fpclassify(float x);
int fpclassify(double x);

double asin(double x);
double atan(double x);
double atan2(double x, double y);

bool isless(double x, double y);

double exp(double x);

//////////////////////////////////////////////////////////////////////////

int isinf(double d);
int isnan(double d);

template <class _Ty>
inline bool isnormal(_Ty _X)
{
    return fpclassify(_X) == FP_NORMAL;
}

double sqrt(double val);
double acos(double val);
double sin(double val);
double cos(double val);
double tan(double val);

//void* memcpy(void* _Dst, void const* _Src, size_t _Size);
double nearbyint(double x);

} // namespace std

//////////////////////////////////////////////////////////////////////////

double rint(double x);
double nearbyint(double x);

long lrint(double x);

double copysign(double number, double sign);
float copysignf(float number, float sign);

double trunc(double val);
float truncf(float val);

long lroundf(float val);

long roundf(float val);

long lround(long val);

double round(double val);

float nextafterf(float x, float y);
double nextafter(double x, double y);

float nearbyintf(float x);
long double nearbyintl(long double x);

float hypotf(float x, float y);

#endif // USING_VC6RT

#endif // base_algorithmvc6_h