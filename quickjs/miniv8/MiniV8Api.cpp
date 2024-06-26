
#include "MiniV8Api.h"
#include <windows.h>

unsigned int g_v8MemSize = 0;

void printDebug(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);

    char* output = (char*)malloc(301);
    vsprintf(output, format, argList);
    OutputDebugStringA(output);
    free(output);

    va_end(argList);
}

void printEmptyFuncInfo(const char* fun, bool isBreak)
{
//     std::string output = fun;
//     output += "\n";
//     OutputDebugStringA(output.c_str());

//     if (isBreak)
//         DebugBreak();
}

void miniv8ReleaseAssert(bool b, const char* info)
{
    if (b)
        return;

    OutputDebugStringA(info);
    DebugBreak();
}

namespace v8 {

bool g_miniv8Enable = true;

namespace platform {
v8::Platform* CreateDefaultPlatform(int thread_pool_size);
bool PumpMessageLoop(v8::Platform*, v8::Isolate*);
}
}

namespace miniv8 {

V8Context::V8Context(V8Isolate* isolate, JSContext* ctx)
{
    m_type = kObjectTypeContext;
    m_isolate = isolate;
    m_ctx = ctx;
    m_isCodeGenerationFromStringsAllowed = true;
    JS_SetContextOpaque(ctx, this);

    char* output = (char*)malloc(0x100);
    sprintf(output, "V8Context::V8Context 1: %p\n", this);
    OutputDebugStringA(output);
    free(output);
}

V8Context::V8Context(const V8Context& other)
{
    m_type = other.m_type;
    m_isolate = other.m_isolate;
    m_ctx = other.m_ctx;
    m_isCodeGenerationFromStringsAllowed = other.m_isCodeGenerationFromStringsAllowed;

    char* output = (char*)malloc(0x100);
    sprintf(output, "V8Context::V8Context 2: %p\n", this);
    OutputDebugStringA(output);
    free(output);
}

} // miniv8

//////////////////////////////////////////////////////////////////////////

v8::Platform* v8::platform::CreateDefaultPlatform(int)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return nullptr;
}

bool v8::platform::PumpMessageLoop(v8::Platform*, v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return false;
}

__int64 v8::Integer::Value(void) const
{
    //printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Data* self = v8::Utils::openHandle<v8::Integer, miniv8::V8Data>(this);
    if (!JS_IsInteger(self->v(self)))
        return 0;

    __int64 result = 0;
    if (0 == JS_ToInt64(self->ctx(), &result, self->v(self)))
        return result;
    return 0;
}

__int64 v8::Value::IntegerValue(void) const
{
    //printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Data* self = v8::Utils::openHandle<v8::Value, miniv8::V8Data>(this);
    if (!JS_IsInteger(self->v(self)))
        return 0;

    __int64 result = 0;
    if (0 == JS_ToInt64(self->ctx(), &result, self->v(self)))
        return result;
    return 0;
}

bool v8::ArrayBuffer::IsExternal(void) const
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return false;
}

bool v8::ArrayBuffer::IsNeuterable(void) const
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return false;
}

bool v8::Boolean::Value(void) const
{
    //printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Data* self = v8::Utils::openHandle<v8::Boolean, miniv8::V8Data>(this);
    return (0 != JS_ToBool(self->ctx(), self->v(self)));
}

bool v8::BooleanObject::ValueOf(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Context::IsCodeGenerationFromStringsAllowed(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Context* self = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(this);
    
    return self->isCodeGenerationFromStringsAllowed();
}

bool v8::Isolate::AddMessageListener(void(__cdecl*)(v8::Local<v8::Message>, v8::Local<v8::Value>), v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return true;
}

bool v8::Isolate::GetHeapSpaceStatistics(v8::HeapSpaceStatistics*, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Isolate::IdleNotificationDeadline(double)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return true;
}

bool v8::Isolate::InContext(void)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    miniv8::V8Isolate* self = (miniv8::V8Isolate*)this;
    return self->isInContext();
}

bool v8::Isolate::IsDead(void)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return false;
}

bool v8::Isolate::IsExecutionTerminating(void)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return false;
}

bool v8::Isolate::WillAutorunMicrotasks(void) const
{
    //printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Message::IsOpaque(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Message::IsSharedCrossOrigin(void) const
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return false;
}

bool v8::Object::Delete(v8::Local<v8::Value> key)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyPtr = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*key);

    if (keyPtr->ctx() != self->ctx())
        DebugBreak();

    JSAtom prop = JS_ValueToAtom(keyPtr->ctx(), keyPtr->v(keyPtr));
    if (0 == prop)
        return false;
    return TRUE == JS_DeleteProperty(self->ctx(), self->v(self), prop, 0);
}

bool v8::Object::IsCallable(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Object::Set(unsigned int index, v8::Local<v8::Value> val)
{
    printEmptyFuncInfo(__FUNCTION__, false);

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* valPtr = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*val);

    if (valPtr->ctx() != self->ctx())
        DebugBreak();

    JSAtom value = JS_ValueToAtom(valPtr->ctx(), valPtr->v(valPtr));
    if (0 == value)
        return false;
    return TRUE == JS_SetPropertyUint32(self->ctx(), self->v(self), index, value);
}

bool v8::Object::Set(v8::Local<v8::Value> key, v8::Local<v8::Value> val)
{
    printEmptyFuncInfo(__FUNCTION__, false);

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyPtr = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*key);
    miniv8::V8Value* valPtr = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*val);

    if (valPtr->ctx() != self->ctx())
        DebugBreak();

    JSAtom prop = JS_ValueToAtom(valPtr->ctx(), keyPtr->v(keyPtr));
    if (0 == prop)
        return false;
    return TRUE == JS_SetProperty(self->ctx(), self->v(self), prop, valPtr->v(valPtr));
}

v8::Maybe<bool> v8::Object::ForceSet(v8::Local<v8::Context> context, v8::Local<v8::Value> key, v8::Local<v8::Value> value, v8::PropertyAttribute attribs)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    return Set(context, key, value);
}

v8::Maybe<bool> v8::Object::Set(v8::Local<v8::Context> context, unsigned int index, v8::Local<v8::Value> val)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* valPtr = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*val);

    miniv8::V8Context* ctx = (miniv8::V8Context*)(*context);
    if (ctx->ctx() != self->ctx())
        DebugBreak();

    if (valPtr->ctx() != self->ctx())
        DebugBreak();

    JSAtom value = JS_ValueToAtom(valPtr->ctx(), valPtr->v(valPtr));
    if (0 == value)
        return v8::Just<bool>(false);

    if (TRUE == JS_SetPropertyUint32(self->ctx(), self->v(self), index, value))
        v8::Just<bool>(true);
    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::Set(v8::Local<v8::Context> context, v8::Local<v8::Value> key, v8::Local<v8::Value> val)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyPtr = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*key);
    miniv8::V8Value* valPtr = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*val);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    if (ctx->ctx() != self->ctx())
        DebugBreak();

    if (valPtr->ctx() != self->ctx())
        DebugBreak();

    JSAtom prop = JS_ValueToAtom(valPtr->ctx(), keyPtr->v(keyPtr));
    if (0 == prop)
        return v8::Just<bool>(false);
    if (TRUE == JS_SetProperty(self->ctx(), self->v(self), prop, valPtr->v(valPtr)))
        v8::Just<bool>(true);

    return v8::Just<bool>(false);
}

namespace miniv8 {

JSValue wrapGetter(JSContext* ctx, JSValueConst thisVal, void* userdata)
{
    miniv8::V8Object* self = (miniv8::V8Object*)JS_GetUserdata(thisVal);
    uint32_t hashVal = (uint32_t)userdata;

    std::map<uint32_t, miniv8::V8Object::AccessorData*>::iterator it = self->m_accessorMap.find(hashVal);
    if (it != self->m_accessorMap.end())
        DebugBreak();

    miniv8::V8Object::AccessorData* accessor = it->second;

    JSValue propQjs = JS_NewStringLen(ctx, accessor->name.c_str(), accessor->name.size());
    miniv8::V8String prop(self->v8Ctx(), propQjs);
    v8::Local<v8::String> propV8 = v8::Utils::convert<miniv8::V8String, v8::String>(&prop);

    JSRuntime* runtime = JS_GetRuntime(ctx);
    miniv8::V8Isolate* isolate = miniv8::V8Isolate::GetCurrent();

    miniv8::V8Context* contextOfSelf = self->v8Ctx();
    miniv8::V8Context* context = (miniv8::V8Context*)JS_GetContextOpaque(ctx);
    miniv8ReleaseAssert(contextOfSelf == context, "wrapGetter fail\n");
    miniv8::V8Object* thisValV8 = miniv8::V8Object::create(context, thisVal); // TODO: free

    miniv8::V8Data dataV8(context, thisVal);

    void* returnValue[4] = { 0 };
    //returnValue[2]; // GetDefaultValue
    returnValue[1] = isolate;

    void* info[v8::PropertyCallbackInfo<v8::Value>::kArgsLength] = { 0 };
    info[1] = thisValV8; // kHolderIndex
    info[2] = isolate; // kIsolateIndex
    info[4] = &returnValue[3]; // kReturnValueIndex
    info[5] = &dataV8; // kDataIndex
    info[6] = thisValV8; // kThisIndex

    v8::PropertyCallbackInfo<v8::Value>* infoV8 = (v8::PropertyCallbackInfo<v8::Value>*)info;
    accessor->getter(propV8, *infoV8);

    v8::ReturnValue<v8::Value>* returnValueV8 = (v8::ReturnValue<v8::Value>*)(&returnValue[3]);
    v8::Local<v8::Value> retV8 = returnValueV8->Get();
    miniv8::V8Value* ret = (miniv8::V8Value*)(*retV8);

    return ret->v(ret);
}

JSValue wrapSetter(JSContext* ctx, JSValueConst thisVal, JSValueConst val, void* userdata)
{
    //typedef void (V8CALL* AccessorSetterCallback)(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info)
    miniv8::V8Object* self = (miniv8::V8Object*)JS_GetUserdata(thisVal);
    uint32_t hashVal = (uint32_t)userdata;

    std::map<uint32_t, miniv8::V8Object::AccessorData*>::iterator it = self->m_accessorMap.find(hashVal);
    if (it != self->m_accessorMap.end())
        DebugBreak();

    miniv8::V8Object::AccessorData* accessor = it->second;

    JSValue propQjs = JS_NewStringLen(ctx, accessor->name.c_str(), accessor->name.size());
    miniv8::V8String prop(self->v8Ctx(), propQjs);
    v8::Local<v8::String> propV8 = v8::Utils::convert<miniv8::V8String, v8::String>(&prop);

    miniv8::V8Isolate* isolate = miniv8::V8Isolate::GetCurrent();
    miniv8::V8Context* contextOfSelf = self->v8Ctx();
    miniv8::V8Context* context = (miniv8::V8Context*)JS_GetContextOpaque(ctx);
    miniv8ReleaseAssert(contextOfSelf == context, "wrapSetter fail\n");

    miniv8::V8Value value(context, val);
    v8::Local<v8::Value> valueV8 = v8::Utils::convert<miniv8::V8Value, v8::Value>(&value);

    miniv8::V8Object* thisValV8 = miniv8::V8Object::create(context, thisVal);
    miniv8::V8Data dataV8(context, thisVal);

    void* returnValue[4] = { 0 };
    //returnValue[2]; // GetDefaultValue
    returnValue[1] = &isolate;

    void* info[v8::PropertyCallbackInfo<void>::kArgsLength] = { 0 };
    info[1] = thisValV8; // kHolderIndex
    info[2] = &isolate; // kIsolateIndex
    info[4] = &returnValue[3]; // kReturnValueIndex
    info[5] = &dataV8; // kDataIndex
    info[6] = thisValV8; // kThisIndex

    v8::PropertyCallbackInfo<void>* infoV8 = (v8::PropertyCallbackInfo<void>*)info;
    accessor->setter(propV8, valueV8, *infoV8);

    return JS_UNDEFINED;
}

} // miniv8


v8::Maybe<bool> v8::Object::SetAccessor(
    v8::Local<v8::Context> context, 
    v8::Local<v8::Name> name,
    v8::AccessorNameGetterCallback getter,
    v8::AccessorNameSetterCallback setter,
    v8::MaybeLocal<v8::Value> data, 
    v8::AccessControl settings, 
    v8::PropertyAttribute attribute
    )
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Just<bool>(false);
}

bool v8::Object::SetAccessor(
    v8::Local<v8::Name>,
    void(__cdecl*)(v8::Local<v8::Name>, class v8::PropertyCallbackInfo<v8::Value> const&),
    void(__cdecl*)(v8::Local<v8::Name>, v8::Local<v8::Value>, v8::PropertyCallbackInfo<void> const&),
    v8::Local<v8::Value>,
    v8::AccessControl,
    v8::PropertyAttribute)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    return false;
}

bool v8::Object::SetAccessor(
    v8::Local<v8::String> name,
    v8::AccessorGetterCallback getter,
    v8::AccessorSetterCallback setter,
    v8::Local<v8::Value> data,
    v8::AccessControl settings,
    v8::PropertyAttribute attribute
    )
{
    printEmptyFuncInfo(__FUNCTION__, false);

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8String* namePtr = v8::Utils::openHandle<v8::String, miniv8::V8String>(*name);

    size_t nameLen = 0;
    const char* nameStr = JS_ToCStringLen(self->ctx(), &nameLen, namePtr->v(namePtr));
    uint32_t hashVal = miniv8::hash(nameStr, nameLen);

    if (self->m_accessorMap.find(hashVal) != self->m_accessorMap.end())
        DebugBreak();

    size_t oldSize = 0;
    if (self->m_props)
        oldSize = self->m_props->size();

    std::vector<JSCFunctionListEntry>* props = new std::vector<JSCFunctionListEntry>();
    props->resize(oldSize + 1);

    if (self->m_props) {
        memcpy(props->data(), self->m_props->data(), oldSize * sizeof(JSCFunctionListEntry));
        delete self->m_props;
    }
    self->m_props = props;

    uint8_t propFlags = JS_PROP_CONFIGURABLE;
    if (attribute & ALL_CAN_WRITE)
        propFlags |= JS_PROP_WRITABLE;

    JSCFunctionListEntry prop = { 0 };
    prop.name = nameStr;
    prop.prop_flags = propFlags;
    prop.def_type = JS_DEF_CGETSET;
    prop.magic = 0;
    prop.u.getset.get.getter = miniv8::wrapGetter;
    prop.u.getset.set.setter = miniv8::wrapSetter;
    prop.u.getset.userdata = (void*)(hashVal);

    self->m_props->push_back(prop);

    miniv8::V8Object::AccessorData* accessorData = new miniv8::V8Object::AccessorData(); // TODO delete
    accessorData->getter = getter;
    accessorData->setter = setter;
    accessorData->data = (miniv8::V8Value*)(*data);

    self->m_accessorMap.insert(std::make_pair(hashVal, accessorData));

//     if (0 == miniv8::V8Object::s_classId) {
//         JS_NewClassID(&miniv8::V8Object::s_classId);
//         JS_SetClassProto(self->ctx(), miniv8::V8Object::s_classId, self->v(self));
//         JS_SetUserdata(self->v(self), self);
//     }
    JS_SetPropertyFunctionList(self->ctx(), self->v(self), self->m_props->data(), self->m_props->size());

    JS_FreeCString(self->ctx(), nameStr);

    return true;
}

v8::Maybe<bool> v8::Object::CreateDataProperty(v8::Local<v8::Context> context, unsigned int index, v8::Local<v8::Value> val)
{
    printEmptyFuncInfo(__FUNCTION__, false);

    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* value = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*val);

    int ret = JS_SetPropertyUint32(ctx->ctx(), self->v(self), index, value->v(value));
    if (TRUE == ret)
        return v8::Just<bool>(true);

    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::CreateDataProperty(v8::Local<v8::Context>context, v8::Local<v8::Name> key, v8::Local<v8::Value> val)
{
    printEmptyFuncInfo(__FUNCTION__, false);

    if (key.IsEmpty() || val.IsEmpty()) // TODO:V8Document::preparePrototypeObject导致这里可能是空的
        return v8::Just<bool>(false);

    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyQjs = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*key);
    miniv8::V8Value* valueQjs = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*val);

    JSAtom keyAtom = JS_ValueToAtom(ctx->ctx(), keyQjs->v(keyQjs));

    int ret = JS_SetProperty(ctx->ctx(), self->v(self), keyAtom, valueQjs->v(valueQjs));
    if (TRUE == ret)
        return v8::Just<bool>(true);
    
    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::DefineOwnProperty(v8::Local<v8::Context> context, v8::Local<v8::Name> key, v8::Local<v8::Value> value, v8::PropertyAttribute attributes)
{
    printEmptyFuncInfo(__FUNCTION__, false);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyQjs = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*key);
    miniv8::V8Value* val = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*value);

    size_t keyStrLen = 0;
    const char* keyStr = JS_ToCStringLen(self->ctx(), &keyStrLen, keyQjs->v(keyQjs));
    
    int flag = 0;
    if (0 == (attributes & v8::ReadOnly))
        flag |= JS_PROP_WRITABLE;
    if (0 == (attributes & v8::DontEnum))
        flag |= JS_PROP_ENUMERABLE;
    if (0 == (attributes & v8::DontDelete))
        flag |= JS_PROP_CONFIGURABLE;

    int ret = JS_DefinePropertyValueStr(ctx->ctx(), self->v(self), keyStr, val->v(val), flag);

    JS_FreeCString(self->ctx(), keyStr);
    if (TRUE == ret)
        return v8::Just<bool>(true);

    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::Delete(v8::Local<v8::Context> context, unsigned int index)
{
    printEmptyFuncInfo(__FUNCTION__, true);
//     miniv8::V8Context* ctx = (miniv8::V8Context*)(*context);
//     JS_NewInt64(ctx->ctx(), index);

    return v8::Just<bool>(false);
    
}

v8::Maybe<bool> v8::Object::Delete(v8::Local<v8::Context> context, v8::Local<v8::Value> key)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyQjs = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*key);

    JSAtom prop = JS_ValueToAtom(ctx->ctx(), keyQjs->v(keyQjs));

    int ret = JS_DeleteProperty(ctx->ctx(), self->v(self), prop, 0);
    if (TRUE == ret)
        return v8::Just<bool>(true);

    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::DeletePrivate(v8::Local<v8::Context> context, v8::Local<v8::Private> key)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyQjs = v8::Utils::openHandle<v8::Private, miniv8::V8Value>(*key);

    JSAtom prop = JS_ValueToAtom(ctx->ctx(), keyQjs->v(keyQjs));

    int ret = JS_DeleteProperty(ctx->ctx(), self->v(self), prop, 0);
    if (TRUE == ret)
        return v8::Just<bool>(true);

    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::Has(v8::Local<v8::Context> context, unsigned int index)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::Has(v8::Local<v8::Context> context, v8::Local<v8::Value> key)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyQjs = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*key);

    JSAtom prop = JS_ValueToAtom(ctx->ctx(), keyQjs->v(keyQjs));

    int ret = JS_HasProperty(ctx->ctx(), self->v(self), prop);
    if (TRUE == ret)
        return v8::Just<bool>(true);

    return v8::Just<bool>(false);
}


v8::Maybe<bool> v8::Object::HasOwnProperty(v8::Local<v8::Context> context, v8::Local<v8::Name> key)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    // TODO：V8Document::preparePrototypeObject -》 v8::Symbol::GetUnscopables，这个key可能是空的
    if (key.IsEmpty())
        return v8::Just<bool>(false);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyQjs = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*key);

    JSAtom prop = JS_ValueToAtom(ctx->ctx(), keyQjs->v(keyQjs));
    int ret = JS_HasProperty(ctx->ctx(), self->v(self), prop);
    if (TRUE == ret)
        return v8::Just<bool>(true);
    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::HasPrivate(v8::Local<v8::Context> context, v8::Local<v8::Private> key)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* keyQjs = v8::Utils::openHandle<v8::Private, miniv8::V8Value>(*key);

    JSAtom prop = JS_ValueToAtom(ctx->ctx(), keyQjs->v(keyQjs));

    int ret = JS_HasProperty(ctx->ctx(), self->v(self), prop);
    if (TRUE == ret)
        return v8::Just<bool>(true);

    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::HasRealIndexedProperty(v8::Local<v8::Context> context, unsigned int index)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::HasRealNamedCallbackProperty(v8::Local<v8::Context>, v8::Local<v8::Name>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::HasRealNamedProperty(v8::Local<v8::Context>, v8::Local<v8::Name>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Just<bool>(false);
}

v8::Maybe<bool> v8::Object::SetPrivate(v8::Local<v8::Context>, v8::Local<v8::Private>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Just<bool>(false);
}

v8::MaybeLocal<v8::Value> v8::Object::CallAsFunction(v8::Local<v8::Context>, v8::Local<v8::Value>, int, v8::Local<v8::Value>* const)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Value>();
}

v8::MaybeLocal<v8::Value> v8::Object::Get(v8::Local<v8::Context>, unsigned int index)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Value>();
}

v8::MaybeLocal<v8::Value> v8::Object::Get(v8::Local<v8::Context> context, v8::Local<v8::Value> key)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8String* keyQjs = v8::Utils::openHandle<v8::Value, miniv8::V8String>(*key);
    if (keyQjs->m_type != miniv8::kObjectTypeString)
        return v8::MaybeLocal<v8::Value>();

    miniv8::V8Isolate* isolate = miniv8::V8Isolate::GetCurrent();
    isolate->enterContext(ctx);

    JSAtom prop = JS_ValueToAtom(ctx->ctx(), keyQjs->v(keyQjs));
    JSValue val = JS_GetProperty(ctx->ctx(), self->v(self), prop);

    isolate->exitContext();

    miniv8::V8Object* v = miniv8::V8Object::create(ctx, val); // TODO: 放到句柄域
    v8::Local<v8::Object> ret = v8::Utils::convert<miniv8::V8Object, v8::Object>(v);

    return v8::MaybeLocal<v8::Value>(ret);
}

v8::MaybeLocal<v8::Value> v8::Object::GetOwnPropertyDescriptor(v8::Local<v8::Context>, v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Value>();
}

v8::MaybeLocal<v8::Value> v8::Object::GetPrivate(v8::Local<v8::Context>, v8::Local<v8::Private>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Value>();
}

v8::MaybeLocal<v8::Value> v8::Object::GetRealNamedProperty(v8::Local<v8::Context>, v8::Local<v8::Name>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Value>();
}

void v8::Object::SetAccessorProperty(v8::Local<v8::Name>, v8::Local<v8::Function>, v8::Local<v8::Function>, v8::PropertyAttribute, v8::AccessControl)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::MaybeLocal<v8::Array> v8::Object::GetOwnPropertyNames(v8::Local<v8::Context>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Array>();
}

v8::MaybeLocal<v8::Array> v8::Object::GetPropertyNames(v8::Local<v8::Context>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Array>();
}

int v8::Object::GetIdentityHash(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Object::InternalFieldCount(void)
{
    //printEmptyFuncInfo(__FUNCTION__, true);
//     return self->m_alignedPointerInInternalFields.size();

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);

    JSValue v = self->v(self);

    printDebug("v8::Object::InternalFieldCount: %p, %d, %d\n", self, self->m_internalFieldCount, JS_IsUndefined(v));
    return self->m_internalFieldCount;
}

v8::Local<v8::Value> v8::Object::SlowGetInternalField(int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    return v8::Local<v8::Value>();
}

void v8::Object::SetInternalField(int index, v8::Local<v8::Value> value)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
}

void v8::Object::SetAlignedPointerInInternalField(int index, void* value)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);

    printDebug("v8::Object::SetAlignedPointerInInternalField: %p, index:%d\n", self, index);

    self->m_alignedPointerInInternalFields.insert(std::make_pair(index, value));
}

void* v8::Object::SlowGetAlignedPointerFromInternalField(int index)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    std::map<int, void*>::iterator it = self->m_alignedPointerInInternalFields.find(index);
    if (it != self->m_alignedPointerInInternalFields.end())
        return it->second;

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);

    JSValueConst prototype = JS_GetPrototype(ctx->ctx(), self->v(self));
    miniv8::V8Object* prototypeObj = (miniv8::V8Object*)JS_GetUserdata(prototype);
    if (prototypeObj) {
        it = prototypeObj->m_alignedPointerInInternalFields.find(index);
        if (it != prototypeObj->m_alignedPointerInInternalFields.end())
            return it->second;
    }

    // 见WindowProxy::installDOMWindow()，要查找原型的原型
    prototype = JS_GetPrototype(ctx->ctx(), prototype);
    prototypeObj = (miniv8::V8Object*)JS_GetUserdata(prototype);
    if (!prototypeObj)
        return nullptr;

    it = prototypeObj->m_alignedPointerInInternalFields.find(index);
    if (it != prototypeObj->m_alignedPointerInInternalFields.end())
        return it->second;

    return nullptr;
}

v8::Maybe<bool> v8::Object::SetPrototype(v8::Local<v8::Context> context, v8::Local<v8::Value> protoValue)
{
    //printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    miniv8::V8Value* protoVal = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*protoValue);

    if (protoVal->m_type == miniv8::kObjectTypeObject) {
        miniv8::V8Object* protoObj = (miniv8::V8Object*)protoVal;

        miniv8ReleaseAssert(
            0 == self->m_alignedPointerInInternalFields.size() ||
            protoObj->m_alignedPointerInInternalFields.size() == self->m_alignedPointerInInternalFields.size(),
            "v8::Object::SetPrototype fail\n");
        self->m_alignedPointerInInternalFields = protoObj->m_alignedPointerInInternalFields;
    }

    JSValueConst protoV = protoVal->v(protoVal);
    JSValue selfV = self->v(self);

    JS_BOOL b = JS_IsUndefined(protoV);
    int ret = JS_SetPrototype(ctx->ctx(), selfV, protoV);

    if (-1 == ret) { // -1有可能是protoV的原型链上，已经有v了。一般出现在global.prototype再设置prototype的时候
        JSValue global = JS_GetGlobalObject(ctx->ctx());
        JSValue globalPrototype = JS_GetPrototype(ctx->ctx(), global);
        if (globalPrototype == selfV) {
            miniv8::V8Object* protoObj = (miniv8::V8Object*)protoVal;
            protoObj = (miniv8::V8Object*)JS_GetUserdata(protoV); // test

            // 这里不规范，不过暂时先这么搞搞，把global.prototype.prototype = protoV 变成 global.prototype = protoV
            printDebug("v8::Object::SetPrototype!: %I64u\n", protoV);
            ret = JS_SetPrototype(ctx->ctx(), global, protoV);            
        }
    }

    if (TRUE == ret)
        return v8::Just<bool>(true);

    return v8::Just<bool>(false);
}

v8::Local<v8::Value> v8::Object::GetPrototype(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    JSContext* ctx = self->ctx();
    miniv8::V8Context* v8ctx = self->v8Ctx();
    if (!ctx)
        DebugBreak();
    JSValue v = miniv8::V8Data::V8Data_v(self);
    //JSValue proto = JS_GetPropertyStr(ctx, v, "prototype");
    JSValue proto = JS_GetPrototype(ctx, v);
    //     if (JS_IsException(proto))
    //         return proto;

    if (JS_IsUndefined(proto)) {
        proto = JS_NewObject(ctx);
        //JS_SetPropertyStr(ctx, v, "prototype", proto);
        JS_SetPrototype(ctx, v, proto);
        DebugBreak();
    }

    printDebug("v8::Object::GetPrototype: %I64u\n", proto);

    miniv8::V8Object* obj = miniv8::V8Object::create(v8ctx, proto); // 这里返回的对象，可以是复用的
    obj->m_internalFieldCount = self->m_internalFieldCount;
    v8::Local<v8::Object> ret = v8::Utils::convert<miniv8::V8Object, v8::Object>(obj);
    return ret;
}

v8::Local<v8::Object> v8::Object::Clone(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    v8::Local<v8::Object> ret = v8::Utils::convert<miniv8::V8Object, v8::Object>(self); // TODO; 这鬼函数有点奇葩
    return ret;
}

v8::Local<v8::Object> v8::Object::FindInstanceInPrototypeChain(v8::Local<v8::FunctionTemplate> templ)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8ReleaseAssert(templ.IsEmpty(), "v8::Object::FindInstanceInPrototypeChain fail\n"); // TODO; 这鬼函数有点奇葩

    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    v8::Local<v8::Object> ret = v8::Utils::convert<miniv8::V8Object, v8::Object>(self);
    return ret;
}

v8::Local<v8::Object> v8::Object::New(v8::Isolate* isolate)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    JSValue val = JS_NewObject(ctx->ctx());

    miniv8::V8Object* v = new miniv8::V8Object(ctx, val); // TODO: 放到句柄域
    v8::Local<v8::Object> ret = v8::Utils::convert<miniv8::V8Object, v8::Object>(v);

    return ret;
}

v8::Local<v8::Array> v8::Object::GetOwnPropertyNames(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Array>();
}

v8::Local<v8::Array> v8::Object::GetPropertyNames(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Array>();
}

v8::Isolate* v8::Object::GetIsolate(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    return (v8::Isolate*)(self->v8Ctx()->isolate());
}

v8::Local<v8::Context> v8::Object::CreationContext(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Object* self = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(this);
    if (self->m_ctx)
        return v8::Utils::convert<miniv8::V8Context, v8::Context>(self->m_ctx);
    return v8::Local<v8::Context>();
}

v8::Local<v8::String> v8::Object::GetConstructorName(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::Value> v8::Object::Get(unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Object::Get(v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

// v8::Local<v8::Value> v8::Object::GetInternalField(int)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
//     return v8::Local<v8::Value>();
// }

v8::Maybe<v8::PropertyAttribute> v8::Object::GetPropertyAttributes(v8::Local<v8::Context>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Just<v8::PropertyAttribute>(v8::ReadOnly);
}

v8::Maybe<v8::PropertyAttribute> v8::Object::GetRealNamedPropertyAttributes(v8::Local<v8::Context>, v8::Local<v8::Name>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Just<v8::PropertyAttribute>(v8::ReadOnly);
}

v8::MaybeLocal<v8::String> v8::Object::ObjectProtoToString(v8::Local<v8::Context>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::String>();
}

bool v8::Promise::HasHandler(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::StackFrame::IsEval(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::String::CanMakeExternal(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::String::ContainsOnlyOneByte(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8String* self = v8::Utils::openHandle<v8::String, miniv8::V8String>(this);

//     if (!self->m_str.empty()) {
// 
//     }
// 
//     size_t len = 0;
//     const char* str = JS_ToCStringLen(self->ctx(), &len, self->v(self));
    return true;
}

bool v8::String::IsExternal(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::String::IsExternalOneByte(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return true;
}

bool v8::String::IsOneByte(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return true;
}

bool v8::String::MakeExternal(v8::String::ExternalOneByteStringResource* resource)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    miniv8::V8String* self = v8::Utils::openHandle<v8::String, miniv8::V8String>(this);
    self->m_externalString = resource;
    return false;
}

bool v8::String::MakeExternal(v8::String::ExternalStringResource* resource)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    miniv8::V8String* self = v8::Utils::openHandle<v8::String, miniv8::V8String>(this);
    self->m_externalString = resource;
    return false;
}

v8::String::ExternalStringResource* v8::String::GetExternalStringResource() const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8String* self = v8::Utils::openHandle<v8::String, miniv8::V8String>(this);
    return (v8::String::ExternalStringResource*)self->m_externalString;
}

v8::String::ExternalStringResourceBase* v8::String::GetExternalStringResourceBase(v8::String::Encoding* encoding_out) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8String* self = v8::Utils::openHandle<v8::String, miniv8::V8String>(this);
    return self->m_externalString;
}

int v8::String::Length(void) const
{
    //TODO: 这函数要搞个缓存，不然影响效率
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8String* self = v8::Utils::openHandle<v8::String, miniv8::V8String>(this);
    size_t len = 0;

    std::string str = self->getStr();
    int length = miniv8::getUtf8Length(str.c_str(), str.size());

    return length;
}

int v8::String::Utf8Length(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8String* self = v8::Utils::openHandle<v8::String, miniv8::V8String>(this);

    std::string str = self->getStr();
    return str.size();
}

int v8::String::Write(uint16_t* buffer, int start, int length, int options) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    //     miniv8::V8Value* self = (miniv8::V8Value*)this;
    //     size_t len = 0;
    // 
    //     const char* str = JS_ToCStringLen2(self->ctx(), &len, self->v(self), FALSE);
    // 
    //     std::string bufUtf8 = miniv8::utf16ToUtf8(std::u16string(buffer, length));
    // 
    //     JSValue JS_NewStringLen(JSContext * ctx, const char* str1, size_t len1);
    return 0;
}

int v8::String::WriteOneByte(uint8_t* buffer, int start, int length, int options) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8String* self = v8::Utils::openHandle<v8::String, miniv8::V8String>(this);
    size_t len = 0;

    const std::string& str = self->getStr();
    memcpy(buffer + start, str.c_str(), length); // TODO: start+ length 没考虑溢出的情况
    return start;
}

int v8::String::WriteUtf8(char*, int, int*, int) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

v8::Local<v8::String> v8::String::Concat(v8::Local<v8::String>, v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::String::NewFromOneByte(v8::Isolate*, unsigned char const*, v8::String::NewStringType, int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::String::NewFromTwoByte(v8::Isolate*, unsigned short const*, v8::String::NewStringType, int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::String::NewFromUtf8(v8::Isolate*, char const*, v8::String::NewStringType, int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::MaybeLocal<v8::String> v8::String::NewFromUtf8(v8::Isolate* isolate, const char* str, v8::NewStringType type, int len)
{
    printEmptyFuncInfo(__FUNCTION__, false);

    miniv8::V8Isolate* iso = (miniv8::V8Isolate*)isolate;
    v8::Local<v8::String> ret = v8::Utils::convert<miniv8::V8String, v8::String>(new miniv8::V8String(str)); // TODO free

    return ret;
}

v8::MaybeLocal<v8::String> v8::String::NewExternalOneByte(v8::Isolate* isolate, v8::String::ExternalOneByteStringResource* resource)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    // TODO:编码还没考虑
    if (!resource)
        return v8::MaybeLocal<v8::String>();         

    std::string str(resource->data(), resource->length());
    v8::Local<v8::String> ret = v8::Utils::convert<miniv8::V8String, v8::String>(new miniv8::V8String(str)); // TODO free
    return ret;
}

v8::MaybeLocal<v8::String> v8::String::NewExternalTwoByte(v8::Isolate*, v8::String::ExternalStringResource*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::String>();
}

v8::MaybeLocal<v8::String> v8::String::NewFromOneByte(v8::Isolate*, unsigned char const*, v8::NewStringType, int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::String>();
}

v8::MaybeLocal<v8::String> v8::String::NewFromTwoByte(v8::Isolate*, unsigned short const*, v8::NewStringType, int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::String>();
}

const v8::String::ExternalOneByteStringResource* v8::String::GetExternalOneByteStringResource(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::String::Utf8Value::Utf8Value(v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::String::Utf8Value::~Utf8Value(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::String::Value::Value(v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::String::Value::~Value(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

bool v8::TryCatch::CanContinue(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::TryCatch::HasCaught(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::TryCatch::HasTerminated(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

// bool v8::V8::AddMessageListener(void(__cdecl*)(v8::Local<v8::Message>, v8::Local<v8::Value>), v8::Local<v8::Value>)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
//     return false;
// }

bool v8::V8::Initialize(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

// bool v8::V8::IsDead(void)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
//     return false;
// }

// bool v8::V8::IsExecutionTerminating(v8::Isolate*)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
//     return false;
// }

bool v8::Value::BooleanValue(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    int ret = JS_ToBool(self->ctx(), self->v(self));
    if (TRUE == ret)
        return true;
    return false;
}

bool v8::Value::Equals(v8::Local<v8::Value> other) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    miniv8::V8Value* otherPtr = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*other);

    return self->v(self) == otherPtr->v(otherPtr);
}

bool v8::Value::IsArgumentsObject(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsArray(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    int ret = JS_IsArray(self->ctx(), self->v(self));
    return ret == TRUE;
}

bool v8::Value::IsArrayBuffer(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    int ret = JS_IsArray(self->ctx(), self->v(self));
    return ret == TRUE;
}

bool v8::Value::IsArrayBufferView(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    int ret = JS_IsArray(self->ctx(), self->v(self));
    return ret == TRUE;
}

bool v8::Value::IsBoolean(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return JS_IsBool(self->v(self)) == TRUE;
}

bool v8::Value::IsBooleanObject(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return JS_IsBool(self->v(self)) == TRUE;
}

bool v8::Value::IsDataView(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return false;
}

bool v8::Value::IsDate(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsExternal(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsFloat32Array(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsFloat64Array(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsFunction(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = (miniv8::V8Value*)this;
    return TRUE  == JS_IsFunction(self->ctx(), self->v(self));
}

bool v8::Value::IsGeneratorFunction(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsGeneratorObject(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsInt16Array(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsInt32(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return JS_IsInteger(self->v(self)) == TRUE;
}

bool v8::Value::IsInt32Array(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsInt8Array(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsMap(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsMapIterator(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsNativeError(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsNumber(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return JS_IsNumber(self->v(self)) == TRUE;
}

bool v8::Value::IsNumberObject(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return JS_IsNumber(self->v(self)) == TRUE;
}

bool v8::Value::IsObject(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return self->isObject;
}

bool v8::Value::IsPromise(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsProxy(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsRegExp(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsSet(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsSetIterator(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsSharedArrayBuffer(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsStringObject(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return JS_IsString(self->v(self)) == TRUE;
}

bool v8::Value::IsSymbol(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsSymbolObject(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsFalse(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    if (JS_IsBool(self->v(self)) == TRUE) {
        int ret = JS_ToBool(self->ctx(), self->v(self));
        if (FALSE == ret)
            return true;
    }
    return false;
}

bool v8::Value::IsTrue(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    if (JS_IsBool(self->v(self)) == TRUE) {
        int ret = JS_ToBool(self->ctx(), self->v(self));
        if (TRUE == ret)
            return true;
    }
    return false;
}

bool v8::Value::IsTypedArray(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsUint16Array(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsUint32(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    return JS_IsInteger(self->v(self)) == TRUE;
}

bool v8::Value::IsUint32Array(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsUint8Array(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsUint8ClampedArray(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsWeakMap(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::IsWeakSet(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

bool v8::Value::StrictEquals(v8::Local<v8::Value>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

double v8::Date::ValueOf(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

double v8::Number::Value(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Number, miniv8::V8Value>(this);
    double res = 0;
    int ret = JS_ToFloat64(self->ctx(), &res, self->v(self));
    if (TRUE == ret)
        return res;
    return 0;
}

double v8::NumberObject::ValueOf(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::NumberObject, miniv8::V8Value>(this);
    double res = 0;
    int ret = JS_ToFloat64(self->ctx(), &res, self->v(self));
    if (TRUE == ret)
        return res;
    return 0;
}

double v8::Value::NumberValue(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    double res = 0;
    int ret = JS_ToFloat64(self->ctx(), &res, self->v(self));
    if (TRUE == ret)
        return res;
    return 0;
}

v8::RegExp::Flags v8::RegExp::GetFlags(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::RegExp::kNone;
}

int v8::Function::GetScriptColumnNumber(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Function::GetScriptLineNumber(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Function::ScriptId(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Int32::Value(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Int32, miniv8::V8Value>(this);
    int32_t res = 0;
    int ret = JS_ToInt32(self->ctx(), &res, self->v(self));
    if (TRUE == ret)
        return res;
    return 0;
}

int v8::Isolate::ContextDisposedNotification(bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Message::GetEndColumn(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Message::GetEndPosition(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Message::GetLineNumber(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Message::GetStartColumn(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Message::GetStartPosition(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::StackFrame::GetColumn(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::StackFrame::GetLineNumber(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::StackFrame::GetScriptId(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::StackTrace::GetFrameCount(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::UnboundScript::GetId(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::Value::Int32Value(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    int32_t res = 0;
    int ret = JS_ToInt32(self->ctx(), &res, self->v(self));
    if (TRUE == ret)
        return res;
    return 0;
}

v8::Local<v8::Value> v8::V8::GetEternal(v8::Isolate*, int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::internal::Object** v8::EscapableHandleScope::Escape(v8::internal::Object**)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

void* v8::V8::ClearWeak(v8::internal::Object**)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

void v8::Isolate::ReportExternalAllocationLimitReached(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetObjectGroupId(v8::internal::Object**, v8::UniqueId)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetReference(v8::internal::Object**, v8::internal::Object**)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetReferenceFromGroup(v8::UniqueId, v8::internal::Object**)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::V8::DisposeGlobal(v8::internal::Object**)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::V8::Eternalize(v8::Isolate*, v8::Value*, int*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::V8::FromJustIsNothing(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::V8::MakeWeak(v8::internal::Object**, void*, void(__cdecl*)(v8::WeakCallbackInfo<void> const&), v8::WeakCallbackType)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::V8::ToLocalEmpty(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

// TODO：全局引用，把一些local的也放到全局句柄里
v8::internal::Object** v8::V8::GlobalizeReference(v8::internal::Isolate* isolate, v8::internal::Object** obj)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Isolate* iso = (miniv8::V8Isolate*)isolate;
    void** index = iso->findGlobalizeHandleEmptyIndex();
    miniv8::V8ObjectType type = (miniv8::V8ObjectType)(**(uint32_t**)obj);
    void* ret = nullptr;
    switch (type) {
    case miniv8::kObjectTypeValue: {
        miniv8::V8Value* ptr = (miniv8::V8Value*)*obj;
        ret = ptr;// new miniv8::V8FunctionTemplate(*ptr);
        break;
    }
    case miniv8::kObjectTypeString: {
        miniv8::V8String* ptr = (miniv8::V8String*)*obj;
        ret = ptr;// new miniv8::V8FunctionTemplate(*ptr);
        break;
    }
    case miniv8::kObjectTypeContext: {
        miniv8::V8Context* ctx = (miniv8::V8Context*)*obj;
        ret = ctx;// new miniv8::V8Context(*ctx);
        break;
    }
    case miniv8::kObjectTypeData: {
        miniv8::V8Data* ptr = (miniv8::V8Data*)*obj;
        ret = ptr;// new miniv8::V8Data(*ptr);
        break;
    }
    case miniv8::kObjectTypeObject: {
        miniv8::V8Object* ptr = (miniv8::V8Object*)*obj;
        ret = ptr;// new miniv8::V8Object(*ptr);
        break;
    }
    case miniv8::kObjectTypeFunction: {
        miniv8::V8Function* ptr = (miniv8::V8Function*)*obj;
        ret = ptr;// new miniv8::V8Function(*ptr);
        break;
    }
    case miniv8::kObjectTypeObjectTemplate: {
        miniv8::V8ObjectTemplate* ptr = (miniv8::V8ObjectTemplate*)*obj;
        ret = ptr;// new miniv8::V8ObjectTemplate(*ptr);
        break;
    }
    case miniv8::kObjectTypeFunctionTemplate: {
        miniv8::V8FunctionTemplate* ptr = (miniv8::V8FunctionTemplate*)*obj;
        ret = ptr;// new miniv8::V8FunctionTemplate(*ptr);
        break;
    }
    default:
        DebugBreak();
        break;
    }
    *index = ret;

//     char* output = (char*)malloc(0x500);
//     sprintf_s(output, 0x500 - 1, "GlobalizeReference: %p %p\n", index, ret);
//     OutputDebugStringA(output);
//     free(output);

    //printDebug("GlobalizeReference: %p %p\n", index, ret);

    return (v8::internal::Object **)index;
}

v8::internal::Object** v8::HandleScope::CreateHandle(v8::internal::HeapObject*, v8::internal::Object*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    return nullptr;
}

// TODO：局部引用，把一些local的也放到isoloate局部引用表里
v8::internal::Object** v8::HandleScope::CreateHandle(v8::internal::Isolate* isolate, v8::internal::Object* obj)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Isolate* iso = (miniv8::V8Isolate*)isolate;
    void** index = iso->findHandleScopeEmptyIndex();
    miniv8::V8ObjectType type = (miniv8::V8ObjectType)(*(uint32_t*)obj);
    void* ret = nullptr;
    switch (type) {
    case miniv8::kObjectTypeContext: {
        miniv8::V8Context* ptr = (miniv8::V8Context*)obj;
        ret = ptr;// new miniv8::V8Context(*ctx);
        break;
    }
    case miniv8::kObjectTypeData: {
        miniv8::V8Data* ptr = (miniv8::V8Data*)obj;
        ret = ptr;// new miniv8::V8Data(*ptr);
        break;
    }
    case miniv8::kObjectTypeString: {
        miniv8::V8String* ptr = (miniv8::V8String*)obj;
        ret = ptr;// new miniv8::V8FunctionTemplate(*ptr);
        break;
    }
    case miniv8::kObjectTypeObjectTemplate: {
        miniv8::V8ObjectTemplate* ptr = (miniv8::V8ObjectTemplate*)obj;
        ret = ptr;// new miniv8::V8ObjectTemplate(*ptr);
        break;
    }
    case miniv8::kObjectTypeFunctionTemplate: {
        miniv8::V8FunctionTemplate* ptr = (miniv8::V8FunctionTemplate*)obj;
        ret = ptr;// new miniv8::V8FunctionTemplate(*ptr);
        break;
    }
    case miniv8::kObjectTypeFunction: {
        miniv8::V8Function* ptr = (miniv8::V8Function*)obj;
        ret = ptr;// new miniv8::V8FunctionTemplate(*ptr);
        break;
    }
    case miniv8::kObjectTypeObject: {
        miniv8::V8Object* ptr = (miniv8::V8Object*)obj;
        ret = ptr;// new miniv8::V8FunctionTemplate(*ptr);
        break;
    }
    default:
        DebugBreak();
        break;
    }
    *index = ret;

    char* output = (char*)malloc(0x100);
    sprintf(output, "HandleScope::CreateHandle: %p %p\n", index, ret);
    OutputDebugStringA(output);
    free(output);

    return (v8::internal::Object **)index;
}

const v8::ScriptCompiler::CachedData * v8::ScriptCompiler::StreamedSource::GetCachedData(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

unsigned int v8::Array::Length(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::ArrayBuffer::ByteLength(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::ArrayBufferView::ByteLength(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::ArrayBufferView::ByteOffset(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::CpuProfileNode::GetCallUid(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::CpuProfileNode::GetHitCount(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::CpuProfileNode::GetHitLineCount(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::CpuProfileNode::GetNodeId(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::HeapProfiler::GetHeapStats(v8::OutputStream*, __int64*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::HeapProfiler::GetObjectId(v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::Isolate::NumberOfHeapSpaces(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::TypedArray::Length(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

unsigned int v8::Uint32::Value(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Uint32, miniv8::V8Value>(this);
    int32_t res = 0;
    int ret = JS_ToInt32(self->ctx(), &res, self->v(self));
    if (TRUE == ret)
        return res;
    return 0;
}

unsigned int v8::Value::Uint32Value(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    uint32_t res = 0;
    int ret = JS_ToUint32(self->ctx(), &res, self->v(self));
    if (TRUE == ret)
        return res;
    return 0;
}

v8::ArrayBuffer::Contents v8::ArrayBuffer::Externalize(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::ArrayBuffer::Contents();
}

v8::ArrayBuffer::Contents v8::ArrayBuffer::GetContents(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::ArrayBuffer::Contents();
}

v8::CpuProfile* v8::CpuProfiler::StopProfiling(v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::CpuProfileNode const* v8::CpuProfile::GetSample(int) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::CpuProfileNode const* v8::CpuProfile::GetTopDownRoot(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::CpuProfileNode const* v8::CpuProfileNode::GetChild(int) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::CpuProfiler* v8::Isolate::GetCpuProfiler(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::EscapableHandleScope::EscapableHandleScope(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::HandleScope::HandleScope(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::HandleScope::~HandleScope(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::HeapProfiler* v8::Isolate::GetHeapProfiler(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::HeapSnapshot const* v8::HeapProfiler::TakeHeapSnapshot(v8::ActivityControl*, v8::HeapProfiler::ObjectNameResolver*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::HeapSpaceStatistics::HeapSpaceStatistics(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::HeapStatistics::HeapStatistics(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::Isolate* v8::Context::GetIsolate(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Context* self = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(this);
    return (v8::Isolate*)self->isolate();
}

v8::Isolate* v8::Isolate::GetCurrent(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return (v8::Isolate*)(miniv8::V8Isolate::GetCurrent());
}

v8::Isolate* v8::Isolate::New(struct v8::Isolate::CreateParams const&)
{
    miniv8::V8Isolate* isolate = new miniv8::V8Isolate();
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8ReleaseAssert((v8::Isolate*)isolate == v8::Isolate::GetCurrent(), "v8::Isolate::New fail\n");
    return (v8::Isolate*)isolate;
}

// v8::Isolate::CreateParams::CreateParams(void)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

v8::Local<v8::AccessorSignature> v8::AccessorSignature::New(v8::Isolate* v8Isolate, v8::Local<v8::FunctionTemplate> functionTemplate)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    //return v8::Utils::convert<miniv8::V8AccessorSignature, v8::AccessorSignature>(new V8AccessorSignature());
    return v8::Local<v8::AccessorSignature>();
}

v8::Local<v8::Array> v8::Array::New(v8::Isolate* v8Isolate, int length)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Isolate* isolate = (miniv8::V8Isolate*)v8Isolate;
    
    miniv8::V8Context* ctx = isolate->getCurrentCtx();

    JSValue val = JS_NewArray(ctx->ctx());
    return v8::Utils::convert<miniv8::V8Array, v8::Array>(new miniv8::V8Array(ctx, val));
}

v8::Local<v8::Array> v8::Map::AsArray(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Array>();
}

v8::Local<v8::Array> v8::Set::AsArray(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Array>();
}

v8::Local<v8::Array> v8::StackTrace::AsArray(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Array>();
}

v8::Local<v8::ArrayBuffer> v8::ArrayBuffer::New(v8::Isolate*, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::ArrayBuffer>();
}

v8::Local<v8::ArrayBuffer> v8::ArrayBuffer::New(v8::Isolate*, void*, unsigned int, v8::ArrayBufferCreationMode)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::ArrayBuffer>();
}

v8::Local<v8::ArrayBuffer> v8::ArrayBufferView::Buffer(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::ArrayBuffer>();
}

v8::Local<v8::Boolean> v8::Value::ToBoolean(v8::Isolate*) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Boolean>();
}

v8::Local<v8::Context> v8::Context::New(v8::Isolate* isolate, v8::ExtensionConfiguration*, v8::MaybeLocal<v8::ObjectTemplate> globalTemplate, v8::MaybeLocal<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Isolate* iso = (miniv8::V8Isolate*)isolate;
    JSContext* ctx = JS_NewContext(iso->getRuntime());
    miniv8::V8Context* ret = new miniv8::V8Context(iso, ctx);

    JSValue global = JS_GetGlobalObject(ctx);
    miniv8::V8Object* globalObj = miniv8::V8Object::create(ret, global);

    miniv8::V8ObjectTemplate* globalTempl = v8::Utils::openHandle<v8::ObjectTemplate, miniv8::V8ObjectTemplate>(*(globalTemplate.ToLocalChecked()));
    globalTempl->newTemplateInstance(ret, globalObj);
    // TODO: 没考虑m_instanceTemplate等情况

    return v8::Utils::convert<miniv8::V8Context, v8::Context>(ret);
}

v8::Local<v8::Object> v8::Context::Global(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Context* self = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(this);
    JSValue val = JS_GetGlobalObject(self->ctx());
    miniv8::V8Object* ret = miniv8::V8Object::create(self, val);
    return v8::Utils::convert<miniv8::V8Object, v8::Object>(ret);
}

v8::Local<v8::Context> v8::Isolate::GetCallingContext(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Isolate* self = (miniv8::V8Isolate*)this;
    miniv8::V8Context* ctx = self->getCurrentCtx();
    return v8::Utils::convert<miniv8::V8Context, v8::Context>(ctx);
}

v8::Local<v8::Context> v8::Isolate::GetCurrentContext(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Isolate* self = (miniv8::V8Isolate*)this;
    miniv8::V8Context* ctx = self->getCurrentCtx();
    return v8::Utils::convert<miniv8::V8Context, v8::Context>(ctx);
}

v8::Local<v8::Context> v8::Isolate::GetEnteredContext(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Isolate* self = (miniv8::V8Isolate*)this;
    miniv8::V8Context* ctx = self->getCurrentCtx();
    return v8::Utils::convert<miniv8::V8Context, v8::Context>(ctx);
}

v8::Local<v8::DataView> v8::DataView::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::DataView>();
}

v8::Local<v8::External> v8::External::New(v8::Isolate* v8isolate, void* userdata)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Isolate* isolate = (miniv8::V8Isolate*)v8isolate;

    miniv8::V8External* ret = new miniv8::V8External(userdata);
    ret->m_userdata = userdata;
    return v8::Utils::convert<miniv8::V8External, v8::External>(ret);
}

v8::Local<v8::Float32Array> v8::Float32Array::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Float32Array>();
}

v8::Local<v8::Float64Array> v8::Float64Array::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Float64Array>();
}

v8::Local<v8::Function> v8::Function::New(v8::Isolate* isolate, v8::FunctionCallback callback, v8::Local<v8::Value> data, int length)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    miniv8::V8Context* ctx = (miniv8::V8Context*)(*context);
    void* dataPtr = static_cast<void*>(v8::External::Cast(*data));

    miniv8::V8Function* ret = new miniv8::V8Function(ctx, callback, dataPtr);
    return v8::Utils::convert<miniv8::V8Function, v8::Function>(ret);
}

v8::MaybeLocal<v8::Function> v8::Function::New(v8::Local<v8::Context> context, v8::FunctionCallback callback, v8::Local<v8::Value> data, int length, v8::ConstructorBehavior behavior)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Context* ctx = (miniv8::V8Context*)(*context);

    void* dataPtr = static_cast<void*>(v8::External::Cast(*data));
    miniv8::V8Function* ret = new miniv8::V8Function(ctx, callback, dataPtr);
    return v8::Utils::convert<miniv8::V8Function, v8::Function>(ret);
}

v8::Local<v8::Value> v8::Function::Call(v8::Local<v8::Value> recv, int argc, v8::Local<v8::Value> argv[])
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Function* self = v8::Utils::openHandle<v8::Function, miniv8::V8Function>(this);
    return Call(v8::Utils::convert<miniv8::V8Context, v8::Context>(self->v8Ctx()), recv, argc, argv).ToLocalChecked();
}

v8::MaybeLocal<v8::Value> v8::Function::Call(v8::Local<v8::Context> context, v8::Local<v8::Value> recv, int argc, v8::Local<v8::Value> argv[])
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Function* self = v8::Utils::openHandle<v8::Function, miniv8::V8Function>(this);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Value* funcObj = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*recv);

    std::vector<JSValueConst> argvWrap;
    argvWrap.resize(argc);
    for (int i = 0; i < argc; ++i) {
        miniv8::V8Value* argVal = (miniv8::V8Value*)(*(argv[i]));
        argvWrap.push_back(argVal->v(argVal));
    }

    JSValue ret = JS_Call(self->ctx(), self->v(self), funcObj->v(funcObj), argc, &argvWrap[0]);
    return v8::Utils::convert<miniv8::V8Value, v8::Value>(new miniv8::V8Value(ctx, ret)); // TODO free
}

v8::Local<v8::Value> v8::Function::GetBoundFunction(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Function::GetDisplayName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Function::GetInferredName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

void v8::Function::SetName(v8::Local<v8::String> name)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Function* self = v8::Utils::openHandle<v8::Function, miniv8::V8Function>(this);
    miniv8::V8String* nameStr = v8::Utils::openHandle<v8::String, miniv8::V8String>(*name);

    std::string nameString = nameStr->getStr();
    self->m_name = nameString;
}

v8::Local<v8::Value> v8::Function::GetName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Function* self = v8::Utils::openHandle<v8::Function, miniv8::V8Function>(this);

    std::string name = self->getName();

    JSValue ret = JS_NewStringLen(self->ctx(), name.c_str(), name.size());
    return v8::Utils::convert<miniv8::V8String, v8::String>(new miniv8::V8String(self->v8Ctx(), ret)); // TODO free
}

v8::MaybeLocal<v8::Object> v8::Function::NewInstance(v8::Local<Context> context, int argc, v8::Local<Value> argv[]) const
{
    printEmptyFuncInfo(__FUNCTION__, true);

    // 暂时没搞和Function::Call的区别
    miniv8::V8Function* self = v8::Utils::openHandle<v8::Function, miniv8::V8Function>(this);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);

    std::vector<JSValueConst> argvWrap;
    argvWrap.resize(argc);
    for (int i = 0; i < argc; ++i) {
        miniv8::V8Value* argVal = (miniv8::V8Value*)(*(argv[i]));
        argvWrap.push_back(argVal->v(argVal));
    }

    JSValue v = self->v(self); // will call JS_NewCFunction
    JSValue ret = JS_CallConstructor(self->ctx(), v, argc, 0 != argc ? &argvWrap[0] : nullptr);

    miniv8::V8Object* obj = miniv8::V8Object::create(ctx, v);
    obj->m_internalFieldCount = self->m_internalFieldCount;
    return (v8::Utils::convert<miniv8::V8Object, v8::Object>(obj)); // TODO free
}

v8::ScriptOrigin v8::Function::GetScriptOrigin(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Function* self = v8::Utils::openHandle<v8::Function, miniv8::V8Function>(this);

    std::string name = self->getName();

    JSValue nameVal = JS_NewStringLen(self->ctx(), name.c_str(), name.size());
    v8::Local<v8::String> nameStr = v8::Utils::convert<miniv8::V8String, v8::String>(new miniv8::V8String(self->v8Ctx(), nameVal));

    return ScriptOrigin(nameStr);
}

//////////////////////////////////////////////////////////////////////////
// Template

void v8::Template::SetAccessorProperty(v8::Local<v8::Name>, v8::Local<v8::FunctionTemplate>, v8::Local<v8::FunctionTemplate>, v8::PropertyAttribute, v8::AccessControl)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Template::SetNativeDataProperty(v8::Local<v8::Name>, void(__cdecl*)(v8::Local<v8::Name>, v8::PropertyCallbackInfo<v8::Value> const&), void(__cdecl*)(v8::Local<v8::Name>, v8::Local<v8::Value>, v8::PropertyCallbackInfo<void> const&), v8::Local<v8::Value>, v8::PropertyAttribute, v8::Local<v8::AccessorSignature>, v8::AccessControl)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

struct FunctionTemplateInfo {
    void* userdata;
    v8::FunctionCallback constructor;
};

namespace v8 {
namespace internal {
class FunctionCallbackArguments { // 为了能访问v8::FunctionCallbackInfo内部变量，借了个v8内部类的壳子
public:
    static JSValue onTemplateCallback(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv, void* userdata);
};

JSValue FunctionCallbackArguments::onTemplateCallback(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv, void* userdata)
{
    miniv8::V8Context* context = (miniv8::V8Context*)JS_GetContextOpaque(ctx);
    FunctionTemplateInfo* info = (FunctionTemplateInfo*)userdata;
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    if (JS_IsUndefined(thisVal))
        thisVal = JS_GetGlobalObject(ctx); // 直接调用alert，thisVal可能为undefined

    std::vector<void*> argValue;
    argValue.resize(argc + 2);

    for (int i = 0; i < argc; ++i) {
        miniv8::V8Value* v = miniv8::V8Value::create(context, argv[argc - i - 1]); // TODO:是不是也要用V8Object::create？
        argValue[i] = v;
    }

    argValue[argc] = miniv8::V8Object::create(context, thisVal);
    argValue[argc + 1] = argValue[argc]; // 如果argc是0的话，v8会在FunctionCallbackInfo<T>::This()里取到这个地址

    void* returnValue[4] = { 0 };
    returnValue[1] = isolate;

    miniv8::V8External external(info->userdata);

    void* implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kArgsLength] = { 0 };
    implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kHolderIndex] = argValue[argc]; // kHolderIndex
    implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kIsolateIndex] = isolate; // kIsolateIndex
    implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kReturnValueDefaultValueIndex] = nullptr; // kReturnValueDefaultValueIndex
    implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kReturnValueIndex] = &returnValue[2]; //kReturnValueIndex
    implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kDataIndex] = &external; // kDataIndex
    implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kCalleeIndex] = nullptr; // kCalleeIndex
    implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kContextSaveIndex] = nullptr; // kContextSaveIndex
    implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kNewTargetIndex] = nullptr; // kNewTargetIndex

    miniv8::FunctionCallbackInfoWrap functionCallbackInfoWrap;
    functionCallbackInfoWrap.implicitArgs = implicitArgs;
    functionCallbackInfoWrap.values = &argValue[argc - 1];
    functionCallbackInfoWrap.length = argc;

    v8::FunctionCallbackInfo<v8::Value>* functionCallbackInfoWrapV8 = (v8::FunctionCallbackInfo<v8::Value>*)&functionCallbackInfoWrap;
    info->constructor(*functionCallbackInfoWrapV8);

    void*** retVal = (void***)&implicitArgs[v8::FunctionCallbackInfo<v8::Value>::kReturnValueIndex];
    if (nullptr == **retVal)
        return JS_UNDEFINED;

    //miniv8::V8Value* ret = v8::Utils::openHandle<v8::Value, miniv8::V8Value>((v8::Value*)(*retVal));
    miniv8::V8Value* ret = (miniv8::V8Value*)*retVal; // 这里直接按照v8，应该就是这样取地址

    return ret->v(ret); // TODO:Free
}
} // internal
} // v8

// from G:\mycode\mb_temp\third_party\WebKit\Source\bindings\core\v8\V8DOMConfiguration.cpp doInstallMethodInternal
void v8::Template::Set(v8::Local<v8::Name> name, v8::Local<v8::Data> value, v8::PropertyAttribute)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    // value可以是v8::FunctionTemplate、v8::Integer等
    miniv8::V8Template* self = v8::Utils::openHandle<v8::Template, miniv8::V8Template>(this);
    
    if (value.IsEmpty())
        return; // TODO: installConstantInternal->v8::Integer::New 没实现
    miniv8::V8Data* val = v8::Utils::openHandle<v8::Data, miniv8::V8Data>(*value); // TODO: free

    miniv8::V8String* nameStr = (miniv8::V8String*)v8::Utils::openHandle<v8::Name, miniv8::V8Name>(*name);
    miniv8ReleaseAssert(
        miniv8::kObjectTypeString == nameStr->m_type || 
        miniv8::kObjectTypeSymbol == nameStr->m_type, // frome setClassString
        "v8::Template::Set fail\n");

    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    if (miniv8::kObjectTypeFunctionTemplate == val->m_type) {
        miniv8::V8FunctionTemplate* funcProp = (miniv8::V8FunctionTemplate*)val;

        miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);

        FunctionTemplateInfo* info = new FunctionTemplateInfo(); // TODO: Free
        info->constructor = funcProp->m_constructor;
        info->userdata = funcProp->m_userdata;

        JSValue v = JS_NewCFunction2(ctx->ctx(), &v8::internal::FunctionCallbackArguments::onTemplateCallback, nameStr->getStr().c_str(), funcProp->m_argLength, JS_CFUNC_generic, 0, info);
        self->m_props.insert(std::make_pair(nameStr->getStr(), v));
    } else if (miniv8::kObjectTypeString == val->m_type) {
        miniv8::V8String* valStr = (miniv8::V8String*)val;
        miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
        JSValue v = valStr->v(valStr);
        self->m_props.insert(std::make_pair(nameStr->getStr(), v));
    } else {
        DebugBreak();
    }
}

v8::Local<v8::Function> v8::FunctionTemplate::GetFunction(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Function>();
}

// static
v8::Local<v8::FunctionTemplate> v8::FunctionTemplate::New(
    v8::Isolate* isolate, 
    v8::FunctionCallback callback, 
    v8::Local<v8::Value> data, 
    v8::Local<v8::Signature> signature, 
    int length,
    v8::ConstructorBehavior behavior
    )
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8FunctionTemplate* self = new miniv8::V8FunctionTemplate();

    if (!data.IsEmpty()) {
        miniv8::V8External* dataVal = (miniv8::V8External*)v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*data); // TODO: free
        miniv8ReleaseAssert(dataVal->m_type == miniv8::kObjectTypeExternal, "v8::FunctionTemplate::New fail\n");

        self->m_userdata = dataVal->getUserdata();
    }
    self->m_constructor = callback;
    self->m_argLength = length;

    v8::Local<v8::FunctionTemplate> ret = v8::Utils::convert<miniv8::V8FunctionTemplate, v8::FunctionTemplate>(self);
    return ret;
}

bool v8::FunctionTemplate::HasInstance(v8::Local<v8::Value> object)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

v8::Local<v8::ObjectTemplate> v8::FunctionTemplate::InstanceTemplate(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8FunctionTemplate* self = v8::Utils::openHandle<v8::FunctionTemplate, miniv8::V8FunctionTemplate>(this);

    if (!self->m_instanceTemplate) { // TODO: 要记得检查m_parentTemplate是否设置过
        self->m_instanceTemplate = (new miniv8::V8ObjectTemplate());
        self->m_instanceTemplate->m_parentTemplate = self;
        self->m_instanceTemplate->m_internalFieldCount = self->m_internalFieldCount;
    }
    v8::Local<v8::ObjectTemplate> ret = v8::Utils::convert<miniv8::V8ObjectTemplate, v8::ObjectTemplate>(self->m_instanceTemplate);
    return ret;
}

v8::Local<v8::ObjectTemplate> v8::FunctionTemplate::PrototypeTemplate(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8FunctionTemplate* self = v8::Utils::openHandle<v8::FunctionTemplate, miniv8::V8FunctionTemplate>(this);

    if (!self->m_prototypeTemplate) { // TODO: 要记得检查m_parentTemplate是否设置过
        self->m_prototypeTemplate = (new miniv8::V8ObjectTemplate());
        self->m_prototypeTemplate->m_parentTemplate = self;
        self->m_prototypeTemplate->m_internalFieldCount = self->m_internalFieldCount;
    }
    v8::Local<v8::ObjectTemplate> ret = v8::Utils::convert<miniv8::V8ObjectTemplate, v8::ObjectTemplate>(self->m_prototypeTemplate);
    return ret;
}

v8::MaybeLocal<v8::Function> v8::FunctionTemplate::GetFunction(v8::Local<v8::Context> context)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    //v8::HandleScope handleScope(isolate); // TODO free

    miniv8::V8FunctionTemplate* self = v8::Utils::openHandle<v8::FunctionTemplate, miniv8::V8FunctionTemplate>(this);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);

    miniv8ReleaseAssert(!self->m_isInit, "v8::FunctionTemplate::GetFunction fail\n");
    self->m_isInit = true;

    miniv8::V8Function* fun = new miniv8::V8Function(ctx, &miniv8::V8FunctionTemplate::functionTemplateCall, self);
    fun->m_internalFieldCount = self->m_internalFieldCount;
    fun->setName(self->getClassName());

    self->newTemplateInstance(ctx, fun);

    if (self->m_instanceTemplate)
        self->m_instanceTemplate->newTemplateInstance(ctx, fun); // TODO: 没考虑m_instanceTemplate的m_instanceTemplate递归情况了
    if (self->m_prototypeTemplate)
        self->m_prototypeTemplate->newTemplateInstance(ctx, fun);

    v8::MaybeLocal<v8::Object> prototype;
    if (self->m_prototypeTemplate) {
        v8::ObjectTemplate* prototypeTemplate = (v8::ObjectTemplate*)(&self->m_prototypeTemplate);
        prototype = prototypeTemplate->NewInstance(context);
    }
    if (!prototype.IsEmpty()) {
        v8::Local<v8::Object> prototypeObj = prototype.ToLocalChecked();
        miniv8::V8Object* prototypeObject = v8::Utils::openHandle<v8::Object, miniv8::V8Object>(*prototypeObj);
        JS_SetPrototype(ctx->ctx(), fun->v(fun), prototypeObject->v(prototypeObject)); // TODO free: 没考虑释放问题，另外不清楚那些访问器能否起作用
    }

    // TODO：没考虑继承

    v8::Local<v8::Function> ret = v8::Utils::convert<miniv8::V8Function, v8::Function>(fun);
    return v8::MaybeLocal<v8::Function>(ret);
}

void v8::FunctionTemplate::Inherit(v8::Local<v8::FunctionTemplate>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    // 要考虑父模版的InstanceTemplate和PrototypeTemplate是否都要继承，以及这两模版的访问器是否也要继承
}

void v8::FunctionTemplate::ReadOnlyPrototype(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::FunctionTemplate::RemovePrototype(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::FunctionTemplate::SetAcceptAnyReceiver(bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::FunctionTemplate::SetCallHandler(v8::FunctionCallback constructor, v8::Local<v8::Value> data, v8::experimental::FastAccessorBuilder* fastHandler)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8External* dataExternal = nullptr;
    if (!data.IsEmpty())
        dataExternal = (miniv8::V8External*)(v8::Utils::openHandle<v8::Value, miniv8::V8Value>(*data));

    miniv8ReleaseAssert(dataExternal->m_type == miniv8::kObjectTypeExternal, "FunctionTemplate::SetCallHandler data fail\n");
    //miniv8ReleaseAssert(data.IsEmpty(), "FunctionTemplate::SetCallHandler data fail\n");
    miniv8ReleaseAssert(!fastHandler, "FunctionTemplate::SetCallHandler fastHandler fail\n");

    miniv8::V8FunctionTemplate* self = v8::Utils::openHandle<v8::FunctionTemplate, miniv8::V8FunctionTemplate>(this);
    self->m_constructor = constructor;
    self->m_userdata = dataExternal ? dataExternal->getUserdata() : nullptr;
}

void v8::FunctionTemplate::SetClassName(v8::Local<v8::String> name)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8FunctionTemplate* self = v8::Utils::openHandle<v8::FunctionTemplate, miniv8::V8FunctionTemplate>(this);
    std::string nameStr = miniv8::getStringFromV8String(name);
    self->setClassName(nameStr);
}

void v8::FunctionTemplate::SetHiddenPrototype(bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::FunctionTemplate::SetLength(int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

//////////////////////////////////////////////////////////////////////////

// static
v8::Local<v8::ObjectTemplate> v8::ObjectTemplate::New(v8::Isolate*, v8::Local<v8::FunctionTemplate> constructor)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8ObjectTemplate* objTemp = new miniv8::V8ObjectTemplate();
    v8::Local<v8::ObjectTemplate> ret = v8::Utils::convert<miniv8::V8ObjectTemplate, v8::ObjectTemplate>(objTemp);
    return ret;
}

void v8::ObjectTemplate::MarkAsUndetectable(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::ObjectTemplate::SetAccessCheckCallback(bool(__cdecl*)(v8::Local<v8::Context>, v8::Local<v8::Object>, v8::Local<v8::Value>), v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::ObjectTemplate::SetAccessor(
    v8::Local<v8::Name> name,
    v8::AccessorNameGetterCallback getter,
    v8::AccessorNameSetterCallback setter,
    v8::Local<v8::Value> data ,
    v8::AccessControl settings,
    v8::PropertyAttribute attribute,
    v8::Local<v8::AccessorSignature> signature
    )
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8String* nameStr = v8::Utils::openHandle<v8::Name, miniv8::V8String>(*name);

    size_t len = 0;
    std::string nameString = nameStr->getStr();
    if (nameString.empty())
        return;
    if (nameString == "length" || "name" == nameString || "toString" == nameString) // window属性的length暂不支持
        return;

    miniv8::V8External* external = v8::Utils::openHandle<v8::Value, miniv8::V8External>(*data);
    miniv8::V8ObjectTemplate* self = v8::Utils::openHandle<v8::ObjectTemplate, miniv8::V8ObjectTemplate>(this);
    miniv8::V8ObjectTemplate::Accessor accessor;
    accessor.nameGetter = getter;
    accessor.nameSetter = setter;
    accessor.data = external->getUserdata();
    accessor.name = nameString;

    self->m_accessors.insert(std::make_pair(nameString, accessor));
}

void v8::ObjectTemplate::SetAccessor(
    v8::Local<v8::String> name,
    v8::AccessorGetterCallback getter,
    v8::AccessorSetterCallback setter, 
    v8::Local<v8::Value> data,
    v8::AccessControl settings,
    v8::PropertyAttribute attribute,
    v8::Local<v8::AccessorSignature> signature
    )
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8String* nameStr = v8::Utils::openHandle<v8::String, miniv8::V8String>(*name);
    size_t len = 0;
    std::string nameString = nameStr->getStr();
    if (nameString.empty())
        return;

    if (nameString == "length" || "name" == nameString || "toString" == nameString) // window属性的length暂不支持
        return;

    miniv8::V8External* external = v8::Utils::openHandle<v8::Value, miniv8::V8External>(*data);
    miniv8::V8ObjectTemplate* self = v8::Utils::openHandle<v8::ObjectTemplate, miniv8::V8ObjectTemplate>(this);

    miniv8::V8ObjectTemplate::Accessor accessor;
    accessor.getter = getter;
    accessor.setter = setter;
    accessor.data = external->getUserdata();

    self->m_accessors.insert(std::make_pair(nameString, accessor));
}

void v8::ObjectTemplate::SetCallAsFunctionHandler(v8::FunctionCallback cb, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::ObjectTemplate::SetHandler(struct v8::IndexedPropertyHandlerConfiguration const&)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::ObjectTemplate::SetHandler(struct v8::NamedPropertyHandlerConfiguration const&)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::ObjectTemplate::SetInternalFieldCount(int count)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8ObjectTemplate* self = v8::Utils::openHandle<v8::ObjectTemplate, miniv8::V8ObjectTemplate>(this);
    self->m_internalFieldCount = count;

    if (self->m_parentTemplate) {
        int parentCount = self->m_parentTemplate->m_internalFieldCount;
        //miniv8ReleaseAssert(parentCount == 0 || parentCount == count, "v8::ObjectTemplate::SetInternalFieldCount fail\n");
        if (parentCount == 0)
            self->m_parentTemplate->m_internalFieldCount = count;
    }

    char* output = (char*)malloc(0x100);
    sprintf(output, "v8::ObjectTemplate::SetInternalFieldCount: %p, %d\n", this, count);
    OutputDebugStringA(output);
    free(output);
}

int v8::ObjectTemplate::InternalFieldCount()
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8ObjectTemplate* self = v8::Utils::openHandle<v8::ObjectTemplate, miniv8::V8ObjectTemplate>(this);

    char* output = (char*)malloc(0x100);
    sprintf(output, "v8::ObjectTemplate::InternalFieldCount: %p\n", self);
    OutputDebugStringA(output);
    free(output);

    return self->m_internalFieldCount;
}

void v8::ObjectTemplate::SetNamedPropertyHandler(void(__cdecl*)(v8::Local<v8::Name>, v8::PropertyCallbackInfo<v8::Value> const&), void(__cdecl*)(v8::Local<v8::Name>, v8::Local<v8::Value>, v8::PropertyCallbackInfo<v8::Value> const&), void(__cdecl*)(v8::Local<v8::Name>, v8::PropertyCallbackInfo<v8::Integer> const&), void(__cdecl*)(v8::Local<v8::Name>, v8::PropertyCallbackInfo<v8::Boolean> const&), void(__cdecl*)(v8::PropertyCallbackInfo<v8::Array> const&), v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::MaybeLocal<v8::Object> v8::ObjectTemplate::NewInstance(v8::Local<v8::Context> context)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8ObjectTemplate* self = v8::Utils::openHandle<v8::ObjectTemplate, miniv8::V8ObjectTemplate>(this);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);

    JSValue v = JS_NewObject(ctx->ctx());
    miniv8::V8Object* obj = new miniv8::V8Object(ctx, v);
    self->newTemplateInstance(ctx, obj);

    v8::Local<v8::Object> ret = v8::Utils::convert<miniv8::V8Object, v8::Object>(obj);
    return v8::MaybeLocal<v8::Object>(ret);
}

//////////////////////////////////////////////////////////////////////////

v8::Local<v8::Int16Array> v8::Int16Array::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)                                                      
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Int16Array* obj = nullptr;// new miniv8::Int16Array();
    v8::Local<v8::Int16Array> ret = v8::Utils::convert<miniv8::V8Int16Array, v8::Int16Array>(obj);

    return ret;
}
                                                                         
v8::Local<v8::Int32> v8::Value::ToInt32(v8::Isolate*) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Int32* obj = nullptr;// new miniv8::V8Int32();
    v8::Local<v8::Int32> ret = v8::Utils::convert<miniv8::V8Int32, v8::Int32>(obj);
    return ret;
}

v8::Local<v8::Int32Array> v8::Int32Array::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Int32Array* obj = nullptr;// new miniv8::Int16Array();
    v8::Local<v8::Int32Array> ret = v8::Utils::convert<miniv8::V8Int32Array, v8::Int32Array>(obj);

    return ret;
}

v8::Local<v8::Int8Array> v8::Int8Array::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Int8Array* obj = nullptr;// new miniv8::Int16Array();
    v8::Local<v8::Int8Array> ret = v8::Utils::convert<miniv8::V8Int8Array, v8::Int8Array>(obj);

    return ret;
}

v8::Local<v8::Integer> v8::Integer::New(v8::Isolate*, int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Integer* obj = nullptr;// new miniv8::Int16Array();
    v8::Local<v8::Integer> ret = v8::Utils::convert<miniv8::V8Integer, v8::Integer>(obj);
    return ret;
}

v8::Local<v8::Integer> v8::Integer::NewFromUnsigned(v8::Isolate*, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Integer* obj = nullptr;// new miniv8::Int16Array();
    v8::Local<v8::Integer> ret = v8::Utils::convert<miniv8::V8Integer, v8::Integer>(obj);
    return ret;
}

v8::Local<v8::Integer> v8::Value::ToInteger(v8::Isolate*) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Integer>();
}

// v8::Local<v8::Integer> v8::Value::ToInteger(void) const
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
//     return v8::Local<v8::Integer>();
// }

v8::Local<v8::Uint16Array> v8::Uint16Array::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Uint16Array>();
}

v8::Local<v8::Uint32Array> v8::Uint32Array::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Uint32Array>();
}

v8::Local<v8::Uint8Array> v8::Uint8Array::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Uint8Array>();
}

v8::Local<v8::Uint8ClampedArray> v8::Uint8ClampedArray::New(v8::Local<v8::ArrayBuffer>, unsigned int, unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Uint8ClampedArray>();
}

v8::Local<v8::Map> v8::Map::New(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Map>();
}

v8::Local<v8::Message> v8::Exception::CreateMessage(v8::Isolate*, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Message>();
}

v8::String::Value::Value(v8::Isolate*, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

// void v8::Isolate::AddGCPrologueCallback(v8::GCCallback callback, v8::GCType gc_type_filter)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

void v8::Isolate::IsolateInBackgroundNotification()
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::Local<v8::Message> v8::TryCatch::Message(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Message>();
}

v8::Local<v8::NativeWeakMap> v8::NativeWeakMap::New(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::NativeWeakMap>();
}

v8::Local<v8::Number> v8::Number::New(v8::Isolate*, double)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Number>();
}

v8::Local<v8::Number> v8::Value::ToNumber(v8::Isolate*) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Number>();
}

// v8::Local<v8::Number> v8::Value::ToNumber(void) const
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
//     return v8::Local<v8::Number>();
// }

v8::Local<v8::Object> v8::Proxy::GetTarget(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Object>();
}

v8::Local<v8::Object> v8::Value::ToObject(v8::Isolate*) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Object>();
}

v8::Local<v8::Private> v8::Private::ForApi(v8::Isolate* isolate, v8::Local<v8::String> name)
{
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8String* nameStr = v8::Utils::openHandle<v8::String, miniv8::V8String>(*name);
    JSValue val = nameStr->v(nameStr);

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);
    miniv8::V8Private* ret = new miniv8::V8Private(ctx, val);

    return v8::Utils::convert<miniv8::V8Private, v8::Private>(ret);
}

v8::Local<v8::Promise> v8::Promise::Resolver::GetPromise(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Promise>();
}

v8::Local<v8::Script> v8::UnboundScript::BindToCurrentContext(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Script>();
}

v8::Local<v8::Set> v8::Set::New(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Set>();
}

v8::Local<v8::Signature> v8::Signature::New(v8::Isolate*, v8::Local<v8::FunctionTemplate>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Signature>();
}

v8::Local<v8::StackFrame> v8::StackTrace::GetFrame(unsigned int) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::StackFrame>();
}

v8::Local<v8::StackTrace> v8::Message::GetStackTrace(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::StackTrace>();
}

v8::Local<v8::StackTrace> v8::StackTrace::CurrentStackTrace(v8::Isolate*, int, v8::StackTrace::StackTraceOptions)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8StackTrace* ret = new miniv8::V8StackTrace();
    return v8::Utils::convert<miniv8::V8StackTrace, v8::StackTrace>(ret);
}

v8::Local<v8::String> v8::CpuProfile::GetTitle(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::CpuProfileNode::GetFunctionName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::CpuProfileNode::GetScriptResourceName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::Message::Get(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::Message::GetSourceLine(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::RegExp::GetSource(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::StackFrame::GetFunctionName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::StackFrame::GetScriptName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::StackFrame::GetScriptNameOrSourceURL(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::StringObject::ValueOf(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::String> v8::Value::ToString(v8::Isolate*) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::String>();
}

v8::Local<v8::Symbol> v8::Symbol::GetIterator(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Symbol>();
}

v8::Local<v8::Symbol> v8::Symbol::GetToStringTag(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Symbol* self = new miniv8::V8Symbol("ToString");
    v8::Local<v8::Symbol> ret = v8::Utils::convert<miniv8::V8Symbol, v8::Symbol>(self);
    return ret;
}

v8::Local<v8::Symbol> v8::Symbol::GetUnscopables(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Symbol>();
}

v8::Local<v8::Symbol> v8::Symbol::New(v8::Isolate*, v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Symbol>();
}

v8::Local<v8::Symbol> v8::SymbolObject::ValueOf(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Symbol>();
}

v8::Local<v8::UnboundScript> v8::Script::GetUnboundScript(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::UnboundScript>();
}

v8::Local<v8::Value> v8::BooleanObject::New(v8::Isolate*, bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Context::GetSecurityToken(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Exception::Error(v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Exception::RangeError(v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Exception::ReferenceError(v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Exception::SyntaxError(v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Exception::TypeError(v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::HeapProfiler::FindObjectById(unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Isolate::ThrowException(v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Message::GetScriptResourceName(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::NativeWeakMap::Get(v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::NumberObject::New(v8::Isolate*, double)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}


v8::Local<v8::Value> v8::Proxy::GetHandler(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Script::Run(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

// v8::Local<v8::Value> v8::StringObject::New(v8::Isolate*, v8::Local<v8::String>)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
//     return v8::Local<v8::Value>();
// }

v8::Local<v8::Value> v8::StringObject::New(v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::Symbol::Name(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::TryCatch::Exception(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

v8::Local<v8::Value> v8::TryCatch::ReThrow(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Value>();
}

void v8::Locker::Initialize(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

// v8::Locker::Locker(v8::Isolate*)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

v8::Locker::~Locker(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::Maybe<__int64> v8::Value::IntegerValue(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    __int64 val = 0;
    v8::Maybe<__int64> ret = v8::Just(val);
    return ret;
}

v8::Maybe<bool> v8::Promise::Resolver::Reject(v8::Local<v8::Context>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<bool> ret = v8::Just(false);
    return ret;
}

v8::Maybe<bool> v8::Promise::Resolver::Resolve(v8::Local<v8::Context>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<bool> ret = v8::Just(false);
    return ret;
}

v8::Maybe<bool> v8::Value::BooleanValue(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<bool> ret = v8::Just(false);
    return ret;
}

v8::Maybe<double> v8::Value::NumberValue(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<double> ret = v8::Just(0.0);
    return ret;
}

v8::Maybe<int> v8::Message::GetEndColumn(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<int> ret = v8::Just(0);
    return ret;
}

v8::Maybe<int> v8::Message::GetLineNumber(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<int> ret = v8::Just(0);
    return ret;
}

v8::Maybe<int> v8::Message::GetStartColumn(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<int> ret = v8::Just(0);
    return ret;
}

v8::Maybe<int> v8::Value::Int32Value(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<int> ret = v8::Just(0);
    return ret;
}

v8::Maybe<unsigned int> v8::Value::Uint32Value(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::Maybe<unsigned int> ret = v8::Just<unsigned int>(0);
    return ret;
}

v8::MaybeLocal<v8::Boolean> v8::Value::ToBoolean(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<Boolean> ret;
    return ret;
}

v8::MaybeLocal<v8::Integer> v8::Value::ToInteger(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Integer> ret;
    return ret;
}

v8::MaybeLocal<v8::Map> v8::Map::Set(v8::Local<v8::Context>, v8::Local<v8::Value>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Map> ret;
    return ret;
}

v8::MaybeLocal<v8::Number> v8::Value::ToNumber(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Number> ret;
    return ret;
}

v8::MaybeLocal<v8::Object> v8::Value::ToObject(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Object> ret;
    return ret;
}

v8::MaybeLocal<v8::Promise::Resolver> v8::Promise::Resolver::New(v8::Local<v8::Context>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Promise::Resolver> ret;
    return ret;
}

v8::MaybeLocal<v8::Promise> v8::Promise::Catch(v8::Local<v8::Context>, v8::Local<v8::Function>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Promise> ret;
    return ret;
}

v8::MaybeLocal<v8::Promise> v8::Promise::Then(v8::Local<v8::Context>, v8::Local<v8::Function>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Promise> ret;
    return ret;
}

v8::MaybeLocal<v8::RegExp> v8::RegExp::New(v8::Local<v8::Context>, v8::Local<v8::String>, v8::RegExp::Flags)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::RegExp> ret;
    return ret;
}

v8::MaybeLocal<v8::Script> v8::Script::Compile(v8::Local<v8::Context>, v8::Local<v8::String>, v8::ScriptOrigin*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Script> ret;
    return ret;
}

v8::MaybeLocal<v8::Script> v8::ScriptCompiler::Compile(v8::Local<v8::Context> context, v8::ScriptCompiler::Source* source, v8::ScriptCompiler::CompileOptions)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8String* sourceStr = v8::Utils::openHandle<v8::String, miniv8::V8String>(*(source->source_string));
    std::string src = sourceStr->getStr();
    std::string name;

    if (!source->resource_name.IsEmpty() && source->resource_name->IsString()) {
        v8::MaybeLocal<v8::String> sourceName = source->resource_name->ToString(context);
        miniv8::V8String* sourceNameStr = v8::Utils::openHandle<v8::String, miniv8::V8String>(*(sourceName.ToLocalChecked()));
        name = sourceNameStr->getStr();
    }

    miniv8::V8Script* script = new miniv8::V8Script();
    script->setSource(src);
    script->setSourceName(name);
    v8::Local<v8::Script> ret = v8::Utils::convert<miniv8::V8Script, v8::Script>(script);
    return ret;
}

v8::MaybeLocal<v8::Script> v8::ScriptCompiler::Compile(v8::Local<v8::Context>, v8::ScriptCompiler::StreamedSource*, v8::Local<v8::String>, v8::ScriptOrigin const&)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Script> ret;
    return ret;
}

static JSValue jsPrint(JSContext* ctx, JSValueConst this_val, int argc, JSValueConst* argv, void* userdata)
{
    int i;
    const char* str;

    for (i = 0; i < argc; i++) {
        if (i != 0)
            printDebug(" ");
        str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;
        OutputDebugStringA(str);
        JS_FreeCString(ctx, str);
    }
    OutputDebugStringA("\n");

    return JS_UNDEFINED;
}

static void printWhenError(JSContext* ctx)
{
    JSValue exception_val = JS_GetException(ctx);
    BOOL is_error = JS_IsError(ctx, exception_val);

    jsPrint(ctx, JS_NULL, 1, &exception_val, nullptr);

    if (is_error) {
        JSValue val = JS_GetPropertyStr(ctx, exception_val, "stack");
        if (!JS_IsUndefined(val)) {
            const char* stack = JS_ToCString(ctx, val);
            printDebug("%s\n", stack);
            JS_FreeCString(ctx, stack);
        }
        JS_FreeValue(ctx, val);
    }
}

v8::MaybeLocal<v8::Value> v8::Script::Run(v8::Local<v8::Context> context)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Script* self = v8::Utils::openHandle<v8::Script, miniv8::V8Script>(this);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);

    miniv8::V8Isolate* isolate = (miniv8::V8Isolate*)v8::Isolate::GetCurrent();
    
    // TODO: 返回值，可能是已经存在的dom之类的
    isolate->enterContext(ctx);
    JSValue ret = JS_Eval(ctx->ctx(), self->getSource().c_str(), self->getSource().size(), self->getSourceName().c_str(), JS_EVAL_TYPE_GLOBAL);
    isolate->exitContext();

    if (JS_IsException(ret))
        printWhenError(ctx->ctx());

    miniv8::V8Value* retVal = new miniv8::V8Value(ctx, ret);
    return v8::Utils::convert<miniv8::V8Value, v8::Value>(retVal);
}

v8::MaybeLocal<v8::Set> v8::Set::Add(v8::Local<v8::Context>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Set> ret;
    return ret;
}

v8::MaybeLocal<v8::String> v8::Message::GetSourceLine(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::String> ret;
    return ret;
}

v8::MaybeLocal<v8::String> v8::Value::ToString(v8::Local<v8::Context> context) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::String> ret;

    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    miniv8::V8Context* ctx = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(*context);

    JSValue v = self->v(self);
    if (miniv8::kObjectTypeString == self->m_type) {
        miniv8::V8String* str = (miniv8::V8String*)self;
        return v8::Utils::convert<miniv8::V8String, v8::String>(str);
    } else if (JS_IsString(v)) {
        miniv8::V8String* str = new miniv8::V8String(ctx, v);
        return v8::Utils::convert<miniv8::V8String, v8::String>(str);
    }

    DebugBreak();
    return ret;
}

v8::MaybeLocal<v8::Uint32> v8::Value::ToArrayIndex(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Uint32> ret;
    return ret;
}

v8::MaybeLocal<v8::Uint32> v8::Value::ToUint32(v8::Local<v8::Context>) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Uint32> ret;
    return ret;
}

v8::MaybeLocal<v8::UnboundScript> v8::ScriptCompiler::CompileUnboundScript(v8::Isolate*, v8::ScriptCompiler::Source*, v8::ScriptCompiler::CompileOptions)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::UnboundScript> ret;
    return ret;
}

v8::MaybeLocal<v8::Value> v8::Date::New(v8::Local<v8::Context>, double)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Value> ret;
    return ret;
}

v8::MaybeLocal<v8::Value> v8::JSON::Parse(v8::Isolate*, v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Value> ret;
    return ret;
}

v8::MaybeLocal<v8::Value> v8::Map::Get(v8::Local<v8::Context>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    v8::MaybeLocal<v8::Value> ret;
    return ret;
}

// v8::Platform* v8::platform::CreateDefaultPlatform(int)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

v8::ResourceConstraints::ResourceConstraints(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::ScriptCompiler::CachedData::CachedData(unsigned char const*, int, v8::ScriptCompiler::CachedData::BufferPolicy)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::ScriptCompiler::CachedData::~CachedData(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::ScriptCompiler::ScriptStreamingTask* v8::ScriptCompiler::StartStreamingScript(v8::Isolate*, v8::ScriptCompiler::StreamedSource*, v8::ScriptCompiler::CompileOptions)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::ScriptCompiler::StreamedSource::StreamedSource(v8::ScriptCompiler::ExternalSourceStream*, v8::ScriptCompiler::StreamedSource::Encoding)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::ScriptCompiler::StreamedSource::~StreamedSource(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::ScriptOrigin v8::Message::GetScriptOrigin(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);

    v8::Local<v8::String> resourceName = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), "v8::Message::GetScriptOrigin");
    return v8::ScriptOrigin(resourceName);
}

v8::TryCatch::TryCatch(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::TryCatch::TryCatch(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

v8::TryCatch::~TryCatch(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

bool v8::ScriptCompiler::ExternalSourceStream::SetBookmark(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

void v8::ScriptCompiler::ExternalSourceStream::ResetToBookmark(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void* v8::External::Value(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8External* self = v8::Utils::openHandle<v8::External, miniv8::V8External>(this);
    return self->getUserdata();
}

// void* v8::ScriptCompiler::CachedData::`scalar deleting destructor'(unsigned int)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }
// 
// void* v8::ScriptCompiler::StreamedSource::`scalar deleting destructor'(unsigned int)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }
// 
// void addAccessor(v8::Local<v8::Context>, char const*, __int64(__cdecl*)(struct JsExecStateInfo*, void*), void*, __int64(__cdecl*)(struct JsExecStateInfo*, void*), void*)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

void v8::ArrayBuffer::Neuter(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Context::AllowCodeGenerationFromStrings(bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Context::DetachGlobal(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Context::Enter(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Context* self = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(this);

    miniv8::V8Isolate* isolate = miniv8::V8Isolate::GetCurrent();
    isolate->enterContext(self);
}

void v8::Context::Exit(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Isolate* isolate = miniv8::V8Isolate::GetCurrent();
    isolate->exitContext();
}

void v8::Context::SetErrorMessageForCodeGenerationFromStrings(v8::Local<v8::String>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Context::SetSecurityToken(v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Context::UseDefaultSecurityToken(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

// void v8::Eternal<v8::FunctionTemplate>::Set<v8::FunctionTemplate>(v8::Isolate*, v8::Local<v8::FunctionTemplate>)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

void v8::HeapProfiler::ClearObjectIds(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::HeapProfiler::SetRetainedObjectInfo(v8::UniqueId, v8::RetainedObjectInfo*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::HeapProfiler::SetWrapperClassInfoProvider(unsigned short, v8::RetainedObjectInfo * (__cdecl*)(unsigned short, v8::Local<v8::Value>))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::HeapProfiler::StartTrackingHeapObjects(bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::HeapProfiler::StopTrackingHeapObjects(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::HeapSnapshot::Delete(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::HeapSnapshot::Serialize(v8::OutputStream*, v8::HeapSnapshot::SerializationFormat) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::AddCallCompletedCallback(void(__cdecl*)(void))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::CancelTerminateExecution(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::Dispose(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::EnqueueMicrotask(void(__cdecl*)(void*), void*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::Enter(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::Exit(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::GetCodeRange(void**, unsigned int*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::GetHeapStatistics(v8::HeapStatistics*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::LowMemoryNotification(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::RemoveCallCompletedCallback(void(__cdecl*)(void))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::RequestInterrupt(void(__cdecl*)(v8::Isolate*, void*), void*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::RunMicrotasks(void)
{
    //printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetAllowCodeGenerationFromStringsCallback(bool(__cdecl*)(v8::Local<v8::Context>, v8::Local<v8::String>))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetAutorunMicrotasks(bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetCaptureStackTraceForUncaughtExceptions(bool, int, v8::StackTrace::StackTraceOptions)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetEventLogger(void(__cdecl*)(char const*, int))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetFailedAccessCheckCallbackFunction(void(__cdecl*)(v8::Local<v8::Object>, v8::AccessType, v8::Local<v8::Value>))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetFatalErrorHandler(void(__cdecl*)(char const*, char const*))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

// void v8::Isolate::SetObjectGroupId<v8::Object>(v8::Persistent<v8::Object, v8::NonCopyablePersistentTraits<v8::Object> > const&, v8::UniqueId)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

void v8::Isolate::SetPromiseRejectCallback(void(__cdecl*)(v8::PromiseRejectMessage))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetStackLimit(unsigned int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::SetUseCounterCallback(void(__cdecl*)(v8::Isolate*, v8::Isolate::UseCounterFeature))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::TerminateExecution(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::VisitHandlesWithClassIds(v8::PersistentHandleVisitor*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Isolate::VisitWeakHandles(v8::PersistentHandleVisitor*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::NativeWeakMap::Set(v8::Local<v8::Value>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::RegisterExtension(v8::Extension*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::ResourceConstraints::ConfigureDefaults(unsigned __int64, unsigned __int64)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::TryCatch::Reset(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::TryCatch::SetVerbose(bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::V8::AddGCEpilogueCallback(void(__cdecl*)(v8::GCType, v8::GCCallbackFlags), v8::GCType)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::V8::AddGCPrologueCallback(void(__cdecl*)(v8::GCType, v8::GCCallbackFlags), v8::GCType)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::V8::InitializePlatform(v8::Platform*)
{
    printEmptyFuncInfo(__FUNCTION__, false);
}

// void v8::V8::SetAllowCodeGenerationFromStringsCallback(bool(__cdecl*)(v8::Local<v8::Context>, v8::Local<v8::String>))
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

// void v8::V8::SetCaptureStackTraceForUncaughtExceptions(bool, int, v8::StackTrace::StackTraceOptions)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

void v8::V8::SetEntropySource(bool(__cdecl*)(unsigned char*, unsigned int))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

// void v8::V8::SetFailedAccessCheckCallbackFunction(void(__cdecl*)(v8::Local<v8::Object>, v8::AccessType, v8::Local<v8::Value>))
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

// void v8::V8::SetFatalErrorHandler(void(__cdecl*)(char const*, char const*))
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

void v8::V8::SetFlagsFromCommandLine(int*, char**, bool)
{
    printEmptyFuncInfo(__FUNCTION__, false);
}

void v8::V8::SetFlagsFromString(char const*, int)
{
    printEmptyFuncInfo(__FUNCTION__, false);
}

// void v8::V8::TerminateExecution(v8::Isolate*)
// {
//     printEmptyFuncInfo(__FUNCTION__, true);
// }

__int64 v8::CpuProfile::GetEndTime(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

__int64 v8::CpuProfile::GetSampleTimestamp(int) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

__int64 v8::CpuProfile::GetStartTime(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

void v8::CpuProfile::Delete(void)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::CpuProfiler::SetIdle(bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::CpuProfiler::SetSamplingInterval(int)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::CpuProfiler::StartProfiling(v8::Local<v8::String>, bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Debug::CancelDebugBreak(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Debug::DebugBreak(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Debug::ProcessDebugMessages(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Debug::SendCommand(v8::Isolate*, unsigned short const*, int, v8::Debug::ClientData*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Debug::SetLiveEditEnabled(v8::Isolate*, bool)
{
    printEmptyFuncInfo(__FUNCTION__, true);
}

void v8::Debug::SetMessageHandler(v8::Isolate*, void(__cdecl*)(v8::Debug::Message const&))
{
    printEmptyFuncInfo(__FUNCTION__, true);
}
bool v8::Debug::CheckDebugBreak(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return true;
}

bool v8::Debug::SetDebugEventListener(v8::Isolate*, void(__cdecl*)(v8::Debug::EventDetails const&), v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return true;
}

int v8::CpuProfile::GetSamplesCount(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::CpuProfileNode::GetChildrenCount(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::CpuProfileNode::GetColumnNumber(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::CpuProfileNode::GetLineNumber(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

int v8::CpuProfileNode::GetScriptId(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return 0;
}

char const* v8::CpuProfileNode::GetBailoutReason(void) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return nullptr;
}

v8::Local<v8::Context> v8::Debug::GetDebugContext(v8::Isolate*)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::Local<v8::Context>();
}

v8::MaybeLocal<v8::Array> v8::Debug::GetInternalProperties(v8::Isolate*, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Array>();
}

v8::MaybeLocal<v8::Value> v8::Debug::Call(v8::Local<v8::Context>, v8::Local<v8::Function>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Value>();
}

v8::MaybeLocal<v8::Value> v8::Debug::GetMirror(v8::Local<v8::Context>, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return v8::MaybeLocal<v8::Value>();
}

bool v8::CpuProfileNode::GetLineTicks(struct v8::CpuProfileNode::LineTick*, unsigned int) const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    return false;
}

int v8::internal::Internals::GetInstanceType(const v8::internal::Object* obj)
{
//     typedef internal::Object O;
//     O* map = ReadField<O*>(obj, kHeapObjectMapOffset);
//     // Map::InstanceType is defined so that it will always be loaded into
//     // the LS 8 bits of one 16-bit word, regardless of endianess.
//     return ReadField<uint16_t>(map, kMapInstanceTypeAndBitFieldOffset) & 0xff;
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    return 0;
}

int v8::internal::Internals::GetOddballKind(const v8::internal::Object* obj)
{
//     typedef internal::Object O;
//     return SmiValue(ReadField<O*>(obj, kOddballKindOffset));
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    return 0;
}

bool v8::internal::Internals::IsExternalTwoByteString(int instance_type)
{
    int representation = (instance_type & kFullStringRepresentationMask);
    return representation == kExternalTwoByteRepresentationTag;
}

uint8_t v8::internal::Internals::GetNodeFlag(v8::internal::Object** obj, int shift)
{
//     uint8_t* addr = reinterpret_cast<uint8_t*>(obj) + kNodeFlagsOffset;
//     return *addr & static_cast<uint8_t>(1U << shift);
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    return 0;
}

void v8::internal::Internals::UpdateNodeFlag(v8::internal::Object** obj, bool value, int shift)
{
//     uint8_t* addr = reinterpret_cast<uint8_t*>(obj) + kNodeFlagsOffset;
//     uint8_t mask = static_cast<uint8_t>(1U << shift);
//     *addr = static_cast<uint8_t>((*addr & ~mask) | (value << shift));
    printEmptyFuncInfo(__FUNCTION__, true);

    miniv8::V8Data* self = v8::Utils::openHandle<v8::Value, miniv8::V8Data>((const v8::Value*)obj);
    uint8_t mask = static_cast<uint8_t>(1U << shift);
    self->m_nodeFlag |= static_cast<uint8_t>((self->m_nodeFlag & ~mask) | (value << shift));
}

uint8_t v8::internal::Internals::GetNodeState(v8::internal::Object** obj)
{
//     uint8_t* addr = reinterpret_cast<uint8_t*>(obj) + kNodeFlagsOffset;
//     return *addr & kNodeStateMask;
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Data* self = v8::Utils::openHandle<v8::Value, miniv8::V8Data>((const v8::Value*)obj);
    return self->m_nodeState & kNodeStateMask;
}

void v8::internal::Internals::UpdateNodeState(v8::internal::Object** obj, uint8_t value)
{
//     uint8_t* addr = reinterpret_cast<uint8_t*>(obj) + kNodeFlagsOffset;
//     *addr = static_cast<uint8_t>((*addr & ~kNodeStateMask) | value);
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Data* self = v8::Utils::openHandle<v8::Value, miniv8::V8Data>((const v8::Value*)obj);
    self->m_nodeState = static_cast<uint8_t>((self->m_nodeState & ~kNodeStateMask) | value);
}

v8::Local<v8::Primitive> v8::Undefined(Isolate* isolate)
{
//     typedef internal::Object* S;
//     typedef internal::Internals I;
//     I::CheckInitialized(isolate);
//     S* slot = I::GetRoot(isolate, I::kUndefinedValueRootIndex);
//     return Local<Primitive>(reinterpret_cast<Primitive*>(slot));
    
    v8::Local<v8::Primitive> ret = v8::Utils::convert<miniv8::V8Data, v8::Primitive>(new miniv8::V8Data(nullptr, JS_UNDEFINED));
    return ret;
}

v8::Local<v8::Primitive> v8::Null(Isolate* isolate)
{
//     typedef internal::Object* S;
//     typedef internal::Internals I;
//     I::CheckInitialized(isolate);
//     S* slot = I::GetRoot(isolate, I::kNullValueRootIndex);
//     return Local<Primitive>(reinterpret_cast<Primitive*>(slot));
    v8::Local<v8::Primitive> ret = v8::Utils::convert<miniv8::V8Data, v8::Primitive>(new miniv8::V8Data(nullptr, JS_NULL));
    return ret;
}

v8::Local<v8::Boolean> v8::True(v8::Isolate* isolate)
{
//     typedef internal::Object* S;
//     typedef internal::Internals I;
//     I::CheckInitialized(isolate);
//     S* slot = I::GetRoot(isolate, I::kTrueValueRootIndex);
//     return Local<Boolean>(reinterpret_cast<Boolean*>(slot));
    v8::Local<v8::Boolean> ret = v8::Utils::convert<miniv8::V8Data, v8::Boolean>(new miniv8::V8Data(nullptr, JS_TRUE));
    return ret;
}

v8::Local<v8::Boolean> v8::False(v8::Isolate* isolate)
{
//     typedef internal::Object* S;
//     typedef internal::Internals I;
//     I::CheckInitialized(isolate);
//     S* slot = I::GetRoot(isolate, I::kFalseValueRootIndex);
//     return Local<Boolean>(reinterpret_cast<Boolean*>(slot));

    v8::Local<v8::Boolean> ret = v8::Utils::convert<miniv8::V8Data, v8::Boolean>(new miniv8::V8Data(nullptr, JS_FALSE));
    return ret;
}

v8::Local<v8::Value> v8::Object::GetInternalField(int index)
{
    DebugBreak();
    v8::Local<v8::Boolean> ret = v8::Utils::convert<miniv8::V8Data, v8::Boolean>(new miniv8::V8Data(nullptr, JS_FALSE));
    return ret;
}

void* v8::Object::GetAlignedPointerFromInternalField(int index)
{
    return SlowGetAlignedPointerFromInternalField(index);
}

v8::Local<v8::String> v8::String::Empty(Isolate* isolate)
{
//     typedef internal::Object* S;
//     typedef internal::Internals I;
//     I::CheckInitialized(isolate);
//     S* slot = I::GetRoot(isolate, I::kEmptyStringRootIndex);
//     return Local<String>(reinterpret_cast<String*>(slot));

    v8::Local<v8::String> ret = v8::Utils::convert<miniv8::V8String, v8::String>(new miniv8::V8String(""));
    return ret;
}

bool v8::Value::IsUndefined() const
{
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    JSValue v = miniv8::V8Data::V8Data_v(self);
    return !!JS_IsUndefined(v);
}

bool v8::Value::QuickIsUndefined() const
{
    DebugBreak();
    return false;
}

bool v8::Value::IsNull() const
{
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);
    JSValue v = miniv8::V8Data::V8Data_v(self);
    return v == JS_NULL;
}

bool v8::Value::QuickIsNull() const
{
    DebugBreak();
    return false;
}

bool v8::Value::IsString() const
{
    miniv8::V8Value* self = v8::Utils::openHandle<v8::Value, miniv8::V8Value>(this);

    if (miniv8::kObjectTypeString == self->m_type)
        return true;
    JSValue v = self->v(self);
    return JS_IsString(v) != 0;
}

bool v8::Value::QuickIsString() const
{
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    return false;
}

void v8::Context::SetAlignedPointerInEmbedderData(int index, void* data)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Context* self = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(this);

    self->m_alignedPointerInEmbedderDatas.insert(std::make_pair(index, data));
}

void v8::Context::SetEmbedderData(int, v8::Local<v8::Value>)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
}

v8::Local<v8::Value> v8::Context::GetEmbedderData(int index)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    DebugBreak();
    return v8::Local<v8::Value>();
}

void* v8::Context::SlowGetAlignedPointerFromEmbedderData(int index)
{
    printEmptyFuncInfo(__FUNCTION__, true);
    miniv8::V8Context* self = v8::Utils::openHandle<v8::Context, miniv8::V8Context>(this);

    std::map<int, void *>::const_iterator it = self->m_alignedPointerInEmbedderDatas.find(index);
    if (it == self->m_alignedPointerInEmbedderDatas.end())
        return nullptr;
    return it->second;
}

void* v8::Context::GetAlignedPointerFromEmbedderData(int index)
{
    return SlowGetAlignedPointerFromEmbedderData(index);
}

namespace v8 {

namespace base {

class SysInfo {
public:
    // Returns the number of logical processors/core on the current machine.
    static int NumberOfProcessors();

    // Returns the number of bytes of physical memory on the current machine.
    static int64_t AmountOfPhysicalMemory();

    // Returns the number of bytes of virtual memory of this process. A return
    // value of zero means that there is no limit on the available virtual memory.
    static int64_t AmountOfVirtualMemory();
};

int SysInfo::NumberOfProcessors()
{
    return 0;
}

int64_t SysInfo::AmountOfPhysicalMemory()
{
    return 0;
}

int64_t SysInfo::AmountOfVirtualMemory()
{
    return 0;
}

}  // namespace base

bool g_patchForCreateDataProperty = false;
const int Function::kLineOffsetNotFound = 0;

}  // namespace v8
