// initializer_list standard header
#pragma once
#ifndef _INITIALIZER_LIST_
#define _INITIALIZER_LIST_

_STD_BEGIN

// TEMPLATE CLASS initializer_list
template<class _Elem>
class initializer_list {	// list of pointers to elements
public:
  typedef _Elem value_type;
  typedef const _Elem& reference;
  typedef const _Elem& const_reference;
  typedef size_t size_type;

  typedef const _Elem* iterator;
  typedef const _Elem* const_iterator;

  initializer_list()
    : _First(0), _Last(0)
  {
    // empty list
  }

  initializer_list(const _Elem *_First_arg,
    const _Elem *_Last_arg)
    : _First(_First_arg), _Last(_Last_arg)
  {
    // construct with pointers
  }

  const _Elem *begin() const
  {
    // get beginning of list
    return (_First);
  }

  const _Elem *end() const
  {
    // get end of list
    return (_Last);
  }

  size_t size() const
  {
    // get length of list
    return ((size_t)(_Last - _First));
  }

private:
  const _Elem *_First;
  const _Elem *_Last;
};

// TEMPLATE FUNCTION begin
template<class _Elem> inline
const _Elem *begin(initializer_list<_Elem> _Ilist)
{
  // get beginning of sequence
  return (_Ilist.begin());
}

// TEMPLATE FUNCTION end
template<class _Elem> inline
const _Elem *end(initializer_list<_Elem> _Ilist)
{
  // get end of sequence
  return (_Ilist.end());
}

_STD_END

#endif /* _INITIALIZER_LIST_ */

/*
* Copyright (c) by P.J. Plauger.weolar All rights reserved.
* Consult your license regarding permissions and restrictions.
V6.50:0009 */
