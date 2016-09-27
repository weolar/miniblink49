// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGeofencingEventType_h
#define WebGeofencingEventType_h

namespace blink {

enum WebGeofencingEventType {
    WebGeofencingEventTypeEnter,
    WebGeofencingEventTypeLeave,
    WebGeofencingEventTypeLast = WebGeofencingEventTypeLeave
};

} // namespace blink

#endif // WebGeofencingEventType_h
