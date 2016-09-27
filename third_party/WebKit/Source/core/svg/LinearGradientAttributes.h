/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef LinearGradientAttributes_h
#define LinearGradientAttributes_h

#include "core/svg/GradientAttributes.h"
#include "core/svg/SVGLength.h"
#include "platform/heap/Handle.h"

namespace blink {

struct LinearGradientAttributes : GradientAttributes {
    DISALLOW_ALLOCATION();
public:
    LinearGradientAttributes()
        : m_x1(SVGLength::create(SVGLengthMode::Width))
        , m_y1(SVGLength::create(SVGLengthMode::Height))
        , m_x2(SVGLength::create(SVGLengthMode::Width))
        , m_y2(SVGLength::create(SVGLengthMode::Height))
        , m_x1Set(false)
        , m_y1Set(false)
        , m_x2Set(false)
        , m_y2Set(false)
    {
        m_x2->setValueAsString("100%", ASSERT_NO_EXCEPTION);
    }

    SVGLength* x1() const { return m_x1.get(); }
    SVGLength* y1() const { return m_y1.get(); }
    SVGLength* x2() const { return m_x2.get(); }
    SVGLength* y2() const { return m_y2.get(); }

    void setX1(PassRefPtrWillBeRawPtr<SVGLength> value) { m_x1 = value; m_x1Set = true; }
    void setY1(PassRefPtrWillBeRawPtr<SVGLength> value) { m_y1 = value; m_y1Set = true; }
    void setX2(PassRefPtrWillBeRawPtr<SVGLength> value) { m_x2 = value; m_x2Set = true; }
    void setY2(PassRefPtrWillBeRawPtr<SVGLength> value) { m_y2 = value; m_y2Set = true; }

    bool hasX1() const { return m_x1Set; }
    bool hasY1() const { return m_y1Set; }
    bool hasX2() const { return m_x2Set; }
    bool hasY2() const { return m_y2Set; }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_x1);
        visitor->trace(m_y1);
        visitor->trace(m_x2);
        visitor->trace(m_y2);
    }

private:
    // Properties
    RefPtrWillBeMember<SVGLength> m_x1;
    RefPtrWillBeMember<SVGLength> m_y1;
    RefPtrWillBeMember<SVGLength> m_x2;
    RefPtrWillBeMember<SVGLength> m_y2;

    // Property states
    bool m_x1Set : 1;
    bool m_y1Set : 1;
    bool m_x2Set : 1;
    bool m_y2Set : 1;
};

#if ENABLE(OILPAN)
// Wrapper object for the LinearGradientAttributes part object.
class LinearGradientAttributesWrapper : public GarbageCollectedFinalized<LinearGradientAttributesWrapper> {
public:
    static LinearGradientAttributesWrapper* create()
    {
        return new LinearGradientAttributesWrapper;
    }

    LinearGradientAttributes& attributes() { return m_attributes; }
    void set(const LinearGradientAttributes& attributes) { m_attributes = attributes; }
    DEFINE_INLINE_TRACE() { visitor->trace(m_attributes); }

private:
    LinearGradientAttributesWrapper()
    {
    }

    LinearGradientAttributes m_attributes;
};
#endif

} // namespace blink

#endif

// vim:ts=4:noet
