// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "node/nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "gin/object_template_builder.h"
#include "gin/dictionary.h"

namespace {

class IntlCollator : public mate::EventEmitter<IntlCollator> {
public:
    explicit IntlCollator(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<IntlCollator>::InitWith(isolate, wrapper);
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "IntlCollator"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("compare", &IntlCollator::compareApi);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "IntlCollator"), prototype->GetFunction());
    }

    int compareApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsString())
            return -1;

        v8::String::Utf8Value str0(args[0]);
        v8::String::Utf8Value str1(args[1]);
        const char* string0 = *str0;
        int stringLen0 = str0.length();

        const char* string1 = *str1;
        int stringLen1 = str1.length();

        if (stringLen0 == 0 && stringLen1 == 0) {
            return 0;
        } else if (stringLen0 != 0 && stringLen1 == 0) {
            return 1;
        } else if (stringLen0 == 0 && stringLen1 != 0) {
            return -1;
        }

        for (int i = 0; i < stringLen0 && i < stringLen1; ++i) {
            if (string0[i] == string1[i])
                continue;
            return string0[i] - string1[i];
        }
        return 0;
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            new IntlCollator(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
};

v8::Persistent<v8::Function> IntlCollator::constructor;
gin::WrapperInfo IntlCollator::kWrapperInfo = { gin::kEmbedderNativeGin };

void initializeCommonIntlCollatorApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv) {
    node::Environment* env = node::Environment::GetCurrent(context);
    IntlCollator::init(env->isolate(), target);
}

}  // namespace

static const char CommonIntlCollatorSricpt[] =
"exports = {};";

static NodeNative nativeCommonIntlCollatorNative{ "IntlCollator", CommonIntlCollatorSricpt, sizeof(CommonIntlCollatorSricpt) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_common_intl_collator, initializeCommonIntlCollatorApi, &nativeCommonIntlCollatorNative)
