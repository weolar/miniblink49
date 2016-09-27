// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/fetch/TextResource.h"

#include "core/html/parser/TextResourceDecoder.h"
#include "platform/SharedBuffer.h"

namespace blink {

TextResource::TextResource(const ResourceRequest& resourceRequest, Resource::Type type, const String& mimeType, const String& charset)
    : Resource(resourceRequest, type)
    , m_decoder(TextResourceDecoder::create(mimeType, charset))
{
}

TextResource::~TextResource()
{
}

void TextResource::setEncoding(const String& chs)
{
    m_decoder->setEncoding(chs, TextResourceDecoder::EncodingFromHTTPHeader);
}

String TextResource::encoding() const
{
    return m_decoder->encoding().name();
}

String TextResource::decodedText() const
{
    ASSERT(m_data);

    String text = m_decoder->decode(m_data->data(), encodedSize());
    text.append(m_decoder->flush());
    return text;
}

} // namespace blink
