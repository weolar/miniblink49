// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/crypto/CryptoHistograms.h"

#include "core/frame/UseCounter.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCryptoAlgorithm.h"
#include "public/platform/WebCryptoAlgorithmParams.h"
#include "public/platform/WebCryptoKeyAlgorithm.h"

namespace blink {

static UseCounter::Feature algorithmIdToFeature(WebCryptoAlgorithmId id)
{
    switch (id) {
    case WebCryptoAlgorithmIdAesCbc:
        return UseCounter::CryptoAlgorithmAesCbc;
    case WebCryptoAlgorithmIdHmac:
        return UseCounter::CryptoAlgorithmHmac;
    case WebCryptoAlgorithmIdRsaSsaPkcs1v1_5:
        return UseCounter::CryptoAlgorithmRsaSsaPkcs1v1_5;
    case WebCryptoAlgorithmIdSha1:
        return UseCounter::CryptoAlgorithmSha1;
    case WebCryptoAlgorithmIdSha256:
        return UseCounter::CryptoAlgorithmSha256;
    case WebCryptoAlgorithmIdSha384:
        return UseCounter::CryptoAlgorithmSha384;
    case WebCryptoAlgorithmIdSha512:
        return UseCounter::CryptoAlgorithmSha512;
    case WebCryptoAlgorithmIdAesGcm:
        return UseCounter::CryptoAlgorithmAesGcm;
    case WebCryptoAlgorithmIdRsaOaep:
        return UseCounter::CryptoAlgorithmRsaOaep;
    case WebCryptoAlgorithmIdAesCtr:
        return UseCounter::CryptoAlgorithmAesCtr;
    case WebCryptoAlgorithmIdAesKw:
        return UseCounter::CryptoAlgorithmAesKw;
    case WebCryptoAlgorithmIdRsaPss:
        return UseCounter::CryptoAlgorithmRsaPss;
    case WebCryptoAlgorithmIdEcdsa:
        return UseCounter::CryptoAlgorithmEcdsa;
    case WebCryptoAlgorithmIdEcdh:
        return UseCounter::CryptoAlgorithmEcdh;
    case WebCryptoAlgorithmIdHkdf:
        return UseCounter::CryptoAlgorithmHkdf;
    case WebCryptoAlgorithmIdPbkdf2:
        return UseCounter::CryptoAlgorithmPbkdf2;
    }

    ASSERT_NOT_REACHED();
    return static_cast<UseCounter::Feature>(0);
}

static void histogramAlgorithmId(ExecutionContext* context, WebCryptoAlgorithmId algorithmId)
{
    UseCounter::Feature feature = algorithmIdToFeature(algorithmId);
    if (feature)
        UseCounter::count(context, feature);
}

void histogramAlgorithm(ExecutionContext* context, const WebCryptoAlgorithm& algorithm)
{
    histogramAlgorithmId(context, algorithm.id());

    // Histogram any interesting parameters for the algorithm. For instance
    // the inner hash for algorithms which include one (HMAC, RSA-PSS, etc)
    switch (algorithm.paramsType()) {
    case WebCryptoAlgorithmParamsTypeHmacImportParams:
        histogramAlgorithm(context, algorithm.hmacImportParams()->hash());
        break;
    case WebCryptoAlgorithmParamsTypeHmacKeyGenParams:
        histogramAlgorithm(context, algorithm.hmacKeyGenParams()->hash());
        break;
    case WebCryptoAlgorithmParamsTypeRsaHashedKeyGenParams:
        histogramAlgorithm(context, algorithm.rsaHashedKeyGenParams()->hash());
        break;
    case WebCryptoAlgorithmParamsTypeRsaHashedImportParams:
        histogramAlgorithm(context, algorithm.rsaHashedImportParams()->hash());
        break;
    case WebCryptoAlgorithmParamsTypeEcdsaParams:
        histogramAlgorithm(context, algorithm.ecdsaParams()->hash());
        break;
    case WebCryptoAlgorithmParamsTypeHkdfParams:
        histogramAlgorithm(context, algorithm.hkdfParams()->hash());
        break;
    case WebCryptoAlgorithmParamsTypePbkdf2Params:
        histogramAlgorithm(context, algorithm.pbkdf2Params()->hash());
        break;
    case WebCryptoAlgorithmParamsTypeEcdhKeyDeriveParams:
    case WebCryptoAlgorithmParamsTypeNone:
    case WebCryptoAlgorithmParamsTypeAesCbcParams:
    case WebCryptoAlgorithmParamsTypeAesGcmParams:
    case WebCryptoAlgorithmParamsTypeAesKeyGenParams:
    case WebCryptoAlgorithmParamsTypeRsaOaepParams:
    case WebCryptoAlgorithmParamsTypeAesCtrParams:
    case WebCryptoAlgorithmParamsTypeRsaPssParams:
    case WebCryptoAlgorithmParamsTypeEcKeyGenParams:
    case WebCryptoAlgorithmParamsTypeEcKeyImportParams:
    case WebCryptoAlgorithmParamsTypeAesDerivedKeyParams:
        break;
    }
}

void histogramKey(ExecutionContext* context, const WebCryptoKey& key)
{
    const WebCryptoKeyAlgorithm& algorithm = key.algorithm();

    histogramAlgorithmId(context, algorithm.id());

    // Histogram any interesting parameters that are attached to the key. For
    // instance the inner hash being used for HMAC.
    switch (algorithm.paramsType()) {
    case WebCryptoKeyAlgorithmParamsTypeHmac:
        histogramAlgorithm(context, algorithm.hmacParams()->hash());
        break;
    case WebCryptoKeyAlgorithmParamsTypeRsaHashed:
        histogramAlgorithm(context, algorithm.rsaHashedParams()->hash());
        break;
    case WebCryptoKeyAlgorithmParamsTypeNone:
    case WebCryptoKeyAlgorithmParamsTypeAes:
    case WebCryptoKeyAlgorithmParamsTypeEc:
        break;
    }
}

void histogramAlgorithmAndKey(ExecutionContext* context, const WebCryptoAlgorithm& algorithm, const WebCryptoKey& key)
{
    // Note that the algorithm ID for |algorithm| and |key| will usually be the
    // same. This is OK because UseCounter only increments things once per the
    // context.
    histogramAlgorithm(context, algorithm);
    histogramKey(context, key);
}

} // namespace blink
