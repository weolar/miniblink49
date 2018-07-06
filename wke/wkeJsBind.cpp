#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
//////////////////////////////////////////////////////////////////////////
#define BUILDING_wke

#include <config.h>
#include "v8.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8StringResource.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8Binding.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8RecursionScope.h"
#include "third_party/WebKit/Source/core/frame/LocalDOMWindow.h"
#include "third_party/WebKit/Source/core/frame/LocalFrame.h"
#include "third_party/WebKit/Source/core/page/ChromeClient.h"
#include "third_party/WebKit/Source/platform/UserGestureIndicator.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
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

    v8::Local<v8::Value> accessorSetterArg;

private:
    JsExecStateInfo()
    {
        isolate = nullptr;
        args = nullptr;
        accessorSetterArg.Clear();
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
        wkeJsValueDouble,
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
        refCount = 0;
    }

    ~WkeJsValue()
    {
        value.Reset();
        context.Reset();
    }

    static void firstJsObjectWeakCallback(const v8::WeakCallbackInfo<WkeJsValue>& data)
    {
        WkeJsValue* self = data.GetParameter();
        delete self;
    }

    v8::Isolate* isolate;
    v8::Persistent<v8::Value> value;
    v8::Persistent<v8::Context> context;

    Type type;
    int intVal;
    double doubleVal;
    bool boolVal;
    WTF::CString stringVal;

    int refCount;
};

typedef WTF::HashMap<jsValue, WkeJsValue*> JsValueMap;
static JsValueMap* s_jsValueMap = nullptr;

static JsValueMap::iterator findJsValueMap(jsValue value)
{
    if (0 == value)
        return s_jsValueMap->end();
    JsValueMap::iterator it = s_jsValueMap->find(value);
    return it;
}

static void freeJsValue(jsValue value)
{
    if (0 == value)
        return;
    JsValueMap::iterator it = s_jsValueMap->find(value);
    if (it == s_jsValueMap->end())
        return;

    WkeJsValue* v = it->value;
    delete v;
    s_jsValueMap->remove(it);
}

static bool isJsValueValid(jsValue value)
{
    if (0 == value)
        return false;

    return (s_jsValueMap->contains(value));
}

static v8::Local<v8::Value> getV8Value(jsValue v, v8::Local<v8::Context> context)
{
    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return v8::Local<v8::Value>(); // v8::Undefined(v8::Isolate::GetCurrent());

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueInt == wkeValue->type) {
        v8::Local<v8::Integer> out = v8::Integer::New(v8::Isolate::GetCurrent(), wkeValue->intVal);
        return out;
    } else if (WkeJsValue::wkeJsValueDouble == wkeValue->type) {
        v8::Local<v8::Number> out = v8::Number::New(v8::Isolate::GetCurrent(), wkeValue->doubleVal);
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
    v8::Local<v8::Value> out = v8::Local<v8::Value>::New(isolate, wkeValue->value);
    return out;
}

static __int64 s_handleCount = 0;

static jsValue createJsValueByLocalValue2(
    v8::Isolate* isolate, 
    v8::Local<v8::Context> context, 
    v8::Local<v8::Value> value,
    WkeJsValue** outWkeJsValue,
    bool isAutoGC
    )
{
    s_handleCount++;

    WkeJsValue* out = new WkeJsValue();
    out->isolate = isolate;
    out->value.Reset(isolate, value);

    out->type = WkeJsValue::wkeJsValueV8Value;
    out->context.Reset(isolate, context);
    if (!isAutoGC)
        out->refCount++;

    if (outWkeJsValue)
        *outWkeJsValue = out;

    s_jsValueMap->add(s_handleCount, out);
    return s_handleCount;
}

static jsValue createJsValueByLocalValue(v8::Isolate* isolate, v8::Local<v8::Context> context, v8::Local<v8::Value> value)
{
    return createJsValueByLocalValue2(isolate, context, value, nullptr, true);
}

static jsValue createEmptyJsValue(WkeJsValue** out)
{
    s_handleCount++;

    *out = new WkeJsValue();
    s_jsValueMap->add(s_handleCount, *out);
    return s_handleCount;
}

int jsArgCount(jsExecState es)
{
    if (!s_execStates || !s_execStates->contains(es))
        return 0;

    if (!es->accessorSetterArg.IsEmpty())
        return 1;

    if (!es || !es->args)
        return 0;
    return es->args->Length();
}

jsType jsArgType(jsExecState es, int argIdx)
{
    return jsTypeOf(jsArg(es, argIdx));
}

static jsValue jsArgImpl(jsExecState es, v8::Local<v8::Value> value)
{
    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    return createJsValueByLocalValue(es->isolate, context, value);
}

jsValue jsArg(jsExecState es, int argIdx)
{
    if (!s_execStates || !s_execStates->contains(es))
        return jsUndefined();

    if (!es->accessorSetterArg.IsEmpty()) {
        if (0 != argIdx)
            return jsUndefined();

        return jsArgImpl(es, es->accessorSetterArg);
    }

    if (!es || !es->args || argIdx >= es->args->Length() || es->context.IsEmpty())
        return jsUndefined();

    v8::Local<v8::Value> value =(*es->args)[argIdx];
    return jsArgImpl(es, value);
}

jsType jsTypeOf(jsValue v)
{
    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return JSTYPE_UNDEFINED;

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueInt == wkeValue->type || WkeJsValue::wkeJsValueDouble == wkeValue->type)
        return JSTYPE_NUMBER;
    else if (WkeJsValue::wkeJsValueBool == wkeValue->type)
        return JSTYPE_BOOLEAN;
    else if (WkeJsValue::wkeJsValueString == wkeValue->type)
        return JSTYPE_STRING;
    else if (WkeJsValue::wkeJsValueUndefined == wkeValue->type)
        return JSTYPE_UNDEFINED;
    else if (WkeJsValue::wkeJsValueNull == wkeValue->type)
        return JSTYPE_NULL;

    v8::Isolate* isolate = wkeValue->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, wkeValue->context);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);

    if (value.IsEmpty())
        return JSTYPE_NULL;

    if (value->IsUndefined())
        return JSTYPE_UNDEFINED;

    if (value->IsBoolean())
        return JSTYPE_BOOLEAN;

    if (value->IsNumber())
        return JSTYPE_NUMBER;

    if (value->IsString())
        return JSTYPE_STRING;

    if (value->IsArray())
        return JSTYPE_ARRAY;

    if (value->IsFunction())
        return JSTYPE_FUNCTION;

    if (value->IsObject())
        return JSTYPE_OBJECT;
    
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

bool jsIsArray(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_ARRAY ? true : false;
}

bool jsIsNull(jsValue v)
{
    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return false;

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value != wkeValue->type)
        return wkeValue->type == WkeJsValue::wkeJsValueNull;

    v8::Isolate* isolate = wkeValue->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);

    return value->IsNull();
}

// bool jsIsArray(jsValue v)
// {
//     JsValueMap::iterator it = findJsValueMap(v);
//     if (it == s_jsValueMap->end())
//         return false;
// 
//     WkeJsValue* wkeValue = it->value;
//     if (WkeJsValue::wkeJsValueV8Value != wkeValue->type)
//         return false;
// 
//     v8::Isolate* isolate = wkeValue->isolate;
//     v8::HandleScope handleScope(isolate);
//     v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
//     return value->IsArray();
// 
//     return false;
// }

bool jsIsTrue(jsValue v)
{
    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return false;

    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueBool == wkeValue->type)
        return wkeValue->boolVal;
    else if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        v8::Isolate* isolate = wkeValue->isolate;
        v8::HandleScope handleScope(isolate);
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

    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return 0;
    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        v8::Isolate* isolate = wkeValue->isolate;
        v8::HandleScope handleScope(isolate);
        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        return value->ToInt32()->Value();
    } else if (WkeJsValue::wkeJsValueInt == wkeValue->type) {
        return wkeValue->intVal;
    } else if (WkeJsValue::wkeJsValueDouble == wkeValue->type) {
        return (int)wkeValue->doubleVal;
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

    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return 0.0;
    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        v8::Isolate* isolate = wkeValue->isolate;
        v8::HandleScope handleScope(isolate);
        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        return value->ToNumber()->Value();
    } else if (WkeJsValue::wkeJsValueInt == wkeValue->type) {
        return (double)wkeValue->intVal;
    } else if (WkeJsValue::wkeJsValueDouble == wkeValue->type)
        return wkeValue->doubleVal;
    return 0.0;
}

bool jsToBoolean(jsExecState es, jsValue v)
{
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return false;

    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return false;
    WkeJsValue* wkeValue = it->value;
    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        v8::Isolate* isolate = wkeValue->isolate;
        v8::HandleScope handleScope(isolate);
        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
        return value->ToBoolean()->Value();
    } else if (WkeJsValue::wkeJsValueBool == wkeValue->type)
        return wkeValue->boolVal;

    return false;
}

const wchar_t* jsToTempStringW(jsExecState es, jsValue v)
{
    const utf8* utf8String = jsToTempString(es, v);
    Vector<UChar> utf16 = WTF::ensureUTF16UChar(String(utf8String), false);
    if (0 == utf16.size())
        return L"";

    return wke::createTempWCharString(utf16.data(), utf16.size());
}

const utf8* jsToTempString(jsExecState es, jsValue v)
{
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return "";

    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return "";

    WkeJsValue* wkeValue = it->value;
    
    Vector<char> sharedStringBuffer;
    if (WkeJsValue::wkeJsValueV8Value == wkeValue->type) {
        if (wkeValue->value.IsEmpty())
            return "";

        v8::Isolate* isolate = es->isolate;
        v8::HandleScope handleScope(isolate);
        v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, es->context);
        v8::Context::Scope contextScope(context);

        v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeValue->isolate, wkeValue->value);
//         if (!value->IsString())
//             return "";

        v8::Local<v8::String> stringValue = value->ToString();
        String stringWTF = blink::v8StringToWebCoreString<String>(stringValue, blink::DoNotExternalize);

        sharedStringBuffer = WTF::ensureStringToUTF8(stringWTF, false);
    } else if (WkeJsValue::wkeJsValueString == wkeValue->type) {
        if (0 == wkeValue->stringVal.length() || 1 == wkeValue->stringVal.length())
            return "";
        sharedStringBuffer.append(wkeValue->stringVal.data(), wkeValue->stringVal.length() - 1);
    } else if (WkeJsValue::wkeJsValueInt == wkeValue->type) {
        WTF::CString intVal = String::format("%d", wkeValue->intVal).utf8();
        int len = intVal.length();
        sharedStringBuffer.append(intVal.data(), len);
    } else if (WkeJsValue::wkeJsValueDouble == wkeValue->type) {
        WTF::CString floatVal = String::format("%f", (float)(wkeValue->doubleVal)).utf8();
        int len = floatVal.length();
        sharedStringBuffer.append(floatVal.data(), len);
    } else if (WkeJsValue::wkeJsValueBool == wkeValue->type) {
        wkeValue->boolVal ?
            sharedStringBuffer.append("true", 4) :
            sharedStringBuffer.append("false", 5);
    } else if (WkeJsValue::wkeJsValueNull == wkeValue->type) {
        sharedStringBuffer.append("null", 4);
    } else if (WkeJsValue::wkeJsValueUndefined == wkeValue->type) {
        sharedStringBuffer.append("undefined", 9);
    }

    if (0 == sharedStringBuffer.size())
        return "";

    return wke::createTempCharString(sharedStringBuffer.data(), sharedStringBuffer.size());
}

const utf8* jsToString(jsExecState es, jsValue v)
{
    return jsToTempString(es, v);
}

const wchar_t* jsToStringW(jsExecState es, jsValue v)
{
    return jsToTempStringW(es, v);
}

void* jsToV8Value(jsExecState es, jsValue v)
{
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return nullptr;

    JsValueMap::iterator it = findJsValueMap(v);
    if (it == s_jsValueMap->end())
        return nullptr;

    WkeJsValue* wkeValue = it->value;

    if (WkeJsValue::wkeJsValueV8Value != wkeValue->type)
        return nullptr;
    return &wkeValue->value;
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
    return jsDouble(f);
}

jsValue jsDouble(double d)
{
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueDouble;
    out->doubleVal = d;
    return ret;
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

jsValue jsArrayBuffer(jsExecState es, char * buffer, size_t size)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Handle<v8::ArrayBuffer> value = v8::ArrayBuffer::New(es->isolate, size);
    memcpy(value->GetContents().Data(), buffer, size);

    if (value.IsEmpty())
        return jsUndefined();

    return createJsValueByLocalValue(es->isolate, context, value);
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
    Vector<UChar> buf = WTF::ensureUTF16UChar(s, true);
    return jsEvalW(es, buf.data());
}

jsValue jsEvalW(jsExecState es, const wchar_t* str)
{
    return jsEvalExW(es, str, true);
}

jsValue jsEvalExW(jsExecState es, const wchar_t* str, bool isInClosure)
{
    if (!s_execStates || !es || !s_execStates->contains(es) || !es->isolate || es->context.IsEmpty() || !str)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    String codeString(str);
    if (codeString.startsWith("javascript:", WTF::TextCaseInsensitive))
        codeString.remove(0, sizeof("javascript:") - 1);
    if (isInClosure) {
        codeString.insert("(function(){", 0);
        codeString.append("})();");
    }

    v8::Isolate* isolate = es->isolate;
    blink::V8RecursionScope::MicrotaskSuppression microtaskSuppression(isolate);

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::MaybeLocal<v8::String> source = v8::String::NewFromUtf8(isolate, WTF::ensureStringToUTF8(codeString, true).data(), v8::NewStringType::kNormal);
    if (source.IsEmpty())
        return jsUndefined();
    v8::Local<v8::Script> script = v8::Script::Compile(source.ToLocalChecked());
    v8::TryCatch trycatch;
    v8::Local<v8::Value> result = script->Run();

    //return wke::v8ValueToJsValue(context, result);
    return createJsValueByLocalValue(isolate, context, result);
}

jsValue jsCall(jsExecState es, jsValue func, jsValue thisValue, jsValue* args, int argCount)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);
    blink::UserGestureIndicator gestureIndicator(blink::DefinitelyProcessingUserGesture);

    v8::Local<v8::Value>* argv = new v8::Local<v8::Value>[argCount];
    for (int i = 0; i < argCount; ++i) {
        argv[i] = getV8Value(args[i], context);
    }

    v8::Local<v8::Value> cbValue = getV8Value(func, context);
    if (cbValue.IsEmpty() || !cbValue->IsFunction())
        return jsUndefined();

    v8::Function* cb = v8::Function::Cast(*cbValue);

    v8::Local<v8::Value> thisValueV8 = getV8Value(thisValue, context);

    blink::V8RecursionScope::MicrotaskSuppression recursionScope(isolate);
    
    if (thisValueV8.IsEmpty() || thisValueV8->IsUndefined())
        thisValueV8 = context->Global();
    v8::MaybeLocal<v8::Value> ret = cb->Call(context, thisValueV8, argCount, argv);

    delete[] argv;

    if (ret.IsEmpty())
        return jsUndefined();

    v8::Local<v8::Value> v8Ret = ret.ToLocalChecked();
    return createJsValueByLocalValue(isolate, context, v8Ret);
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

void jsDeleteObjectProp(jsExecState es, jsValue object, const char* prop)
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

    v8::Local<v8::Object> obj = objectLocal->ToObject();
    v8::MaybeLocal<v8::String> propV8 = v8::String::NewFromUtf8(isolate, prop, v8::NewStringType::kNormal, -1);
    if (propV8.IsEmpty())
        return;
    obj->Delete(propV8.ToLocalChecked());
}

bool jsIsValidExecState(jsExecState es)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return false;
    return true;
}

bool jsIsJsValueValid(jsExecState es, jsValue object)
{
    if (!jsIsValidExecState(es))
        return false;

    JsValueMap::iterator it = findJsValueMap(object);
    if (it == s_jsValueMap->end())
        return false;

    return true;
}

jsValue jsGetAt(jsExecState es, jsValue object, int index)
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
    if (value.IsEmpty() || !value->IsArray())
        return jsUndefined();

    v8::Local<v8::Array> obj = v8::Local<v8::Array>::Cast(value);
    v8::MaybeLocal<v8::Value> retValue = obj->Get(context, index);
    if (retValue.IsEmpty())
        return jsUndefined();

    v8::TryCatch tryCatch;
    tryCatch.SetVerbose(true);
    if (tryCatch.HasCaught() || retValue.IsEmpty())
        return jsUndefined();

    return createJsValueByLocalValue(isolate, context, retValue.ToLocalChecked());
}

void jsSetAt(jsExecState es, jsValue object, int index, jsValue value)
{
//     JSC::JSValue o = JSC::JSValue::decode(object);
//     JSC::JSValue v = JSC::JSValue::decode(value);
//     o.put((JSC::ExecState*)es, index, v);

    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return;
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> v8Object = getV8Value(object, context);
    if (v8Object.IsEmpty() || !v8Object->IsArray())
        return;

    v8::Local<v8::Value> v8Value = getV8Value(value, context);
    if (v8Value.IsEmpty())
        return;

    v8::Local<v8::Array> obj = v8::Local<v8::Array>::Cast(v8Object);
    v8::Maybe<bool> result = obj->Set(context, index, v8Value);

    return;
}

int jsGetLength(jsExecState es, jsValue object)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return 0;
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> value = getV8Value(object, context);
    if (value.IsEmpty() || !value->IsArray())
        return 0;

    v8::Local<v8::Array> obj = v8::Local<v8::Array>::Cast(value);
    uint32_t retValue = obj->Length();

    return retValue;
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
    RELEASE_ASSERT(!es->context.IsEmpty());

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Object> globalObj = context->Global();

    v8::MaybeLocal<v8::String> nameMaybeLocal = v8::String::NewFromUtf8(isolate, "wkeWebViewToV8Context", v8::NewStringType::kNormal, -1);
    if (nameMaybeLocal.IsEmpty())
        return nullptr;

    v8::Local<v8::Value> wkeWebViewV8 = blink::V8HiddenValue::getHiddenValue(isolate, globalObj, nameMaybeLocal.ToLocalChecked());
    ASSERT(!wkeWebViewV8.IsEmpty());
    wke::CWebView* webView = static_cast<wke::CWebView*>(v8::External::Cast(*wkeWebViewV8)->Value());
    return webView;
}

void jsGC()
{
    //WebCore::gcController().garbageCollectNow();
}

bool jsAddRef(jsExecState es, jsValue val)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return false;
    RELEASE_ASSERT(!es->context.IsEmpty());

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    JsValueMap::iterator it = findJsValueMap(val);
    if (it == s_jsValueMap->end())
        return false;

    WkeJsValue* wkeValue = it->value;

    if (WkeJsValue::wkeJsValueV8Value != wkeValue->type)
        return false;

    wkeValue->refCount++;

    wke::CWebView* webview = jsGetWebView(es);
    webview->getPersistentJsValue().insert(val);
    //wkeValue->value.SetWeak<WkeJsValue>(wkeValue, &WkeJsValue::firstJsObjectWeakCallback, v8::WeakCallbackType::kInternalFields);

    return true;
}

static void deletePersistentJsValue(jsValue v);

bool jsReleaseRef(jsExecState es, jsValue val)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return false;
    RELEASE_ASSERT(!es->context.IsEmpty());

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    JsValueMap::iterator it = findJsValueMap(val);
    if (it == s_jsValueMap->end())
        return false;

    WkeJsValue* wkeValue = it->value;

    if (WkeJsValue::wkeJsValueV8Value != wkeValue->type)
        return false;

    wkeValue->refCount--;
    RELEASE_ASSERT(wkeValue->refCount >= 0);
    if (0 != wkeValue->refCount)
        return false;

    RELEASE_ASSERT(WkeJsValue::wkeJsValueV8Value == wkeValue->type);
    wkeValue->value.Reset();
    delete wkeValue;
    s_jsValueMap->remove(it);

    wke::CWebView* webview = jsGetWebView(es);
    std::set<jsValue>& persistentJsValues = webview->getPersistentJsValue();
    std::set<jsValue>::iterator iter = persistentJsValues.find(val);
    RELEASE_ASSERT(iter != persistentJsValues.end());
    persistentJsValues.erase(iter);

    return true;
}

struct AddFunctionInfo {
    AddFunctionInfo(wkeJsNativeFunction nativeFunction, void* param) {
        this->nativeFunction = nativeFunction;
        this->param = param;
    }

    wkeJsNativeFunction nativeFunction;
    void* param;
};

static void functionCallbackImpl(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    AddFunctionInfo* addFunctionInfo = static_cast<AddFunctionInfo*>(v8::External::Cast(*info.Data())->Value());
    wkeJsNativeFunction func = addFunctionInfo->nativeFunction;

    JsExecStateInfo* execState = JsExecStateInfo::create();
    execState->args = &info;
    execState->isolate = isolate;
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    execState->context.Reset(isolate, context);

    wke::AutoDisableFreeV8TempObejct autoDisableFreeV8TempObejct;
    jsValue retVal = func(execState, addFunctionInfo->param);

    v8::Local<v8::Value> rv = getV8Value(retVal, context);
    info.GetReturnValue().Set(rv);
}

static void addFunction(v8::Local<v8::Context> context, const char* name, wkeJsNativeFunction nativeFunction, void* param, unsigned int argCount)
{
    v8::Isolate* isolate = context->GetIsolate();
    if (!isolate->InContext())
        return;
    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Object> object = context->Global();
    v8::Local<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New(isolate);    
    v8::Local<v8::Value> data = v8::External::New(isolate, new AddFunctionInfo(nativeFunction, param));

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
    wkeJsNativeFunction getter;
    void* getterParam;

    wkeJsNativeFunction setter;
    void* setterParam;

    jsData* jsDataObj;

    void set(wkeJsNativeFunction getter, void* getterParam, wkeJsNativeFunction setter, void* setterParam)
    {
        this->getter = getter;
        this->getterParam = getterParam;
        this->setter = setter;
        this->setterParam = setterParam;
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
        this->m_persistentValue = 0;
    }

    ~NativeGetterSetterWrap() {}

    static void AccessorGetterCallbackImpl(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        v8::Isolate* isolate = info.GetIsolate();
        NativeGetterSetterWrap* getterSetter = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*info.Data())->Value());

        jsExecState execState = JsExecStateInfo::create();
        execState->args = nullptr;
        execState->isolate = isolate;
        execState->context.Reset(isolate, isolate->GetCurrentContext());

        wke::AutoDisableFreeV8TempObejct autoDisableFreeV8TempObejct;
        jsValue retJsValue = getterSetter->getter(execState, getterSetter->getterParam);

        info.GetReturnValue().Set(getV8Value(retJsValue, isolate->GetCurrentContext()));
    }

    static void AccessorSetterCallbackImpl(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        v8::Isolate* isolate = info.GetIsolate();

        NativeGetterSetterWrap* getterSetter = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*info.Data())->Value());

        jsExecState execState = JsExecStateInfo::create();
        execState->args = nullptr;
        execState->accessorSetterArg = value;
        execState->isolate = isolate;
        execState->context.Reset(isolate, isolate->GetCurrentContext());

        wke::AutoDisableFreeV8TempObejct autoDisableFreeV8TempObejct;
        getterSetter->setter(execState, getterSetter->setterParam);

        info.GetReturnValue().SetUndefined();
    }

    static NativeGetterSetterWrap* createWrapAndAddToGlobalObjForRelease(v8::Isolate* isolate, v8::Local<v8::Object>& globalObj)
    {
        v8::MaybeLocal<v8::String> addAccessorDataMaybeLocal = v8::String::NewFromUtf8(isolate, "wkeAddAccessorData", v8::NewStringType::kNormal, -1);
        if (addAccessorDataMaybeLocal.IsEmpty())
            return nullptr;

        Vector<NativeGetterSetterWrap*>* cachedWraps = nullptr;
        
        v8::Local<v8::Value> dataMap = blink::V8HiddenValue::getHiddenValue(isolate, globalObj, addAccessorDataMaybeLocal.ToLocalChecked());
        if (dataMap.IsEmpty()) {
            cachedWraps = new Vector<NativeGetterSetterWrap*>();
            dataMap = v8::External::New(isolate, cachedWraps);

            blink::V8HiddenValue::setHiddenValue(isolate, globalObj, addAccessorDataMaybeLocal.ToLocalChecked(), dataMap);
        } else {
            cachedWraps = static_cast<Vector<NativeGetterSetterWrap*>*>(v8::External::Cast(*dataMap)->Value());
        }
        NativeGetterSetterWrap* wrap = new NativeGetterSetterWrap();
        cachedWraps->append(wrap);

        wrap->m_cachedWraps = cachedWraps;
        return wrap;
    }

    static void secondJsObjectWeakCallback(const v8::WeakCallbackInfo<NativeGetterSetterWrap>& data)
    {
        NativeGetterSetterWrap* self = data.GetParameter();
        self->gc();
    }

    void gc()
    {
        for (size_t i = 0; i < m_cachedWraps->size(); ++i) {
            NativeGetterSetterWrap* wrap = m_cachedWraps->at(i);
            if (wrap != this)
                continue;

            if (jsDataObj && jsDataObj->finalize)
                jsDataObj->finalize(jsDataObj);

            m_cachedWraps->remove(i);

            delete this;
            return;
        }
    }

    static void firstJsObjectWeakCallback(const v8::WeakCallbackInfo<NativeGetterSetterWrap>& data)
    {
        NativeGetterSetterWrap* self = data.GetParameter();
        freeJsValue(self->m_persistentValue);
        //data.SetSecondPassCallback(secondJsObjectWeakCallback);
        self->gc();       
    }

    void set(jsValue persistentValue) { m_persistentValue = persistentValue; }
    jsValue getPersistentValue() { return m_persistentValue; }


private:
    jsValue m_persistentValue;
    Vector<NativeGetterSetterWrap*>* m_cachedWraps;
};

static void addAccessor(v8::Local<v8::Context> context, const char* name, wkeJsNativeFunction getter, void* getterParam, wkeJsNativeFunction setter, void* setterParam)
{
    if (!getter && !setter)
        return;

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
    wrap->set(getter, getterParam, setter, setterParam);
    v8::Local<v8::Value> data = v8::External::New(isolate, wrap);

    v8::AccessorNameGetterCallback v8Getter = getter ? &NativeGetterSetterWrap::AccessorGetterCallbackImpl : nullptr;
    v8::AccessorNameSetterCallback v8Setter = setter ? &NativeGetterSetterWrap::AccessorSetterCallbackImpl : nullptr;

    bool setOk = globalObj->SetAccessor(nameMaybeLocal.ToLocalChecked(), v8Getter, v8Setter, data, (v8::AccessControl)(v8::ALL_CAN_READ | v8::ALL_CAN_WRITE));
}


#define MAX_NAME_LENGTH 32
#define MAX_FUNCTION_COUNT 1024

#define JS_FUNC   (0)
#define JS_GETTER (1)
#define JS_SETTER (2)

struct jsFunctionInfo {
    jsFunctionInfo() {
        memset(name, 0, MAX_NAME_LENGTH);
        fn = nullptr;
        param = nullptr;
        settet = nullptr;
        setterParam = nullptr;
        gettet = nullptr;
        getterParam = nullptr;
        argCount = 0;
        funcType = JS_FUNC;
    }

    char name[MAX_NAME_LENGTH];
    wkeJsNativeFunction fn;
    void* param;

    wkeJsNativeFunction settet;
    void* setterParam;

    wkeJsNativeFunction gettet;
    void* getterParam;

    unsigned int argCount;
    unsigned int funcType;
};

static Vector<jsFunctionInfo>* s_jsFunctionsPtr = nullptr;

static jsValue wkeJsBindFunctionWrap(jsExecState es, void* param)
{
    jsNativeFunction fn = (jsNativeFunction)param;
    return fn(es);
}

void jsBindFunction(const char* name, jsNativeFunction fn, unsigned int argCount)
{
    wkeJsBindFunction(name, wkeJsBindFunctionWrap, fn, argCount);
}

static void wkeJsBindSetterGetter(const char* name, wkeJsNativeFunction fn, void* param, unsigned int funcType)
{
    if (!s_jsFunctionsPtr)
        s_jsFunctionsPtr = new Vector<jsFunctionInfo>();
    Vector<jsFunctionInfo>& s_jsFunctions = *s_jsFunctionsPtr;

    for (unsigned int i = 0; i < s_jsFunctions.size(); ++i) {
        if (strncmp(name, s_jsFunctions[i].name, MAX_NAME_LENGTH) == 0) {
            JS_GETTER == funcType ? s_jsFunctions[i].gettet = fn : s_jsFunctions[i].settet = fn;
            return;
        }
    }

    jsFunctionInfo funcInfo;
    strncpy(funcInfo.name, name, MAX_NAME_LENGTH - 1);
    funcInfo.name[MAX_NAME_LENGTH - 1] = '\0';
    JS_GETTER == funcType ? funcInfo.gettet = fn : funcInfo.settet = fn;
    JS_GETTER == funcType ? funcInfo.getterParam = param : funcInfo.setterParam = param;

    funcInfo.argCount = 0;
    funcInfo.funcType |= funcType;

    s_jsFunctions.append(funcInfo);
}

void jsBindGetter(const char* name, jsNativeFunction fn)
{
    wkeJsBindSetterGetter(name, wkeJsBindFunctionWrap, fn, JS_GETTER);
}

void jsBindSetter(const char* name, jsNativeFunction fn)
{
    wkeJsBindSetterGetter(name, wkeJsBindFunctionWrap, fn, JS_SETTER);
}

void wkeJsBindFunction(const char* name, wkeJsNativeFunction fn, void* param, unsigned int argCount)
{
    if (!s_jsFunctionsPtr)
        s_jsFunctionsPtr = new Vector<jsFunctionInfo>();
    Vector<jsFunctionInfo>& s_jsFunctions = *s_jsFunctionsPtr;

    for (unsigned int i = 0; i < s_jsFunctions.size(); ++i) {
        if (s_jsFunctions[i].funcType == JS_FUNC && strncmp(name, s_jsFunctions[i].name, MAX_NAME_LENGTH) == 0) {
            s_jsFunctions[i].fn = fn;
            s_jsFunctions[i].param = param;
            s_jsFunctions[i].argCount = argCount;
            return;
        }
    }

    jsFunctionInfo funcInfo;
    strncpy(funcInfo.name, name, MAX_NAME_LENGTH - 1);
    funcInfo.name[MAX_NAME_LENGTH - 1] = '\0';
    funcInfo.fn = fn;
    funcInfo.param = param;
    funcInfo.argCount = argCount;
    funcInfo.funcType = JS_FUNC;

    s_jsFunctions.append(funcInfo);
}

void wkeJsBindGetter(const char* name, wkeJsNativeFunction fn, void* param)
{
    wkeJsBindSetterGetter(name, fn, param, JS_GETTER);
}

void wkeJsBindSetter(const char* name, wkeJsNativeFunction fn, void* param)
{
    wkeJsBindSetterGetter(name, fn, param, JS_GETTER);
}

jsValue js_outputMsg(jsExecState es, void* param)
{
    //ASSERT(jsArgCount(es) == 1);
    //ASSERT(jsArgType(es, 0) == JSTYPE_STRING);

    jsValue value = jsArg(es, 0);
    OutputDebugStringW(jsToTempStringW(es, value));

    return jsUndefined();
}

jsValue js_getWebViewName(jsExecState es, void* param)
{
    wkeWebView webView = jsGetWebView(es);
    return jsString(es, webView->name());
}

jsValue js_setWebViewName(jsExecState es, void* param)
{
    const char* name = jsToTempString(es, jsArg(es, 0));
    wkeWebView webView = jsGetWebView(es);
    webView->setName(name);

    return jsUndefined();
}

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

void* g_testObject = nullptr;

jsValue jsObject(jsExecState es, jsData* data)
{
    if (!s_execStates || !s_execStates->contains(es) || !es || es->context.IsEmpty())
        return jsUndefined();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);

    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObj = context->Global();

    v8::Local<v8::ObjectTemplate> objTemplate = v8::ObjectTemplate::New(isolate);
    
    NativeGetterSetterWrap* wrap = NativeGetterSetterWrap::createWrapAndAddToGlobalObjForRelease(isolate, globalObj);

    wrap->set(data);
    objTemplate->SetNamedPropertyHandler(namedPropertyGetterCallback, namedPropertySetterCallback, nullptr, nullptr, nullptr, v8::External::New(isolate, wrap));
    
    WkeJsValue* wkeJsValue = nullptr;
    v8::Local<v8::Object> objInst = objTemplate->NewInstance(context).ToLocalChecked();
    jsValue retValue = createJsValueByLocalValue2(isolate, context, objInst, &wkeJsValue, false);
    wkeJsValue->value.SetWeak<NativeGetterSetterWrap>(wrap, &NativeGetterSetterWrap::firstJsObjectWeakCallback, v8::WeakCallbackType::kInternalFields);
    wrap->set(retValue);

    return retValue;
}

void jsFunctionConstructCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
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

    int argsLength = args.Length();
    jsValue* argv = new jsValue[argsLength];
    for (int i = 0; i < argsLength; ++i)
        argv[i] = createJsValueByLocalValue(isolate, context, args[i]);

    wke::AutoDisableFreeV8TempObejct autoDisableFreeV8TempObejct;
    jsValue retWkeValue = wrap->jsDataObj->callAsFunction(execState, wkeData, argv, argsLength);
    args.GetReturnValue().Set(getV8Value(retWkeValue, context));
}

jsValue jsFunction(jsExecState es, jsData* data)
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

jsData* jsGetData(jsExecState es, jsValue value)
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

    v8::Local<v8::Value> wkeWebViewV8 = blink::V8HiddenValue::getHiddenValue(isolate, globalObj, nameMaybeLocal.ToLocalChecked());
    ASSERT(wkeWebViewV8.IsEmpty());

    CWebView* wkeWebView = webPage->wkeWebView();
    ASSERT(wkeWebView);
    wkeWebViewV8 = v8::External::New(isolate, wkeWebView);

    blink::V8HiddenValue::setHiddenValue(isolate, globalObj, nameMaybeLocal.ToLocalChecked(), wkeWebViewV8);

}

jsExecState createTempExecStateByV8Context(v8::Local<v8::Context> context)
{
    if (context.IsEmpty())
        return nullptr;
    JsExecStateInfo* execState = JsExecStateInfo::create();
    execState->context.Reset(context->GetIsolate(), context);
    execState->isolate = context->GetIsolate();
    return execState;
}

void onCreateGlobalObjectInSubFrame(content::WebFrameClientImpl* client, blink::WebLocalFrame* frame, 
    v8::Local<v8::Context> context, int extensionGroup, int worldId)
{
    content::WebPage* webPage = client->webPage();
    CWebView* wkeWebView = webPage->wkeWebView();
    if (!wkeWebView)
        return;

    v8::Isolate* isolate = context->GetIsolate();
    setWkeWebViewToV8Context(client, context);
}

void onCreateGlobalObjectInMainFrame(content::WebFrameClientImpl* client, blink::WebLocalFrame* frame, v8::Local<v8::Context> context, int extensionGroup, int worldId)
{
    content::WebPage* webPage = client->webPage();
    CWebView* wkeWebView = webPage->wkeWebView();
    if (!wkeWebView)
        return;

    if (!s_jsValueMap)
        s_jsValueMap = new JsValueMap();
    if (!s_execStates)
        s_execStates = new Vector<jsExecState>();
    
    v8::Isolate* isolate = context->GetIsolate();
    setWkeWebViewToV8Context(client, context);

    addFunction(context, "outputMsg", js_outputMsg, nullptr, 1);
    addAccessor(context, "webViewName", js_getWebViewName, nullptr, js_setWebViewName, nullptr);
    
    blink::WebScriptSource injectSource("window.chrome = {app:null, runtime:null}");
    frame->executeScript(injectSource);

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
                addFunction(context, s_jsFunctions[i].name, s_jsFunctions[i].fn, s_jsFunctions[i].param, s_jsFunctions[i].argCount);
            else {
                wkeJsNativeFunction getter = s_jsFunctions[i].gettet;
                void* getterParam = s_jsFunctions[i].getterParam;
                wkeJsNativeFunction setter = s_jsFunctions[i].settet;
                void* setterParam = s_jsFunctions[i].setterParam;
                addAccessor(context, s_jsFunctions[i].name, getter, getterParam, setter, setterParam);
            }
        }
    }
}

static void deletePersistentJsValue(jsValue v)
{
    JsValueMap::iterator iter = findJsValueMap(v);
    if (iter != s_jsValueMap->end()) {
        WkeJsValue* value = iter->value;
        RELEASE_ASSERT(WkeJsValue::wkeJsValueV8Value == value->type);
        value->value.Reset();
        delete value;
    }
    s_jsValueMap->remove(iter);
}

static void deletePersistentJsValues(CWebView* wkeWebView)
{
    std::set<jsValue>& persistentJsValues = wkeWebView->getPersistentJsValue();
    for (std::set<jsValue>::iterator it = persistentJsValues.begin(); it != persistentJsValues.end(); ++it) {
        jsValue v = *it;
        deletePersistentJsValue(v);
    }
    persistentJsValues.clear();
}

void onReleaseGlobalObject(content::WebFrameClientImpl* client, blink::WebLocalFrame* frame, v8::Local<v8::Context> context, int worldId)
{
    content::WebPage* webPage = client->webPage();
    CWebView* wkeWebView = webPage->wkeWebView();
    if (!wkeWebView)
        return;

    if (!frame->parent())
        deletePersistentJsValues(wkeWebView);

    v8::Isolate* isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::Local<v8::Object> globalObj = context->Global();
    
    v8::MaybeLocal<v8::String> addAccessorDataMaybeLocal = v8::String::NewFromUtf8(isolate, "wkeAddAccessorData", v8::NewStringType::kNormal, -1);
    if (addAccessorDataMaybeLocal.IsEmpty())
        return;

    v8::Local<v8::Value> dataMap = blink::V8HiddenValue::getHiddenValue(isolate, globalObj, addAccessorDataMaybeLocal.ToLocalChecked());
    if (dataMap.IsEmpty())
        return;
    
    Vector<NativeGetterSetterWrap*>* cachedWraps = static_cast<Vector<NativeGetterSetterWrap*>*>(v8::External::Cast(*dataMap)->Value());
    ASSERT(cachedWraps);
    for (size_t i = 0; i < cachedWraps->size(); ++i) {
        NativeGetterSetterWrap* wrap = cachedWraps->at(i);
        if (wrap->jsDataObj && wrap->jsDataObj->finalize)
            wrap->jsDataObj->finalize(wrap->jsDataObj);
        
        JsValueMap::iterator it = findJsValueMap(wrap->getPersistentValue());
        if (it != s_jsValueMap->end()) {
            WkeJsValue* wkeJsValue = it->value;
            wkeJsValue->value.Reset();
        }
        delete wrap;
    }
    cachedWraps->clear();
}

AutoDisableFreeV8TempObejct::AutoDisableFreeV8TempObejct()
{
    ++m_disableCount;
}

AutoDisableFreeV8TempObejct::~AutoDisableFreeV8TempObejct()
{
    --m_disableCount;
}

int AutoDisableFreeV8TempObejct::m_disableCount = 0;

void freeV8TempObejctOnOneFrameBefore()
{
    if (!isMainThread() || !s_jsValueMap || AutoDisableFreeV8TempObejct::isDisable())
        return;

    JsValueMap* jsValueMap = nullptr;
    for (JsValueMap::iterator it = s_jsValueMap->begin(); it != s_jsValueMap->end(); ++it) {
        WkeJsValue* value = it->value;
        if (0 == value->refCount) {
            delete value;
            continue;
        }

        if (!jsValueMap)
            jsValueMap = new JsValueMap();
        jsValueMap->add(it->key, value);
    }

    if (jsValueMap) {
        delete s_jsValueMap;
        s_jsValueMap = jsValueMap;
    } else
        s_jsValueMap->clear();

    for (Vector<jsExecState>::iterator it = s_execStates->begin(); it != s_execStates->end(); ++it) {
        jsExecState state = *it;
        delete state;
    } 
    s_execStates->clear();

    freeTempCharStrings();
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
        return wke::createJsValueString(context, WTF::ensureStringToUTF8(stringWTF, true).data());
    } else if (v8Value->IsTrue()) {
        return jsBoolean(true);
    } else if (v8Value->IsFalse()) {
        return jsBoolean(false);
    } else if (v8Value->IsUndefined()) {
        return jsUndefined();
    } else if (v8Value->IsObject()) {
        //return wke::createJsValueString(context, "Object");
        return createJsValueByLocalValue(context->GetIsolate(), context, v8Value);
    } else if (v8Value->IsInt32()) {
        v8::Local<v8::Int32> v8Number = v8Value->ToInt32();
        return jsInt(v8Number->Value());
    } else if (v8Value->IsUint32()) {
        v8::Local<v8::Uint32> v8Number = v8Value->ToUint32();
        return jsInt(v8Number->Value());
    } else if (v8Value->IsNumber()) {
        v8::Local<v8::Number> v8Number = v8Value->ToNumber();
        return jsDouble(v8Number->Value());
    }

    return jsUndefined();
}

};

#endif
