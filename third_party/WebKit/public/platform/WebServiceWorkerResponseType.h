// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerResponseType_h
#define WebServiceWorkerResponseType_h

namespace blink {

enum WebServiceWorkerResponseType {
    WebServiceWorkerResponseTypeBasic,
    WebServiceWorkerResponseTypeCORS,
    WebServiceWorkerResponseTypeDefault,
    WebServiceWorkerResponseTypeError,
    WebServiceWorkerResponseTypeOpaque,
    WebServiceWorkerResponseTypeLast = WebServiceWorkerResponseTypeOpaque
};

} // namespace blink

#endif // WebServiceWorkerResponseType_h
