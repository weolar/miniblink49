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
#include "core/loader/TextResourceDecoderBuilder.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

static inline bool canReferToParentFrameEncoding(const LocalFrame* frame, const LocalFrame* parentFrame)
{
    return parentFrame && parentFrame->document()->securityOrigin()->canAccess(frame->document()->securityOrigin());
}


TextResourceDecoderBuilder::TextResourceDecoderBuilder(const AtomicString& mimeType, const AtomicString& encoding)
    : m_mimeType(mimeType)
    , m_encoding(encoding)
{
}

TextResourceDecoderBuilder::~TextResourceDecoderBuilder()
{
}


inline PassOwnPtr<TextResourceDecoder> TextResourceDecoderBuilder::createDecoderInstance(Document* document)
{
    if (LocalFrame* frame = document->frame()) {
        if (Settings* settings = frame->settings())
            return TextResourceDecoder::create(m_mimeType, settings->defaultTextEncodingName(), settings->usesEncodingDetector());
    }

    return TextResourceDecoder::create(m_mimeType, String());
}

inline void TextResourceDecoderBuilder::setupEncoding(TextResourceDecoder* decoder, Document* document)
{
    LocalFrame* frame = document->frame();
    LocalFrame* parentFrame = 0;
    if (frame && frame->tree().parent() && frame->tree().parent()->isLocalFrame())
        parentFrame = toLocalFrame(frame->tree().parent());

    if (!m_encoding.isEmpty())
        decoder->setEncoding(m_encoding.string(), TextResourceDecoder::EncodingFromHTTPHeader);

    // Set the hint encoding to the parent frame encoding only if
    // the parent and the current frames share the security origin.
    // We impose this condition because somebody can make a child frameg63
    // containing a carefully crafted html/javascript in one encoding
    // that can be mistaken for hintEncoding (or related encoding) by
    // an auto detector. When interpreted in the latter, it could be
    // an attack vector.
    // FIXME: This might be too cautious for non-7bit-encodings and
    // we may consider relaxing this later after testing.
    if (frame && canReferToParentFrameEncoding(frame, parentFrame)) {
        if (parentFrame->document()->encodingWasDetectedHeuristically())
            decoder->setHintEncoding(parentFrame->document()->encoding());

        if (m_encoding.isEmpty())
            decoder->setEncoding(parentFrame->document()->encoding(), TextResourceDecoder::EncodingFromParentFrame);
    }
}

PassOwnPtr<TextResourceDecoder> TextResourceDecoderBuilder::buildFor(Document* document)
{
    OwnPtr<TextResourceDecoder> decoder = createDecoderInstance(document);
    setupEncoding(decoder.get(), document);
    return decoder.release();
}

void TextResourceDecoderBuilder::clear()
{
    m_encoding = nullAtom;
}

}
