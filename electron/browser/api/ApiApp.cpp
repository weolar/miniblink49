
#include "ApiApp.h"
#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "common/ThreadCall.h"
#include "gin/dictionary.h"
#include "NodeRegisterHelp.h"

using namespace v8;
using namespace node;

namespace atom {

#pragma warning(push)
#pragma warning(disable:4309)
#pragma warning(disable:4838)
static const char helloNative[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };
#pragma warning(pop)

static NodeNative nativeHello{ "hello", helloNative, sizeof(helloNative) };

// 静态方法，用于注册类和方法
void App::init(Local<Object> target, Environment* env) {
    Isolate* isolate = env->isolate();

    // Function模板
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, newFunction);
    // 类名
    tpl->SetClassName(String::NewFromUtf8(isolate, "App"));
    // InternalField
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    v8::Local<v8::Template> t = tpl->InstanceTemplate();
    // 设置Prototype函数

    NODE_SET_METHOD(t, "quit", nullFunction);
    NODE_SET_METHOD(t, "exit", nullFunction);
    NODE_SET_METHOD(t, "focus", nullFunction);
    NODE_SET_METHOD(t, "getVersion", nullFunction);
    NODE_SET_METHOD(t, "setVersion", nullFunction);
    NODE_SET_METHOD(t, "getName", nullFunction);
    NODE_SET_METHOD(t, "setName", nullFunction);
    NODE_SET_METHOD(t, "isReady", nullFunction);
    NODE_SET_METHOD(t, "addRecentDocument", nullFunction);
    NODE_SET_METHOD(t, "clearRecentDocuments", nullFunction);
    NODE_SET_METHOD(t, "setAppUserModelId", nullFunction);
    NODE_SET_METHOD(t, "isDefaultProtocolClient", nullFunction);
    NODE_SET_METHOD(t, "setAsDefaultProtocolClient", nullFunction);
    NODE_SET_METHOD(t, "removeAsDefaultProtocolClient", nullFunction);
    NODE_SET_METHOD(t, "setBadgeCount", nullFunction);
    NODE_SET_METHOD(t, "getBadgeCount", nullFunction);
    NODE_SET_METHOD(t, "getLoginItemSettings", nullFunction);
    NODE_SET_METHOD(t, "setLoginItemSettings", nullFunction);
    NODE_SET_METHOD(t, "setUserTasks", nullFunction);
    NODE_SET_METHOD(t, "getJumpListSettings", nullFunction);
    NODE_SET_METHOD(t, "setJumpList", nullFunction);
    NODE_SET_METHOD(t, "setPath", nullFunction);
    NODE_SET_METHOD(t, "getPath", nullFunction);
    NODE_SET_METHOD(t, "setDesktopName", nullFunction);
    NODE_SET_METHOD(t, "getLocale", nullFunction);
    NODE_SET_METHOD(t, "makeSingleInstance", nullFunction);
    NODE_SET_METHOD(t, "releaseSingleInstance", nullFunction);
    NODE_SET_METHOD(t, "relaunch", nullFunction);
    NODE_SET_METHOD(t, "isAccessibilitySupportEnabled", nullFunction);
    NODE_SET_METHOD(t, "disableHardwareAcceleration", nullFunction);
    // 设置constructor
    constructor.Reset(isolate, tpl->GetFunction());
    // export `BrowserWindow`
    target->Set(String::NewFromUtf8(isolate, "App"), tpl->GetFunction());
}

App::App() {

}

App::~App() {
}

// new方法
void App::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.IsConstructCall()) {
        if (args.Length() > 1) {
            return;
        }
        // 使用new调用 `new Point(...)`
        gin::Dictionary options(args.GetIsolate(), args[0]->ToObject());
        // new一个对象
        App* con = new App();
        // 包装this指针
		con->Wrap(args.This(), isolate);
        args.GetReturnValue().Set(args.This());
    }
    else {
        // 使用`Point(...)`
        const int argc = 2;
        Local<Value> argv[argc] = { args[0], args[1] };
        // 使用constructor构建Function
        Local<Function> cons = Local<Function>::New(isolate, constructor);
        args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
}

// 空实现
void App::nullFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
}

Persistent<Function> App::constructor;

static void initializeAppApi(Local<Object> target,
    Local<Value> unused,
    Local<Context> context) {
    Environment* env = Environment::GetCurrent(context);
    App::init(target, env);
}

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_app, initializeAppApi, &nativeHello)

} // atom
