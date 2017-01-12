#include "nodeblink.h"
#include <node_object_wrap.h>
using namespace v8;
using namespace node;
const char hello_native[] = { 239,187,191,39,117,115,101,32,115,116,114,105,99,116,39,59,10,99,111,110,115,116,32,98,105,110,100,105,110,103,32,61,32,112,114,111,99,101,115,115,46,98,105,110,100,105,110,103,40,39,104,101,108,108,111,39,41,59,10,101,120,112,111,114,116,115,46,77,101,116,104,111,100,32,61,32,98,105,110,100,105,110,103,46,77,101,116,104,111,100,59,10,10,10 };

node_native native_hello{ "hello", hello_native, sizeof(hello_native) };

// 继承node的ObjectWrap，一般自定义C++类都应该继承node的ObjectWrap
class Window : public node::ObjectWrap {
public:
	// 静态方法，用于注册类和方法
	static void Init(Local<Object> target, Environment* env) {
		Isolate* isolate = Isolate::GetCurrent();

		// Function模板
		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
		// 类名
		tpl->SetClassName(String::NewFromUtf8(isolate, "Point"));
		// InternalField
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		// 设置Prototype函数
		NODE_SET_PROTOTYPE_METHOD(tpl, "move", Move);
		NODE_SET_PROTOTYPE_METHOD(tpl, "toString", toString);
		// 设置constructor
		constructor.Reset(isolate, tpl->GetFunction());
		// export `Point`
		target->Set(String::NewFromUtf8(isolate, "Point"),
			tpl->GetFunction());
	}

private:
	// 不允许转换构造函数进行的隐式转换
	explicit Window(double _x = 0, double _y = 0) {

	}
	~Window() {

	}
	// new方法
	static void New(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);

		if (args.IsConstructCall()) {
			// 使用new调用 `new Point(...)`
			double _x = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
			double _y = args[1]->IsUndefined() ? 0 : args[1]->NumberValue();
			// new一个对象
			Window* point = new Window(_x, _y);
			// 包装this指针
			point->Wrap(args.This());
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
	// move(x, y)
	static void Move(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* point = ObjectWrap::Unwrap<Window>(args.Holder());
		// 参数
		double _x = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
		double _y = args[1]->IsUndefined() ? 0 : args[1]->NumberValue();
		// move
		point->_x += _x;
		point->_y += _y;
	}
	// toString方法
	static void toString(const v8::FunctionCallbackInfo<v8::Value>& args) {
		Isolate* isolate = Isolate::GetCurrent();
		HandleScope scope(isolate);
		// 解封this指针
		Window* point = ObjectWrap::Unwrap<Window>(args.Holder());
		// 设置返回值
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, "aa"));
	}
	static v8::Persistent<v8::Function> constructor;
	// fields
	double _x;
	double _y;
};

void Initialize(Local<Object> target,
	Local<Value> unused,
	Local<Context> context) {
	Environment* env = Environment::GetCurrent(context);
	Window::Init(target, env);

}
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT(atom_browser_window, Initialize, &native_hello)