/*
    Copyright (C) 1999 Lars Knoll (knoll@mpi-hd.mpg.de)
    Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
    Copyright (C) 2006, 2008 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#ifndef TextResourceDecoder_h
#define TextResourceDecoder_h

#include "core/CoreExport.h"
#include "wtf/RefCounted.h"
#include "wtf/text/TextEncoding.h"

namespace blink {

class HTMLMetaCharsetParser;

class CORE_EXPORT TextResourceDecoder {
public:
    enum EncodingSource {
        DefaultEncoding,
        AutoDetectedEncoding,
        EncodingFromContentSniffing,
        EncodingFromXMLHeader,
        EncodingFromMetaTag,
        EncodingFromCSSCharset,
        EncodingFromHTTPHeader,
        EncodingFromParentFrame
    };

    static PassOwnPtr<TextResourceDecoder> create(const String& mimeType, const WTF::TextEncoding& defaultEncoding = WTF::TextEncoding(), bool usesEncodingDetector = false)
    {
        return adoptPtr(new TextResourceDecoder(mimeType, defaultEncoding, usesEncodingDetector));
    }
    ~TextResourceDecoder();

    void setEncoding(const WTF::TextEncoding&, EncodingSource);
    const WTF::TextEncoding& encoding() const { return m_encoding; }
    bool encodingWasDetectedHeuristically() const
    {
        return m_source == AutoDetectedEncoding
            || m_source == EncodingFromContentSniffing;
    }

    String decode(const char* data, size_t length);
    String flush();

    void setHintEncoding(const WTF::TextEncoding& encoding)
    {
        m_hintEncoding = encoding.name();
    }

    void useLenientXMLDecoding() { m_useLenientXMLDecoding = true; }
    bool sawError() const { return m_sawError; }
    size_t checkForBOM(const char*, size_t);

private:
    TextResourceDecoder(const String& mimeType, const WTF::TextEncoding& defaultEncoding, bool usesEncodingDetector);

    enum ContentType { PlainTextContent, HTMLContent, XMLContent, CSSContent }; // PlainText only checks for BOM.
    static ContentType determineContentType(const String& mimeType);
    static const WTF::TextEncoding& defaultEncoding(ContentType, const WTF::TextEncoding& defaultEncoding);

    bool checkForCSSCharset(const char*, size_t, bool& movedDataToBuffer);
    bool checkForXMLCharset(const char*, size_t, bool& movedDataToBuffer);
    void checkForMetaCharset(const char*, size_t);
    void detectJapaneseEncoding(const char*, size_t);
    bool shouldAutoDetect() const;

    ContentType m_contentType;
    WTF::TextEncoding m_encoding;
    OwnPtr<TextCodec> m_codec;
    EncodingSource m_source;
    const char* m_hintEncoding;
    Vector<char> m_buffer;
    bool m_checkedForBOM;
    bool m_checkedForCSSCharset;
    bool m_checkedForXMLCharset;
    bool m_checkedForMetaCharset;
    bool m_useLenientXMLDecoding; // Don't stop on XML decoding errors.
    bool m_sawError;
    bool m_usesEncodingDetector;

    OwnPtr<HTMLMetaCharsetParser> m_charsetParser;
};

}

#endif
