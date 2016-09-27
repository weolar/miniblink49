/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef GCObservation_h
#define GCObservation_h

#include "bindings/core/v8/ScopedPersistent.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include <v8.h>

namespace blink {

class GCObservation final : public GarbageCollectedFinalized<GCObservation>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static GCObservation* create(v8::Local<v8::Value> observedValue)
    {
        return new GCObservation(observedValue);
    }

    // Caution: It is only feasible to determine whether an object was
    // "near death"; it may have been kept alive through a weak
    // handle. After reaching near-death, having been collected is the
    // common case.
    bool wasCollected() const { return m_collected; }
    void setWasCollected();

    DEFINE_INLINE_TRACE() { }

private:
    explicit GCObservation(v8::Local<v8::Value>);

    ScopedPersistent<v8::Value> m_observed;
    bool m_collected;
};

} // namespace blink

#endif // GCObservation_h
