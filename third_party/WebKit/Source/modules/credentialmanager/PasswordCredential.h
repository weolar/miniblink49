// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PasswordCredential_h
#define PasswordCredential_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "modules/credentialmanager/Credential.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"

namespace blink {

class DOMFormData;
class WebPasswordCredential;

class PasswordCredential final : public Credential {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PasswordCredential* create(const String& id, const String& password, ExceptionState& exceptionState)
    {
        return create(id, password, emptyString(), emptyString(), exceptionState);
    }

    static PasswordCredential* create(const String& id, const String& password, const String& name, ExceptionState& exceptionState)
    {
        return create(id, password, name, emptyString(), exceptionState);
    }

    static PasswordCredential* create(const String& id, const String& password, const String& name, const String& icon, ExceptionState&);
    static PasswordCredential* create(WebPasswordCredential*);

    // PasswordCredential.idl
    const String& password() const;
    DOMFormData* formData() const { return m_formData; }

    DECLARE_VIRTUAL_TRACE();

private:
    PasswordCredential(WebPasswordCredential*);
    PasswordCredential(const String& id, const String& password, const String& name, const KURL& icon);

    Member<DOMFormData> m_formData;
};

} // namespace blink

#endif // PasswordCredential_h
