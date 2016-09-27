// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InterpolableValue_h
#define InterpolableValue_h

#include "core/CoreExport.h"
#include "core/animation/animatable/AnimatableValue.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class CORE_EXPORT InterpolableValue : public NoBaseWillBeGarbageCollected<InterpolableValue> {
    DECLARE_EMPTY_VIRTUAL_DESTRUCTOR_WILL_BE_REMOVED(InterpolableValue);
public:
    virtual bool isNumber() const { return false; }
    virtual bool isBool() const { return false; }
    virtual bool isList() const { return false; }
    virtual bool isAnimatableValue() const { return false; }

    virtual PassOwnPtrWillBeRawPtr<InterpolableValue> clone() const = 0;

    DEFINE_INLINE_VIRTUAL_TRACE() { }

private:
    virtual void interpolate(const InterpolableValue& to, const double progress, InterpolableValue& result) const = 0;
    virtual void add(const InterpolableValue& rhs, InterpolableValue& result) const = 0;
    virtual void multiply(double scalar, InterpolableValue& result) const = 0;

    friend class Interpolation;
    friend class PairwisePrimitiveInterpolation;

    // Keep interpolate private, but allow calls within the hierarchy without
    // knowledge of type.
    friend class DeferredLegacyStyleInterpolation;
    friend class InterpolableNumber;
    friend class InterpolableBool;
    friend class InterpolableList;

    friend class AnimationInterpolableValueTest;
};

class CORE_EXPORT InterpolableNumber final : public InterpolableValue {
public:
    static PassOwnPtrWillBeRawPtr<InterpolableNumber> create(double value)
    {
        return adoptPtrWillBeNoop(new InterpolableNumber(value));
    }

    virtual bool isNumber() const override final { return true; }
    double value() const { return m_value; }
    virtual PassOwnPtrWillBeRawPtr<InterpolableValue> clone() const override final { return create(m_value); }

private:
    virtual void interpolate(const InterpolableValue& to, const double progress, InterpolableValue& result) const override final;
    virtual void add(const InterpolableValue& rhs, InterpolableValue& result) const override final;
    virtual void multiply(double scalar, InterpolableValue& result) const override final;
    double m_value;

    explicit InterpolableNumber(double value)
        : m_value(value)
    {
    }

};

class CORE_EXPORT InterpolableBool final : public InterpolableValue {
public:
    static PassOwnPtrWillBeRawPtr<InterpolableBool> create(bool value)
    {
        return adoptPtrWillBeNoop(new InterpolableBool(value));
    }

    virtual bool isBool() const override final { return true; }
    bool value() const { return m_value; }
    virtual PassOwnPtrWillBeRawPtr<InterpolableValue> clone() const override final { return create(m_value); }

private:
    virtual void interpolate(const InterpolableValue& to, const double progress, InterpolableValue& result) const override final;
    virtual void add(const InterpolableValue& rhs, InterpolableValue& result) const override final;
    virtual void multiply(double scalar, InterpolableValue& result) const override final { ASSERT_NOT_REACHED(); }
    bool m_value;

    explicit InterpolableBool(bool value)
        : m_value(value)
    {
    }

};

class CORE_EXPORT InterpolableList : public InterpolableValue {
public:
    // Explicitly delete operator= because MSVC automatically generate
    // copy constructors and operator= for dll-exported classes.
    // Since InterpolableList is not copyable, automatically generated
    // operator= causes MSVC compiler error.
    // However, we cannot use WTF_MAKE_NONCOPYABLE because InterpolableList
    // has its own copy constructor. So just delete operator= here.
    InterpolableList& operator=(const InterpolableList&) = delete;

    static PassOwnPtrWillBeRawPtr<InterpolableList> create(const InterpolableList &other)
    {
        return adoptPtrWillBeNoop(new InterpolableList(other));
    }

    static PassOwnPtrWillBeRawPtr<InterpolableList> create(size_t size)
    {
        return adoptPtrWillBeNoop(new InterpolableList(size));
    }

    virtual bool isList() const override final { return true; }
    void set(size_t position, PassOwnPtrWillBeRawPtr<InterpolableValue> value)
    {
        ASSERT(position < m_size);
        m_values[position] = value;
    }
    const InterpolableValue* get(size_t position) const
    {
        ASSERT(position < m_size);
        return m_values[position].get();
    }
    size_t length() const { return m_size; }
    virtual PassOwnPtrWillBeRawPtr<InterpolableValue> clone() const override final { return create(*this); }

    DECLARE_VIRTUAL_TRACE();

private:
    virtual void interpolate(const InterpolableValue& to, const double progress, InterpolableValue& result) const override final;
    virtual void add(const InterpolableValue& rhs, InterpolableValue& result) const override final;
    virtual void multiply(double scalar, InterpolableValue& result) const override final;
    explicit InterpolableList(size_t size)
        : m_size(size)
        , m_values(m_size)
    {
    }

    InterpolableList(const InterpolableList& other)
        : m_size(other.m_size)
        , m_values(m_size)
    {
        for (size_t i = 0; i < m_size; i++)
            set(i, other.m_values[i]->clone());
    }

    size_t m_size;
    WillBeHeapVector<OwnPtrWillBeMember<InterpolableValue>> m_values;
};

// FIXME: Remove this when we can.
class InterpolableAnimatableValue : public InterpolableValue {
public:
    static PassOwnPtrWillBeRawPtr<InterpolableAnimatableValue> create(PassRefPtrWillBeRawPtr<AnimatableValue> value)
    {
        return adoptPtrWillBeNoop(new InterpolableAnimatableValue(value));
    }

    virtual bool isAnimatableValue() const override final { return true; }
    AnimatableValue* value() const { return m_value.get(); }
    virtual PassOwnPtrWillBeRawPtr<InterpolableValue> clone() const override final { return create(m_value); }

    DECLARE_VIRTUAL_TRACE();

private:
    virtual void interpolate(const InterpolableValue &to, const double progress, InterpolableValue& result) const override final;
    virtual void add(const InterpolableValue& rhs, InterpolableValue& result) const override final { ASSERT_NOT_REACHED(); }
    virtual void multiply(double scalar, InterpolableValue& result) const override final { ASSERT_NOT_REACHED(); }
    RefPtrWillBeMember<AnimatableValue> m_value;

    InterpolableAnimatableValue(PassRefPtrWillBeRawPtr<AnimatableValue> value)
        : m_value(value)
    {
    }
};

DEFINE_TYPE_CASTS(InterpolableNumber, InterpolableValue, value, value->isNumber(), value.isNumber());
DEFINE_TYPE_CASTS(InterpolableBool, InterpolableValue, value, value->isBool(), value.isBool());
DEFINE_TYPE_CASTS(InterpolableList, InterpolableValue, value, value->isList(), value.isList());
DEFINE_TYPE_CASTS(InterpolableAnimatableValue, InterpolableValue, value, value->isAnimatableValue(), value.isAnimatableValue());

}

#endif
