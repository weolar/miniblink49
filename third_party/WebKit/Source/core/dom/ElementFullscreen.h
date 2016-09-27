// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ElementFullscreen_h
#define ElementFullscreen_h

#include "core/events/EventTarget.h"

namespace blink {

class Element;

class ElementFullscreen {
public:
    static void requestFullscreen(Element&);

    static void webkitRequestFullscreen(Element&);

    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(webkitfullscreenchange);
    DEFINE_STATIC_ATTRIBUTE_EVENT_LISTENER(webkitfullscreenerror);
};

} // namespace blink

#endif // ElementFullscreen_h
