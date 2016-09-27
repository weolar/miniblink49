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

#ifndef WebCrypto_h
#define WebCrypto_h

#include "WebCommon.h"
#include "WebCryptoAlgorithm.h"
#include "WebCryptoKey.h"
#include "WebPrivatePtr.h"
#include "WebString.h"
#include "WebVector.h"

#if INSIDE_BLINK
#include "platform/heap/Handle.h"
#endif

namespace blink {

class CryptoResult;
class WebString;

enum WebCryptoErrorType {
    WebCryptoErrorTypeType,
    WebCryptoErrorTypeNotSupported,
    WebCryptoErrorTypeSyntax,
    WebCryptoErrorTypeInvalidAccess,
    WebCryptoErrorTypeData,
    WebCryptoErrorTypeOperation,
};

class WebCryptoResult {
public:
    WebCryptoResult(const WebCryptoResult& o)
    {
        assign(o);
    }

    ~WebCryptoResult()
    {
        reset();
    }

    WebCryptoResult& operator=(const WebCryptoResult& o)
    {
        assign(o);
        return *this;
    }

    // Note that WebString is NOT safe to pass across threads.
    //
    // Error details are surfaced in an exception, and MUST NEVER reveal any
    // secret information such as bytes of the key or plain text. An
    // appropriate error would be something like:
    //   "iv must be 16 bytes long".
    BLINK_PLATFORM_EXPORT void completeWithError(WebCryptoErrorType, const WebString&);

    // Makes a copy of the input data given as a pointer and byte length.
    BLINK_PLATFORM_EXPORT void completeWithBuffer(const void*, unsigned);
    BLINK_PLATFORM_EXPORT void completeWithJson(const char* utf8Data, unsigned length);
    BLINK_PLATFORM_EXPORT void completeWithBoolean(bool);
    BLINK_PLATFORM_EXPORT void completeWithKey(const WebCryptoKey&);
    BLINK_PLATFORM_EXPORT void completeWithKeyPair(const WebCryptoKey& publicKey, const WebCryptoKey& privateKey);

    // Returns true if the underlying operation was cancelled.
    // This method can be called from any thread.
    BLINK_PLATFORM_EXPORT bool cancelled() const;

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT explicit WebCryptoResult(const PassRefPtrWillBeRawPtr<CryptoResult>&);
#endif

private:
    BLINK_PLATFORM_EXPORT void reset();
    BLINK_PLATFORM_EXPORT void assign(const WebCryptoResult&);

    WebPrivatePtr<CryptoResult> m_impl;
};

class WebCryptoDigestor {
public:
    virtual ~WebCryptoDigestor() { }

    // consume() will return |true| on the successful addition of data to the
    // partially generated digest. It will return |false| when that fails. After
    // a return of |false|, consume() should not be called again (nor should
    // finish() be called).
    virtual bool consume(const unsigned char* data, unsigned dataSize) { return false; }

    // finish() will return |true| if the digest has been successfully computed
    // and put into the result buffer, otherwise it will return |false|. In
    // either case, neither finish() nor consume() should be called again after
    // a call to finish(). resultData is valid until the WebCrytpoDigestor
    // object is destroyed.
    virtual bool finish(unsigned char*& resultData, unsigned& resultDataSize) { return false; }

protected:
    WebCryptoDigestor() { }
};

class WebCrypto {
public:
    // WebCrypto is the interface for starting one-shot cryptographic
    // operations.
    //
    // -----------------------
    // Completing the request
    // -----------------------
    //
    // Implementations signal completion by calling one of the methods on
    // "result". Only a single result/error should be set for the request.
    // Different operations expect different result types based on the
    // algorithm parameters; see the Web Crypto standard for details.
    //
    // The result can be set either synchronously while handling the request,
    // or asynchronously after the method has returned. When completing
    // asynchronously make a copy of the WebCryptoResult and call it from the
    // same thread that started the request.
    //
    // If the request was cancelled it is not necessary for implementations to
    // set the result.
    //
    // -----------------------
    // Threading
    // -----------------------
    //
    // The WebCrypto interface will be called from blink threads (main or
    // web worker). All communication back to Blink must be on this same thread.
    //
    // Notably:
    //
    //   * The WebCryptoResult can be copied between threads, however all
    //     methods other than the destructor must be called from the origin
    //     Blink thread.
    //
    //   * WebCryptoKey and WebCryptoAlgorithm ARE threadsafe. They can be
    //     safely copied between threads and accessed. Copying is cheap because
    //     they are internally reference counted.
    //
    // -----------------------
    // Inputs
    // -----------------------
    //
    //   * Data buffers are passed as (basePointer, byteLength) pairs.
    //     These buffers are only valid during the call itself. Asynchronous
    //     implementations wishing to access it after the function returns
    //     should make a copy.
    //
    //   * All WebCryptoKeys are guaranteeed to be !isNull().
    //
    //   * All WebCryptoAlgorithms are guaranteed to be !isNull()
    //
    //   * Look to the Web Crypto spec for an explanation of the parameter. The
    //     method names here have a 1:1 correspondence with those of
    //     crypto.subtle, with the exception of "verify" which is here called
    //     "verifySignature".
    //
    // -----------------------
    // Guarantees on input validity
    // -----------------------
    //
    // Implementations MUST carefully sanitize algorithm inputs before using
    // them, as they come directly from the user. Few checks have been done on
    // algorithm parameters prior to passing to the embedder.
    //
    // Only the following checks can be assumed as having already passed:
    //
    //  * The key is extractable when calling into exportKey/wrapKey.
    //  * The key usages permit the operation being requested.
    //  * The key's algorithm matches that of the requested operation.
    //
    virtual void encrypt(const WebCryptoAlgorithm&, const WebCryptoKey&, const unsigned char* data, unsigned dataSize, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void decrypt(const WebCryptoAlgorithm&, const WebCryptoKey&, const unsigned char* data, unsigned dataSize, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void sign(const WebCryptoAlgorithm&, const WebCryptoKey&, const unsigned char* data, unsigned dataSize, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void verifySignature(const WebCryptoAlgorithm&, const WebCryptoKey&, const unsigned char* signature, unsigned signatureSize, const unsigned char* data, unsigned dataSize, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void digest(const WebCryptoAlgorithm&, const unsigned char* data, unsigned dataSize, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void generateKey(const WebCryptoAlgorithm&, bool extractable, WebCryptoKeyUsageMask, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void importKey(WebCryptoKeyFormat, const unsigned char* keyData, unsigned keyDataSize, const WebCryptoAlgorithm&, bool extractable, WebCryptoKeyUsageMask, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void exportKey(WebCryptoKeyFormat, const WebCryptoKey&, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void wrapKey(WebCryptoKeyFormat, const WebCryptoKey& key, const WebCryptoKey& wrappingKey, const WebCryptoAlgorithm&, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void unwrapKey(WebCryptoKeyFormat, const unsigned char* wrappedKey, unsigned wrappedKeySize, const WebCryptoKey&, const WebCryptoAlgorithm& unwrapAlgorithm, const WebCryptoAlgorithm& unwrappedKeyAlgorithm, bool extractable, WebCryptoKeyUsageMask, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void deriveBits(const WebCryptoAlgorithm&, const WebCryptoKey&, unsigned length, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }
    virtual void deriveKey(const WebCryptoAlgorithm& algorithm, const WebCryptoKey& baseKey, const WebCryptoAlgorithm& importAlgorithm, const WebCryptoAlgorithm& keyLengthAlgorithm, bool extractable, WebCryptoKeyUsageMask, WebCryptoResult result) { result.completeWithError(WebCryptoErrorTypeNotSupported, ""); }

    // This is the exception to the "Completing the request" guarantees
    // outlined above. This is useful for Blink internal crypto and is not part
    // of the WebCrypto standard. createDigestor must provide the result via
    // the WebCryptoDigestor object synchronously. createDigestor may return 0
    // if it fails to create a WebCryptoDigestor. If it succeeds, the
    // WebCryptoDigestor returned by createDigestor must be freed by the
    // caller.
    virtual WebCryptoDigestor* createDigestor(WebCryptoAlgorithmId algorithmId) { return nullptr; }

    // -----------------------
    // Structured clone
    // -----------------------
    //
    // deserializeKeyForClone() and serializeKeyForClone() are used for
    // implementing structured cloning of WebCryptoKey.
    //
    // Blink is responsible for saving and restoring all of the attributes of
    // WebCryptoKey EXCEPT for the actual key data:
    //
    // In other words, Blink takes care of serializing:
    //   * Key usages
    //   * Key extractability
    //   * Key algorithm
    //   * Key type (public, private, secret)
    //
    // The embedder is responsible for saving the key data itself.
    //
    // Visibility of the serialized key data:
    //
    // The serialized key data will NOT be visible to web pages. So if the
    // serialized format were to include key bytes as plain text, this wouldn't
    // make it available to web pages.
    //
    // Longevity of the key data:
    //
    // The serialized key data is intended to be long lived (years) and MUST
    // be using a stable format. For instance a key might be persisted to
    // IndexedDB and should be able to be deserialized correctly in the
    // future.
    //
    // Error handling and asynchronous completion:
    //
    // Serialization/deserialization must complete synchronously, and will
    // block the JavaScript thread.
    //
    // The only reasons to fail serialization/deserialization are:
    //   * Key serialization not yet implemented
    //   * The bytes to deserialize were corrupted

    // Creates a new key given key data which was written using
    // serializeKeyForClone(). Returns true on success.
    virtual bool deserializeKeyForClone(const WebCryptoKeyAlgorithm&, WebCryptoKeyType, bool extractable, WebCryptoKeyUsageMask, const unsigned char* keyData, unsigned keyDataSize, WebCryptoKey&) { return false; }

    // Writes the key data into the given WebVector.
    // Returns true on success.
    virtual bool serializeKeyForClone(const WebCryptoKey&, WebVector<unsigned char>&) { return false; }

protected:
    virtual ~WebCrypto() { }
};

} // namespace blink

#endif // WebCrypto_h
