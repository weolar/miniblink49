/*
 * Copyright (C) 2009-2010 Google Inc. All rights reserved.
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

#ifndef CrossThreadTask_h
#define CrossThreadTask_h

#include "core/dom/ExecutionContext.h"
#include "core/dom/ExecutionContextTask.h"
#include "platform/ThreadSafeFunctional.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/TypeTraits.h"

namespace blink {

// createCrossThreadTask(...) is similar to but safer than
// CallClosureTask::create(bind(...)) for cross-thread task posting.
// postTask(CallClosureTask::create(bind(...))) is not thread-safe
// due to temporary objects, see http://crbug.com/390851 for details.
//
// Example:
//     void func1(int, const String&);
//     createCrossThreadTask(func1, 42, str);
// func1(42, str2) will be called, where |str2| is a deep copy of
// |str| (created by str.isolatedCopy()).
//
// Don't (if you pass the task across threads):
//     bind(func1, 42, str);
//     bind(func1, 42, str.isolatedCopy());
//
// Usage:
//     For functions:
//         void functionEC(MP1, ..., MPn, ExecutionContext*);
//         void function(MP1, ..., MPn);
//         class C {
//             void memberEC(MP1, ..., MPn, ExecutionContext*);
//             void member(MP1, ..., MPn);
//         };
//     We create tasks represented by PassOwnPtr<ExecutionContextTask>:
//         [1] createCrossThreadTask(functionEC, const P1& p1, ..., const Pn& pn);
//         [2] createCrossThreadTask(memberEC, C* ptr, const P1& p1, ..., const Pn& pn);
//         [3] createCrossThreadTask(function, const P1& p1, ..., const Pn& pn);
//         [4] createCrossThreadTask(member, C* ptr, const P1& p1, ..., const Pn& pn);
//         [5] createCrossThreadTask(member, const WeakPtr<C>& ptr, const P1& p1, ..., const Pn& pn);
//         [6] createCrossThreadTask(member, RawPtr<C> p0, const P1& p1, ..., const Pn& pn);
//     and then the following are called on the target thread:
//         [1]   functionEC(p1, ..., pn, context);
//         [2]   ptr->memberEC(p1, ..., pn, context);
//         [3]   function(p1, ..., pn);
//         [4,5] ptr->member(p1, ..., pn);
//         [6]   p0->member(p1, ..., pn);
//
// ExecutionContext:
//     |context| is supplied by the target thread.
//
// Deep copies by threadSafeBind():
//     |p0|, |p1|, ..., |pn| are processed by threadSafeBind() and thus
//     CrossThreadCopier.
//     You don't have to call manually e.g. isolatedCopy().
//     To pass things that cannot be copied by CrossThreadCopier
//     (e.g. pointers), use AllowCrossThreadAccess() explicitly.
//     |ptr| is assumed safe to be passed across threads, and
//     AllowCrossThreadAccess() is applied automatically.

namespace internal {

class CallClosureWithExecutionContextTask final : public CallClosureTaskBase<void(ExecutionContext*)> {
public:
    // Do not use |create| other than in createCrossThreadTask and
    // createSameThreadTask.
    // See http://crbug.com/390851
    static PassOwnPtr<CallClosureWithExecutionContextTask> create(PassOwnPtr<Function<void(ExecutionContext*)>> closure, bool isSameThread = false)
    {
        return adoptPtr(new CallClosureWithExecutionContextTask(closure, isSameThread));
    }

    void performTask(ExecutionContext* context) override
    {
        checkThread();
        (*m_closure)(context);
    }

private:
    CallClosureWithExecutionContextTask(PassOwnPtr<Function<void(ExecutionContext*)>> closure, bool isSameThread)
        : CallClosureTaskBase<void(ExecutionContext*)>(closure, isSameThread)
    {
    }
};

} // namespace internal

// RETTYPE, PS, and MPS are added as template parameters to circumvent MSVC 18.00.21005.1 (VS 2013) issues.

// [1] createCrossThreadTask() for non-member functions (with ExecutionContext* argument).
// (P = <P1, ..., Pn>, MP = <MP1, ..., MPn, ExecutionContext*>)
template<typename... P, typename... MP,
    typename RETTYPE = PassOwnPtr<ExecutionContextTask>, size_t PS = sizeof...(P), size_t MPS = sizeof...(MP)>
typename WTF::EnableIf<PS + 1 == MPS, RETTYPE>::Type createCrossThreadTask(void (*function)(MP...), const P&... parameters)
{
    return internal::CallClosureWithExecutionContextTask::create(threadSafeBind<ExecutionContext*>(function, parameters...));
}

// [2] createCrossThreadTask() for member functions of class C (with ExecutionContext* argument) + raw pointer (C*).
// (P = <P1, ..., Pn>, MP = <MP1, ..., MPn, ExecutionContext*>)
template<typename C, typename... P, typename... MP,
    typename RETTYPE = PassOwnPtr<ExecutionContextTask>, size_t PS = sizeof...(P), size_t MPS = sizeof...(MP)>
typename WTF::EnableIf<PS + 1 == MPS, RETTYPE>::Type createCrossThreadTask(void (C::*function)(MP...), C* p, const P&... parameters)
{
    return internal::CallClosureWithExecutionContextTask::create(threadSafeBind<ExecutionContext*>(function, AllowCrossThreadAccess(p), parameters...));
}

// [3] createCrossThreadTask() for non-member functions
// (P = <P1, ..., Pn>, MP = <MP1, ..., MPn>)
template<typename... P, typename... MP,
    typename RETTYPE = PassOwnPtr<ExecutionContextTask>, size_t PS = sizeof...(P), size_t MPS = sizeof...(MP)>
typename WTF::EnableIf<PS == MPS, RETTYPE>::Type createCrossThreadTask(void (*function)(MP...), const P&... parameters)
{
    return internal::CallClosureTask::create(threadSafeBind(function, parameters...));
}

// [4] createCrossThreadTask() for member functions of class C + raw pointer (C*)
// [5] createCrossThreadTask() for member functions of class C + weak pointer (const WeakPtr<C>&)
// (P = <P1, ..., Pn>, MP = <MP1, ..., MPn>)
template<typename C, typename... P, typename... MP,
    typename RETTYPE = PassOwnPtr<ExecutionContextTask>, size_t PS = sizeof...(P), size_t MPS = sizeof...(MP)>
typename WTF::EnableIf<PS == MPS, RETTYPE>::Type createCrossThreadTask(void (C::*function)(MP...), C* p, const P&... parameters)
{
    return internal::CallClosureTask::create(threadSafeBind(function, AllowCrossThreadAccess(p), parameters...));
}

template<typename C, typename... P, typename... MP,
    typename RETTYPE = PassOwnPtr<ExecutionContextTask>, size_t PS = sizeof...(P), size_t MPS = sizeof...(MP)>
typename WTF::EnableIf<PS == MPS, RETTYPE>::Type createCrossThreadTask(void (C::*function)(MP...), const WeakPtr<C>& p, const P&... parameters)
{
    return internal::CallClosureTask::create(threadSafeBind(function, AllowCrossThreadAccess(p), parameters...));
}

// [6] createCrossThreadTask() for member functions + pointers to class C other than C* or const WeakPtr<C>&
// (P = <P0, P1, ..., Pn>, MP = <MP1, ..., MPn>)
template<typename C, typename... P, typename... MP,
    typename RETTYPE = PassOwnPtr<ExecutionContextTask>, size_t PS = sizeof...(P), size_t MPS = sizeof...(MP)>
typename WTF::EnableIf<PS == MPS + 1, RETTYPE>::Type createCrossThreadTask(void (C::*function)(MP...), const P&... parameters)
{
    return internal::CallClosureTask::create(threadSafeBind(function, parameters...));
}

} // namespace blink

#endif // CrossThreadTask_h
