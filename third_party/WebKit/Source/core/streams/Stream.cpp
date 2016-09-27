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

#include "config.h"
#include "core/streams/Stream.h"

#include "platform/blob/BlobData.h"
#include "platform/blob/BlobRegistry.h"
#include "platform/blob/BlobURL.h"

namespace blink {

Stream::Stream(ExecutionContext* context, const String& mediaType)
    : ActiveDOMObject(context)
    , m_mediaType(mediaType)
    , m_isNeutered(false)
{
    // Create a new internal URL for a stream and register it with the provided
    // media type.
    m_internalURL = BlobURL::createInternalStreamURL();
    BlobRegistry::registerStreamURL(m_internalURL, m_mediaType);
}

void Stream::addData(const char* data, size_t len)
{
    RefPtr<RawData> buffer(RawData::create());
    buffer->mutableData()->resize(len);
    memcpy(buffer->mutableData()->data(), data, len);
    BlobRegistry::addDataToStream(m_internalURL, buffer);
}

void Stream::flush()
{
    BlobRegistry::flushStream(m_internalURL);
}

void Stream::finalize()
{
    BlobRegistry::finalizeStream(m_internalURL);
}

void Stream::abort()
{
    BlobRegistry::abortStream(m_internalURL);
}

Stream::~Stream()
{
    BlobRegistry::unregisterStreamURL(m_internalURL);
}

void Stream::suspend()
{
}

void Stream::resume()
{
}

void Stream::stop()
{
    neuter();
    abort();
}

DEFINE_TRACE(Stream)
{
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
