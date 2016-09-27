// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VREyeParameters_h
#define VREyeParameters_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/DOMPoint.h"
#include "core/dom/DOMRect.h"
#include "modules/vr/VRFieldOfView.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/vr/WebVR.h"
#include "wtf/Forward.h"

namespace blink {

class VREyeParameters final : public GarbageCollected<VREyeParameters>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static VREyeParameters* create()
    {
        return new VREyeParameters();
    }

    VRFieldOfView* minimumFieldOfView() const { return m_minimumFieldOfView; }
    VRFieldOfView* maximumFieldOfView() const { return m_maximumFieldOfView; }
    VRFieldOfView* recommendedFieldOfView() const { return m_recommendedFieldOfView; }
    DOMPoint* eyeTranslation() const { return m_eyeTranslation; }
    VRFieldOfView* currentFieldOfView() const { return m_currentFieldOfView; }
    DOMRect* renderRect() const { return m_renderRect; }

    void setFromWebVREyeParameters(const WebVREyeParameters&);

    DECLARE_VIRTUAL_TRACE()

private:
    VREyeParameters();

    Member<VRFieldOfView> m_minimumFieldOfView;
    Member<VRFieldOfView> m_maximumFieldOfView;
    Member<VRFieldOfView> m_recommendedFieldOfView;
    Member<DOMPoint> m_eyeTranslation;

    Member<VRFieldOfView> m_currentFieldOfView;
    Member<DOMRect> m_renderRect;
};

} // namespace blink

#endif // VREyeParameters_h
