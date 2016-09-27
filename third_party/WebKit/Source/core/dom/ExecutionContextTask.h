/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
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

#ifndef ExecutionContextTask_h
#define ExecutionContextTask_h

#include "core/CoreExport.h"
#include "wtf/FastAllocBase.h"
#include "wtf/Functional.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExecutionContext;

class CORE_EXPORT ExecutionContextTask {
    WTF_MAKE_NONCOPYABLE(ExecutionContextTask);
    WTF_MAKE_FAST_ALLOCATED(ExecutionContextTask);
public:
    ExecutionContextTask() { }
    virtual ~ExecutionContextTask() { }
    virtual void performTask(ExecutionContext*) = 0;
    virtual String taskNameForInstrumentation() const { return String(); }
};

namespace internal {

template<typename T>
class CallClosureTaskBase : public ExecutionContextTask {
protected:
    CallClosureTaskBase(PassOwnPtr<Function<T>> closure, bool isSameThread)
        : m_closure(closure)
#if ENABLE(ASSERT)
        , m_isSameThread(isSameThread)
        , m_createdThread(currentThread())
#endif
    {
    }

    void checkThread()
    {
#if ENABLE(ASSERT)
        if (m_isSameThread) {
            RELEASE_ASSERT(m_createdThread == currentThread());
        }
#endif
    }

    OwnPtr<Function<T>> m_closure;

private:
#if ENABLE(ASSERT)
    bool m_isSameThread;
    ThreadIdentifier m_createdThread;
#endif
};

class CallClosureTask final : public CallClosureTaskBase<void()> {
public:
    // Do not use |create| other than in createCrossThreadTask and
    // createSameThreadTask.
    // See http://crbug.com/390851
    static PassOwnPtr<CallClosureTask> create(PassOwnPtr<Closure> closure, bool isSameThread = false)
    {
        return adoptPtr(new CallClosureTask(closure, isSameThread));
    }

    void performTask(ExecutionContext*) override
    {
        checkThread();
        (*m_closure)();
    }

private:
    CallClosureTask(PassOwnPtr<Closure> closure, bool isSameThread)
        : CallClosureTaskBase<void()>(closure, isSameThread)
    {
    }
};

} // namespace internal

// Create tasks passed within a single thread.
// When posting tasks within a thread, use |createSameThreadTask| instead
// of using |bind| directly to state explicitly that there is no need to care
// about thread safety when posting the task.
// When posting tasks across threads, use |createCrossThreadTask|.
template<typename FunctionType, typename... P>
PassOwnPtr<ExecutionContextTask> createSameThreadTask(
    FunctionType function, const P&... parameters)
{
    return internal::CallClosureTask::create(bind(function, parameters...), true);
}

} // namespace

#endif
