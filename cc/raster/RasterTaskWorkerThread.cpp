/*
* Copyright (C) 2013 weolar Inc.  All rights reserved.
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
*     * Neither the name of weolar Inc. nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*/

#include "config.h"
#include "core/html/parser/RasterTaskWorkerThread.h"

#include "platform/Task.h"
#include "platform/ThreadSafeFunctional.h"
#include "public/platform/Platform.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

static RasterTaskWorkerThread* s_sharedThread = 0;

RasterTaskWorkerThread::RasterTaskWorkerThread()
{
}

RasterTaskWorkerThread::~RasterTaskWorkerThread()
{
}

void RasterTaskWorkerThread::init()
{
    ASSERT(!s_sharedThread);
    s_sharedThread = new RasterTaskWorkerThread;
}

void RasterTaskWorkerThread::setupHTMLParserThread()
{
    ASSERT(m_thread);
    m_thread->initialize();
}

void RasterTaskWorkerThread::shutdown()
{
    ASSERT(s_sharedThread);
    // currentThread will always be non-null in production, but can be null in Chromium unit tests.
    if (Platform::current()->currentThread() && s_sharedThread->isRunning()) {
        s_sharedThread->postTask(threadSafeBind(&RasterTaskWorkerThread::cleanupHTMLParserThread, AllowCrossThreadAccess(s_sharedThread)));
    }
    delete s_sharedThread;
    s_sharedThread = 0;
}

void RasterTaskWorkerThread::cleanupHTMLParserThread()
{
    m_thread->shutdown();
}

RasterTaskWorkerThread* RasterTaskWorkerThread::shared()
{
    return s_sharedThread;
}

WebThread& RasterTaskWorkerThread::platformThread()
{
    if (!isRunning()) {
        m_thread = WebThreadSupportingGC::create("RasterTaskWorkerThread");
        postTask(threadSafeBind(&RasterTaskWorkerThread::setupHTMLParserThread, AllowCrossThreadAccess(this)));
    }
    return m_thread->platformThread();
}

bool RasterTaskWorkerThread::isRunning()
{
    return !!m_thread;
}

void RasterTaskWorkerThread::postTask(PassOwnPtr<Closure> closure)
{
    platformThread().postTask(FROM_HERE, new Task(closure));
}

} // namespace blink
