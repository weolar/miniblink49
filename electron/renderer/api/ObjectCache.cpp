// Copyright (c) 2020 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "renderer/api/ObjectCache.h"

#include <utility>

namespace atom {

namespace api {

namespace context_bridge {

ObjectCache::ObjectCache() {}
ObjectCache::~ObjectCache() = default;

void ObjectCache::CacheProxiedObject(v8::Local<v8::Value> from, v8::Local<v8::Value> proxy_value)
{
    if (from->IsObject() && !from->IsNullOrUndefined()) {
        auto obj = from.As<v8::Object>();
        int hash = obj->GetIdentityHash();

        proxy_map_[hash].push_front(std::make_pair(from, proxy_value));
    }
}

v8::MaybeLocal<v8::Value> ObjectCache::GetCachedProxiedObject(v8::Local<v8::Value> from) const
{
    if (!from->IsObject() || from->IsNullOrUndefined())
        return v8::MaybeLocal<v8::Value>();

    v8::Local<v8::Object> obj = from.As<v8::Object>();
    int hash = obj->GetIdentityHash();
    std::unordered_map<int, std::forward_list<ObjectCachePair> >::const_iterator iter = proxy_map_.find(hash);
    if (iter == proxy_map_.end())
        return v8::MaybeLocal<v8::Value>();

    const std::forward_list<ObjectCachePair>& list = iter->second;
    for (const auto& pair : list) {
        v8::Local<v8::Value> from_cmp = pair.first;
        if (from_cmp == from) {
            if (pair.second.IsEmpty())
                return v8::MaybeLocal<v8::Value>();
            return pair.second;
        }
    }
    return v8::MaybeLocal<v8::Value>();
}

}  // namespace context_bridge

}  // namespace api

}  // namespace electron
