/*
 * Copyright (C) 2008, 2009 Google Inc. All rights reserved.
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

#ifndef ScriptController_h
#define ScriptController_h

#include "bindings/core/v8/SharedPersistent.h"
#include "bindings/core/v8/WindowProxyManager.h"
#include "core/CoreExport.h"
#include "core/fetch/AccessControlStatus.h"
#include "core/fetch/CrossOriginAccessControl.h"
#include "core/frame/LocalFrame.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/Vector.h"
#include "wtf/text/TextPosition.h"
#include <v8.h>

struct NPObject;

namespace blink {

class DOMWrapperWorld;
class ExecutionContext;
class HTMLDocument;
class HTMLPlugInElement;
class KURL;
class ScriptState;
class ScriptSourceCode;
class SecurityOrigin;
class WindowProxy;
class Widget;

typedef WTF::Vector<v8::Extension*> V8Extensions;

enum ReasonForCallingCanExecuteScripts {
    AboutToExecuteScript,
    NotAboutToExecuteScript
};

class CORE_EXPORT ScriptController final : public NoBaseWillBeGarbageCollectedFinalized<ScriptController> {
    WTF_MAKE_NONCOPYABLE(ScriptController);
public:
    enum ExecuteScriptPolicy {
        ExecuteScriptWhenScriptsDisabled,
        DoNotExecuteScriptWhenScriptsDisabled
    };

    static PassOwnPtrWillBeRawPtr<ScriptController> create(LocalFrame* frame)
    {
        return adoptPtrWillBeNoop(new ScriptController(frame));
    }

    ~ScriptController();
    DECLARE_TRACE();

    bool initializeMainWorld();
    WindowProxy* windowProxy(DOMWrapperWorld&);
    WindowProxy* existingWindowProxy(DOMWrapperWorld&);

    // Evaluate JavaScript in the main world.
    void executeScriptInMainWorld(const String&, ExecuteScriptPolicy = DoNotExecuteScriptWhenScriptsDisabled);
    void executeScriptInMainWorld(const ScriptSourceCode&, AccessControlStatus = NotSharableCrossOrigin, double* compilationFinishTime = 0);
    v8::Local<v8::Value> executeScriptInMainWorldAndReturnValue(const ScriptSourceCode&);
    v8::Local<v8::Value> executeScriptAndReturnValue(v8::Local<v8::Context>, const ScriptSourceCode&, AccessControlStatus = NotSharableCrossOrigin, double* compilationFinishTime = 0);

    // Executes JavaScript in an isolated world. The script gets its own global scope,
    // its own prototypes for intrinsic JavaScript objects (String, Array, and so-on),
    // and its own wrappers for all DOM nodes and DOM constructors.
    //
    // If an isolated world with the specified ID already exists, it is reused.
    // Otherwise, a new world is created.
    //
    // FIXME: Get rid of extensionGroup here.
    // FIXME: We don't want to support multiple scripts.
    void executeScriptInIsolatedWorld(int worldID, const WillBeHeapVector<ScriptSourceCode>& sources, int extensionGroup, Vector<v8::Local<v8::Value>>* results);

    // Returns true if argument is a JavaScript URL.
    bool executeScriptIfJavaScriptURL(const KURL&);

    v8::MaybeLocal<v8::Value> callFunction(v8::Local<v8::Function>, v8::Local<v8::Value>, int argc, v8::Local<v8::Value> argv[]);
    static v8::MaybeLocal<v8::Value> callFunction(ExecutionContext*, v8::Local<v8::Function>, v8::Local<v8::Value> receiver, int argc, v8::Local<v8::Value> info[], v8::Isolate*);

    // Returns true if the current world is isolated, and has its own Content
    // Security Policy. In this case, the policy of the main world should be
    // ignored when evaluating resources injected into the DOM.
    bool shouldBypassMainWorldCSP();

    // Creates a property of the global object of a frame.
    bool bindToWindowObject(LocalFrame*, const String& key, NPObject*);

    PassRefPtr<SharedPersistent<v8::Object>> createPluginWrapper(Widget*);

    void enableEval();
    void disableEval(const String& errorMessage);

    static bool canAccessFromCurrentOrigin(LocalFrame*);

    static void setCaptureCallStackForUncaughtExceptions(bool);
    void collectIsolatedContexts(Vector<std::pair<ScriptState*, SecurityOrigin*>>&);

    bool canExecuteScripts(ReasonForCallingCanExecuteScripts);

    TextPosition eventHandlerPosition() const;

    void clearWindowProxy();
    void updateDocument();

    void namedItemAdded(HTMLDocument*, const AtomicString&);
    void namedItemRemoved(HTMLDocument*, const AtomicString&);

    void updateSecurityOrigin(SecurityOrigin*);
    void clearScriptObjects();
    void cleanupScriptObjectsForPlugin(Widget*);

    void clearForClose();

    NPObject* createScriptObjectForPluginElement(HTMLPlugInElement*);
    NPObject* windowScriptNPObject();

    // Registers a v8 extension to be available on webpages. Will only
    // affect v8 contexts initialized after this call. Takes ownership of
    // the v8::Extension object passed.
    static void registerExtensionIfNeeded(v8::Extension*);
    static V8Extensions& registeredExtensions();

    v8::Isolate* isolate() const { return m_windowProxyManager->isolate(); }

    WindowProxyManager* windowProxyManager() const { return m_windowProxyManager.get(); }

private:
    explicit ScriptController(LocalFrame*);

    LocalFrame* frame() const { return toLocalFrame(m_windowProxyManager->frame()); }

    typedef HashMap<Widget*, NPObject*> PluginObjectMap;

    v8::Local<v8::Value> evaluateScriptInMainWorld(const ScriptSourceCode&, AccessControlStatus, ExecuteScriptPolicy, double* compilationFinishTime = 0);

    OwnPtrWillBeMember<WindowProxyManager> m_windowProxyManager;
    const String* m_sourceURL;

    // A mapping between Widgets and their corresponding script object.
    // This list is used so that when the plugin dies, we can immediately
    // invalidate all sub-objects which are associated with that plugin.
    // The frame keeps a NPObject reference for each item on the list.
    PluginObjectMap m_pluginObjects;

    NPObject* m_windowScriptNPObject;
};

} // namespace blink

#endif // ScriptController_h
