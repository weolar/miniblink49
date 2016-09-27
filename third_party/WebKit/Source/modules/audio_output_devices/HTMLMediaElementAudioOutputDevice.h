// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLMediaElementAudioOutputDevice_h
#define HTMLMediaElementAudioOutputDevice_h

#include "bindings/core/v8/ScriptPromise.h"
#include "core/dom/ContextLifecycleObserver.h"
#include "core/dom/ExceptionCode.h"
#include "core/html/HTMLMediaElement.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class HTMLMediaElement;
class ScriptState;

class HTMLMediaElementAudioOutputDevice final : public NoBaseWillBeGarbageCollectedFinalized<HTMLMediaElementAudioOutputDevice>, public WillBeHeapSupplement<HTMLMediaElement> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(HTMLMediaElementAudioOutputDevice);
public:
    DECLARE_VIRTUAL_TRACE();
    static String sinkId(HTMLMediaElement&);
    static ScriptPromise setSinkId(ScriptState*, HTMLMediaElement&, const String& sinkId);
    static HTMLMediaElementAudioOutputDevice& from(HTMLMediaElement&);
    void setSinkId(const String&);

private:
    HTMLMediaElementAudioOutputDevice();
    static const char* supplementName();

    String m_sinkId;
};

} // namespace blink

#endif
