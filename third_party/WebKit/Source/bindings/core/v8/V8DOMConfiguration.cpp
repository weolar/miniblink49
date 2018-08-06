/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "bindings/core/v8/V8DOMConfiguration.h"

#include "bindings/core/v8/V8ObjectConstructor.h"
#include "platform/TraceEvent.h"

namespace blink {

namespace {

void installAttributeInternal(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, const V8DOMConfiguration::AttributeConfiguration& attribute, const DOMWrapperWorld& world)
{
    if (attribute.exposeConfiguration == V8DOMConfiguration::OnlyExposedToPrivateScript
        && !world.isPrivateScriptIsolatedWorld())
        return;

    v8::Local<v8::Name> name = v8AtomicString(isolate, attribute.name);
    v8::AccessorNameGetterCallback getter = attribute.getter;
    v8::AccessorNameSetterCallback setter = attribute.setter;
    if (world.isMainWorld()) {
        if (attribute.getterForMainWorld)
            getter = attribute.getterForMainWorld;
        if (attribute.setterForMainWorld)
            setter = attribute.setterForMainWorld;
    }
    v8::Local<v8::Value> data = v8::External::New(isolate, const_cast<WrapperTypeInfo*>(attribute.data));

    ASSERT(attribute.propertyLocationConfiguration);
    if (attribute.propertyLocationConfiguration & V8DOMConfiguration::OnInstance)
        instanceTemplate->SetAccessor(name, getter, setter, data, attribute.settings, attribute.attribute);
    if (attribute.propertyLocationConfiguration & V8DOMConfiguration::OnPrototype)
        prototypeTemplate->SetAccessor(name, getter, setter, data, attribute.settings, attribute.attribute);
    if (attribute.propertyLocationConfiguration & V8DOMConfiguration::OnInterface)
        ASSERT_NOT_REACHED();
}

template<class FunctionOrTemplate>
v8::Local<FunctionOrTemplate> createAccessorFunctionOrTemplate(v8::Isolate*, v8::FunctionCallback, v8::Local<v8::Value> data, v8::Local<v8::Signature>, int length);

template<>
v8::Local<v8::FunctionTemplate> createAccessorFunctionOrTemplate<v8::FunctionTemplate>(v8::Isolate* isolate, v8::FunctionCallback callback, v8::Local<v8::Value> data, v8::Local<v8::Signature> signature, int length)
{
    v8::Local<v8::FunctionTemplate> functionTemplate;
    if (callback) {
        functionTemplate = v8::FunctionTemplate::New(isolate, callback, data, signature, length);
        if (!functionTemplate.IsEmpty()) {
            functionTemplate->RemovePrototype();
            functionTemplate->SetAcceptAnyReceiver(false);
        }
    }
    return functionTemplate;
}

template<>
v8::Local<v8::Function> createAccessorFunctionOrTemplate<v8::Function>(v8::Isolate* isolate, v8::FunctionCallback callback, v8::Local<v8::Value> data, v8::Local<v8::Signature> signature, int length)
{
    if (!callback)
        return v8::Local<v8::Function>();

    v8::Local<v8::FunctionTemplate> functionTemplate = createAccessorFunctionOrTemplate<v8::FunctionTemplate>(isolate, callback, data, signature, length);
    if (functionTemplate.IsEmpty())
        return v8::Local<v8::Function>();

    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Function> function;
    if (!functionTemplate->GetFunction(context).ToLocal(&function))
        return v8::Local<v8::Function>();
    return function;
}

template<class ObjectOrTemplate, class FunctionOrTemplate>
void installAccessorInternal(v8::Isolate* isolate, v8::Local<ObjectOrTemplate> instanceOrTemplate, v8::Local<ObjectOrTemplate> prototypeOrTemplate, v8::Local<FunctionOrTemplate> interfaceOrTemplate, v8::Local<v8::Signature> signature, const V8DOMConfiguration::AccessorConfiguration& accessor, const DOMWrapperWorld& world)
{
    if (accessor.exposeConfiguration == V8DOMConfiguration::OnlyExposedToPrivateScript
        && !world.isPrivateScriptIsolatedWorld())
        return;

    v8::Local<v8::Name> name = v8AtomicString(isolate, accessor.name);
    v8::FunctionCallback getterCallback = accessor.getter;
    v8::FunctionCallback setterCallback = accessor.setter;
    if (world.isMainWorld()) {
        if (accessor.getterForMainWorld)
            getterCallback = accessor.getterForMainWorld;
        if (accessor.setterForMainWorld)
            setterCallback = accessor.setterForMainWorld;
    }
    // Support [LenientThis] by not specifying the signature.  V8 does not do
    // the type checking against holder if no signature is specified.  Note that
    // info.Holder() passed to callbacks will be *unsafe*.
    if (accessor.holderCheckConfiguration == V8DOMConfiguration::DoNotCheckHolder)
        signature = v8::Local<v8::Signature>();
    v8::Local<v8::Value> data = v8::External::New(isolate, const_cast<WrapperTypeInfo*>(accessor.data));

    ASSERT(accessor.propertyLocationConfiguration);
    if (accessor.propertyLocationConfiguration &
        (V8DOMConfiguration::OnInstance | V8DOMConfiguration::OnPrototype)) {
        v8::Local<FunctionOrTemplate> getter = createAccessorFunctionOrTemplate<FunctionOrTemplate>(isolate, getterCallback, data, signature, 0);
        v8::Local<FunctionOrTemplate> setter = createAccessorFunctionOrTemplate<FunctionOrTemplate>(isolate, setterCallback, data, signature, 1);
        if (accessor.propertyLocationConfiguration & V8DOMConfiguration::OnInstance)
            instanceOrTemplate->SetAccessorProperty(name, getter, setter, accessor.attribute, accessor.settings);
        if (accessor.propertyLocationConfiguration & V8DOMConfiguration::OnPrototype)
            prototypeOrTemplate->SetAccessorProperty(name, getter, setter, accessor.attribute, accessor.settings);
    }
    if (accessor.propertyLocationConfiguration & V8DOMConfiguration::OnInterface) {
        v8::Local<FunctionOrTemplate> getter = createAccessorFunctionOrTemplate<FunctionOrTemplate>(isolate, getterCallback, data, v8::Local<v8::Signature>(), 0);
        v8::Local<FunctionOrTemplate> setter = createAccessorFunctionOrTemplate<FunctionOrTemplate>(isolate, setterCallback, data, v8::Local<v8::Signature>(), 1);
        interfaceOrTemplate->SetAccessorProperty(name, getter, setter, accessor.attribute, accessor.settings);
    }
}

void installConstantInternal(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> functionDescriptor, v8::Local<v8::ObjectTemplate> prototypeTemplate, const V8DOMConfiguration::ConstantConfiguration& constant)
{
    v8::Local<v8::String> constantName = v8AtomicString(isolate, constant.name);
    v8::PropertyAttribute attributes =
        static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
    v8::Local<v8::Primitive> value;
    switch (constant.type) {
    case V8DOMConfiguration::ConstantTypeShort:
    case V8DOMConfiguration::ConstantTypeLong:
    case V8DOMConfiguration::ConstantTypeUnsignedShort:
        value = v8::Integer::New(isolate, constant.ivalue);
        break;
    case V8DOMConfiguration::ConstantTypeUnsignedLong:
        value = v8::Integer::NewFromUnsigned(isolate, constant.ivalue);
        break;
    case V8DOMConfiguration::ConstantTypeFloat:
    case V8DOMConfiguration::ConstantTypeDouble:
        value = v8::Number::New(isolate, constant.dvalue);
        break;
    case V8DOMConfiguration::ConstantTypeString:
        value = v8AtomicString(isolate, constant.svalue);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
    functionDescriptor->Set(constantName, value, attributes);
    prototypeTemplate->Set(constantName, value, attributes);
}

void doInstallMethodInternal(v8::Local<v8::ObjectTemplate> target, v8::Local<v8::Name> name, v8::Local<v8::FunctionTemplate> functionTemplate, v8::PropertyAttribute attribute)
{
    target->Set(name, functionTemplate, attribute);
}

void doInstallMethodInternal(v8::Local<v8::FunctionTemplate> target, v8::Local<v8::Name> name, v8::Local<v8::FunctionTemplate> functionTemplate, v8::PropertyAttribute attribute)
{
    target->Set(name, functionTemplate, attribute);
}

template<class ObjectOrTemplate, class Configuration>
void installMethodInternal(v8::Isolate* isolate, v8::Local<ObjectOrTemplate> objectOrTemplate, v8::Local<v8::Signature> signature, v8::PropertyAttribute attribute, const Configuration& callback, const DOMWrapperWorld& world)
{
    if (callback.exposeConfiguration == V8DOMConfiguration::OnlyExposedToPrivateScript
        && !world.isPrivateScriptIsolatedWorld())
        return;

    v8::Local<v8::FunctionTemplate> functionTemplate = v8::FunctionTemplate::New(isolate, callback.callbackForWorld(world), v8Undefined(), signature, callback.length);
    functionTemplate->RemovePrototype();
    doInstallMethodInternal(objectOrTemplate, callback.methodName(isolate), functionTemplate, attribute);
}

} // namespace

void V8DOMConfiguration::installAttributes(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, const AttributeConfiguration* attributes, size_t attributeCount)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    for (size_t i = 0; i < attributeCount; ++i)
        installAttributeInternal(isolate, instanceTemplate, prototypeTemplate, attributes[i], world);
}

void V8DOMConfiguration::installAttribute(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, const AttributeConfiguration& attribute)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    installAttributeInternal(isolate, instanceTemplate, prototypeTemplate, attribute, world);
}

void V8DOMConfiguration::installAccessors(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, v8::Local<v8::FunctionTemplate> interfaceTemplate, v8::Local<v8::Signature> signature, const AccessorConfiguration* accessors, size_t accessorCount)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    for (size_t i = 0; i < accessorCount; ++i)
        installAccessorInternal(isolate, instanceTemplate, prototypeTemplate, interfaceTemplate, signature, accessors[i], world);
}

void V8DOMConfiguration::installAccessor(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> instanceTemplate, v8::Local<v8::ObjectTemplate> prototypeTemplate, v8::Local<v8::FunctionTemplate> interfaceTemplate, v8::Local<v8::Signature> signature, const AccessorConfiguration& accessor)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    installAccessorInternal(isolate, instanceTemplate, prototypeTemplate, interfaceTemplate, signature, accessor, world);
}

void V8DOMConfiguration::installAccessor(v8::Isolate* isolate, v8::Local<v8::Object> instance, v8::Local<v8::Object> prototype, v8::Local<v8::Function> interface_name, v8::Local<v8::Signature> signature, const AccessorConfiguration& accessor)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    installAccessorInternal(isolate, instance, prototype, interface_name, signature, accessor, world);
}

void V8DOMConfiguration::installConstants(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> functionDescriptor, v8::Local<v8::ObjectTemplate> prototypeTemplate, const ConstantConfiguration* constants, size_t constantCount)
{
    for (size_t i = 0; i < constantCount; ++i)
        installConstantInternal(isolate, functionDescriptor, prototypeTemplate, constants[i]);
}

void V8DOMConfiguration::installConstant(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> functionDescriptor, v8::Local<v8::ObjectTemplate> prototypeTemplate, const ConstantConfiguration& constant)
{
    installConstantInternal(isolate, functionDescriptor, prototypeTemplate, constant);
}

void V8DOMConfiguration::installConstantWithGetter(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> functionDescriptor, v8::Local<v8::ObjectTemplate> prototypeTemplate, const char* name, v8::AccessorNameGetterCallback getter)
{
    v8::Local<v8::String> constantName = v8AtomicString(isolate, name);
    v8::PropertyAttribute attributes =
        static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);
    functionDescriptor->SetNativeDataProperty(constantName, getter, 0, v8::Local<v8::Value>(), attributes);
    prototypeTemplate->SetNativeDataProperty(constantName, getter, 0, v8::Local<v8::Value>(), attributes);
}

void V8DOMConfiguration::installMethods(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> prototypeTemplate, v8::Local<v8::Signature> signature, v8::PropertyAttribute attribute, const MethodConfiguration* callbacks, size_t callbackCount)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    for (size_t i = 0; i < callbackCount; ++i)
        installMethodInternal(isolate, prototypeTemplate, signature, attribute, callbacks[i], world);
}

void V8DOMConfiguration::installMethod(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> functionDescriptor, v8::Local<v8::Signature> signature, v8::PropertyAttribute attribute, const MethodConfiguration& callback)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    installMethodInternal(isolate, functionDescriptor, signature, attribute, callback, world);
}

void V8DOMConfiguration::installMethod(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> prototypeTemplate, v8::Local<v8::Signature> signature, v8::PropertyAttribute attribute, const MethodConfiguration& callback)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    installMethodInternal(isolate, prototypeTemplate, signature, attribute, callback, world);
}

void V8DOMConfiguration::installMethod(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> prototypeTemplate, v8::Local<v8::Signature> signature, v8::PropertyAttribute attribute, const SymbolKeyedMethodConfiguration& callback)
{
    const DOMWrapperWorld& world = DOMWrapperWorld::current(isolate);
    installMethodInternal(isolate, prototypeTemplate, signature, attribute, callback, world);
}

static void setClassString(v8::Isolate* isolate, v8::Local<v8::ObjectTemplate> objectTemplate, const char* classString)
{
    objectTemplate->Set(v8::Symbol::GetToStringTag(isolate), v8AtomicString(isolate, classString), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontEnum));
}

v8::Local<v8::Signature> V8DOMConfiguration::installDOMClassTemplate(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> functionDescriptor, const char* interfaceName, v8::Local<v8::FunctionTemplate> parentClass, size_t fieldCount,
    const AttributeConfiguration* attributes, size_t attributeCount,
    const AccessorConfiguration* accessors, size_t accessorCount,
    const MethodConfiguration* callbacks, size_t callbackCount)
{
    functionDescriptor->SetClassName(v8AtomicString(isolate, interfaceName));
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionDescriptor->InstanceTemplate();
    v8::Local<v8::ObjectTemplate> prototypeTemplate = functionDescriptor->PrototypeTemplate();
    setClassString(isolate, prototypeTemplate, interfaceName);
    instanceTemplate->SetInternalFieldCount(fieldCount);
    if (!parentClass.IsEmpty()) {
        functionDescriptor->Inherit(parentClass);
        // Marks the prototype object as one of native-backed objects.
        // This is needed since bug 110436 asks WebKit to tell native-initiated prototypes from pure-JS ones.
        // This doesn't mark kinds "root" classes like Node, where setting this changes prototype chain structure.
        prototypeTemplate->SetInternalFieldCount(v8PrototypeInternalFieldcount);
    }

    v8::Local<v8::Signature> defaultSignature = v8::Signature::New(isolate, functionDescriptor);
    if (attributeCount)
        installAttributes(isolate, instanceTemplate, prototypeTemplate, attributes, attributeCount);
    if (accessorCount)
        installAccessors(isolate, instanceTemplate, prototypeTemplate, functionDescriptor, defaultSignature, accessors, accessorCount);
    if (callbackCount)
        installMethods(isolate, prototypeTemplate, defaultSignature, static_cast<v8::PropertyAttribute>(v8::None), callbacks, callbackCount);
    return defaultSignature;
}

v8::Local<v8::FunctionTemplate> V8DOMConfiguration::domClassTemplate(v8::Isolate* isolate, WrapperTypeInfo* wrapperTypeInfo, void (*configureDOMClassTemplate)(v8::Local<v8::FunctionTemplate>, v8::Isolate*))
{
    V8PerIsolateData* data = V8PerIsolateData::from(isolate);
    v8::Local<v8::FunctionTemplate> result = data->existingDOMTemplate(wrapperTypeInfo);
    if (!result.IsEmpty())
        return result;

    TRACE_EVENT_SCOPED_SAMPLING_STATE("blink", "BuildDOMTemplate");
    result = v8::FunctionTemplate::New(isolate, V8ObjectConstructor::isValidConstructorMode);
    configureDOMClassTemplate(result, isolate);
    data->setDOMTemplate(wrapperTypeInfo, result);
    return result;
}

} // namespace blink
