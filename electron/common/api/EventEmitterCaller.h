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

}  // namespace internal

// obj.emit.apply(obj, name, args...);
// The caller is responsible of allocating a HandleScope.
template<typename StringType, typename... Args>
v8::Local<v8::Value> emitEvent(v8::Isolate* isolate,
                               v8::Local<v8::Object> obj,
                               const StringType& name,
                               const internal::ValueVector& args) {
  internal::ValueVector concatenated_args = { gin::StringToV8(isolate, name) };
  concatenated_args.reserve(1 + args.size());
  concatenated_args.insert(concatenated_args.end(), args.begin(), args.end());
  return internal::callEmitWithArgs(isolate, obj, &concatenated_args);
}

// obj.emit(name, args...);
// The caller is responsible of allocating a HandleScope.
template<typename StringType, typename... Args>
v8::Local<v8::Value> emitEvent(v8::Isolate* isolate,
                               v8::Local<v8::Object> obj,
                               const StringType& name,
                               const Args&... args) {
  internal::ValueVector converted_args = {
      gin::StringToV8(isolate, name),
      gin::ConvertToV8(isolate, args)...,
  };
  return internal::callEmitWithArgs(isolate, obj, &converted_args);
}

// obj.emit(name, event, listArgs...);
// The caller is responsible of allocating a HandleScope.
template<typename StringType, typename... Args>
v8::Local<v8::Value> emitEvent(v8::Isolate* isolate, v8::Local<v8::Object> obj,
    const StringType& name, v8::Local<v8::Object> event, const base::ListValue& args) {
    internal::ValueVector converted_args = {
        gin::StringToV8(isolate, name),
        gin::ConvertToV8(isolate, event)
    };

    bool boolVal = false;
    int intVal = 0;
    double doubleVal = 0;
    std::string strVal;
    for (size_t i = 0; i < args.GetSize(); ++i) {
        const base::Value* outValue;
        args.Get(i, &outValue);
        base::Value::Type type = outValue->GetType();
        switch (type) {
        case base::Value::TYPE_BOOLEAN:
            outValue->GetAsBoolean(&boolVal);
            converted_args.push_back(v8::Boolean::New(isolate, boolVal));
            break;
        case base::Value::TYPE_INTEGER:
            outValue->GetAsInteger(&intVal);
            converted_args.push_back(v8::Integer::New(isolate, intVal));
            break;
        case base::Value::TYPE_DOUBLE:
            outValue->GetAsDouble(&doubleVal);
            converted_args.push_back(v8::Number::New(isolate, doubleVal));
            break;
        case base::Value::TYPE_STRING:
            outValue->GetAsString(&strVal);
            converted_args.push_back(v8::String::NewFromUtf8(isolate, strVal.c_str(), v8::NewStringType::kNormal, strVal.size()).ToLocalChecked());
            break;
        default:
            converted_args.push_back(v8::Null(isolate));
            break;
        }
    }

    return internal::callEmitWithArgs(isolate, obj, &converted_args);
}

}  // namespace mate

#endif  // ATOM_COMMON_API_EVENT_EMITTER_CALLER_H_
