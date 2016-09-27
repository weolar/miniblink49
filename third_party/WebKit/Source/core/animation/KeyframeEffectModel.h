/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef KeyframeEffectModel_h
#define KeyframeEffectModel_h

#include "core/CoreExport.h"
#include "core/animation/AnimationEffect.h"
#include "core/animation/EffectModel.h"
#include "core/animation/InterpolationEffect.h"
#include "core/animation/PropertyHandle.h"
#include "core/animation/StringKeyframe.h"
#include "core/animation/animatable/AnimatableValueKeyframe.h"
#include "platform/animation/TimingFunction.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/Vector.h"

namespace blink {

class Element;
class KeyframeEffectModelTest;

class CORE_EXPORT KeyframeEffectModelBase : public EffectModel {
public:
    // FIXME: Implement accumulation.

    using PropertySpecificKeyframeVector = WillBeHeapVector<OwnPtrWillBeMember<Keyframe::PropertySpecificKeyframe>>;
    class PropertySpecificKeyframeGroup : public NoBaseWillBeGarbageCollected<PropertySpecificKeyframeGroup> {
    public:
        void appendKeyframe(PassOwnPtrWillBeRawPtr<Keyframe::PropertySpecificKeyframe>);
        const PropertySpecificKeyframeVector& keyframes() const { return m_keyframes; }

        DECLARE_TRACE();

    private:
        void removeRedundantKeyframes();
        bool addSyntheticKeyframeIfRequired(PassRefPtr<TimingFunction> neutralKeyframeEasing);

        PropertySpecificKeyframeVector m_keyframes;

        friend class KeyframeEffectModelBase;
    };

    bool isReplaceOnly();

    PropertyHandleSet properties() const;

    using KeyframeVector = WillBeHeapVector<RefPtrWillBeMember<Keyframe>>;
    const KeyframeVector& getFrames() const { return m_keyframes; }
    void setFrames(KeyframeVector& keyframes);

    const PropertySpecificKeyframeVector& getPropertySpecificKeyframes(PropertyHandle property) const
    {
        ensureKeyframeGroups();
        return m_keyframeGroups->get(property)->keyframes();
    }

    // EffectModel implementation.
    virtual void sample(int iteration, double fraction, double iterationDuration, OwnPtrWillBeRawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>>&) const override;

    virtual bool isKeyframeEffectModel() const override { return true; }

    virtual bool isAnimatableValueKeyframeEffectModel() const { return false; }
    virtual bool isStringKeyframeEffectModel() const { return false; }

    bool hasSyntheticKeyframes() const
    {
        ensureKeyframeGroups();
        return m_hasSyntheticKeyframes;
    }

    DECLARE_VIRTUAL_TRACE();

    // FIXME: This is a hack used to resolve CSSValues to AnimatableValues while we have a valid handle on an element.
    // This should be removed once AnimatableValues are obsolete.
    void forceConversionsToAnimatableValues(Element&, const ComputedStyle* baseStyle);
    bool updateNeutralKeyframeAnimatableValues(CSSPropertyID, PassRefPtrWillBeRawPtr<AnimatableValue>);

    template<typename T>
    inline void forEachInterpolation(const T& callback) { m_interpolationEffect->forEachInterpolation(callback); }

    static KeyframeVector normalizedKeyframesForInspector(const KeyframeVector& keyframes) { return normalizedKeyframes(keyframes); }

    bool affects(PropertyHandle property) const override
    {
        ensureKeyframeGroups();
        return m_keyframeGroups->contains(property);
    }

    virtual bool isTransformRelatedEffect() const override;

protected:
    KeyframeEffectModelBase(PassRefPtrWillBeRawPtr<TimingFunction> neutralKeyframeEasing)
        : m_neutralKeyframeEasing(neutralKeyframeEasing)
    {
    }

    static KeyframeVector normalizedKeyframes(const KeyframeVector& keyframes);

    // Lazily computes the groups of property-specific keyframes.
    void ensureKeyframeGroups() const;
    void ensureInterpolationEffect(Element* = nullptr, const ComputedStyle* baseStyle = nullptr) const;
    void snapshotCompositableProperties(Element&, const ComputedStyle* baseStyle);

    KeyframeVector m_keyframes;
    // The spec describes filtering the normalized keyframes at sampling time
    // to get the 'property-specific keyframes'. For efficiency, we cache the
    // property-specific lists.
    using KeyframeGroupMap = WillBeHeapHashMap<PropertyHandle, OwnPtrWillBeMember<PropertySpecificKeyframeGroup>>;
    mutable OwnPtrWillBeMember<KeyframeGroupMap> m_keyframeGroups;
    mutable RefPtrWillBeMember<InterpolationEffect> m_interpolationEffect;
    RefPtr<TimingFunction> m_neutralKeyframeEasing;

    mutable bool m_hasSyntheticKeyframes;

    friend class KeyframeEffectModelTest;
};

template <class Keyframe>
class KeyframeEffectModel final : public KeyframeEffectModelBase {
public:
    using KeyframeVector = WillBeHeapVector<RefPtrWillBeMember<Keyframe>>;
    static PassRefPtrWillBeRawPtr<KeyframeEffectModel<Keyframe>> create(const KeyframeVector& keyframes, PassRefPtrWillBeRawPtr<TimingFunction> neutralKeyframeEasing = nullptr)
    {
        return adoptRefWillBeNoop(new KeyframeEffectModel(keyframes, neutralKeyframeEasing));
    }

private:
    KeyframeEffectModel(const KeyframeVector& keyframes, PassRefPtrWillBeRawPtr<TimingFunction> neutralKeyframeEasing)
        : KeyframeEffectModelBase(neutralKeyframeEasing)
    {
        m_keyframes.appendVector(keyframes);
    }

    virtual bool isAnimatableValueKeyframeEffectModel() const { return false; }
    virtual bool isStringKeyframeEffectModel() const { return false; }
};

using KeyframeVector = KeyframeEffectModelBase::KeyframeVector;
using PropertySpecificKeyframeVector = KeyframeEffectModelBase::PropertySpecificKeyframeVector;

using AnimatableValueKeyframeEffectModel = KeyframeEffectModel<AnimatableValueKeyframe>;
using AnimatableValueKeyframeVector = AnimatableValueKeyframeEffectModel::KeyframeVector;
using AnimatableValuePropertySpecificKeyframeVector = AnimatableValueKeyframeEffectModel::PropertySpecificKeyframeVector;

using StringKeyframeEffectModel = KeyframeEffectModel<StringKeyframe>;
using StringKeyframeVector = StringKeyframeEffectModel::KeyframeVector;
using StringPropertySpecificKeyframeVector = StringKeyframeEffectModel::PropertySpecificKeyframeVector;

DEFINE_TYPE_CASTS(KeyframeEffectModelBase, EffectModel, value, value->isKeyframeEffectModel(), value.isKeyframeEffectModel());
DEFINE_TYPE_CASTS(AnimatableValueKeyframeEffectModel, KeyframeEffectModelBase, value, value->isAnimatableValueKeyframeEffectModel(), value.isAnimatableValueKeyframeEffectModel());
DEFINE_TYPE_CASTS(StringKeyframeEffectModel, KeyframeEffectModelBase, value, value->isStringKeyframeEffectModel(), value.isStringKeyframeEffectModel());

inline const AnimatableValueKeyframeEffectModel* toAnimatableValueKeyframeEffectModel(const EffectModel* base)
{
    return toAnimatableValueKeyframeEffectModel(toKeyframeEffectModelBase(base));
}

inline AnimatableValueKeyframeEffectModel* toAnimatableValueKeyframeEffectModel(EffectModel* base)
{
    return toAnimatableValueKeyframeEffectModel(toKeyframeEffectModelBase(base));
}

inline const StringKeyframeEffectModel* toStringKeyframeEffectModel(const EffectModel* base)
{
    return toStringKeyframeEffectModel(toKeyframeEffectModelBase(base));
}

inline StringKeyframeEffectModel* toStringKeyframeEffectModel(EffectModel* base)
{
    return toStringKeyframeEffectModel(toKeyframeEffectModelBase(base));
}

template <>
inline bool KeyframeEffectModel<AnimatableValueKeyframe>::isAnimatableValueKeyframeEffectModel() const { return true; }

template <>
inline bool KeyframeEffectModel<StringKeyframe>::isStringKeyframeEffectModel() const { return true; }

} // namespace blink

#endif // KeyframeEffectModel_h
