/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "public/web/WebTestingSupport.h"

#include "bindings/modules/v8/V8InternalsPartial.h"
#include "core/testing/v8/WebCoreTestSupport.h"
#include "public/web/WebLocalFrame.h"
#include <v8/include/v8.h>

namespace blink {

v8::Local<v8::Value> WebTestingSupport::createInternalsObject(v8::Local<v8::Context> context)
{
    V8InternalsPartial::initialize();
    return WebCoreTestSupport::createInternalsObject(context);
}

void WebTestingSupport::injectInternalsObject(WebLocalFrame* frame)
{
    V8InternalsPartial::initialize();
    v8::HandleScope handleScope(v8::Isolate::GetCurrent());
    WebCoreTestSupport::injectInternalsObject(frame->mainWorldScriptContext());
}

void WebTestingSupport::resetInternalsObject(WebLocalFrame* frame)
{
    v8::HandleScope handleScope(v8::Isolate::GetCurrent());
    WebCoreTestSupport::resetInternalsObject(frame->mainWorldScriptContext());
}

}
