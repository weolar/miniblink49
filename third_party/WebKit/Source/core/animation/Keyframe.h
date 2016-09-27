// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Keyframe_h
#define Keyframe_h

#include "core/CoreExport.h"
#include "core/animation/AnimationEffect.h"
#include "core/animation/EffectModel.h"
#include "core/animation/PropertyHandle.h"
#include "core/animation/animatable/AnimatableValue.h"

namespace blink {

using PropertyHandleSet = HashSet<PropertyHandle>;

class Element;
class ComputedStyle;

// FIXME: Make Keyframe immutable
class CORE_EXPORT Keyframe : public RefCountedWillBeGarbageCollectedFinalized<Keyframe> {
public:
    virtual ~Keyframe() { }

    void setOffset(double offset) { m_offset = offset; }
    double offset() const { return m_offset; }

    void setComposite(EffectModel::CompositeOperation composite) { m_composite = composite; }
    EffectModel::CompositeOperation composite() const { return m_composite; }

    void setEasing(PassRefPtr<TimingFunction> easing) { m_easing = easing; }
    TimingFunction& easing() const { return *m_easing; }

    static bool compareOffsets(const RefPtrWillBeMember<Keyframe>& a, const RefPtrWillBeMember<Keyframe>& b)
    {
        return a->offset() < b->offset();
    }

    virtual PropertyHandleSet properties() const = 0;

    virtual PassRefPtrWillBeRawPtr<Keyframe> clone() const = 0;
    PassRefPtrWillBeRawPtr<Keyframe> cloneWithOffset(double offset) const
    {
        RefPtrWillBeRawPtr<Keyframe> theClone = clone();
        theClone->setOffset(offset);
        return theClone.release();
    }

    virtual bool isAnimatableValueKeyframe() const { return false; }
    virtual bool isStringKeyframe() const { return false; }

    DEFINE_INLINE_VIRTUAL_TRACE() { }

    class PropertySpecificKeyframe : public NoBaseWillBeGarbageCollectedFinalized<PropertySpecificKeyframe> {
    public:
        virtual ~PropertySpecificKeyframe() { }
        double offset() const { return m_offset; }
        TimingFunction& easing() const { return *m_easing; }
        EffectModel::CompositeOperation composite() const { return m_composite; }
        virtual PassOwnPtrWillBeRawPtr<PropertySpecificKeyframe> cloneWithOffset(double offset) const = 0;

        // FIXME: Remove this once CompositorAnimations no longer depends on AnimatableValues
        virtual void populateAnimatableValue(CSSPropertyID, Element&, const ComputedStyle* baseStyle) const { }
        virtual const PassRefPtrWillBeRawPtr<AnimatableValue> getAnimatableValue() const = 0;

        virtual bool isAnimatableValuePropertySpecificKeyframe() const { return false; }
        virtual bool isCSSPropertySpecificKeyframe() const { return false; }
        virtual bool isSVGPropertySpecificKeyframe() const { return false; }

        virtual PassOwnPtrWillBeRawPtr<PropertySpecificKeyframe> neutralKeyframe(double offset, PassRefPtr<TimingFunction> easing) const = 0;
        virtual PassRefPtrWillBeRawPtr<Interpolation> maybeCreateInterpolation(PropertyHandle, Keyframe::PropertySpecificKeyframe& end, Element*, const ComputedStyle* baseStyle) const = 0;

        DEFINE_INLINE_VIRTUAL_TRACE() { }

    protected:
        PropertySpecificKeyframe(double offset, PassRefPtr<TimingFunction> easing, EffectModel::CompositeOperation);

        double m_offset;
        RefPtr<TimingFunction> m_easing;
        EffectModel::CompositeOperation m_composite;
    };

    virtual PassOwnPtrWillBeRawPtr<PropertySpecificKeyframe> createPropertySpecificKeyframe(PropertyHandle) const = 0;

protected:
    Keyframe()
        : m_offset(nullValue())
        , m_composite(EffectModel::CompositeReplace)
        , m_easing(LinearTimingFunction::shared())
    {
    }
    Keyframe(double offset, EffectModel::CompositeOperation composite, PassRefPtr<TimingFunction> easing)
        : m_offset(offset)
        , m_composite(composite)
        , m_easing(easing)
    {
    }

    double m_offset;
    EffectModel::CompositeOperation m_composite;
    RefPtr<TimingFunction> m_easing;
};

} // namespace blink

#endif // Keyframe_h
