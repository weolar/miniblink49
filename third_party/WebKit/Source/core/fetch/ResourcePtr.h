/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ResourcePtr_h
#define ResourcePtr_h

#include "core/CoreExport.h"
#include "core/fetch/Resource.h"

namespace blink {

class CORE_EXPORT ResourcePtrBase {
public:
    Resource* get() const { return m_resource; }
    bool operator!() const { return !m_resource; }
    void clear() { setResource(nullptr); }

    // This conversion operator allows implicit conversion to bool but not to other integer types.
    typedef Resource* ResourcePtrBase::*UnspecifiedBoolType;
    operator UnspecifiedBoolType() const { return m_resource ? &ResourcePtrBase::m_resource : nullptr; }

protected:
    ResourcePtrBase() : m_resource(nullptr) { }
    explicit ResourcePtrBase(Resource*);
    explicit ResourcePtrBase(const ResourcePtrBase&);
    ~ResourcePtrBase();

    void setResource(Resource*);

private:
    friend class Resource;
    ResourcePtrBase& operator=(const ResourcePtrBase&) = delete;

    Resource* m_resource;
};

inline ResourcePtrBase::ResourcePtrBase(Resource* res)
    : m_resource(res)
{
    if (m_resource)
        m_resource->registerHandle(this);
}

inline ResourcePtrBase::~ResourcePtrBase()
{
    if (m_resource)
        m_resource->unregisterHandle(this);
}

inline ResourcePtrBase::ResourcePtrBase(const ResourcePtrBase& o)
    : m_resource(o.m_resource)
{
    if (m_resource)
        m_resource->registerHandle(this);
}

template <class R> class ResourcePtr final : public ResourcePtrBase {
public:
    ResourcePtr() { }
    ResourcePtr(R* res) : ResourcePtrBase(res) { }
    ResourcePtr(const ResourcePtr<R>& o) : ResourcePtrBase(o) { }
    template<typename U> ResourcePtr(const ResourcePtr<U>& o) : ResourcePtrBase(cast(o.get())) { }

    R* get() const { return static_cast<R*>(ResourcePtrBase::get()); }
    R* operator->() const { return get(); }

    ResourcePtr& operator=(R* res) { setResource(res); return *this; }
    ResourcePtr& operator=(const ResourcePtr& o) { setResource(o.get()); return *this; }
    template<typename U> ResourcePtr& operator=(const ResourcePtr<U>& o) { setResource(cast(o.get())); return *this; }

    bool operator==(const ResourcePtrBase& o) const { return get() == o.get(); }
    bool operator!=(const ResourcePtrBase& o) const { return get() != o.get(); }
private:
    template<typename U> static R* cast(U* u) { return u; }
};

template <class R, class RR> bool operator==(const ResourcePtr<R>& h, const RR* res)
{
    return h.get() == res;
}
template <class R, class RR> bool operator==(const RR* res, const ResourcePtr<R>& h)
{
    return h.get() == res;
}
template <class R, class RR> bool operator!=(const ResourcePtr<R>& h, const RR* res)
{
    return h.get() != res;
}
template <class R, class RR> bool operator!=(const RR* res, const ResourcePtr<R>& h)
{
    return h.get() != res;
}
}

#endif
