// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScreenOrientation_h
#define ScreenOrientation_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/events/EventTarget.h"
#include "core/frame/DOMWindowProperty.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/screen_orientation/WebScreenOrientationType.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExecutionContext;
class LocalFrame;
class ScriptPromise;
class ScriptState;
class ScreenOrientationController;

class ScreenOrientation final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<ScreenOrientation>
    , public DOMWindowProperty {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(ScreenOrientation);
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ScreenOrientation);
public:
    static ScreenOrientation* create(LocalFrame*);

    ~ScreenOrientation() override;

    // EventTarget implementation.
    const WTF::AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    String type() const;
    unsigned short angle() const;

    void setType(WebScreenOrientationType);
    void setAngle(unsigned short);

    ScriptPromise lock(ScriptState*, const AtomicString& orientation);
    void unlock();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(change);

    // Helper being used by this class and LockOrientationCallback.
    static const AtomicString& orientationTypeToString(WebScreenOrientationType);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit ScreenOrientation(LocalFrame*);

    ScreenOrientationController* controller();

    WebScreenOrientationType m_type;
    unsigned short m_angle;
};

} // namespace blink

#endif // ScreenOrientation_h
