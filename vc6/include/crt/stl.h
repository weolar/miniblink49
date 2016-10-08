// stl.h supplemental header
#ifndef _STL_H_
#define _STL_H_
#include <algorithm>
#include <deque>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <utility>
#include <vector>
using namespace std;

		// TEMPLATE CLASS Deque
template<class _Ty>
	class Deque : public deque<_Ty, allocator<_Ty> > {
public:
	typedef Deque<_Ty> _Myt;
	typedef allocator<_Ty> _A;
	explicit Deque()
		: deque<_Ty, _A>() {}
	explicit Deque(size_type _N, const _Ty& _V = _Ty())
		: deque<_Ty, _A>(_N, _V) {}
	typedef const_iterator _It;
	Deque(_It _F, _It _L)
		: deque<_Ty, _A>(_F, _L) {}
	void swap(_Myt& _X)
		{deque<_Ty, _A>::swap((deque<_Ty, _A>&)_X); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	};

		// TEMPLATE CLASS List
template<class _Ty>
	class List : public list<_Ty, allocator<_Ty> > {
public:
	typedef List<_Ty> _Myt;
	typedef allocator<_Ty> _A;
	explicit List()
		: list<_Ty, _A>() {}
	explicit List(size_type _N, const _Ty& _V = _Ty())
		: list<_Ty, _A>(_N, _V) {}
	typedef const_iterator _It;
	List(_It _F, _It _L)
		: list<_Ty, _A>(_F, _L) {}
	void swap(_Myt& _X)
		{list<_Ty, _A>::swap((list<_Ty, _A>&)_X); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	};

		// TEMPLATE CLASS Map
template<class _K, class _Ty, class _Pr = less<_K> >
	class Map : public map<_K, _Ty, _Pr, allocator<_Ty> > {
public:
	typedef Map<_K, _Ty, _Pr> _Myt;
	typedef allocator<_Ty> _A;
	explicit Map(const _Pr& _Pred = _Pr())
		: map<_K, _Ty, _Pr, _A>(_Pred) {}
	typedef const_iterator _It;
	Map(_It _F, _It _L, const _Pr& _Pred = _Pr())
		: map<_K, _Ty, _Pr, _A>(_F, _L, _Pred) {}
	void swap(_Myt& _X)
		{map<_K, _Ty, _Pr, _A>::
			swap((map<_K, _Ty, _Pr, _A>&)_X); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	};

		// TEMPLATE CLASS Multimap
template<class _K, class _Ty, class _Pr = less<_K> >
	class Multimap
		: public multimap<_K, _Ty, _Pr, allocator<_Ty> > {
public:
	typedef Multimap<_K, _Ty, _Pr> _Myt;
	typedef allocator<_Ty> _A;
	explicit Multimap(const _Pr& _Pred = _Pr())
		: multimap<_K, _Ty, _Pr, _A>(_Pred) {}
	typedef const_iterator _It;
	Multimap(_It _F, _It _L, const _Pr& _Pred = _Pr())
		: multimap<_K, _Ty, _Pr, _A>(_F, _L, _Pred) {}
	void swap(_Myt& _X)
		{multimap<_K, _Ty, _Pr, _A>::
			swap((multimap<_K, _Ty, _Pr, _A>&)_X); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	};

		// TEMPLATE CLASS Set
template<class _K, class _Pr = less<_K> >
	class Set : public set<_K, _Pr, allocator<_K> > {
public:
	typedef Set<_K, _Pr> _Myt;
	typedef allocator<_K> _A;
	explicit Set(const _Pr& _Pred = _Pr())
		: set<_K, _Pr, _A>(_Pred) {}
	typedef const_iterator _It;
	Set(_It _F, _It _L, const _Pr& _Pred = _Pr())
		: set<_K, _Pr, _A>(_F, _L, _Pred) {}
	void swap(_Myt& _X)
		{set<_K, _Pr, _A>::swap((set<_K, _Pr, _A>&)_X); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	};

		// TEMPLATE CLASS Multiset
template<class _K, class _Pr = less<_K> >
	class Multiset : public multiset<_K, _Pr, allocator<_K> > {
public:
	typedef Multiset<_K, _Pr> _Myt;
	typedef allocator<_K> _A;
	explicit Multiset(const _Pr& _Pred = _Pr())
		: multiset<_K, _Pr, _A>(_Pred) {}
	typedef const_iterator _It;
	Multiset(_It _F, _It _L, const _Pr& _Pred = _Pr())
		: multiset<_K, _Pr, _A>(_F, _L, _Pred) {}
	void swap(_Myt& _X)
		{multiset<_K, _Pr, _A>::
			swap((multiset<_K, _Pr, _A>&)_X); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	};

		// TEMPLATE CLASS Vector
template<class _Ty>
	class Vector : public vector<_Ty, allocator<_Ty> > {
public:
	typedef Vector<_Ty> _Myt;
	typedef allocator<_Ty> _A;
	explicit Vector()
		: vector<_Ty, _A>(_Al) {}
	explicit Vector(size_type _N, const _Ty& _V = _Ty())
		: vector<_Ty, _A>(_N, _V) {}
	typedef const_iterator _It;
	Vector(_It _F, _It _L)
		: vector<_Ty, _A>(_F, _L) {}
	void swap(_Myt& _X)
		{vector<_Ty, _A>::swap((vector<_Ty, _A>&)_X); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	};

		// CLASS bit_vector
class bit_vector : public vector<_Bool, _Bool_allocator> {
public:
	typedef _Bool _Ty;
	typedef _Bool_allocator _A;
	typedef bit_vector _Myt;
	explicit bit_vector()
		: vector<_Bool, _Bool_allocator>() {}
	explicit bit_vector(size_type _N, const _Ty& _V = _Ty())
		: vector<_Bool, _Bool_allocator>(_N, _V) {}
	typedef const_iterator _It;
	bit_vector(_It _F, _It _L)
		: vector<_Bool, _Bool_allocator>(_F, _L) {}
	void swap(_Myt& _X)
		{vector<_Bool, _Bool_allocator>::
			swap((vector<_Bool, _Bool_allocator>&)_X); }
	friend void swap(_Myt& _X, _Myt& _Y)
		{_X.swap(_Y); }
	};

		// TEMPLATE CLASS priority_queue
template<class _C = vector<_Ty>,
	class _Pr = less<_C::value_type> >
	class Priority_queue
		: public priority_queue<_C::value_type, _C, _Pr,
			allocator<_C::value_type> > {
public:
	typedef _C::value_type _Ty;
	typedef allocator<_C::value_type> _A;
	explicit Priority_queue(const _Pr& _X = _Pr())
		: priority_queue<_Ty, _C, _Pr, _A>(_X) {}
	typedef const value_type *_It;
	Priority_queue(_It _F, _It _L, const _Pr& _X = _Pr())
		: priority_queue<_Ty, _C, _Pr, _A>(_F, _L, _X) {}
	};

		// TEMPLATE CLASS queue
template<class _C = deque<_Ty> >
	class Queue
		: public queue<_C::value_type, _C,
			allocator<_C::value_type> > {
	};

		// TEMPLATE CLASS stack
template<class _C = deque<_Ty> >
	class Stack
		: public stack<_C::value_type, _C,
			allocator<_C::value_type> > {
	};

		// MACRO DEFINITIONS
#define deque			Deque
#define list			List
#define map				Map
#define multimap		Multimap
#define set				Set
#define multiset		Multiset
#define vector			Vector
#define priority_queue	Priority_queue
#define queue			Queue
#define stack			Stack

#endif	/* _STL_H_ */

/*
 * Copyright (c) 1996 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */

/*
 * This file is derived from software bearing the following
 * restrictions:
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this
 * software and its documentation for any purpose is hereby
 * granted without fee, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation.
 * Hewlett-Packard Company makes no representations about the
 * suitability of this software for any purpose. It is provided
 * "as is" without express or implied warranty.
 */

