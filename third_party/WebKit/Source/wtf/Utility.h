// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Utility_h
#define Utility_h

#include "wtf/TypeTraits.h"

namespace WTF {

// TODO(jbroman): When a C++11 standard library is available, replace this with
// std::forward from <utility>.

template <typename T>
T&& forward(typename RemoveReference<T>::Type& t) { return static_cast<T&&>(t); }

template <typename T>
T&& forward(typename RemoveReference<T>::Type&& t) { return static_cast<T&&>(t); }

} // namespace WTF

#endif // Utility_h
