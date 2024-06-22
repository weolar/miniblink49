// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_COMMON_API_EVENT_EMITTER_CALLER_H_
#define ATOM_COMMON_API_EVENT_EMITTER_CALLER_H_

#include <vector>

#include "gin/converter.h"
#include "v8.h"

namespace mate {

namespace internal {

    using ValueVector = std::vector<v8::Local<v8::Value>>;

    v8::Local<v8::Value> callEmitWithArgs(v8::Isolate* isolate,
        v8::Local<v8::Object> obj,
        ValueVector* args);

    v8::Local<v8::Value> emitEventImpl(v8::Isolate* isolate, v8::Local<v8::Object> obj, internal::ValueVector& converted_args, v8::Local<v8::Object> event, const base::ListValue& args);

} // namespace internal

// obj.emit.apply(obj, name, args...);
// The caller is responsible of allocating a HandleScope.
template <typename StringType, typename... Args>
v8::Local<v8::Value> emitEvent(v8::Isolate* isolate,
    v8::Local<v8::Object> obj,
    const StringType& name,
    const internal::ValueVector& args)
{
    internal::ValueVector concatenated_args = { gin::StringToV8(isolate, name) };
    concatenated_args.reserve(1 + args.size());
    concatenated_args.insert(concatenated_args.end(), args.begin(), args.end());
    return internal::callEmitWithArgs(isolate, obj, &concatenated_args);
}

// obj.emit(name, args...);
// The caller is responsible of allocating a HandleScope.
template <typename StringType, typename... Args>
v8::Local<v8::Value> emitEvent(v8::Isolate* isolate,
    v8::Local<v8::Object> obj,
    const StringType& name,
    const Args&... args)
{
    internal::ValueVector converted_args = {
        gin::StringToV8(isolate, name),
        gin::ConvertToV8(isolate, args)...,
    };
    return internal::callEmitWithArgs(isolate, obj, &converted_args);
}

// obj.emit(name, event, listArgs...);
// The caller is responsible of allocating a HandleScope.
template <typename StringType, typename... Args>
v8::Local<v8::Value> emitEvent(v8::Isolate* isolate, v8::Local<v8::Object> obj, const StringType& name, v8::Local<v8::Object> event, const base::ListValue& args)
{
    internal::ValueVector converted_args = {
        gin::StringToV8(isolate, name),
        gin::ConvertToV8(isolate, event)
    };
    return internal::emitEventImpl(isolate, obj, converted_args, event, args);
}

} // namespace mate

#endif // ATOM_COMMON_API_EVENT_EMITTER_CALLER_H_
