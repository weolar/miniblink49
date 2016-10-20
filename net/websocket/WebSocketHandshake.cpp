/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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

#include "net/websocket/WebSocketHandshake.h"

#include "third_party/WebKit/Source/core/loader/CookieJar.h"
#include "third_party/WebKit/Source/core/dom/Document.h"
#include "third_party/WebKit/Source/platform/Logging.h"
#include "third_party/WebKit/Source/platform/network/WebSocketHandshakeRequest.h"
#include <wtf/CryptographicallyRandomNumber.h>
// #include <wtf/MD5.h>
// #include <wtf/SHA1.h>
#include "third_party/skia/src/utils/SkSHA1.h"
#include <wtf/StdLibExtras.h>
#include <wtf/StringExtras.h>
#include <wtf/Vector.h>
#include <wtf/text/Base64.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>
#include <wtf/text/WTFString.h>
// #include <wtf/unicode/CharacterNames.h>

using namespace blink;

namespace net {

static String resourceName(const blink::KURL& url)
{
    StringBuilder name;
    name.append(url.path());
    if (name.isEmpty())
        name.append('/');
    if (!url.query().isNull()) {
        name.append('?');
        name.append(url.query());
    }
    String result = name.toString();
    ASSERT(!result.isEmpty());
    ASSERT(!result.contains(' '));
    return result;
}

static String hostName(const blink::KURL& url, bool secure)
{
    ASSERT(url.protocolIs("wss") == secure);
    StringBuilder builder;
    builder.append(url.host().lower());
    if (url.port() && ((!secure && url.port() != 80) || (secure && url.port() != 443))) {
        builder.append(':');
        builder.appendNumber(url.port());
    }
    return builder.toString();
}

static const size_t maxInputSampleSize = 128;
static const UChar horizontalEllipsis = 0x2026;

static String trimInputSample(const char* p, size_t len)
{
    String s = String(p, std::min<size_t>(len, maxInputSampleSize));
    if (len > maxInputSampleSize)
        s.append(horizontalEllipsis);
    return s;
}

static String generateSecWebSocketKey()
{
    static const size_t nonceSize = 16;
    unsigned char key[nonceSize];
    cryptographicallyRandomValues(key, nonceSize);
    return base64Encode((const char*)key, nonceSize, Base64DoNotInsertLFs);
}

String WebSocketHandshake::getExpectedWebSocketAccept(const String& secWebSocketKey)
{
    static const char* const webSocketKeyGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    CString keyData = secWebSocketKey.ascii();
    SkSHA1 sha1;

    sha1.write(reinterpret_cast<const uint8_t*>(keyData.data()), keyData.length());
    sha1.write(reinterpret_cast<const uint8_t*>(webSocketKeyGUID), strlen(webSocketKeyGUID));
    SkSHA1::Digest hash;
    sha1.finish(hash);
    return base64Encode((const char*)hash.data, 20);
}

WebSocketHandshake::WebSocketHandshake(const KURL& url, const String& protocol, ExecutionContext* context)
    : m_url(url)
    , m_clientProtocol(protocol)
    , m_secure(m_url.protocolIs("wss"))
    , m_context(context)
    , m_mode(Incomplete)
{
    m_secWebSocketKey = generateSecWebSocketKey();
    m_expectedAccept = getExpectedWebSocketAccept(m_secWebSocketKey);
}

WebSocketHandshake::~WebSocketHandshake()
{
}

const blink::KURL& WebSocketHandshake::url() const
{
    return m_url;
}

void WebSocketHandshake::setURL(const blink::KURL& url)
{
    m_url = url;
}

const String WebSocketHandshake::host() const
{
    return m_url.host().lower();
}

const String& WebSocketHandshake::clientProtocol() const
{
    return m_clientProtocol;
}

void WebSocketHandshake::setClientProtocol(const String& protocol)
{
    m_clientProtocol = protocol;
}

bool WebSocketHandshake::secure() const
{
    return m_secure;
}

String WebSocketHandshake::clientOrigin() const
{
    return m_context->securityOrigin()->toString();
}

String WebSocketHandshake::clientLocation() const
{
    StringBuilder builder;
    builder.append(m_secure ? "wss" : "ws");
    builder.appendLiteral("://");
    builder.append(hostName(m_url, m_secure));
    builder.append(resourceName(m_url));
    return builder.toString();
}

CString WebSocketHandshake::clientHandshakeMessage() const
{
    // Keep the following consistent with clientHandshakeRequest().
    StringBuilder builder;

    builder.appendLiteral("GET ");
    builder.append(resourceName(m_url));
    builder.appendLiteral(" HTTP/1.1\r\n");

    Vector<String> fields;
    fields.append("Upgrade: websocket");
    fields.append("Connection: Upgrade");
    fields.append("Host: " + hostName(m_url, m_secure));
    fields.append("Origin: " + clientOrigin());
    if (!m_clientProtocol.isEmpty())
        fields.append("Sec-WebSocket-Protocol: " + m_clientProtocol);

    blink::KURL url = httpURLForAuthenticationAndCookies();
    if (m_context->isDocument()) {
        Document& document = toDocument(*m_context);
        String cookie = cookieRequestHeaderFieldValue(&document, url);
        if (!cookie.isEmpty())
            fields.append("Cookie: " + cookie);
        // Set "Cookie2: <cookie>" if cookies 2 exists for url?
    }

    // Add no-cache headers to avoid compatibility issue.
    // There are some proxies that rewrite "Connection: upgrade"
    // to "Connection: close" in the response if a request doesn't contain
    // these headers.
    fields.append("Pragma: no-cache");
    fields.append("Cache-Control: no-cache");

    fields.append("Sec-WebSocket-Key: " + m_secWebSocketKey);
    fields.append("Sec-WebSocket-Version: 13");
    const String extensionValue = m_extensionDispatcher.createHeaderValue();
    if (extensionValue.length())
        fields.append("Sec-WebSocket-Extensions: " + extensionValue);

    // Add a User-Agent header.
    fields.append("User-Agent: " + m_context->userAgent(m_context->url()));

    // Fields in the handshake are sent by the client in a random order; the
    // order is not meaningful.  Thus, it's ok to send the order we constructed
    // the fields.

    for (auto& field : fields) {
        builder.append(field);
        builder.appendLiteral("\r\n");
    }

    builder.appendLiteral("\r\n");

    return builder.toString().utf8();
}

PassRefPtr<WebSocketHandshakeRequest> WebSocketHandshake::clientHandshakeRequest() const
{
    PassRefPtr<WebSocketHandshakeRequest> request = WebSocketHandshakeRequest::create(m_url);
    request->addHeaderField("Connection", "Upgrade");
    request->addHeaderField("Host", hostName(m_url, m_secure).utf8().data());
    request->addHeaderField("Origin", clientOrigin().utf8().data());
    if (!m_clientProtocol.isEmpty())
        request->addHeaderField("Sec-WebSocket-Protocol", m_clientProtocol.utf8().data());
    blink::KURL url = httpURLForAuthenticationAndCookies();
    if (m_context->isDocument()) {
        Document* document = toDocument(m_context);
        String cookie = cookieRequestHeaderFieldValue(document, url);
        if (!cookie.isEmpty())
            request->addHeaderField("Cookie", cookie.utf8().data());
        // Set "Cookie2: <cookie>" if cookies 2 exists for url?
    }

    request->addHeaderField("Pragma", "no-cache");
    request->addHeaderField("CacheControl", "no-cache");

    request->addHeaderField("Sec-WebSocket-Key", m_secWebSocketKey.utf8().data());
    request->addHeaderField("Sec-WebSocket-Version", "13");
    const String extensionValue = m_extensionDispatcher.createHeaderValue();
    if (extensionValue.length())
        request->addHeaderField("Sec-WebSocket-Extensions", extensionValue.utf8().data());

    // Add a User-Agent header.
    request->addHeaderField("User-Agent", m_context->userAgent(m_context->url()).utf8().data());

    return request;
}

void WebSocketHandshake::reset()
{
    m_mode = Incomplete;
    m_extensionDispatcher.reset();
}

void WebSocketHandshake::clearScriptExecutionContext()
{
    m_context = nullptr;
}

int WebSocketHandshake::readServerHandshake(const char* header, size_t len)
{
    m_mode = Incomplete;
    int statusCode;
    String statusText;
    int lineLength = readStatusLine(header, len, statusCode, statusText);
    if (lineLength == -1)
        return -1;
    if (statusCode == -1) {
        m_mode = Failed; // m_failureReason is set inside readStatusLine().
        return len;
    }
    WTF_LOG(Network, "WebSocketHandshake %p readServerHandshake() Status code is %d", this, statusCode);

    m_serverHandshakeResponse = WebSocketHandshakeResponse();
    m_serverHandshakeResponse.setStatusCode(statusCode);
    m_serverHandshakeResponse.setStatusText(statusText);

    if (statusCode != 101) {
        m_mode = Failed;
        m_failureReason = "Unexpected response code: " + String::number(statusCode);
        return len;
    }
    m_mode = Normal;
    if (!strnstr(header, "\r\n\r\n", len)) {
        // Just hasn't been received fully yet.
        m_mode = Incomplete;
        return -1;
    }
    const char* p = readHTTPHeaders(header + lineLength, header + len);
    if (!p) {
        WTF_LOG(Network, "WebSocketHandshake %p readServerHandshake() readHTTPHeaders() failed", this);
        m_mode = Failed; // m_failureReason is set inside readHTTPHeaders().
        return len;
    }
    if (!checkResponseHeaders()) {
        WTF_LOG(Network, "WebSocketHandshake %p readServerHandshake() checkResponseHeaders() failed", this);
        m_mode = Failed;
        return p - header;
    }

    m_mode = Connected;
    return p - header;
}

WebSocketHandshake::Mode WebSocketHandshake::mode() const
{
    return m_mode;
}

String WebSocketHandshake::failureReason() const
{
    return m_failureReason;
}

String WebSocketHandshake::serverWebSocketProtocol() const
{
    return m_serverHandshakeResponse.headerFields().get("Sec-WebSocket-Protocol");
}

String WebSocketHandshake::serverSetCookie() const
{
    return m_serverHandshakeResponse.headerFields().get("Set-Cookie");
}

String WebSocketHandshake::serverSetCookie2() const
{
    return m_serverHandshakeResponse.headerFields().get("Set-Cookie2");
}

String WebSocketHandshake::serverUpgrade() const
{
    return m_serverHandshakeResponse.headerFields().get("Upgrade");
}

String WebSocketHandshake::serverConnection() const
{
    return m_serverHandshakeResponse.headerFields().get("Connection");
}

String WebSocketHandshake::serverWebSocketAccept() const
{
    return m_serverHandshakeResponse.headerFields().get("Sec-WebSocket-Accept");
}

String WebSocketHandshake::acceptedExtensions() const
{
    return m_extensionDispatcher.acceptedExtensions();
}

const WebSocketHandshakeResponse& WebSocketHandshake::serverHandshakeResponse() const
{
    return m_serverHandshakeResponse;
}

void WebSocketHandshake::addExtensionProcessor(PassOwnPtr<WebSocketExtensionProcessor> processor)
{
    m_extensionDispatcher.addProcessor(processor);
}

blink::KURL WebSocketHandshake::httpURLForAuthenticationAndCookies() const
{
    blink::KURL url = m_url;
    bool couldSetProtocol = url.setProtocol(m_secure ? "https" : "http");
    ASSERT_UNUSED(couldSetProtocol, couldSetProtocol);
    return url;
}

// Returns the header length (including "\r\n"), or -1 if we have not received enough data yet.
// If the line is malformed or the status code is not a 3-digit number,
// statusCode and statusText will be set to -1 and a null string, respectively.
int WebSocketHandshake::readStatusLine(const char* header, size_t headerLength, int& statusCode, String& statusText)
{
    // Arbitrary size limit to prevent the server from sending an unbounded
    // amount of data with no newlines and forcing us to buffer it all.
    static const int maximumLength = 1024;

    statusCode = -1;
    statusText = String();

    const char* space1 = nullptr;
    const char* space2 = nullptr;
    const char* p;
    size_t consumedLength;

    for (p = header, consumedLength = 0; consumedLength < headerLength; p++, consumedLength++) {
        if (*p == ' ') {
            if (!space1)
                space1 = p;
            else if (!space2)
                space2 = p;
        } else if (*p == '\0') {
            // The caller isn't prepared to deal with null bytes in status
            // line. WebSockets specification doesn't prohibit this, but HTTP
            // does, so we'll just treat this as an error.
            m_failureReason = "Status line contains embedded null";
            return p + 1 - header;
        } else if (*p == '\n')
            break;
    }
    if (consumedLength == headerLength)
        return -1; // We have not received '\n' yet.

    const char* end = p + 1;
    int lineLength = end - header;
    if (lineLength > maximumLength) {
        m_failureReason = "Status line is too long";
        return maximumLength;
    }

    // The line must end with "\r\n".
    if (lineLength < 2 || *(end - 2) != '\r') {
        m_failureReason = "Status line does not end with CRLF";
        return lineLength;
    }

    if (!space1 || !space2) {
        m_failureReason = "No response code found: " + trimInputSample(header, lineLength - 2);
        return lineLength;
    }

    String statusCodeString(space1 + 1, space2 - space1 - 1);
    if (statusCodeString.length() != 3) // Status code must consist of three digits.
        return lineLength;
    for (int i = 0; i < 3; ++i)
        if (statusCodeString[i] < '0' || statusCodeString[i] > '9') {
            m_failureReason = "Invalid status code: " + statusCodeString;
            return lineLength;
        }

    bool ok = false;
    statusCode = statusCodeString.toInt(&ok);
    ASSERT(ok);

    statusText = String(space2 + 1, end - space2 - 3); // Exclude "\r\n".
    return lineLength;
}

const char* WebSocketHandshake::readHTTPHeaders(const char* start, const char* end)
{
    AtomicString name;
    AtomicString value;
    bool sawSecWebSocketExtensionsHeaderField = false;
    bool sawSecWebSocketAcceptHeaderField = false;
    bool sawSecWebSocketProtocolHeaderField = false;
    const char* p = start;
    for (; p < end; /*p++*/) {
        size_t consumedLength = parseHTTPHeader(p, end - p, m_failureReason, name, value);
        if (!consumedLength)
            return 0;
        p += consumedLength;

        // Stop once we consumed an empty line.
        if (name.isEmpty())
            break;

        if (equalIgnoringCase("sec-websocket-extensions", name)) {
            if (sawSecWebSocketExtensionsHeaderField) {
                m_failureReason = "The Sec-WebSocket-Extensions header MUST NOT appear more than once in an HTTP response";
                return 0;
            }
            if (!m_extensionDispatcher.processHeaderValue(value)) {
                m_failureReason = m_extensionDispatcher.failureReason();
                return 0;
            }
            sawSecWebSocketExtensionsHeaderField = true;
        } else if (equalIgnoringCase("Sec-WebSocket-Accept", name)) {
            if (sawSecWebSocketAcceptHeaderField) {
                m_failureReason = "The Sec-WebSocket-Accept header MUST NOT appear more than once in an HTTP response";
                return 0;
            }
            m_serverHandshakeResponse.addHeaderField(name, value);
            sawSecWebSocketAcceptHeaderField = true;
        } else if (equalIgnoringCase("Sec-WebSocket-Protocol", name)) {
            if (sawSecWebSocketProtocolHeaderField) {
                m_failureReason = "The Sec-WebSocket-Protocol header MUST NOT appear more than once in an HTTP response";
                return 0;
            }
            m_serverHandshakeResponse.addHeaderField(name, value);
            sawSecWebSocketProtocolHeaderField = true;
        } else
            m_serverHandshakeResponse.addHeaderField(name, value);
    }
    return p;
}

bool WebSocketHandshake::checkResponseHeaders()
{
    const String& serverWebSocketProtocol = this->serverWebSocketProtocol();
    const String& serverUpgrade = this->serverUpgrade();
    const String& serverConnection = this->serverConnection();
    const String& serverWebSocketAccept = this->serverWebSocketAccept();

    if (serverUpgrade.isNull()) {
        m_failureReason = "Error during WebSocket handshake: 'Upgrade' header is missing";
        return false;
    }
    if (serverConnection.isNull()) {
        m_failureReason = "Error during WebSocket handshake: 'Connection' header is missing";
        return false;
    }
    if (serverWebSocketAccept.isNull()) {
        m_failureReason = "Error during WebSocket handshake: 'Sec-WebSocket-Accept' header is missing";
        return false;
    }

    if (!equalIgnoringCase(serverUpgrade, "websocket")) {
        m_failureReason = "Error during WebSocket handshake: 'Upgrade' header value is not 'WebSocket'";
        return false;
    }
    if (!equalIgnoringCase(serverConnection, "upgrade")) {
        m_failureReason = "Error during WebSocket handshake: 'Connection' header value is not 'Upgrade'";
        return false;
    }

    if (serverWebSocketAccept != m_expectedAccept) {
        m_failureReason = "Error during WebSocket handshake: Sec-WebSocket-Accept mismatch";
        return false;
    }
    if (!serverWebSocketProtocol.isNull()) {
        if (m_clientProtocol.isEmpty()) {
            m_failureReason = "Error during WebSocket handshake: Sec-WebSocket-Protocol mismatch";
            return false;
        }
        Vector<String> result;
        static const char* subProtocolSeperator = ", ";
        m_clientProtocol.split(String(subProtocolSeperator), result);
        if (!result.contains(serverWebSocketProtocol)) {
            m_failureReason = "Error during WebSocket handshake: Sec-WebSocket-Protocol mismatch";
            return false;
        }
    }
    return true;
}

} // namespace net
