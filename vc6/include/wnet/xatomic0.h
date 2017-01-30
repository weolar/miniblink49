/* xatomic0.h internal header */

#pragma once
#ifndef _XATOMIC0_H
#define _XATOMIC0_H
#ifndef RC_INVOKED
#include <yvals.h>

//#include <atomic>

 #pragma pack(push,_CRT_PACKING)
 #pragma warning(push,3)
 #pragma push_macro("new")
 #undef new

_STD_BEGIN


		/* ENUM memory_order */
typedef enum _memory_order {
	memory_order_relaxed,
	memory_order_consume,
	memory_order_acquire,
	memory_order_release,
	memory_order_acq_rel,
	memory_order_seq_cst
	} memory_order;

typedef unsigned long _Uint32t;
typedef _Uint32t _Uint4_t;
typedef _Uint4_t _Atomic_integral_t;

	/* SET SIZES AND FLAGS FOR COMPILER AND TARGET ARCHITECTURE */
	/* Note: the xxx_SIZE macros are used to generate function names,
		so they must expand to the digits representing
		the number of bytes in the type; they cannot be expressions
		that give the number of bytes. */

  #define _WCHAR_T_SIZE		2
  #define _SHORT_SIZE		2
  #define _INT_SIZE			4
  #define _LONG_SIZE		4
  #define _LONGLONG_SIZE	8

  #if defined(_WIN64)
   #define _ADDR_SIZE	8

  #else /* defined(_WIN64) */
   #define _ADDR_SIZE	4
  #endif /* defined(_WIN64) */

		/* TYPEDEF _Atomic_flag_t */

typedef long _Atomic_flag_t;

  #define _ATOMIC_MAXBYTES_LOCK_FREE	8
  #define _ATOMIC_FLAG_USES_LOCK		0
  #define _ATOMIC_FENCE_USES_LOCK		0

		/* DECLARATIONS NEEDED FOR ATOMIC REFERENCE COUNTING */
inline _Uint4_t _Atomic_load_4(volatile _Uint4_t *, memory_order);
inline int _Atomic_compare_exchange_weak_4(
	volatile _Uint4_t *, _Uint4_t *, _Uint4_t, memory_order, memory_order);
inline _Uint4_t _Atomic_fetch_add_4(
	volatile _Uint4_t *, _Uint4_t, memory_order);
inline _Uint4_t _Atomic_fetch_sub_4(
	volatile _Uint4_t *, _Uint4_t, memory_order);

typedef _Atomic_integral_t _Atomic_counter_t;

  #if defined(__cplusplus)
inline _Atomic_integral_t
	_Get_atomic_count(const _Atomic_counter_t& _Counter)
	{	// get counter
	return (_Counter);
	}

inline void _Init_atomic_counter(_Atomic_counter_t& _Counter,
	_Atomic_integral_t _Value)
	{	// non-atomically initialize counter
	_Counter = _Value;
	}

inline _Atomic_integral_t _Inc_atomic_counter_explicit(
	_Atomic_counter_t& _Counter, memory_order _Order)
	{	// atomically increment counter and return result
	return (_Atomic_fetch_add_4(&_Counter, 1, _Order) + 1);
	}

inline _Atomic_integral_t _Inc_atomic_counter(_Atomic_counter_t& _Counter)
	{	// atomically increment counter and return result
	return (_Inc_atomic_counter_explicit(_Counter, memory_order_seq_cst));
	}

inline _Atomic_integral_t _Dec_atomic_counter_explicit(
	_Atomic_counter_t& _Counter, memory_order _Order)
	{	// atomically decrement counter and return result
	return (_Atomic_fetch_sub_4(&_Counter, 1, _Order) - 1);
	}

inline _Atomic_integral_t _Dec_atomic_counter(_Atomic_counter_t& _Counter)
	{	// atomically decrement counter and return result
	return (_Dec_atomic_counter_explicit(_Counter, memory_order_seq_cst));
	}

inline _Atomic_integral_t _Load_atomic_counter_explicit(
	_Atomic_counter_t& _Counter, memory_order _Order)
	{	// atomically load counter and return result
	return (_Atomic_load_4(&_Counter, _Order));
	}

inline _Atomic_integral_t _Load_atomic_counter(_Atomic_counter_t& _Counter)
	{	// atomically load counter and return result
	return (_Load_atomic_counter_explicit(_Counter, memory_order_seq_cst));
	}

inline _Atomic_integral_t _Compare_increment_atomic_counter_explicit(
	_Atomic_counter_t& _Counter,
	_Atomic_integral_t _Expected,
	memory_order _Order)
	{	// atomically compare and increment counter and return result
	return (_Atomic_compare_exchange_weak_4(
		&_Counter, &_Expected, _Expected + 1,
		_Order, _Order));
	}

inline _Atomic_integral_t _Compare_increment_atomic_counter(
	_Atomic_counter_t& _Counter, _Atomic_integral_t _Expected)
	{	// atomically compare and increment counter and return result
	return (_Compare_increment_atomic_counter_explicit(
		_Counter, _Expected, memory_order_seq_cst));
	}

  #else /* defined(__cplusplus) */
#define _Get_atomic_count(_Counter)	_Counter

#define _Init_atomic_counter(_Counter, _Value)	\
	_Counter = _Value

#define _Inc_atomic_counter_explicit(_Counter, _Order)	\
	(_Atomic_fetch_add_4(&_Counter, 1, _Order) + 1)

#define _Inc_atomic_counter(_Counter)	\
	(_Inc_atomic_counter_explicit(_Counter, memory_order_seq_cst))

#define _Dec_atomic_counter_explicit(_Counter, _Order)	\
	(_Atomic_fetch_sub_4(&_Counter, 1, _Order) - 1)

#define _Dec_atomic_counter(_Counter)	\
	(_Dec_atomic_counter_explicit(_Counter, memory_order_seq_cst))

#define _Load_atomic_counter_explicit(_Counter, _Order)	\
	_Atomic_load_4(&_Counter, _Order)

#define _Load_atomic_counter(_Counter)	\
	_Load_atomic_counter_explicit(_Counter, memory_order_seq_cst)

#define _Compare_increment_atomic_counter_explicit(_Counter, _Expected, _Order)	\
	_Atomic_compare_exchange_weak_4(&_Counter, &_Expected, _Expected + 1, \
	_Order, _Order)

#define _Compare_increment_atomic_counter(_Counter, _Expected)	\
	_Compare_increment_atomic_counter_explicit( \
		_Counter, _Expected, memory_order_seq_cst)
  #endif /* defined(__cplusplus) */

		/* SPIN LOCKS */
_EXTERN_C
_CRTIMP2_PURE void __cdecl _Lock_shared_ptr_spin_lock();
_CRTIMP2_PURE void __cdecl _Unlock_shared_ptr_spin_lock();
_END_EXTERN_C
_STD_END
 #pragma pop_macro("new")
 #pragma warning(pop)
 //#pragma pack(pop)
 #pragma pack(pop, _CRT_PACKING)
#endif /* RC_INVOKED */
#endif /* _XATOMIC0_H */

/*
 * Copyright (c) by P.J. Plauger. All rights reserved.
 * Consult your license regarding permissions and restrictions.
V6.50:0009 */
