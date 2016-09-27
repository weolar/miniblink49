/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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

#ifndef WebCryptoKeyAlgorithmParams_h
#define WebCryptoKeyAlgorithmParams_h

#include "WebCommon.h"
#include "WebCryptoAlgorithm.h"
#include "WebVector.h"

namespace blink {

// Interface used for serializing WebCryptoKeyAlgorithmParams to a javascript
// dictionary.
class WebCryptoKeyAlgorithmDictionary {
public:
    virtual ~WebCryptoKeyAlgorithmDictionary() { }

    virtual void setString(const char*, const char*) = 0;
    virtual void setUint(const char*, unsigned) = 0;
    virtual void setAlgorithm(const char*, const WebCryptoAlgorithm&) = 0;
    virtual void setUint8Array(const char*, const WebVector<unsigned char>&) = 0;
};

enum WebCryptoKeyAlgorithmParamsType {
    WebCryptoKeyAlgorithmParamsTypeNone,
    WebCryptoKeyAlgorithmParamsTypeHmac,
    WebCryptoKeyAlgorithmParamsTypeAes,
    WebCryptoKeyAlgorithmParamsTypeRsaHashed,
    WebCryptoKeyAlgorithmParamsTypeEc,
};

class WebCryptoKeyAlgorithmParams {
public:
    virtual ~WebCryptoKeyAlgorithmParams() { }
    virtual WebCryptoKeyAlgorithmParamsType type() const
    {
        return WebCryptoKeyAlgorithmParamsTypeNone;
    }

    virtual void writeToDictionary(WebCryptoKeyAlgorithmDictionary*) const = 0;
};

class WebCryptoAesKeyAlgorithmParams : public WebCryptoKeyAlgorithmParams {
public:
    explicit WebCryptoAesKeyAlgorithmParams(unsigned short lengthBits)
        : m_lengthBits(lengthBits)
    {
    }

    unsigned short lengthBits() const
    {
        return m_lengthBits;
    }

    virtual WebCryptoKeyAlgorithmParamsType type() const
    {
        return WebCryptoKeyAlgorithmParamsTypeAes;
    }

    virtual void writeToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const
    {
        dict->setUint("length", m_lengthBits);
    }

private:
    unsigned short m_lengthBits;
};

class WebCryptoHmacKeyAlgorithmParams : public WebCryptoKeyAlgorithmParams {
public:
    WebCryptoHmacKeyAlgorithmParams(const WebCryptoAlgorithm& hash, unsigned lengthBits)
        : m_hash(hash)
        , m_lengthBits(lengthBits)
    {
    }

    const WebCryptoAlgorithm& hash() const
    {
        return m_hash;
    }

    unsigned lengthBits() const
    {
        return m_lengthBits;
    }

    virtual WebCryptoKeyAlgorithmParamsType type() const
    {
        return WebCryptoKeyAlgorithmParamsTypeHmac;
    }

    virtual void writeToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const
    {
        dict->setAlgorithm("hash", m_hash);
        dict->setUint("length", m_lengthBits);
    }

private:
    WebCryptoAlgorithm m_hash;
    unsigned m_lengthBits;
};

class WebCryptoRsaHashedKeyAlgorithmParams : public WebCryptoKeyAlgorithmParams {
public:
    WebCryptoRsaHashedKeyAlgorithmParams(unsigned modulusLengthBits, const unsigned char* publicExponent, unsigned publicExponentSize, const WebCryptoAlgorithm& hash)
        : m_modulusLengthBits(modulusLengthBits)
        , m_publicExponent(publicExponent, publicExponentSize)
        , m_hash(hash)
    {
    }

    unsigned modulusLengthBits() const
    {
        return m_modulusLengthBits;
    }

    const WebVector<unsigned char>& publicExponent() const
    {
        return m_publicExponent;
    }

    const WebCryptoAlgorithm& hash() const
    {
        return m_hash;
    }

    virtual WebCryptoKeyAlgorithmParamsType type() const
    {
        return WebCryptoKeyAlgorithmParamsTypeRsaHashed;
    }

    virtual void writeToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const
    {
        dict->setAlgorithm("hash", m_hash);
        dict->setUint("modulusLength", m_modulusLengthBits);
        dict->setUint8Array("publicExponent", m_publicExponent);
    }

private:
    unsigned m_modulusLengthBits;
    WebVector<unsigned char> m_publicExponent;
    WebCryptoAlgorithm m_hash;
};

class WebCryptoEcKeyAlgorithmParams : public WebCryptoKeyAlgorithmParams {
public:
    explicit WebCryptoEcKeyAlgorithmParams(WebCryptoNamedCurve namedCurve)
        : m_namedCurve(namedCurve)
    {
    }

    WebCryptoNamedCurve namedCurve() const
    {
        return m_namedCurve;
    }

    virtual WebCryptoKeyAlgorithmParamsType type() const
    {
        return WebCryptoKeyAlgorithmParamsTypeEc;
    }

    virtual void writeToDictionary(WebCryptoKeyAlgorithmDictionary* dict) const
    {
        switch (m_namedCurve) {
        case WebCryptoNamedCurveP256:
            dict->setString("namedCurve", "P-256");
            break;
        case WebCryptoNamedCurveP384:
            dict->setString("namedCurve", "P-384");
            break;
        case WebCryptoNamedCurveP521:
            dict->setString("namedCurve", "P-521");
            break;
        }
    }

private:
    const WebCryptoNamedCurve m_namedCurve;
};

} // namespace blink

#endif
