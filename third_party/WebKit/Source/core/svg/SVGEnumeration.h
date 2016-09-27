/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#ifndef SVGEnumeration_h
#define SVGEnumeration_h

#include "core/svg/properties/SVGProperty.h"

namespace blink {

class ExceptionState;

class SVGEnumerationBase : public SVGPropertyBase {
public:
    typedef std::pair<unsigned short, String> StringEntry;
    typedef Vector<StringEntry> StringEntries;

    // SVGEnumeration does not have a tear-off type.
    typedef void TearOffType;
    typedef unsigned short PrimitiveType;

    ~SVGEnumerationBase() override;

    unsigned short value() const { return m_value <= maxExposedEnumValue() ? m_value : 0; }
    void setValue(unsigned short, ExceptionState&);

    // SVGPropertyBase:
    virtual PassRefPtrWillBeRawPtr<SVGEnumerationBase> clone() const = 0;
    PassRefPtrWillBeRawPtr<SVGPropertyBase> cloneForAnimation(const String&) const override;

    String valueAsString() const override;
    void setValueAsString(const String&, ExceptionState&);

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement*) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement*) override;

    static AnimatedPropertyType classType() { return AnimatedEnumeration; }

    // Ensure that |SVGAnimatedEnumerationBase::setBaseVal| is used instead of |SVGAnimatedProperty<SVGEnumerationBase>::setBaseVal|.
    void setValue(unsigned short) { ASSERT_NOT_REACHED(); }

    static unsigned short valueOfLastEnum(const StringEntries& entries) { return entries.last().first; }

protected:
    SVGEnumerationBase(unsigned short value, const StringEntries& entries, unsigned short maxExposed)
        : SVGPropertyBase(classType())
        , m_value(value)
        , m_maxExposed(maxExposed)
        , m_entries(entries)
    {
    }

    // This is the maximum value of all the internal enumeration values.
    // This assumes that |m_entries| are sorted.
    unsigned short maxInternalEnumValue() const { return valueOfLastEnum(m_entries); }

    // This is the maximum value that is exposed as an IDL constant on the relevant interface.
    unsigned short maxExposedEnumValue() const { return m_maxExposed; }

    // Used by SVGMarkerOrientEnumeration.
    virtual void notifyChange() { }

    unsigned short m_value;
    const unsigned short m_maxExposed;
    const StringEntries& m_entries;
};
typedef SVGEnumerationBase::StringEntries SVGEnumerationStringEntries;

template<typename Enum> const SVGEnumerationStringEntries& getStaticStringEntries();
template<typename Enum> unsigned short getMaxExposedEnumValue()
{
    return SVGEnumerationBase::valueOfLastEnum(getStaticStringEntries<Enum>());
}

template<typename Enum>
class SVGEnumeration : public SVGEnumerationBase {
public:
    static PassRefPtrWillBeRawPtr<SVGEnumeration<Enum>> create(Enum newValue)
    {
        return adoptRefWillBeNoop(new SVGEnumeration<Enum>(newValue));
    }

    ~SVGEnumeration() override {}

    PassRefPtrWillBeRawPtr<SVGEnumerationBase> clone() const override
    {
        return create(enumValue());
    }

    Enum enumValue() const
    {
        ASSERT(m_value <= maxInternalEnumValue());
        return static_cast<Enum>(m_value);
    }

    void setEnumValue(Enum value)
    {
        m_value = value;
        notifyChange();
    }

protected:
    explicit SVGEnumeration(Enum newValue)
        : SVGEnumerationBase(newValue, getStaticStringEntries<Enum>(), getMaxExposedEnumValue<Enum>())
    {
    }
};

} // namespace blink

#endif // SVGEnumeration_h
