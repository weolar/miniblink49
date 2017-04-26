// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/runtime/runtime-utils.h"

#include "src/arguments.h"
#include "src/base/macros.h"
#include "src/base/platform/mutex.h"
#include "src/conversions-inl.h"
#include "src/factory.h"
#if USING_VC6RT == 1
#include <atomic>
#endif

// Implement Atomic accesses to SharedArrayBuffers as defined in the
// SharedArrayBuffer draft spec, found here
// https://github.com/tc39/ecmascript_sharedmem

namespace v8 {
namespace internal {

namespace {

inline bool AtomicIsLockFree(uint32_t size) {
  return size == 1 || size == 2 || size == 4;
}

#if V8_CC_GNU

template <typename T>
inline T CompareExchangeSeqCst(T* p, T oldval, T newval) {
  (void)__atomic_compare_exchange_n(p, &oldval, newval, 0, __ATOMIC_SEQ_CST,
                                    __ATOMIC_SEQ_CST);
  return oldval;
}

template <typename T>
inline T AddSeqCst(T* p, T value) {
  return __atomic_fetch_add(p, value, __ATOMIC_SEQ_CST);
}

template <typename T>
inline T SubSeqCst(T* p, T value) {
  return __atomic_fetch_sub(p, value, __ATOMIC_SEQ_CST);
}

template <typename T>
inline T AndSeqCst(T* p, T value) {
  return __atomic_fetch_and(p, value, __ATOMIC_SEQ_CST);
}

template <typename T>
inline T OrSeqCst(T* p, T value) {
  return __atomic_fetch_or(p, value, __ATOMIC_SEQ_CST);
}

template <typename T>
inline T XorSeqCst(T* p, T value) {
  return __atomic_fetch_xor(p, value, __ATOMIC_SEQ_CST);
}

template <typename T>
inline T ExchangeSeqCst(T* p, T value) {
  return __atomic_exchange_n(p, value, __ATOMIC_SEQ_CST);
}

#elif V8_CC_MSVC

#define _InterlockedCompareExchange32 _InterlockedCompareExchange
#define _InterlockedExchange32 _InterlockedExchange
#define _InterlockedExchangeAdd32 _InterlockedExchangeAdd
#define _InterlockedAnd32 _InterlockedAnd
#define _InterlockedOr32 _InterlockedOr
#define _InterlockedXor32 _InterlockedXor



#ifndef InterlockedCompareExchange64
__int64 InterlockedCompareExchange64(volatile LONGLONG* m, __int64 iNew, __int64 iOld)
{
	__int64 iReturn;
	_asm {
		push esi
			mov esi, m
			mov eax, dword ptr iOld
			mov edx, dword ptr iOld + 4
			mov ebx, dword ptr iNew
			mov ecx, dword ptr iNew + 4
			LOCK CMPXCHG8B[esi]
			pop esi
			mov dword ptr iReturn, eax
			mov dword ptr iReturn + 4, edx
	};
	return iReturn;
}
#endif // _InterlockedCompareExchange64

#ifndef _InterlockedExchange64
LONGLONG
FORCEINLINE
InterlockedExchange64(
	__inout LONGLONG volatile *Target,
	__in    LONGLONG Value
	)
{
	LONGLONG Old;

	do {
		Old = *Target;
	} while (InterlockedCompareExchange64(Target,
		Value,
		Old) != Old);

	return Old;
}
#define _InterlockedExchange64 InterlockedExchange64
#endif // !_InterlockedExchange64

#ifndef _InterlockedExchangeAdd64
LONGLONG
FORCEINLINE
InterlockedExchangeAdd64(
	__inout LONGLONG volatile *Addend,
	__in    LONGLONG Value
	)
{
	LONGLONG Old;

	do {
		Old = *Addend;
	} while (InterlockedCompareExchange64(Addend,
		Old + Value,
		Old) != Old);

	return Old;
}
#define _InterlockedExchangeAdd64 InterlockedExchangeAdd64
#endif // !_InterlockedExchangeAdd64

#ifndef _InterlockedAnd64
FORCEINLINE
LONGLONG
InterlockedAnd64(
	__inout LONGLONG volatile *Destination,
	__in    LONGLONG Value
	)
{
	LONGLONG Old;

	do {
		Old = *Destination;
	} while (InterlockedCompareExchange64(Destination,
		Old & Value,
		Old) != Old);

	return Old;
}
#define _InterlockedAnd64 InterlockedAnd64
#endif // !_InterlockedAnd64

#ifndef _InterlockedOr64
FORCEINLINE
LONGLONG
InterlockedOr64(
	__inout LONGLONG volatile *Destination,
	__in    LONGLONG Value
	)
{
	LONGLONG Old;

	do {
		Old = *Destination;
	} while (InterlockedCompareExchange64(Destination,
		Old | Value,
		Old) != Old);

	return Old;
}
#define _InterlockedOr64 InterlockedOr64
#endif // !_InterlockedOr64

#ifndef _InterlockedXor64
FORCEINLINE
LONGLONG
InterlockedXor64(
	__inout LONGLONG volatile *Destination,
	__in    LONGLONG Value
	)
{
	LONGLONG Old;

	do {
		Old = *Destination;
	} while (InterlockedCompareExchange64(Destination,
		Old ^ Value,
		Old) != Old);

	return Old;
}
#define _InterlockedXor64 InterlockedXor64
#endif // !_InterlockedXor64

#define INTEGER_TYPES(V)                           \
  V(int8_t, 8, char)                               \
  V(uint8_t, 8, char)                              \
  V(int16_t, 16, short)  /* NOLINT(runtime/int) */ \
  V(uint16_t, 16, short) /* NOLINT(runtime/int) */ \
  V(int32_t, 32, long)   /* NOLINT(runtime/int) */ \
  V(uint32_t, 32, long)  /* NOLINT(runtime/int) */ \
  V(int64_t, 64, LONGLONG)                         \
  V(uint64_t, 64, LONGLONG)

#define ATOMIC_OPS(type, suffix, vctype)                                       \
  inline type CompareExchangeSeqCst(volatile type* p, type oldval,             \
                                    type newval) {                             \
    return _InterlockedCompareExchange##suffix(                                \
        reinterpret_cast<volatile vctype*>(p), bit_cast<vctype>(newval),       \
        bit_cast<vctype>(oldval));                                             \
  }                                                                            \
  inline type LoadSeqCst(volatile type* p) { return *p; }                      \
  inline void StoreSeqCst(volatile type* p, type value) {                      \
    _InterlockedExchange##suffix(reinterpret_cast<volatile vctype*>(p),        \
                                 bit_cast<vctype>(value));                     \
  }                                                                            \
  inline type AddSeqCst(volatile type* p, type value) {                        \
    return _InterlockedExchangeAdd##suffix(                                    \
        reinterpret_cast<volatile vctype*>(p), bit_cast<vctype>(value));       \
  }                                                                            \
  inline type SubSeqCst(volatile type* p, type value) {                        \
    return _InterlockedExchangeAdd##suffix(                                    \
        reinterpret_cast<volatile vctype*>(p), -bit_cast<vctype>(value));      \
  }                                                                            \
  inline type AndSeqCst(volatile type* p, type value) {                        \
    return _InterlockedAnd##suffix(reinterpret_cast<volatile vctype*>(p),      \
                                   bit_cast<vctype>(value));                   \
  }                                                                            \
  inline type OrSeqCst(volatile type* p, type value) {                         \
    return _InterlockedOr##suffix(reinterpret_cast<volatile vctype*>(p),       \
                                  bit_cast<vctype>(value));                    \
  }                                                                            \
  inline type XorSeqCst(volatile type* p, type value) {                        \
    return _InterlockedXor##suffix(reinterpret_cast<volatile vctype*>(p),      \
                                   bit_cast<vctype>(value));                   \
  }                                                                            \
  inline type ExchangeSeqCst(volatile type* p, type value) {                   \
    return _InterlockedExchange##suffix(reinterpret_cast<volatile vctype*>(p), \
                                        bit_cast<vctype>(value));              \
  }

ATOMIC_OPS(int8_t, 8, char)
ATOMIC_OPS(uint8_t, 8, char)
ATOMIC_OPS(int16_t, 16, short)  /* NOLINT(runtime/int) */
ATOMIC_OPS(uint16_t, 16, short) /* NOLINT(runtime/int) */
ATOMIC_OPS(int32_t, 32, long)   /* NOLINT(runtime/int) */
ATOMIC_OPS(uint32_t, 32, long)  /* NOLINT(runtime/int) */

#undef ATOMIC_OPS_INTEGER
#undef ATOMIC_OPS

#undef InterlockedCompareExchange32
#undef InterlockedExchange32
#undef InterlockedExchangeAdd32
#undef InterlockedAnd32
#undef InterlockedOr32
#undef InterlockedXor32
#undef InterlockedExchangeAdd16
#undef InterlockedCompareExchange8
#undef InterlockedExchangeAdd8

#else

#error Unsupported platform!

#endif

template <typename T>
T FromObject(Handle<Object> number);

template <>
inline uint8_t FromObject<uint8_t>(Handle<Object> number) {
  return NumberToUint32(*number);
}

template <>
inline int8_t FromObject<int8_t>(Handle<Object> number) {
  return NumberToInt32(*number);
}

template <>
inline uint16_t FromObject<uint16_t>(Handle<Object> number) {
  return NumberToUint32(*number);
}

template <>
inline int16_t FromObject<int16_t>(Handle<Object> number) {
  return NumberToInt32(*number);
}

template <>
inline uint32_t FromObject<uint32_t>(Handle<Object> number) {
  return NumberToUint32(*number);
}

template <>
inline int32_t FromObject<int32_t>(Handle<Object> number) {
  return NumberToInt32(*number);
}


inline Object* ToObject(Isolate* isolate, int8_t t) { return Smi::FromInt(t); }

inline Object* ToObject(Isolate* isolate, uint8_t t) { return Smi::FromInt(t); }

inline Object* ToObject(Isolate* isolate, int16_t t) { return Smi::FromInt(t); }

inline Object* ToObject(Isolate* isolate, uint16_t t) {
  return Smi::FromInt(t);
}


inline Object* ToObject(Isolate* isolate, int32_t t) {
  return *isolate->factory()->NewNumber(t);
}


inline Object* ToObject(Isolate* isolate, uint32_t t) {
  return *isolate->factory()->NewNumber(t);
}


template <typename T>
inline Object* DoCompareExchange(Isolate* isolate, void* buffer, size_t index,
                                 Handle<Object> oldobj, Handle<Object> newobj) {
  T oldval = FromObject<T>(oldobj);
  T newval = FromObject<T>(newobj);
  T result =
      CompareExchangeSeqCst(static_cast<T*>(buffer) + index, oldval, newval);
  return ToObject(isolate, result);
}


template <typename T>
inline Object* DoAdd(Isolate* isolate, void* buffer, size_t index,
                     Handle<Object> obj) {
  T value = FromObject<T>(obj);
  T result = AddSeqCst(static_cast<T*>(buffer) + index, value);
  return ToObject(isolate, result);
}


template <typename T>
inline Object* DoSub(Isolate* isolate, void* buffer, size_t index,
                     Handle<Object> obj) {
  T value = FromObject<T>(obj);
  T result = SubSeqCst(static_cast<T*>(buffer) + index, value);
  return ToObject(isolate, result);
}


template <typename T>
inline Object* DoAnd(Isolate* isolate, void* buffer, size_t index,
                     Handle<Object> obj) {
  T value = FromObject<T>(obj);
  T result = AndSeqCst(static_cast<T*>(buffer) + index, value);
  return ToObject(isolate, result);
}


template <typename T>
inline Object* DoOr(Isolate* isolate, void* buffer, size_t index,
                    Handle<Object> obj) {
  T value = FromObject<T>(obj);
  T result = OrSeqCst(static_cast<T*>(buffer) + index, value);
  return ToObject(isolate, result);
}


template <typename T>
inline Object* DoXor(Isolate* isolate, void* buffer, size_t index,
                     Handle<Object> obj) {
  T value = FromObject<T>(obj);
  T result = XorSeqCst(static_cast<T*>(buffer) + index, value);
  return ToObject(isolate, result);
}


template <typename T>
inline Object* DoExchange(Isolate* isolate, void* buffer, size_t index,
                          Handle<Object> obj) {
  T value = FromObject<T>(obj);
  T result = ExchangeSeqCst(static_cast<T*>(buffer) + index, value);
  return ToObject(isolate, result);
}


// Uint8Clamped functions

uint8_t ClampToUint8(int32_t value) {
  if (value < 0) return 0;
  if (value > 255) return 255;
  return value;
}


inline Object* DoCompareExchangeUint8Clamped(Isolate* isolate, void* buffer,
                                             size_t index,
                                             Handle<Object> oldobj,
                                             Handle<Object> newobj) {
  typedef int32_t convert_type;
  uint8_t oldval = ClampToUint8(FromObject<convert_type>(oldobj));
  uint8_t newval = ClampToUint8(FromObject<convert_type>(newobj));
  uint8_t result = CompareExchangeSeqCst(static_cast<uint8_t*>(buffer) + index,
                                         oldval, newval);
  return ToObject(isolate, result);
}


#define DO_UINT8_CLAMPED_OP(name, op)                                        \
  inline Object* Do##name##Uint8Clamped(Isolate* isolate, void* buffer,      \
                                        size_t index, Handle<Object> obj) {  \
    typedef int32_t convert_type;                                            \
    uint8_t* p = static_cast<uint8_t*>(buffer) + index;                      \
    convert_type operand = FromObject<convert_type>(obj);                    \
    uint8_t expected;                                                        \
    uint8_t result;                                                          \
    do {                                                                     \
      expected = *p;                                                         \
      result = ClampToUint8(static_cast<convert_type>(expected) op operand); \
    } while (CompareExchangeSeqCst(p, expected, result) != expected);        \
    return ToObject(isolate, expected);                                      \
  }

DO_UINT8_CLAMPED_OP(Add, +)
DO_UINT8_CLAMPED_OP(Sub, -)
DO_UINT8_CLAMPED_OP(And, &)
DO_UINT8_CLAMPED_OP(Or, | )
DO_UINT8_CLAMPED_OP(Xor, ^)

#undef DO_UINT8_CLAMPED_OP


inline Object* DoExchangeUint8Clamped(Isolate* isolate, void* buffer,
                                      size_t index, Handle<Object> obj) {
  typedef int32_t convert_type;
  uint8_t* p = static_cast<uint8_t*>(buffer) + index;
  uint8_t result = ClampToUint8(FromObject<convert_type>(obj));
  uint8_t expected;
  do {
    expected = *p;
  } while (CompareExchangeSeqCst(p, expected, result) != expected);
  return ToObject(isolate, expected);
}


}  // anonymous namespace

// Duplicated from objects.h
// V has parameters (Type, type, TYPE, C type, element_size)
#define INTEGER_TYPED_ARRAYS(V)          \
  V(Uint8, uint8, UINT8, uint8_t, 1)     \
  V(Int8, int8, INT8, int8_t, 1)         \
  V(Uint16, uint16, UINT16, uint16_t, 2) \
  V(Int16, int16, INT16, int16_t, 2)     \
  V(Uint32, uint32, UINT32, uint32_t, 4) \
  V(Int32, int32, INT32, int32_t, 4)

RUNTIME_FUNCTION(Runtime_ThrowNotIntegerSharedTypedArrayError) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 0);
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate,
      NewTypeError(MessageTemplate::kNotIntegerSharedTypedArray, value));
}

RUNTIME_FUNCTION(Runtime_ThrowNotInt32SharedTypedArrayError) {
  HandleScope scope(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 0);
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate, NewTypeError(MessageTemplate::kNotInt32SharedTypedArray, value));
}

RUNTIME_FUNCTION(Runtime_ThrowInvalidAtomicAccessIndexError) {
  HandleScope scope(isolate);
  DCHECK_EQ(0, args.length());
  THROW_NEW_ERROR_RETURN_FAILURE(
      isolate, NewRangeError(MessageTemplate::kInvalidAtomicAccessIndex));
}

RUNTIME_FUNCTION(Runtime_AtomicsCompareExchange) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(JSTypedArray, sta, 0);
  CONVERT_SIZE_ARG_CHECKED(index, 1);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(oldobj, 2);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(newobj, 3);
  CHECK(sta->GetBuffer()->is_shared());
  CHECK_LT(index, NumberToSize(sta->length()));

  uint8_t* source = static_cast<uint8_t*>(sta->GetBuffer()->backing_store()) +
                    NumberToSize(sta->byte_offset());

  switch (sta->type()) {
#define TYPED_ARRAY_CASE(Type, typeName, TYPE, ctype, size) \
  case kExternal##Type##Array:                              \
    return DoCompareExchange<ctype>(isolate, source, index, oldobj, newobj);

    INTEGER_TYPED_ARRAYS(TYPED_ARRAY_CASE)
#undef TYPED_ARRAY_CASE

    case kExternalUint8ClampedArray:
      return DoCompareExchangeUint8Clamped(isolate, source, index, oldobj,
                                           newobj);

    default:
      break;
  }

  UNREACHABLE();
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_AtomicsAdd) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSTypedArray, sta, 0);
  CONVERT_SIZE_ARG_CHECKED(index, 1);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(value, 2);
  CHECK(sta->GetBuffer()->is_shared());
  CHECK_LT(index, NumberToSize(sta->length()));

  uint8_t* source = static_cast<uint8_t*>(sta->GetBuffer()->backing_store()) +
                    NumberToSize(sta->byte_offset());

  switch (sta->type()) {
#define TYPED_ARRAY_CASE(Type, typeName, TYPE, ctype, size) \
  case kExternal##Type##Array:                              \
    return DoAdd<ctype>(isolate, source, index, value);

    INTEGER_TYPED_ARRAYS(TYPED_ARRAY_CASE)
#undef TYPED_ARRAY_CASE

    case kExternalUint8ClampedArray:
      return DoAddUint8Clamped(isolate, source, index, value);

    default:
      break;
  }

  UNREACHABLE();
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_AtomicsSub) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSTypedArray, sta, 0);
  CONVERT_SIZE_ARG_CHECKED(index, 1);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(value, 2);
  CHECK(sta->GetBuffer()->is_shared());
  CHECK_LT(index, NumberToSize(sta->length()));

  uint8_t* source = static_cast<uint8_t*>(sta->GetBuffer()->backing_store()) +
                    NumberToSize(sta->byte_offset());

  switch (sta->type()) {
#define TYPED_ARRAY_CASE(Type, typeName, TYPE, ctype, size) \
  case kExternal##Type##Array:                              \
    return DoSub<ctype>(isolate, source, index, value);

    INTEGER_TYPED_ARRAYS(TYPED_ARRAY_CASE)
#undef TYPED_ARRAY_CASE

    case kExternalUint8ClampedArray:
      return DoSubUint8Clamped(isolate, source, index, value);

    default:
      break;
  }

  UNREACHABLE();
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_AtomicsAnd) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSTypedArray, sta, 0);
  CONVERT_SIZE_ARG_CHECKED(index, 1);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(value, 2);
  CHECK(sta->GetBuffer()->is_shared());
  CHECK_LT(index, NumberToSize(sta->length()));

  uint8_t* source = static_cast<uint8_t*>(sta->GetBuffer()->backing_store()) +
                    NumberToSize(sta->byte_offset());

  switch (sta->type()) {
#define TYPED_ARRAY_CASE(Type, typeName, TYPE, ctype, size) \
  case kExternal##Type##Array:                              \
    return DoAnd<ctype>(isolate, source, index, value);

    INTEGER_TYPED_ARRAYS(TYPED_ARRAY_CASE)
#undef TYPED_ARRAY_CASE

    case kExternalUint8ClampedArray:
      return DoAndUint8Clamped(isolate, source, index, value);

    default:
      break;
  }

  UNREACHABLE();
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_AtomicsOr) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSTypedArray, sta, 0);
  CONVERT_SIZE_ARG_CHECKED(index, 1);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(value, 2);
  CHECK(sta->GetBuffer()->is_shared());
  CHECK_LT(index, NumberToSize(sta->length()));

  uint8_t* source = static_cast<uint8_t*>(sta->GetBuffer()->backing_store()) +
                    NumberToSize(sta->byte_offset());

  switch (sta->type()) {
#define TYPED_ARRAY_CASE(Type, typeName, TYPE, ctype, size) \
  case kExternal##Type##Array:                              \
    return DoOr<ctype>(isolate, source, index, value);

    INTEGER_TYPED_ARRAYS(TYPED_ARRAY_CASE)
#undef TYPED_ARRAY_CASE

    case kExternalUint8ClampedArray:
      return DoOrUint8Clamped(isolate, source, index, value);

    default:
      break;
  }

  UNREACHABLE();
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_AtomicsXor) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSTypedArray, sta, 0);
  CONVERT_SIZE_ARG_CHECKED(index, 1);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(value, 2);
  CHECK(sta->GetBuffer()->is_shared());
  CHECK_LT(index, NumberToSize(sta->length()));

  uint8_t* source = static_cast<uint8_t*>(sta->GetBuffer()->backing_store()) +
                    NumberToSize(sta->byte_offset());

  switch (sta->type()) {
#define TYPED_ARRAY_CASE(Type, typeName, TYPE, ctype, size) \
  case kExternal##Type##Array:                              \
    return DoXor<ctype>(isolate, source, index, value);

    INTEGER_TYPED_ARRAYS(TYPED_ARRAY_CASE)
#undef TYPED_ARRAY_CASE

    case kExternalUint8ClampedArray:
      return DoXorUint8Clamped(isolate, source, index, value);

    default:
      break;
  }

  UNREACHABLE();
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_AtomicsExchange) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSTypedArray, sta, 0);
  CONVERT_SIZE_ARG_CHECKED(index, 1);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(value, 2);
  CHECK(sta->GetBuffer()->is_shared());
  CHECK_LT(index, NumberToSize(sta->length()));

  uint8_t* source = static_cast<uint8_t*>(sta->GetBuffer()->backing_store()) +
                    NumberToSize(sta->byte_offset());

  switch (sta->type()) {
#define TYPED_ARRAY_CASE(Type, typeName, TYPE, ctype, size) \
  case kExternal##Type##Array:                              \
    return DoExchange<ctype>(isolate, source, index, value);

    INTEGER_TYPED_ARRAYS(TYPED_ARRAY_CASE)
#undef TYPED_ARRAY_CASE

    case kExternalUint8ClampedArray:
      return DoExchangeUint8Clamped(isolate, source, index, value);

    default:
      break;
  }

  UNREACHABLE();
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_AtomicsIsLockFree) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(size, 0);
  uint32_t usize = NumberToUint32(*size);
  return isolate->heap()->ToBoolean(AtomicIsLockFree(usize));
}
}  // namespace internal
}  // namespace v8
