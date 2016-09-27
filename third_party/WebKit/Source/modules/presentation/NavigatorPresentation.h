// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorPresentation_h
#define NavigatorPresentation_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class Navigator;
class Presentation;

class NavigatorPresentation final : public GarbageCollectedFinalized<NavigatorPresentation>, public HeapSupplement<Navigator> {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorPresentation);
public:
    static NavigatorPresentation& from(Navigator&);
    static Presentation* presentation(Navigator&);

    DECLARE_VIRTUAL_TRACE();

private:
    NavigatorPresentation();

    static const char* supplementName();
    Presentation* presentation();

    Member<Presentation> m_presentation;
};

} // namespace blink

#endif // NavigatorPresentation_h
