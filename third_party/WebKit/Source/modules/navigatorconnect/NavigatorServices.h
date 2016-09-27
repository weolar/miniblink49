// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorServices_h
#define NavigatorServices_h

#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExecutionContext;
class ServicePortCollection;
class Navigator;

class NavigatorServices final : public GarbageCollectedFinalized<NavigatorServices>, public HeapSupplement<Navigator> {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorServices);
    WTF_MAKE_NONCOPYABLE(NavigatorServices);
public:
    virtual ~NavigatorServices();
    static NavigatorServices& from(Navigator&);

    static ServicePortCollection* services(ExecutionContext*, Navigator&);
    ServicePortCollection* services(ExecutionContext*);

    DECLARE_VIRTUAL_TRACE();

private:
    NavigatorServices();
    static const char* supplementName();

    Member<ServicePortCollection> m_services;
};

} // namespace blink

#endif // NavigatorServices_h
