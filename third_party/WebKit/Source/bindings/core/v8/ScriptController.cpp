/*
 * Copyright (C) 2008, 2009 Google Inc. All rights reserved.
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2014 Opera Software ASA. All rights reserved.
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
#include "bindings/core/v8/ScriptController.h"

#include "bindings/core/v8/BindingSecurity.h"
#include "bindings/core/v8/NPV8Object.h"
#include "bindings/core/v8/ScriptCallStackFactory.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8Event.h"
#include "bindings/core/v8/V8GCController.h"
#include "bindings/core/v8/V8HTMLElement.h"
#include "bindings/core/v8/V8NPObject.h"
#include "bindings/core/v8/V8PerContextData.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "bindings/core/v8/V8Window.h"
#include "bindings/core/v8/WindowProxy.h"
#include "bindings/core/v8/npruntime_impl.h"
#include "bindings/core/v8/npruntime_priv.h"
#include "core/dom/Document.h"
#include "core/dom/Node.h"
#include "core/dom/ScriptableDocumentParser.h"
#include "core/events/Event.h"
#include "core/events/EventListener.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/ProgressTracker.h"
#include "core/plugins/PluginView.h"
#include "platform/NotImplemented.h"
#include "platform/TraceEvent.h"
#include "platform/UserGestureIndicator.h"
#include "platform/Widget.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "wtf/CurrentTime.h"
#include "wtf/StdLibExtras.h"
#include "wtf/StringExtras.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/TextPosition.h"

namespace blink {

bool ScriptController::canAccessFromCurrentOrigin(LocalFrame *frame)
{
    if (!frame)
        return false;
    v8::Isolate* isolate = toIsolate(frame);
    return !isolate->InContext() || BindingSecurity::shouldAllowAccessToFrame(isolate, frame);
}

ScriptController::ScriptController(LocalFrame* frame)
    : m_windowProxyManager(WindowProxyManager::create(*frame))
    , m_sourceURL(0)
    , m_windowScriptNPObject(0)
{
}

ScriptController::~ScriptController()
{
}

DEFINE_TRACE(ScriptController)
{
#if ENABLE(OILPAN)
    visitor->trace(m_windowProxyManager);
#endif
}

void ScriptController::clearScriptObjects()
{
    PluginObjectMap::iterator it = m_pluginObjects.begin();
    for (; it != m_pluginObjects.end(); ++it) {
        _NPN_UnregisterObject(it->value);
        _NPN_ReleaseObject(it->value);
    }

    m_pluginObjects.clear();

    if (m_windowScriptNPObject) {
        // Dispose of the underlying V8 object before releasing our reference
        // to it, so that if a plugin fails to release it properly we will
        // only leak the NPObject wrapper, not the object, its document, or
        // anything else they reference.
        disposeUnderlyingV8Object(isolate(), m_windowScriptNPObject);
        _NPN_ReleaseObject(m_windowScriptNPObject);
        m_windowScriptNPObject = 0;
    }
}

void ScriptController::clearForClose()
{
    double start = currentTime();
    m_windowProxyManager->clearForClose();
    Platform::current()->histogramCustomCounts("WebCore.ScriptController.clearForClose", (currentTime() - start) * 1000, 0, 10000, 50);
}

void ScriptController::updateSecurityOrigin(SecurityOrigin* origin)
{
    m_windowProxyManager->mainWorldProxy()->updateSecurityOrigin(origin);
}

v8::MaybeLocal<v8::Value> ScriptController::callFunction(v8::Local<v8::Function> function, v8::Local<v8::Value> receiver, int argc, v8::Local<v8::Value> info[])
{
    // Keep LocalFrame (and therefore ScriptController) alive.
    RefPtrWillBeRawPtr<LocalFrame> protect(frame());
    return ScriptController::callFunction(frame()->document(), function, receiver, argc, info, isolate());
}

v8::MaybeLocal<v8::Value> ScriptController::callFunction(ExecutionContext* context, v8::Local<v8::Function> function, v8::Local<v8::Value> receiver, int argc, v8::Local<v8::Value> info[], v8::Isolate* isolate)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::willCallFunction(context, DevToolsFunctionInfo(function));
    v8::MaybeLocal<v8::Value> result = V8ScriptRunner::callFunction(function, context, receiver, argc, info, isolate);
    InspectorInstrumentation::didCallFunction(cookie);
    return result;
}

v8::Local<v8::Value> ScriptController::executeScriptAndReturnValue(v8::Local<v8::Context> context, const ScriptSourceCode& source, AccessControlStatus accessControlStatus, double* compilationFinishTime)
{
    TRACE_EVENT1("devtools.timeline", "EvaluateScript", "data", InspectorEvaluateScriptEvent::data(frame(), source.url().string(), source.startLine()));
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::willEvaluateScript(frame(), source.url().string(), source.startLine());

    v8::Local<v8::Value> result;
    {
        V8CacheOptions v8CacheOptions(V8CacheOptionsDefault);
        if (frame()->settings())
            v8CacheOptions = frame()->settings()->v8CacheOptions();

        // Isolate exceptions that occur when compiling and executing
        // the code. These exceptions should not interfere with
        // javascript code we might evaluate from C++ when returning
        // from here.
        v8::TryCatch tryCatch;
        tryCatch.SetVerbose(true);

        v8::Local<v8::Script> script;
        if (!v8Call(V8ScriptRunner::compileScript(source, isolate(), accessControlStatus, v8CacheOptions), script, tryCatch))
            return result;

        if (compilationFinishTime) {
            *compilationFinishTime = WTF::monotonicallyIncreasingTime();
        }
        // Keep LocalFrame (and therefore ScriptController) alive.
        RefPtrWillBeRawPtr<LocalFrame> protect(frame());
        if (!v8Call(V8ScriptRunner::runCompiledScript(isolate(), script, frame()->document()), result, tryCatch))
            return result;
    }

    InspectorInstrumentation::didEvaluateScript(cookie);
    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "UpdateCounters", TRACE_EVENT_SCOPE_THREAD, "data", InspectorUpdateCountersEvent::data());

    return result;
}

bool ScriptController::initializeMainWorld()
{
    if (m_windowProxyManager->mainWorldProxy()->isContextInitialized())
        return false;
    return windowProxy(DOMWrapperWorld::mainWorld())->isContextInitialized();
}

WindowProxy* ScriptController::existingWindowProxy(DOMWrapperWorld& world)
{
    return m_windowProxyManager->existingWindowProxy(world);
}

WindowProxy* ScriptController::windowProxy(DOMWrapperWorld& world)
{
    WindowProxy* windowProxy = m_windowProxyManager->windowProxy(world);
    if (!windowProxy->isContextInitialized() && windowProxy->initializeIfNeeded() && world.isMainWorld())
        frame()->loader().dispatchDidClearWindowObjectInMainWorld();
    // FIXME: There are some situations where we can return an uninitialized
    // context. This is broken.
    return windowProxy;
}

bool ScriptController::shouldBypassMainWorldCSP()
{
    v8::HandleScope handleScope(isolate());
    v8::Local<v8::Context> context = isolate()->GetCurrentContext();
    if (context.IsEmpty() || !toDOMWindow(context))
        return false;
    DOMWrapperWorld& world = DOMWrapperWorld::current(isolate());
    return world.isIsolatedWorld() ? world.isolatedWorldHasContentSecurityPolicy() : false;
}

TextPosition ScriptController::eventHandlerPosition() const
{
    ScriptableDocumentParser* parser = frame()->document()->scriptableDocumentParser();
    if (parser)
        return parser->textPosition();
    return TextPosition::minimumPosition();
}

// Create a V8 object with an interceptor of NPObjectPropertyGetter.
bool ScriptController::bindToWindowObject(LocalFrame* frame, const String& key, NPObject* object)
{
    ScriptState* scriptState = ScriptState::forMainWorld(frame);
    if (!scriptState->contextIsValid())
        return false;

    ScriptState::Scope scope(scriptState);
    v8::Local<v8::Object> value = createV8ObjectForNPObject(isolate(), object, 0);

    // Attach to the global object.
    return v8CallBoolean(scriptState->context()->Global()->Set(scriptState->context(), v8String(isolate(), key), value));
}

void ScriptController::enableEval()
{
    if (!m_windowProxyManager->mainWorldProxy()->isContextInitialized())
        return;
    v8::HandleScope handleScope(isolate());
    m_windowProxyManager->mainWorldProxy()->context()->AllowCodeGenerationFromStrings(true);
}

void ScriptController::disableEval(const String& errorMessage)
{
    if (!m_windowProxyManager->mainWorldProxy()->isContextInitialized())
        return;
    v8::HandleScope handleScope(isolate());
    v8::Local<v8::Context> v8Context = m_windowProxyManager->mainWorldProxy()->context();
    v8Context->AllowCodeGenerationFromStrings(false);
    v8Context->SetErrorMessageForCodeGenerationFromStrings(v8String(isolate(), errorMessage));
}

PassRefPtr<SharedPersistent<v8::Object>> ScriptController::createPluginWrapper(Widget* widget)
{
    ASSERT(widget);

    if (!widget->isPluginView())
        return nullptr;

    v8::HandleScope handleScope(isolate());
    v8::Local<v8::Object> scriptableObject = toPluginView(widget)->scriptableObject(isolate());

    if (scriptableObject.IsEmpty())
        return nullptr;

    // LocalFrame Memory Management for NPObjects
    // -------------------------------------
    // NPObjects are treated differently than other objects wrapped by JS.
    // NPObjects can be created either by the browser (e.g. the main
    // window object) or by the plugin (the main plugin object
    // for a HTMLEmbedElement). Further, unlike most DOM Objects, the frame
    // is especially careful to ensure NPObjects terminate at frame teardown because
    // if a plugin leaks a reference, it could leak its objects (or the browser's objects).
    //
    // The LocalFrame maintains a list of plugin objects (m_pluginObjects)
    // which it can use to quickly find the wrapped embed object.
    //
    // Inside the NPRuntime, we've added a few methods for registering
    // wrapped NPObjects. The purpose of the registration is because
    // javascript garbage collection is non-deterministic, yet we need to
    // be able to tear down the plugin objects immediately. When an object
    // is registered, javascript can use it. When the object is destroyed,
    // or when the object's "owning" object is destroyed, the object will
    // be un-registered, and the javascript engine must not use it.
    //
    // Inside the javascript engine, the engine can keep a reference to the
    // NPObject as part of its wrapper. However, before accessing the object
    // it must consult the _NPN_Registry.
    if (isWrappedNPObject(scriptableObject)) {
        // Track the plugin object. We've been given a reference to the object.
        m_pluginObjects.set(widget, v8ObjectToNPObject(scriptableObject));
    }

    return SharedPersistent<v8::Object>::create(scriptableObject, isolate());
}

void ScriptController::cleanupScriptObjectsForPlugin(Widget* nativeHandle)
{
    PluginObjectMap::iterator it = m_pluginObjects.find(nativeHandle);
    if (it == m_pluginObjects.end())
        return;

    _NPN_UnregisterObject(it->value);
    _NPN_ReleaseObject(it->value);

    m_pluginObjects.remove(it);
}

V8Extensions& ScriptController::registeredExtensions()
{
    DEFINE_STATIC_LOCAL(V8Extensions, extensions, ());
    return extensions;
}

void ScriptController::registerExtensionIfNeeded(v8::Extension* extension)
{
    const V8Extensions& extensions = registeredExtensions();
    for (size_t i = 0; i < extensions.size(); ++i) {
        if (extensions[i] == extension)
            return;
    }
    v8::RegisterExtension(extension);
    registeredExtensions().append(extension);
}

static NPObject* createNoScriptObject()
{
    notImplemented();
    return 0;
}

static NPObject* createScriptObject(LocalFrame* frame, v8::Isolate* isolate)
{
    ScriptState* scriptState = ScriptState::forMainWorld(frame);
    if (!scriptState->contextIsValid())
        return createNoScriptObject();

    ScriptState::Scope scope(scriptState);
    LocalDOMWindow* window = frame->localDOMWindow();
    v8::Local<v8::Value> global = toV8(window, scriptState->context()->Global(), scriptState->isolate());
    if (global.IsEmpty())
        return createNoScriptObject();
    ASSERT(global->IsObject());

    return npCreateV8ScriptObject(isolate, 0, v8::Local<v8::Object>::Cast(global), window);
}

NPObject* ScriptController::windowScriptNPObject()
{
    if (m_windowScriptNPObject)
        return m_windowScriptNPObject;

    if (canExecuteScripts(NotAboutToExecuteScript)) {
        // JavaScript is enabled, so there is a JavaScript window object.
        // Return an NPObject bound to the window object.
		m_windowScriptNPObject = createScriptObject(frame(), isolate());
        _NPN_RegisterObject(m_windowScriptNPObject, 0);
    } else {
        // JavaScript is not enabled, so we cannot bind the NPObject to the
        // JavaScript window object. Instead, we create an NPObject of a
        // different class, one which is not bound to a JavaScript object.
        m_windowScriptNPObject = createNoScriptObject();
    }
    return m_windowScriptNPObject;
}

NPObject* ScriptController::createScriptObjectForPluginElement(HTMLPlugInElement* plugin)
{
    // Can't create NPObjects when JavaScript is disabled.
    if (!canExecuteScripts(NotAboutToExecuteScript))
        return createNoScriptObject();

    ScriptState* scriptState = ScriptState::forMainWorld(frame());
    if (!scriptState->contextIsValid())
        return createNoScriptObject();

    ScriptState::Scope scope(scriptState);
    LocalDOMWindow* window = frame()->localDOMWindow();
    v8::Local<v8::Value> v8plugin = toV8(plugin, scriptState->context()->Global(), scriptState->isolate());
    if (v8plugin.IsEmpty() || !v8plugin->IsObject())
        return createNoScriptObject();
    return npCreateV8ScriptObject(scriptState->isolate(), 0, v8::Local<v8::Object>::Cast(v8plugin), window);
}

void ScriptController::clearWindowProxy()
{
    // V8 binding expects ScriptController::clearWindowProxy only be called
    // when a frame is loading a new page. This creates a new context for the new page.

    double start = currentTime();
    // The V8 context must be available for |clearScriptObjects()|.
    // The below call must be before |clearForNavigation()| which disposes the V8 context.
    clearScriptObjects();

    m_windowProxyManager->clearForNavigation();
    Platform::current()->histogramCustomCounts("WebCore.ScriptController.clearWindowProxy", (currentTime() - start) * 1000, 0, 10000, 50);
}

void ScriptController::setCaptureCallStackForUncaughtExceptions(bool value)
{
    v8::V8::SetCaptureStackTraceForUncaughtExceptions(value, ScriptCallStack::maxCallStackSizeToCapture, stackTraceOptions);
}

void ScriptController::collectIsolatedContexts(Vector<std::pair<ScriptState*, SecurityOrigin*>>& result)
{
    m_windowProxyManager->collectIsolatedContexts(result);
}

void ScriptController::updateDocument()
{
    // For an uninitialized main window windowProxy, do not incur the cost of context initialization.
    if (!m_windowProxyManager->mainWorldProxy()->isGlobalInitialized())
        return;

    if (!initializeMainWorld())
        windowProxy(DOMWrapperWorld::mainWorld())->updateDocument();
}

void ScriptController::namedItemAdded(HTMLDocument* doc, const AtomicString& name)
{
    windowProxy(DOMWrapperWorld::mainWorld())->namedItemAdded(doc, name);
}

void ScriptController::namedItemRemoved(HTMLDocument* doc, const AtomicString& name)
{
    windowProxy(DOMWrapperWorld::mainWorld())->namedItemRemoved(doc, name);
}

static bool isInPrivateScriptIsolateWorld(v8::Isolate* isolate)
{
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    return !context.IsEmpty() && toDOMWindow(context) && DOMWrapperWorld::current(isolate).isPrivateScriptIsolatedWorld();
}

bool ScriptController::canExecuteScripts(ReasonForCallingCanExecuteScripts reason)
{
    // For performance reasons, we check isInPrivateScriptIsolateWorld() only if
    // canExecuteScripts is going to return false.
    if (!blink::RuntimeEnabledFeatures::cspCheckEnabled())
        return true;

    if (frame()->document() && frame()->document()->isSandboxed(SandboxScripts)) {
        if (isInPrivateScriptIsolateWorld(isolate()))
            return true;
        // FIXME: This message should be moved off the console once a solution to https://bugs.webkit.org/show_bug.cgi?id=103274 exists.
        if (reason == AboutToExecuteScript)
            frame()->document()->addConsoleMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, "Blocked script execution in '" + frame()->document()->url().elidedString() + "' because the document's frame is sandboxed and the 'allow-scripts' permission is not set."));
        return false;
    }

    if (frame()->document() && frame()->document()->isViewSource()) {
        ASSERT(frame()->document()->securityOrigin()->isUnique());
        return true;
    }

    FrameLoaderClient* client = frame()->loader().client();
    if (!client)
        return false;
    Settings* settings = frame()->settings();
    const bool allowed = client->allowScript(settings && settings->scriptEnabled())
        || isInPrivateScriptIsolateWorld(isolate());
    if (!allowed && reason == AboutToExecuteScript)
        client->didNotAllowScript();
    return allowed;
}

bool ScriptController::executeScriptIfJavaScriptURL(const KURL& url)
{
    if (!protocolIsJavaScript(url))
        return false;

    bool shouldBypassMainWorldContentSecurityPolicy = ContentSecurityPolicy::shouldBypassMainWorld(frame()->document());
    if (!frame()->page()
        || (!shouldBypassMainWorldContentSecurityPolicy && !frame()->document()->contentSecurityPolicy()->allowJavaScriptURLs(frame()->document()->url(), eventHandlerPosition().m_line)))
        return true;

    if (frame()->loader().stateMachine()->isDisplayingInitialEmptyDocument())
        frame()->loader().progress().progressStarted();

    // We need to hold onto the LocalFrame here because executing script can
    // destroy the frame.
    RefPtrWillBeRawPtr<LocalFrame> protect(frame());
    RefPtrWillBeRawPtr<Document> ownerDocument(frame()->document());

    const int javascriptSchemeLength = sizeof("javascript:") - 1;

    bool locationChangeBefore = frame()->navigationScheduler().locationChangePending();

    String decodedURL = decodeURLEscapeSequences(url.string());
    v8::HandleScope handleScope(isolate());
    v8::Local<v8::Value> result = evaluateScriptInMainWorld(ScriptSourceCode(decodedURL.substring(javascriptSchemeLength)), NotSharableCrossOrigin, DoNotExecuteScriptWhenScriptsDisabled);

    // If executing script caused this frame to be removed from the page, we
    // don't want to try to replace its document!
    if (!frame()->page())
        return true;

    if (result.IsEmpty() || !result->IsString())
        return true;
    String scriptResult = toCoreString(v8::Local<v8::String>::Cast(result));

    // We're still in a frame, so there should be a DocumentLoader.
    ASSERT(frame()->document()->loader());
    if (!locationChangeBefore && frame()->navigationScheduler().locationChangePending())
        return true;

    frame()->loader().replaceDocumentWhileExecutingJavaScriptURL(scriptResult, ownerDocument.get());
    return true;
}

void ScriptController::executeScriptInMainWorld(const String& script, ExecuteScriptPolicy policy)
{
    v8::HandleScope handleScope(isolate());
    evaluateScriptInMainWorld(ScriptSourceCode(script), NotSharableCrossOrigin, policy);
}

void ScriptController::executeScriptInMainWorld(const ScriptSourceCode& sourceCode, AccessControlStatus accessControlStatus, double* compilationFinishTime)
{
    v8::HandleScope handleScope(isolate());
    evaluateScriptInMainWorld(sourceCode, accessControlStatus, DoNotExecuteScriptWhenScriptsDisabled, compilationFinishTime);
}

v8::Local<v8::Value> ScriptController::executeScriptInMainWorldAndReturnValue(const ScriptSourceCode& sourceCode)
{
    return evaluateScriptInMainWorld(sourceCode, NotSharableCrossOrigin, DoNotExecuteScriptWhenScriptsDisabled);
}

v8::Local<v8::Value> ScriptController::evaluateScriptInMainWorld(const ScriptSourceCode& sourceCode, AccessControlStatus accessControlStatus, ExecuteScriptPolicy policy, double* compilationFinishTime)
{
    if (policy == DoNotExecuteScriptWhenScriptsDisabled && !canExecuteScripts(AboutToExecuteScript))
        return v8::Local<v8::Value>();

    String sourceURL = sourceCode.url();
    const String* savedSourceURL = m_sourceURL;
    m_sourceURL = &sourceURL;

    ScriptState* scriptState = ScriptState::forMainWorld(frame());
    if (!scriptState->contextIsValid())
        return v8::Local<v8::Value>();
    v8::EscapableHandleScope handleScope(isolate());
    ScriptState::Scope scope(scriptState);

    RefPtrWillBeRawPtr<LocalFrame> protect(frame());
    if (frame()->loader().stateMachine()->isDisplayingInitialEmptyDocument())
        frame()->loader().didAccessInitialDocument();

    v8::Local<v8::Value> object = executeScriptAndReturnValue(scriptState->context(), sourceCode, accessControlStatus, compilationFinishTime);
    m_sourceURL = savedSourceURL;

    if (object.IsEmpty())
        return v8::Local<v8::Value>();

    return handleScope.Escape(object);
}

void ScriptController::executeScriptInIsolatedWorld(int worldID, const WillBeHeapVector<ScriptSourceCode>& sources, int extensionGroup, Vector<v8::Local<v8::Value>>* results)
{
    ASSERT(worldID > 0);

    RefPtr<DOMWrapperWorld> world = DOMWrapperWorld::ensureIsolatedWorld(isolate(), worldID, extensionGroup);
    WindowProxy* isolatedWorldWindowProxy = windowProxy(*world);
    if (!isolatedWorldWindowProxy->isContextInitialized())
        return;

    ScriptState* scriptState = isolatedWorldWindowProxy->scriptState();
    v8::Context::Scope scope(scriptState->context());
    v8::Local<v8::Array> resultArray = v8::Array::New(isolate(), sources.size());

    for (size_t i = 0; i < sources.size(); ++i) {
        v8::Local<v8::Value> evaluationResult = executeScriptAndReturnValue(scriptState->context(), sources[i]);
        if (evaluationResult.IsEmpty())
            evaluationResult = v8::Local<v8::Value>::New(isolate(), v8::Undefined(isolate()));
        if (!v8CallBoolean(resultArray->Set(scriptState->context(), v8::Integer::New(scriptState->isolate(), i), evaluationResult)))
            return;
    }

    if (results) {
        for (size_t i = 0; i < resultArray->Length(); ++i) {
            v8::Local<v8::Value> value;
            if (!resultArray->Get(scriptState->context(), i).ToLocal(&value))
                return;
            results->append(value);
        }
    }
}

} // namespace blink
