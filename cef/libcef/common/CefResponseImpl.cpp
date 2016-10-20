// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "libcef/common/CefResponseImpl.h"
#include "include/base/cef_logging.h"
#include <string>
#include "libcef/common/StringUtil.h"
#include "third_party/WebKit/public/platform/WebHTTPHeaderVisitor.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"

#define CHECK_READONLY_RETURN_VOID() \
  if (m_readOnly) { \
    NOTREACHED() << "object is read only"; \
    return; \
  }

#define CHECK_READONLY_RETURN(val) \
  if (m_readOnly) { \
    NOTREACHED() << "object is read only"; \
    return val; \
      }

// CefResponse ----------------------------------------------------------------

// static
CefRefPtr<CefResponse> CefResponse::Create() {
    CefRefPtr<CefResponse> response(new CefResponseImpl());
    return response;
}

// CefResponseImpl ------------------------------------------------------------

CefResponseImpl::CefResponseImpl()
    : m_statusCode(0),
    m_readOnly(false) {
}

bool CefResponseImpl::IsReadOnly() {
    MutexLocker lockScope(m_lock);
    return m_readOnly;
}

int CefResponseImpl::GetStatus() {
    MutexLocker lockScope(m_lock);
    return m_statusCode;
}

void CefResponseImpl::SetStatus(int status) {
    MutexLocker lockScope(m_lock);
    CHECK_READONLY_RETURN_VOID();
    m_statusCode = status;
}

CefString CefResponseImpl::GetStatusText() {
    MutexLocker lockScope(m_lock);
    return m_statusText;
}

void CefResponseImpl::SetStatusText(const CefString& statusText) {
    MutexLocker lockScope(m_lock);
    CHECK_READONLY_RETURN_VOID();
    m_statusText = statusText;
}

CefString CefResponseImpl::GetMimeType() {
    MutexLocker lockScope(m_lock);
    return m_mimeType;
}

void CefResponseImpl::SetMimeType(const CefString& mimeType) {
    MutexLocker lockScope(m_lock);
    CHECK_READONLY_RETURN_VOID();
    m_mimeType = mimeType;
}

CefString CefResponseImpl::GetHeader(const CefString& name) {
    MutexLocker lockScope(m_lock);

    CefString value;

    HeaderMap::const_iterator it = m_headerMap.find(name);
    if (it != m_headerMap.end())
        value = it->second;

    return value;
}

void CefResponseImpl::GetHeaderMap(HeaderMap& map) {
    MutexLocker lockScope(m_lock);
    map = m_headerMap;
}

void CefResponseImpl::SetHeaderMap(const HeaderMap& headerMap) {
    MutexLocker lockScope(m_lock);
    CHECK_READONLY_RETURN_VOID();
    m_headerMap = headerMap;
}

// net::HttpResponseHeaders* CefResponseImpl::GetResponseHeaders() {
//   MutexLocker lockScope(m_lock);
// 
//   std::string response;
//   std::string status_text;
//   bool has_content_type_header = false;
// 
//   if (!m_statusText.empty())
//     status_text = m_statusText;
//   else
//     status_text = (m_statusCode == 200)?"OK":"ERROR";
// 
//   base::SStringPrintf(&response, "HTTP/1.1 %d %s", m_statusCode,
//                       status_text.c_str());
//   if (m_headerMap.size() > 0) {
//     for (HeaderMap::const_iterator header = m_headerMap.begin();
//         header != m_headerMap.end();
//         ++header) {
//       const CefString& key = header->first;
//       const CefString& value = header->second;
// 
//       if (!key.empty()) {
//         // Delimit with "\0" as required by net::HttpResponseHeaders.
//         std::string key_str(key);
//         std::string value_str(value);
//         base::StringAppendF(&response, "%c%s: %s", '\0', key_str.c_str(),
//                             value_str.c_str());
// 
//         if (!has_content_type_header &&
//             key_str == net::HttpRequestHeaders::kContentType) {
//           has_content_type_header = true;
//         }
//       }
//     }
//   }
// 
//   if (!has_content_type_header) {
//     std::string mime_type;
//     if (!m_mimeType.empty())
//       mime_type = m_mimeType;
//     else
//       mime_type = "text/html";
// 
//     base::StringAppendF(&response, "%c%s: %s", '\0',
//         net::HttpRequestHeaders::kContentType, mime_type.c_str());
//   }
// 
//   return new net::HttpResponseHeaders(response);
// }
// 
// void CefResponseImpl::SetResponseHeaders(
//     const net::HttpResponseHeaders& headers) {
//   MutexLocker lockScope(m_lock);
//   CHECK_READONLY_RETURN_VOID();
// 
//   m_headerMap.empty();
// 
//   void* iter = NULL;
//   std::string name, value;
//   while (headers.EnumerateHeaderLines(&iter, &name, &value))
//     m_headerMap.insert(std::make_pair(name, value));
// 
//   m_statusCode = headers.response_code();
//   m_statusText = headers.GetStatusText();
// 
//   std::string mime_type;
//   if (headers.GetMimeType(&mime_type))
//     m_mimeType = mime_type;
//   else
//     m_mimeType.clear();
// }

blink::WebURLResponse CefResponseImpl::GetWebResponseHeaders() {
    MutexLocker lockScope(m_lock);
    blink::WebURLResponse response;
    response.initialize();
    response.setMIMEType(blink::WebString::fromUTF8(m_mimeType.ToString()));
    response.setHTTPStatusCode(m_statusCode);

    std::string statustText;
    bool has_content_type_header = false;

    if (!m_statusText.empty())
        statustText = m_statusText;
    else
        statustText = (m_statusCode == 200) ? "OK" : "ERROR";

    response.setHTTPStatusText(blink::WebString::fromUTF8(statustText.c_str()));

    return response;
}

void CefResponseImpl::Set(const blink::WebURLResponse& response) {
    DCHECK(!response.isNull());

    MutexLocker lockScope(m_lock);
    CHECK_READONLY_RETURN_VOID();

    blink::WebString str;
    m_statusCode = response.httpStatusCode();
    str = response.httpStatusText();
    cef::WebStringToCefString(str, m_statusText);
    str = response.mimeType();
    cef::WebStringToCefString(str, m_mimeType);

    class HeaderVisitor : public blink::WebHTTPHeaderVisitor {
    public:
        explicit HeaderVisitor(HeaderMap* map) : map_(map) {}

        void visitHeader(const blink::WebString& name, const blink::WebString& value) override {
            CefString cefName;
            cef::WebStringToCefString(name, cefName);
            CefString cefValue;
            cef::WebStringToCefString(name, cefValue);
            map_->insert(std::make_pair(cefName, cefValue));
        }

    private:
        HeaderMap* map_;
    };

    HeaderVisitor visitor(&m_headerMap);
    response.visitHTTPHeaderFields(&visitor);
}

// void CefResponseImpl::Set(const net::URLRequest* request) {
//   DCHECK(request);
// 
//   const net::HttpResponseHeaders* headers = request->response_headers();
//   if (headers)
//     SetResponseHeaders(*headers);
// }

void CefResponseImpl::SetReadOnly(bool read_only) {
    MutexLocker lockScope(m_lock);
    m_readOnly = read_only;
}
#endif
