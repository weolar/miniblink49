/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#include "bindings/core/v8/CustomElementConstructorBuilder.h"
#include "config.h"

#include "bindings/core/v8/CustomElementBinding.h"
#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8Document.h"
#include "bindings/core/v8/V8HTMLElement.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "bindings/core/v8/V8PerContextData.h"
#include "bindings/core/v8/V8SVGElement.h"
#include "core/HTMLNames.h"
#include "core/SVGNames.h"
#include "core/dom/Document.h"
#include "core/dom/ElementRegistrationOptions.h"
#include "core/dom/custom/CustomElementDefinition.h"
#include "core/dom/custom/CustomElementDescriptor.h"
#include "core/dom/custom/CustomElementException.h"
#include "core/dom/custom/CustomElementProcessingStack.h"
#include "wtf/Assertions.h"

namespace blink {

static void constructCustomElement(const v8::FunctionCallbackInfo<v8::Value>&);

CustomElementConstructorBuilder::CustomElementConstructorBuilder(ScriptState* scriptState, const ElementRegistrationOptions& options)
    : m_scriptState(scriptState)
    , m_options(options)
{
    ASSERT(m_scriptState->context() == m_scriptState->isolate()->GetCurrentContext());
}

bool CustomElementConstructorBuilder::isFeatureAllowed() const
{
    return m_scriptState->world().isMainWorld();
}

bool CustomElementConstructorBuilder::validateOptions(const AtomicString& type, QualifiedName& tagName, ExceptionState& exceptionState)
{
    ASSERT(m_prototype.IsEmpty());
    v8::TryCatch tryCatch(m_scriptState->isolate());
    if (!m_scriptState->perContextData()) {
        // FIXME: This should generate an InvalidContext exception at a later point.
        CustomElementException::throwException(CustomElementException::ContextDestroyedCheckingPrototype, type, exceptionState);
        tryCatch.ReThrow();
        return false;
    }

    if (m_options.hasPrototype()) {
        ASSERT(m_options.prototype().isObject());
        m_prototype = m_options.prototype().v8Value().As<v8::Object>();
    } else {
        m_prototype = v8::Object::New(m_scriptState->isolate());
        v8::Local<v8::Object> basePrototype = m_scriptState->perContextData()->prototypeForType(&V8HTMLElement::wrapperTypeInfo);
        if (!basePrototype.IsEmpty()) {
            if (!v8CallBoolean(m_prototype->SetPrototype(m_scriptState->context(), basePrototype)))
                return false;
        }
    }

    AtomicString namespaceURI = HTMLNames::xhtmlNamespaceURI;
    if (hasValidPrototypeChainFor(&V8SVGElement::wrapperTypeInfo))
        namespaceURI = SVGNames::svgNamespaceURI;

    ASSERT(!tryCatch.HasCaught());

    AtomicString localName;

    if (m_options.hasExtends()) {
        localName = AtomicString(m_options.extends().lower());

        if (!Document::isValidName(localName)) {
            CustomElementException::throwException(CustomElementException::ExtendsIsInvalidName, type, exceptionState);
            tryCatch.ReThrow();
            return false;
        }
        if (CustomElement::isValidName(localName)) {
            CustomElementException::throwException(CustomElementException::ExtendsIsCustomElementName, type, exceptionState);
            tryCatch.ReThrow();
            return false;
        }
    } else {
        if (namespaceURI == SVGNames::svgNamespaceURI) {
            CustomElementException::throwException(CustomElementException::ExtendsIsInvalidName, type, exceptionState);
            tryCatch.ReThrow();
            return false;
        }
        localName = type;
    }

    ASSERT(!tryCatch.HasCaught());
    tagName = QualifiedName(nullAtom, localName, namespaceURI);
    return true;
}

PassRefPtrWillBeRawPtr<CustomElementLifecycleCallbacks> CustomElementConstructorBuilder::createCallbacks()
{
    ASSERT(!m_prototype.IsEmpty());
    v8::TryCatch exceptionCatcher(m_scriptState->isolate());
    exceptionCatcher.SetVerbose(true);

    v8::MaybeLocal<v8::Function> created = retrieveCallback("createdCallback");
    v8::MaybeLocal<v8::Function> attached = retrieveCallback("attachedCallback");
    v8::MaybeLocal<v8::Function> detached = retrieveCallback("detachedCallback");
    v8::MaybeLocal<v8::Function> attributeChanged = retrieveCallback("attributeChangedCallback");

    m_callbacks = V8CustomElementLifecycleCallbacks::create(m_scriptState.get(), m_prototype, created, attached, detached, attributeChanged);
    return m_callbacks.get();
}

v8::MaybeLocal<v8::Function> CustomElementConstructorBuilder::retrieveCallback(const char* name)
{
    v8::Local<v8::Value> value;
    if (!m_prototype->Get(m_scriptState->context(), v8String(m_scriptState->isolate(), name)).ToLocal(&value) || !value->IsFunction())
        return v8::MaybeLocal<v8::Function>();
    return v8::MaybeLocal<v8::Function>(value.As<v8::Function>());
}

bool CustomElementConstructorBuilder::createConstructor(Document* document, CustomElementDefinition* definition, ExceptionState& exceptionState)
{
    ASSERT(!m_prototype.IsEmpty());
    ASSERT(m_constructor.IsEmpty());
    ASSERT(document);

    v8::Isolate* isolate = m_scriptState->isolate();
    v8::Local<v8::Context> context = m_scriptState->context();

    if (!prototypeIsValid(definition->descriptor().type(), exceptionState))
        return false;

    v8::Local<v8::FunctionTemplate> constructorTemplate = v8::FunctionTemplate::New(isolate);
    constructorTemplate->SetCallHandler(constructCustomElement);
    if (!constructorTemplate->GetFunction(context).ToLocal(&m_constructor)) {
        CustomElementException::throwException(CustomElementException::ContextDestroyedRegisteringDefinition, definition->descriptor().type(), exceptionState);
        return false;
    }

    const CustomElementDescriptor& descriptor = definition->descriptor();

    v8::Local<v8::String> v8TagName = v8String(isolate, descriptor.localName());
    v8::Local<v8::Value> v8Type;
    if (descriptor.isTypeExtension())
        v8Type = v8String(isolate, descriptor.type());
    else
        v8Type = v8::Null(isolate);

    m_constructor->SetName(v8Type->IsNull() ? v8TagName : v8Type.As<v8::String>());

    V8HiddenValue::setHiddenValue(isolate, m_constructor, V8HiddenValue::customElementDocument(isolate), toV8(document, context->Global(), isolate));
    V8HiddenValue::setHiddenValue(isolate, m_constructor, V8HiddenValue::customElementNamespaceURI(isolate), v8String(isolate, descriptor.namespaceURI()));
    V8HiddenValue::setHiddenValue(isolate, m_constructor, V8HiddenValue::customElementTagName(isolate), v8TagName);
    V8HiddenValue::setHiddenValue(isolate, m_constructor, V8HiddenValue::customElementType(isolate), v8Type);

    v8::Local<v8::String> prototypeKey = v8String(isolate, "prototype");
    if (!v8CallBoolean(m_constructor->HasOwnProperty(context, prototypeKey)))
        return false;
    // This sets the property *value*; calling Set is safe because
    // "prototype" is a non-configurable data property so there can be
    // no side effects.
    if (!v8CallBoolean(m_constructor->Set(context, prototypeKey, m_prototype)))
        return false;
    // This *configures* the property. DefineOwnProperty of a function's
    // "prototype" does not affect the value, but can reconfigure the
    // property.
    if (!v8CallBoolean(m_constructor->DefineOwnProperty(context, prototypeKey, m_prototype, v8::PropertyAttribute(v8::ReadOnly | v8::DontEnum | v8::DontDelete))))
        return false;

    v8::Local<v8::String> constructorKey = v8String(isolate, "constructor");
    v8::Local<v8::Value> constructorPrototype;
    if (!m_prototype->Get(context, constructorKey).ToLocal(&constructorPrototype))
        return false;

    if (!v8CallBoolean(m_constructor->SetPrototype(context, constructorPrototype)))
        return false;

    V8HiddenValue::setHiddenValue(isolate, m_prototype, V8HiddenValue::customElementIsInterfacePrototypeObject(isolate), v8::True(isolate));
    if (!v8CallBoolean(m_prototype->DefineOwnProperty(context, v8String(isolate, "constructor"), m_constructor, v8::DontEnum)))
        return false;

    return true;
}

bool CustomElementConstructorBuilder::prototypeIsValid(const AtomicString& type, ExceptionState& exceptionState) const
{
    if (m_prototype->InternalFieldCount() || !V8HiddenValue::getHiddenValue(m_scriptState->isolate(), m_prototype, V8HiddenValue::customElementIsInterfacePrototypeObject(m_scriptState->isolate())).IsEmpty()) {
        CustomElementException::throwException(CustomElementException::PrototypeInUse, type, exceptionState);
        return false;
    }

    v8::PropertyAttribute propertyAttribute;
    if (!v8Call(m_prototype->GetPropertyAttributes(m_scriptState->context(), v8String(m_scriptState->isolate(), "constructor")), propertyAttribute) || (propertyAttribute & v8::DontDelete)) {
        CustomElementException::throwException(CustomElementException::ConstructorPropertyNotConfigurable, type, exceptionState);
        return false;
    }

    return true;
}

bool CustomElementConstructorBuilder::didRegisterDefinition(CustomElementDefinition* definition) const
{
    ASSERT(!m_constructor.IsEmpty());

    return m_callbacks->setBinding(definition, CustomElementBinding::create(m_scriptState->isolate(), m_prototype));
}

ScriptValue CustomElementConstructorBuilder::bindingsReturnValue() const
{
    return ScriptValue(m_scriptState.get(), m_constructor);
}

bool CustomElementConstructorBuilder::hasValidPrototypeChainFor(const WrapperTypeInfo* type) const
{
    v8::Local<v8::Object> elementPrototype = m_scriptState->perContextData()->prototypeForType(type);
    if (elementPrototype.IsEmpty())
        return false;

    v8::Local<v8::Value> chain = m_prototype;
    while (!chain.IsEmpty() && chain->IsObject()) {
        if (chain == elementPrototype)
            return true;
        chain = chain.As<v8::Object>()->GetPrototype();
    }

    return false;
}

static void constructCustomElement(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();

    if (!info.IsConstructCall()) {
        V8ThrowException::throwTypeError(isolate, "DOM object constructor cannot be called as a function.");
        return;
    }

    if (info.Length() > 0) {
        V8ThrowException::throwTypeError(isolate, "This constructor should be called without arguments.");
        return;
    }

    Document* document = V8Document::toImpl(V8HiddenValue::getHiddenValue(info.GetIsolate(), info.Callee(), V8HiddenValue::customElementDocument(isolate)).As<v8::Object>());
    TOSTRING_VOID(V8StringResource<>, namespaceURI, V8HiddenValue::getHiddenValue(isolate, info.Callee(), V8HiddenValue::customElementNamespaceURI(isolate)));
    TOSTRING_VOID(V8StringResource<>, tagName, V8HiddenValue::getHiddenValue(isolate, info.Callee(), V8HiddenValue::customElementTagName(isolate)));
    v8::Local<v8::Value> maybeType = V8HiddenValue::getHiddenValue(info.GetIsolate(), info.Callee(), V8HiddenValue::customElementType(isolate));
    TOSTRING_VOID(V8StringResource<>, type, maybeType);

    ExceptionState exceptionState(ExceptionState::ConstructionContext, "CustomElement", info.Holder(), info.GetIsolate());
    CustomElementProcessingStack::CallbackDeliveryScope deliveryScope;
    RefPtrWillBeRawPtr<Element> element = document->createElementNS(namespaceURI, tagName, maybeType->IsNull() ? nullAtom : type, exceptionState);
    if (exceptionState.throwIfNeeded())
        return;
    v8SetReturnValueFast(info, element.release(), document);
}

} // namespace blink
