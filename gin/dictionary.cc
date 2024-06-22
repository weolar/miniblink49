// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/dictionary.h"
#include "base/strings/string_util.h"

namespace gin {

Dictionary::Dictionary(v8::Isolate* isolate)
    : isolate_(isolate) {
}

Dictionary::Dictionary(v8::Isolate* isolate, v8::Local<v8::Object> object)
    : isolate_(isolate),
    object_(object)
{
}

Dictionary::~Dictionary()
{
}

Dictionary Dictionary::CreateEmpty(v8::Isolate* isolate)
{
    Dictionary dictionary(isolate);
    dictionary.object_ = v8::Object::New(isolate);
    return dictionary;
}

v8::Local<v8::Value> Converter<Dictionary>::ToV8(v8::Isolate* isolate, Dictionary val)
{
    return val.object_;
}

bool Converter<Dictionary>::FromV8(v8::Isolate* isolate, v8::Local<v8::Value> val, Dictionary* out)
{
    if (!val->IsObject())
        return false;
    *out = Dictionary(isolate, v8::Local<v8::Object>::Cast(val));
    return true;
}

void Dictionary::GetBydefaultVal(const char* name, bool defaultVal, bool* outResult) const
{
    v8::Local<v8::Value> result;
    if (!this->Get(name, &result) || !result->IsBoolean()) {
        *outResult = defaultVal;
        return;
    }
    *outResult = result->ToBoolean()->BooleanValue();
}

void Dictionary::GetBydefaultVal(const char* name, int defaultVal, int* outResult) const
{
    v8::Local<v8::Value> result;
    if (!this->Get(name, &result) || !result->IsInt32()) {
        *outResult = defaultVal;
        return;
    }

    v8::Local<v8::Context> context = this->isolate()->GetCurrentContext();
    v8::Local<v8::Int32> v = result->ToInt32(context).ToLocalChecked();
    *outResult = v->Value();
}

void Dictionary::GetBydefaultVal(const char* name, double defaultVal, double* outResult) const
{
    v8::Local<v8::Value> result;
    if (!this->Get(name, &result) || !result->IsNumber()) {
        *outResult = defaultVal;
        return;
    }

    v8::Local<v8::Context> context = this->isolate()->GetCurrentContext();
    v8::Local<v8::Number> v = result->ToNumber(context).ToLocalChecked();
    *outResult = v->Value();
}

void Dictionary::GetBydefaultVal(const char* name, std::string defaultVal, std::string* outResult) const
{
    v8::Local<v8::Value> result;
    if (!this->Get(name, &result) || !result->IsString()) {
        *outResult = defaultVal;
        return;
    }

    v8::Local<v8::Context> context = this->isolate()->GetCurrentContext();
    v8::Local<v8::String> v = result->ToString(context).ToLocalChecked();

    int length = v->Length();
    if (0 > length) {
        *outResult = defaultVal;
        return;
    }

    v8::String::Utf8Value str(v);
    *outResult = *str;
}

void Dictionary::GetBydefaultVal(const char* name, std::wstring defaultVal, std::wstring* outResult) const
{
    std::string defaultValA = base::UTF16ToUTF8(defaultVal);
    std::string outResultA;
    GetBydefaultVal(name, defaultValA, &outResultA);

    *outResult = base::UTF8ToWide(outResultA);
}

}  // namespace gin
