/* intrinvc6.h values header for Microsoft VC6 C/C++  */

#ifndef intrinvc6_h
#define intrinvc6_h

#if defined(_M_IX86)

//void /*__stdcall*/ _ReadWriteBarrier(void);
//#define _Compiler_barrier()	_ReadWriteBarrier()
#define _Compiler_barrier() 

// #pragma push_macro("_InterlockedExchange64")
// #pragma push_macro("_InterlockedExchangeAdd64")
// #pragma push_macro("_InterlockedAnd64")
// #pragma push_macro("_InterlockedOr64")
// #pragma push_macro("_InterlockedXor64")
// 
// #undef _InterlockedExchange64
// #undef _InterlockedExchangeAdd64
// #undef _InterlockedAnd64
// #undef _InterlockedOr64
// #undef _InterlockedXor64
// 
// #define _InterlockedExchange64		_InterlockedExchange64_INLINE
// #define _InterlockedExchangeAdd64	_InterlockedExchangeAdd64_INLINE
// #define _InterlockedAnd64			_InterlockedAnd64_INLINE
// #define _InterlockedOr64			_InterlockedOr64_INLINE
// #define _InterlockedXor64			_InterlockedXor64_INLINE

typedef unsigned long _Uint32t;
typedef _Uint32t _Uint4_t;
typedef _Uint4_t _Atomic_integral_t;
typedef long _Atomic_flag_t;

#define  _LONGLONG __int64
#define _ULONGLONG unsigned __int64

#define  LONGLONG __int64
#define ULONGLONG unsigned __int64

#define _ATOMIC_MAXBYTES_LOCK_FREE	8
#define _ATOMIC_FLAG_USES_LOCK		0
#define _ATOMIC_FENCE_USES_LOCK		0

//////////////////////////////////////////////////////////////////////////
#define _InterlockedCompareExchange64		InterlockedCompareExchange64

#define _InterlockedExchange8 InterlockedExchange8
#define _InterlockedCompareExchange8 InterlockedCompareExchange8
#define _InterlockedExchangeAdd8 InterlockedExchangeAdd8
#define _InterlockedAnd8 InterlockedAnd8
#define _InterlockedOr8 InterlockedOr8
#define _InterlockedXor8 InterlockedXor8

#define _InterlockedExchange16 InterlockedExchange16
#define _InterlockedCompareExchange16 InterlockedCompareExchange16
#define _InterlockedExchangeAdd16 InterlockedExchangeAdd16
#define _InterlockedAnd16 InterlockedAnd16
#define _InterlockedOr16 InterlockedOr16
#define _InterlockedXor16 InterlockedXor16

#define _InterlockedExchange InterlockedExchange
#define _InterlockedCompareExchange InterlockedCompareExchange
#define _InterlockedExchangeAdd InterlockedExchangeAdd
#define _InterlockedAnd InterlockedAnd
#define _InterlockedOr InterlockedOr
#define _InterlockedXor InterlockedXor

extern "C" {

    WINBASEAPI __int64 WINAPI InterlockedCompareExchange64(__int64 volatile *Destination, __int64 ExChange, __int64 Comperand);

    char WINAPI InterlockedExchange8(char volatile * _Target, char _Value);
    char WINAPI InterlockedCompareExchange8(char volatile * _Destination, char _Exchange, char _Comparand);
    char WINAPI InterlockedExchangeAdd8(char volatile * _Addend, char _Value);
    char WINAPI InterlockedAnd8(char volatile * _Value, char _Mask);
    char WINAPI InterlockedOr8(char volatile * _Value, char _Mask);
    char WINAPI InterlockedXor8(char volatile * _Value, char _Mask);

    short WINAPI InterlockedExchange16(short volatile * _Target, short _Value);
    short WINAPI InterlockedCompareExchange16(short volatile * _Destination, short _Exchange, short _Comparand);
    short WINAPI InterlockedExchangeAdd16(short volatile * _Addend, short _Value);
    short WINAPI InterlockedAnd16(short volatile * _Value, short _Mask);
    short WINAPI InterlockedOr16(short volatile * _Value, short _Mask);
    short WINAPI InterlockedXor16(short volatile * _Value, short _Mask);

    long WINAPI InterlockedAnd(long volatile * _Value, long _Mask);
    long WINAPI InterlockedOr(long volatile * _Value, long _Mask);
    long WINAPI InterlockedXor(long volatile * _Value, long _Mask);

}

// #pragma intrinsic(InterlockedCompareExchange64)
// 
// #pragma intrinsic(InterlockedExchange8)
// #pragma intrinsic(InterlockedCompareExchange8)
// #pragma intrinsic(InterlockedExchangeAdd8)
// #pragma intrinsic(InterlockedOr8)
// #pragma intrinsic(InterlockedXor8)
// 
// #pragma intrinsic(InterlockedExchange16)
// #pragma intrinsic(InterlockedCompareExchange16)
// #pragma intrinsic(InterlockedExchangeAdd16)
// #pragma intrinsic(InterlockedOr16)
// #pragma intrinsic(InterlockedXor16)
//////////////////////////////////////////////////////////////////////////

inline _LONGLONG _InterlockedExchange64(volatile _LONGLONG *_Tgt, _LONGLONG _Value)
{
    _LONGLONG _Oldval;
    _Compiler_barrier();
    do {
        _Oldval = *_Tgt;
    } while (_Oldval != _InterlockedCompareExchange64(_Tgt, _Value, _Oldval));
    _Compiler_barrier();

    return (_Oldval);
}

inline _LONGLONG _InterlockedExchangeAdd64(volatile _LONGLONG *_Tgt, _LONGLONG _Value)
{
    _LONGLONG _Oldval, _Newval;
    _Compiler_barrier();
    do {
        _Oldval = *_Tgt;
        _Newval = _Oldval + _Value;
    } while (_Oldval != _InterlockedCompareExchange64(_Tgt, _Newval, _Oldval));
    _Compiler_barrier();

    return (_Oldval);
}

inline _LONGLONG _InterlockedAnd64(volatile _LONGLONG *_Tgt, _LONGLONG _Value)
{
    _LONGLONG _Oldval, _Newval;
    _Compiler_barrier();
    do {
        _Oldval = *_Tgt;
        _Newval = _Oldval & _Value;
    } while (_Oldval != _InterlockedCompareExchange64(_Tgt, _Newval, _Oldval));
    _Compiler_barrier();

    return (_Oldval);
}

inline _LONGLONG _InterlockedOr64(volatile _LONGLONG *_Tgt, _LONGLONG _Value)
{
    _LONGLONG _Oldval, _Newval;
    _Compiler_barrier();
    do {
        _Oldval = *_Tgt;
        _Newval = _Oldval | _Value;
    } while (_Oldval != _InterlockedCompareExchange64(_Tgt, _Newval, _Oldval));
    _Compiler_barrier();

    return (_Oldval);
}

inline _LONGLONG _InterlockedXor64(volatile _LONGLONG *_Tgt, _LONGLONG _Value)
{
    _LONGLONG _Oldval, _Newval;
    _Compiler_barrier();
    do {
        _Oldval = *_Tgt;
        _Newval = _Oldval ^ _Value;
    } while (_Oldval != _InterlockedCompareExchange64(_Tgt, _Newval, _Oldval));
    _Compiler_barrier();

    return (_Oldval);
}
#endif /* defined(_M_IX86) */

#endif // intrinvc6_h