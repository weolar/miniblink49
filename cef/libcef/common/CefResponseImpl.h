// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_COMMON_RESPONSE_IMPL_H_
#define CEF_LIBCEF_COMMON_RESPONSE_IMPL_H_
#pragma once

#include "include/cef_response.h"

#include "wtf/ThreadingPrimitives.h"

namespace net {
// class HttpResponseHeaders;
// class URLRequest;
}

namespace blink {
class WebURLResponse;
}

// Implementation of CefResponse.
class CefResponseImpl : public CefResponse {
 public:
  CefResponseImpl();

  // CefResponse methods.
  bool IsReadOnly() override;
  int GetStatus() override;
  void SetStatus(int status) override;
  CefString GetStatusText() override;
  void SetStatusText(const CefString& statusText) override;
  CefString GetMimeType() override;
  void SetMimeType(const CefString& mimeType) override;
  CefString GetHeader(const CefString& name) override;
  void GetHeaderMap(HeaderMap& headerMap) override;
  void SetHeaderMap(const HeaderMap& headerMap) override;

//   net::HttpResponseHeaders* GetResponseHeaders();
//   void SetResponseHeaders(const net::HttpResponseHeaders& headers);

  void Set(const blink::WebURLResponse& response);
  //void Set(const net::URLRequest* request);

  blink::WebURLResponse GetWebResponseHeaders();

  void SetReadOnly(bool read_only);

 protected:
  int m_statusCode;
  CefString m_statusText;
  CefString m_mimeType;
  HeaderMap m_headerMap;
  bool m_readOnly;

  WTF::Mutex m_lock;

  IMPLEMENT_REFCOUNTING(CefResponseImpl);
};

#endif  // CEF_LIBCEF_COMMON_RESPONSE_IMPL_H_
