// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/vr/VREyeParameters.h"

namespace blink {

namespace {

void setDomPoint(DOMPoint* point, const WebVRVector3& vec)
{
    point->setX(vec.x);
    point->setY(vec.y);
    point->setZ(vec.z);
    point->setW(0.0);
}

} // namespace

VREyeParameters::VREyeParameters()
{
    m_minimumFieldOfView = new VRFieldOfView();
    m_maximumFieldOfView = new VRFieldOfView();
    m_recommendedFieldOfView = new VRFieldOfView();
    m_eyeTranslation = DOMPoint::create(0, 0, 0, 0);

    m_currentFieldOfView = new VRFieldOfView();
    m_renderRect = DOMRect::create(0, 0, 0, 0);
}

void VREyeParameters::setFromWebVREyeParameters(const WebVREyeParameters &state)
{
    // FIXME: We should expose proper min/max FOV eventually but for now set the
    // min/max equal to the recommended FOV to reduce need for synchronous
    // queries and reduce rendering complexity.
    m_minimumFieldOfView->setFromWebVRFieldOfView(state.recommendedFieldOfView);
    m_maximumFieldOfView->setFromWebVRFieldOfView(state.recommendedFieldOfView);
    m_recommendedFieldOfView->setFromWebVRFieldOfView(state.recommendedFieldOfView);
    setDomPoint(m_eyeTranslation, state.eyeTranslation);

    m_currentFieldOfView->setFromWebVRFieldOfView(state.recommendedFieldOfView);
    m_renderRect = DOMRect::create(state.renderRect.x, state.renderRect.y, state.renderRect.width, state.renderRect.height);
}

DEFINE_TRACE(VREyeParameters)
{
    visitor->trace(m_minimumFieldOfView);
    visitor->trace(m_maximumFieldOfView);
    visitor->trace(m_recommendedFieldOfView);
    visitor->trace(m_eyeTranslation);
    visitor->trace(m_currentFieldOfView);
    visitor->trace(m_renderRect);
}

} // namespace blink
