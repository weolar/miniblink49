// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebNotificationData_h
#define WebNotificationData_h

#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebVector.h"

namespace blink {

// Structure representing the data associated with a Web Notification.
struct WebNotificationData {
    enum Direction {
        DirectionLeftToRight,
        DirectionRightToLeft
    };

    WebNotificationData()
        : direction(DirectionLeftToRight)
        , silent(false)
    {
    }

    WebNotificationData(const WebString& title, Direction direction, const WebString& lang, const WebString& body, const WebString& tag, const WebURL& icon, const WebVector<int>& vibrate, bool silent, const WebVector<char>& data)
        : title(title)
        , direction(direction)
        , lang(lang)
        , body(body)
        , tag(tag)
        , icon(icon)
        , vibrate(vibrate)
        , silent(silent)
        , data(data)
    {
    }

    WebString title;
    Direction direction;
    WebString lang;
    WebString body;
    WebString tag;
    WebURL icon;
    WebVector<int> vibrate;
    bool silent;
    WebVector<char> data;
};

} // namespace blink

#endif // WebNotificationData_h
