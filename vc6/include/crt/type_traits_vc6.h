// type_traits standard header
#pragma once
#ifndef _TYPE_TRAITS_VC6_
#define _TYPE_TRAITS_VC6_

_STD_BEGIN

// VC++ V14 SUPPORT
#define _IS_BASE_OF(_Base, _Der)	\
	: _Cat_base<__is_base_of(_Base, _Der)>
#define _IS_CONVERTIBLE(_From, _To)	\
	: _Cat_base<__is_convertible_to(_From, _To)>
#define _IS_UNION(_Ty)	\
	: _Cat_base<__is_union(_Ty)>
#define _IS_CLASS(_Ty)	\
	: _Cat_base<__is_class(_Ty)>
#define _IS_POD(_Ty)	\
	: _Cat_base<__is_pod(_Ty)>
#define _IS_EMPTY(_Ty)	\
	: _Cat_base<__is_empty(_Ty)>
#define _IS_POLYMORPHIC(_Ty)	\
	: _Cat_base<__is_polymorphic(_Ty)>
#define _IS_ABSTRACT(_Ty)	\
	: _Cat_base<__is_abstract(_Ty)>
#define _IS_FINAL(_Ty)	\
	: _Cat_base<__is_final(_Ty)>
#define _IS_STANDARD_LAYOUT(_Ty)	\
	: _Cat_base<__is_standard_layout(_Ty)>
#define _IS_TRIVIAL(_Ty)	\
	: _Cat_base<__is_trivial(_Ty)>
#define _IS_TRIVIALLY_COPYABLE(_Ty)	\
	: _Cat_base<__is_trivially_copyable(_Ty)>
#define _HAS_TRIVIAL_DESTRUCTOR(_Ty)	\
	: _Cat_base<__has_trivial_destructor(_Ty)>
#define _HAS_VIRTUAL_DESTRUCTOR(_Ty)	\
	: _Cat_base<__has_virtual_destructor(_Ty)>
#define _UNDERLYING_TYPE(_Ty)	\
	__underlying_type(_Ty)
#define _IS_LITERAL_TYPE(_Ty)	\
	: _Cat_base<__is_literal_type(_Ty)>
#define _IS_ENUM(_Ty)	\
	: _Cat_base<__is_enum(_Ty)>
#define _IS_DESTRUCTIBLE(_Ty) \
	: _Cat_base<__is_destructible(_Ty)>
#define _IS_NOTHROW_ASSIGNABLE(_To, _From) \
	: _Cat_base<__is_nothrow_assignable(_To, _From)>
#define _IS_NOTHROW_DESTRUCTIBLE(_Ty) \
	: _Cat_base<__is_nothrow_destructible(_Ty)>
#define _IS_TRIVIALLY_ASSIGNABLE(_To, _From) \
	: _Cat_base<__is_trivially_assignable(_To, _From)>
#define _IS_CONSTRUCTIBLE \
	__is_constructible
#define _IS_NOTHROW_CONSTRUCTIBLE \
	__is_nothrow_constructible
#define _IS_TRIVIALLY_CONSTRUCTIBLE \
	__is_trivially_constructible

template <typename T1, typename T2>
struct is_convertible {
private:
  struct True_ {
    char x[2];
  };
  struct False_ {
  };

  static True_ helper(T2 const &);
  static False_ helper(...);

public:
  static bool const value = (
    sizeof(True_) == sizeof(is_convertible::helper(T1()))
    );
};

template <bool, class T = void>
struct enable_if {
};

template <class T>
struct enable_if<true, T> {
  typedef T type;
};

template<class T>
struct remove_extent {
  typedef T type;
};

template<class T>
struct remove_extent<T[]> {
  typedef T type;
};

template<class T, size_t N>
struct remove_extent<T[N]> {
  typedef T type;
};

typedef decltype(nullptr) nullptr_t;

template<class T, T v>
struct integral_constant {
  static constexpr T value = v;
  typedef T value_type;
  typedef integral_constant type;
  constexpr operator value_type() const { return value; }
  constexpr value_type operator()() const { return value; }
};

template <class T, T v> const T integral_constant<T, v>::value;

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

template <class T> struct is_const : false_type {};
template <class T> struct is_const<const T> : true_type {};

template <class T> struct is_void : false_type {};
template <> struct is_void<void> : true_type {};

// TEMPLATE CLASS is_array
template<class _Ty>
struct is_array
  : false_type
{	// determine whether _Ty is an array
};

template<class _Ty, size_t _Nx>
struct is_array<_Ty[_Nx]>
  : true_type
{	// determine whether _Ty is an array
};

template<class _Ty>
struct is_array<_Ty[]>
  : true_type
{	// determine whether _Ty is an array
};

// TEMPLATE CLASS is_lvalue_reference
template<class _Ty>
struct is_lvalue_reference
  : false_type
{	// determine whether _Ty is an lvalue reference
};

template<class _Ty>
struct is_lvalue_reference<_Ty&>
  : true_type
{	// determine whether _Ty is an lvalue reference
};

// TEMPLATE CLASS is_rvalue_reference
template<class _Ty>
struct is_rvalue_reference
  : false_type
{	// determine whether _Ty is an rvalue reference
};

template<class _Ty>
struct is_rvalue_reference<_Ty&&>
  : true_type
{	// determine whether _Ty is an rvalue reference
};

// TEMPLATE CLASS _Cat_base
template<bool _Val>
struct _Cat_base
  : integral_constant<bool, _Val>
{	// base class for type predicates
};

// TEMPLATE CLASS is_reference
template<class _Ty>
struct is_reference
  : _Cat_base<is_lvalue_reference<_Ty>::value
  || is_rvalue_reference<_Ty>::value>
{	// determine whether _Ty is a reference
};

template<class _Ty>
struct _Is_memfunptr
{	// base class for member function pointer predicates
  typedef false_type _Bool_type;
};

// TEMPLATE CLASS is_member_object_pointer
template<class _Ty,
  bool _Pmf = _Is_memfunptr<_Ty>::_Bool_type::value>
struct _Is_member_object_pointer
  : false_type
{	// determine whether _Ty is a pointer to member object
};

template<class _Ty1,
class _Ty2>
struct _Is_member_object_pointer<_Ty1 _Ty2::*, false>
  : true_type
{	// determine whether _Ty is a pointer to member object
  typedef _Ty2 _Class_type;
};

// TEMPLATE CLASS conditional
template<bool _Test,
class _Ty1,
class _Ty2>
struct conditional
{	// type is _Ty2 for assumed !_Test
  typedef _Ty2 type;
};

template<class _Ty1,
class _Ty2>
struct conditional<true, _Ty1, _Ty2>
{	// type is _Ty1 for _Test
  typedef _Ty1 type;
};

// TEMPLATE CLASS is_same
template<class _Ty1,
class _Ty2>
struct is_same
  : false_type
{	// determine whether _Ty1 and _Ty2 are the same type
};

template<class _Ty1>
struct is_same<_Ty1, _Ty1>
  : true_type
{	// determine whether _Ty1 and _Ty2 are the same type
};

// TEMPLATE CLASS remove_const
template<class _Ty>
struct remove_const
{	// remove top level const qualifier
  typedef _Ty type;
};

template<class _Ty>
struct remove_const<const _Ty>
{	// remove top level const qualifier
  typedef _Ty type;
};

template<class _Ty>
struct remove_const<const _Ty[]>
{	// remove top level const qualifier
  typedef _Ty type[];
};

template<class _Ty, size_t _Nx>
struct remove_const<const _Ty[_Nx]>
{	// remove top level const qualifier
  typedef _Ty type[_Nx];
};

// TEMPLATE CLASS remove_volatile
template<class _Ty>
struct remove_volatile
{	// remove top level volatile qualifier
  typedef _Ty type;
};

template<class _Ty>
struct remove_volatile<volatile _Ty>
{	// remove top level volatile qualifier
  typedef _Ty type;
};

template<class _Ty>
struct remove_volatile<volatile _Ty[]>
{	// remove top level volatile qualifier
  typedef _Ty type[];
};

template<class _Ty, size_t _Nx>
struct remove_volatile<volatile _Ty[_Nx]>
{	// remove top level volatile qualifier
  typedef _Ty type[_Nx];
};

// TEMPLATE CLASS remove_cv
template<class _Ty>
struct remove_cv
{	// remove top level const and volatile qualifiers
  typedef typename remove_const<typename remove_volatile<_Ty>::type>::type
    type;
};

template<class _Ty>
struct is_member_object_pointer
  : _Is_member_object_pointer<typename remove_cv<_Ty>::type>::type
{	// determine whether _Ty is a pointer to member object
};

// TEMPLATE CLASS is_member_function_pointer
template<class _Ty>
struct is_member_function_pointer
  : _Is_memfunptr<typename remove_cv<_Ty>::type>::_Bool_type
{	// determine whether _Ty is a pointer to member function
};

// TEMPLATE CLASS is_pointer
template<class _Ty>
struct _Is_pointer
  : false_type
{	// determine whether _Ty is a pointer
};

template<class _Ty>
struct _Is_pointer<_Ty *>
  : _Cat_base<!is_member_object_pointer<_Ty *>::value
  && !is_member_function_pointer<_Ty *>::value>
{	// determine whether _Ty is a pointer
};

template<class _Ty>
struct is_pointer
  : _Is_pointer<typename remove_cv<_Ty>::type>
{	// determine whether _Ty is a pointer
};

// TEMPLATE CLASS is_null_pointer

template<class _Ty>
struct is_null_pointer
  : _Cat_base<is_same<typename remove_cv<_Ty>::type, nullptr_t>::value>
{	// determine whether _Ty is nullptr_t
};

// TEMPLATE CLASS is_union
template<class _Ty>
struct is_union _IS_UNION(_Ty)
{	// determine whether _Ty is a union
};

// TEMPLATE CLASS is_class
template<class _Ty>
struct is_class _IS_CLASS(_Ty)
{	// determine whether _Ty is a class
};

// TEMPLATE CLASS _Is_integral
template<class _Ty>
struct _Is_integral
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<bool>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<char>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<unsigned char>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<signed char>
  : true_type
{	// determine whether _Ty is integral
};

#ifdef _NATIVE_WCHAR_T_DEFINED
template<>
struct _Is_integral<wchar_t>
  : true_type
{	// determine whether _Ty is integral
};
#endif /* _NATIVE_WCHAR_T_DEFINED */

template<>
struct _Is_integral<unsigned short>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<signed short>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<unsigned int>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<signed int>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<unsigned long>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<signed long>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<char16_t>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<char32_t>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<__int64>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_integral<unsigned __int64>
  : true_type
{	// determine whether _Ty is integral
};

// TEMPLATE CLASS is_integral
template<class _Ty>
struct is_integral
  : _Is_integral<typename remove_cv<_Ty>::type>
{	// determine whether _Ty is integral
};

// TEMPLATE CLASS _Is_unsigned_integral
template<class _Ty>
struct _Is_unsigned_integral
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<bool>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<char>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<unsigned char>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<signed char>
  : false_type
{	// determine whether _Ty is integral
};

#ifdef _NATIVE_WCHAR_T_DEFINED
template<>
struct _Is_unsigned_integral<wchar_t>
  : false_type
{	// determine whether _Ty is integral
};
#endif /* _NATIVE_WCHAR_T_DEFINED */

template<>
struct _Is_unsigned_integral<unsigned short>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<signed short>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<unsigned int>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<signed int>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<unsigned long>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<signed long>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<char16_t>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<char32_t>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<__int64>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_unsigned_integral<unsigned __int64>
  : true_type
{	// determine whether _Ty is integral
};


template<class _Ty>
struct is_unsigned
  : _Is_unsigned_integral<typename remove_cv<_Ty>::type>
{	// determine whether _Ty is integral
};

// TEMPLATE CLASS _Is_signed_integral
template<class _Ty>
struct _Is_signed_integral
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<bool>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<char>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<unsigned char>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<signed char>
  : true_type
{	// determine whether _Ty is integral
};

#ifdef _NATIVE_WCHAR_T_DEFINED
template<>
struct _Is_signed_integral<wchar_t>
  : false_type
{	// determine whether _Ty is integral
};
#endif /* _NATIVE_WCHAR_T_DEFINED */

template<>
struct _Is_signed_integral<unsigned short>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<signed short>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<unsigned int>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<signed int>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<unsigned long>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<signed long>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<char16_t>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<char32_t>
  : false_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<__int64>
  : true_type
{	// determine whether _Ty is integral
};

template<>
struct _Is_signed_integral<unsigned __int64>
  : false_type
{	// determine whether _Ty is integral
};


template<class _Ty>
struct is_signed
  : _Is_signed_integral<typename remove_cv<_Ty>::type>
{	// determine whether _Ty is integral
};


// TEMPLATE CLASS _Is_floating_point
template<class _Ty>
struct _Is_floating_point
  : false_type
{	// determine whether _Ty is floating point
};

template<>
struct _Is_floating_point<float>
  : true_type
{	// determine whether _Ty is floating point
};

template<>
struct _Is_floating_point<double>
  : true_type
{	// determine whether _Ty is floating point
};

template<>
struct _Is_floating_point<long double>
  : true_type
{	// determine whether _Ty is floating point
};

// TEMPLATE CLASS is_floating_point
template<class _Ty>
struct is_floating_point
  : _Is_floating_point<typename remove_cv<_Ty>::type>
{	// determine whether _Ty is floating point
};

// TEMPLATE CLASS is_arithmetic
template<class _Ty>
struct is_arithmetic
  : _Cat_base<is_integral<_Ty>::value
  || is_floating_point<_Ty>::value>
{	// determine whether _Ty is an arithmetic type
};

// TEMPLATE CLASS is_fundamental
template<class _Ty>
struct is_fundamental
  : _Cat_base<is_arithmetic<_Ty>::value
  || is_void<_Ty>::value
  || is_null_pointer<_Ty>::value>
{	// determine whether _Ty is a fundamental type
};

template<class _Ty>
struct is_enum
  _IS_ENUM(_Ty)
{	// determine whether _Ty is an enumerated type
};

template<bool,
class _Ty1,
class _Ty2>
struct _If
{	// type is _Ty2 for assumed false
  typedef _Ty2 type;
};

template<class _Ty1,
class _Ty2>
struct _If<true, _Ty1, _Ty2>
{	// type is _Ty1 for assumed true
  typedef _Ty1 type;
};

template<class _Ty>
struct _Change_sign
{	// signed/unsigned partners to _Ty
  static_assert(
    ((is_integral<_Ty>::value || is_enum<_Ty>::value)
      && !is_same<_Ty, bool>::value),
    "make_signed<T>/make_unsigned<T> require that T shall be a (possibly "
    "cv-qualified) integral type or enumeration but not a bool type.");

  typedef
    typename _If<is_same<_Ty, signed char>::value
    || is_same<_Ty, unsigned char     >::value, signed char,
    typename _If<is_same<_Ty, short       >::value
    || is_same<_Ty, unsigned short    >::value, short,
    typename _If<is_same<_Ty, int         >::value
    || is_same<_Ty, unsigned int      >::value, int,
    typename _If<is_same<_Ty, long        >::value
    || is_same<_Ty, unsigned long     >::value, long,
    typename _If<is_same<_Ty, long long   >::value
    || is_same<_Ty, unsigned long long>::value, long long,
    typename _If<sizeof(_Ty) == sizeof(signed char), signed char,
    typename _If<sizeof(_Ty) == sizeof(short), short,
    typename _If<sizeof(_Ty) == sizeof(int), int,
    typename _If<sizeof(_Ty) == sizeof(long), long,
    long long
    >::type>::type>::type>::type>::type>::type>::type>::type>::type
    _Signed;

  typedef
    typename _If<is_same<_Signed, signed char>::value, unsigned char,
    typename _If<is_same<_Signed, short      >::value, unsigned short,
    typename _If<is_same<_Signed, int        >::value, unsigned int,
    typename _If<is_same<_Signed, long       >::value, unsigned long,
    unsigned long long
    >::type>::type>::type>::type
    _Unsigned;
};

template<class _Ty>
struct _Change_sign<const _Ty>
{	// signed/unsigned partners to _Ty
  typedef const typename _Change_sign<_Ty>::_Signed _Signed;
  typedef const typename _Change_sign<_Ty>::_Unsigned _Unsigned;
};

template<class _Ty>
struct _Change_sign<volatile _Ty>
{	// signed/unsigned partners to _Ty
  typedef volatile typename _Change_sign<_Ty>::_Signed _Signed;
  typedef volatile typename _Change_sign<_Ty>::_Unsigned _Unsigned;
};

template<class _Ty>
struct _Change_sign<const volatile _Ty>
{	// signed/unsigned partners to _Ty
  typedef const volatile typename _Change_sign<_Ty>::_Signed _Signed;
  typedef const volatile typename _Change_sign<_Ty>::_Unsigned _Unsigned;
};

// TEMPLATE CLASS make_unsigned
template<class _Ty>
struct make_unsigned
{	// unsigned partner to _Ty
  typedef typename _Change_sign<_Ty>::_Unsigned type;
};

_STD_END

#endif /* _TYPE_TRAITS_VC6_ */

/*
* Copyright (c) by P.J. Plauger. All rights reserved.
* Consult your license regarding permissions and restrictions.
V6.50:0009 */