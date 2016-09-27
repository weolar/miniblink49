/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WTF_Functional_h
#define WTF_Functional_h

#include "wtf/Assertions.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/ThreadSafeRefCounted.h"
#include "wtf/WeakPtr.h"

namespace WTF {

// Functional.h provides a very simple way to bind a function pointer and arguments together into a function object
// that can be stored, copied and invoked, similar to how boost::bind and std::bind in C++11.

// Use threadSafeBind() or createCrossThreadTask() if the function/task is
// called on a (potentially) different thread from the current thread.

// A FunctionWrapper is a class template that can wrap a function pointer or a member function pointer and
// provide a unified interface for calling that function.
template<typename>
class FunctionWrapper;

// Bound static functions:
template<typename R, typename... Params>
class FunctionWrapper<R(*)(Params...)> {
public:
    typedef R ResultType;

    explicit FunctionWrapper(R(*function)(Params...))
        : m_function(function)
    {
    }

    R operator()(Params... params)
    {
        return m_function(params...);
    }

private:
    R(*m_function)(Params...);
};

// Bound member functions:

template<typename R, typename C, typename... Params>
class FunctionWrapper<R(C::*)(Params...)> {
public:
    typedef R ResultType;

    explicit FunctionWrapper(R(C::*function)(Params...))
        : m_function(function)
    {
    }

    R operator()(C* c, Params... params)
    {
        return (c->*m_function)(params...);
    }

    R operator()(PassOwnPtr<C> c, Params... params)
    {
        return (c.get()->*m_function)(params...);
    }

    R operator()(const WeakPtr<C>& c, Params... params)
    {
        C* obj = c.get();
        if (!obj)
            return R();
        return (obj->*m_function)(params...);
    }

private:
    R(C::*m_function)(Params...);
};

template<typename T> struct ParamStorageTraits {
    typedef T StorageType;

    static StorageType wrap(const T& value) { return value; }
    static const T& unwrap(const StorageType& value) { return value; }
};

template<typename T> struct ParamStorageTraits<PassRefPtr<T>> {
    typedef RefPtr<T> StorageType;

    static StorageType wrap(PassRefPtr<T> value) { return value; }
    static T* unwrap(const StorageType& value) { return value.get(); }
};

template<typename T> struct ParamStorageTraits<RefPtr<T>> {
    typedef RefPtr<T> StorageType;

    static StorageType wrap(RefPtr<T> value) { return value.release(); }
    static T* unwrap(const StorageType& value) { return value.get(); }
};

template<typename> class RetainPtr;

template<typename T> struct ParamStorageTraits<RetainPtr<T>> {
    typedef RetainPtr<T> StorageType;

    static StorageType wrap(const RetainPtr<T>& value) { return value; }
    static typename RetainPtr<T>::PtrType unwrap(const StorageType& value) { return value.get(); }
};

template<typename>
class Function;

template<typename R, typename... Args>
class Function<R(Args...)> {
    WTF_MAKE_NONCOPYABLE(Function);
public:
    virtual ~Function() { }
    virtual R operator()(Args... args) = 0;
protected:
    Function() = default;
};

template<int boundArgsCount, typename FunctionWrapper, typename FunctionType>
class PartBoundFunctionImpl;

// Specialization for unbound functions.
template<typename FunctionWrapper, typename R, typename... UnboundParams>
class PartBoundFunctionImpl<0, FunctionWrapper, R(UnboundParams...)> final : public Function<typename FunctionWrapper::ResultType(UnboundParams...)> {
public:
    PartBoundFunctionImpl(FunctionWrapper functionWrapper)
        : m_functionWrapper(functionWrapper)
    {
    }

    typename FunctionWrapper::ResultType operator()(UnboundParams... params) override
    {
        return m_functionWrapper(params...);
    }

private:
    FunctionWrapper m_functionWrapper;
};

template<typename FunctionWrapper, typename R, typename P1, typename... UnboundParams>
class PartBoundFunctionImpl<1, FunctionWrapper, R(P1, UnboundParams...)> final : public Function<typename FunctionWrapper::ResultType(UnboundParams...)> {
public:
    PartBoundFunctionImpl(FunctionWrapper functionWrapper, const P1& p1)
        : m_functionWrapper(functionWrapper)
        , m_p1(ParamStorageTraits<P1>::wrap(p1))
    {
    }

    typename FunctionWrapper::ResultType operator()(UnboundParams... params) override
    {
        return m_functionWrapper(ParamStorageTraits<P1>::unwrap(m_p1), params...);
    }

private:
    FunctionWrapper m_functionWrapper;
    typename ParamStorageTraits<P1>::StorageType m_p1;
};

template<typename FunctionWrapper, typename R, typename P1, typename P2, typename... UnboundParams>
class PartBoundFunctionImpl<2, FunctionWrapper, R(P1, P2, UnboundParams...)> final : public Function<typename FunctionWrapper::ResultType(UnboundParams...)> {
public:
    PartBoundFunctionImpl(FunctionWrapper functionWrapper, const P1& p1, const P2& p2)
        : m_functionWrapper(functionWrapper)
        , m_p1(ParamStorageTraits<P1>::wrap(p1))
        , m_p2(ParamStorageTraits<P2>::wrap(p2))
    {
    }

    typename FunctionWrapper::ResultType operator()(UnboundParams... params) override
    {
        return m_functionWrapper(ParamStorageTraits<P1>::unwrap(m_p1), ParamStorageTraits<P2>::unwrap(m_p2), params...);
    }

private:
    FunctionWrapper m_functionWrapper;
    typename ParamStorageTraits<P1>::StorageType m_p1;
    typename ParamStorageTraits<P2>::StorageType m_p2;
};

template<typename FunctionWrapper, typename R, typename P1, typename P2, typename P3, typename... UnboundParams>
class PartBoundFunctionImpl<3, FunctionWrapper, R(P1, P2, P3, UnboundParams...)> final : public Function<typename FunctionWrapper::ResultType(UnboundParams...)> {
public:
    PartBoundFunctionImpl(FunctionWrapper functionWrapper, const P1& p1, const P2& p2, const P3& p3)
        : m_functionWrapper(functionWrapper)
        , m_p1(ParamStorageTraits<P1>::wrap(p1))
        , m_p2(ParamStorageTraits<P2>::wrap(p2))
        , m_p3(ParamStorageTraits<P3>::wrap(p3))
    {
    }

    typename FunctionWrapper::ResultType operator()(UnboundParams... params) override
    {
        return m_functionWrapper(ParamStorageTraits<P1>::unwrap(m_p1), ParamStorageTraits<P2>::unwrap(m_p2), ParamStorageTraits<P3>::unwrap(m_p3), params...);
    }

private:
    FunctionWrapper m_functionWrapper;
    typename ParamStorageTraits<P1>::StorageType m_p1;
    typename ParamStorageTraits<P2>::StorageType m_p2;
    typename ParamStorageTraits<P3>::StorageType m_p3;
};

template<typename FunctionWrapper, typename R, typename P1, typename P2, typename P3, typename P4, typename... UnboundParams>
class PartBoundFunctionImpl<4, FunctionWrapper, R(P1, P2, P3, P4, UnboundParams...)> final : public Function<typename FunctionWrapper::ResultType(UnboundParams...)> {
public:
    PartBoundFunctionImpl(FunctionWrapper functionWrapper, const P1& p1, const P2& p2, const P3& p3, const P4& p4)
        : m_functionWrapper(functionWrapper)
        , m_p1(ParamStorageTraits<P1>::wrap(p1))
        , m_p2(ParamStorageTraits<P2>::wrap(p2))
        , m_p3(ParamStorageTraits<P3>::wrap(p3))
        , m_p4(ParamStorageTraits<P4>::wrap(p4))
    {
    }

    typename FunctionWrapper::ResultType operator()(UnboundParams... params) override
    {
        return m_functionWrapper(ParamStorageTraits<P1>::unwrap(m_p1), ParamStorageTraits<P2>::unwrap(m_p2), ParamStorageTraits<P3>::unwrap(m_p3), ParamStorageTraits<P4>::unwrap(m_p4), params...);
    }

private:
    FunctionWrapper m_functionWrapper;
    typename ParamStorageTraits<P1>::StorageType m_p1;
    typename ParamStorageTraits<P2>::StorageType m_p2;
    typename ParamStorageTraits<P3>::StorageType m_p3;
    typename ParamStorageTraits<P4>::StorageType m_p4;
};

template<typename FunctionWrapper, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename... UnboundParams>
class PartBoundFunctionImpl<5, FunctionWrapper, R(P1, P2, P3, P4, P5, UnboundParams...)> final : public Function<typename FunctionWrapper::ResultType(UnboundParams...)> {
public:
    PartBoundFunctionImpl(FunctionWrapper functionWrapper, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5)
        : m_functionWrapper(functionWrapper)
        , m_p1(ParamStorageTraits<P1>::wrap(p1))
        , m_p2(ParamStorageTraits<P2>::wrap(p2))
        , m_p3(ParamStorageTraits<P3>::wrap(p3))
        , m_p4(ParamStorageTraits<P4>::wrap(p4))
        , m_p5(ParamStorageTraits<P5>::wrap(p5))
    {
    }

    typename FunctionWrapper::ResultType operator()(UnboundParams... params) override
    {
        return m_functionWrapper(ParamStorageTraits<P1>::unwrap(m_p1), ParamStorageTraits<P2>::unwrap(m_p2), ParamStorageTraits<P3>::unwrap(m_p3), ParamStorageTraits<P4>::unwrap(m_p4), ParamStorageTraits<P5>::unwrap(m_p5), params...);
    }

private:
    FunctionWrapper m_functionWrapper;
    typename ParamStorageTraits<P1>::StorageType m_p1;
    typename ParamStorageTraits<P2>::StorageType m_p2;
    typename ParamStorageTraits<P3>::StorageType m_p3;
    typename ParamStorageTraits<P4>::StorageType m_p4;
    typename ParamStorageTraits<P5>::StorageType m_p5;
};

template<typename FunctionWrapper, typename R, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename... UnboundParams>
class PartBoundFunctionImpl<6, FunctionWrapper, R(P1, P2, P3, P4, P5, P6, UnboundParams...)> final : public Function<typename FunctionWrapper::ResultType(UnboundParams...)> {
public:
    PartBoundFunctionImpl(FunctionWrapper functionWrapper, const P1& p1, const P2& p2, const P3& p3, const P4& p4, const P5& p5, const P6& p6)
        : m_functionWrapper(functionWrapper)
        , m_p1(ParamStorageTraits<P1>::wrap(p1))
        , m_p2(ParamStorageTraits<P2>::wrap(p2))
        , m_p3(ParamStorageTraits<P3>::wrap(p3))
        , m_p4(ParamStorageTraits<P4>::wrap(p4))
        , m_p5(ParamStorageTraits<P5>::wrap(p5))
        , m_p6(ParamStorageTraits<P6>::wrap(p6))
    {
    }

    typename FunctionWrapper::ResultType operator()(UnboundParams... params) override
    {
        return m_functionWrapper(ParamStorageTraits<P1>::unwrap(m_p1), ParamStorageTraits<P2>::unwrap(m_p2), ParamStorageTraits<P3>::unwrap(m_p3), ParamStorageTraits<P4>::unwrap(m_p4), ParamStorageTraits<P5>::unwrap(m_p5), ParamStorageTraits<P6>::unwrap(m_p6), params...);
    }

private:
    FunctionWrapper m_functionWrapper;
    typename ParamStorageTraits<P1>::StorageType m_p1;
    typename ParamStorageTraits<P2>::StorageType m_p2;
    typename ParamStorageTraits<P3>::StorageType m_p3;
    typename ParamStorageTraits<P4>::StorageType m_p4;
    typename ParamStorageTraits<P5>::StorageType m_p5;
    typename ParamStorageTraits<P6>::StorageType m_p6;
};

template<typename... UnboundArgs, typename FunctionType, typename... BoundArgs>
PassOwnPtr<Function<typename FunctionWrapper<FunctionType>::ResultType(UnboundArgs...)>> bind(FunctionType function, const BoundArgs&... boundArgs)
{
    const int boundArgsCount = sizeof...(BoundArgs);
    using BoundFunctionType = PartBoundFunctionImpl<boundArgsCount, FunctionWrapper<FunctionType>, typename FunctionWrapper<FunctionType>::ResultType(BoundArgs..., UnboundArgs...)>;
    return adoptPtr(new BoundFunctionType(FunctionWrapper<FunctionType>(function), boundArgs...));
}

typedef Function<void()> Closure;

}

using WTF::Function;
using WTF::bind;
using WTF::Closure;

#endif // WTF_Functional_h
