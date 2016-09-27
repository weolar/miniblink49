/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
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

#ifndef XMLHttpRequestEventTarget_h
#define XMLHttpRequestEventTarget_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/events/EventTarget.h"

namespace blink {

class XMLHttpRequestEventTarget : public RefCountedGarbageCollectedEventTargetWithInlineData<XMLHttpRequestEventTarget> {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(XMLHttpRequestEventTarget);
    DEFINE_WRAPPERTYPEINFO();
public:
    DEFINE_ATTRIBUTE_EVENT_LISTENER(abort);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(error);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(load);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(loadend);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(loadstart);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(progress);
    DEFINE_ATTRIBUTE_EVENT_LISTENER(timeout);
};

} // namespace blink

#endif // XMLHttpRequestEventTarget_h
