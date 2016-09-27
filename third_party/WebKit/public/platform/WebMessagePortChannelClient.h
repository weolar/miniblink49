/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef WebMessagePortChannelClient_h
#define WebMessagePortChannelClient_h

#include "WebCommon.h"
#include <v8.h>

namespace blink {

// Provides an interface for users of WebMessagePortChannel to be notified
// when messages are available. This also gives users of WebMessagePortChannel
// access to the V8 Context this message port lives in.
class BLINK_PLATFORM_EXPORT WebMessagePortChannelClient {
public:
    // Alerts that new messages have arrived, which are retrieved by calling
    // WebMessagePortChannel::tryGetMessage. Note that this may be called
    // on any thread.
    virtual void messageAvailable() = 0;

    // Returns the V8 isolate this message port lives in.
    // Do not rely on this API, it is only exposed so content code can convert
    // messages to base::Value, and will be removed when this conversion can be
    // integrated into blink itself.
    virtual v8::Isolate* scriptIsolate() = 0;

    // Returns a V8 context messages sent to this port can be (de)serialized in.
    // Can return null if no valid V8 context could be determined.
    // Do not rely on this API, it is only exposed so content code can convert
    // messages to base::Value, and will be removed when this conversion can be
    // integrated into blink itself.
    // FIXME: Remove this method when no longer needed (http://crbug.com/461906)
    virtual v8::Local<v8::Context> scriptContextForMessageConversion() = 0;

protected:
    ~WebMessagePortChannelClient() { }
};

} // namespace blink

#endif
