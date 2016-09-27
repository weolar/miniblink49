/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "public/web/WebBindings.h"

#include "bindings/core/v8/NPV8Object.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8ArrayBufferView.h"
#include "bindings/core/v8/V8DOMWrapper.h"
#include "bindings/core/v8/V8Element.h"
#include "bindings/core/v8/V8NPObject.h"
#include "bindings/core/v8/V8NPUtils.h"
#include "bindings/core/v8/V8Range.h"
#include "bindings/core/v8/npruntime_impl.h"
#include "bindings/core/v8/npruntime_priv.h"
#include "core/dom/Range.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "public/web/WebArrayBuffer.h"
#include "public/web/WebArrayBufferView.h"
#include "public/web/WebElement.h"
#include "public/web/WebRange.h"

namespace blink {

bool WebBindings::construct(NPP npp, NPObject* object, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    return _NPN_Construct(npp, object, args, argCount, result);
}

NPObject* WebBindings::createObject(NPP npp, NPClass* npClass)
{
    return _NPN_CreateObject(npp, npClass);
}

bool WebBindings::enumerate(NPP npp, NPObject* object, NPIdentifier** identifier, uint32_t* identifierCount)
{
    return _NPN_Enumerate(npp, object, identifier, identifierCount);
}

bool WebBindings::evaluate(NPP npp, NPObject* object, NPString* script, NPVariant* result)
{
    return _NPN_Evaluate(npp, object, script, result);
}

bool WebBindings::evaluateHelper(NPP npp, bool popupsAllowed, NPObject* object, NPString* script, NPVariant* result)
{
    return _NPN_EvaluateHelper(npp, popupsAllowed, object, script, result);
}

NPIdentifier WebBindings::getIntIdentifier(int32_t number)
{
    return _NPN_GetIntIdentifier(number);
}

bool WebBindings::getProperty(NPP npp, NPObject* object, NPIdentifier property, NPVariant* result)
{
    return _NPN_GetProperty(npp, object, property, result);
}

NPIdentifier WebBindings::getStringIdentifier(const NPUTF8* string)
{
    return _NPN_GetStringIdentifier(string);
}

void WebBindings::getStringIdentifiers(const NPUTF8** names, int32_t nameCount, NPIdentifier* identifiers)
{
    _NPN_GetStringIdentifiers(names, nameCount, identifiers);
}

bool WebBindings::hasMethod(NPP npp, NPObject* object, NPIdentifier method)
{
    return _NPN_HasMethod(npp, object, method);
}

bool WebBindings::hasProperty(NPP npp, NPObject* object, NPIdentifier property)
{
    return _NPN_HasProperty(npp, object, property);
}

bool WebBindings::identifierIsString(NPIdentifier identifier)
{
    return _NPN_IdentifierIsString(identifier);
}

int32_t WebBindings::intFromIdentifier(NPIdentifier identifier)
{
    return _NPN_IntFromIdentifier(identifier);
}

void WebBindings::initializeVariantWithStringCopy(NPVariant* variant, const NPString* value)
{
    _NPN_InitializeVariantWithStringCopy(variant, value);
}

bool WebBindings::invoke(NPP npp, NPObject* object, NPIdentifier method, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    return _NPN_Invoke(npp, object, method, args, argCount, result);
}

bool WebBindings::invokeDefault(NPP npp, NPObject* object, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    return _NPN_InvokeDefault(npp, object, args, argCount, result);
}

void WebBindings::releaseObject(NPObject* object)
{
    return _NPN_ReleaseObject(object);
}

void WebBindings::releaseVariantValue(NPVariant* variant)
{
    _NPN_ReleaseVariantValue(variant);
}

bool WebBindings::removeProperty(NPP npp, NPObject* object, NPIdentifier identifier)
{
    return _NPN_RemoveProperty(npp, object, identifier);
}

NPObject* WebBindings::retainObject(NPObject* object)
{
    return _NPN_RetainObject(object);
}

void WebBindings::setException(NPObject* object, const NPUTF8* message)
{
    _NPN_SetException(object, message);
}

bool WebBindings::setProperty(NPP npp, NPObject* object, NPIdentifier identifier, const NPVariant* value)
{
    return _NPN_SetProperty(npp, object, identifier, value);
}

void WebBindings::registerObjectOwner(NPP)
{
}

void WebBindings::unregisterObjectOwner(NPP)
{
}

NPP WebBindings::getObjectOwner(NPObject*)
{
    return 0;
}

void WebBindings::unregisterObject(NPObject* object)
{
    _NPN_UnregisterObject(object);
}

void WebBindings::dropV8WrapperForObject(NPObject* object)
{
    forgetV8ObjectForNPObject(object);
}

NPUTF8* WebBindings::utf8FromIdentifier(NPIdentifier identifier)
{
    return _NPN_UTF8FromIdentifier(identifier);
}

void WebBindings::extractIdentifierData(const NPIdentifier& identifier, const NPUTF8*& string, int32_t& number, bool& isString)
{
    PrivateIdentifier* data = static_cast<PrivateIdentifier*>(identifier);
    if (!data) {
        isString = false;
        number = 0;
        return;
    }

    isString = data->isString;
    if (isString)
        string = data->value.string;
    else
        number = data->value.number;
}

static bool getRangeImpl(NPObject* object, WebRange* webRange, v8::Isolate* isolate)
{
    if (!object)
        return false;

    V8NPObject* v8NPObject = npObjectToV8NPObject(object);
    if (!v8NPObject)
        return false;

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Object> v8Object = v8::Local<v8::Object>::New(isolate, v8NPObject->v8Object);
    if (v8Object.IsEmpty())
        return false;
    if (!V8Range::wrapperTypeInfo.equals(toWrapperTypeInfo(v8Object)))
        return false;

    Range* native = V8Range::hasInstance(v8Object, isolate) ? V8Range::toImpl(v8Object) : 0;
    if (!native)
        return false;

    *webRange = WebRange(native);
    return true;
}

static bool getNodeImpl(NPObject* object, WebNode* webNode, v8::Isolate* isolate)
{
    if (!object)
        return false;

    V8NPObject* v8NPObject = npObjectToV8NPObject(object);
    if (!v8NPObject)
        return false;

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Object> v8Object = v8::Local<v8::Object>::New(isolate, v8NPObject->v8Object);
    if (v8Object.IsEmpty())
        return false;
    Node* native = V8Node::hasInstance(v8Object, isolate) ? V8Node::toImpl(v8Object) : 0;
    if (!native)
        return false;

    *webNode = WebNode(native);
    return true;
}

static bool getElementImpl(NPObject* object, WebElement* webElement, v8::Isolate* isolate)
{
    if (!object)
        return false;

    V8NPObject* v8NPObject = npObjectToV8NPObject(object);
    if (!v8NPObject)
        return false;

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Object> v8Object = v8::Local<v8::Object>::New(isolate, v8NPObject->v8Object);
    if (v8Object.IsEmpty())
        return false;
    Element* native = V8Element::hasInstance(v8Object, isolate) ? V8Element::toImpl(v8Object) : 0;
    if (!native)
        return false;

    *webElement = WebElement(native);
    return true;
}

static bool getArrayBufferImpl(NPObject* object, WebArrayBuffer* arrayBuffer, v8::Isolate* isolate)
{
    if (!object)
        return false;

    V8NPObject* v8NPObject = npObjectToV8NPObject(object);
    if (!v8NPObject)
        return false;

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Object> v8Object = v8::Local<v8::Object>::New(isolate, v8NPObject->v8Object);
    if (v8Object.IsEmpty())
        return false;
    DOMArrayBuffer* impl = V8ArrayBuffer::hasInstance(v8Object, isolate) ? V8ArrayBuffer::toImpl(v8Object) : 0;
    if (!impl)
        return false;

    *arrayBuffer = WebArrayBuffer(impl);
    return true;
}

static bool getArrayBufferViewImpl(NPObject* object, WebArrayBufferView* arrayBufferView, v8::Isolate* isolate)
{
    if (!object)
        return false;

    V8NPObject* v8NPObject = npObjectToV8NPObject(object);
    if (!v8NPObject)
        return false;

    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Object> v8Object = v8::Local<v8::Object>::New(isolate, v8NPObject->v8Object);
    if (v8Object.IsEmpty())
        return false;
    DOMArrayBufferView* impl = V8ArrayBufferView::hasInstance(v8Object, isolate) ? V8ArrayBufferView::toImpl(v8Object) : 0;
    if (!impl)
        return false;

    *arrayBufferView = WebArrayBufferView(impl);
    return true;
}

static NPObject* makeIntArrayImpl(const WebVector<int>& data, v8::Isolate* isolate)
{
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Array> result = v8::Array::New(isolate, data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        if (!v8CallBoolean(result->Set(isolate->GetCurrentContext(), v8::Integer::New(isolate, i), v8::Number::New(isolate, data[i])))) {
            result.Clear();
            break;
        }
    }

    LocalDOMWindow* window = currentDOMWindow(isolate);
    return npCreateV8ScriptObject(isolate, 0, result, window);
}

static NPObject* makeStringArrayImpl(const WebVector<WebString>& data, v8::Isolate* isolate)
{
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Array> result = v8::Array::New(isolate, data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        if (!v8CallBoolean(result->Set(isolate->GetCurrentContext(), v8::Integer::New(isolate, i), v8String(isolate, data[i])))) {
            result.Clear();
            break;
        }
    }

    LocalDOMWindow* window = currentDOMWindow(isolate);
    return npCreateV8ScriptObject(isolate, 0, result, window);
}

bool WebBindings::getRange(NPObject* range, WebRange* webRange)
{
    return getRangeImpl(range, webRange, v8::Isolate::GetCurrent());
}

bool WebBindings::getArrayBuffer(NPObject* arrayBuffer, WebArrayBuffer* webArrayBuffer)
{
    return getArrayBufferImpl(arrayBuffer, webArrayBuffer, v8::Isolate::GetCurrent());
}

bool WebBindings::getArrayBufferView(NPObject* arrayBufferView, WebArrayBufferView* webArrayBufferView)
{
    return getArrayBufferViewImpl(arrayBufferView, webArrayBufferView, v8::Isolate::GetCurrent());
}

bool WebBindings::getNode(NPObject* node, WebNode* webNode)
{
    return getNodeImpl(node, webNode, v8::Isolate::GetCurrent());
}

bool WebBindings::getElement(NPObject* element, WebElement* webElement)
{
    return getElementImpl(element, webElement, v8::Isolate::GetCurrent());
}

NPObject* WebBindings::makeIntArray(const WebVector<int>& data)
{
    return makeIntArrayImpl(data, v8::Isolate::GetCurrent());
}

NPObject* WebBindings::makeStringArray(const WebVector<WebString>& data)
{
    return makeStringArrayImpl(data, v8::Isolate::GetCurrent());
}

void WebBindings::pushExceptionHandler(ExceptionHandler handler, void* data)
{
    blink::pushExceptionHandler(handler, data);
}

void WebBindings::popExceptionHandler()
{
    blink::popExceptionHandler();
}

void WebBindings::toNPVariant(v8::Local<v8::Value> object, NPObject* root, NPVariant* result)
{
    convertV8ObjectToNPVariant(v8::Isolate::GetCurrent(), object, root, result);
}

v8::Local<v8::Value> WebBindings::toV8Value(const NPVariant* variant)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    if (variant->type == NPVariantType_Object) {
        NPObject* object = NPVARIANT_TO_OBJECT(*variant);
        V8NPObject* v8Object = npObjectToV8NPObject(object);
        if (!v8Object)
            return v8::Undefined(isolate);
        return convertNPVariantToV8Object(isolate, variant, v8Object->rootObject->frame()->script().windowScriptNPObject());
    }
    // Safe to pass 0 since we have checked the script object class to make sure the
    // argument is a primitive v8 type.
    return convertNPVariantToV8Object(isolate, variant, 0);
}

} // namespace blink
