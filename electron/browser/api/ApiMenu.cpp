
#include "node/include/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "gin/object_template_builder.h"
#include "gin/dictionary.h"
#include "wke.h"

namespace atom {

class Menu : public mate::EventEmitter<Menu> {
public:
    explicit Menu(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<Menu>::InitWith(isolate, wrapper);
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Menu"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("quit", &Menu::nullFunction);
        builder.SetMethod("_setApplicationMenu", &Menu::setApplicationMenuApi);
        builder.SetMethod("_sendActionToFirstResponder", &Menu::sendActionToFirstResponderApi);
        builder.SetMethod("_buildFromTemplate", &Menu::buildFromTemplate);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Menu"), prototype->GetFunction());
    }

    void nullFunction() {
    }

    // Set the global menubar.
    void setApplicationMenuApi(/*Menu* menu*/int menuTemplateId) {
        DebugBreak();
    }

    void sendActionToFirstResponderApi(const std::string& action) {
        DebugBreak();
    }

    int buildFromTemplate(const base::ListValue& menuTemplate) {
        return 0x1234432;
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            new Menu(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
};

v8::Persistent<v8::Function> Menu::constructor;
gin::WrapperInfo Menu::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeMenuApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Menu::init(env->isolate(), target);
}

static const char BrowserMenuNative[] =
"console.log('BrowserMenuNative');"
"exports = function {};";

static NodeNative nativeBrowserMenuNative{ "Menu", BrowserMenuNative, sizeof(BrowserMenuNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_menu, initializeMenuApi, &nativeBrowserMenuNative)

}