
#ifndef __string_vc6__
#define __string_vc6__

_STD_BEGIN

template<class _Ty> inline
string _Integral_to_string(const char *_Fmt, _Ty _Val)
{
  static_assert(is_integral<_Ty>::value, "_Ty must be integral");
  char _Buf[21];
  int _Len = sprintf(_Buf, _Fmt, _Val);
  return (string(_Buf, _Len));
}

template<class _Ty> inline
string _Floating_to_string(const char *_Fmt, _Ty _Val)
{
  static_assert(is_floating_point<_Ty>::value, "_Ty must be floating point");

  char _Buf[51];
  int _Len = sprintf(_Buf, _Fmt, _Val);
  return (string(_Buf, _Len));
}

// to_string NARROW CONVERSIONS
inline string to_string(int _Val)
{	// convert int to string
  return (_Integral_to_string("%d", _Val));
}

inline string to_string(unsigned int _Val)
{	// convert unsigned int to string
  return (_Integral_to_string("%u", _Val));
}

inline string to_string(long _Val)
{	// convert long to string
  return (_Integral_to_string("%ld", _Val));
}

inline string to_string(unsigned long _Val)
{	// convert unsigned long to string
  return (_Integral_to_string("%lu", _Val));
}

inline string to_string(long long _Val)
{	// convert long long to string
  return (_Integral_to_string("%lld", _Val));
}

inline string to_string(unsigned long long _Val)
{	// convert unsigned long long to string
  return (_Integral_to_string("%llu", _Val));
}

inline string to_string(float _Val)
{	// convert float to string
  return (_Floating_to_string("%f", _Val));
}

inline string to_string(double _Val)
{	// convert double to string
  return (_Floating_to_string("%f", _Val));
}

inline string to_string(long double _Val)
{	// convert long double to string
  return (_Floating_to_string("%Lf", _Val));
}

_STD_END

#endif // __string_vc6__