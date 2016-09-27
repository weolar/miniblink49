// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NonInterpolableValue_h
#define NonInterpolableValue_h

#include "platform/heap/Handle.h"

namespace blink {

// Holds components of a keyframe value that do not change when interpolating with another keyframe.
class NonInterpolableValue : public RefCountedWillBeGarbageCollectedFinalized<NonInterpolableValue> {
public:
    virtual ~NonInterpolableValue() { }

    typedef const void* Type;
    virtual Type type() const = 0;

    DEFINE_INLINE_VIRTUAL_TRACE() { }
};

// These macros provide safe downcasts of NonInterpolableValue subclasses with debug assertions.
// See CSSValueAnimationType for example usage.
#define DECLARE_NON_INTERPOLABLE_VALUE_TYPE() \
    static Type staticType; \
    virtual Type type() const { return staticType; }

#define DEFINE_NON_INTERPOLABLE_VALUE_TYPE(T) \
    NonInterpolableValue::Type T::staticType = &T::staticType;

#define DEFINE_NON_INTERPOLABLE_VALUE_TYPE_CASTS(T) \
    DEFINE_TYPE_CASTS(T, NonInterpolableValue, value, value->type() == T::staticType, value.type() == T::staticType);

} // namespace blink

#endif // NonInterpolableValue_h
