/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
G*     * Redistributions in binary form must reproduce the above
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

#ifndef SVGAnimatedProperty_h
#define SVGAnimatedProperty_h

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGParsingError.h"
#include "core/svg/properties/SVGPropertyInfo.h"
#include "core/svg/properties/SVGPropertyTearOff.h"
#include "platform/heap/Handle.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class SVGElement;

class SVGAnimatedPropertyBase : public RefCountedWillBeGarbageCollectedFinalized<SVGAnimatedPropertyBase>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO_NOT_REACHED();
    WTF_MAKE_NONCOPYABLE(SVGAnimatedPropertyBase);
public:
    virtual ~SVGAnimatedPropertyBase();

    virtual SVGPropertyBase* currentValueBase() = 0;
    virtual bool isAnimating() const = 0;

    virtual PassRefPtrWillBeRawPtr<SVGPropertyBase> createAnimatedValue() = 0;
    virtual void setAnimatedValue(PassRefPtrWillBeRawPtr<SVGPropertyBase>) = 0;
    virtual void animationEnded();

    virtual void setBaseValueAsString(const String& value, SVGParsingError& parseError) = 0;
    virtual bool needsSynchronizeAttribute() = 0;
    virtual void synchronizeAttribute();

    AnimatedPropertyType type() const
    {
        return m_type;
    }

    SVGElement* contextElement() const
    {
        return m_contextElement;
    }

    const QualifiedName& attributeName() const
    {
        return m_attributeName;
    }

    bool isReadOnly() const
    {
        return m_isReadOnly;
    }

    void setReadOnly()
    {
        m_isReadOnly = true;
    }

    bool isSpecified() const;

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_contextElement);
    }

protected:
    SVGAnimatedPropertyBase(AnimatedPropertyType, SVGElement*, const QualifiedName& attributeName);

private:
    const AnimatedPropertyType m_type;
    bool m_isReadOnly;

    // This reference is kept alive from V8 wrapper
    RawPtrWillBeMember<SVGElement> m_contextElement;

    const QualifiedName& m_attributeName;
};

template <typename Property>
class SVGAnimatedPropertyCommon : public SVGAnimatedPropertyBase {
public:
    Property* baseValue()
    {
        return m_baseValue.get();
    }

    Property* currentValue()
    {
        return m_currentValue ? m_currentValue.get() : m_baseValue.get();
    }

    const Property* currentValue() const
    {
        return const_cast<SVGAnimatedPropertyCommon*>(this)->currentValue();
    }

    SVGPropertyBase* currentValueBase() override
    {
        return currentValue();
    }

    bool isAnimating() const override
    {
        return m_currentValue;
    }

    void setBaseValueAsString(const String& value, SVGParsingError& parseError) override
    {
        TrackExceptionState es;

        m_baseValue->setValueAsString(value, es);

        if (es.hadException())
            parseError = ParsingAttributeFailedError;
    }

    PassRefPtrWillBeRawPtr<SVGPropertyBase> createAnimatedValue() override
    {
        return m_baseValue->clone();
    }

    void setAnimatedValue(PassRefPtrWillBeRawPtr<SVGPropertyBase> passValue) override
    {
        RefPtrWillBeRawPtr<SVGPropertyBase> value = passValue;
        ASSERT(value->type() == Property::classType());
        m_currentValue = static_pointer_cast<Property>(value.release());
    }

    void animationEnded() override
    {
        m_currentValue.clear();

        SVGAnimatedPropertyBase::animationEnded();
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_baseValue);
        visitor->trace(m_currentValue);
        SVGAnimatedPropertyBase::trace(visitor);
    }

protected:
    SVGAnimatedPropertyCommon(SVGElement* contextElement, const QualifiedName& attributeName, PassRefPtrWillBeRawPtr<Property> initialValue)
        : SVGAnimatedPropertyBase(Property::classType(), contextElement, attributeName)
        , m_baseValue(initialValue)
    {
    }

private:
    RefPtrWillBeMember<Property> m_baseValue;
    RefPtrWillBeMember<Property> m_currentValue;
};

// Implementation of SVGAnimatedProperty which uses primitive types.
// This is for classes which return primitive type for its "animVal".
// Examples are SVGAnimatedBoolean, SVGAnimatedNumber, etc.
template <typename Property, typename TearOffType = typename Property::TearOffType, typename PrimitiveType = typename Property::PrimitiveType>
class SVGAnimatedProperty : public SVGAnimatedPropertyCommon<Property> {
public:
    bool needsSynchronizeAttribute() override
    {
        // DOM attribute synchronization is only needed if tear-off is being touched from javascript or the property is being animated.
        // This prevents unnecessary attribute creation on target element.
        return m_baseValueUpdated || this->isAnimating();
    }

    void synchronizeAttribute() override
    {
        SVGAnimatedPropertyBase::synchronizeAttribute();
        m_baseValueUpdated = false;
    }

    // SVGAnimated* DOM Spec implementations:

    // baseVal()/setBaseVal()/animVal() are only to be used from SVG DOM implementation.
    // Use currentValue() from C++ code.
    PrimitiveType baseVal()
    {
        return this->baseValue()->value();
    }

    void setBaseVal(PrimitiveType value, ExceptionState& exceptionState)
    {
        if (this->isReadOnly()) {
            exceptionState.throwDOMException(NoModificationAllowedError, "The attribute is read-only.");
            return;
        }

        this->baseValue()->setValue(value);
        m_baseValueUpdated = true;

        ASSERT(this->attributeName() != QualifiedName::null());
        this->contextElement()->invalidateSVGAttributes();
        this->contextElement()->svgAttributeChanged(this->attributeName());
    }

    PrimitiveType animVal()
    {
        return this->currentValue()->value();
    }

protected:
    SVGAnimatedProperty(SVGElement* contextElement, const QualifiedName& attributeName, PassRefPtrWillBeRawPtr<Property> initialValue)
        : SVGAnimatedPropertyCommon<Property>(contextElement, attributeName, initialValue)
        , m_baseValueUpdated(false)
    {
    }

    bool m_baseValueUpdated;
};

// Implementation of SVGAnimatedProperty which uses tear-off value types.
// These classes has "void" for its PrimitiveType.
// This is for classes which return special type for its "animVal".
// Examples are SVGAnimatedLength, SVGAnimatedRect, SVGAnimated*List, etc.
template <typename Property, typename TearOffType>
class SVGAnimatedProperty<Property, TearOffType, void> : public SVGAnimatedPropertyCommon<Property> {
public:
    static PassRefPtrWillBeRawPtr<SVGAnimatedProperty<Property>> create(SVGElement* contextElement, const QualifiedName& attributeName, PassRefPtrWillBeRawPtr<Property> initialValue)
    {
        return adoptRefWillBeNoop(new SVGAnimatedProperty<Property>(contextElement, attributeName, initialValue));
    }

    void setAnimatedValue(PassRefPtrWillBeRawPtr<SVGPropertyBase> value) override
    {
        SVGAnimatedPropertyCommon<Property>::setAnimatedValue(value);
        updateAnimValTearOffIfNeeded();
    }

    void animationEnded() override
    {
        SVGAnimatedPropertyCommon<Property>::animationEnded();
        updateAnimValTearOffIfNeeded();
    }

    bool needsSynchronizeAttribute() override
    {
        // DOM attribute synchronization is only needed if tear-off is being touched from javascript or the property is being animated.
        // This prevents unnecessary attribute creation on target element.
        return m_baseValTearOff || this->isAnimating();
    }

    // SVGAnimated* DOM Spec implementations:

    // baseVal()/animVal() are only to be used from SVG DOM implementation.
    // Use currentValue() from C++ code.
    virtual TearOffType* baseVal()
    {
        if (!m_baseValTearOff) {
            m_baseValTearOff = TearOffType::create(this->baseValue(), this->contextElement(), PropertyIsNotAnimVal, this->attributeName());
            if (this->isReadOnly())
                m_baseValTearOff->setIsReadOnlyProperty();
        }

        return m_baseValTearOff.get();
    }

    TearOffType* animVal()
    {
        if (!m_animValTearOff)
            m_animValTearOff = TearOffType::create(this->currentValue(), this->contextElement(), PropertyIsAnimVal, this->attributeName());

        return m_animValTearOff.get();
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_baseValTearOff);
        visitor->trace(m_animValTearOff);
        SVGAnimatedPropertyCommon<Property>::trace(visitor);
    }

protected:
    SVGAnimatedProperty(SVGElement* contextElement, const QualifiedName& attributeName, PassRefPtrWillBeRawPtr<Property> initialValue)
        : SVGAnimatedPropertyCommon<Property>(contextElement, attributeName, initialValue)
    {
    }

private:
    void updateAnimValTearOffIfNeeded()
    {
        if (m_animValTearOff)
            m_animValTearOff->setTarget(this->currentValue());
    }

    // When still (not animated):
    //     Both m_animValTearOff and m_baseValTearOff target m_baseValue.
    // When animated:
    //     m_animValTearOff targets m_currentValue.
    //     m_baseValTearOff targets m_baseValue.
    RefPtrWillBeMember<TearOffType> m_baseValTearOff;
    RefPtrWillBeMember<TearOffType> m_animValTearOff;
};

} // namespace blink

#endif // SVGAnimatedProperty_h
