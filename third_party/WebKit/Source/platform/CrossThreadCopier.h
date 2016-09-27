/*
 * Copyright (C) 2009, 2010 Google Inc. All rights reserved.
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

#ifndef CrossThreadCopier_h
#define CrossThreadCopier_h

#include "platform/PlatformExport.h"
#include "platform/heap/Handle.h"
#include "wtf/Assertions.h"
#include "wtf/Forward.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RawPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/ThreadSafeRefCounted.h"
#include "wtf/TypeTraits.h"

namespace blink {

    class IntRect;
    class IntSize;
    class KURL;
    class ResourceError;
    class ResourceRequest;
    class ResourceResponse;
    struct CrossThreadResourceResponseData;
    struct CrossThreadResourceRequestData;

    template<typename T> struct CrossThreadCopierPassThrough {
        typedef T Type;
        static Type copy(const T& parameter)
        {
            return parameter;
        }
    };

    template<bool isConvertibleToInteger, bool isThreadSafeRefCounted, bool isGarbageCollected, typename T> struct CrossThreadCopierBase;

    // Integers get passed through without any changes.
    template<typename T> struct CrossThreadCopierBase<true, false, false, T> : public CrossThreadCopierPassThrough<T> {
    };

    // To allow a type to be passed across threads using its copy constructor, add a forward declaration of the type and
    // a CopyThreadCopierBase<false, false, TypeName> : public CrossThreadCopierPassThrough<TypeName> { }; to this file.
    template<> struct CrossThreadCopierBase<false, false, false, IntRect> : public CrossThreadCopierPassThrough<IntRect> {
    };

    template<> struct CrossThreadCopierBase<false, false, false, IntSize> : public CrossThreadCopierPassThrough<IntSize> {
    };

    // Custom copy methods.
    template<typename T> struct CrossThreadCopierBase<false, true, false, T> {
        typedef typename WTF::RemoveTemplate<T, RefPtr>::Type TypeWithoutRefPtr;
        typedef typename WTF::RemoveTemplate<TypeWithoutRefPtr, PassRefPtr>::Type TypeWithoutPassRefPtr;
        typedef typename WTF::RemovePointer<TypeWithoutPassRefPtr>::Type RefCountedType;

        // Verify that only one of the above did a change.
        static_assert((WTF::IsSameType<RefPtr<RefCountedType>, T>::value
                        || WTF::IsSameType<PassRefPtr<RefCountedType>, T>::value
                        || WTF::IsSameType<RefCountedType*, T>::value),
                        "only one type modification should be allowed");

        typedef PassRefPtr<RefCountedType> Type;
        static Type copy(const T& refPtr)
        {
            return refPtr;
        }
    };

    template<typename T> struct CrossThreadCopierBase<false, false, false, PassOwnPtr<T>> {
        typedef PassOwnPtr<T> Type;
        static Type copy(Type ownPtr)
        {
            return ownPtr;
        }
    };

    template<typename T> struct CrossThreadCopierBase<false, false, false, WeakMember<T>*> {
        typedef WeakMember<T>* Type;
        static Type copy(Type ptr)
        {
            return ptr;
        }
    };

    template<> struct CrossThreadCopierBase<false, false, false, KURL> {
        typedef KURL Type;
        PLATFORM_EXPORT static Type copy(const KURL&);
    };

    template<> struct CrossThreadCopierBase<false, false, false, String> {
        typedef String Type;
        PLATFORM_EXPORT static Type copy(const String&);
    };

    template<> struct CrossThreadCopierBase<false, false, false, ResourceError> {
        typedef ResourceError Type;
        PLATFORM_EXPORT static Type copy(const ResourceError&);
    };

    template<> struct CrossThreadCopierBase<false, false, false, ResourceRequest> {
        typedef PassOwnPtr<CrossThreadResourceRequestData> Type;
        PLATFORM_EXPORT static Type copy(const ResourceRequest&);
    };

    template<> struct CrossThreadCopierBase<false, false, false, ResourceResponse> {
        typedef PassOwnPtr<CrossThreadResourceResponseData> Type;
        PLATFORM_EXPORT static Type copy(const ResourceResponse&);
    };

    template<typename T> struct CrossThreadCopierBase<false, false, true, T> {
        typedef typename WTF::RemovePointer<T>::Type TypeWithoutPointer;
        typedef RawPtr<TypeWithoutPointer> Type;
        static Type copy(const T& ptr)
        {
            return ptr;
        }
    };

    template<typename T> struct CrossThreadCopierBase<false, false, true, RawPtr<T>> {
        typedef RawPtr<T> Type;
        static Type copy(const Type& ptr)
        {
            return ptr;
        }
    };

    template<typename T> struct CrossThreadCopierBase<false, false, true, Member<T>> {
        typedef RawPtr<T> Type;
        static Type copy(const Member<T>& ptr)
        {
            return ptr;
        }
    };

    template<typename T> struct CrossThreadCopierBase<false, false, true, WeakMember<T>> {
        typedef RawPtr<T> Type;
        static Type copy(const WeakMember<T>& ptr)
        {
            return ptr;
        }
    };

    template<typename T> struct CrossThreadCopier : public CrossThreadCopierBase<WTF::IsConvertibleToInteger<T>::value,
        WTF::IsSubclassOfTemplate<typename WTF::RemoveTemplate<T, RefPtr>::Type, ThreadSafeRefCounted>::value
            || WTF::IsSubclassOfTemplate<typename WTF::RemovePointer<T>::Type, ThreadSafeRefCounted>::value
            || WTF::IsSubclassOfTemplate<typename WTF::RemoveTemplate<T, PassRefPtr>::Type, ThreadSafeRefCounted>::value,
        WTF::IsSubclassOfTemplate<typename WTF::RemovePointer<T>::Type, GarbageCollected>::value
            || WTF::IsSubclassOfTemplate<typename WTF::RemoveTemplate<T, RawPtr>::Type, GarbageCollected>::value
            || WTF::IsSubclassOfTemplate<typename WTF::RemoveTemplate<T, Member>::Type, GarbageCollected>::value
            || WTF::IsSubclassOfTemplate<typename WTF::RemoveTemplate<T, WeakMember>::Type, GarbageCollected>::value,
        T> {
    };

    // |T| is |C*| or |const WeakPtr<C>&|.
    template<typename T> struct AllowCrossThreadAccessWrapper {
        STACK_ALLOCATED();
    public:
        explicit AllowCrossThreadAccessWrapper(T value) : m_value(value) { }
        T value() const { return m_value; }
    private:
        // This raw pointer is safe since AllowCrossThreadAccessWrapper is
        // always stack-allocated. Ideally this should be Member<T> if T is
        // garbage-collected and T* otherwise, but we don't want to introduce
        // another template magic just for distinguishing Member<T> from T*.
        // From the perspective of GC, T* always works correctly.
        GC_PLUGIN_IGNORE("")
        T m_value;
    };

    template<typename T> struct CrossThreadCopierBase<false, false, false, AllowCrossThreadAccessWrapper<T>> {
        typedef T Type;
        static Type copy(const AllowCrossThreadAccessWrapper<T>& wrapper) { return wrapper.value(); }
    };

    template<typename T> AllowCrossThreadAccessWrapper<T*> AllowCrossThreadAccess(T* value)
    {
        return AllowCrossThreadAccessWrapper<T*>(value);
    }

    template<typename T> AllowCrossThreadAccessWrapper<const WeakPtr<T>&> AllowCrossThreadAccess(const WeakPtr<T>& value)
    {
        return AllowCrossThreadAccessWrapper<const WeakPtr<T>&>(value);
    }

    // FIXME: Move to a different header file. AllowAccessLater is for cross-thread access
    // that is not cross-thread (tasks posted to a queue guaranteed to run on the same thread).
    template<typename T> struct AllowAccessLaterWrapper {
    public:
        explicit AllowAccessLaterWrapper(T* value) : m_value(value) { }
        T* value() const { return m_value; }
    private:
        // This raw pointer is safe since AllowAccessLaterWrapper is
        // always stack-allocated. Ideally this should be Member<T> if T is
        // garbage-collected and T* otherwise, but we don't want to introduce
        // another template magic just for distinguishing Member<T> from T*.
        // From the perspective of GC, T* always works correctly.
        GC_PLUGIN_IGNORE("")
        T* m_value;
    };

    template<typename T> struct CrossThreadCopierBase<false, false, false, AllowAccessLaterWrapper<T>> {
        typedef T* Type;
        static Type copy(const AllowAccessLaterWrapper<T>& wrapper) { return wrapper.value(); }
    };

    template<typename T> AllowAccessLaterWrapper<T> AllowAccessLater(T* value)
    {
        return AllowAccessLaterWrapper<T>(value);
    }


} // namespace blink

#endif // CrossThreadCopier_h
