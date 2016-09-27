/*
 * Copyright (C) 2012 Google, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL UNIVERSITY OF SZEGED OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutSVGEllipse_h
#define LayoutSVGEllipse_h

#include "core/layout/svg/LayoutSVGShape.h"

namespace blink {

class LayoutSVGEllipse final : public LayoutSVGShape {
public:
    explicit LayoutSVGEllipse(SVGGeometryElement*);
    virtual ~LayoutSVGEllipse();

    virtual ShapeGeometryCodePath geometryCodePath() const override { return m_usePathFallback ? PathGeometry : EllipseGeometryFastPath; }

    virtual const char* name() const override { return "LayoutSVGEllipse"; }

private:
    void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;
    void updateShapeFromElement() override;
    void updateStrokeAndFillBoundingBoxes() override;
    virtual bool isShapeEmpty() const override { return m_usePathFallback ? LayoutSVGShape::isShapeEmpty() : m_fillBoundingBox.isEmpty(); }
    virtual bool shapeDependentStrokeContains(const FloatPoint&) override;
    virtual bool shapeDependentFillContains(const FloatPoint&, const WindRule) const override;
    void calculateRadiiAndCenter();
    bool hasContinuousStroke(const SVGComputedStyle&) const;

private:
    FloatPoint m_center;
    FloatSize m_radii;
    bool m_usePathFallback;
};

}

#endif
