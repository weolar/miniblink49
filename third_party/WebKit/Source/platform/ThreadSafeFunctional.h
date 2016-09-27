// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ThreadSafeFunctional_h
#define ThreadSafeFunctional_h

#include "platform/CrossThreadCopier.h"
#include "wtf/Functional.h"

namespace blink {

// threadSafeBind() is bind() for cross-thread task posting.
// threadSafeBind() applies CrossThreadCopier to the arguments.
//
// Example:
//     void func1(int, const String&);
//     f = threadSafeBind(func1, 42, str);
// func1(42, str2) will be called when |f()| is executed,
// where |str2| is a deep copy of |str| (created by str.isolatedCopy()).
//
// threadSafeBind(str) is similar to bind(str.isolatedCopy()), but the latter
// is NOT thread-safe due to temporary objects (https://crbug.com/390851).
//
// Don't (if you pass the task across threads):
//     bind(func1, 42, str);
//     bind(func1, 42, str.isolatedCopy());

template<typename... FreeVariableTypes, typename FunctionType>
PassOwnPtr<Function<typename WTF::FunctionWrapper<FunctionType>::ResultType(FreeVariableTypes...)>> threadSafeBind(
    FunctionType function)
{
    return bind<FreeVariableTypes...>(function);
}

template<typename... FreeVariableTypes, typename FunctionType, typename P1>
PassOwnPtr<Function<typename WTF::FunctionWrapper<FunctionType>::ResultType(FreeVariableTypes...)>> threadSafeBind(
    FunctionType function,
    const P1& parameter1)
{
    return bind<FreeVariableTypes...>(function,
        CrossThreadCopier<P1>::copy(parameter1));
}

template<typename... FreeVariableTypes, typename FunctionType, typename P1, typename P2>
PassOwnPtr<Function<typename WTF::FunctionWrapper<FunctionType>::ResultType(FreeVariableTypes...)>> threadSafeBind(
    FunctionType function,
    const P1& parameter1, const P2& parameter2)
{
    return bind<FreeVariableTypes...>(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2));
}

template<typename... FreeVariableTypes, typename FunctionType, typename P1, typename P2, typename P3>
PassOwnPtr<Function<typename WTF::FunctionWrapper<FunctionType>::ResultType(FreeVariableTypes...)>> threadSafeBind(
    FunctionType function,
    const P1& parameter1, const P2& parameter2, const P3& parameter3)
{
    return bind<FreeVariableTypes...>(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3));
}

template<typename... FreeVariableTypes, typename FunctionType, typename P1, typename P2, typename P3, typename P4>
PassOwnPtr<Function<typename WTF::FunctionWrapper<FunctionType>::ResultType(FreeVariableTypes...)>> threadSafeBind(
    FunctionType function,
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4)
{
    return bind<FreeVariableTypes...>(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4));
}

template<typename... FreeVariableTypes, typename FunctionType, typename P1, typename P2, typename P3, typename P4, typename P5>
PassOwnPtr<Function<typename WTF::FunctionWrapper<FunctionType>::ResultType(FreeVariableTypes...)>> threadSafeBind(
    FunctionType function,
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5)
{
    return bind<FreeVariableTypes...>(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4),
        CrossThreadCopier<P5>::copy(parameter5));
}

template<typename... FreeVariableTypes, typename FunctionType, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
PassOwnPtr<Function<typename WTF::FunctionWrapper<FunctionType>::ResultType(FreeVariableTypes...)>> threadSafeBind(
    FunctionType function,
    const P1& parameter1, const P2& parameter2, const P3& parameter3, const P4& parameter4, const P5& parameter5, const P6& parameter6)
{
    return bind<FreeVariableTypes...>(function,
        CrossThreadCopier<P1>::copy(parameter1),
        CrossThreadCopier<P2>::copy(parameter2),
        CrossThreadCopier<P3>::copy(parameter3),
        CrossThreadCopier<P4>::copy(parameter4),
        CrossThreadCopier<P5>::copy(parameter5),
        CrossThreadCopier<P6>::copy(parameter6));
}

} // namespace blink

#endif // ThreadSafeFunctional_h
