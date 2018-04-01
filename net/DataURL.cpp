/*
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>
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
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
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

#include "config.h"
#include "DataURL.h"

#include "third_party/WebKit/Source/wtf/text/UTF8.h"
#include "third_party/WebKit/Source/wtf/text/Base64.h"
#include "third_party/WebKit/Source/wtf/text/TextEncoding.h"
#include "third_party/WebKit/Source/wtf/CurrentTime.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/public/platform/WebURLLoader.h"
#include "third_party/WebKit/public/platform/WebURLLoaderClient.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/network/HTTPParsers.h"
#include "third_party/WebKit/Source/platform/MIMETypeRegistry.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "wtf/text/WTFStringUtil.h"

namespace net {

// void handleDataURL(blink::WebURLLoader* handle, blink::WebURLLoaderClient* client, const blink::KURL& kurl)
// {
//     if (!handle || !client)
//         return;
// 
//     String url = kurl.string();
// 
//     int index = url.find(',');
//     if (index == -1) {
//         blink::WebURLError error;
//         error.domain = blink::WebString(url);
//         error.localizedDescription = blink::WebString::fromUTF8("Cannot show DataURL\n");
//         client->didFail(handle, error);
//         return;
//     }
// 
//     String mediaType = url.substring(5, index - 5);
//     String data = url.substring(index + 1);
// 
//     bool base64 = mediaType.endsWith(";base64", WTF::TextCaseInsensitive);
//     if (base64)
//         mediaType = mediaType.left(mediaType.length() - 7);
// 
//     if (mediaType.isEmpty())
//         mediaType = "text/plain";
// 
//     String mimeType = blink::extractMIMETypeFromMediaType(WTF::AtomicString(mediaType));
//     String charset = blink::extractCharsetFromMediaType(WTF::AtomicString(mediaType));
// 
//     if (charset.isEmpty())
//         charset = "US-ASCII";
// 
//     blink::WebURLResponse response;
//     response.initialize();
//     response.setMIMEType(mimeType);
//     response.setTextEncodingName(charset);
//     response.setURL(blink::WebURL(kurl));
// 
//     int64_t totalEncodedDataLength = 0;
//     if (base64) {
//         data = blink::decodeURLEscapeSequences(data);
//         client->didReceiveResponse(handle, response);
// 
//         Vector<char> out;
//         if (WTF::base64Decode(data, out, WTF::isSpaceOrNewline) && out.size() > 0) {
//             response.setExpectedContentLength(out.size());
//             client->didReceiveData(handle, out.data(), out.size(), 0);
//         }
//         totalEncodedDataLength = out.size();
//     } else {
//         WTF::TextEncoding encoding(charset);
//         data = blink::decodeURLEscapeSequences(data, encoding);
//         client->didReceiveResponse(handle, response);
// 
//         WTF::CString encodedData = encoding.encode(data, WTF::URLEncodedEntitiesForUnencodables);
//         response.setExpectedContentLength(encodedData.length());
//         if (encodedData.length())
//             client->didReceiveData(handle, encodedData.data(), encodedData.length(), 0);
//         totalEncodedDataLength = encodedData.length();
//     }
// 
//     client->didFinishLoading(handle, currentTime(), totalEncodedDataLength);
// }

void handleDataURL(blink::WebURLLoader* handle, blink::WebURLLoaderClient* client, const blink::KURL& kurl)
{
    Vector<char> out;
    String mimeType;
    String charset;
    bool ok = parseDataURL(kurl, mimeType, charset, out);
    if (!ok) {
        blink::WebURLError error;
        error.domain = blink::WebString(kurl);
        error.localizedDescription = blink::WebString::fromUTF8("Cannot show DataURL\n");
        client->didFail(handle, error);
        return;
    }

    blink::WebURLResponse response;
    response.initialize();
    response.setMIMEType(mimeType);
    response.setTextEncodingName(charset);
    response.setURL(blink::WebURL(kurl));
    response.setExpectedContentLength(out.size());
    client->didReceiveResponse(handle, response);
    client->didReceiveData(handle, out.data(), out.size(), 0);
    client->didFinishLoading(handle, currentTime(), out.size());
}

bool parseDataURL(const blink::KURL& kurl, String& mimeType, String& charset, Vector<char>& out)
{
    out.clear();
    String url = WTF::ensureStringToUTF8String(kurl.string());

    int index = url.find(',');
    if (index == -1)
        return false;
    
    String mediaType = url.substring(5, index - 5);
    String data = url.substring(index + 1);

    bool base64 = mediaType.endsWith(";base64", WTF::TextCaseInsensitive);
    if (base64)
        mediaType = mediaType.left(mediaType.length() - 7);

    if (mediaType.isEmpty())
        mediaType = "text/plain";

    mimeType = blink::extractMIMETypeFromMediaType(WTF::AtomicString(mediaType));
    charset = blink::extractCharsetFromMediaType(WTF::AtomicString(mediaType));

    if (charset.isEmpty())
        charset = "US-ASCII";

    int64_t totalEncodedDataLength = 0;
    if (base64) {
        data = WTF::ensureStringToUTF8String(blink::decodeURLEscapeSequences(data));
        if (!(WTF::base64Decode(data, out, WTF::isSpaceOrNewline) && out.size() > 0))
            return false;
        
        totalEncodedDataLength = out.size();
    } else {
        WTF::TextEncoding encoding(charset);
        data = WTF::ensureStringToUTF8String(blink::decodeURLEscapeSequences(data, encoding));

        WTF::CString encodedData = encoding.encode(data, WTF::URLEncodedEntitiesForUnencodables);
        if (0 == encodedData.length())
            return false;
        
        out.append(encodedData.data(), encodedData.length());
        totalEncodedDataLength = encodedData.length();
    }

    return true;
}

} // namespace WebCore
