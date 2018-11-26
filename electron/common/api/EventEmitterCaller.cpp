// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/EventEmitterCaller.h"

#include "node/src/node.h"
#include "base/values.h"

namespace mate {

namespace internal {

v8::Local<v8::Value> callEmitWithArgs(v8::Isolate* isolate, v8::Local<v8::Object> obj, ValueVector* args) {
#if V8_MAJOR_VERSION == 5
    // Perform microtask checkpoint after running JavaScript.
    v8::MicrotasksScope script_scope(isolate, v8::MicrotasksScope::kRunMicrotasks);
#endif
    // Use node::MakeCallback to call the callback, and it will also run pending
    // tasks in Node.js.
    return node::MakeCallback(isolate, obj, "emit", args->size(), &args->front()); 
}

v8::Local<v8::Value> emitEventImpl(v8::Isolate* isolate, v8::Local<v8::Object> obj,
    internal::ValueVector& converted_args, v8::Local<v8::Object> event, const base::ListValue& args) {

    bool boolVal = false;
    int intVal = 0;
    double doubleVal = 0;
    std::string strVal;
    const base::ListValue* listValue = nullptr;
    const base::DictionaryValue* dictionaryValue = nullptr;
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
        case base::Value::TYPE_LIST:
            outValue->GetAsList(&listValue);
            converted_args.push_back(gin::Converter<base::ListValue>::ToV8(isolate, *listValue));
            break;
        case base::Value::TYPE_DICTIONARY:
            outValue->GetAsDictionary(&dictionaryValue);
            converted_args.push_back(gin::Converter<base::DictionaryValue>::ToV8(isolate, *dictionaryValue));
            break;
        case base::Value::TYPE_NULL:
            converted_args.push_back(v8::Null(isolate));
            break;
        default:
            *(int*)1 = 1;
            converted_args.push_back(v8::Null(isolate));
            break;
        }
    }

    return callEmitWithArgs(isolate, obj, &converted_args);
}

}  // namespace internal

}  // namespace mate
