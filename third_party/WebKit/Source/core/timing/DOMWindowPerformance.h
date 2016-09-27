// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMWindowPerformance_h
#define DOMWindowPerformance_h

#include "core/CoreExport.h"
#include "core/frame/DOMWindowProperty.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "wtf/Noncopyable.h"

namespace blink {

class DOMWindow;
class Performance;

class CORE_EXPORT DOMWindowPerformance final : public NoBaseWillBeGarbageCollected<DOMWindowPerformance>, public WillBeHeapSupplement<LocalDOMWindow>, public DOMWindowProperty {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DOMWindowPerformance);
    DECLARE_EMPTY_VIRTUAL_DESTRUCTOR_WILL_BE_REMOVED(DOMWindowPerformance);
    WTF_MAKE_NONCOPYABLE(DOMWindowPerformance);
public:
    static DOMWindowPerformance& from(LocalDOMWindow&);
    static Performance* performance(DOMWindow&);

    DECLARE_TRACE();

private:
    explicit DOMWindowPerformance(LocalDOMWindow&);
    static const char* supplementName();

    Performance* performance();

    // TODO(sof): try to move this direct reference and instead rely on frame().
    RawPtrWillBeMember<LocalDOMWindow> m_window;
    PersistentWillBeMember<Performance> m_performance;
};

} // namespace blink

#endif // DOMWindowPerformance_h
