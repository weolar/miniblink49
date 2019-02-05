/* xatomic.h internal header */
#pragma once
#ifndef _XATOMIC_H
#define _XATOMIC_H
#ifndef RC_INVOKED

#include <stdlib.h>
#include <string.h>

#include <intrin.h>
#include <xutility>

#include <intrinvc6.h>

#pragma pack(push,_CRT_PACKING)
#pragma warning(push,3)
#pragma push_macro("new")
#undef new

#pragma warning (disable: 4100 4390 4793 6326)

#if defined(_M_ARM)
#define _Memory_barrier()     __dmb(_ARM_BARRIER_ISH)
#endif /* defined(_M_ARM) */

#if defined(_M_ARM64)
#define _Memory_barrier()     __dmb(_ARM64_BARRIER_ISH)
#endif /* defined(_M_ARM64) */

#ifndef _CONCAT
#define _CONCATX(x, y)	x ## y
#define _CONCAT(x, y)		_CONCATX(x, y)
#endif /* _CONCAT */

#define ATOMIC_BOOL_LOCK_FREE	\
	(1 <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define _ATOMIC_CHAR_LOCK_FREE	\
	(1 <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define _ATOMIC_CHAR16_T_LOCK_FREE	\
	(2 <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define _ATOMIC_CHAR32_T_LOCK_FREE	\
	(2 <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define _ATOMIC_WCHAR_T_LOCK_FREE	\
	(_WCHAR_T_SIZE <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define _ATOMIC_SHORT_LOCK_FREE	\
	(_SHORT_SIZE <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define _ATOMIC_INT_LOCK_FREE	\
	(_INT_SIZE <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define _ATOMIC_LONG_LOCK_FREE	\
	(_LONG_SIZE <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define _ATOMIC_LLONG_LOCK_FREE	\
	(_LONGLONG_SIZE <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)
#define ATOMIC_POINTER_LOCK_FREE	\
	(_ADDR_SIZE <= _ATOMIC_MAXBYTES_LOCK_FREE ? 2 : 0)

/* Interlocked intrinsic mapping for _nf/_acq/_rel */
#if defined(_M_ARM) || defined(_M_ARM64)
#define _INTRIN_RELAXED(x)	_CONCAT(x, _nf)
#define _INTRIN_ACQUIRE(x)	_CONCAT(x, _acq)
#define _INTRIN_RELEASE(x)	_CONCAT(x, _rel)
#define _INTRIN_SEQ_CST(x)	x
#else /* defined(_M_ARM) || defined(_M_ARM64) */
#define _INTRIN_RELAXED(x)	x
#define _INTRIN_ACQUIRE(x)	x
#define _INTRIN_RELEASE(x)	x
#define _INTRIN_SEQ_CST(x)	x
#endif /* defined(_M_ARM) || defined(_M_ARM64) */

_STD_BEGIN
/* TYPEDEFS FOR INTERNAL ARITHMETIC TYPES */
typedef unsigned char _Uint1_t;
typedef unsigned short _Uint2_t;
//typedef _Uint32t _Uint4_t;
typedef unsigned __int64 _Uint8_t;

#define _ATOMIC_FLAG_TEST_AND_SET _Atomic_flag_test_and_set
#define _ATOMIC_FLAG_CLEAR _Atomic_flag_clear

#define _ATOMIC_THREAD_FENCE _Atomic_thread_fence
#define _ATOMIC_SIGNAL_FENCE _Atomic_signal_fence

#ifndef _INVALID_MEMORY_ORDER

#if _ITERATOR_DEBUG_LEVEL == 2
#define _INVALID_MEMORY_ORDER \
     {_DEBUG_ERROR("Invalid memory_order"); \
     _SCL_SECURE_INVALID_ARGUMENT}

#elif _ITERATOR_DEBUG_LEVEL == 1
#define _INVALID_MEMORY_ORDER \
     _SCL_SECURE_VALIDATE("Invalid memory_order" && 0)

#elif _ITERATOR_DEBUG_LEVEL == 0
#define _INVALID_MEMORY_ORDER
#endif /* _ITERATOR_DEBUG_LEVEL */
#endif /* _INVALID_MEMORY_ORDER */

inline memory_order _Memory_order_upper_bound(memory_order _Order1,
    memory_order _Order2)
{   /* find upper bound of two memory orders,
    based on the following partial order:

    seq_cst
    |
    acq_rel
    /     \
    acquire  release
    |       |
    consume    |
    \     /
    relaxed

    */

    static const memory_order _Upper[6][6] = {	/* combined upper bounds */
        { memory_order_relaxed, memory_order_consume, memory_order_acquire,
        memory_order_release, memory_order_acq_rel, memory_order_seq_cst },
        { memory_order_consume, memory_order_consume, memory_order_acquire,
        memory_order_acq_rel, memory_order_acq_rel, memory_order_seq_cst },
        { memory_order_acquire, memory_order_acquire, memory_order_acquire,
        memory_order_acq_rel, memory_order_acq_rel, memory_order_seq_cst },
        { memory_order_release, memory_order_acq_rel, memory_order_acq_rel,
        memory_order_release, memory_order_acq_rel, memory_order_seq_cst },
        { memory_order_acq_rel, memory_order_acq_rel, memory_order_acq_rel,
        memory_order_acq_rel, memory_order_acq_rel, memory_order_seq_cst },
        { memory_order_seq_cst, memory_order_seq_cst, memory_order_seq_cst,
        memory_order_seq_cst, memory_order_seq_cst, memory_order_seq_cst
        }
    };

    if ((_Order1 < 0) || (6 <= _Order1)
        || (_Order2 < 0) || (6 <= _Order2))
    {	/* launder memory order */
        _INVALID_MEMORY_ORDER;
        return (memory_order_seq_cst);
    }
    return (_Upper[_Order1][_Order2]);
}

inline void _Validate_compare_exchange_memory_order(
    memory_order _Success, memory_order _Failure)
{	/* validate success/failure */
    /* _Failure may not be memory_order_release or memory_order_acq_rel
    and may not be stronger than _Success */
    switch (_Failure)
    {
    case memory_order_relaxed:
        break;

    case memory_order_seq_cst:
        if (_Success != memory_order_seq_cst)
            _INVALID_MEMORY_ORDER;
        break;

    case memory_order_acquire:
        if ((_Success == memory_order_consume) ||
            (_Success == memory_order_relaxed))
            _INVALID_MEMORY_ORDER;
        break;

    case memory_order_consume:
        if (_Success == memory_order_relaxed)
            _INVALID_MEMORY_ORDER;
        break;

    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
}


/* _Atomic_store_1 */
inline void _Store_relaxed_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* store _Value atomically with relaxed memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    __iso_volatile_store8((volatile char *)_Tgt, _Value);

#else
    *_Tgt = _Value;
#endif
}

inline void _Store_release_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* store _Value atomically with release memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store8((volatile char *)_Tgt, _Value);

#else
    _Compiler_barrier();
    *_Tgt = _Value;
#endif
}

inline void _Store_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* store _Value atomically with
    sequentially consistent memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store8((volatile char *)_Tgt, _Value);
    _Memory_barrier();

#else
    _INTRIN_SEQ_CST(_InterlockedExchange8)((volatile char *)_Tgt, _Value);
#endif
}

inline void _Atomic_store_1(
    volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
{	/* store _Value atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        _Store_relaxed_1(_Tgt, _Value);
        break;

    case memory_order_release:
        _Store_release_1(_Tgt, _Value);
        break;

    case memory_order_seq_cst:
        _Store_seq_cst_1(_Tgt, _Value);
        break;

    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
}

/* _Atomic_load_1 */
inline _Uint1_t _Load_seq_cst_1(volatile _Uint1_t *_Tgt)
{	/* load from *_Tgt atomically with
    sequentially consistent memory order */
    _Uint1_t _Value;

#if defined(_M_ARM) || defined(_M_ARM64)
    _Value = __iso_volatile_load8((volatile char *)_Tgt);
    _Memory_barrier();

#else
    _Value = *_Tgt;
    _Compiler_barrier();
#endif

    return (_Value);
}

inline _Uint1_t _Load_relaxed_1(volatile _Uint1_t *_Tgt)
{	/* load from *_Tgt atomically with
    relaxed memory order */
    _Uint1_t _Value;

#if defined(_M_ARM) || defined(_M_ARM64)
    _Value = __iso_volatile_load8((volatile char *)_Tgt);

#else
    _Value = *_Tgt;
#endif

    return (_Value);
}

inline _Uint1_t _Load_acquire_1(volatile _Uint1_t *_Tgt)
{	/* load from *_Tgt atomically with
    acquire memory order */

    return (_Load_seq_cst_1(_Tgt));
}

inline _Uint1_t _Atomic_load_1(
    volatile _Uint1_t *_Tgt, memory_order _Order)
{	/* load from *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Load_relaxed_1(_Tgt));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Load_acquire_1(_Tgt));

    case memory_order_seq_cst:
        return (_Load_seq_cst_1(_Tgt));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_exchange_1 */
inline _Uint1_t _Exchange_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedExchange8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Exchange_relaxed_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedExchange8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Exchange_acquire_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedExchange8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Exchange_release_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedExchange8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Atomic_exchange_1(
    volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
{	/* exchange _Value and *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Exchange_relaxed_1(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Exchange_acquire_1(_Tgt, _Value));

    case memory_order_release:
        return (_Exchange_release_1(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Exchange_seq_cst_1(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_compare_exchange_weak_1, _Atomic_compare_exchange_strong_1 */
inline int _Compare_exchange_seq_cst_1(volatile _Uint1_t *_Tgt,
    _Uint1_t *_Exp, _Uint1_t _Value)
{	/* compare and exchange values atomically with
    sequentially consistent memory order */
    _Uint1_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint1_t _Prev = _INTRIN_SEQ_CST(_InterlockedCompareExchange8)((volatile char *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_relaxed_1(volatile _Uint1_t *_Tgt,
    _Uint1_t *_Exp, _Uint1_t _Value)
{	/* compare and exchange values atomically with
    relaxed memory order */
    _Uint1_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint1_t _Prev = _INTRIN_RELAXED(_InterlockedCompareExchange8)((volatile char *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_acquire_1(volatile _Uint1_t *_Tgt,
    _Uint1_t *_Exp, _Uint1_t _Value)
{	/* compare and exchange values atomically with
    acquire memory order */
    _Uint1_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint1_t _Prev = _INTRIN_ACQUIRE(_InterlockedCompareExchange8)((volatile char *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_release_1(volatile _Uint1_t *_Tgt,
    _Uint1_t *_Exp, _Uint1_t _Value)
{	/* compare and exchange values atomically with
    release memory order */
    _Uint1_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint1_t _Prev = _INTRIN_RELEASE(_InterlockedCompareExchange8)((volatile char *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Atomic_compare_exchange_strong_1(
    volatile _Uint1_t *_Tgt, _Uint1_t *_Exp, _Uint1_t _Value,
    memory_order _Order1, memory_order _Order2)
{	/* compare and exchange values atomically */
    _Validate_compare_exchange_memory_order(_Order1, _Order2);

    switch (_Memory_order_upper_bound(_Order1, _Order2))
    {
    case memory_order_relaxed:
        return (_Compare_exchange_relaxed_1(_Tgt, _Exp, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Compare_exchange_acquire_1(_Tgt, _Exp, _Value));

    case memory_order_release:
        return (_Compare_exchange_release_1(_Tgt, _Exp, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Compare_exchange_seq_cst_1(_Tgt, _Exp, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline int _Atomic_compare_exchange_weak_1(
    volatile _Uint1_t *_Tgt, _Uint1_t *_Exp, _Uint1_t _Value,
    memory_order _Order1, memory_order _Order2)
{	/* compare and exchange values atomically */
    /* No weak compare-exchange is currently available,
    even for ARM, so fall back to strong */
    return (_Atomic_compare_exchange_strong_1(_Tgt, _Exp, _Value,
        _Order1, _Order2));
}

/* _Atomic_fetch_add_1, _Atomic_fetch_sub_1 */
inline _Uint1_t _Fetch_add_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* add _Value to *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedExchangeAdd8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_add_relaxed_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* add _Value to *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedExchangeAdd8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_add_acquire_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* add _Value to *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedExchangeAdd8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_add_release_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* add _Value to *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedExchangeAdd8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Atomic_fetch_add_1(
    volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
{	/* add _Value to *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_add_relaxed_1(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_add_acquire_1(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_add_release_1(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_add_seq_cst_1(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline _Uint1_t _Atomic_fetch_sub_1(
    volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
{	/* subtract _Value from *_Tgt atomically */
    return (_Atomic_fetch_add_1(_Tgt, 0 - _Value, _Order));
}

/* _Atomic_fetch_and_1 */
inline _Uint1_t _Fetch_and_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* and _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedAnd8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_and_relaxed_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* and _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedAnd8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_and_acquire_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* and _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedAnd8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_and_release_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* and _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedAnd8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Atomic_fetch_and_1(
    volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
{	/* and _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_and_relaxed_1(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_and_acquire_1(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_and_release_1(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_and_seq_cst_1(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_fetch_or_1 */
inline _Uint1_t _Fetch_or_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* or _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedOr8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_or_relaxed_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* or _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedOr8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_or_acquire_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* or _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedOr8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_or_release_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* or _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedOr8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Atomic_fetch_or_1(
    volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
{	/* or _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_or_relaxed_1(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_or_acquire_1(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_or_release_1(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_or_seq_cst_1(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_fetch_xor_1 */
inline _Uint1_t _Fetch_xor_seq_cst_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* xor _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedXor8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_xor_relaxed_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* xor _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedXor8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_xor_acquire_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* xor _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedXor8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Fetch_xor_release_1(volatile _Uint1_t *_Tgt, _Uint1_t _Value)
{	/* xor _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedXor8)((volatile char *)_Tgt, _Value));
}

inline _Uint1_t _Atomic_fetch_xor_1(
    volatile _Uint1_t *_Tgt, _Uint1_t _Value, memory_order _Order)
{	/* xor _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_xor_relaxed_1(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_xor_acquire_1(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_xor_release_1(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_xor_seq_cst_1(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_store_2 */
inline void _Store_relaxed_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* store _Value atomically with relaxed memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    __iso_volatile_store16((volatile short *)_Tgt, _Value);

#else
    *_Tgt = _Value;
#endif
}

inline void _Store_release_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* store _Value atomically with release memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store16((volatile short *)_Tgt, _Value);

#else
    _Compiler_barrier();
    *_Tgt = _Value;
#endif
}

inline void _Store_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* store _Value atomically with
    sequentially consistent memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store16((volatile short *)_Tgt, _Value);
    _Memory_barrier();

#else
    _INTRIN_SEQ_CST(_InterlockedExchange16)((volatile short *)_Tgt, _Value);
#endif
}

inline void _Atomic_store_2(
    volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
{	/* store _Value atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        _Store_relaxed_2(_Tgt, _Value);
        break;

    case memory_order_release:
        _Store_release_2(_Tgt, _Value);
        break;

    case memory_order_seq_cst:
        _Store_seq_cst_2(_Tgt, _Value);
        break;

    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
}

/* _Atomic_load_2 */
inline _Uint2_t _Load_seq_cst_2(volatile _Uint2_t *_Tgt)
{	/* load from *_Tgt atomically with
    sequentially consistent memory order */
    _Uint2_t _Value;

#if defined(_M_ARM) || defined(_M_ARM64)
    _Value = __iso_volatile_load16((volatile short *)_Tgt);
    _Memory_barrier();

#else
    _Value = *_Tgt;
    _Compiler_barrier();
#endif

    return (_Value);
}

inline _Uint2_t _Load_relaxed_2(volatile _Uint2_t *_Tgt)
{	/* load from *_Tgt atomically with
    relaxed memory order */
    _Uint2_t _Value;

#if defined(_M_ARM) || defined(_M_ARM64)
    _Value = __iso_volatile_load16((volatile short *)_Tgt);

#else
    _Value = *_Tgt;
#endif

    return (_Value);
}

inline _Uint2_t _Load_acquire_2(volatile _Uint2_t *_Tgt)
{	/* load from *_Tgt atomically with
    acquire memory order */

    return (_Load_seq_cst_2(_Tgt));
}

inline _Uint2_t _Atomic_load_2(
    volatile _Uint2_t *_Tgt, memory_order _Order)
{	/* load from *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Load_relaxed_2(_Tgt));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Load_acquire_2(_Tgt));

    case memory_order_seq_cst:
        return (_Load_seq_cst_2(_Tgt));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_exchange_2 */
inline _Uint2_t _Exchange_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedExchange16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Exchange_relaxed_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedExchange16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Exchange_acquire_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedExchange16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Exchange_release_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedExchange16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Atomic_exchange_2(
    volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
{	/* exchange _Value and *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Exchange_relaxed_2(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Exchange_acquire_2(_Tgt, _Value));

    case memory_order_release:
        return (_Exchange_release_2(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Exchange_seq_cst_2(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_compare_exchange_weak_2, _Atomic_compare_exchange_strong_2 */
inline int _Compare_exchange_seq_cst_2(volatile _Uint2_t *_Tgt,
    _Uint2_t *_Exp, _Uint2_t _Value)
{	/* compare and exchange values atomically with
    sequentially consistent memory order */
    _Uint2_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint2_t _Prev = _INTRIN_SEQ_CST(_InterlockedCompareExchange16)((volatile short *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_relaxed_2(volatile _Uint2_t *_Tgt,
    _Uint2_t *_Exp, _Uint2_t _Value)
{	/* compare and exchange values atomically with
    relaxed memory order */
    _Uint2_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint2_t _Prev = _INTRIN_RELAXED(_InterlockedCompareExchange16)((volatile short *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_acquire_2(volatile _Uint2_t *_Tgt,
    _Uint2_t *_Exp, _Uint2_t _Value)
{	/* compare and exchange values atomically with
    acquire memory order */
    _Uint2_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint2_t _Prev = _INTRIN_ACQUIRE(_InterlockedCompareExchange16)((volatile short *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_release_2(volatile _Uint2_t *_Tgt,
    _Uint2_t *_Exp, _Uint2_t _Value)
{	/* compare and exchange values atomically with
    release memory order */
    _Uint2_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint2_t _Prev = _INTRIN_RELEASE(_InterlockedCompareExchange16)((volatile short *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Atomic_compare_exchange_strong_2(
    volatile _Uint2_t *_Tgt, _Uint2_t *_Exp, _Uint2_t _Value,
    memory_order _Order1, memory_order _Order2)
{	/* compare and exchange values atomically */
    _Validate_compare_exchange_memory_order(_Order1, _Order2);

    switch (_Memory_order_upper_bound(_Order1, _Order2))
    {
    case memory_order_relaxed:
        return (_Compare_exchange_relaxed_2(_Tgt, _Exp, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Compare_exchange_acquire_2(_Tgt, _Exp, _Value));

    case memory_order_release:
        return (_Compare_exchange_release_2(_Tgt, _Exp, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Compare_exchange_seq_cst_2(_Tgt, _Exp, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline int _Atomic_compare_exchange_weak_2(
    volatile _Uint2_t *_Tgt, _Uint2_t *_Exp, _Uint2_t _Value,
    memory_order _Order1, memory_order _Order2)
{	/* compare and exchange values atomically */
    /* No weak compare-exchange is currently available,
    even for ARM, so fall back to strong */
    return (_Atomic_compare_exchange_strong_2(_Tgt, _Exp, _Value,
        _Order1, _Order2));
}

/* _Atomic_fetch_add_2, _Atomic_fetch_sub_2 */
inline _Uint2_t _Fetch_add_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* add _Value to *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedExchangeAdd16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_add_relaxed_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* add _Value to *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedExchangeAdd16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_add_acquire_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* add _Value to *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedExchangeAdd16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_add_release_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* add _Value to *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedExchangeAdd16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Atomic_fetch_add_2(
    volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
{	/* add _Value to *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_add_relaxed_2(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_add_acquire_2(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_add_release_2(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_add_seq_cst_2(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline _Uint2_t _Atomic_fetch_sub_2(
    volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
{	/* subtract _Value from *_Tgt atomically */
    return (_Atomic_fetch_add_2(_Tgt, 0 - _Value, _Order));
}

/* _Atomic_fetch_and_2 */
inline _Uint2_t _Fetch_and_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* and _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedAnd16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_and_relaxed_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* and _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedAnd16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_and_acquire_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* and _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedAnd16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_and_release_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* and _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedAnd16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Atomic_fetch_and_2(
    volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
{	/* and _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_and_relaxed_2(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_and_acquire_2(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_and_release_2(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_and_seq_cst_2(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_fetch_or_2 */
inline _Uint2_t _Fetch_or_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* or _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedOr16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_or_relaxed_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* or _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedOr16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_or_acquire_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* or _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedOr16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_or_release_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* or _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedOr16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Atomic_fetch_or_2(
    volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
{	/* or _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_or_relaxed_2(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_or_acquire_2(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_or_release_2(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_or_seq_cst_2(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_fetch_xor_2 */
inline _Uint2_t _Fetch_xor_seq_cst_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* xor _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedXor16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_xor_relaxed_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* xor _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedXor16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_xor_acquire_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* xor _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedXor16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Fetch_xor_release_2(volatile _Uint2_t *_Tgt, _Uint2_t _Value)
{	/* xor _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedXor16)((volatile short *)_Tgt, _Value));
}

inline _Uint2_t _Atomic_fetch_xor_2(
    volatile _Uint2_t *_Tgt, _Uint2_t _Value, memory_order _Order)
{	/* xor _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_xor_relaxed_2(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_xor_acquire_2(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_xor_release_2(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_xor_seq_cst_2(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_store_4 */
inline void _Store_relaxed_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* store _Value atomically with relaxed memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    __iso_volatile_store32((volatile int *)_Tgt, _Value);

#else
    *_Tgt = _Value;
#endif
}

inline void _Store_release_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* store _Value atomically with release memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store32((volatile int *)_Tgt, _Value);

#else
    _Compiler_barrier();
    *_Tgt = _Value;
#endif
}

inline void _Store_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* store _Value atomically with
    sequentially consistent memory order */

#if defined(_M_ARM) || defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store32((volatile int *)_Tgt, _Value);
    _Memory_barrier();

#else
    _INTRIN_SEQ_CST(_InterlockedExchange)((volatile long *)_Tgt, _Value);
#endif
}

inline void _Atomic_store_4(
    volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
{	/* store _Value atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        _Store_relaxed_4(_Tgt, _Value);
        break;

    case memory_order_release:
        _Store_release_4(_Tgt, _Value);
        break;

    case memory_order_seq_cst:
        _Store_seq_cst_4(_Tgt, _Value);
        break;

    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
}

/* _Atomic_load_4 */
inline _Uint4_t _Load_seq_cst_4(volatile _Uint4_t *_Tgt)
{	/* load from *_Tgt atomically with
    sequentially consistent memory order */
    _Uint4_t _Value;

#if defined(_M_ARM) || defined(_M_ARM64)
    _Value = __iso_volatile_load32((volatile int *)_Tgt);
    _Memory_barrier();

#else
    _Value = *_Tgt;
    _Compiler_barrier();
#endif

    return (_Value);
}

inline _Uint4_t _Load_relaxed_4(volatile _Uint4_t *_Tgt)
{	/* load from *_Tgt atomically with
    relaxed memory order */
    _Uint4_t _Value;

#if defined(_M_ARM) || defined(_M_ARM64)
    _Value = __iso_volatile_load32((volatile int *)_Tgt);

#else
    _Value = *_Tgt;
#endif

    return (_Value);
}

inline _Uint4_t _Load_acquire_4(volatile _Uint4_t *_Tgt)
{	/* load from *_Tgt atomically with
    acquire memory order */

    return (_Load_seq_cst_4(_Tgt));
}

inline _Uint4_t _Atomic_load_4(
    volatile _Uint4_t *_Tgt, memory_order _Order)
{	/* load from *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Load_relaxed_4(_Tgt));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Load_acquire_4(_Tgt));

    case memory_order_seq_cst:
        return (_Load_seq_cst_4(_Tgt));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_exchange_4 */
inline _Uint4_t _Exchange_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedExchange)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Exchange_relaxed_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedExchange)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Exchange_acquire_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedExchange)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Exchange_release_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedExchange)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Atomic_exchange_4(
    volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
{	/* exchange _Value and *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Exchange_relaxed_4(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Exchange_acquire_4(_Tgt, _Value));

    case memory_order_release:
        return (_Exchange_release_4(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Exchange_seq_cst_4(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_compare_exchange_weak_4, _Atomic_compare_exchange_strong_4 */
inline int _Compare_exchange_seq_cst_4(volatile _Uint4_t *_Tgt,
    _Uint4_t *_Exp, _Uint4_t _Value)
{	/* compare and exchange values atomically with
    sequentially consistent memory order */
    _Uint4_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint4_t _Prev = _INTRIN_SEQ_CST(_InterlockedCompareExchange)((volatile long *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_relaxed_4(volatile _Uint4_t *_Tgt,
    _Uint4_t *_Exp, _Uint4_t _Value)
{	/* compare and exchange values atomically with
    relaxed memory order */
    _Uint4_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint4_t _Prev = _INTRIN_RELAXED(_InterlockedCompareExchange)((volatile long *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_acquire_4(volatile _Uint4_t *_Tgt,
    _Uint4_t *_Exp, _Uint4_t _Value)
{	/* compare and exchange values atomically with
    acquire memory order */
    _Uint4_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint4_t _Prev = _INTRIN_ACQUIRE(_InterlockedCompareExchange)((volatile long *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_release_4(volatile _Uint4_t *_Tgt,
    _Uint4_t *_Exp, _Uint4_t _Value)
{	/* compare and exchange values atomically with
    release memory order */
    _Uint4_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint4_t _Prev = _INTRIN_RELEASE(_InterlockedCompareExchange)((volatile long *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Atomic_compare_exchange_strong_4(
    volatile _Uint4_t *_Tgt, _Uint4_t *_Exp, _Uint4_t _Value,
    memory_order _Order1, memory_order _Order2)
{	/* compare and exchange values atomically */
    _Validate_compare_exchange_memory_order(_Order1, _Order2);

    switch (_Memory_order_upper_bound(_Order1, _Order2))
    {
    case memory_order_relaxed:
        return (_Compare_exchange_relaxed_4(_Tgt, _Exp, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Compare_exchange_acquire_4(_Tgt, _Exp, _Value));

    case memory_order_release:
        return (_Compare_exchange_release_4(_Tgt, _Exp, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Compare_exchange_seq_cst_4(_Tgt, _Exp, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline int _Atomic_compare_exchange_weak_4(
    volatile _Uint4_t *_Tgt, _Uint4_t *_Exp, _Uint4_t _Value,
    memory_order _Order1, memory_order _Order2)
{	/* compare and exchange values atomically */
    /* No weak compare-exchange is currently available,
    even for ARM, so fall back to strong */
    return (_Atomic_compare_exchange_strong_4(_Tgt, _Exp, _Value,
        _Order1, _Order2));
}

/* _Atomic_fetch_add_4, _Atomic_fetch_sub_4 */
inline _Uint4_t _Fetch_add_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* add _Value to *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedExchangeAdd)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_add_relaxed_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* add _Value to *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedExchangeAdd)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_add_acquire_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* add _Value to *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedExchangeAdd)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_add_release_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* add _Value to *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedExchangeAdd)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Atomic_fetch_add_4(
    volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
{	/* add _Value to *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_add_relaxed_4(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_add_acquire_4(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_add_release_4(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_add_seq_cst_4(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline _Uint4_t _Atomic_fetch_sub_4(
    volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
{	/* subtract _Value from *_Tgt atomically */
    return (_Atomic_fetch_add_4(_Tgt, 0 - _Value, _Order));
}

/* _Atomic_fetch_and_4 */
inline _Uint4_t _Fetch_and_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* and _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedAnd)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_and_relaxed_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* and _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedAnd)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_and_acquire_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* and _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedAnd)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_and_release_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* and _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedAnd)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Atomic_fetch_and_4(
    volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
{	/* and _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_and_relaxed_4(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_and_acquire_4(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_and_release_4(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_and_seq_cst_4(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_fetch_or_4 */
inline _Uint4_t _Fetch_or_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* or _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedOr)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_or_relaxed_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* or _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedOr)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_or_acquire_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* or _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedOr)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_or_release_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* or _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedOr)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Atomic_fetch_or_4(
    volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
{	/* or _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_or_relaxed_4(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_or_acquire_4(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_or_release_4(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_or_seq_cst_4(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_fetch_xor_4 */
inline _Uint4_t _Fetch_xor_seq_cst_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* xor _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedXor)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_xor_relaxed_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* xor _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedXor)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_xor_acquire_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* xor _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedXor)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Fetch_xor_release_4(volatile _Uint4_t *_Tgt, _Uint4_t _Value)
{	/* xor _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedXor)((volatile long *)_Tgt, _Value));
}

inline _Uint4_t _Atomic_fetch_xor_4(
    volatile _Uint4_t *_Tgt, _Uint4_t _Value, memory_order _Order)
{	/* xor _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_xor_relaxed_4(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_xor_acquire_4(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_xor_release_4(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_xor_seq_cst_4(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_store_8 */
inline void _Store_relaxed_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* store _Value atomically with relaxed memory order */

#if defined(_M_X64)
    *_Tgt = _Value;

#elif defined(_M_ARM64)
    __iso_volatile_store64((volatile _LONGLONG *)_Tgt, _Value);

#else
    _INTRIN_RELAXED(_InterlockedExchange64)((volatile _LONGLONG *)_Tgt, _Value);
#endif
}

inline void _Store_release_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* store _Value atomically with release memory order */

#if defined(_M_X64)
    _Compiler_barrier();
    *_Tgt = _Value;

#elif defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store64((volatile _LONGLONG *)_Tgt, _Value);

#else
    _INTRIN_RELEASE(_InterlockedExchange64)((volatile _LONGLONG *)_Tgt, _Value);
#endif
}

inline void _Store_seq_cst_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* store _Value atomically with
    sequentially consistent memory order */

#if defined(_M_ARM64)
    _Memory_barrier();
    __iso_volatile_store64((volatile _LONGLONG *)_Tgt, _Value);
    _Memory_barrier();

#else
    _INTRIN_SEQ_CST(_InterlockedExchange64)((volatile _LONGLONG *)_Tgt, _Value);
#endif
}

inline void _Atomic_store_8(
    volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
{	/* store _Value atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        _Store_relaxed_8(_Tgt, _Value);
        break;

    case memory_order_release:
        _Store_release_8(_Tgt, _Value);
        break;

    case memory_order_seq_cst:
        _Store_seq_cst_8(_Tgt, _Value);
        break;

    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
}

/* _Atomic_load_8 */
inline _Uint8_t _Load_seq_cst_8(volatile _Uint8_t *_Tgt)
{	/* load from *_Tgt atomically with
    sequentially consistent memory order */
    _Uint8_t _Value;

#if defined(_M_X64)
    _Value = *_Tgt;
    _Compiler_barrier();

#elif defined(_M_ARM)
    _Value = __ldrexd((volatile _LONGLONG *)_Tgt);
    _Memory_barrier();

#elif defined(_M_ARM64)
    _Value = __iso_volatile_load64((volatile _LONGLONG *)_Tgt);
    _Memory_barrier();

#else
    _Value = _InterlockedOr64((volatile _LONGLONG *)_Tgt, 0);
#endif

    return (_Value);
}

inline _Uint8_t _Load_relaxed_8(volatile _Uint8_t *_Tgt)
{	/* load from *_Tgt atomically with
    relaxed memory order */
    _Uint8_t _Value;

#if defined(_M_X64)
    _Value = *_Tgt;

#elif defined(_M_ARM)
    _Value = __ldrexd((volatile _LONGLONG *)_Tgt);

#elif defined(_M_ARM64)
    _Value = __iso_volatile_load64((volatile _LONGLONG *)_Tgt);

#else
    _Value = _InterlockedOr64((volatile _LONGLONG *)_Tgt, 0);
#endif

    return (_Value);
}

inline _Uint8_t _Load_acquire_8(volatile _Uint8_t *_Tgt)
{	/* load from *_Tgt atomically with
    acquire memory order */

    return (_Load_seq_cst_8(_Tgt));
}

inline _Uint8_t _Atomic_load_8(
    volatile _Uint8_t *_Tgt, memory_order _Order)
{	/* load from *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Load_relaxed_8(_Tgt));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Load_acquire_8(_Tgt));

    case memory_order_seq_cst:
        return (_Load_seq_cst_8(_Tgt));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_exchange_8 */
inline _Uint8_t _Exchange_seq_cst_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedExchange64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Exchange_relaxed_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedExchange64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Exchange_acquire_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedExchange64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Exchange_release_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* exchange _Value and *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedExchange64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Atomic_exchange_8(
    volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
{	/* exchange _Value and *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Exchange_relaxed_8(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Exchange_acquire_8(_Tgt, _Value));

    case memory_order_release:
        return (_Exchange_release_8(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Exchange_seq_cst_8(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_compare_exchange_weak_8, _Atomic_compare_exchange_strong_8 */
inline int _Compare_exchange_seq_cst_8(volatile _Uint8_t *_Tgt,
    _Uint8_t *_Exp, _Uint8_t _Value)
{	/* compare and exchange values atomically with
    sequentially consistent memory order */
    _Uint8_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint8_t _Prev = _INTRIN_SEQ_CST(_InterlockedCompareExchange64)((volatile _LONGLONG *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_relaxed_8(volatile _Uint8_t *_Tgt,
    _Uint8_t *_Exp, _Uint8_t _Value)
{	/* compare and exchange values atomically with
    relaxed memory order */
    _Uint8_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint8_t _Prev = _INTRIN_RELAXED(_InterlockedCompareExchange64)((volatile _LONGLONG *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_acquire_8(volatile _Uint8_t *_Tgt,
    _Uint8_t *_Exp, _Uint8_t _Value)
{	/* compare and exchange values atomically with
    acquire memory order */
    _Uint8_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint8_t _Prev = _INTRIN_ACQUIRE(_InterlockedCompareExchange64)((volatile _LONGLONG *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Compare_exchange_release_8(volatile _Uint8_t *_Tgt,
    _Uint8_t *_Exp, _Uint8_t _Value)
{	/* compare and exchange values atomically with
    release memory order */
    _Uint8_t _Old_exp = *_Exp;	/* read before atomic operation */

    _Uint8_t _Prev = _INTRIN_RELEASE(_InterlockedCompareExchange64)((volatile _LONGLONG *)_Tgt,
        _Value, _Old_exp);

    if (_Prev == _Old_exp)
        return (1);
    else
    {	/* copy old value */
        *_Exp = _Prev;
        return (0);
    }
}

inline int _Atomic_compare_exchange_strong_8(
    volatile _Uint8_t *_Tgt, _Uint8_t *_Exp, _Uint8_t _Value,
    memory_order _Order1, memory_order _Order2)
{	/* compare and exchange values atomically */
    _Validate_compare_exchange_memory_order(_Order1, _Order2);

    switch (_Memory_order_upper_bound(_Order1, _Order2))
    {
    case memory_order_relaxed:
        return (_Compare_exchange_relaxed_8(_Tgt, _Exp, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Compare_exchange_acquire_8(_Tgt, _Exp, _Value));

    case memory_order_release:
        return (_Compare_exchange_release_8(_Tgt, _Exp, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Compare_exchange_seq_cst_8(_Tgt, _Exp, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline int _Atomic_compare_exchange_weak_8(
    volatile _Uint8_t *_Tgt, _Uint8_t *_Exp, _Uint8_t _Value,
    memory_order _Order1, memory_order _Order2)
{	/* compare and exchange values atomically */
    /* No weak compare-exchange is currently available,
    even for ARM, so fall back to strong */
    return (_Atomic_compare_exchange_strong_8(_Tgt, _Exp, _Value,
        _Order1, _Order2));
}

/* _Atomic_fetch_add_8, _Atomic_fetch_sub_8 */
inline _Uint8_t _Fetch_add_seq_cst_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* add _Value to *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedExchangeAdd64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_add_relaxed_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* add _Value to *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedExchangeAdd64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_add_acquire_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* add _Value to *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedExchangeAdd64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_add_release_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* add _Value to *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedExchangeAdd64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Atomic_fetch_add_8(
    volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
{	/* add _Value to *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_add_relaxed_8(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_add_acquire_8(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_add_release_8(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_add_seq_cst_8(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline _Uint8_t _Atomic_fetch_sub_8(
    volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
{	/* subtract _Value from *_Tgt atomically */
    return (_Atomic_fetch_add_8(_Tgt, 0 - _Value, _Order));
}

/* _Atomic_fetch_and_8 */
inline _Uint8_t _Fetch_and_seq_cst_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* and _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedAnd64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_and_relaxed_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* and _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedAnd64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_and_acquire_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* and _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedAnd64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_and_release_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* and _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedAnd64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Atomic_fetch_and_8(
    volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
{	/* and _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_and_relaxed_8(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_and_acquire_8(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_and_release_8(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_and_seq_cst_8(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_fetch_or_8 */
inline _Uint8_t _Fetch_or_seq_cst_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* or _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedOr64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_or_relaxed_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* or _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedOr64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_or_acquire_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* or _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedOr64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_or_release_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* or _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedOr64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Atomic_fetch_or_8(
    volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
{	/* or _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_or_relaxed_8(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_or_acquire_8(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_or_release_8(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_or_seq_cst_8(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

/* _Atomic_fetch_xor_8 */
inline _Uint8_t _Fetch_xor_seq_cst_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* xor _Value with *_Tgt atomically with
    sequentially consistent memory order */

    return (_INTRIN_SEQ_CST(_InterlockedXor64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_xor_relaxed_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* xor _Value with *_Tgt atomically with
    relaxed memory order */

    return (_INTRIN_RELAXED(_InterlockedXor64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_xor_acquire_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* xor _Value with *_Tgt atomically with
    acquire memory order */

    return (_INTRIN_ACQUIRE(_InterlockedXor64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Fetch_xor_release_8(volatile _Uint8_t *_Tgt, _Uint8_t _Value)
{	/* xor _Value with *_Tgt atomically with
    release memory order */

    return (_INTRIN_RELEASE(_InterlockedXor64)((volatile _LONGLONG *)_Tgt, _Value));
}

inline _Uint8_t _Atomic_fetch_xor_8(
    volatile _Uint8_t *_Tgt, _Uint8_t _Value, memory_order _Order)
{	/* xor _Value with *_Tgt atomically */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_Fetch_xor_relaxed_8(_Tgt, _Value));

    case memory_order_consume:
    case memory_order_acquire:
        return (_Fetch_xor_acquire_8(_Tgt, _Value));

    case memory_order_release:
        return (_Fetch_xor_release_8(_Tgt, _Value));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_Fetch_xor_seq_cst_8(_Tgt, _Value));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline int _Atomic_flag_test_and_set(volatile _Atomic_flag_t *_Flag,
    memory_order _Order)
{	/* atomically test flag and set to true */
    switch (_Order)
    {
    case memory_order_relaxed:
        return (_INTRIN_RELAXED(_interlockedbittestandset)((LONG *)_Flag, 0));

    case memory_order_consume:
    case memory_order_acquire:
        return (_INTRIN_ACQUIRE(_interlockedbittestandset)((LONG *)_Flag, 0));

    case memory_order_release:
        return (_INTRIN_RELEASE(_interlockedbittestandset)((LONG *)_Flag, 0));

    case memory_order_acq_rel:
    case memory_order_seq_cst:
        return (_INTRIN_SEQ_CST(_interlockedbittestandset)((LONG *)_Flag, 0));

    default:
        _INVALID_MEMORY_ORDER;
        return (0);
    }
}

inline void _Atomic_flag_clear(volatile _Atomic_flag_t *_Flag,
    memory_order _Order)
{	/* atomically clear flag */
    static_assert(sizeof(_Atomic_flag_t) == sizeof(_Uint4_t),
        "Unexpected _Atomic_flag_t size");

    switch (_Order)
    {
    case memory_order_relaxed:
    case memory_order_release:
    case memory_order_seq_cst:
        _Atomic_store_4((volatile _Uint4_t *)_Flag, 0, _Order);
        break;

    default:
        _INVALID_MEMORY_ORDER;
        break;
    }
}

inline void _Atomic_thread_fence(memory_order _Order)
{	/* force memory visibility and inhibit compiler reordering */
#if defined(_M_ARM) || defined(_M_ARM64)
    if (_Order != memory_order_relaxed)
    {
        _Memory_barrier();
    }

#else
    _Compiler_barrier();
    if (_Order == memory_order_seq_cst)
    {	/* force visibility */
        static _Uint4_t _Guard;
        _Atomic_exchange_4(&_Guard, 0, memory_order_seq_cst);
        _Compiler_barrier();
    }
#endif
}

inline void _Atomic_signal_fence(memory_order _Order)
{	/* inhibit compiler reordering */
    _Compiler_barrier();
}

#if defined(_M_ARM) || defined(_M_ARM64)
#define _YIELD_PROCESSOR __yield()

#else
#define _YIELD_PROCESSOR
#endif

/* SPIN LOCK FOR LOCKING VERSIONS OF OPERATIONS */
/* Use acquire semantics on lock and release on unlock. Given our
current atomic_flag implementation, this ensures not just
atomicity but also sequential consistency. */

inline void _Lock_spin_lock(
    volatile _Atomic_flag_t *_Flag)
{	/* spin until _Flag successfully set */
    while (_ATOMIC_FLAG_TEST_AND_SET(_Flag, memory_order_acquire))
        _YIELD_PROCESSOR;
}

inline void _Unlock_spin_lock(
    volatile _Atomic_flag_t *_Flag)
{	/* release previously obtained lock */
    _ATOMIC_FLAG_CLEAR(_Flag, memory_order_release);
}

#define _CSTD 

/* ATOMIC OPERATIONS FOR OBJECTS WITH SIZES THAT
DON'T MATCH THE SIZE OF ANY INTEGRAL TYPE */
inline void _Atomic_copy(
    volatile _Atomic_flag_t *_Flag, size_t _Size,
    volatile void *_Tgt, volatile const void *_Src,
    memory_order _Order)
{	/* atomically copy *_Src to *_Tgt with memory ordering */
    _Lock_spin_lock(_Flag);
    _CSTD memcpy((void *)_Tgt, (void *)_Src, _Size);
    _Unlock_spin_lock(_Flag);
}

inline void _Atomic_exchange(
    volatile _Atomic_flag_t *_Flag, size_t _Size,
    volatile void *_Tgt, volatile void *_Src,
    memory_order _Order)
{	/* atomically swap *_Src and *_Tgt with memory ordering */
    unsigned char *_Left = (unsigned char *)_Tgt;
    unsigned char *_Right = (unsigned char *)_Src;

    _Lock_spin_lock(_Flag);
    for (; 0 < _Size; --_Size)
    {	/* copy bytes */
        unsigned char _Tmp = *_Left;
        *_Left++ = *_Right;
        *_Right++ = _Tmp;
    }
    _Unlock_spin_lock(_Flag);
}

inline int _Atomic_compare_exchange_weak(
    volatile _Atomic_flag_t *_Flag, size_t _Size,
    volatile void *_Tgt, volatile void *_Exp, const volatile void *_Src,
    memory_order _Order1, memory_order _Order2)
{	/* atomically compare and exchange with memory ordering */
    int _Result;

    _Lock_spin_lock(_Flag);
    _Result = _CSTD memcmp((const void *)_Tgt, (const void *)_Exp, _Size) == 0;
    if (_Result != 0)
        _CSTD memcpy((void *)_Tgt, (void *)_Src, _Size);
    else
        _CSTD memcpy((void *)_Exp, (void *)_Tgt, _Size);
    _Unlock_spin_lock(_Flag);
    return (_Result);
}

inline int _Atomic_compare_exchange_strong(
    volatile _Atomic_flag_t *_Flag, size_t _Size,
    volatile void *_Tgt, volatile void *_Exp, const volatile void *_Src,
    memory_order _Order1, memory_order _Order2)
{	/* atomically compare and exchange with memory ordering */
    return (_Atomic_compare_exchange_weak(_Flag, _Size, _Tgt, _Exp, _Src,
        _Order1, _Order2));
}

/* LOCK-FREE PROPERTY FOR INTEGRAL TYPES */
inline int _Atomic_is_lock_free_1(void)
{	/* return true if 1-byte atomic values are lock-free */
    return (1 <= _ATOMIC_MAXBYTES_LOCK_FREE);
}

inline int _Atomic_is_lock_free_2(void)
{	/* return true if 2-byte atomic values are lock-free */
    return (2 <= _ATOMIC_MAXBYTES_LOCK_FREE);
}

inline int _Atomic_is_lock_free_4(void)
{	/* return true if 4-byte atomic values are lock-free */
    return (4 <= _ATOMIC_MAXBYTES_LOCK_FREE);
}

inline int _Atomic_is_lock_free_8(void)
{	/* return true if 8-byte atomic values are lock-free */
    return (8 <= _ATOMIC_MAXBYTES_LOCK_FREE);
}
_STD_END

#if defined(_M_IX86)
// #pragma pop_macro("_InterlockedExchange64")
// #pragma pop_macro("_InterlockedExchangeAdd64")
// #pragma pop_macro("_InterlockedAnd64")
// #pragma pop_macro("_InterlockedOr64")
// #pragma pop_macro("_InterlockedXor64")
#endif /* defined(_M_IX86) */

#pragma pop_macro("new")
#pragma warning(pop)
#pragma pack(pop, _CRT_PACKING) //#pragma pack(pop)

#endif /* RC_INVOKED */
#endif /* _XATOMIC_H */

/*
* Copyright (c) by P.J. Plauger. All rights reserved.
* Consult your license regarding permissions and restrictions.
V6.50:0009 */
