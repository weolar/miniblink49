
#include "node/include/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/event_emitter.h"
#include "gin/object_template_builder.h"
#include "wke.h"

namespace atom {

class Electron : public mate::EventEmitter<Electron> {
public:
    explicit Electron(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<Electron>::InitWith(isolate, wrapper);
    }

    static void init(v8::Local<v8::Object> target, v8::Isolate* isolate) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "electron"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("quit", &Electron::nullFunction);
        

        constructor.Reset(isolate, prototype->GetFunction());
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

v8::Persistent<v8::Function> Electron::constructor;
gin::WrapperInfo Electron::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeElectronApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Electron::init(target, env->isolate());
}

static const char BrowserElectronNative[] =
"var Module = require('module');"
"console.log('BrowserElectronNative:' + module);"
"//require('./../browser/browser-window');"
"//var electron = Module._load('./common/electron', null, false);"
"//const App = process.binding('atom_browser_app').App;"
"//var electron = {app : App};"
"exports = {};";

static const char BrowserElectronNative2[] = "./common/electron";

static NodeNative nativeBrowserAppNative{ "electron", BrowserElectronNative, sizeof(BrowserElectronNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_electron, initializeElectronApi, &nativeBrowserAppNative)

}