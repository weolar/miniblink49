// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/Crypto.h"

#include "public/platform/Platform.h"
#include "public/platform/WebCrypto.h"
#include "public/platform/WebCryptoAlgorithm.h"

namespace blink {

static WebCryptoAlgorithmId toWebCryptoAlgorithmId(HashAlgorithm algorithm)
{
    switch (algorithm) {
    case HashAlgorithmSha1:
        return WebCryptoAlgorithmIdSha1;
    case HashAlgorithmSha256:
        return WebCryptoAlgorithmIdSha256;
    case HashAlgorithmSha384:
        return WebCryptoAlgorithmIdSha384;
    case HashAlgorithmSha512:
        return WebCryptoAlgorithmIdSha512;
    };

    ASSERT_NOT_REACHED();
    return WebCryptoAlgorithmIdSha256;
}

bool computeDigest(HashAlgorithm algorithm, const char* digestable, size_t length, DigestValue& digestResult)
{
    WebCryptoAlgorithmId algorithmId = toWebCryptoAlgorithmId(algorithm);
    WebCrypto* crypto = Platform::current()->crypto();
    unsigned char* result;
    unsigned resultSize;

    ASSERT(crypto);

    OwnPtr<WebCryptoDigestor> digestor = adoptPtr(crypto->createDigestor(algorithmId));
    if (!digestor.get() || !digestor->consume(reinterpret_cast<const unsigned char*>(digestable), length) || !digestor->finish(result, resultSize))
        return false;

    digestResult.append(static_cast<uint8_t*>(result), resultSize);
    return true;
}

PassOwnPtr<WebCryptoDigestor> createDigestor(HashAlgorithm algorithm)
{
    return adoptPtr(Platform::current()->crypto()->createDigestor(toWebCryptoAlgorithmId(algorithm)));
}

void finishDigestor(WebCryptoDigestor* digestor, DigestValue& digestResult)
{
    unsigned char* result = 0;
    unsigned resultSize = 0;

    if (!digestor->finish(result, resultSize))
        return;

    ASSERT(result);

    digestResult.append(static_cast<uint8_t*>(result), resultSize);
}

} // namespace blink
