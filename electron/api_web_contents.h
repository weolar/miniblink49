#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"
#include "electron.h"
#include "dictionary.h"
using namespace v8;
using namespace node;

// 继承node的ObjectWrap，一般自定义C++类都应该继承node的ObjectWrap
class WebContents :
    public node::ObjectWrap {
public:
    // 静态方法，用于注册类和方法
    static void init(Local<Object> target, Environment* env);
    static WebContents* create(Isolate* isolate, gin::Dictionary options);
    // 不允许转换构造函数进行的隐式转换
    explicit WebContents(v8::Isolate* isolate, const gin::Dictionary& options);
    ~WebContents();

private:
    // new方法
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    // _loadURL
    static void _LoadURL(const v8::FunctionCallbackInfo<v8::Value>& args);
    
    // 空实现
    static void nullFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;
public:
    wkeWebView m_view;
};
