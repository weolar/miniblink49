
#include "node/include/nodeblink.h"
#include "common/OptionsSwitches.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/event_emitter.h"
#include "gin/object_template_builder.h"
#include "wke.h"

namespace atom {

class App : public mate::EventEmitter<App> {
public:
    explicit App(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<App>::InitWith(isolate, wrapper);
    }

    static void init(v8::Local<v8::Object> target, v8::Isolate* isolate) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "App"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("quit", &App::nullFunction);
        builder.SetMethod("exit", &App::nullFunction);
        builder.SetMethod("focus", &App::nullFunction);
        builder.SetMethod("getVersion", &App::nullFunction);
        builder.SetMethod("setVersion", &App::nullFunction);
        builder.SetMethod("getName", &App::nullFunction);
        builder.SetMethod("setName", &App::nullFunction);
        builder.SetMethod("isReady", &App::nullFunction);
        builder.SetMethod("addRecentDocument", &App::nullFunction);
        builder.SetMethod("clearRecentDocuments", &App::nullFunction);
        builder.SetMethod("setAppUserModelId", &App::nullFunction);
        builder.SetMethod("isDefaultProtocolClient", &App::nullFunction);
        builder.SetMethod("setAsDefaultProtocolClient", &App::nullFunction);
        builder.SetMethod("removeAsDefaultProtocolClient", &App::nullFunction);
        builder.SetMethod("setBadgeCount", &App::nullFunction);
        builder.SetMethod("getBadgeCount", &App::nullFunction);
        builder.SetMethod("getLoginItemSettings", &App::nullFunction);
        builder.SetMethod("setLoginItemSettings", &App::nullFunction);
        builder.SetMethod("setUserTasks", &App::nullFunction);
        builder.SetMethod("getJumpListSettings", &App::nullFunction);
        builder.SetMethod("setJumpList", &App::nullFunction);
        builder.SetMethod("setPath", &App::nullFunction);
        builder.SetMethod("getPath", &App::nullFunction);
        builder.SetMethod("setDesktopName", &App::nullFunction);
        builder.SetMethod("getLocale", &App::nullFunction);
        builder.SetMethod("makeSingleInstance", &App::nullFunction);
        builder.SetMethod("releaseSingleInstance", &App::nullFunction);
        builder.SetMethod("relaunch", &App::nullFunction);
        builder.SetMethod("isAccessibilitySupportEnabled", &App::nullFunction);
        builder.SetMethod("disableHardwareAcceleration", &App::nullFunction);

        constructor.Reset(isolate, prototype->GetFunction());
    }

    // 空实现
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

v8::Persistent< v8::Function> App::constructor;
gin::WrapperInfo App::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeAppApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    App::init(target, nullptr);
}

static const char BrowserAppNative[] = ";;";
static NodeNative nativeBrowserAppNative{ "App", BrowserAppNative, sizeof(BrowserAppNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_app, initializeAppApi, &nativeBrowserAppNative)

}