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

#ifndef SVGMatrixTearOff_h
#define SVGMatrixTearOff_h

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "platform/transforms/AffineTransform.h"
#include "wtf/RefCounted.h"

namespace blink {

class SVGTransformTearOff;

// SVGMatrixTearOff wraps a AffineTransform for Javascript.
// Its instance can either hold a static value, or this can be teared off from |SVGTransform.matrix|.
// This does not derive from SVGPropertyTearOff, as its instances are never tied to an animated property nor an XML attribute.
class CORE_EXPORT SVGMatrixTearOff final : public RefCountedWillBeGarbageCollectedFinalized<SVGMatrixTearOff>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGMatrixTearOff> create(const AffineTransform& value)
    {
        return adoptRefWillBeNoop(new SVGMatrixTearOff(value));
    }

    static PassRefPtrWillBeRawPtr<SVGMatrixTearOff> create(SVGTransformTearOff* target)
    {
        return adoptRefWillBeNoop(new SVGMatrixTearOff(target));
    }

    ~SVGMatrixTearOff();

    double a() { return value().a(); }
    double b() { return value().b(); }
    double c() { return value().c(); }
    double d() { return value().d(); }
    double e() { return value().e(); }
    double f() { return value().f(); }

    void setA(double, ExceptionState&);
    void setB(double, ExceptionState&);
    void setC(double, ExceptionState&);
    void setD(double, ExceptionState&);
    void setE(double, ExceptionState&);
    void setF(double, ExceptionState&);

    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> translate(double tx, double ty);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> scale(double);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> scaleNonUniform(double sx, double sy);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> rotate(double);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> flipX();
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> flipY();
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> skewX(double);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> skewY(double);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> multiply(PassRefPtrWillBeRawPtr<SVGMatrixTearOff>);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> inverse(ExceptionState&);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> rotateFromVector(double x, double y, ExceptionState&);

    SVGTransformTearOff* contextTransform() { return m_contextTransform; }

    const AffineTransform& value() const;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGMatrixTearOff(const AffineTransform&);
    explicit SVGMatrixTearOff(SVGTransformTearOff*);

    AffineTransform* mutableValue();
    void commitChange();

    AffineTransform m_staticValue;

    RawPtrWillBeMember<SVGTransformTearOff> m_contextTransform;
};

} // namespace blink

#endif // SVGMatrixTearOff_h
