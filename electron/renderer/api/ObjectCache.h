// Copyright (c) 2020 Slack Technologies, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef SHELL_RENDERER_API_CONTEXT_BRIDGE_OBJECT_CACHE_H_
#define SHELL_RENDERER_API_CONTEXT_BRIDGE_OBJECT_CACHE_H_

#include <forward_list>
#include <unordered_map>
#include <utility>
#include "v8.h"

namespace atom {

namespace api {

namespace context_bridge {

using ObjectCachePair = std::pair<v8::Local<v8::Value>, v8::Local<v8::Value>>;

class ObjectCache final {
public:
    ObjectCache();
    ~ObjectCache();

    void CacheProxiedObject(v8::Local<v8::Value> from, v8::Local<v8::Value> proxy_value);
    v8::MaybeLocal<v8::Value> GetCachedProxiedObject(v8::Local<v8::Value> from) const;

private:
    // object_identity ==> [from_value, proxy_value]
    std::unordered_map<int, std::forward_list<ObjectCachePair>> proxy_map_;
};

}  // namespace context_bridge

}  // namespace api

}  // namespace atom

#endif  // SHELL_RENDERER_API_CONTEXT_BRIDGE_OBJECT_CACHE_H_
