/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#include "config.h"
#include "core/inspector/InspectorInputAgent.h"

#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/input/EventHandler.h"
#include "core/inspector/InspectorPageAgent.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "platform/JSONValues.h"
#include "platform/PlatformTouchEvent.h"
#include "platform/PlatformTouchPoint.h"
#include "platform/geometry/FloatSize.h"
#include "platform/geometry/IntPoint.h"
#include "platform/geometry/IntRect.h"
#include "platform/geometry/IntSize.h"
#include "wtf/CurrentTime.h"

namespace {

class SyntheticInspectorTouchPoint : public blink::PlatformTouchPoint {
public:
    SyntheticInspectorTouchPoint(int id, State state, const blink::IntPoint& screenPos, const blink::IntPoint& pos, int radiusX, int radiusY, double rotationAngle, double force)
    {
        m_id = id;
        m_screenPos = screenPos;
        m_pos = pos;
        m_state = state;
        m_radius = blink::FloatSize(radiusX, radiusY);
        m_rotationAngle = rotationAngle;
        m_force = force;
    }
};

class SyntheticInspectorTouchEvent : public blink::PlatformTouchEvent {
public:
    SyntheticInspectorTouchEvent(const blink::PlatformEvent::Type type, unsigned modifiers, double timestamp)
    {
        m_type = type;
        m_modifiers = modifiers;
        m_timestamp = timestamp;
    }

    void append(const blink::PlatformTouchPoint& point)
    {
        m_touchPoints.append(point);
    }
};

void ConvertInspectorPoint(blink::LocalFrame* frame, const blink::IntPoint& point, blink::IntPoint* convertedPoint, blink::IntPoint* globalPoint)
{
    *convertedPoint = frame->view()->convertToContainingWindow(point);
    *globalPoint = frame->page()->chromeClient().viewportToScreen(blink::IntRect(point, blink::IntSize(0, 0))).location();
}

} // namespace

namespace blink {

InspectorInputAgent::InspectorInputAgent(InspectorPageAgent* pageAgent)
    : InspectorBaseAgent<InspectorInputAgent, InspectorFrontend::Input>("Input")
    , m_pageAgent(pageAgent)
{
}

InspectorInputAgent::~InspectorInputAgent()
{
}

void InspectorInputAgent::dispatchTouchEvent(ErrorString* error, const String& type, const RefPtr<JSONArray>& touchPoints, const int* modifiers, const double* timestamp)
{
    PlatformEvent::Type convertedType;
    if (type == "touchStart") {
        convertedType = PlatformEvent::TouchStart;
    } else if (type == "touchEnd") {
        convertedType = PlatformEvent::TouchEnd;
    } else if (type == "touchMove") {
        convertedType = PlatformEvent::TouchMove;
    } else {
        *error = "Unrecognized type: " + type;
        return;
    }

    unsigned convertedModifiers = modifiers ? *modifiers : 0;

    SyntheticInspectorTouchEvent event(convertedType, convertedModifiers, timestamp ? *timestamp : currentTime());

    int autoId = 0;
    for (auto& touchPoint : *touchPoints) {
        RefPtr<JSONObject> pointObj;
        String state;
        int x, y, radiusX, radiusY, id;
        double rotationAngle, force;
        touchPoint->asObject(&pointObj);
        if (!pointObj->getString("state", &state)) {
            *error = "TouchPoint missing 'state'";
            return;
        }
        if (!pointObj->getNumber("x", &x)) {
            *error = "TouchPoint missing 'x' coordinate";
            return;
        }
        if (!pointObj->getNumber("y", &y)) {
            *error = "TouchPoint missing 'y' coordinate";
            return;
        }
        if (!pointObj->getNumber("radiusX", &radiusX))
            radiusX = 1;
        if (!pointObj->getNumber("radiusY", &radiusY))
            radiusY = 1;
        if (!pointObj->getNumber("rotationAngle", &rotationAngle))
            rotationAngle = 0.0f;
        if (!pointObj->getNumber("force", &force))
            force = 1.0f;
        if (pointObj->getNumber("id", &id)) {
            if (autoId > 0)
                id = -1;
            autoId = -1;
        } else {
            id = autoId++;
        }
        if (id < 0) {
            *error = "All or none of the provided TouchPoints must supply positive integer ids.";
            return;
        }

        PlatformTouchPoint::State convertedState;
        if (state == "touchPressed") {
            convertedState = PlatformTouchPoint::TouchPressed;
        } else if (state == "touchReleased") {
            convertedState = PlatformTouchPoint::TouchReleased;
        } else if (state == "touchMoved") {
            convertedState = PlatformTouchPoint::TouchMoved;
        } else if (state == "touchStationary") {
            convertedState = PlatformTouchPoint::TouchStationary;
        } else if (state == "touchCancelled") {
            convertedState = PlatformTouchPoint::TouchCancelled;
        } else {
            *error = "Unrecognized state: " + state;
            return;
        }

        // Some platforms may have flipped coordinate systems, but the given coordinates
        // assume the origin is in the top-left of the window. Convert.
        IntPoint convertedPoint, globalPoint;
        ConvertInspectorPoint(m_pageAgent->inspectedFrame(), IntPoint(x, y), &convertedPoint, &globalPoint);

        SyntheticInspectorTouchPoint point(id++, convertedState, globalPoint, convertedPoint, radiusX, radiusY, rotationAngle, force);
        event.append(point);
    }

    m_pageAgent->inspectedFrame()->eventHandler().handleTouchEvent(event);
}

DEFINE_TRACE(InspectorInputAgent)
{
    visitor->trace(m_pageAgent);
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink

