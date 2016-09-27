// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorUserMedia_h
#define NavigatorUserMedia_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class Navigator;
class MediaDevices;

class NavigatorUserMedia final : public GarbageCollected<NavigatorUserMedia>, public HeapSupplement<Navigator> {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorUserMedia)
public:
    static MediaDevices* mediaDevices(Navigator&);
    DECLARE_VIRTUAL_TRACE();

private:
    NavigatorUserMedia();
    MediaDevices* getMediaDevices();
    static const char* supplementName();
    static NavigatorUserMedia& from(Navigator&);

    Member<MediaDevices> m_mediaDevices;
};

} // namespace blink

#endif
