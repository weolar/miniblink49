#include "nodeblink.h"

using namespace v8;
using namespace node;
const char hello_native[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };

node_native native_hello{ "hello", hello_native, sizeof(hello_native) };

static void Method(const FunctionCallbackInfo<Value>& args) {
	Environment* env = Environment::GetCurrent(args);
	args.GetReturnValue().Set(String::NewFromUtf8(env->isolate(), "hello,world"));
}

void Initialize(Local<Object> target,
	Local<Value> unused,
	Local<Context> context) {
	Environment* env = Environment::GetCurrent(context);
	env->SetMethod(target, "Method", Method);
}
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT(hello, Initialize, &native_hello)