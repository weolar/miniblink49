// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FederatedCredential_h
#define FederatedCredential_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "modules/credentialmanager/Credential.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"

namespace blink {

class FederatedCredentialData;
class WebFederatedCredential;

class FederatedCredential final : public Credential {
    DEFINE_WRAPPERTYPEINFO();
public:
    static FederatedCredential* create(const FederatedCredentialData&, ExceptionState&);
    static FederatedCredential* create(WebFederatedCredential*);

    // FederatedCredential.idl
    const KURL& provider() const;

    // TODO(mkwst): This is a stub, as we don't yet have any support on the Chromium-side.
    const String protocol() const { return String(); }

private:
    FederatedCredential(WebFederatedCredential*);
    FederatedCredential(const String& id, const KURL& provider, const String& name, const KURL& icon);
};

} // namespace blink

#endif // FederatedCredential_h
