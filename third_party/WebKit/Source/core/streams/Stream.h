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

#ifndef Stream_h
#define Stream_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/dom/ActiveDOMObject.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExecutionContext;

class CORE_EXPORT Stream final : public GarbageCollectedFinalized<Stream>, public ScriptWrappable, public ActiveDOMObject {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(Stream);
    DEFINE_WRAPPERTYPEINFO();
public:
    static Stream* create(ExecutionContext* context, const String& mediaType)
    {
        Stream* stream = new Stream(context, mediaType);
        stream->suspendIfNeeded();
        return stream;
    }

    ~Stream() override;

    // Returns the internal URL referring to this stream.
    const KURL& url() const { return m_internalURL; }
    // Returns the media type of this stream.
    const String& type() const { return m_mediaType; }

    // Appends data to this stream.
    void addData(const char* data, size_t len);
    // Flushes contents buffered in the stream.
    void flush();
    // Mark this stream finalized so that a reader of this stream is notified
    // of EOF.
    void finalize();
    // Mark this stream finalized due to an error so that a reader of this
    // stream is notified of EOF due to the error.
    void abort();

    // Allow an external reader class to mark this object neutered so that they
    // won't load the corresponding stream again. All stream objects are
    // read-once for now.
    void neuter() { m_isNeutered = true; }
    bool isNeutered() const { return m_isNeutered; }

    // Implementation of ActiveDOMObject.
    //
    // FIXME: Implement suspend() and resume() when necessary.
    void suspend() override;
    void resume() override;
    void stop() override;

    DECLARE_VIRTUAL_TRACE();

protected:
    Stream(ExecutionContext*, const String& mediaType);

    // This is an internal URL referring to the blob data associated with this object. It serves
    // as an identifier for this blob. The internal URL is never used to source the blob's content
    // into an HTML or for FileRead'ing, public blob URLs must be used for those purposes.
    KURL m_internalURL;

    String m_mediaType;

    bool m_isNeutered;
};

} // namespace blink

#endif // Stream_h
