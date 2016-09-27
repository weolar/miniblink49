/*
 * Copyright (C) 2013 Google Inc.  All rights reserved.
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

#include "config.h"
#include "core/html/parser/HTMLParserThread.h"

#include "platform/Task.h"
#include "platform/ThreadSafeFunctional.h"
#include "public/platform/Platform.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

static HTMLParserThread* s_sharedThread = 0;

HTMLParserThread::HTMLParserThread()
{
}

HTMLParserThread::~HTMLParserThread()
{
}

void HTMLParserThread::init()
{
    ASSERT(!s_sharedThread);
    s_sharedThread = new HTMLParserThread;
}

void HTMLParserThread::setupHTMLParserThread()
{
    ASSERT(m_thread);
    m_thread->initialize();
}

void HTMLParserThread::shutdown()
{
    ASSERT(s_sharedThread);
    // currentThread will always be non-null in production, but can be null in Chromium unit tests.
    if (Platform::current()->currentThread() && s_sharedThread->isRunning()) {
        s_sharedThread->postTask(threadSafeBind(&HTMLParserThread::cleanupHTMLParserThread, AllowCrossThreadAccess(s_sharedThread)));
    }
    delete s_sharedThread;
    s_sharedThread = 0;
}

void HTMLParserThread::cleanupHTMLParserThread()
{
    m_thread->shutdown();
}

HTMLParserThread* HTMLParserThread::shared()
{
    return s_sharedThread;
}

WebThread& HTMLParserThread::platformThread()
{
    if (!isRunning()) {
        m_thread = WebThreadSupportingGC::create("HTMLParserThread");
        postTask(threadSafeBind(&HTMLParserThread::setupHTMLParserThread, AllowCrossThreadAccess(this)));
    }
    return m_thread->platformThread();
}

bool HTMLParserThread::isRunning()
{
    return !!m_thread;
}

void HTMLParserThread::postTask(PassOwnPtr<Closure> closure)
{
    platformThread().postTask(FROM_HERE, new Task(closure));
}

} // namespace blink
