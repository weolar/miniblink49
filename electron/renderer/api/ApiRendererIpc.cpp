
#include "node/include/nodeblink.h"
#include "browser/api/ApiWebContents.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "gin/dictionary.h"
#include "gin/arguments.h"
#include "gin/object_template_builder.h"
#include "wke.h"
#include "base/values.h"
#include <xstring>

namespace atom {

class IpcRenderer : public mate::EventEmitter<IpcRenderer> {
public:
    explicit IpcRenderer(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<IpcRenderer>::InitWith(isolate, wrapper);
    }

    static void init(v8::Local<v8::Object> target, v8::Isolate* isolate) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, IpcRenderer::newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "IpcRenderer"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("send", &IpcRenderer::rendererIpcSend);
        builder.SetMethod("sendSync", &IpcRenderer::rendererIpcSendSync);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "ipcRenderer"), prototype->GetFunction());
    }

    void rendererIpcSend(const std::string& channel, const base::ListValue& arguments) {
        wkeWebView view = wkeGetWebViewForCurrentContext();
        if (!view)
            return;
        WebContents* webContents = (WebContents*)wkeGetUserKeyValue(view, "WebContents");
        if (!webContents)
            return;
        webContents->rendererPostMessageToMain(channel, arguments);
    }

    std::string rendererIpcSendSync(const std::string& channel, const base::ListValue& arguments) {
        wkeWebView view = wkeGetWebViewForCurrentContext();
        if (!view)
            return "";
        WebContents* webContents = (WebContents*)wkeGetUserKeyValue(view, "WebContents");
        if (!webContents)
            return "";

        std::string json;
        webContents->rendererSendMessageToMain(channel, arguments, &json);

        if (0 == json.size())
            json = "{}";

        return json;
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        new IpcRenderer(isolate, args.This());

        if (args.IsConstructCall()) {
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
};

v8::Persistent<v8::Function> IpcRenderer::constructor;
gin::WrapperInfo IpcRenderer::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeRendererIpcApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    IpcRenderer::init(target, context->GetIsolate());
}

static const char RendererIpcNative[] =
"exports = {};";

static NodeNative nativeRendererIpcNative{ "ipc-renderer", RendererIpcNative, sizeof(RendererIpcNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_renderer_ipc, initializeRendererIpcApi, &nativeRendererIpcNative)

}