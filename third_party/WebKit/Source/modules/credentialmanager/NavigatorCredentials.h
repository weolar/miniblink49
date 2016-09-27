// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorCredentials_h
#define NavigatorCredentials_h

#include "core/frame/DOMWindowProperty.h"
#include "core/frame/Navigator.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class CredentialsContainer;
class Navigator;

class NavigatorCredentials final : public GarbageCollectedFinalized<NavigatorCredentials>, public HeapSupplement<Navigator>, public DOMWindowProperty {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorCredentials);
public:
    static NavigatorCredentials& from(Navigator&);
    virtual ~NavigatorCredentials();

    // NavigatorCredentials.idl
    static CredentialsContainer* credentials(Navigator&);

    DECLARE_TRACE();

private:
    explicit NavigatorCredentials(Navigator&);
    CredentialsContainer* credentials();

    static const char* supplementName();

    Member<CredentialsContainer> m_credentialsContainer;
};

} // namespace blink

#endif // NavigatorCredentials_h
