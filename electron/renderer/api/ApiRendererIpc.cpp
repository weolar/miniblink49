
#include "node/include/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/event_emitter.h"
#include "gin/object_template_builder.h"
#include "wke.h"

namespace atom {

class Ipc : public mate::EventEmitter<Ipc> {
public:
    explicit Ipc(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<Ipc>::InitWith(isolate, wrapper);
    }

    static void init(v8::Local<v8::Object> target, v8::Isolate* isolate) {
        //         v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);
        // 
        //         prototype->SetClassName(v8::String::NewFromUtf8(isolate, "ipc"));
        //         gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        //         builder.SetMethod("quit", &Ipc::nullFunction);
        //         
        //         constructor.Reset(isolate, prototype->GetFunction());
        //         target->Set(v8::String::NewFromUtf8(isolate, "ipc"), prototype->GetFunction());
    }

    void nullFunction() {
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);

        if (args.IsConstructCall()) {
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
};

v8::Persistent<v8::Function> Ipc::constructor;
gin::WrapperInfo Ipc::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeIpcApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Ipc::init(target, env->isolate());
}

static const char RendererIpcNative[] =
"exports = {};";

static NodeNative nativeRendererIpcNative{ "ipc", RendererIpcNative, sizeof(RendererIpcNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_renderer_ipc, initializeIpcApi, &nativeRendererIpcNative)

}