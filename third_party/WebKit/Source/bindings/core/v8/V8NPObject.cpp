/*
* Copyright (C) 2006, 2007, 2008, 2009 Google Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*     * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following disclaimer
* in the documentation and/or other materials provided with the
* distribution.
*     * Neither the name of Google Inc. nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"

#include "bindings/core/v8/V8NPObject.h"

#include "bindings/core/v8/NPV8Object.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8GlobalValueMap.h"
#include "bindings/core/v8/V8HTMLAppletElement.h"
#include "bindings/core/v8/V8HTMLEmbedElement.h"
#include "bindings/core/v8/V8HTMLObjectElement.h"
#include "bindings/core/v8/V8NPUtils.h"
#include "bindings/core/v8/V8ObjectConstructor.h"
#include "bindings/core/v8/npruntime_impl.h"
#include "bindings/core/v8/npruntime_priv.h"
#include "core/html/HTMLPlugInElement.h"
#include "v8-util.h"
#include "wtf/OwnPtr.h"

namespace blink {

enum InvokeFunctionType {
    InvokeMethod = 1,
    InvokeConstruct = 2,
    InvokeDefault = 3
};

struct IdentifierRep {
    int number() const { return m_isString ? 0 : m_value.m_number; }
    const char* string() const { return m_isString ? m_value.m_string : 0; }

    union {
        const char* m_string;
        int m_number;
    } m_value;
    bool m_isString;
};

// FIXME: need comments.
// Params: holder could be HTMLEmbedElement or NPObject
static void npObjectInvokeImpl(const v8::FunctionCallbackInfo<v8::Value>& info, InvokeFunctionType functionId)
{
    NPObject* npObject;
    v8::Isolate* isolate = info.GetIsolate();

    // These three types are subtypes of HTMLPlugInElement.
    HTMLPlugInElement* element = V8HTMLAppletElement::toImplWithTypeCheck(isolate, info.Holder());
    if (!element) {
        element = V8HTMLEmbedElement::toImplWithTypeCheck(isolate, info.Holder());
        if (!element) {
            element = V8HTMLObjectElement::toImplWithTypeCheck(isolate, info.Holder());
        }
    }
    if (element) {
        if (RefPtr<SharedPersistent<v8::Object>> wrapper = element->pluginWrapper()) {
            v8::HandleScope handleScope(isolate);
            npObject = v8ObjectToNPObject(wrapper->newLocal(isolate));
        } else {
            npObject = 0;
        }
    } else {
        // The holder object is not a subtype of HTMLPlugInElement, it must be an NPObject which has three
        // internal fields.
        if (info.Holder()->InternalFieldCount() != npObjectInternalFieldCount) {
            V8ThrowException::throwReferenceError(info.GetIsolate(), "NPMethod called on non-NPObject");
            return;
        }

        npObject = v8ObjectToNPObject(info.Holder());
    }

    // Verify that our wrapper wasn't using a NPObject which has already been deleted.
    if (!npObject || !_NPN_IsAlive(npObject)) {
        V8ThrowException::throwReferenceError(isolate, "NPObject deleted");
        return;
    }

    // Wrap up parameters.
    int numArgs = info.Length();
    OwnPtr<NPVariant[]> npArgs = adoptArrayPtr(new NPVariant[numArgs]);

    for (int i = 0; i < numArgs; i++)
        convertV8ObjectToNPVariant(isolate, info[i], npObject, &npArgs[i]);

    NPVariant result;
    VOID_TO_NPVARIANT(result);

    bool retval = true;
    switch (functionId) {
    case InvokeMethod:
        if (npObject->_class->invoke) {
            v8::Local<v8::String> functionName = v8::Local<v8::String>::Cast(info.Data());
            NPIdentifier identifier = getStringIdentifier(info.GetIsolate(), functionName);
            retval = npObject->_class->invoke(npObject, identifier, npArgs.get(), numArgs, &result);
        }
        break;
    case InvokeConstruct:
        if (npObject->_class->construct)
            retval = npObject->_class->construct(npObject, npArgs.get(), numArgs, &result);
        break;
    case InvokeDefault:
        if (npObject->_class->invokeDefault)
            retval = npObject->_class->invokeDefault(npObject, npArgs.get(), numArgs, &result);
        break;
    default:
        break;
    }

    if (!retval)
        V8ThrowException::throwGeneralError(isolate, "Error calling method on NPObject.");

    for (int i = 0; i < numArgs; i++)
        _NPN_ReleaseVariantValue(&npArgs[i]);

    // Unwrap return values.
    v8::Local<v8::Value> returnValue;
    if (_NPN_IsAlive(npObject))
        returnValue = convertNPVariantToV8Object(isolate, &result, npObject);
    _NPN_ReleaseVariantValue(&result);

    v8SetReturnValue(info, returnValue);
}


void npObjectMethodHandler(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    return npObjectInvokeImpl(info, InvokeMethod);
}


void npObjectInvokeDefaultHandler(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.IsConstructCall()) {
        npObjectInvokeImpl(info, InvokeConstruct);
        return;
    }

    npObjectInvokeImpl(info, InvokeDefault);
}

class V8TemplateMapTraits : public V8GlobalValueMapTraits<PrivateIdentifier*, v8::FunctionTemplate, v8::kWeakWithParameter> {
public:
    typedef v8::GlobalValueMap<PrivateIdentifier*, v8::FunctionTemplate, V8TemplateMapTraits> MapType;
    typedef PrivateIdentifier WeakCallbackDataType;

    static WeakCallbackDataType* WeakCallbackParameter(MapType* map, PrivateIdentifier* key, const v8::Local<v8::FunctionTemplate>& value)
    {
        return key;
    }

    static void DisposeCallbackData(WeakCallbackDataType* callbackData) { }

    static MapType* MapFromWeakCallbackInfo(
        const v8::WeakCallbackInfo<WeakCallbackDataType>&);

    static PrivateIdentifier* KeyFromWeakCallbackInfo(
        const v8::WeakCallbackInfo<WeakCallbackDataType>& data)
    {
        return data.GetParameter();
    }

    // Dispose traits:
    static void OnWeakCallback(const v8::WeakCallbackInfo<WeakCallbackDataType>& data) { }
    static void Dispose(v8::Isolate* isolate, v8::Global<v8::FunctionTemplate> value, PrivateIdentifier* key) { }
    static void DisposeWeak(const v8::WeakCallbackInfo<WeakCallbackDataType>& data) { }
};


class V8NPTemplateMap {
public:
    // NPIdentifier is PrivateIdentifier*.
    typedef v8::GlobalValueMap<PrivateIdentifier*, v8::FunctionTemplate, V8TemplateMapTraits> MapType;

    v8::Local<v8::FunctionTemplate> get(PrivateIdentifier* key)
    {
        return m_map.Get(key);
    }

    void set(PrivateIdentifier* key, v8::Local<v8::FunctionTemplate> handle)
    {
        ASSERT(!m_map.Contains(key));
        m_map.Set(key, handle);
    }

    static V8NPTemplateMap& sharedInstance(v8::Isolate* isolate)
    {
        DEFINE_STATIC_LOCAL(V8NPTemplateMap, map, (isolate));
        ASSERT(isolate == map.m_map.GetIsolate());
        return map;
    }

    friend class V8TemplateMapTraits;

private:
    explicit V8NPTemplateMap(v8::Isolate* isolate)
        : m_map(isolate)
    {
    }

    MapType m_map;
};

V8TemplateMapTraits::MapType* V8TemplateMapTraits::MapFromWeakCallbackInfo(const v8::WeakCallbackInfo<WeakCallbackDataType>& data)
{
    return &V8NPTemplateMap::sharedInstance(data.GetIsolate()).m_map;
}

static v8::Local<v8::Value> npObjectGetProperty(v8::Isolate* isolate, v8::Local<v8::Object> self, NPIdentifier identifier, v8::Local<v8::Value> key)
{
    NPObject* npObject = v8ObjectToNPObject(self);

    // Verify that our wrapper wasn't using a NPObject which
    // has already been deleted.
    if (!npObject || !_NPN_IsAlive(npObject))
        return V8ThrowException::throwReferenceError(isolate, "NPObject deleted");

    if (npObject->_class->hasProperty && npObject->_class->getProperty && npObject->_class->hasProperty(npObject, identifier)) {
        if (!_NPN_IsAlive(npObject))
            return V8ThrowException::throwReferenceError(isolate, "NPObject deleted");

        NPVariant result;
        VOID_TO_NPVARIANT(result);
        if (!npObject->_class->getProperty(npObject, identifier, &result))
            return v8Undefined();

        v8::Local<v8::Value> returnValue;
        if (_NPN_IsAlive(npObject))
            returnValue = convertNPVariantToV8Object(isolate, &result, npObject);
        _NPN_ReleaseVariantValue(&result);
        return returnValue;
    }

    if (!_NPN_IsAlive(npObject))
        return V8ThrowException::throwReferenceError(isolate, "NPObject deleted");

    if (key->IsString() && npObject->_class->hasMethod && npObject->_class->hasMethod(npObject, identifier)) {
        if (!_NPN_IsAlive(npObject))
            return V8ThrowException::throwReferenceError(isolate, "NPObject deleted");

        PrivateIdentifier* id = static_cast<PrivateIdentifier*>(identifier);
        v8::Local<v8::FunctionTemplate> functionTemplate = V8NPTemplateMap::sharedInstance(isolate).get(id);
        // Cache templates using identifier as the key.
        if (functionTemplate.IsEmpty()) {
            // Create a new template.
            functionTemplate = v8::FunctionTemplate::New(isolate);
            functionTemplate->SetCallHandler(npObjectMethodHandler, key);
            V8NPTemplateMap::sharedInstance(isolate).set(id, functionTemplate);
        }
        v8::Local<v8::Function> v8Function;
        if (!functionTemplate->GetFunction(isolate->GetCurrentContext()).ToLocal(&v8Function))
            return v8Undefined();
        v8Function->SetName(v8::Local<v8::String>::Cast(key));
        return v8Function;
    }

    return v8Undefined();
}

void npObjectNamedPropertyGetter(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = getStringIdentifier(info.GetIsolate(), name);
    v8SetReturnValue(info, npObjectGetProperty(info.GetIsolate(), info.Holder(), identifier, name));
}

void npObjectIndexedPropertyGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = _NPN_GetIntIdentifier(index);
    v8SetReturnValue(info, npObjectGetProperty(info.GetIsolate(), info.Holder(), identifier, v8::Number::New(info.GetIsolate(), index)));
}

void npObjectGetNamedProperty(v8::Local<v8::Object> self, v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = getStringIdentifier(info.GetIsolate(), name);
    v8SetReturnValue(info, npObjectGetProperty(info.GetIsolate(), self, identifier, name));
}

void npObjectGetIndexedProperty(v8::Local<v8::Object> self, uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = _NPN_GetIntIdentifier(index);
    v8SetReturnValue(info, npObjectGetProperty(info.GetIsolate(), self, identifier, v8::Number::New(info.GetIsolate(), index)));
}

void npObjectQueryProperty(v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Integer>& info)
{
    NPIdentifier identifier = getStringIdentifier(info.GetIsolate(), name);
    if (npObjectGetProperty(info.GetIsolate(), info.Holder(), identifier, name).IsEmpty())
        return;
    v8SetReturnValueInt(info, 0);
}

static v8::Local<v8::Value> npObjectSetProperty(v8::Local<v8::Object> self, NPIdentifier identifier, v8::Local<v8::Value> value, v8::Isolate* isolate)
{
    NPObject* npObject = v8ObjectToNPObject(self);

    // Verify that our wrapper wasn't using a NPObject which has already been deleted.
    if (!npObject || !_NPN_IsAlive(npObject)) {
        V8ThrowException::throwReferenceError(isolate, "NPObject deleted");
        return value; // Intercepted, but an exception was thrown.
    }

    if (npObject->_class->hasProperty && npObject->_class->setProperty && npObject->_class->hasProperty(npObject, identifier)) {
        if (!_NPN_IsAlive(npObject))
            return V8ThrowException::throwReferenceError(isolate, "NPObject deleted");

        NPVariant npValue;
        VOID_TO_NPVARIANT(npValue);
        convertV8ObjectToNPVariant(isolate, value, npObject, &npValue);
        bool success = npObject->_class->setProperty(npObject, identifier, &npValue);
        _NPN_ReleaseVariantValue(&npValue);
        if (success)
            return value; // Intercept the call.
    }
    return v8Undefined();
}


void npObjectNamedPropertySetter(v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = getStringIdentifier(info.GetIsolate(), name);
    v8SetReturnValue(info, npObjectSetProperty(info.Holder(), identifier, value, info.GetIsolate()));
}


void npObjectIndexedPropertySetter(uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = _NPN_GetIntIdentifier(index);
    v8SetReturnValue(info, npObjectSetProperty(info.Holder(), identifier, value, info.GetIsolate()));
}

void npObjectSetNamedProperty(v8::Local<v8::Object> self, v8::Local<v8::String> name, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = getStringIdentifier(info.GetIsolate(), name);
    v8SetReturnValue(info, npObjectSetProperty(self, identifier, value, info.GetIsolate()));
}

void npObjectSetIndexedProperty(v8::Local<v8::Object> self, uint32_t index, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    NPIdentifier identifier = _NPN_GetIntIdentifier(index);
    v8SetReturnValue(info, npObjectSetProperty(self, identifier, value, info.GetIsolate()));
}

void npObjectPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info, bool namedProperty)
{
    NPObject* npObject = v8ObjectToNPObject(info.Holder());

    // Verify that our wrapper wasn't using a NPObject which
    // has already been deleted.
    if (!npObject || !_NPN_IsAlive(npObject)) {
        V8ThrowException::throwReferenceError(info.GetIsolate(), "NPObject deleted");
        return;
    }

    if (NP_CLASS_STRUCT_VERSION_HAS_ENUM(npObject->_class) && npObject->_class->enumerate) {
        uint32_t count;
        NPIdentifier* identifiers;
        if (npObject->_class->enumerate(npObject, &identifiers, &count)) {
            uint32_t propertiesCount = 0;
            for (uint32_t i = 0; i < count; ++i) {
                IdentifierRep* identifier = static_cast<IdentifierRep*>(identifiers[i]);
                if (namedProperty == identifier->m_isString)
                    ++propertiesCount;
            }
            v8::Local<v8::Array> properties = v8::Array::New(info.GetIsolate(), propertiesCount);
            for (uint32_t i = 0, propertyIndex = 0; i < count; ++i) {
                IdentifierRep* identifier = static_cast<IdentifierRep*>(identifiers[i]);
                if (namedProperty == identifier->m_isString) {
                    ASSERT(propertyIndex < propertiesCount);
                    v8::Local<v8::Value> value;
                    if (namedProperty)
                        value = v8AtomicString(info.GetIsolate(), identifier->string());
                    else
                        value = v8::Integer::New(info.GetIsolate(), identifier->number());
                    v8::Local<v8::Number> index = v8::Integer::New(info.GetIsolate(), propertyIndex++);
                    if (!v8CallBoolean(properties->Set(info.GetIsolate()->GetCurrentContext(), index, value)))
                        return;
                }
            }

            v8SetReturnValue(info, properties);
            return;
        }
    }
}

void npObjectNamedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info)
{
    npObjectPropertyEnumerator(info, true);
}

void npObjectIndexedPropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array>& info)
{
    npObjectPropertyEnumerator(info, false);
}

static DOMWrapperMap<NPObject>& staticNPObjectMap()
{
    DEFINE_STATIC_LOCAL(DOMWrapperMap<NPObject>, npObjectMap, (v8::Isolate::GetCurrent()));
    return npObjectMap;
}

template <>
inline void DOMWrapperMap<NPObject>::PersistentValueMapTraits::Dispose(
    v8::Isolate* isolate,
    v8::Global<v8::Object> value,
    NPObject* npObject)
{
    ASSERT(npObject);
    if (_NPN_IsAlive(npObject))
        _NPN_ReleaseObject(npObject);
}

template <>
inline void DOMWrapperMap<NPObject>::PersistentValueMapTraits::DisposeWeak(v8::Isolate* isolate, void* internalFields[v8::kInternalFieldsInWeakCallback], NPObject* npObject)
{
    ASSERT(npObject);
    if (_NPN_IsAlive(npObject))
        _NPN_ReleaseObject(npObject);
}

v8::Local<v8::Object> createV8ObjectForNPObject(v8::Isolate* isolate, NPObject* object, NPObject* root)
{
    static v8::Eternal<v8::FunctionTemplate> npObjectDesc;

    ASSERT(isolate->InContext());

    // If this is a v8 object, just return it.
    V8NPObject* v8NPObject = npObjectToV8NPObject(object);
    if (v8NPObject)
        return v8::Local<v8::Object>::New(isolate, v8NPObject->v8Object);

    // If we've already wrapped this object, just return it.
    v8::Local<v8::Object> wrapper = staticNPObjectMap().newLocal(isolate, object);
    if (!wrapper.IsEmpty())
        return wrapper;

    // FIXME: we should create a Wrapper type as a subclass of JSObject. It has two internal fields, field 0 is the wrapped
    // pointer, and field 1 is the type. There should be an api function that returns unused type id. The same Wrapper type
    // can be used by DOM bindings.
    if (npObjectDesc.IsEmpty()) {
        v8::Local<v8::FunctionTemplate> templ = v8::FunctionTemplate::New(isolate);
        templ->InstanceTemplate()->SetInternalFieldCount(npObjectInternalFieldCount);
        templ->InstanceTemplate()->SetNamedPropertyHandler(npObjectNamedPropertyGetter, npObjectNamedPropertySetter, npObjectQueryProperty, 0, npObjectNamedPropertyEnumerator);
        templ->InstanceTemplate()->SetIndexedPropertyHandler(npObjectIndexedPropertyGetter, npObjectIndexedPropertySetter, 0, 0, npObjectIndexedPropertyEnumerator);
        templ->InstanceTemplate()->SetCallAsFunctionHandler(npObjectInvokeDefaultHandler);
        npObjectDesc.Set(isolate, templ);
    }

    // FIXME: Move staticNPObjectMap() to DOMDataStore.
    // Use V8DOMWrapper::createWrapper() and
    // V8DOMWrapper::associateObjectWithWrapper()
    // to create a wrapper object.
    v8::Local<v8::Function> v8Function;
    if (!npObjectDesc.Get(isolate)->GetFunction(isolate->GetCurrentContext()).ToLocal(&v8Function))
        return v8::Local<v8::Object>();
    v8::Local<v8::Object> value;
    if (!V8ObjectConstructor::newInstance(isolate, v8Function).ToLocal(&value))
        return v8::Local<v8::Object>();

    V8DOMWrapper::setNativeInfo(value, npObjectTypeInfo(), npObjectToScriptWrappable(object));

    // KJS retains the object as part of its wrapper (see Bindings::CInstance).
    _NPN_RetainObject(object);
    _NPN_RegisterObject(object, root);

    bool wrapperDidNotExist = staticNPObjectMap().set(object, npObjectTypeInfo(), value);
    ASSERT_UNUSED(wrapperDidNotExist, wrapperDidNotExist);
    ASSERT(V8DOMWrapper::hasInternalFieldsSet(value));
    return value;
}

void forgetV8ObjectForNPObject(NPObject* object)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::HandleScope scope(isolate);
    v8::Local<v8::Object> wrapper = staticNPObjectMap().newLocal(isolate, object);
    if (!wrapper.IsEmpty()) {
        V8DOMWrapper::clearNativeInfo(wrapper, npObjectTypeInfo());
        staticNPObjectMap().removeAndDispose(object);
        _NPN_ReleaseObject(object);
    }
}

} // namespace blink
