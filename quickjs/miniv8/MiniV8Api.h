
#include "G:/mycode/mb_temp/v8_5_7/include/v8.h"
#include "G:/mycode/mb_temp/v8_5_7/include/v8-profiler.h"
#include "G:/mycode/mb_temp/v8_5_7/include/v8-debug.h"
//#include "G:/mycode/mb_temp/v8_5_7/include/sys-info.h"

#include "g:/mycode/quickjs-master/quickjs.h"
#include <vector>
#include <map>
#include <string>

void printEmptyFuncInfo(const char* fun, bool isBreak);
void miniv8ReleaseAssert(bool b, const char* info);
void printDebug(const char* format, ...);

namespace v8 {

class Utils {
public:
    template<class From, class To>
    static inline To* openHandle(const From* that, bool allowEmptyHandle = false);

    template<class From, class To>
    static inline Local<To> convert(From* obj);
};

}

namespace miniv8 {

extern JSClassID g_mbClassId;

uint32_t hash(const char* arKey, size_t nKeyLength);
size_t getUtf8Length(const char* str, size_t clen);
std::string utf16ToUtf8(const std::wstring& u16str);
std::string utf16leToUtf8(const std::wstring& u16str);
std::string utf16beToUtf8(const std::wstring& u16str);
std::wstring utf8ToUtf16le(const std::string& u8str, bool addbom, bool* ok);
std::wstring utf8ToUtf16be(const std::string& u8str, bool addbom, bool* ok);

class V8Context;

struct FunctionCallbackInfoWrap {
    void* implicitArgs;
    void* values;
    int length;
};

enum V8ObjectType : uint32_t {
    kObjectTypeContext,
    kObjectTypeData,
    kObjectTypePrivate,
    kObjectTypeValue,
    kObjectTypePrimitive,
    kObjectTypeName,
    kObjectTypeSymbol,
    kObjectTypeString,
    kObjectTypeAccessorSignature,
    kObjectTypeArray,
    kObjectTypeObject,
    kObjectTypeFunction,
    kObjectTypeExternal,
    kObjectTypeArrayBuffer,
    kObjectTypeInt16Array,
    kObjectTypeInt32Array,
    kObjectTypeInt8Array,
    kObjectTypeInt32,
    kObjectTypeInteger,
    kObjectTypeTemplate,
    kObjectTypeObjectTemplate,
    kObjectTypeFunctionTemplate,
};

class V8Isolate {
public:
    V8Isolate();
    ~V8Isolate();

    static V8Isolate* GetCurrent()
    {
        return m_inst; // TODO: save at tls
    }

    bool isInContext(void);
    void enterContext(V8Context* ctx);
    void exitContext();

    V8Context* getCurrentCtx();
    //V8Context* setCurrentCtx(V8Context* ctx);

    JSRuntime* getRuntime() const
    {
        return m_runtime;
    }

    void** findGlobalizeHandleEmptyIndex();
    void** findHandleScopeEmptyIndex();

private:
    void* m_apiPointer[64]; // 这个必须放最前面，v8的机制如此
    const int kHandlesSize = 0x10000;
    std::vector<void*> m_globalizeHandles;
    std::vector<void*> m_handleScopeHandles;
    JSRuntime* m_runtime;
    //V8Context* m_currentCtx;
    std::vector<V8Context*> m_currentCtxs;
    static V8Isolate* m_inst;
};

class V8StackTrace {
public:
    V8StackTrace()
    {

    }
    int getFrameCount() const
    {
        return 0;
    }
};

class V8Context {
public:
    V8Context(V8Isolate* isolate, JSContext* ctx);
    V8Context(const V8Context& other);

    JSContext* ctx() { return m_ctx; }
    V8Isolate* isolate() { return m_isolate; }

    bool isCodeGenerationFromStringsAllowed() const { return m_isCodeGenerationFromStringsAllowed; }

private:
    friend class v8::Context;

    V8ObjectType m_type; // 这个必须放最前面，方便识别这是什么类型
    JSContext* m_ctx;
    V8Isolate* m_isolate;
    bool m_isCodeGenerationFromStringsAllowed;

    std::map<int, void*> m_alignedPointerInEmbedderDatas;
};

// 不能有虚函数，否则第一个字段不是m_type
class V8Data {
public:
    V8Data(V8Context* ctx, JSValue value)
    {
        m_type = kObjectTypeData;
        m_ctx = ctx;
        m_qjsValue = value;
        m_nodeClassId = 0;
        m_nodeFlag = 0;
        m_nodeState = 0;

        v = V8Data_v;
        isObject = false;
        isExternal = false;
        isFunciton = false;
    }

    static JSValue V8Data_v(V8Data* self)
    {
        return self->m_qjsValue;
    }

    JSContext* ctx() const
    {
        if (!m_ctx)
            return nullptr;
        return m_ctx->ctx();
    }

    V8Context* v8Ctx() const
    {
        return m_ctx;
    }

    V8ObjectType m_type;
    uint16_t m_nodeClassId;
    uint8_t m_nodeFlag;
    uint8_t m_nodeState;
protected:
    JSValue m_qjsValue;
    V8Context* m_ctx;

    typedef JSValue(*FN_v)(V8Data* self);

public:
    FN_v v;
    bool isObject;
    bool isExternal;
    bool isFunciton;
};

class V8Private : public V8Data {
public:
    V8Private(V8Context* ctx, JSValue value) : V8Data(ctx, value)
    {
        m_type = kObjectTypePrivate;
    }
};

class V8Value : public V8Data {
public:
    V8Value(V8Context* ctx, JSValue value) : V8Data(ctx, value)
    {
        m_type = kObjectTypeValue;
    }

    static V8Value* create(V8Context* ctx, JSValue value);
};

class V8Object : public V8Value {
public:
    static V8Object* create(V8Context* ctx, JSValue value);
    ~V8Object();

    static JSClassID getClassId() { return s_classId; }

private:
    V8Object(V8Context* ctx, JSValue value);

    friend class v8::Object;
    friend class v8::ObjectTemplate;
    friend class v8::FunctionTemplate;
    friend class v8::Function;
    friend class V8Array;
    friend class V8Function;
    friend class V8Function;
    friend class V8Template;
    
    friend JSValue wrapGetter(JSContext* ctx, JSValueConst thisVal, void* userdata);
    friend JSValue wrapSetter(JSContext* ctx, JSValueConst this_val, JSValueConst val, void* userdata);

    struct AccessorData {
        std::string name;
        v8::AccessorGetterCallback getter;
        v8::AccessorSetterCallback setter;
        V8Value* data;
    };

    static JSClassID s_classId;
    std::vector<JSCFunctionListEntry>* m_props; // 不定长的数组
    std::map<uint32_t, AccessorData*> m_accessorMap;

    int m_internalFieldCount;
    std::map<int, void*> m_alignedPointerInInternalFields;
};

class V8Primitive : public V8Value {
public:
    V8Primitive(V8Context* ctx, JSValue value) : V8Value(ctx, value)
    {
        m_type = kObjectTypePrimitive;
    }
};

class V8Name : public V8Primitive {
public:
    V8Name(V8Context* ctx, JSValue value) : V8Primitive(ctx, value)
    {
        m_type = kObjectTypeName;
    }
};

class V8String : public V8Name {
public:
    V8String(V8Context* ctx, JSValue value) : V8Name(ctx, value)
    {
        m_type = kObjectTypeString;
        v = V8String_v;
        m_externalString = nullptr;        
    }
    V8String(const std::string& str) : V8Name(nullptr, JS_NULL)
    {
        m_type = kObjectTypeString;
        v = V8String_v;
        m_str = str;
        m_externalString = nullptr;        
    }

    std::string getStr();

    static JSValue V8String_v(V8Data* self);

    void ensureQjsVal(V8Context* ctx);
protected:
    friend class v8::String;
    std::string m_str;
    v8::String::ExternalStringResourceBase* m_externalString;
};

class V8Symbol : public V8String { // 暂时设定为string的子类
public:
    V8Symbol(V8Context* ctx, JSValue value) : V8String(ctx, value)
    {
        m_type = kObjectTypeSymbol;
        v = V8String_v;
    }
    V8Symbol(const std::string& str) : V8String(nullptr, JS_NULL)
    {
        m_type = kObjectTypeSymbol;
        m_str = str;
        v = V8String_v;
    }
};

class V8AccessorSignature : public V8Data {
public:
    V8AccessorSignature(V8Context* ctx, JSValue value)
        : V8Data(ctx, value)
    {
        m_type = kObjectTypeAccessorSignature;
    }
};

class V8Array : public V8Object {
public:
    V8Array(V8Context* ctx, JSValue value)
        : V8Object(ctx, value)
    {
        m_type = kObjectTypeArray;
    }
};

class V8Function : public V8Object {
public:
    V8Function(V8Context* ctx, v8::FunctionCallback callback, void* data)
        : V8Object(ctx, JS_NULL)
    {
        m_type = kObjectTypeFunction;
        m_callback = callback;
        isFunciton = true;
        m_data = data;
        v = V8Function_v;
    }

    // 为了延迟初始化
    static JSValue V8Function_v(V8Data* self);
    std::string getName();
    void setName(const std::string& name);

private:
    friend class v8::Function;
    static JSValue onV8FunctionWrap(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv, void* userdata);

    v8::FunctionCallback m_callback;
    void* m_data;
    std::string m_name;
};

class V8External : public V8Value {
public:
    V8External(void* userdata) : V8Value(nullptr, JS_NULL)
    {
        m_type = kObjectTypeExternal;
        m_userdata = userdata;
        isExternal = true;
    }

    void* getUserdata()
    {
        return m_userdata;
    }

private:
    friend class v8::External;
    void* m_userdata;
};

class V8Template {
public:
    V8Template()
    {
        m_type = kObjectTypeTemplate;
        m_isInit = false;
        m_internalFieldCount = 0;
        m_parentTemplate = nullptr;
    }

    V8ObjectType m_type;

    struct Accessor {
        std::string name;
        v8::AccessorGetterCallback getter;
        v8::AccessorSetterCallback setter;

        v8::AccessorNameGetterCallback nameGetter;
        v8::AccessorNameSetterCallback nameSetter;
        void* data;
        Accessor()
        {
            getter = nullptr;
            setter = nullptr;
            nameGetter = nullptr;
            nameSetter = nullptr;
            data = nullptr;
        }
    };
    void newTemplateInstance(miniv8::V8Context* ctx, miniv8::V8Object* obj);
protected:
    friend class v8::ObjectTemplate;
    friend class v8::FunctionTemplate;
    friend class v8::Template;


    std::map<std::string, Accessor> m_accessors;

    int m_internalFieldCount;
    V8Template* m_parentTemplate;
    std::map<std::string, JSValue> m_props;
    bool m_isInit;
};

class V8ObjectTemplate : public V8Template {
public:
    V8ObjectTemplate()
    {
        printDebug("V8ObjectTemplate: %p\n", this);

        m_type = kObjectTypeObjectTemplate;
    }

private:
    friend class v8::ObjectTemplate;
    friend class v8::FunctionTemplate;
    friend class V8FunctionTemplate;
};

class V8FunctionTemplate : public V8Template {
public:
    V8FunctionTemplate()
    {
        //printDebug("V8FunctionTemplate: %p\n", this);
        m_type = kObjectTypeFunctionTemplate;
        m_userdata = nullptr;
        m_instanceTemplate = nullptr;
        m_prototypeTemplate = nullptr;
        m_argLength = 0;
    }
private:
    std::string getClassName();
    void setClassName(const std::string& name);

    v8::FunctionCallback m_constructor;
    void* m_userdata;
    std::string m_name;
    int m_argLength;
    V8ObjectTemplate* m_instanceTemplate;
    V8ObjectTemplate* m_prototypeTemplate;

    friend class v8::Template;
    friend class v8::ObjectTemplate;
    friend class v8::FunctionTemplate;
    friend class V8ObjectTemplate;
    static void V8CALL functionTemplateCall(const v8::FunctionCallbackInfo<v8::Value>& info);
};

class V8Script {
public:
    void setSource(const std::string& src) { m_src = src; }
    const std::string& getSource() const { return m_src; }

    void setSourceName(const std::string& name) { m_srcName = name; }
    const std::string& getSourceName() const { return m_srcName; }

private:
    std::string m_src;
    std::string m_srcName;
};

inline std::string getStringFromV8String(v8::Local<v8::String> str)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8String* v8string = v8::Utils::openHandle<v8::String, miniv8::V8String>(*str);

//     miniv8::V8Isolate* isolate = miniv8::V8Isolate::GetCurrent();
//     JSContext* ctx = v8string->ctx();
//     if (!ctx)
//         ctx = isolate->getCurrentCtx()->ctx();
// 
//     size_t len = 0;
//     const char* nameString = JS_ToCStringLen(ctx, &len, v8string->v(v8string));
//     std::string ret(nameString);
//     JS_FreeCString(v8string->ctx(), nameString);
// 
//     return ret;
    return v8string->getStr();
}

class V8ArrayBuffer : public V8Value {
public:
    V8ArrayBuffer(V8Context* ctx, JSValue value)
        : V8Value(ctx, value)
    {
        m_type = kObjectTypeArrayBuffer;
    }
};

class V8Int16Array : public V8ArrayBuffer {
public:
    V8Int16Array(V8Context* ctx, JSValue value)
        : V8ArrayBuffer(ctx, value)
    {
        m_type = kObjectTypeInt16Array;
    }
};

class V8Int32Array : public V8ArrayBuffer {
public:
    V8Int32Array(V8Context* ctx, JSValue value)
        : V8ArrayBuffer(ctx, value)
    {
        m_type = kObjectTypeInt32Array;
    }
};

class V8Int8Array : public V8ArrayBuffer {
public:
    V8Int8Array(V8Context* ctx, JSValue value)
        : V8ArrayBuffer(ctx, value)
    {
        m_type = kObjectTypeInt8Array;
    }
};

class V8Int32 : public V8Value {
public:
    V8Int32(V8Context* ctx, JSValue value)
        : V8Value(ctx, value)
    {
        m_type = kObjectTypeInt32;
    }
};

class V8Integer : public V8Value {
public:
    V8Integer(V8Context* ctx, JSValue value)
        : V8Value(ctx, value)
    {
        m_type = kObjectTypeInteger;
    }
};

} // miniv8

//////////////////////////////////////////////////////////////////////////

namespace v8 {

template<class From, class To>
static inline To* Utils::openHandle(const From* that, bool allowEmptyHandle)
{
    return *(To**)that;
}

template<class From, class To>
static inline v8::Local<To> Utils::convert(From* obj)
{
    //return Local<To>(reinterpret_cast<To*>(obj));
    if (!obj)
        return Local<To>(nullptr);
    miniv8::V8Isolate* isolate = miniv8::V8Isolate::GetCurrent();
    void** ret = isolate->findHandleScopeEmptyIndex();
    *ret = obj;
    return Local<To>(reinterpret_cast<To*>(ret));
}

} // v8
