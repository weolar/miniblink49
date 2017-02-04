#include "nodeblink.h"
#include <node_object_wrap.h>
#include "wke.h"

using namespace v8;
using namespace node;

namespace atom {

class Electron :
    public mate::EventEmitter<Electron> {
public:
    // 静态方法，用于注册类和方法
    static void init(Local<Object> target, Environment* env);

    // 不允许转换构造函数进行的隐式转换
    explicit Electron();
    ~Electron();

private:
    // new方法
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

    static void nullFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;
};

} // atom