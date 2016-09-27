/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
 * Copyright (C) 2012, Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef NavigatorContentUtils_h
#define NavigatorContentUtils_h

#include "modules/ModulesExport.h"
#include "modules/navigatorcontentutils/NavigatorContentUtilsClient.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExceptionState;
class LocalFrame;
class Navigator;

class MODULES_EXPORT NavigatorContentUtils final : public NoBaseWillBeGarbageCollectedFinalized<NavigatorContentUtils>, public WillBeHeapSupplement<LocalFrame> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(NavigatorContentUtils);
public:
    virtual ~NavigatorContentUtils();

    static NavigatorContentUtils* from(LocalFrame&);
    static const char* supplementName();

    static void registerProtocolHandler(Navigator&, const String& scheme, const String& url, const String& title, ExceptionState&);
    static String isProtocolHandlerRegistered(Navigator&, const String& scheme, const String& url, ExceptionState&);
    static void unregisterProtocolHandler(Navigator&, const String& scheme, const String& url, ExceptionState&);

    static PassOwnPtrWillBeRawPtr<NavigatorContentUtils> create(PassOwnPtr<NavigatorContentUtilsClient>);

    DEFINE_INLINE_VIRTUAL_TRACE() { WillBeHeapSupplement<LocalFrame>::trace(visitor); }

    void setClientForTest(PassOwnPtr<NavigatorContentUtilsClient> client) { m_client = client; }

private:
    explicit NavigatorContentUtils(PassOwnPtr<NavigatorContentUtilsClient> client)
        : m_client(client)
    { }

    NavigatorContentUtilsClient* client() { return m_client.get(); }

    OwnPtr<NavigatorContentUtilsClient> m_client;
};

} // namespace blink

#endif // NavigatorContentUtils_h
