// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/screen_orientation/ScreenOrientation.h"

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/LocalFrame.h"
#include "modules/EventTargetModules.h"
#include "modules/screen_orientation/LockOrientationCallback.h"
#include "modules/screen_orientation/ScreenOrientationController.h"
#include "public/platform/modules/screen_orientation/WebScreenOrientationType.h"

// This code assumes that WebScreenOrientationType values are included in WebScreenOrientationLockType.
#define STATIC_ASSERT_MATCHING_ENUM(enum1, enum2) \
    static_assert(static_cast<unsigned>(blink::enum1) == static_cast<unsigned>(blink::enum2), "mismatching enum values")
STATIC_ASSERT_MATCHING_ENUM(WebScreenOrientationPortraitPrimary, WebScreenOrientationLockPortraitPrimary);
STATIC_ASSERT_MATCHING_ENUM(WebScreenOrientationPortraitSecondary, WebScreenOrientationLockPortraitSecondary);
STATIC_ASSERT_MATCHING_ENUM(WebScreenOrientationLandscapePrimary, WebScreenOrientationLockLandscapePrimary);
STATIC_ASSERT_MATCHING_ENUM(WebScreenOrientationLandscapeSecondary, WebScreenOrientationLockLandscapeSecondary);

namespace blink {

struct ScreenOrientationInfo {
    const AtomicString& name;
    unsigned orientation;
};

static ScreenOrientationInfo* orientationsMap(unsigned& length)
{
    DEFINE_STATIC_LOCAL(const AtomicString, portraitPrimary, ("portrait-primary", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, portraitSecondary, ("portrait-secondary", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, landscapePrimary, ("landscape-primary", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, landscapeSecondary, ("landscape-secondary", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, any, ("any", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, portrait, ("portrait", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, landscape, ("landscape", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, natural, ("natural", AtomicString::ConstructFromLiteral));

    static ScreenOrientationInfo orientationMap[] = {
        { portraitPrimary, WebScreenOrientationLockPortraitPrimary },
        { portraitSecondary, WebScreenOrientationLockPortraitSecondary },
        { landscapePrimary, WebScreenOrientationLockLandscapePrimary },
        { landscapeSecondary, WebScreenOrientationLockLandscapeSecondary },
        { any, WebScreenOrientationLockAny },
        { portrait, WebScreenOrientationLockPortrait },
        { landscape, WebScreenOrientationLockLandscape },
        { natural, WebScreenOrientationLockNatural }
    };
    length = WTF_ARRAY_LENGTH(orientationMap);

    return orientationMap;
}

const AtomicString& ScreenOrientation::orientationTypeToString(WebScreenOrientationType orientation)
{
    unsigned length = 0;
    ScreenOrientationInfo* orientationMap = orientationsMap(length);
    for (unsigned i = 0; i < length; ++i) {
        if (static_cast<unsigned>(orientation) == orientationMap[i].orientation)
            return orientationMap[i].name;
    }

    ASSERT_NOT_REACHED();
    return nullAtom;
}

static WebScreenOrientationLockType stringToOrientationLock(const AtomicString& orientationLockString)
{
    unsigned length = 0;
    ScreenOrientationInfo* orientationMap = orientationsMap(length);
    for (unsigned i = 0; i < length; ++i) {
        if (orientationMap[i].name == orientationLockString)
            return static_cast<WebScreenOrientationLockType>(orientationMap[i].orientation);
    }

    ASSERT_NOT_REACHED();
    return WebScreenOrientationLockDefault;
}

// static
ScreenOrientation* ScreenOrientation::create(LocalFrame* frame)
{
    ASSERT(frame);

    // Check if the ScreenOrientationController is supported for the
    // frame. It will not be for all LocalFrames, or the frame may
    // have been detached.
    if (!ScreenOrientationController::from(*frame))
        return nullptr;

    ScreenOrientation* orientation = new ScreenOrientation(frame);
    ASSERT(orientation->controller());
    // FIXME: ideally, we would like to provide the ScreenOrientationController
    // the case where it is not defined but for the moment, it is eagerly
    // created when the LocalFrame is created so we shouldn't be in that
    // situation.
    // In order to create the ScreenOrientationController lazily, we would need
    // to be able to access WebFrameClient from modules/.

    orientation->controller()->setOrientation(orientation);
    return orientation;
}

ScreenOrientation::ScreenOrientation(LocalFrame* frame)
    : DOMWindowProperty(frame)
    , m_type(WebScreenOrientationUndefined)
    , m_angle(0)
{
}

ScreenOrientation::~ScreenOrientation()
{
}

const WTF::AtomicString& ScreenOrientation::interfaceName() const
{
    return EventTargetNames::ScreenOrientation;
}

ExecutionContext* ScreenOrientation::executionContext() const
{
    if (!m_frame)
        return 0;
    return m_frame->document();
}

String ScreenOrientation::type() const
{
    return orientationTypeToString(m_type);
}

unsigned short ScreenOrientation::angle() const
{
    return m_angle;
}

void ScreenOrientation::setType(WebScreenOrientationType type)
{
    m_type = type;
}

void ScreenOrientation::setAngle(unsigned short angle)
{
    m_angle = angle;
}

ScriptPromise ScreenOrientation::lock(ScriptState* state, const AtomicString& lockString)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(state);
    ScriptPromise promise = resolver->promise();

    Document* document = m_frame ? m_frame->document() : 0;

    if (!document || !controller()) {
        DOMException* exception = DOMException::create(InvalidStateError, "The object is no longer associated to a document.");
        resolver->reject(exception);
        return promise;
    }

    if (document->isSandboxed(SandboxOrientationLock)) {
        DOMException* exception = DOMException::create(SecurityError, "The document is sandboxed and lacks the 'allow-orientation-lock' flag.");
        resolver->reject(exception);
        return promise;
    }

    controller()->lock(stringToOrientationLock(lockString), new LockOrientationCallback(resolver));
    return promise;
}

void ScreenOrientation::unlock()
{
    if (!controller())
        return;

    controller()->unlock();
}

ScreenOrientationController* ScreenOrientation::controller()
{
    if (!m_frame)
        return 0;

    return ScreenOrientationController::from(*m_frame);
}

DEFINE_TRACE(ScreenOrientation)
{
    RefCountedGarbageCollectedEventTargetWithInlineData<ScreenOrientation>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

} // namespace blink
