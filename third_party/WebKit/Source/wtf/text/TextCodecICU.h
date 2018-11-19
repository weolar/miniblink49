/*
 * Copyright (C) 2004, 2006, 2007, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov <ap@nypop.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TextCodecICU_h
#define TextCodecICU_h

#include <unicode/utypes.h>
#include "wtf/text/TextCodec.h"
#include "wtf/text/TextEncoding.h"

typedef struct UConverter UConverter;

namespace WTF {

class TextCodecInput;

class TextCodecICU final : public TextCodec {
public:
    static void registerEncodingNames(EncodingNameRegistrar);
    static void registerCodecs(TextCodecRegistrar);

    ~TextCodecICU() override;

private:
    TextCodecICU(const TextEncoding&);
    static PassOwnPtr<TextCodec> create(const TextEncoding&, const void*);

    String decode(const char*, size_t length, FlushBehavior, bool stopOnError, bool& sawError) override;
    CString encode(const UChar*, size_t length, UnencodableHandling) override;
    CString encode(const LChar*, size_t length, UnencodableHandling) override;

    template<typename CharType>
    CString encodeCommon(const CharType*, size_t length, UnencodableHandling);
    CString encodeInternal(const TextCodecInput&, UnencodableHandling);

    void createICUConverter() const;
    void releaseICUConverter() const;
#if defined(USING_SYSTEM_ICU)
    bool needsGBKFallbacks() const { return m_needsGBKFallbacks; }
    void setNeedsGBKFallbacks(bool needsFallbacks) { m_needsGBKFallbacks = needsFallbacks; }
#endif

    int decodeToBuffer(UChar* buffer, UChar* bufferLimit, const char*& source,
        const char* sourceLimit, int32_t* offsets, bool flush, UErrorCode&);

    TextEncoding m_encoding;
    mutable UConverter* m_converterICU;
#if defined(USING_SYSTEM_ICU)
    mutable bool m_needsGBKFallbacks;
#endif
    static const int kIncrementalDataChunkLength = 6;
    char m_incrementalDataChunk[kIncrementalDataChunkLength];
    int m_incrementalDataChunkLength = 0;

    bool hasValidChar();
    bool toUnicode(unsigned char c, UChar& uc);

//     int m_partialSequenceSize;
//     uint8_t m_partialSequence[kIncrementalDataChunkLength;
};

struct ICUConverterWrapper {
    WTF_MAKE_NONCOPYABLE(ICUConverterWrapper); WTF_MAKE_FAST_ALLOCATED(ICUConverterWrapper);
public:
    ICUConverterWrapper() : converter(0) { }
    ~ICUConverterWrapper();

    UConverter* converter;
};

} // namespace WTF

#endif // TextCodecICU_h
