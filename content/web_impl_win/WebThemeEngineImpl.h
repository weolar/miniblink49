// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebThemeEngineImpl_h
#define WebThemeEngineImpl_h

#include "third_party/WebKit/public/platform/WebThemeEngine.h"

namespace content {

class NativeThemeWin;
class CustomTheme;

class WebThemeEngineImpl : public blink::WebThemeEngine {
public:
    WebThemeEngineImpl();
    ~WebThemeEngineImpl();

    // WebThemeEngine methods:
    virtual blink::WebSize getSize(Part) OVERRIDE;
    // Paint the given the given theme part.
    virtual void paint(blink::WebCanvas*, blink::WebThemeEngine::Part, blink::WebThemeEngine::State, const blink::WebRect&, const ExtraParams*);
private:
    NativeThemeWin* m_nativeThemeWin;
    CustomTheme* m_theme;
};

}  // namespace content

#endif  // WebThemeEngineImpl_h
