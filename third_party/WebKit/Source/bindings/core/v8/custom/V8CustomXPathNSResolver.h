/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef V8CustomXPathNSResolver_h
#define V8CustomXPathNSResolver_h

#include "bindings/core/v8/ScriptState.h"
#include "core/xml/XPathNSResolver.h"
#include "wtf/Forward.h"
#include "wtf/RefPtr.h"
#include <v8.h>

namespace blink {

// V8CustomXPathNSResolver does not create a persistent handle to the
// given resolver object. So the lifetime of V8CustomXPathNSResolver
// must not exceed the lifetime of the passed handle.
class V8CustomXPathNSResolver final : public XPathNSResolver {
public:
    static V8CustomXPathNSResolver* create(ScriptState*, v8::Local<v8::Object> resolver);

    AtomicString lookupNamespaceURI(const String& prefix) override;

    DECLARE_VIRTUAL_TRACE();

private:
    V8CustomXPathNSResolver(ScriptState*, v8::Local<v8::Object> resolver);

    RefPtr<ScriptState> m_scriptState;
    v8::Local<v8::Object> m_resolver; // Handle to resolver object.
};

} // namespace blink

#endif // V8CustomXPathNSResolver_h
