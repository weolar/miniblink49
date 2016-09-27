/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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
#include "core/dom/StringCallback.h"

#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace {

class DispatchCallbackTask final : public ExecutionContextTask {
public:
    static PassOwnPtr<DispatchCallbackTask> create(StringCallback* callback, const String& data, const String& taskName)
    {
        return adoptPtr(new DispatchCallbackTask(callback, data, taskName));
    }

    void performTask(ExecutionContext*) override
    {
        m_callback->handleEvent(m_data);
    }

    String taskNameForInstrumentation() const override
    {
        return m_taskName;
    }

private:
    DispatchCallbackTask(StringCallback* callback, const String& data, const String& taskName)
        : m_callback(callback)
        , m_data(data)
        , m_taskName(taskName)
    {
    }

    Persistent<StringCallback> m_callback;
    const String m_data;
    const String m_taskName;
};

} // namespace

void StringCallback::scheduleCallback(StringCallback* callback, ExecutionContext* context, const String& data, const String& instrumentationName)
{
    context->postTask(FROM_HERE, DispatchCallbackTask::create(callback, data, instrumentationName));
}

} // namespace blink
