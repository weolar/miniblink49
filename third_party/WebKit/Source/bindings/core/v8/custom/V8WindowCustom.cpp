/*
 * Copyright (C) 2009, 2011 Google Inc. All rights reserved.
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
#include "bindings/core/v8/V8Window.h"

#include "bindings/core/v8/BindingSecurity.h"
#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8EventListener.h"
#include "bindings/core/v8/V8EventListenerList.h"
#include "bindings/core/v8/V8HTMLCollection.h"
#include "bindings/core/v8/V8HiddenValue.h"
#include "bindings/core/v8/V8Node.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/MessagePort.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLDocument.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "platform/LayoutTestSupport.h"
#include "wtf/Assertions.h"
#include "wtf/OwnPtr.h"

namespace blink {

void V8Window::eventAttributeGetterCustom(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    LocalFrame* frame = toLocalDOMWindow(V8Window::toImpl(info.Holder()))->frame();
    ExceptionState exceptionState(ExceptionState::GetterContext, "event", "Window", info.Holder(), info.GetIsolate());
    if (!BindingSecurity::shouldAllowAccessToFrame(info.GetIsolate(), frame, exceptionState)) {
        exceptionState.throwIfNeeded();
        return;
    }

    ASSERT(frame);
    // This is a fast path to retrieve info.Holder()->CreationContext().
    v8::Local<v8::Context> context = toV8Context(frame, DOMWrapperWorld::current(info.GetIsolate()));
    if (context.IsEmpty())
        return;

    v8::Local<v8::Value> jsEvent = V8HiddenValue::getHiddenValue(info.GetIsolate(), context->Global(), V8HiddenValue::event(info.GetIsolate()));
    if (jsEvent.IsEmpty())
        return;
    v8SetReturnValue(info, jsEvent);
}

void V8Window::eventAttributeSetterCustom(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    LocalFrame* frame = toLocalDOMWindow(V8Window::toImpl(info.Holder()))->frame();
    ExceptionState exceptionState(ExceptionState::SetterContext, "event", "Window", info.Holder(), info.GetIsolate());
    if (!BindingSecurity::shouldAllowAccessToFrame(info.GetIsolate(), frame, exceptionState)) {
        exceptionState.throwIfNeeded();
        return;
    }

    ASSERT(frame);
    // This is a fast path to retrieve info.Holder()->CreationContext().
    v8::Local<v8::Context> context = toV8Context(frame, DOMWrapperWorld::current(info.GetIsolate()));
    if (context.IsEmpty())
        return;

    V8HiddenValue::setHiddenValue(info.GetIsolate(), context->Global(), V8HiddenValue::event(info.GetIsolate()), value);
}

void V8Window::frameElementAttributeGetterCustom(const v8::PropertyCallbackInfo<v8::Value>& info)
{
    LocalDOMWindow* impl = toLocalDOMWindow(V8Window::toImpl(info.Holder()));
    ExceptionState exceptionState(ExceptionState::GetterContext, "frame", "Window", info.Holder(), info.GetIsolate());

    // Do the security check against the parent frame rather than
    // frameElement() itself, so that a remote parent frame can be handled
    // properly. In that case, there's no frameElement(), yet we should still
    // throw a proper exception and deny access.
    Frame* target = impl->frame() ? impl->frame()->tree().parent() : nullptr;
    if (!BindingSecurity::shouldAllowAccessToFrame(info.GetIsolate(), target, exceptionState)) {
        v8SetReturnValueNull(info);
        exceptionState.throwIfNeeded();
        return;
    }

    // The wrapper for an <iframe> should get its prototype from the context of the frame it's in, rather than its own frame.
    // So, use its containing document as the creation context when wrapping.
    v8::Local<v8::Value> creationContext = toV8(&impl->frameElement()->document(), info.Holder(), info.GetIsolate());
    RELEASE_ASSERT(!creationContext.IsEmpty());
    v8::Local<v8::Value> wrapper = toV8(impl->frameElement(), v8::Local<v8::Object>::Cast(creationContext), info.GetIsolate());
    v8SetReturnValue(info, wrapper);
}

void V8Window::openerAttributeSetterCustom(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    DOMWindow* impl = V8Window::toImpl(info.Holder());
    ExceptionState exceptionState(ExceptionState::SetterContext, "opener", "Window", info.Holder(), isolate);
    if (!BindingSecurity::shouldAllowAccessToFrame(info.GetIsolate(), impl->frame(), exceptionState)) {
        exceptionState.throwIfNeeded();
        return;
    }

    // Opener can be shadowed if it is in the same domain.
    // Have a special handling of null value to behave
    // like Firefox. See bug http://b/1224887 & http://b/791706.
    if (value->IsNull()) {
        // impl->frame() has to be a non-null LocalFrame.  Otherwise, the
        // same-origin check would have failed.
        ASSERT(impl->frame());
        toLocalFrame(impl->frame())->loader().setOpener(0);
    }

    // Delete the accessor from this object.
    info.Holder()->Delete(isolate->GetCurrentContext(), v8AtomicString(isolate, "opener"));

    // Put property on the front (this) object.
    if (info.This()->IsObject()) {
        v8::Maybe<bool> unused = v8::Local<v8::Object>::Cast(info.This())->Set(isolate->GetCurrentContext(), v8AtomicString(isolate, "opener"), value);
        ALLOW_UNUSED_LOCAL(unused);
    }
}

static bool isLegacyTargetOriginDesignation(v8::Local<v8::Value> value)
{
    if (value->IsString() || value->IsStringObject())
        return true;
    return false;
}


void V8Window::postMessageMethodCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    ExceptionState exceptionState(ExceptionState::ExecutionContext, "postMessage", "Window", info.Holder(), info.GetIsolate());
    if (UNLIKELY(info.Length() < 2)) {
        setMinimumArityTypeError(exceptionState, 2, info.Length());
        exceptionState.throwIfNeeded();
        return;
    }

    // None of these need to be RefPtr because info and context are guaranteed
    // to hold on to them.
    DOMWindow* window = V8Window::toImpl(info.Holder());
    LocalDOMWindow* source = callingDOMWindow(info.GetIsolate());

    ASSERT(window);
    UseCounter::countIfNotPrivateScript(info.GetIsolate(), window->frame(), UseCounter::WindowPostMessage);

    // If called directly by WebCore we don't have a calling context.
    if (!source) {
        exceptionState.throwTypeError("No active calling context exists.");
        exceptionState.throwIfNeeded();
        return;
    }

    // This function has variable arguments and can be:
    // Per current spec:
    //   postMessage(message, targetOrigin)
    //   postMessage(message, targetOrigin, {sequence of transferrables})
    // Legacy non-standard implementations in webkit allowed:
    //   postMessage(message, {sequence of transferrables}, targetOrigin);
    OwnPtrWillBeRawPtr<MessagePortArray> portArray = adoptPtrWillBeNoop(new MessagePortArray);
    ArrayBufferArray arrayBufferArray;
    int targetOriginArgIndex = 1;
    if (info.Length() > 2) {
        int transferablesArgIndex = 2;
        if (isLegacyTargetOriginDesignation(info[2])) {
            UseCounter::countIfNotPrivateScript(info.GetIsolate(), window->frame(), UseCounter::WindowPostMessageWithLegacyTargetOriginArgument);
            targetOriginArgIndex = 2;
            transferablesArgIndex = 1;
        }
        if (!SerializedScriptValue::extractTransferables(info.GetIsolate(), info[transferablesArgIndex], transferablesArgIndex, *portArray, arrayBufferArray, exceptionState)) {
            exceptionState.throwIfNeeded();
            return;
        }
    }
    TOSTRING_VOID(V8StringResource<TreatNullAndUndefinedAsNullString>, targetOrigin, info[targetOriginArgIndex]);

    RefPtr<SerializedScriptValue> message = SerializedScriptValueFactory::instance().create(info.GetIsolate(), info[0], portArray.get(), &arrayBufferArray, exceptionState);
    if (exceptionState.throwIfNeeded())
        return;

    window->postMessage(message.release(), portArray.get(), targetOrigin, source, exceptionState);
    exceptionState.throwIfNeeded();
}

// FIXME(fqian): returning string is cheating, and we should
// fix this by calling toString function on the receiver.
// However, V8 implements toString in JavaScript, which requires
// switching context of receiver. I consider it is dangerous.
void V8Window::toStringMethodCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> domWrapper = V8Window::findInstanceInPrototypeChain(info.This(), info.GetIsolate());
    v8::Local<v8::Object> target = domWrapper.IsEmpty() ? info.This() : domWrapper;
    v8::Local<v8::String> value;
    if (target->ObjectProtoToString(info.GetIsolate()->GetCurrentContext()).ToLocal(&value))
        v8SetReturnValue(info, value);
}

void V8Window::openMethodCustom(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    DOMWindow* impl = V8Window::toImpl(info.Holder());
    ExceptionState exceptionState(ExceptionState::ExecutionContext, "open", "Window", info.Holder(), info.GetIsolate());
    if (!BindingSecurity::shouldAllowAccessToFrame(info.GetIsolate(), impl->frame(), exceptionState)) {
        exceptionState.throwIfNeeded();
        return;
    }

    TOSTRING_VOID(V8StringResource<TreatNullAndUndefinedAsNullString>, urlString, info[0]);
    AtomicString frameName;
    if (info[1]->IsUndefined() || info[1]->IsNull()) {
        frameName = "_blank";
    } else {
        TOSTRING_VOID(V8StringResource<>, frameNameResource, info[1]);
        frameName = frameNameResource;
    }
    TOSTRING_VOID(V8StringResource<TreatNullAndUndefinedAsNullString>, windowFeaturesString, info[2]);

    // |impl| has to be a LocalDOMWindow, since RemoteDOMWindows wouldn't have
    // passed the BindingSecurity check above.
    RefPtrWillBeRawPtr<DOMWindow> openedWindow = toLocalDOMWindow(impl)->open(urlString, frameName, windowFeaturesString, callingDOMWindow(info.GetIsolate()), enteredDOMWindow(info.GetIsolate()));
    if (!openedWindow)
        return;

    v8SetReturnValueFast(info, openedWindow.release(), impl);
}

// We lazy create interfaces like testRunner and internals on first access
// inside layout tests since creating the bindings is expensive. Then we store
// them in a hidden Map on the window so that later accesses will reuse the same
// wrapper.
static bool installTestInterfaceIfNeeded(LocalFrame& frame, v8::Local<v8::String> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    if (!LayoutTestSupport::isRunningLayoutTest())
        return false;

    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    AtomicString propName = toCoreAtomicString(name);

    v8::Local<v8::Value> interfaces = V8HiddenValue::getHiddenValue(isolate, info.Holder(), V8HiddenValue::testInterfaces(isolate));
    if (interfaces.IsEmpty()) {
        interfaces = v8::Map::New(isolate);
        V8HiddenValue::setHiddenValue(isolate, info.Holder(), V8HiddenValue::testInterfaces(isolate), interfaces);
    }

    v8::Local<v8::Map> interfacesMap = interfaces.As<v8::Map>();
    v8::Local<v8::Value> result = v8CallOrCrash(interfacesMap->Get(context, name));
    if (!result->IsUndefined()) {
        v8SetReturnValue(info, result);
        return true;
    }

    v8::Local<v8::Value> interfaceName = frame.loader().client()->createTestInterface(propName);
    if (!interfaceName.IsEmpty()) {
        v8CallOrCrash(interfacesMap->Set(context, name, interfaceName));
        v8SetReturnValue(info, interfaceName);
        return true;
    }

    return false;
}

void V8Window::namedPropertyGetterCustom(v8::Local<v8::Name> name, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::String> nameString;
    v8::Local<v8::Symbol> s;
    v8::Local<v8::Value> nameV;
    int length = 0;

    if (name->IsSymbol()) {
        s = name.As<v8::Symbol>();
        nameV = s->Name();
        if (nameV->IsString()) {
            nameString = nameV.As<v8::String>();
            length = nameString->Length();
        } else
            return;
    } else if (name->IsString()) {
        nameString = name.As<v8::String>();
    } else
        return;

    DOMWindow* window = V8Window::toImpl(info.Holder());
    if (!window)
        return;

    Frame* frame = window->frame();
    // window is detached from a frame.
    if (!frame)
        return;

    AtomicString propName = toCoreAtomicString(nameString);
    if (propName == "Symbol.toPrimitive") {
        return;
    }

    // Note that the spec doesn't allow any cross-origin named access to the window object. However,
    // UAs have traditionally allowed named access to named child browsing contexts, even across
    // origins. So first, search child frames for a frame with a matching name.
    Frame* child = frame->tree().scopedChild(propName);
    if (child) {
        v8SetReturnValueFast(info, child->domWindow(), window);
        return;
    }

    // Search IDL functions defined in the prototype
    if (!info.Holder()->GetRealNamedProperty(info.GetIsolate()->GetCurrentContext(), nameString).IsEmpty())
        return;

    // Frame could have been detached in call to GetRealNamedProperty.
    frame = window->frame();
    // window is detached.
    if (!frame)
        return;

    // If the frame is remote, the caller will never be able to access further named results.
    if (!frame->isLocalFrame())
        return;

    if (installTestInterfaceIfNeeded(toLocalFrame(*frame), nameString, info))
        return;

    // Search named items in the document.
    Document* doc = toLocalFrame(frame)->document();
    if (!doc || !doc->isHTMLDocument())
        return;

    // This is an AllCanRead interceptor.  Check that the caller has access to the named results.
    if (!BindingSecurity::shouldAllowAccessToFrame(info.GetIsolate(), frame, DoNotReportSecurityError))
        return;

    bool hasNamedItem = toHTMLDocument(doc)->hasNamedItem(propName);
    bool hasIdItem = doc->hasElementWithId(propName);

    if (!hasNamedItem && !hasIdItem)
        return;

    if (!hasNamedItem && hasIdItem && !doc->containsMultipleElementsWithId(propName)) {
        v8SetReturnValueFast(info, doc->getElementById(propName), window);
        return;
    }

    RefPtrWillBeRawPtr<HTMLCollection> items = doc->windowNamedItems(propName);
    if (!items->isEmpty()) {
        // TODO(esprehn): Firefox doesn't return an HTMLCollection here if there's
        // multiple with the same name, but Chrome and Safari does. What's the
        // right behavior?
        if (items->hasExactlyOneItem()) {
            v8SetReturnValueFast(info, items->item(0), window);
            return;
        }
        v8SetReturnValueFast(info, items.release(), window);
        return;
    }
}

static bool securityCheck(v8::Local<v8::Object> host)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Object> window = V8Window::findInstanceInPrototypeChain(host, isolate);
    if (window.IsEmpty())
        return false; // the frame is gone.

    DOMWindow* targetWindow = V8Window::toImpl(window);
    ASSERT(targetWindow);
    if (!targetWindow->isLocalDOMWindow())
        return false;

    LocalFrame* target = toLocalDOMWindow(targetWindow)->frame();
    if (!target)
        return false;

    // Notify the loader's client if the initial document has been accessed.
    if (target->loader().stateMachine()->isDisplayingInitialEmptyDocument())
        target->loader().didAccessInitialDocument();

    return BindingSecurity::shouldAllowAccessToFrame(isolate, target, DoNotReportSecurityError);
}

bool V8Window::namedSecurityCheckCustom(v8::Local<v8::Object> host, v8::Local<v8::Value> key, v8::AccessType type, v8::Local<v8::Value>)
{
    return securityCheck(host);
}

bool V8Window::indexedSecurityCheckCustom(v8::Local<v8::Object> host, uint32_t index, v8::AccessType type, v8::Local<v8::Value>)
{
    return securityCheck(host);
}

} // namespace blink
