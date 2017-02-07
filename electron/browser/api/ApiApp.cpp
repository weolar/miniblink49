
#include "node/include/nodeblink.h"
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

    ~App() {
        DebugBreak();
    }

    static void init(v8::Local<v8::Object> target, v8::Isolate* isolate) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "App"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("quit", &App::nullFunction);
        builder.SetMethod("exit", &App::nullFunction);
        builder.SetMethod("focus", &App::nullFunction);
        builder.SetMethod("getVersion", &App::getVersionApi);
        builder.SetMethod("setVersion", &App::setVersionApi);
        builder.SetMethod("getName", &App::getNameApi);
        builder.SetMethod("setName", &App::setNameApi);
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
        builder.SetMethod("setPath", &App::setPathApi);
        builder.SetMethod("getPath", &App::getPathApi);
        builder.SetMethod("setDesktopName", &App::setDesktopNameApi);
        builder.SetMethod("getLocale", &App::nullFunction);
        builder.SetMethod("makeSingleInstance", &App::nullFunction);
        builder.SetMethod("releaseSingleInstance", &App::nullFunction);
        builder.SetMethod("relaunch", &App::nullFunction);
        builder.SetMethod("isAccessibilitySupportEnabled", &App::isAccessibilitySupportEnabled);
        builder.SetMethod("disableHardwareAcceleration", &App::disableHardwareAcceleration);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "App"), prototype->GetFunction());
    }

    void nullFunction() {
        OutputDebugStringA("nullFunction begin \n");
        //DebugBreak();
        OutputDebugStringA("nullFunction over\n");
    }

    bool isReady() const { return true; }

    bool isAccessibilitySupportEnabled() { return false; }
    void disableHardwareAcceleration() {}

    void setVersionApi(const std::string& version) { m_version = version; }
    std::string getVersionApi() const { return m_version; }

    void setNameApi(const std::string& name) { m_name = name; }
    std::string getNameApi() const { return m_name; }

    void setPathApi(const std::string& path) { m_path = path; }
    std::string getPathApi() const { return m_path; }

    void setDesktopNameApi(const std::string& desktopName) { ; }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        v8::HandleScope scope(isolate);

        if (args.IsConstructCall()) {
            new App(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;

    std::string m_version;
    std::string m_name;
    std::string m_path;
};

v8::Persistent< v8::Function> App::constructor;
gin::WrapperInfo App::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeAppApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    App::init(target, env->isolate());
}

static const char BrowserAppNative[] = "console.log('BrowserAppNative');;";
static NodeNative nativeBrowserAppNative{ "App", BrowserAppNative, sizeof(BrowserAppNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_app, initializeAppApi, &nativeBrowserAppNative)

}