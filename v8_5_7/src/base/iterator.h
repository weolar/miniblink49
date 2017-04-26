// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_BASE_ITERATOR_H_
#define V8_BASE_ITERATOR_H_

#include <iterator>

#include "src/base/macros.h"
//////////////////////////////////////////////////////////////////////////
// for USING_VC6RT

namespace stdvc6 {
	//	ITERATOR STUFF (from <iterator>)
	// ITERATOR TAGS (from <iterator>)
	struct input_iterator_tag
	{	// identifying tag for input iterators
	};

	struct _Mutable_iterator_tag
	{	// identifying tag for mutable iterators
	};

	struct output_iterator_tag
		: _Mutable_iterator_tag
	{	// identifying tag for output iterators
	};

	struct forward_iterator_tag
		: input_iterator_tag, _Mutable_iterator_tag
	{	// identifying tag for forward iterators
	};

	struct bidirectional_iterator_tag
		: forward_iterator_tag
	{	// identifying tag for bidirectional iterators
	};

	struct random_access_iterator_tag
		: bidirectional_iterator_tag
	{	// identifying tag for random-access iterators
	};

	// TEMPLATE STRUCT _Param_tester
	template<class...>
	struct _Param_tester
	{	// test if parameters are valid
		typedef void type;
	};

	// TEMPLATE CLASS integral_constant
	template<class _Ty, _Ty _Val>
	struct integral_constant
	{	// convenient template for integral constant types
		static const _Ty value = _Val;

		typedef _Ty value_type;
		typedef integral_constant<_Ty, _Val> type;

		operator value_type() const
		{	// return stored value
			return (value);
		}

		value_type operator()() const
		{	// return stored value
			return (value);
		}
	};

	typedef integral_constant<bool, true> true_type;
	typedef integral_constant<bool, false> false_type;

	// TEMPLATE CLASS _Is_iterator
	template<class,
	class = void>
	struct _Is_iterator
		: false_type
	{	// default definition
	};

	template<class _Ty>
	struct _Is_iterator<_Ty, typename _Param_tester<
		typename _Ty::iterator_category,
		typename _Ty::value_type,
		typename _Ty::difference_type,
		typename _Ty::pointer,
		typename _Ty::reference
	>::type>
		: true_type
	{	// defined if _Ty::* types exist
	};

	template<class _Ty>
	struct _Is_iterator<_Ty *>
		: true_type
	{	// defined for pointers
	};

	// TEMPLATE CLASS iterator_traits
	template<class _Iter,
		bool = _Is_iterator<_Iter>::value>
	struct _Iterator_traits_base
	{	// get traits from iterator _Iter
		typedef typename _Iter::iterator_category iterator_category;
		typedef typename _Iter::value_type value_type;
		typedef typename _Iter::difference_type difference_type;

		typedef typename _Iter::pointer pointer;
		typedef typename _Iter::reference reference;
	};

	template<class _Iter>
	struct _Iterator_traits_base<_Iter, false>
	{	// empty for non-iterators
	};

	template<class _Iter>
	struct iterator_traits
		: _Iterator_traits_base<_Iter>
	{	// get traits from iterator _Iter, if possible
	};

	template<class _Ty>
	struct iterator_traits<_Ty *>
	{	// get traits from pointer
		typedef random_access_iterator_tag iterator_category;
		typedef _Ty value_type;
		typedef ptrdiff_t difference_type;

		typedef _Ty *pointer;
		typedef _Ty& reference;
	};

	template<class _Ty>
	struct iterator_traits<const _Ty *>
	{	// get traits from const pointer
		typedef random_access_iterator_tag iterator_category;
		typedef _Ty value_type;
		typedef ptrdiff_t difference_type;

		typedef const _Ty *pointer;
		typedef const _Ty& reference;
	};

} // stdvc6
  //////////////////////////////////////////////////////////////////////////

namespace v8 {
namespace base {

// The intention of the base::iterator_range class is to encapsulate two
// iterators so that the range defined by the iterators can be used like
// a regular STL container (actually only a subset of the full container
// functionality is available usually).
template <typename ForwardIterator>
class iterator_range {
 public:
  typedef ForwardIterator iterator;
  typedef ForwardIterator const_iterator;
  typedef typename stdvc6::iterator_traits<iterator>::pointer pointer;
  typedef typename stdvc6::iterator_traits<iterator>::reference reference;
  typedef typename stdvc6::iterator_traits<iterator>::value_type value_type;
  typedef
      typename stdvc6::iterator_traits<iterator>::difference_type difference_type;

  iterator_range() : begin_(), end_() {}
  template <typename ForwardIterator2>
  iterator_range(ForwardIterator2 const& begin, ForwardIterator2 const& end)
      : begin_(begin), end_(end) {}

  iterator begin() { return begin_; }
  iterator end() { return end_; }
  const_iterator begin() const { return begin_; }
  const_iterator end() const { return end_; }
  const_iterator cbegin() const { return begin_; }
  const_iterator cend() const { return end_; }

  bool empty() const { return cbegin() == cend(); }

  // Random Access iterators only.
  reference operator[](difference_type n) { return begin()[n]; }
  difference_type size() const { return cend() - cbegin(); }

 private:
  const_iterator const begin_;
  const_iterator const end_;
};

}  // namespace base
}  // namespace v8

#endif  // V8_BASE_ITERATOR_H_
