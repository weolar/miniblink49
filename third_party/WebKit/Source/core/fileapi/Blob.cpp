/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "core/fileapi/Blob.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/DOMURL.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/fileapi/BlobPropertyBag.h"
#include "platform/blob/BlobRegistry.h"
#include "platform/blob/BlobURL.h"

namespace blink {

namespace {

class BlobURLRegistry final : public URLRegistry {
public:
    virtual void registerURL(SecurityOrigin*, const KURL&, URLRegistrable*) override;
    virtual void unregisterURL(const KURL&) override;

    static URLRegistry& registry();
};

void BlobURLRegistry::registerURL(SecurityOrigin* origin, const KURL& publicURL, URLRegistrable* registrableObject)
{
    ASSERT(&registrableObject->registry() == this);
    Blob* blob = static_cast<Blob*>(registrableObject);
    BlobRegistry::registerPublicBlobURL(origin, publicURL, blob->blobDataHandle());
}

void BlobURLRegistry::unregisterURL(const KURL& publicURL)
{
    BlobRegistry::revokePublicBlobURL(publicURL);
}

URLRegistry& BlobURLRegistry::registry()
{
    // This is called on multiple threads.
    // (This code assumes it is safe to register or unregister URLs on
    // BlobURLRegistry (that is implemented by the embedder) on
    // multiple threads.)
    AtomicallyInitializedStaticReference(BlobURLRegistry, instance, new BlobURLRegistry());
    return instance;
}

} // namespace

Blob::Blob(PassRefPtr<BlobDataHandle> dataHandle)
    : m_blobDataHandle(dataHandle)
    , m_hasBeenClosed(false)
{
}

Blob::~Blob()
{
}

// static
Blob* Blob::create(const HeapVector<ArrayBufferOrArrayBufferViewOrBlobOrString>& blobParts, const BlobPropertyBag& options, ExceptionState& exceptionState)
{
    ASSERT(options.hasType());
    if (!options.type().containsOnlyASCII()) {
        exceptionState.throwDOMException(SyntaxError, "The 'type' property must consist of ASCII characters.");
        return nullptr;
    }

    ASSERT(options.hasEndings());
    bool normalizeLineEndingsToNative = options.endings() == "native";

    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->setContentType(options.type().lower());

    populateBlobData(blobData.get(), blobParts, normalizeLineEndingsToNative);

    long long blobSize = blobData->length();
    return new Blob(BlobDataHandle::create(blobData.release(), blobSize));
}

void Blob::clampSliceOffsets(long long size, long long& start, long long& end)
{
    ASSERT(size != -1);

    // Convert the negative value that is used to select from the end.
    if (start < 0)
        start = start + size;
    if (end < 0)
        end = end + size;

    // Clamp the range if it exceeds the size limit.
    if (start < 0)
        start = 0;
    if (end < 0)
        end = 0;
    if (start >= size) {
        start = 0;
        end = 0;
    } else if (end < start)
        end = start;
    else if (end > size)
        end = size;
}

Blob* Blob::slice(long long start, long long end, const String& contentType, ExceptionState& exceptionState) const
{
    if (hasBeenClosed()) {
        exceptionState.throwDOMException(InvalidStateError, "Blob has been closed.");
        return nullptr;
    }

    long long size = this->size();
    clampSliceOffsets(size, start, end);

    long long length = end - start;
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->setContentType(contentType);
    blobData->appendBlob(m_blobDataHandle, start, length);
    return Blob::create(BlobDataHandle::create(blobData.release(), length));
}

void Blob::close(ExecutionContext* executionContext, ExceptionState& exceptionState)
{
    if (hasBeenClosed()) {
        exceptionState.throwDOMException(InvalidStateError, "Blob has been closed.");
        return;
    }

    // Dereferencing a Blob that has been closed should result in
    // a network error. Revoke URLs registered against it through
    // its UUID.
    DOMURL::revokeObjectUUID(executionContext, uuid());

    // A Blob enters a 'readability state' of closed, where it will report its
    // size as zero. Blob and FileReader operations now throws on
    // being passed a Blob in that state. Downstream uses of closed Blobs
    // (e.g., XHR.send()) consider them as empty.
    OwnPtr<BlobData> blobData = BlobData::create();
    blobData->setContentType(type());
    m_blobDataHandle = BlobDataHandle::create(blobData.release(), 0);
    m_hasBeenClosed = true;
}

void Blob::appendTo(BlobData& blobData) const
{
    blobData.appendBlob(m_blobDataHandle, 0, m_blobDataHandle->size());
}

URLRegistry& Blob::registry() const
{
    return BlobURLRegistry::registry();
}

} // namespace blink
