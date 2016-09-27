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

#ifndef NormalizeAlgorithm_h
#define NormalizeAlgorithm_h

#include "bindings/modules/v8/UnionTypesModules.h"
#include "modules/ModulesExport.h"
#include "public/platform/WebCrypto.h"
#include "public/platform/WebCryptoAlgorithm.h"
#include "public/platform/WebString.h"
#include "wtf/Assertions.h"
#include "wtf/Forward.h"

namespace blink {

class Dictionary;

struct AlgorithmError {
    WebCryptoErrorType errorType;
    WebString errorDetails;
};

typedef DictionaryOrString AlgorithmIdentifier;

// Converts a javascript AlgorithmIdentifier to a WebCryptoAlgorithm object.
//
// This corresponds with "normalizing" [1] the algorithm, and then validating
// the expected parameters for the algorithm/operation combination.
//
// On success returns true and sets the WebCryptoAlgorithm.
//
// On failure normalizeAlgorithm returns false and sets the AlgorithmError with
// a error type and a (non-localized) debug string.
//
// [1] http://www.w3.org/TR/WebCryptoAPI/#algorithm-normalizing-rules
MODULES_EXPORT bool normalizeAlgorithm(const AlgorithmIdentifier&, WebCryptoOperation, WebCryptoAlgorithm&, AlgorithmError*) WARN_UNUSED_RETURN;

} // namespace blink

#endif
