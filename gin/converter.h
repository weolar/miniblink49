// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_CONVERTER_H_
#define GIN_CONVERTER_H_

#include <string>
#include <vector>

#include "base/logging.h"
#include "base/strings/string_piece.h"
#include "base/values.h"
#include "gin/gin_export.h"
#include "v8.h"

namespace gin {

template<typename KeyType>
bool SetProperty(v8::Isolate* isolate,
                 v8::Local<v8::Object> object,
                 KeyType key,
                 v8::Local<v8::Value> value) {
  auto maybe = object->Set(isolate->GetCurrentContext(), key, value);
  return !maybe.IsNothing() && maybe.FromJust();
}

template<typename T>
struct ToV8ReturnsMaybe {
  static const bool value = false;
};

template<typename T, typename Enable = void>
struct Converter {};

template<>
struct GIN_EXPORT Converter<bool> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    bool val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     bool* out);
};

template<>
struct GIN_EXPORT Converter<int32_t> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    int32_t val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     int32_t* out);
};

template<>
struct GIN_EXPORT Converter<uint32_t> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    uint32_t val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     uint32_t* out);
};

template<>
struct GIN_EXPORT Converter<int64_t> {
  // Warning: JavaScript cannot represent 64 integers precisely.
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    int64_t val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     int64_t* out);
};

template<>
struct GIN_EXPORT Converter<uint64_t> {
  // Warning: JavaScript cannot represent 64 integers precisely.
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    uint64_t val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     uint64_t* out);
};

template<>
struct GIN_EXPORT Converter<float> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    float val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     float* out);
};

template<>
struct GIN_EXPORT Converter<double> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    double val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     double* out);
};

template<>
struct GIN_EXPORT Converter<base::StringPiece> {
  // This crashes when val.size() > v8::String::kMaxLength.
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    const base::StringPiece& val);
  // No conversion out is possible because StringPiece does not contain storage.
};

template<>
struct GIN_EXPORT Converter<std::string> {
  // This crashes when val.size() > v8::String::kMaxLength.
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    const std::string& val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     std::string* out);
};

template<>
struct GIN_EXPORT Converter<v8::Local<v8::Function> > {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     v8::Local<v8::Function>* out);
};

template<>
struct GIN_EXPORT Converter<v8::Local<v8::Object> > {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    v8::Local<v8::Object> val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     v8::Local<v8::Object>* out);
};

template<>
struct GIN_EXPORT Converter<v8::Local<v8::ArrayBuffer> > {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    v8::Local<v8::ArrayBuffer> val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     v8::Local<v8::ArrayBuffer>* out);
};

template<>
struct GIN_EXPORT Converter<v8::Local<v8::External> > {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    v8::Local<v8::External> val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     v8::Local<v8::External>* out);
};

template<>
struct GIN_EXPORT Converter<v8::Local<v8::Value> > {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                    v8::Local<v8::Value> val);
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     v8::Local<v8::Value>* out);
};

template<>
struct GIN_EXPORT Converter<v8::Local<v8::String> > {
    static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
        v8::Local<v8::String> val);
    static bool FromV8(v8::Isolate* isolate,
        v8::Local<v8::Value> val,
        v8::Local<v8::String>* out);
};

template<>
struct GIN_EXPORT Converter<v8::Local<v8::Array> > {
    static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
        v8::Local<v8::Array> val);
    static bool FromV8(v8::Isolate* isolate,
        v8::Local<v8::Value> val,
        v8::Local<v8::Array>* out);
};

template<>
struct GIN_EXPORT Converter<base::ListValue> {
    static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
        const base::ListValue& val);
    static bool FromV8(v8::Isolate* isolate,
        v8::Local<v8::Value> val,
        base::ListValue* out);
};

template<>
struct GIN_EXPORT Converter<base::DictionaryValue> {
    static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
        const base::DictionaryValue& val);
    static bool FromV8(v8::Isolate* isolate,
        v8::Local<v8::Value> val,
        base::DictionaryValue* out);
};

template<typename T>
struct Converter<std::vector<T> > {
  static v8::MaybeLocal<v8::Value> ToV8(v8::Local<v8::Context> context,
                                        const std::vector<T>& val) {
    v8::Isolate* isolate = context->GetIsolate();
    v8::Local<v8::Array> result(
        v8::Array::New(isolate, static_cast<int>(val.size())));
    for (uint32_t i = 0; i < val.size(); ++i) {
      auto maybe = result->Set(context, i, Converter<T>::ToV8(isolate, val[i]));
      if (maybe.IsNothing() || !maybe.FromJust())
        return v8::MaybeLocal<v8::Value>();
    }
    return result;
  }

  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     std::vector<T>* out) {
    if (!val->IsArray())
      return false;

    std::vector<T> result;
    v8::Local<v8::Array> array(v8::Local<v8::Array>::Cast(val));
    uint32_t length = array->Length();
    for (uint32_t i = 0; i < length; ++i) {
      v8::Local<v8::Value> v8_item;
      if (!array->Get(isolate->GetCurrentContext(), i).ToLocal(&v8_item))
        return false;
      T item;
      if (!Converter<T>::FromV8(isolate, v8_item, &item))
        return false;
      result.push_back(item);
    }

    out->swap(result);
    return true;
  }
};

template<typename T>
struct ToV8ReturnsMaybe<std::vector<T>> {
  static const bool value = true;
};

// Convenience functions that deduce T.
template<typename T>
v8::Local<v8::Value> ConvertToV8(v8::Isolate* isolate, T input) {
  return Converter<T>::ToV8(isolate, input);
}

template<typename T>
v8::MaybeLocal<v8::Value> ConvertToV8(v8::Local<v8::Context> context, T input) {
  return Converter<T>::ToV8(context, input);
}

v8::Local<v8::Value> ConvertToV8(v8::Isolate* isolate, const base::ListValue& input);

template<typename T, bool = ToV8ReturnsMaybe<T>::value> struct ToV8Traits;

template <typename T>
struct ToV8Traits<T, true> {
  static bool TryConvertToV8(v8::Isolate* isolate,
                             T input,
                             v8::Local<v8::Value>* output) {
    auto maybe = ConvertToV8(isolate->GetCurrentContext(), input);
    if (maybe.IsEmpty())
      return false;
    *output = maybe.ToLocalChecked();
    return true;
  }
};

template <typename T>
struct ToV8Traits<T, false> {
  static bool TryConvertToV8(v8::Isolate* isolate,
                             T input,
                             v8::Local<v8::Value>* output) {
    *output = ConvertToV8(isolate, input);
    return true;
  }
};

template <typename T>
bool TryConvertToV8(v8::Isolate* isolate,
                    T input,
                    v8::Local<v8::Value>* output) {
  return ToV8Traits<T>::TryConvertToV8(isolate, input, output);
}

// This crashes when input.size() > v8::String::kMaxLength.
GIN_EXPORT inline v8::Local<v8::String> StringToV8(
    v8::Isolate* isolate,
    const base::StringPiece& input) {
  return ConvertToV8(isolate, input).As<v8::String>();
}

// This crashes when input.size() > v8::String::kMaxLength.
GIN_EXPORT v8::Local<v8::String> StringToSymbol(v8::Isolate* isolate,
                                                 const base::StringPiece& val);

template<typename T>
bool ConvertFromV8(v8::Isolate* isolate, v8::Local<v8::Value> input,
                   T* result) {
  return Converter<T>::FromV8(isolate, input, result);
}

GIN_EXPORT std::string V8ToString(v8::Local<v8::Value> value);

}  // namespace gin

#endif  // GIN_CONVERTER_H_
