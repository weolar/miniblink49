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

#ifndef KeyAlgorithm_h
#define KeyAlgorithm_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebCryptoKeyAlgorithm.h"
#include "wtf/Forward.h"

namespace blink {

class KeyAlgorithm : public GarbageCollectedFinalized<KeyAlgorithm>, public ScriptWrappable {
public:
    ~KeyAlgorithm() override;

    static KeyAlgorithm* create(const WebCryptoKeyAlgorithm&);
    static KeyAlgorithm* createHash(const WebCryptoAlgorithm&);

    String name();

    // Needed by SpecialWrapFor and for casting.
    bool isAesKeyAlgorithm() const;
    bool isHmacKeyAlgorithm() const;
    bool isRsaHashedKeyAlgorithm() const;

    DECLARE_VIRTUAL_TRACE();

protected:
    explicit KeyAlgorithm(const WebCryptoKeyAlgorithm&);

    WebCryptoKeyAlgorithm m_algorithm;
};

#define DEFINE_KEY_ALGORITHM_TYPE_CASTS(thisType) \
    DEFINE_TYPE_CASTS(thisType, KeyAlgorithm, value, value->is##thisType(), value.is##thisType())

} // namespace blink

#endif
