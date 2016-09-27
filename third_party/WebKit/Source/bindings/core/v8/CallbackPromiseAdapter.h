/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef CallbackPromiseAdapter_h
#define CallbackPromiseAdapter_h

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "public/platform/WebCallbacks.h"

namespace blink {

// This class provides an easy way to convert from a Script-exposed
// class (i.e. a class that has a toV8() overload) that uses Promises
// to a WebKit API class that uses WebCallbacks. You can define
// separate Success and Error classes, but this example just uses one
// object for both.
//
// To use:
//
// class MyClass ... {
//    typedef blink::WebMyClass WebType;
//
//    // Takes ownership of |webInstance|.
//    static PassRefPtr<MyClass> take(ScriptPromiseResolver* resolver,
//                                    PassOwnPtr<blink::WebMyClass> webInstance) {
//        // convert/create as appropriate, but often it's just:
//        return MyClass::create(webInstance);
//
//        // Since promise resolving is done as an async task, it's not
//        // guaranteed that the script context has seen the promise resolve
//        // immediately after calling onSuccess/onError. You can use the
//        // ScriptPromise from the resolver to schedule a task that executes
//        // after resolving:
//        ScriptState::Scope scope(resolver->scriptState());
//        resolver->promise().then(...);
//    }
//
// Now when calling into a WebKit API that requires a WebCallbacks<blink::WebMyClass, blink::WebMyClass>*:
//
//    // call signature: callSomeMethod(WebCallbacks<MyClass, MyClass>* callbacks)
//    webObject->callSomeMethod(new CallbackPromiseAdapter<MyClass, MyClass>(resolver, scriptExecutionContext));
//
// Note:
// - This class does not manage its own lifetime. In this example that ownership
//   of the WebCallbacks instance is being passed in and it is up to the callee
//   to free the WebCallbacks instance.
// - onSuccess and onError take ownership of the given WebType instance.
template<typename S, typename T>
class CallbackPromiseAdapter final : public WebCallbacks<typename S::WebType, typename T::WebType> {
    WTF_MAKE_NONCOPYABLE(CallbackPromiseAdapter);
public:
    explicit CallbackPromiseAdapter(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver)
    {
        ASSERT(m_resolver);
    }
    ~CallbackPromiseAdapter() override { }

    // Takes ownership of |result|.
    void onSuccess(typename S::WebType* result) override
    {
        OwnPtr<typename S::WebType> ownPtr = adoptPtr(result);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        if (!result) {
            m_resolver->resolve(v8::Null(m_resolver->scriptState()->isolate()));
            return;
        }
        m_resolver->resolve(S::take(m_resolver.get(), ownPtr.release()));
    }

    // Takes ownership of |error|.
    void onError(typename T::WebType* error) override
    {
        OwnPtr<typename T::WebType> ownPtr = adoptPtr(error);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->reject(T::take(m_resolver.get(), ownPtr.release()));
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

template<typename T>
class CallbackPromiseAdapter<void, T> final : public WebCallbacks<void, typename T::WebType> {
    WTF_MAKE_NONCOPYABLE(CallbackPromiseAdapter);
public:
    explicit CallbackPromiseAdapter(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver)
    {
        ASSERT(m_resolver);
    }
    ~CallbackPromiseAdapter() override { }

    void onSuccess() override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->resolve();
    }

    // Takes ownership of |error|.
    void onError(typename T::WebType* error) override
    {
        OwnPtr<typename T::WebType> ownPtr = adoptPtr(error);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->reject(T::take(m_resolver.get(), ownPtr.release()));
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

template<typename S>
class CallbackPromiseAdapter<S, void> final : public WebCallbacks<typename S::WebType, void> {
    WTF_MAKE_NONCOPYABLE(CallbackPromiseAdapter);
public:
    explicit CallbackPromiseAdapter(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver)
    {
        ASSERT(m_resolver);
    }
    ~CallbackPromiseAdapter() override { }

    // Takes ownership of |result|.
    void onSuccess(typename S::WebType* result) override
    {
        OwnPtr<typename S::WebType> ownPtr = adoptPtr(result);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->resolve(S::take(m_resolver.get(), ownPtr.release()));
    }

    void onError() override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->reject();
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

template<typename T>
class CallbackPromiseAdapter<bool, T> final : public WebCallbacks<bool, typename T::WebType> {
    WTF_MAKE_NONCOPYABLE(CallbackPromiseAdapter);
public:
    explicit CallbackPromiseAdapter(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver)
    {
        ASSERT(m_resolver);
    }
    ~CallbackPromiseAdapter() override { }

    // TODO(nhiroki): onSuccess should take ownership of a bool object for
    // consistency. (http://crbug.com/493531)
    void onSuccess(bool* result) override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->resolve(*result);
    }

    // Takes ownership of |error|.
    void onError(typename T::WebType* error) override
    {
        OwnPtr<typename T::WebType> ownPtr = adoptPtr(error);
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->reject(T::take(m_resolver.get(), ownPtr.release()));
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

template<>
class CallbackPromiseAdapter<void, void> final : public WebCallbacks<void, void> {
    WTF_MAKE_NONCOPYABLE(CallbackPromiseAdapter);
public:
    explicit CallbackPromiseAdapter(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver)
    {
        ASSERT(m_resolver);
    }
    ~CallbackPromiseAdapter() override { }

    void onSuccess() override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->resolve();
    }

    void onError() override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->reject();
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

template<>
class CallbackPromiseAdapter<bool, void> final : public WebCallbacks<bool, void> {
    WTF_MAKE_NONCOPYABLE(CallbackPromiseAdapter);
public:
    explicit CallbackPromiseAdapter(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
        : m_resolver(resolver)
    {
        ASSERT(m_resolver);
    }
    ~CallbackPromiseAdapter() override { }

    // TODO(nhiroki): onSuccess should take ownership of a bool object for
    // consistency. (http://crbug.com/493531)
    void onSuccess(bool* result) override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->resolve(*result);
    }

    void onError() override
    {
        if (!m_resolver->executionContext() || m_resolver->executionContext()->activeDOMObjectsAreStopped())
            return;
        m_resolver->reject();
    }

private:
    RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

} // namespace blink

#endif
