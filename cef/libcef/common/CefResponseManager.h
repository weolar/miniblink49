// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_LIBCEF_COMMON_RESPONSE_MANAGER_H_
#define CEF_LIBCEF_COMMON_RESPONSE_MANAGER_H_

class CefResponseManager {
public:
    // Used for handling response messages.
    class Handler : public virtual CefBase {
    public:
        //virtual void OnResponse(const Cef_Response_Params& params) = 0;
    };
};

#endif  // CEF_LIBCEF_COMMON_RESPONSE_MANAGER_H_