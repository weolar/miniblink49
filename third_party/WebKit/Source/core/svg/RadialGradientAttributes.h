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

#ifndef RadialGradientAttributes_h
#define RadialGradientAttributes_h

#include "core/svg/GradientAttributes.h"
#include "core/svg/SVGLength.h"

namespace blink {
struct RadialGradientAttributes final : GradientAttributes {
    DISALLOW_ALLOCATION();
public:
    RadialGradientAttributes()
        : m_cx(SVGLength::create(SVGLengthMode::Width))
        , m_cy(SVGLength::create(SVGLengthMode::Height))
        , m_r(SVGLength::create(SVGLengthMode::Other))
        , m_fx(SVGLength::create(SVGLengthMode::Width))
        , m_fy(SVGLength::create(SVGLengthMode::Height))
        , m_fr(SVGLength::create(SVGLengthMode::Other))
        , m_cxSet(false)
        , m_cySet(false)
        , m_rSet(false)
        , m_fxSet(false)
        , m_fySet(false)
        , m_frSet(false)
    {
        m_cx->setValueAsString("50%", IGNORE_EXCEPTION);
        m_cy->setValueAsString("50%", IGNORE_EXCEPTION);
        m_r->setValueAsString("50%", IGNORE_EXCEPTION);
    }

    SVGLength* cx() const { return m_cx.get(); }
    SVGLength* cy() const { return m_cy.get(); }
    SVGLength* r() const { return m_r.get(); }
    SVGLength* fx() const { return m_fx.get(); }
    SVGLength* fy() const { return m_fy.get(); }
    SVGLength* fr() const { return m_fr.get(); }

    void setCx(PassRefPtrWillBeRawPtr<SVGLength> value) { m_cx = value; m_cxSet = true; }
    void setCy(PassRefPtrWillBeRawPtr<SVGLength> value) { m_cy = value; m_cySet = true; }
    void setR(PassRefPtrWillBeRawPtr<SVGLength> value) { m_r = value; m_rSet = true; }
    void setFx(PassRefPtrWillBeRawPtr<SVGLength> value) { m_fx = value; m_fxSet = true; }
    void setFy(PassRefPtrWillBeRawPtr<SVGLength> value) { m_fy = value; m_fySet = true; }
    void setFr(PassRefPtrWillBeRawPtr<SVGLength> value) { m_fr = value; m_frSet = true; }

    bool hasCx() const { return m_cxSet; }
    bool hasCy() const { return m_cySet; }
    bool hasR() const { return m_rSet; }
    bool hasFx() const { return m_fxSet; }
    bool hasFy() const { return m_fySet; }
    bool hasFr() const { return m_frSet; }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_cx);
        visitor->trace(m_cy);
        visitor->trace(m_r);
        visitor->trace(m_fx);
        visitor->trace(m_fy);
        visitor->trace(m_fr);
    }

private:
    // Properties
    RefPtrWillBeMember<SVGLength> m_cx;
    RefPtrWillBeMember<SVGLength> m_cy;
    RefPtrWillBeMember<SVGLength> m_r;
    RefPtrWillBeMember<SVGLength> m_fx;
    RefPtrWillBeMember<SVGLength> m_fy;
    RefPtrWillBeMember<SVGLength> m_fr;

    // Property states
    bool m_cxSet : 1;
    bool m_cySet : 1;
    bool m_rSet : 1;
    bool m_fxSet : 1;
    bool m_fySet : 1;
    bool m_frSet : 1;
};

#if ENABLE(OILPAN)
// Wrapper object for the RadialGradientAttributes part object.
class RadialGradientAttributesWrapper : public GarbageCollectedFinalized<RadialGradientAttributesWrapper> {
public:
    static RadialGradientAttributesWrapper* create()
    {
        return new RadialGradientAttributesWrapper;
    }

    RadialGradientAttributes& attributes() { return m_attributes; }
    void set(const RadialGradientAttributes& attributes) { m_attributes = attributes; }
    DEFINE_INLINE_TRACE() { visitor->trace(m_attributes); }

private:
    RadialGradientAttributesWrapper()
    {
    }

    RadialGradientAttributes m_attributes;
};
#endif

} // namespace blink

#endif
