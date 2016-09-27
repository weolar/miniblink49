// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ServiceWorkerWindowClient_h
#define ServiceWorkerWindowClient_h

#include "bindings/core/v8/ScriptPromise.h"
#include "modules/ModulesExport.h"
#include "modules/serviceworkers/ServiceWorkerClient.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class ScriptPromiseResolver;
class ScriptState;

class MODULES_EXPORT ServiceWorkerWindowClient final : public ServiceWorkerClient {
    DEFINE_WRAPPERTYPEINFO();
public:
    // To be used by CallbackPromiseAdapter.
    typedef WebServiceWorkerClientInfo WebType;

    static ServiceWorkerWindowClient* take(ScriptPromiseResolver*, PassOwnPtr<WebType>);

    static ServiceWorkerWindowClient* create(const WebServiceWorkerClientInfo&);
    ~ServiceWorkerWindowClient() override;

    // WindowClient.idl
    String visibilityState() const;
    bool focused() const { return m_isFocused; }
    ScriptPromise focus(ScriptState*);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit ServiceWorkerWindowClient(const WebServiceWorkerClientInfo&);

    WebPageVisibilityState m_pageVisibilityState;
    bool m_isFocused;
};

} // namespace blink

#endif // ServiceWorkerWindowClient_h
