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

#ifndef SVGAngleTearOff_h
#define SVGAngleTearOff_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/svg/SVGAngle.h"
#include "core/svg/properties/SVGPropertyTearOff.h"

namespace blink {

class SVGAngleTearOff final : public SVGPropertyTearOff<SVGAngle>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGAngleTearOff> create(PassRefPtrWillBeRawPtr<SVGAngle> target, SVGElement* contextElement, PropertyIsAnimValType propertyIsAnimVal, const QualifiedName& attributeName = QualifiedName::null())
    {
        return adoptRefWillBeNoop(new SVGAngleTearOff(target, contextElement, propertyIsAnimVal, attributeName));
    }

    enum {
        SVG_ANGLETYPE_UNKNOWN = SVGAngle::SVG_ANGLETYPE_UNKNOWN,
        SVG_ANGLETYPE_UNSPECIFIED = SVGAngle::SVG_ANGLETYPE_UNSPECIFIED,
        SVG_ANGLETYPE_DEG = SVGAngle::SVG_ANGLETYPE_DEG,
        SVG_ANGLETYPE_RAD = SVGAngle::SVG_ANGLETYPE_RAD,
        SVG_ANGLETYPE_GRAD = SVGAngle::SVG_ANGLETYPE_GRAD
    };

    ~SVGAngleTearOff() override;

    unsigned short unitType() { return hasExposedAngleUnit() ? target()->unitType() : SVGAngle::SVG_ANGLETYPE_UNKNOWN; }

    void setValue(float, ExceptionState&);
    float value() { return target()->value(); }

    void setValueInSpecifiedUnits(float, ExceptionState&);
    float valueInSpecifiedUnits() { return target()->valueInSpecifiedUnits(); }

    void newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits, ExceptionState&);
    void convertToSpecifiedUnits(unsigned short unitType, ExceptionState&);

    String valueAsString() { return hasExposedAngleUnit() ? target()->valueAsString() : String::number(0); }
    void setValueAsString(const String&, ExceptionState&);

private:
    SVGAngleTearOff(PassRefPtrWillBeRawPtr<SVGAngle>, SVGElement*, PropertyIsAnimValType, const QualifiedName&);

    bool hasExposedAngleUnit() { return target()->unitType() <= SVGAngle::SVG_ANGLETYPE_GRAD; }
};

} // namespace blink

#endif // SVGAngleTearOff_h
