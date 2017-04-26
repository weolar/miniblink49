#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
//////////////////////////////////////////////////////////////////////////
#define BUILDING_wke

#include <config.h>
#include "v8.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8StringResource.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8Binding.h"
#include "third_party/WebKit/Source/core/frame/LocalDOMWindow.h"
#include "third_party/WebKit/Source/core/frame/LocalFrame.h"
#include "third_party/WebKit/Source/core/page/ChromeClient.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8RecursionScope.h"
#include "content/browser/WebFrameClientImpl.h"
#include "content/browser/WebPage.h"

//cexer: 必须包含在后面，因为其中的 wke.h -> windows.h 会定义 max、min，导致 WebCore 内部的 max、min 出现错乱。
#include "wke/wke.h"
#include "wke/wkeJsBind.h"
#include "wke/wkeWebView.h"

//////////////////////////////////////////////////////////////////////////

struct JsExecStateInfo {
public:
    static JsExecStateInfo* create();
    ~JsExecStateInfo() 
    {
        if (!context.IsEmpty())
            context.Reset();
    }
   
    v8::Isolate* isolate;
    const v8::FunctionCallbackInfo<v8::Value>* args;
    v8::Persistent<v8::Context> context;

private:
    JsExecStateInfo()
    {
        isolate = nullptr;
        args = nullptr;
    }
};
typedef JsExecStateInfo* jsExecState;
static Vector<jsExecState>* s_execStates = nullptr;

JsExecStateInfo* JsExecStateInfo::create()
{
    JsExecStateInfo* retVal = new JsExecStateInfo();
    s_execStates->append(retVal);
    return retVal;
}

// jsValue 的值分两种情况，一种是c++创建的，一种是v8创建再转换成jsValue的
class WkeJsValue {
public:
    enum Type {
        wkeJsValueInt,
        wkeJsValueFloat,
        wkeJsValueBool,
        wkeJsValueString,
        wkeJsValueV8Value,
        wkeJsValueNull,
        wkeJsValueUndefined,
    } ;

    WkeJsValue()
    {
        isolate = nullptr;
        type = wkeJsValueUndefined;
    }

    ~WkeJsValue()
    {
        value.Reset();
        context.Reset();
    }
    v8::Isolate* isolate;
    v8::Persistent<v8::Value> value;
    v8::Persistent<v8::Context> context;

    Type type;
    int intVal;
    float floatVal;
    bool boolVal;
    WTF::CString stringVal;
};

typedef WTF::HashMap<jsValue, WkeJsValue*> JsValueMap;
static JsValueMap* jsValueMap = nullptr;

static bool isJsValueValid(jsValue value)
{
    return (jsValueMap->contains(value));
}

static v8::Local<v8::Value> getV8Value(jsValue v, v8::Local<v8::Context> context)
{
    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return v8::Local<v8::Value>(); // v8::Undefined(v8::Isolate::GetCurrent());

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueInt == wkeValue->type) {
        v8::Local<v8::Integer> out = v8::Integer::New(v8::Isolate::GetCurrent(), wkeValue->intVal);
        return out;
    } else if (WkeJsValue::wkeJsValueFloat == wkeValue->type) {
        v8::Local<v8::Number> out = v8::Number::New(v8::Isolate::GetCurrent(), wkeValue->floatVal);
        return out;
    } else if (WkeJsValue::wkeJsValueBool == wkeValue->type) {
        v8::Local<v8::Boolean> out = v8::Boolean::New(v8::Isolate::GetCurrent(), wkeValue->boolVal);
        return out;
    } else if (WkeJsValue::wkeJsValueUndefined == wkeValue->type) {
        v8::Local<v8::Primitive> out = v8::Undefined(v8::Isolate::GetCurrent());
        return out;
    } else if (WkeJsValue::wkeJsValueNull == wkeValue->type) {
        v8::Local<v8::Primitive> out = v8::Null(v8::Isolate::GetCurrent());
        return out;
    } else if (WkeJsValue::wkeJsValueString == wkeValue->type) {
        v8::MaybeLocal<v8::String> out = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), wkeValue->stringVal.data(), v8::NewStringType::kNormal, -1);
        if (out.IsEmpty())
            return v8::Local<v8::Value>();
        return out.ToLocalChecked();
    }
    
    v8::Isolate* isolate = wkeValue->isolate;
//     v8::HandleScope handleScope(isolate);
//     v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> out = v8::Local<v8::Value>::New(isolate, wkeValue->value);
    return out;
}

// static v8::Persistent<v8::Value> getV8Value2222(jsValue v, v8::Local<v8::Context> context)
// {
//     JsValueMap::iterator it = jsValueMap->find(v);
//     if (it == jsValueMap->end())
//         return v8::Persistent<v8::Value>();
// 
//     WkeJsValue* wkeValue = it->value;
//     v8::Isolate* isolate = wkeValue->isolate;
//     v8::HandleScope handleScope(isolate);
//     v8::Context::Scope contextScope(context);
// 
//     return wkeValue->value;
// }

static jsValue createJsValueByLocalValue(v8::Isolate* isolate, v8::Local<v8::Context> context, v8::Local<v8::Value> value)
{
    static __int64 handleCount = 0;
    handleCount++;

    WkeJsValue* out = new WkeJsValue();
    out->isolate = isolate;
    out->value.Reset(isolate, value);

    out->type = WkeJsValue::wkeJsValueV8Value;
    out->context.Reset(isolate, context);

    jsValueMap->add(handleCount, out);
    return handleCount;
}

static jsValue createEmptyJsValue(WkeJsValue** out)
{
    static __int64 handleCount = 0;
    handleCount++;

    *out = new WkeJsValue();
    jsValueMap->add(handleCount, *out);
    return handleCount;
}

int jsArgCount(jsExecState es)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->args)
        return 0;
    return es->args->Length();
}

jsType jsArgType(jsExecState es, int argIdx)
{
    return jsTypeOf(jsArg(es, argIdx));
}

jsValue jsArg(jsExecState es, int argIdx)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->args || argIdx >= es->args->Length() || es->context.IsEmpty())
        return jsUndefined();
    v8::Local<v8::Value> value =(*es->args)[argIdx];

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    return createJsValueByLocalValue(es->isolate, context, value);
}

jsType jsTypeOf(jsValue v)
{
    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return JSTYPE_UNDEFINED;

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueInt == wkeValue->type || WkeJsValue::wkeJsValueFloat == wkeValue->type)
        return JSTYPE_NUMBER;
    else if (WkeJsValue::wkeJsValueBool == wkeValue->type)
        return JSTYPE_BOOLEAN;
    else if (WkeJsValue::wkeJsValueString == wkeValue->type)
        return JSTYPE_STRING;
    else if (WkeJsValue::wkeJsValueNull == wkeValue->type || WkeJsValue::wkeJsValueUndefined == wkeValue->type)
        return JSTYPE_UNDEFINED;

    v8::Isolate* isolate = wkeValue->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, wkeValue->context);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);

    if (value.IsEmpty())
        return JSTYPE_UNDEFINED;

    if (value->IsUndefined())
        return JSTYPE_UNDEFINED;

    if (value->IsBoolean())
        return JSTYPE_BOOLEAN;

    if (value->IsNumber())
        return JSTYPE_NUMBER;

    if (value->IsString())
        return JSTYPE_STRING;

    if (value->IsObject())
        return JSTYPE_OBJECT;
    
    if (value->IsFunction())
        return JSTYPE_FUNCTION;

    return JSTYPE_UNDEFINED;
}

bool jsIsNumber(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_NUMBER ? true : false;
}

bool jsIsString(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_STRING ? true : false;
}

bool jsIsBoolean(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_BOOLEAN ? true : false;
}

bool jsIsObject(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_OBJECT ? true : false;
}

bool jsIsFunction(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_FUNCTION ? true : false;
}

bool jsIsUndefined(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_UNDEFINED ? true : false;
}

bool jsIsNull(jsValue v)
{
    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return false;

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value != wkeValue->type)
        return wkeValue->type == WkeJsValue::wkeJsValueNull;

    v8::Isolate* isolate = wkeValue->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, wkeValue->context);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
    return value->IsNull();
    
}

bool jsIsArray(jsValue v)
{
    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return false;

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value != wkeValue->type)
        return false;
    v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
    return value->IsArray();

    return false;
}

bool jsIsTrue(jsValue v)
{
    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return false;

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueBool == wkeValue->type)
        return wkeValue->boolVal;
    else if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        if (!value->IsBoolean())
            return false;
        v8::Local<v8::Boolean> boolValue = value->ToBoolean();
        return boolValue->Value();
    }

    return false;
}

bool jsIsFalse(jsValue v)
{
    return !jsIsTrue(v);
}

int jsToInt(jsExecState es, jsValue v)
{
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return 0;

    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return 0;
    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        return value->ToInt32()->Value();
    } else if (WkeJsValue::wkeJsValueInt == wkeValue->type) {
        return wkeValue->intVal;
    }
    return 0;
}

float jsToFloat(jsExecState es, jsValue v)
{
    return (float)jsToDouble(es, v);
}

double jsToDouble(jsExecState es, jsValue v)
{
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return 0.0;

    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return 0.0;
    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        return value->ToNumber()->Value();
    } else if (WkeJsValue::wkeJsValueFloat == wkeValue->type)
        return wkeValue->floatVal;
    return 0.0;
}

bool jsToBoolean(jsExecState es, jsValue v)
{
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return false;

    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return false;
    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        return value->ToBoolean()->Value();
    } else if (WkeJsValue::wkeJsValueBool == wkeValue->type)
        return wkeValue->boolVal;

    return false;
}


static Vector<char> s_sharedStringBuffer;
static Vector<wchar_t> s_sharedStringBufferW;

const utf8* jsToTempString(jsExecState es, jsValue v)
{
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return "";

    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return "";

    WkeJsValue* wkeValue = it->value;
    s_sharedStringBuffer.clear();
    WTF::CString sharedStringBuffer;

    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        if (wkeValue->value.IsEmpty())
            return "";

        v8::Isolate* isolate = es->isolate;
        v8::HandleScope handleScope(isolate);
        v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, es->context);
        v8::Context::Scope contextScope(context);

        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        if (!value->IsString())
            return "";

        v8::Local<v8::String> stringValue = value->ToString();
        String stringWTF = blink::v8StringToWebCoreString<String>(stringValue, blink::DoNotExternalize);
        sharedStringBuffer = stringWTF.utf8();
    } else if (WkeJsValue::wkeJsValueString == wkeValue->type) {
        sharedStringBuffer = wkeValue->stringVal;
    }

    s_sharedStringBuffer.resize(sharedStringBuffer.length());
    memcpy(s_sharedStringBuffer.data(), sharedStringBuffer.data(), sharedStringBuffer.length());
    s_sharedStringBuffer.append('\0');
    return s_sharedStringBuffer.data();

    return "";    
}

const wchar_t* jsToTempStringW(jsExecState es, jsValue v)
{
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return L"";

    JsValueMap::iterator it = jsValueMap->find(v);
    if (it == jsValueMap->end())
        return L"";

    WkeJsValue* wkeValue = it->value;
    String stringWTF;
    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        if (wkeValue->value.IsEmpty())
            return L"";

        v8::Isolate* isolate = es->isolate;
        v8::HandleScope handleScope(isolate);
        v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, es->context);
        v8::Context::Scope contextScope(context);

        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        if (!value->IsString())
            return L"";

        v8::Local<v8::String> stringValue = value->ToString();
        stringWTF = blink::v8StringToWebCoreString<String>(stringValue, blink::DoNotExternalize);
    } else if (WkeJsValue::wkeJsValueString == wkeValue->type) {
        stringWTF = String::fromUTF8(wkeValue->stringVal.data());
    }

    s_sharedStringBufferW = WTF::ensureStringToUChars(stringWTF);
    return s_sharedStringBufferW.data();

    return L"";
}

jsValue jsInt(int n)
{
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueInt;
    out->intVal = n;
    return ret;
}

jsValue jsFloat(float f)
{
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueFloat;
    out->floatVal = f;
    return ret;
}

jsValue jsDouble(double d)
{
    return jsFloat((float)d);
}

jsValue jsBoolean(bool b)
{
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueBool;
    out->boolVal = b;
    return ret;
}

jsValue jsUndefined()
{
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueUndefined;
    return ret;
}

jsValue jsNull()
{
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueNull;
    return ret;
}

jsValue jsTrue()
{
    return jsBoolean(true);
}

jsValue jsFalse()
{
    return jsBoolean(false);
}

jsValue jsString(jsExecState es, const utf8* str)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::MaybeLocal<v8::String> value = v8::String::NewFromUtf8(es->isolate, str, v8::NewStringType::kNormal, -1);
    if (value.IsEmpty())
        return jsUndefined();
    return createJsValueByLocalValue(es->isolate, context, value.ToLocalChecked());
}

jsValue jsStringW(jsExecState es, const wchar_t* str)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::MaybeLocal<v8::String> value = v8::String::NewFromTwoByte(es->isolate, (const uint16_t*)str, v8::NewStringType::kNormal, -1);
    if (value.IsEmpty())
        return jsUndefined();
    return createJsValueByLocalValue(es->isolate, context, value.ToLocalChecked());
}

jsValue jsEmptyObject(jsExecState es)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    return createJsValueByLocalValue(es->isolate, context, v8::Object::New(es->isolate));
}

jsValue jsEmptyArray(jsExecState es)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);
    return createJsValueByLocalValue(es->isolate, context, v8::Array::New(es->isolate));
}

//return the window object


//return the window object
jsValue jsGlobalObject(jsExecState es)
{
    if (!s_execStates || !es || !s_execStates->contains(es) || !es->isolate || es->context.IsEmpty())
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();
    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> obj = context->Global();

    v8::MaybeLocal<v8::String> str = v8::String::NewFromUtf8(isolate, "window", v8::NewStringType::kNormal, -1);
    if (str.IsEmpty())
        return jsUndefined();
    v8::Local<v8::Value> val = obj->Get(str.ToLocalChecked());
    if (val.IsEmpty() || !val->IsObject())
        return jsUndefined();
    
    jsValue ret = createJsValueByLocalValue(es->isolate, context, val);
    return ret;
}

jsValue jsEval(jsExecState es, const utf8* str)
{
    String s = String::fromUTF8(str);
    Vector<UChar> buf = WTF::ensureUTF16UChar(s);
    return jsEvalW(es, buf.data());
}

jsValue jsEvalW(jsExecState es, const wchar_t* str)
{
    if (!s_execStates || !es || !s_execStates->contains(es) || !es->isolate || es->context.IsEmpty() || !str)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    String codeString(str);
    if (codeString.startsWith("javascript:", WTF::TextCaseInsensitive))
        codeString.remove(0, sizeof("javascript:") - 1);
    codeString.insert("(function(){", 0);
    codeString.append("})();");

    v8::Isolate* isolate = es->isolate;
    blink::V8RecursionScope::MicrotaskSuppression microtaskSuppression(isolate);

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::MaybeLocal<v8::String> source = v8::String::NewFromUtf8(isolate, codeString.utf8().data(), v8::NewStringType::kNormal);
    if (source.IsEmpty())
        return jsUndefined();
    v8::Local<v8::Script> script = v8::Script::Compile(source.ToLocalChecked());
    v8::TryCatch trycatch;
    v8::Local<v8::Value> result = script->Run();

    return wke::v8ValueToJsValue(context, result);
}

jsValue jsCall(jsExecState es, jsValue func, jsValue thisValue, jsValue* args, int argCount)
{
//     JSC::ExecState* exec = (JSC::ExecState*)es;
// 
//     if (!jsIsFunction(func))
//         return jsUndefined();
// 
//     JSC::JSValue jsThisValue = JSC::JSValue::decode(thisValue);
//     if (!jsThisValue.isObject())
//         jsThisValue = exec->globalThisValue();
// 
//     JSC::MarkedArgumentBuffer argList;
//     for (int i = 0; i < argCount; i++)
//         argList.append(JSC::JSValue::decode(args[i]));
// 
//     JSC::CallData callData;
//     JSC::JSObject* object = JSC::asObject(JSC::JSValue::decode(func));
//     JSC::CallType callType = object->methodTable()->getCallData(object, callData);
// 
//     JSC::JSValue value = JSC::call(exec, object, callType, callData, jsThisValue, argList);
//     return JSC::JSValue::encode(value);
    return jsUndefined();
}

jsValue jsCallGlobal(jsExecState es, jsValue func, jsValue* args, int argCount)
{
    return jsCall(es, func, jsUndefined(), args, argCount);
}

jsValue jsGet(jsExecState es, jsValue object, const char* prop)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> value = getV8Value(object, context);
    if (value.IsEmpty() || !value->IsObject())
        return jsUndefined();

    v8::Local<v8::Object> obj = value->ToObject();

    v8::TryCatch tryCatch;
    tryCatch.SetVerbose(true);

    v8::MaybeLocal<v8::String> propV8 = v8::String::NewFromUtf8(isolate, prop, v8::NewStringType::kNormal, -1);
    if (propV8.IsEmpty())
        return jsUndefined();
    v8::Local<v8::String> propV8Local = propV8.ToLocalChecked();

    v8::Local<v8::Value> retValue = obj->Get(propV8Local);
    if (!tryCatch.HasCaught() && !retValue.IsEmpty())
        return createJsValueByLocalValue(isolate, context, retValue);
    return jsUndefined();
}

void jsSet(jsExecState es, jsValue object, const char* prop, jsValue value)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return;
    if (es->context.IsEmpty())
        DebugBreak();
    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> objectLocal = getV8Value(object, context);
    if (objectLocal.IsEmpty() || !objectLocal->IsObject())
        return;

    v8::Local<v8::Value> valueLocal = getV8Value(value, context);
    if (valueLocal.IsEmpty())
        return;
    
    v8::Local<v8::Object> obj = objectLocal->ToObject();

    v8::TryCatch tryCatch;
    tryCatch.SetVerbose(true);

    v8::MaybeLocal<v8::String> propV8 = v8::String::NewFromUtf8(isolate, prop, v8::NewStringType::kNormal, -1);
    if (propV8.IsEmpty())
        return;
    v8::Local<v8::String> propV8Local = propV8.ToLocalChecked();

    obj->Set(propV8Local, valueLocal);
}

jsValue jsGetGlobal(jsExecState es, const char* prop)
{
    return jsGet(es, jsGlobalObject(es), prop);
}

void jsSetGlobal(jsExecState es, const char* prop, jsValue v)
{
    jsSet(es, jsGlobalObject(es), prop, v);
}

jsValue jsGetAt(jsExecState es, jsValue object, int index)
{
//     JSC::JSValue o = JSC::JSValue::decode(object);
// 
//     JSC::JSValue ret = o.get((JSC::ExecState*)es, index);
//     return JSC::JSValue::encode(ret);
    return jsUndefined();
}

void jsSetAt(jsExecState es, jsValue object, int index, jsValue value)
{
//     JSC::JSValue o = JSC::JSValue::decode(object);
//     JSC::JSValue v = JSC::JSValue::decode(value);
// 
//     o.put((JSC::ExecState*)es, index, v);
}

int jsGetLength(jsExecState es, jsValue object)
{
//     JSC::ExecState* exec = (JSC::ExecState*)es;
//     JSC::JSValue o = JSC::JSValue::decode(object);
//     return o.get(exec, JSC::Identifier(exec, "length")).toInt32(exec);
    return jsUndefined();
}

void jsSetLength(jsExecState es, jsValue object, int length)
{
//     JSC::ExecState* exec = (JSC::ExecState*)es;
//     JSC::JSValue o = JSC::JSValue::decode(object);
// 
//     JSC::PutPropertySlot slot;
//     o.put(exec, JSC::Identifier(exec, "length"), JSC::jsNumber(length), slot);
}

wkeWebView jsGetWebView(jsExecState es)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return nullptr;
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);

    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObj = context->Global();

    v8::MaybeLocal<v8::String> nameMaybeLocal = v8::String::NewFromUtf8(isolate, "wkeWebViewToV8Context", v8::NewStringType::kNormal, -1);
    if (nameMaybeLocal.IsEmpty())
        return nullptr;
	//zero
#if V8_MINOR_VERSION == 7
	v8::Local<v8::Value> wkeWebViewV8 = blink::V8HiddenValue::getHiddenValue(isolate, globalObj, nameMaybeLocal.ToLocalChecked());
#else
    v8::Local<v8::Value> wkeWebViewV8 = globalObj->GetHiddenValue(nameMaybeLocal.ToLocalChecked());
#endif
	ASSERT(!wkeWebViewV8.IsEmpty());
    wke::CWebView* webView = static_cast<wke::CWebView*>(v8::External::Cast(*wkeWebViewV8)->Value());
    return webView;
}

void jsGC()
{
    //WebCore::gcController().garbageCollectNow();
}


static void functionCallbackImpl(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    jsNativeFunction func = static_cast<jsNativeFunction>(v8::External::Cast(*info.Data())->Value());
    JsExecStateInfo* execState = JsExecStateInfo::create();
    execState->args = &info;
    execState->isolate = isolate;
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    execState->context.Reset(isolate, context);
    jsValue retVal = func(execState);

    v8::Local<v8::Value> rv = getV8Value(retVal, context);
    info.GetReturnValue().Set(rv);
}

static void addFunction(v8::Local<v8::Context> context, const char* name, jsNativeFunction nativeFunction, unsigned int argCount)
{
    v8::Isolate* isolate = context->GetIsolate();
    if (!isolate->InContext())
        return;
    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Object> object = context->Global();
    v8::Local<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New(isolate);
    v8::Local<v8::Value> data = v8::External::New(isolate, nativeFunction);

    // Set the function handler callback.
    tmpl->SetCallHandler(functionCallbackImpl, data);

    // Retrieve the function object and set the name.
    v8::Local<v8::Function> func = tmpl->GetFunction();
    if (func.IsEmpty())
        return;
    
    v8::MaybeLocal<v8::String> nameV8 = v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal, -1);
    if (nameV8.IsEmpty())
        return;
    v8::Local<v8::String> nameV8Local = nameV8.ToLocalChecked();
    func->SetName(nameV8Local);

    object->Set(nameV8Local, func);
}

class NativeGetterSetterWrap {
public:
    jsNativeFunction getter;
    jsNativeFunction setter;

    jsData* jsDataObj;

    void set(jsNativeFunction getter, jsNativeFunction setter)
    {
        this->getter = getter;
        this->setter = setter;
        this->jsDataObj = nullptr;
    }

    void set(jsData* jsDataObj)
    {
        this->getter = nullptr;
        this->setter = nullptr;
        this->jsDataObj = jsDataObj;
    }

    NativeGetterSetterWrap()
    {
        this->getter = nullptr;
        this->setter = nullptr;
        this->jsDataObj = nullptr;
    }

    static void AccessorGetterCallbackImpl(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        v8::Isolate* isolate = info.GetIsolate();
        NativeGetterSetterWrap* getterSetter = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*info.Data())->Value());

        jsExecState execState = JsExecStateInfo::create();
        execState->args = nullptr;
        execState->isolate = isolate;
        execState->context.Reset(isolate, isolate->GetCurrentContext());
        jsValue retJsValue = getterSetter->getter(execState);

        info.GetReturnValue().Set(getV8Value(retJsValue, isolate->GetCurrentContext()));
    }

    static void AccessorSetterCallbackImpl(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        v8::Isolate* isolate = info.GetIsolate();

        NativeGetterSetterWrap* getterSetter = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*info.Data())->Value());

        jsExecState execState = JsExecStateInfo::create();
        execState->args = nullptr;
        execState->isolate = isolate;
        execState->context.Reset(isolate, isolate->GetCurrentContext());
        getterSetter->setter(execState);

        info.GetReturnValue().SetUndefined();
    }

    static NativeGetterSetterWrap* createWrapAndAddToGlobalObjForRelease(v8::Isolate* isolate, v8::Local<v8::Object>& globalObj)
    {
        v8::MaybeLocal<v8::String> addAccessorDataMaybeLocal = v8::String::NewFromUtf8(isolate, "wkeAddAccessorData", v8::NewStringType::kNormal, -1);
        if (addAccessorDataMaybeLocal.IsEmpty())
            return nullptr;

        Vector<NativeGetterSetterWrap*>* cachedWraps = new Vector<NativeGetterSetterWrap*>();
		//zero
#if V8_MINOR_VERSION == 7
		v8::Local<v8::Value> dataMap = blink::V8HiddenValue::getHiddenValue(isolate, globalObj, addAccessorDataMaybeLocal.ToLocalChecked());
#else
        v8::Local<v8::Value> dataMap = globalObj->GetHiddenValue(addAccessorDataMaybeLocal.ToLocalChecked());
#endif
        if (dataMap.IsEmpty()) {
            dataMap = v8::External::New(isolate, cachedWraps);
			//zero
#if V8_MINOR_VERSION == 7
			blink::V8HiddenValue::setHiddenValue(isolate, globalObj, addAccessorDataMaybeLocal.ToLocalChecked(), dataMap);
#else
            globalObj->SetHiddenValue(addAccessorDataMaybeLocal.ToLocalChecked(), dataMap);
#endif
        }
        NativeGetterSetterWrap* wrap = new NativeGetterSetterWrap();
        cachedWraps->append(wrap);
        return wrap;
    }
};

static void addAccessor(v8::Local<v8::Context> context, const char* name, jsNativeFunction getter, jsNativeFunction setter)
{
    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObj = v8::Local<v8::Object>::Cast(context->Global()->GetPrototype());

    v8::TryCatch tryCatch;
    tryCatch.SetVerbose(true);

    v8::MaybeLocal<v8::String> nameMaybeLocal = v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal, -1);
    if (nameMaybeLocal.IsEmpty())
        return;

    NativeGetterSetterWrap* wrap = NativeGetterSetterWrap::createWrapAndAddToGlobalObjForRelease(isolate, globalObj);
    wrap->set(getter, setter);
    v8::Local<v8::Value> data = v8::External::New(isolate, wrap);

    bool setOk = globalObj->SetAccessor(nameMaybeLocal.ToLocalChecked(),
        NativeGetterSetterWrap::AccessorGetterCallbackImpl, NativeGetterSetterWrap::AccessorSetterCallbackImpl,
        data, (v8::AccessControl)(v8::ALL_CAN_READ | v8::ALL_CAN_WRITE));
}


#define MAX_NAME_LENGTH 32
#define MAX_FUNCTION_COUNT 1024

#define JS_FUNC   (0)
#define JS_GETTER (1)
#define JS_SETTER (2)

struct jsFunctionInfo {
    char name[MAX_NAME_LENGTH];
    jsNativeFunction fn;
    unsigned int argCount;
    unsigned int funcType;
};

static Vector<jsFunctionInfo>* s_jsFunctionsPtr = nullptr;

void jsBindFunction(const char* name, jsNativeFunction fn, unsigned int argCount)
{
    if (!s_jsFunctionsPtr)
        s_jsFunctionsPtr = new Vector<jsFunctionInfo>();
    Vector<jsFunctionInfo>& s_jsFunctions = *s_jsFunctionsPtr;

    for (unsigned int i = 0; i < s_jsFunctions.size(); ++i) {
        if (s_jsFunctions[i].funcType == JS_FUNC && strncmp(name, s_jsFunctions[i].name, MAX_NAME_LENGTH) == 0) {
            s_jsFunctions[i].fn = fn;
            s_jsFunctions[i].argCount = argCount;
            return;
        }
    }

    jsFunctionInfo funcInfo;
    strncpy(funcInfo.name, name, MAX_NAME_LENGTH - 1);
    funcInfo.name[MAX_NAME_LENGTH - 1] = '\0';
    funcInfo.fn = fn;
    funcInfo.argCount = argCount;
    funcInfo.funcType = JS_FUNC;

    s_jsFunctions.append(funcInfo);
}

void jsBindGetter(const char* name, jsNativeFunction fn)
{
    if (!s_jsFunctionsPtr)
        s_jsFunctionsPtr = new Vector<jsFunctionInfo>();
    Vector<jsFunctionInfo>& s_jsFunctions = *s_jsFunctionsPtr;

    for (unsigned int i = 0; i < s_jsFunctions.size(); ++i) {
        if (s_jsFunctions[i].funcType == JS_GETTER && strncmp(name, s_jsFunctions[i].name, MAX_NAME_LENGTH) == 0) {
            s_jsFunctions[i].fn = fn;
            return;
        }
    }

    jsFunctionInfo funcInfo;
    strncpy(funcInfo.name, name, MAX_NAME_LENGTH - 1);
    funcInfo.name[MAX_NAME_LENGTH - 1] = '\0';
    funcInfo.fn = fn;
    funcInfo.argCount = 0;
    funcInfo.funcType = JS_GETTER;

    s_jsFunctions.append(funcInfo);
}

void jsBindSetter(const char* name, jsNativeFunction fn)
{
    if (!s_jsFunctionsPtr)
        s_jsFunctionsPtr = new Vector<jsFunctionInfo>();
    Vector<jsFunctionInfo>& s_jsFunctions = *s_jsFunctionsPtr;

    for (unsigned int i = 0; i < s_jsFunctions.size(); ++i) {
        if (s_jsFunctions[i].funcType == JS_SETTER && strncmp(name, s_jsFunctions[i].name, MAX_NAME_LENGTH) == 0) {
            s_jsFunctions[i].fn = fn;
            return;
        }
    }

    jsFunctionInfo funcInfo;
    strncpy(funcInfo.name, name, MAX_NAME_LENGTH - 1);
    funcInfo.name[MAX_NAME_LENGTH - 1] = '\0';
    funcInfo.fn = fn;
    funcInfo.argCount = 1;
    funcInfo.funcType = JS_SETTER;

    s_jsFunctions.append(funcInfo);
}

jsValue JS_CALL js_outputMsg(jsExecState es)
{
    //ASSERT(jsArgCount(es) == 1);
    //ASSERT(jsArgType(es, 0) == JSTYPE_STRING);

    jsValue value = jsArg(es, 0);
    OutputDebugStringW(jsToTempStringW(es, value));

    return jsUndefined();
}

jsValue JS_CALL js_getWebViewName(jsExecState es)
{
    wkeWebView webView = jsGetWebView(es);
    return jsString(es, webView->name());
}

jsValue JS_CALL js_setWebViewName(jsExecState es)
{
    const char* name = jsToTempString(es, jsArg(es, 0));
    wkeWebView webView = jsGetWebView(es);
    webView->setName(name);

    return jsUndefined();
}

// JSValueRef objectGetPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exception)
// {
//     JSC::ExecState* exec = toJS(ctx);
//     JSC::JSObject* obj = toJS(object);
// 
//     jsData* p = (jsData*)JSObjectGetPrivate(object);
//     if (!p || !p->propertyGet)
//         return false;
// 
//     WTF::CString str = propertyName->ustring().latin1();
//     const char* name = str.data();
//     jsValue ret = p->propertyGet(exec, JSC::JSValue::encode(obj), name);
// 
//     return toRef(exec, JSC::JSValue::decode(ret));
// }
// 
// bool objectSetPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
// {
//     JSC::ExecState* exec = toJS(ctx);
//     JSC::JSObject* obj = toJS(object);
// 
//     jsData* p = (jsData*)JSObjectGetPrivate(object);
//     if (!p || !p->propertySet)
//         return false;
// 
//     WTF::CString str = propertyName->ustring().latin1();
//     const char* name = str.data();
//     return p->propertySet(exec, JSC::JSValue::encode(obj), name, JSC::JSValue::encode(toJS(exec,value)));
// }
// 
// void objectFinalize(JSObjectRef object)
// {
//     jsData* p = (jsData*)JSObjectGetPrivate(object);
//     if (p && p->finalize)
//         p->finalize(p);
// }
// 
// JSValueRef objectCallAsFunctionCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception)
// {
//     JSC::ExecState* exec = toJS(ctx);
//     JSC::JSObject* obj = toJS(function);
// 
//     jsData* p = (jsData*)JSObjectGetPrivate(function);
//     if (!p || !p->callAsFunction)
//         return false;
// 
//     jsValue args[10] = { 0 };
//     for (int i = 0; i < argumentCount; ++i)
//         args[i] = JSC::JSValue::encode(toJS(exec, arguments[i]));
// 
//     jsValue ret = p->callAsFunction(exec, JSC::JSValue::encode(toJS(function)), args, argumentCount);
//     return toRef(exec, JSC::JSValue::decode(ret));
// }

static void namedPropertyGetterCallback(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NativeGetterSetterWrap* wrap = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*info.Data())->Value());

    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
    jsExecState execState = JsExecStateInfo::create();
    execState->isolate = info.GetIsolate();
    execState->context.Reset(execState->isolate, context);
    jsValue object = createJsValueByLocalValue(info.GetIsolate(), context, info.Data());

    String stringWTF = blink::v8StringToWebCoreString<String>(property, blink::DoNotExternalize);
    jsValue retVal = wrap->jsDataObj->propertyGet(execState, object, stringWTF.utf8().data());

    v8::Local<v8::Value> rv = getV8Value(retVal, context);
    info.GetReturnValue().Set(rv);
}

static void namedPropertySetterCallback(v8::Local<v8::String> property, v8::Local<v8::Value> valueV8, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NativeGetterSetterWrap* wrap = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*info.Data())->Value());

    jsExecState execState = JsExecStateInfo::create();
    v8::Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
    execState->isolate = info.GetIsolate();
    execState->context.Reset(execState->isolate, context);
    jsValue object = createJsValueByLocalValue(info.GetIsolate(), context, info.Data());

    String stringWTF = blink::v8StringToWebCoreString<String>(property, blink::DoNotExternalize);

    jsValue value = createJsValueByLocalValue(info.GetIsolate(), context, valueV8);
    wrap->jsDataObj->propertySet(execState, object, stringWTF.utf8().data(), value);
}

WKE_API jsValue jsObject(jsExecState es, jsData* data)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || es->context.IsEmpty())
        return jsUndefined();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);

    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObj = context->Global();

    v8::Local<v8::ObjectTemplate> obj = v8::ObjectTemplate::New(isolate);
    
    NativeGetterSetterWrap* wrap = NativeGetterSetterWrap::createWrapAndAddToGlobalObjForRelease(isolate, globalObj);
    wrap->set(data);
    obj->SetNamedPropertyHandler(namedPropertyGetterCallback, namedPropertySetterCallback, nullptr, nullptr, nullptr, v8::External::New(isolate, wrap));

    jsValue retValue = createJsValueByLocalValue(isolate, context, obj->NewInstance(context).ToLocalChecked());
    return retValue;
}

void jsFunctionConstructCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
//     if (!args.IsConstructCall())
//         return;

    v8::Local<v8::Value> data = args.Data();

    NativeGetterSetterWrap* wrap = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*data)->Value());

    JsExecStateInfo* execState = JsExecStateInfo::create();
    execState->args = &args;
    execState->isolate = isolate;
    v8::Local<v8::Context> context = execState->isolate->GetCurrentContext();
    execState->context.Reset(isolate, context);
    
    jsValue wkeData = createJsValueByLocalValue(isolate, context, data);
    jsValue retWkeValue = wrap->jsDataObj->callAsFunction(execState, wkeData, nullptr, args.Length());
}

WKE_API jsValue jsFunction(jsExecState es, jsData* data)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || es->context.IsEmpty() || !es->isolate)
        return jsUndefined();
    
    v8::Isolate* isolate = es->isolate;
	v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, es->context);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObj = context->Global();

    NativeGetterSetterWrap* wrap = NativeGetterSetterWrap::createWrapAndAddToGlobalObjForRelease(isolate, globalObj);
    wrap->set(data);
    v8::Local<v8::Value> dataLocal = v8::External::New(isolate, wrap);

    v8::Local<v8::FunctionTemplate> functionTemplate = v8::FunctionTemplate::New(isolate, jsFunctionConstructCallback, dataLocal);
    v8::Local<v8::Function> v8Function;
    if (!functionTemplate->GetFunction(context).ToLocal(&v8Function))
        DebugBreak();
    jsValue retVal = createJsValueByLocalValue(isolate, context, v8Function);
    return retVal;
}

WKE_API jsData* jsGetData(jsExecState es, jsValue value)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || es->context.IsEmpty() || !es->isolate)
        return nullptr;

    v8::Isolate* isolate = es->isolate;
	v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> valueV8 = getV8Value(value, context);
    if (valueV8.IsEmpty() || !valueV8->IsExternal())
        return nullptr;

    NativeGetterSetterWrap* wrap = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*valueV8)->Value());
    ASSERT(wrap);
    return wrap->jsDataObj;
}

namespace wke {

static void setWkeWebViewToV8Context(content::WebFrameClientImpl* client, v8::Local<v8::Context> context)
{
    content::WebPage* webPage = client->webPage();
    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObj = context->Global();
    
    v8::MaybeLocal<v8::String> nameMaybeLocal = v8::String::NewFromUtf8(isolate, "wkeWebViewToV8Context", v8::NewStringType::kNormal, -1);
    if (nameMaybeLocal.IsEmpty())
        return;
	//zero
#if V8_MINOR_VERSION == 7
	v8::Local<v8::Value> wkeWebViewV8 = blink::V8HiddenValue::getHiddenValue(isolate, globalObj, nameMaybeLocal.ToLocalChecked());
#else
    v8::Local<v8::Value> wkeWebViewV8 = globalObj->GetHiddenValue(nameMaybeLocal.ToLocalChecked());
#endif
	ASSERT(wkeWebViewV8.IsEmpty());

    CWebView* wkeWebView = webPage->wkeWebView();
    ASSERT(wkeWebView);
    wkeWebViewV8 = v8::External::New(isolate, wkeWebView);
	//zero
#if V8_MINOR_VERSION == 7
	blink::V8HiddenValue::setHiddenValue(isolate, globalObj, nameMaybeLocal.ToLocalChecked(), wkeWebViewV8);
#else
    globalObj->SetHiddenValue(nameMaybeLocal.ToLocalChecked(), wkeWebViewV8);
#endif
}

jsExecState createTempExecStateByV8Context(v8::Local<v8::Context> context)
{
    JsExecStateInfo* execState = JsExecStateInfo::create();
    execState->context.Reset(context->GetIsolate(), context);
    execState->isolate = context->GetIsolate();
    return execState;
}

void onCreateGlobalObject(content::WebFrameClientImpl* client, blink::WebLocalFrame* frame, v8::Local<v8::Context> context, int extensionGroup, int worldId)
{
    content::WebPage* webPage = client->webPage();
    CWebView* wkeWebView = webPage->wkeWebView();
    if (!wkeWebView)
        return;

    if (!jsValueMap)
        jsValueMap = new JsValueMap();
    if (!s_execStates)
        s_execStates = new Vector<jsExecState>();
    
    v8::Isolate* isolate = context->GetIsolate();
    setWkeWebViewToV8Context(client, context);

    addFunction(context, "outputMsg", js_outputMsg, 1);
    addAccessor(context, "webViewName", js_getWebViewName, js_setWebViewName);

    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);

    JsExecStateInfo* execState = JsExecStateInfo::create();
    execState->args = nullptr;
    execState->isolate = isolate;
    execState->context.Reset(isolate, context);
    jsSetGlobal(execState, "wke", ::jsString(execState, wkeGetVersionString()));

    if (s_jsFunctionsPtr) {
        Vector<jsFunctionInfo>& s_jsFunctions = *s_jsFunctionsPtr;

        for (size_t i = 0; i < s_jsFunctions.size(); ++i) {
            if (s_jsFunctions[i].funcType == JS_FUNC)
                addFunction(context, s_jsFunctions[i].name, s_jsFunctions[i].fn, s_jsFunctions[i].argCount);
            else if (s_jsFunctions[i].funcType == JS_GETTER || s_jsFunctions[i].funcType == JS_SETTER) {
                jsNativeFunction getter = nullptr;
                jsNativeFunction setter = nullptr;
                if (s_jsFunctions[i].funcType == JS_GETTER)
                    getter = s_jsFunctions[i].fn;
                else if (s_jsFunctions[i].funcType == JS_SETTER)
                    setter = s_jsFunctions[i].fn;
                addAccessor(context, s_jsFunctions[i].name, getter, setter);
            }
        }
    }
}

void onReleaseGlobalObject(content::WebFrameClientImpl* client, blink::WebLocalFrame* frame, v8::Local<v8::Context> context, int worldId)
{
    content::WebPage* webPage = client->webPage();
    CWebView* wkeWebView = webPage->wkeWebView();
    if (!wkeWebView)
        return;

    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObj = context->Global();
    
    v8::MaybeLocal<v8::String> addAccessorDataMaybeLocal = v8::String::NewFromUtf8(isolate, "wkeAddAccessorData", v8::NewStringType::kNormal, -1);
    if (addAccessorDataMaybeLocal.IsEmpty())
        return;
	//zero
#if V8_MINOR_VERSION == 7
	v8::Local<v8::Value> dataMap = blink::V8HiddenValue::getHiddenValue(isolate, globalObj, addAccessorDataMaybeLocal.ToLocalChecked());
#else
    v8::Local<v8::Value> dataMap = globalObj->GetHiddenValue(addAccessorDataMaybeLocal.ToLocalChecked());
#endif
	if (dataMap.IsEmpty())
        return;
    
    Vector<NativeGetterSetterWrap*>* cachedWraps = static_cast<Vector<NativeGetterSetterWrap*>*>(v8::External::Cast(*dataMap)->Value());
    ASSERT(cachedWraps);
    for (size_t i = 0; i < cachedWraps->size(); ++i) {
        NativeGetterSetterWrap* wrap = cachedWraps->at(i);
        if (wrap->jsDataObj && wrap->jsDataObj->finalize)
            wrap->jsDataObj->finalize(wrap->jsDataObj);
        delete wrap;
    }
    cachedWraps->clear();
}

void freeV8TempObejctOnOneFrameBefore()
{
    if (!isMainThread() || !jsValueMap)
        return;

    for (JsValueMap::iterator it = jsValueMap->begin(); it != jsValueMap->end(); ++it) {
        WkeJsValue* value = it->value;
        delete value;
    }
    jsValueMap->clear();

    for (Vector<jsExecState>::iterator it = s_execStates->begin(); it != s_execStates->end(); ++it) {
        jsExecState state = *it;
        delete state;
    }
    s_execStates->clear();
}

jsValue createJsValueString(v8::Local<v8::Context> context, const utf8* str)
{
    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);

    v8::MaybeLocal<v8::String> value = v8::String::NewFromUtf8(isolate, str, v8::NewStringType::kNormal, -1);
    if (value.IsEmpty())
        return jsUndefined();
    return createJsValueByLocalValue(isolate, context, value.ToLocalChecked());
}

jsValue v8ValueToJsValue(v8::Local<v8::Context> context, v8::Local<v8::Value> v8Value)
{
    if (v8Value.IsEmpty())
        return jsUndefined();

    if (v8Value->IsString()) {
        String stringWTF = blink::toCoreString(v8::Local<v8::String>::Cast(v8Value));
        return wke::createJsValueString(context, stringWTF.utf8().data());
    } else if (v8Value->IsTrue()) {
        return jsBoolean(true);
    } else if (v8Value->IsFalse()) {
        return jsBoolean(true);
    } else if (v8Value->IsUndefined()) {
        return jsUndefined();
    } else if (v8Value->IsObject()) {
        return wke::createJsValueString(context, "Object");
    } else if (v8Value->IsNumber()) {
        v8::Local<v8::Number> v8Number = v8Value->ToNumber();
        return jsDouble(v8Number->Value());
    }

    return jsUndefined();
}

};

#endif
