// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/screen_orientation/ScreenOrientationInspectorAgent.h"

#include "core/InspectorTypeBuilder.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/InspectorState.h"
#include "modules/screen_orientation/ScreenOrientation.h"
#include "modules/screen_orientation/ScreenOrientationController.h"

namespace blink {

namespace ScreenOrientationInspectorAgentState {
static const char angle[] = "angle";
static const char type[] = "type";
static const char overrideEnabled[] = "overrideEnabled";
}

namespace {

WebScreenOrientationType WebScreenOrientationTypeFromString(const String& type)
{
    if (type == TypeBuilder::getEnumConstantValue(TypeBuilder::ScreenOrientation::OrientationType::PortraitPrimary))
        return WebScreenOrientationPortraitPrimary;
    if (type == TypeBuilder::getEnumConstantValue(TypeBuilder::ScreenOrientation::OrientationType::PortraitSecondary))
        return WebScreenOrientationPortraitSecondary;
    if (type == TypeBuilder::getEnumConstantValue(TypeBuilder::ScreenOrientation::OrientationType::LandscapePrimary))
        return WebScreenOrientationLandscapePrimary;
    if (type == TypeBuilder::getEnumConstantValue(TypeBuilder::ScreenOrientation::OrientationType::LandscapeSecondary))
        return WebScreenOrientationLandscapeSecondary;
    return WebScreenOrientationUndefined;
}

} // namespace

// static
PassOwnPtrWillBeRawPtr<ScreenOrientationInspectorAgent> ScreenOrientationInspectorAgent::create(LocalFrame& frame)
{
    return adoptPtrWillBeNoop(new ScreenOrientationInspectorAgent(frame));
}

ScreenOrientationInspectorAgent::~ScreenOrientationInspectorAgent()
{
}

ScreenOrientationInspectorAgent::ScreenOrientationInspectorAgent(LocalFrame& frame)
    : InspectorBaseAgent<ScreenOrientationInspectorAgent, InspectorFrontend::ScreenOrientation>("ScreenOrientation")
    , m_frame(frame)
{
}

void ScreenOrientationInspectorAgent::setScreenOrientationOverride(ErrorString* error, int angle, const String& typeString)
{
    if (angle < 0 || angle >= 360) {
        *error = "Angle should be in [0; 360) interval";
        return;
    }
    WebScreenOrientationType type = WebScreenOrientationTypeFromString(typeString);
    if (type == WebScreenOrientationUndefined) {
        *error = "Wrong type value";
        return;
    }
    ScreenOrientationController* controller = ScreenOrientationController::from(m_frame);
    if (!controller) {
        *error = "Cannot connect to orientation controller";
        return;
    }
    m_state->setBoolean(ScreenOrientationInspectorAgentState::overrideEnabled, true);
    m_state->setLong(ScreenOrientationInspectorAgentState::angle, angle);
    m_state->setLong(ScreenOrientationInspectorAgentState::type, type);
    controller->setOverride(type, angle);
}

void ScreenOrientationInspectorAgent::clearScreenOrientationOverride(ErrorString* error)
{
    ScreenOrientationController* controller = ScreenOrientationController::from(m_frame);
    if (!controller) {
        *error = "Cannot connect to orientation controller";
        return;
    }
    m_state->setBoolean(ScreenOrientationInspectorAgentState::overrideEnabled, false);
    controller->clearOverride();
}

void ScreenOrientationInspectorAgent::disable(ErrorString*)
{
    m_state->setBoolean(ScreenOrientationInspectorAgentState::overrideEnabled, false);
    if (ScreenOrientationController* controller = ScreenOrientationController::from(m_frame))
        controller->clearOverride();
}

void ScreenOrientationInspectorAgent::restore()
{
    if (m_state->getBoolean(ScreenOrientationInspectorAgentState::overrideEnabled)) {
        WebScreenOrientationType type = static_cast<WebScreenOrientationType>(m_state->getLong(ScreenOrientationInspectorAgentState::type));
        int angle = m_state->getLong(ScreenOrientationInspectorAgentState::angle);
        if (ScreenOrientationController* controller = ScreenOrientationController::from(m_frame))
            controller->setOverride(type, angle);
    }
}

} // namespace blink
