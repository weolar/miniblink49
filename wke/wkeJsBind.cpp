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
#include "content/web_impl_win/WebThreadImpl.h"
#include "wke/wke.h"
#include "wke/wkeJsBind.h"
#include "wke/wkeWebView.h"
#include "wke/wkeUtil.h"

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
    };

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

typedef JsExecStateInfo* jsExecState;
static Vector<jsExecState>* s_execStates = nullptr;

typedef WTF::HashMap<jsValue, WkeJsValue*> JsValueMap;
static JsValueMap* s_jsValueMap = nullptr;

static void ensureStaticVar()
{
    if (!s_jsValueMap)
        s_jsValueMap = new JsValueMap();

    if (!s_execStates)
        s_execStates = new Vector<jsExecState>();
}

JsExecStateInfo* JsExecStateInfo::create()
{
    ensureStaticVar();

    JsExecStateInfo* retVal = new JsExecStateInfo();
    s_execStates->append(retVal);
    return retVal;
}

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

int WKE_CALL_TYPE jsArgCount(jsExecState es)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es))
        return 0;

    if (!es->accessorSetterArg.IsEmpty())
        return 1;

    if (!es || !es->args)
        return 0;
    return es->args->Length();
}

jsType WKE_CALL_TYPE jsArgType(jsExecState es, int argIdx)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    return jsTypeOf(jsArg(es, argIdx));
}

static jsValue jsArgImpl(jsExecState es, v8::Local<v8::Value> value)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    return createJsValueByLocalValue(es->isolate, context, value);
}

jsValue WKE_CALL_TYPE jsArg(jsExecState es, int argIdx)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

jsType WKE_CALL_TYPE jsTypeOf(jsValue v)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

bool WKE_CALL_TYPE jsIsNumber(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_NUMBER ? true : false;
}

bool WKE_CALL_TYPE jsIsString(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_STRING ? true : false;
}

bool WKE_CALL_TYPE jsIsBoolean(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_BOOLEAN ? true : false;
}

bool WKE_CALL_TYPE jsIsObject(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_OBJECT ? true : false;
}

bool WKE_CALL_TYPE jsIsFunction(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_FUNCTION ? true : false;
}

bool WKE_CALL_TYPE jsIsUndefined(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_UNDEFINED ? true : false;
}

bool WKE_CALL_TYPE jsIsArray(jsValue v)
{
    return jsTypeOf(v) == JSTYPE_ARRAY ? true : false;
}

bool WKE_CALL_TYPE jsIsNull(jsValue v)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

bool WKE_CALL_TYPE jsIsTrue(jsValue v)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

        v8::Local<v8::Boolean> boolValue = value->ToBoolean(wkeValue->isolate);
        return boolValue->Value();
    }

    return false;
}

bool WKE_CALL_TYPE jsIsFalse(jsValue v)
{
    return !jsIsTrue(v);
}

int WKE_CALL_TYPE jsToInt(jsExecState es, jsValue v)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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
        return value->ToInt32(isolate)->Value();
    } else if (WkeJsValue::wkeJsValueInt == wkeValue->type) {
        return wkeValue->intVal;
    } else if (WkeJsValue::wkeJsValueDouble == wkeValue->type) {
        return (int)wkeValue->doubleVal;
    }
    return 0;
}

float WKE_CALL_TYPE jsToFloat(jsExecState es, jsValue v)
{
    return (float)jsToDouble(es, v);
}

double WKE_CALL_TYPE jsToDouble(jsExecState es, jsValue v)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

bool WKE_CALL_TYPE jsToBoolean(jsExecState es, jsValue v)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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
        return value->ToBoolean(isolate)->Value();
    } else if (WkeJsValue::wkeJsValueBool == wkeValue->type)
        return wkeValue->boolVal;

    return false;
}

const wchar_t* WKE_CALL_TYPE jsToTempStringW(jsExecState es, jsValue v)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    const utf8* utf8String = jsToTempString(es, v);
    Vector<UChar> utf16 = WTF::ensureUTF16UChar(String(utf8String), false);
    if (0 == utf16.size())
        return L"";

    return wke::createTempWCharString(utf16.data(), utf16.size());
}

const utf8* WKE_CALL_TYPE jsToTempString(jsExecState es, jsValue v)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

        v8::Local<v8::String> stringValue = value->ToString(isolate);
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

const utf8* WKE_CALL_TYPE jsToString(jsExecState es, jsValue v)
{
    return jsToTempString(es, v);
}

const wchar_t* WKE_CALL_TYPE jsToStringW(jsExecState es, jsValue v)
{
    return jsToTempStringW(es, v);
}

void* WKE_CALL_TYPE jsToV8Value(jsExecState es, jsValue v)
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

jsValue WKE_CALL_TYPE jsInt(int n)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueInt;
    out->intVal = n;
    return ret;
}

jsValue WKE_CALL_TYPE jsFloat(float f)
{
    return jsDouble(f);
}

jsValue WKE_CALL_TYPE jsDouble(double d)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueDouble;
    out->doubleVal = d;
    return ret;
}

jsValue WKE_CALL_TYPE jsBoolean(bool b)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueBool;
    out->boolVal = b;
    return ret;
}

jsValue WKE_CALL_TYPE jsUndefined()
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueUndefined;
    return ret;
}

jsValue WKE_CALL_TYPE jsNull()
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    WkeJsValue* out;
    jsValue ret = createEmptyJsValue(&out);
    out->type = WkeJsValue::wkeJsValueNull;
    return ret;
}

jsValue WKE_CALL_TYPE jsTrue()
{
    return jsBoolean(true);
}

jsValue WKE_CALL_TYPE jsFalse()
{
    return jsBoolean(false);
}

jsValue WKE_CALL_TYPE jsString(jsExecState es, const utf8* str)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

jsValue WKE_CALL_TYPE jsStringW(jsExecState es, const wchar_t* str)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

jsValue WKE_CALL_TYPE jsArrayBuffer(jsExecState es, char * buffer, size_t size)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    if (es->context.IsEmpty())
        DebugBreak();

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::ArrayBuffer> value = v8::ArrayBuffer::New(es->isolate, size);
    memcpy(value->GetContents().Data(), buffer, size);

    if (value.IsEmpty())
        return jsUndefined();

    return createJsValueByLocalValue(es->isolate, context, value);
}

wkeMemBuf* WKE_CALL_TYPE jsGetArrayBuffer(jsExecState es, jsValue value)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es)
        return nullptr;

    JsValueMap::iterator it = findJsValueMap(value);
    if (it == s_jsValueMap->end())
        return nullptr;

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> valueV8 = getV8Value(value, context);
    if (valueV8.IsEmpty() || !valueV8->IsArrayBuffer())
        return nullptr;

    v8::ArrayBuffer* arrayBuffer = v8::ArrayBuffer::Cast(*valueV8);
    v8::ArrayBuffer::Contents contents = arrayBuffer->GetContents();
    wkeMemBuf* result = wkeCreateMemBuf(nullptr, contents.Data(), contents.ByteLength());

    return result;
}

jsValue WKE_CALL_TYPE jsEmptyObject(jsExecState es)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

jsValue WKE_CALL_TYPE jsEmptyArray(jsExecState es)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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
jsValue WKE_CALL_TYPE jsGlobalObject(jsExecState es)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

jsValue WKE_CALL_TYPE jsEval(jsExecState es, const utf8* str)
{
    String s = String::fromUTF8(str);
    Vector<UChar> buf = WTF::ensureUTF16UChar(s, true);
    return jsEvalW(es, buf.data());
}

jsValue WKE_CALL_TYPE jsEvalW(jsExecState es, const wchar_t* str)
{
    return jsEvalExW(es, str, true);
}

jsValue WKE_CALL_TYPE jsEvalExW(jsExecState es, const wchar_t* str, bool isInClosure)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

    v8::MaybeLocal<v8::Script> maybeScript = v8::Script::Compile(context, source.ToLocalChecked());
    if (maybeScript.IsEmpty())
        return jsUndefined();

    v8::Local<v8::Script> script = maybeScript.ToLocalChecked();
    v8::TryCatch trycatch(isolate);
    v8::Local<v8::Value> result = script->Run(context).FromMaybe(v8::Local<v8::Value>());

    return createJsValueByLocalValue(isolate, context, result);
}

jsExceptionInfo* g_jsExceptionInfo = nullptr;

static char* mallocEmpty()
{
    char* result = (char*)malloc(1);
    result[0] = 0;
    return result;
}

static char* strDupWithLengthLimit(const char* str, int length)
{
    if (!str || 0 == length)
        return mallocEmpty();

    if (length > 1000)
        length = 1000;
    char* result = (char*)malloc(length + 1);
    memcpy(result, str, length);
    result[length] = 0;
    return result;
}

static std::string* saveCallstack(v8::Local<v8::StackTrace> stackTrace)
{
    std::string* outString = new std::string();

    const v8::StackTrace::StackTraceOptions options = static_cast<v8::StackTrace::StackTraceOptions>(
        v8::StackTrace::kLineNumber |
        v8::StackTrace::kColumnOffset |
        v8::StackTrace::kScriptId |
        v8::StackTrace::kScriptNameOrSourceURL |
        v8::StackTrace::kFunctionName);

    int stackNum = 50;
    if (stackTrace.IsEmpty())
        stackTrace = v8::StackTrace::CurrentStackTrace(v8::Isolate::GetCurrent(), stackNum, options);
    int count = stackTrace->GetFrameCount();

    for (int i = 0; i < count; ++i) {
        v8::Local<v8::StackFrame> stackFrame = stackTrace->GetFrame(v8::Isolate::GetCurrent(), i);
        int frameCount = stackTrace->GetFrameCount();
        int line = stackFrame->GetLineNumber();
        v8::Local<v8::String> scriptName = stackFrame->GetScriptNameOrSourceURL();
        v8::Local<v8::String> funcName = stackFrame->GetFunctionName();

        std::string scriptNameWTF;
        std::string funcNameWTF;

        if (!scriptName.IsEmpty()) {
            v8::String::Utf8Value scriptNameUtf8(v8::Isolate::GetCurrent(), scriptName);
            scriptNameWTF = *scriptNameUtf8;
        }

        if (!funcName.IsEmpty()) {
            v8::String::Utf8Value funcNameUtf8(v8::Isolate::GetCurrent(), funcName);
            funcNameWTF = *funcNameUtf8;
        }
        std::vector<char> output;
        output.resize(1000);
        sprintf(&output[0], "line:%d, [", line);
        *outString += (&output[0]);

        if (!scriptNameWTF.empty()) {
            *outString += (scriptNameWTF.c_str());
        }
        *outString += ("] , [");

        if (!funcNameWTF.empty()) {
            *outString += (funcNameWTF.c_str());
        }
        *outString += ("]\n");
    }
    *outString += ("\n");

    return outString;
}

jsValue WKE_CALL_TYPE jsCall(jsExecState es, jsValue func, jsValue thisValue, jsValue* args, int argCount)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    RELEASE_ASSERT(!es->context.IsEmpty());

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
    if (cbValue.IsEmpty() || !cbValue->IsFunction()) {
        delete[] argv;
        return jsUndefined();
    }

    v8::Function* cb = v8::Function::Cast(*cbValue);

    v8::Local<v8::Value> thisValueV8 = getV8Value(thisValue, context);

    blink::V8RecursionScope::MicrotaskSuppression recursionScope(isolate);
    
    if (thisValueV8.IsEmpty() || thisValueV8->IsUndefined())
        thisValueV8 = context->Global();

    v8::TryCatch tryCatch(isolate);
    tryCatch.SetVerbose(true);

    v8::MaybeLocal<v8::Value> ret = cb->Call(context, thisValueV8, argCount, argv);

    if (tryCatch.HasCaught()) {
        wke::AutoAllowRecordJsExceptionInfo autoAllowRecordJsExceptionInfo;
        wke::recordJsExceptionInfo(tryCatch);
    } else {
        delete g_jsExceptionInfo;
        g_jsExceptionInfo = nullptr;
    }
    
    delete[] argv;

    if (ret.IsEmpty())
        return jsUndefined();

    v8::Local<v8::Value> v8Ret = ret.ToLocalChecked();
    if (v8Ret->IsUndefined())
        return jsUndefined();
    return createJsValueByLocalValue(isolate, context, v8Ret);
}

const utf8* WKE_CALL_TYPE jsGetCallstack(jsExecState es)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    std::string* callstack = saveCallstack(v8::Local<v8::StackTrace>());
    const utf8* result = wke::createTempCharString(callstack->c_str(), callstack->size());
    delete callstack;
    return result;
}

jsValue WKE_CALL_TYPE jsCallGlobal(jsExecState es, jsValue func, jsValue* args, int argCount)
{
    return jsCall(es, func, jsUndefined(), args, argCount);
}

jsExceptionInfo* WKE_CALL_TYPE jsGetLastErrorIfException(jsExecState es)
{
    return g_jsExceptionInfo;
}

jsValue WKE_CALL_TYPE jsThrowException(jsExecState es, const utf8* exception)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    RELEASE_ASSERT(!es->context.IsEmpty());

    v8::Isolate* isolate = es->isolate;
    isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, exception)));
    return jsUndefined();
}

jsValue WKE_CALL_TYPE jsGet(jsExecState es, jsValue object, const char* prop)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    RELEASE_ASSERT(!es->context.IsEmpty());

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> value = getV8Value(object, context);
    if (value.IsEmpty() || !value->IsObject())
        return jsUndefined();

    v8::Local<v8::Object> obj = value->ToObject(isolate);
    v8::TryCatch tryCatch(isolate);
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

void WKE_CALL_TYPE jsSet(jsExecState es, jsValue object, const char* prop, jsValue value)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return;
    RELEASE_ASSERT(!es->context.IsEmpty());
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
    
    v8::Local<v8::Object> obj = objectLocal->ToObject(isolate);
    v8::TryCatch tryCatch(isolate);
    tryCatch.SetVerbose(true);

    v8::MaybeLocal<v8::String> propV8 = v8::String::NewFromUtf8(isolate, prop, v8::NewStringType::kNormal, -1);
    if (propV8.IsEmpty())
        return;
    v8::Local<v8::String> propV8Local = propV8.ToLocalChecked();

    obj->Set(propV8Local, valueLocal);
}

jsValue WKE_CALL_TYPE jsGetGlobal(jsExecState es, const char* prop)
{
    return jsGet(es, jsGlobalObject(es), prop);
}

void WKE_CALL_TYPE jsSetGlobal(jsExecState es, const char* prop, jsValue v)
{
    jsSet(es, jsGlobalObject(es), prop, v);
}

void WKE_CALL_TYPE jsDeleteObjectProp(jsExecState es, jsValue object, const char* prop)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return;
    RELEASE_ASSERT(!es->context.IsEmpty());
    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> objectLocal = getV8Value(object, context);
    if (objectLocal.IsEmpty() || !objectLocal->IsObject())
        return;

    v8::Local<v8::Object> obj = objectLocal->ToObject(isolate);
    v8::MaybeLocal<v8::String> propV8 = v8::String::NewFromUtf8(isolate, prop, v8::NewStringType::kNormal, -1);
    if (propV8.IsEmpty())
        return;

    obj->Delete(context, propV8.ToLocalChecked());
}

bool WKE_CALL_TYPE jsIsValidExecState(jsExecState es)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return false;
    return true;
}

bool WKE_CALL_TYPE jsIsJsValueValid(jsExecState es, jsValue object)
{
    if (!jsIsValidExecState(es))
        return false;

    JsValueMap::iterator it = findJsValueMap(object);
    if (it == s_jsValueMap->end())
        return false;

    return true;
}

jsValue WKE_CALL_TYPE jsGetAt(jsExecState es, jsValue object, int index)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return jsUndefined();
    RELEASE_ASSERT(!es->context.IsEmpty());

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> value = getV8Value(object, context);
    if (value.IsEmpty()/* || !value->IsArray()*/)
        return jsUndefined();

    //v8::Local<v8::Array> obj = v8::Local<v8::Array>::Cast(value);
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(value);
    v8::MaybeLocal<v8::Value> retValue = obj->Get(context, index);
    if (retValue.IsEmpty())
        return jsUndefined();

    v8::TryCatch tryCatch(isolate);
    tryCatch.SetVerbose(true);
    if (tryCatch.HasCaught() || retValue.IsEmpty())
        return jsUndefined();

    return createJsValueByLocalValue(isolate, context, retValue.ToLocalChecked());
}

void WKE_CALL_TYPE jsSetAt(jsExecState es, jsValue object, int index, jsValue value)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return;
    RELEASE_ASSERT(!es->context.IsEmpty());

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

jsKeys* WKE_CALL_TYPE jsGetKeys(jsExecState es, jsValue object)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return nullptr;
    RELEASE_ASSERT(!es->context.IsEmpty());

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(es->isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> value = getV8Value(object, context);
    v8::Local<v8::Object> obj = value->ToObject(isolate);
    v8::Local<v8::Array> arrKeys = obj->GetPropertyNames(context).FromMaybe(v8::Local<v8::Array>());

    if (0 == arrKeys->Length())
        return nullptr;
    jsKeys* result = wke::createTempJsKeys(arrKeys->Length());
    
    for (uint32_t i = 0; i < result->length; ++i) {
        v8::Local<v8::Value> value = arrKeys->Get(v8::Integer::New(isolate, i));
        v8::Local<v8::String> str = value->ToString(isolate);
        v8::String::Utf8Value strUtf8(isolate, str);

        if (0 == strUtf8.length())
            continue;
        char* keyPtr = new char[strUtf8.length() + 1];
        strncpy(keyPtr, *strUtf8, strUtf8.length() + 1);
        result->keys[i] = keyPtr;
    }
    return result;
}

int WKE_CALL_TYPE jsGetLength(jsExecState es, jsValue object)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || !es->isolate)
        return 0;
    RELEASE_ASSERT(!es->context.IsEmpty());

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

void WKE_CALL_TYPE jsSetLength(jsExecState es, jsValue object, int length)
{
//     JSC::ExecState* exec = (JSC::ExecState*)es;
//     JSC::JSValue o = JSC::JSValue::decode(object);
// 
//     JSC::PutPropertySlot slot;
//     o.put(exec, JSC::Identifier(exec, "length"), JSC::jsNumber(length), slot);
}

wkeWebView WKE_CALL_TYPE jsGetWebView(jsExecState es)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

void WKE_CALL_TYPE jsGC()
{
    //WebCore::gcController().garbageCollectNow();
    wkeGC(nullptr, 1);
}

bool WKE_CALL_TYPE jsAddRef(jsExecState es, jsValue val)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

bool WKE_CALL_TYPE jsReleaseRef(jsExecState es, jsValue val)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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
//     wkeValue->value.Reset();
//     delete wkeValue;
//     s_jsValueMap->remove(it);

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
    v8::Local<v8::Function> func = tmpl->GetFunction(context).FromMaybe(v8::Local<v8::Function>());
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

    v8::TryCatch tryCatch(isolate);
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


#define MAX_NAME_LENGTH 1024
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

static jsValue WKE_CALL_TYPE wkeJsBindFunctionWrap(jsExecState es, void* param)
{
    jsNativeFunction fn = (jsNativeFunction)param;
    return fn(es);
}

void WKE_CALL_TYPE jsBindFunction(const char* name, jsNativeFunction fn, unsigned int argCount)
{
    wkeJsBindFunction(name, wkeJsBindFunctionWrap, fn, argCount);
}

static void wkeJsBindSetterGetter(const char* name, wkeJsNativeFunction fn, void* param, unsigned int funcType)
{
    if (!s_jsFunctionsPtr)
        s_jsFunctionsPtr = new Vector<jsFunctionInfo>();
    Vector<jsFunctionInfo>& s_jsFunctions = *s_jsFunctionsPtr;

    if (strlen(name) > MAX_NAME_LENGTH - 1)
        return;

    for (unsigned int i = 0; i < s_jsFunctions.size(); ++i) {
        if (strcmp(name, s_jsFunctions[i].name) == 0) {
            JS_GETTER == funcType ? s_jsFunctions[i].gettet = fn : s_jsFunctions[i].settet = fn;
            return;
        }
    }

    jsFunctionInfo funcInfo;
    strcpy(funcInfo.name, name);
    funcInfo.name[MAX_NAME_LENGTH - 1] = '\0';
    JS_GETTER == funcType ? funcInfo.gettet = fn : funcInfo.settet = fn;
    JS_GETTER == funcType ? funcInfo.getterParam = param : funcInfo.setterParam = param;

    funcInfo.argCount = 0;
    funcInfo.funcType |= funcType;

    s_jsFunctions.append(funcInfo);
}

void WKE_CALL_TYPE jsBindGetter(const char* name, jsNativeFunction fn)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wkeJsBindSetterGetter(name, wkeJsBindFunctionWrap, fn, JS_GETTER);
}

void WKE_CALL_TYPE jsBindSetter(const char* name, jsNativeFunction fn)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    wkeJsBindSetterGetter(name, wkeJsBindFunctionWrap, fn, JS_SETTER);
}

void WKE_CALL_TYPE wkeJsBindFunction(const char* name, wkeJsNativeFunction fn, void* param, unsigned int argCount)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!name || strlen(name) > MAX_NAME_LENGTH - 1)
        return;

    if (!s_jsFunctionsPtr)
        s_jsFunctionsPtr = new Vector<jsFunctionInfo>();
    Vector<jsFunctionInfo>& s_jsFunctions = *s_jsFunctionsPtr;

    for (unsigned int i = 0; i < s_jsFunctions.size(); ++i) {
        if (s_jsFunctions[i].funcType == JS_FUNC && strcmp(name, s_jsFunctions[i].name) == 0) {
            s_jsFunctions[i].fn = fn;
            s_jsFunctions[i].param = param;
            s_jsFunctions[i].argCount = argCount;
            return;
        }
    }

    jsFunctionInfo funcInfo;
    strcpy(funcInfo.name, name);
    funcInfo.name[MAX_NAME_LENGTH - 1] = '\0';
    funcInfo.fn = fn;
    funcInfo.param = param;
    funcInfo.argCount = argCount;
    funcInfo.funcType = JS_FUNC;

    s_jsFunctions.append(funcInfo);
}

void WKE_CALL_TYPE wkeJsBindGetter(const char* name, wkeJsNativeFunction fn, void* param)
{
    wkeJsBindSetterGetter(name, fn, param, JS_GETTER);
}

void WKE_CALL_TYPE wkeJsBindSetter(const char* name, wkeJsNativeFunction fn, void* param)
{
    wkeJsBindSetterGetter(name, fn, param, JS_GETTER);
}

jsValue WKE_CALL_TYPE js_outputMsg(jsExecState es, void* param)
{
    if (jsArgCount(es) != 1 || jsArgType(es, 0) != JSTYPE_STRING)
        return jsUndefined();

    jsValue value = jsArg(es, 0);
    OutputDebugStringW(jsToTempStringW(es, value));

    return jsUndefined();
}

jsValue WKE_CALL_TYPE js_getWebViewName(jsExecState es, void* param)
{
    wkeWebView webView = jsGetWebView(es);
    return jsString(es, webView->name());
}

jsValue WKE_CALL_TYPE js_setWebViewName(jsExecState es, void* param)
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

jsValue WKE_CALL_TYPE jsObject(jsExecState es, jsData* data)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

    v8::Local<v8::External> external = v8::External::New(isolate, wrap);
    objTemplate->SetNamedPropertyHandler(namedPropertyGetterCallback, namedPropertySetterCallback, nullptr, nullptr, nullptr, external);
    
    WkeJsValue* wkeJsValue = nullptr;
    v8::Local<v8::Object> objInst = objTemplate->NewInstance(context).ToLocalChecked();

    blink::V8HiddenValue::setHiddenValue(isolate, objInst, v8::String::NewFromUtf8(isolate, "wkeJsData", v8::NewStringType::kNormal, -1).ToLocalChecked(), external);

    jsValue retValue = createJsValueByLocalValue2(isolate, context, objInst, &wkeJsValue, false);
    wkeJsValue->value.SetWeak<NativeGetterSetterWrap>(wrap, &NativeGetterSetterWrap::firstJsObjectWeakCallback, v8::WeakCallbackType::kInternalFields);
    wrap->set(retValue);

    return retValue;
}

void jsFunctionConstructCallback(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
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

jsValue WKE_CALL_TYPE jsFunction(jsExecState es, jsData* data)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
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

jsData* WKE_CALL_TYPE jsGetData(jsExecState es, jsValue value)
{
    wke::checkThreadCallIsValid(__FUNCTION__);
    if (!s_execStates || !s_execStates->contains(es) || !es || es->context.IsEmpty() || !es->isolate)
        return nullptr;

    v8::Isolate* isolate = es->isolate;
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate, es->context);
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> valueV8 = getV8Value(value, context);
    if (valueV8.IsEmpty())
        return nullptr;

    v8::Local<v8::Value> external = valueV8;
    NativeGetterSetterWrap* wrap = nullptr;
    if (!external->IsExternal()) {
        if (!valueV8->IsObject())
            return nullptr;

        v8::Local<v8::Object> obj = valueV8->ToObject(isolate);
        external = blink::V8HiddenValue::getHiddenValue(isolate, obj, v8::String::NewFromUtf8(isolate, "wkeJsData", v8::NewStringType::kNormal, -1).ToLocalChecked());
        if (external.IsEmpty() || !external->IsExternal())
            return nullptr;
    }

    wrap = static_cast<NativeGetterSetterWrap*>(v8::External::Cast(*external)->Value());
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

    ensureStaticVar();
    
    v8::Isolate* isolate = context->GetIsolate();
    setWkeWebViewToV8Context(client, context);

    addFunction(context, "outputMsg", js_outputMsg, nullptr, 1);
    addAccessor(context, "webViewName", js_getWebViewName, nullptr, js_setWebViewName, nullptr);
    
    const char* injectCode =
        "window.chrome = {app:null, runtime:null};"
        "window.Intl = {DateTimeFormat : function (locales, options) {return {format : function(event) {return event.toLocaleString(locales, options);}};}}";

    blink::WebScriptSource injectSource(blink::WebString::fromUTF8(injectCode));
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

            v8::Local<v8::Value> value = v8::Local<v8::Value>::New(wkeJsValue->isolate, wkeJsValue->value);
            if (value.IsEmpty() && value->IsObject()) {
                v8::Local<v8::Object> obj = value->ToObject(isolate);
                blink::V8HiddenValue::deleteHiddenValue(isolate, obj, v8::String::NewFromUtf8(isolate, "wkeJsData", v8::NewStringType::kNormal, -1).ToLocalChecked());
            }

            wkeJsValue->value.Reset();
        }
        delete wrap;
    }
    cachedWraps->clear();
}

AutoDisableFreeV8TempObejct::AutoDisableFreeV8TempObejct()
{
    ++m_disableCount;
    content::WebThreadImpl* threadImpl = (content::WebThreadImpl*)blink::Platform::current()->currentThread();
    threadImpl->disableScheduler();
}

AutoDisableFreeV8TempObejct::~AutoDisableFreeV8TempObejct()
{
    content::WebThreadImpl* threadImpl = (content::WebThreadImpl*)blink::Platform::current()->currentThread();
    threadImpl->enableScheduler();
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
        v8::Local<v8::Int32> v8Number = v8Value->ToInt32(context->GetIsolate());
        return jsInt(v8Number->Value());
    } else if (v8Value->IsUint32()) {
        v8::Local<v8::Uint32> v8Number = v8Value->ToUint32(context).FromMaybe(v8::Local<v8::Uint32>());
        return jsInt(v8Number->Value());
    } else if (v8Value->IsNumber()) {
        v8::Local<v8::Number> v8Number = v8Value->ToNumber(context->GetIsolate());
        return jsDouble(v8Number->Value());
    }

    return jsUndefined();
}

static void freeExceptionInfo()
{
    if (!g_jsExceptionInfo)
        return;

    if (g_jsExceptionInfo->message)
        free((void*)g_jsExceptionInfo->message);

    if (g_jsExceptionInfo->sourceLine)
        free((void*)g_jsExceptionInfo->sourceLine);

    if (g_jsExceptionInfo->scriptResourceName)
        free((void*)g_jsExceptionInfo->scriptResourceName);

    delete g_jsExceptionInfo;
    g_jsExceptionInfo = nullptr;
}

void recordJsExceptionInfo(const v8::TryCatch& tryCatch)
{
    if (!AutoAllowRecordJsExceptionInfo::isAllow() || !tryCatch.HasCaught()) {
        freeExceptionInfo();
        g_jsExceptionInfo = nullptr;
        return;
    }

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    if (!g_jsExceptionInfo)
        g_jsExceptionInfo = new jsExceptionInfo();

    v8::Local<v8::Message> message = tryCatch.Message();
    v8::String::Utf8Value messageUtf8(isolate, message->Get());
    g_jsExceptionInfo->message = strDupWithLengthLimit(*messageUtf8, messageUtf8.length());

    v8::String::Utf8Value sourceLineUtf8(isolate, message->GetSourceLine(isolate->GetCurrentContext()).FromMaybe(v8::Local<v8::Value>()));
    g_jsExceptionInfo->sourceLine = strDupWithLengthLimit(*sourceLineUtf8, sourceLineUtf8.length());

    if (!message->GetScriptResourceName().IsEmpty()) {
        v8::String::Utf8Value scriptResourceNameUtf8(isolate, message->GetScriptResourceName()->ToString(isolate));
        g_jsExceptionInfo->scriptResourceName = strDupWithLengthLimit(*scriptResourceNameUtf8, scriptResourceNameUtf8.length());
    } else
        g_jsExceptionInfo->scriptResourceName = mallocEmpty();

    g_jsExceptionInfo->lineNumber = message->GetLineNumber(isolate->GetCurrentContext()).FromJust();
    g_jsExceptionInfo->startPosition = message->GetStartPosition();
    g_jsExceptionInfo->endPosition = message->GetEndPosition();
    g_jsExceptionInfo->startColumn = message->GetStartColumn();
    g_jsExceptionInfo->endColumn = message->GetEndColumn();

    std::string* callstackString = saveCallstack(message->GetStackTrace());
    g_jsExceptionInfo->callstackString = strDupWithLengthLimit(callstackString->c_str(), callstackString->size());
    delete callstackString;
}

AutoAllowRecordJsExceptionInfo::AutoAllowRecordJsExceptionInfo()
{
    if (0 == m_allowCount)
        freeExceptionInfo();
    m_allowCount++;
}

AutoAllowRecordJsExceptionInfo::~AutoAllowRecordJsExceptionInfo()
{
    m_allowCount--;
}

int AutoAllowRecordJsExceptionInfo::m_allowCount = 0;

};

#endif
