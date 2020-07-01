/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "bindings/core/v8/V8Initializer.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/RejectedPromises.h"
#include "bindings/core/v8/ScriptCallStackFactory.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptProfiler.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8DOMException.h"
#include "bindings/core/v8/V8ErrorEvent.h"
#include "bindings/core/v8/V8ErrorHandler.h"
#include "bindings/core/v8/V8GCController.h"
#include "bindings/core/v8/V8History.h"
#include "bindings/core/v8/V8Location.h"
#include "bindings/core/v8/V8PerContextData.h"
#include "bindings/core/v8/V8Window.h"
#include "bindings/core/v8/WorkerScriptController.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/fetch/AccessControlStatus.h"
#include "core/frame/ConsoleTypes.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/inspector/ScriptArguments.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/workers/WorkerGlobalScope.h"
#include "platform/EventDispatchForbiddenScope.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "public/platform/Platform.h"
#include "public/platform/WebScheduler.h"
#include "public/platform/WebThread.h"
#include "wtf/AddressSanitizer.h"
#include "wtf/ArrayBufferContents.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"
#include <v8-debug.h>

void* sk_malloc_throw(size_t size);
void sk_free(void* p);

namespace blink {

static Frame* findFrame(v8::Isolate* isolate, v8::Local<v8::Object> host, v8::Local<v8::Value> data)
{
    const WrapperTypeInfo* type = WrapperTypeInfo::unwrap(data);

    if (V8Window::wrapperTypeInfo.equals(type)) {
        v8::Local<v8::Object> windowWrapper = V8Window::findInstanceInPrototypeChain(host, isolate);
        if (windowWrapper.IsEmpty())
            return 0;
        return V8Window::toImpl(windowWrapper)->frame();
    }

    if (V8History::wrapperTypeInfo.equals(type))
        return V8History::toImpl(host)->frame();

    if (V8Location::wrapperTypeInfo.equals(type))
        return V8Location::toImpl(host)->frame();

    // This function can handle only those types listed above.
    ASSERT_NOT_REACHED();
    return 0;
}

static void reportFatalErrorInMainThread(const char* location, const char* message)
{
    int memoryUsageMB = Platform::current()->actualMemoryUsageMB();
    printf("V8 error: %s (%s).  Current memory usage: %d MB\n", message, location, memoryUsageMB);
    CRASH();
}

static PassRefPtrWillBeRawPtr<ScriptCallStack> extractCallStack(v8::Isolate* isolate, v8::Local<v8::Message> message, int* const scriptId)
{
    v8::Local<v8::StackTrace> stackTrace = message->GetStackTrace();
    RefPtrWillBeRawPtr<ScriptCallStack> callStack = nullptr;
    *scriptId = message->GetScriptOrigin().ScriptID()->Value();
    // Currently stack trace is only collected when inspector is open.
    if (!stackTrace.IsEmpty() && stackTrace->GetFrameCount() > 0) {
        callStack = createScriptCallStack(isolate, stackTrace, ScriptCallStack::maxCallStackSizeToCapture);
        bool success = false;
        int topScriptId = callStack->at(0).scriptId().toInt(&success);
        if (success && topScriptId == *scriptId)
            *scriptId = 0;
    } else {
        Vector<ScriptCallFrame> callFrames;
        callStack = ScriptCallStack::create(callFrames);
    }
    return callStack.release();
}

static String extractResourceName(v8::Local<v8::Message> message, const Document* document)
{
    v8::Local<v8::Value> resourceName = message->GetScriptOrigin().ResourceName();
    bool shouldUseDocumentURL = document && (resourceName.IsEmpty() || !resourceName->IsString());
    return shouldUseDocumentURL ? document->url() : toCoreString(resourceName.As<v8::String>());
}

static String extractMessageForConsole(v8::Isolate* isolate, v8::Local<v8::Value> data)
{
    if (V8DOMWrapper::isWrapper(isolate, data)) {
        v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(data);
        const WrapperTypeInfo* type = toWrapperTypeInfo(obj);
        if (V8DOMException::wrapperTypeInfo.isSubclass(type)) {
            DOMException* exception = V8DOMException::toImpl(obj);
            if (exception && !exception->messageForConsole().isEmpty())
                return exception->toStringForConsole();
        }
    }
    return emptyString();
}

static void messageHandlerInMainThread(v8::Local<v8::Message> message, v8::Local<v8::Value> data)
{
    ASSERT(isMainThread());
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    // If called during context initialization, there will be no entered window.
    LocalDOMWindow* enteredWindow = enteredDOMWindow(isolate);
    if (!enteredWindow || !enteredWindow->isCurrentlyDisplayedInFrame())
        return;
    
    int scriptId = 0;
    RefPtrWillBeRawPtr<ScriptCallStack> callStack = extractCallStack(isolate, message, &scriptId);
    String resourceName = extractResourceName(message, enteredWindow->document());
    AccessControlStatus accessControlStatus = NotSharableCrossOrigin;
    if (message->IsOpaque())
        accessControlStatus = OpaqueResource;
    else if (message->IsSharedCrossOrigin())
        accessControlStatus = SharableCrossOrigin;

    ScriptState* scriptState = ScriptState::current(isolate);
    String errorMessage = toCoreStringWithNullCheck(message->Get());
    int lineNumber = 0;
    int columnNumber = 0;
    if (v8Call(message->GetLineNumber(scriptState->context()), lineNumber)
        && v8Call(message->GetStartColumn(scriptState->context()), columnNumber))
        ++columnNumber;
    RefPtrWillBeRawPtr<ErrorEvent> event = ErrorEvent::create(errorMessage, resourceName, lineNumber, columnNumber, &scriptState->world());

    String messageForConsole = extractMessageForConsole(isolate, data);
    if (!messageForConsole.isEmpty())
        event->setUnsanitizedMessage("Uncaught " + messageForConsole);

    // This method might be called while we're creating a new context. In this case, we
    // avoid storing the exception object, as we can't create a wrapper during context creation.
    // FIXME: Can we even get here during initialization now that we bail out when GetEntered returns an empty handle?
    LocalFrame* frame = enteredWindow->document()->frame();
    if (frame && frame->script().existingWindowProxy(scriptState->world())) {
        V8ErrorHandler::storeExceptionOnErrorEventWrapper(isolate, event.get(), data, scriptState->context()->Global());
    }

    if (scriptState->world().isPrivateScriptIsolatedWorld()) {
        // We allow a private script to dispatch error events even in a EventDispatchForbiddenScope scope.
        // Without having this ability, it's hard to debug the private script because syntax errors
        // in the private script are not reported to console (the private script just crashes silently).
        // Allowing error events in private scripts is safe because error events don't propagate to
        // other isolated worlds (which means that the error events won't fire any event listeners
        // in user's scripts).
        EventDispatchForbiddenScope::AllowUserAgentEvents allowUserAgentEvents;
        enteredWindow->document()->reportException(event.release(), scriptId, callStack, accessControlStatus);
    } else {
        enteredWindow->document()->reportException(event.release(), scriptId, callStack, accessControlStatus);
    }
}

namespace {

static RejectedPromises& rejectedPromisesOnMainThread()
{
    ASSERT(isMainThread());
    DEFINE_STATIC_LOCAL(OwnPtrWillBePersistent<RejectedPromises>, rejectedPromises, (adoptPtrWillBeNoop(new RejectedPromises())));
    return *rejectedPromises;
}

} // namespace

void V8Initializer::reportRejectedPromisesOnMainThread()
{
    rejectedPromisesOnMainThread().processQueue();
}

static void promiseRejectHandlerInMainThread(v8::PromiseRejectMessage data)
{
    ASSERT(isMainThread());
    if (data.GetEvent() == v8::kPromiseHandlerAddedAfterReject) {
        rejectedPromisesOnMainThread().handlerAdded(data);
        return;
    }

    ASSERT(data.GetEvent() == v8::kPromiseRejectWithNoHandler);

    v8::Local<v8::Promise> promise = data.GetPromise();

    v8::Isolate* isolate = promise->GetIsolate();
    // There is no entered window during microtask callbacks from V8,
    // thus we call toDOMWindow() instead of enteredDOMWindow().
    LocalDOMWindow* window = currentDOMWindow(isolate);
    if (!window || !window->isCurrentlyDisplayedInFrame())
        return;

    v8::Local<v8::Value> exception = data.GetValue();
    if (V8DOMWrapper::isWrapper(isolate, exception)) {
        // Try to get the stack & location from a wrapped exception object (e.g. DOMException).
        ASSERT(exception->IsObject());
        v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(exception);
        v8::Local<v8::Value> error = V8HiddenValue::getHiddenValue(isolate, obj, V8HiddenValue::error(isolate));
        if (!error.IsEmpty())
            exception = error;
    }

    int scriptId = 0;
    int lineNumber = 0;
    int columnNumber = 0;
    String resourceName;
    String errorMessage;
    RefPtrWillBeRawPtr<ScriptCallStack> callStack = nullptr;

    v8::Local<v8::Message> message = v8::Exception::CreateMessage(isolate, exception);
    if (!message.IsEmpty()) {
        if (v8Call(message->GetLineNumber(isolate->GetCurrentContext()), lineNumber)
            && v8Call(message->GetStartColumn(isolate->GetCurrentContext()), columnNumber))
            ++columnNumber;
        resourceName = extractResourceName(message, window->document());
        errorMessage = toCoreStringWithNullCheck(message->Get());
        callStack = extractCallStack(isolate, message, &scriptId);
    } else if (!exception.IsEmpty() && exception->IsInt32()) {
        // For Smi's the message would be empty.
        errorMessage = "Uncaught " + String::number(exception.As<v8::Integer>()->Value());
    }

    String messageForConsole = extractMessageForConsole(isolate, data.GetValue());
    if (!messageForConsole.isEmpty())
        errorMessage = "Uncaught " + messageForConsole;

    ScriptState* scriptState = ScriptState::current(isolate);
    rejectedPromisesOnMainThread().rejectedWithNoHandler(scriptState, data, errorMessage, resourceName, scriptId, lineNumber, columnNumber, callStack);
}

static void promiseRejectHandlerInWorker(v8::PromiseRejectMessage data)
{
    v8::Local<v8::Promise> promise = data.GetPromise();

    // Bail out if called during context initialization.
    v8::Isolate* isolate = promise->GetIsolate();
    ScriptState* scriptState = ScriptState::current(isolate);
    if (!scriptState->contextIsValid())
        return;

    ExecutionContext* executionContext = scriptState->executionContext();
    if (!executionContext)
        return;

    ASSERT(executionContext->isWorkerGlobalScope());
    WorkerScriptController* scriptController = toWorkerGlobalScope(executionContext)->script();
    ASSERT(scriptController);

    if (data.GetEvent() == v8::kPromiseHandlerAddedAfterReject) {
        scriptController->rejectedPromises()->handlerAdded(data);
        return;
    }

    ASSERT(data.GetEvent() == v8::kPromiseRejectWithNoHandler);

    int scriptId = 0;
    int lineNumber = 0;
    int columnNumber = 0;
    String resourceName;
    String errorMessage;

    v8::Local<v8::Message> message = v8::Exception::CreateMessage(isolate, data.GetValue());
    if (!message.IsEmpty()) {
        TOSTRING_VOID(V8StringResource<>, resourceName, message->GetScriptOrigin().ResourceName());
        scriptId = message->GetScriptOrigin().ScriptID()->Value();
        if (v8Call(message->GetLineNumber(scriptState->context()), lineNumber)
            && v8Call(message->GetStartColumn(scriptState->context()), columnNumber))
            ++columnNumber;
        // message->Get() can be empty here. https://crbug.com/450330
        errorMessage = toCoreStringWithNullCheck(message->Get());
    }
    scriptController->rejectedPromises()->rejectedWithNoHandler(scriptState, data, errorMessage, resourceName, scriptId, lineNumber, columnNumber, nullptr);
}

static void failedAccessCheckCallbackInMainThread(v8::Local<v8::Object> host, v8::AccessType type, v8::Local<v8::Value> data)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    Frame* target = findFrame(isolate, host, data);
    if (!target)
        return;
    DOMWindow* targetWindow = target->domWindow();

    // FIXME: We should modify V8 to pass in more contextual information (context, property, and object).
    ExceptionState exceptionState(ExceptionState::UnknownContext, 0, 0, isolate->GetCurrentContext()->Global(), isolate);
    exceptionState.throwSecurityError(targetWindow->sanitizedCrossDomainAccessErrorMessage(callingDOMWindow(isolate)), targetWindow->crossDomainAccessErrorMessage(callingDOMWindow(isolate)));
    exceptionState.throwIfNeeded();
}

static bool codeGenerationCheckCallbackInMainThread(v8::Local<v8::Context> context)
{
    if (ExecutionContext* executionContext = toExecutionContext(context)) {
        if (ContentSecurityPolicy* policy = toDocument(executionContext)->contentSecurityPolicy())
            return policy->allowEval(ScriptState::from(context), ContentSecurityPolicy::SendReport, ContentSecurityPolicy::WillThrowException);
    }
    return false;
}

static void idleGCTaskInMainThread(double deadlineSeconds)
{
    ASSERT(isMainThread());
    ASSERT(RuntimeEnabledFeatures::v8IdleTasksEnabled());
    bool gcFinished = false;
    v8::Isolate* isolate = v8::Isolate::GetCurrent();

    Platform* platform = Platform::current();
    if (deadlineSeconds > platform->monotonicallyIncreasingTime())
        gcFinished = isolate->IdleNotificationDeadline(deadlineSeconds);

    if (gcFinished)
        platform->currentThread()->scheduler()->postIdleTaskAfterWakeup(FROM_HERE, WTF::bind<double>(idleGCTaskInMainThread));
    else
        platform->currentThread()->scheduler()->postIdleTask(FROM_HERE, WTF::bind<double>(idleGCTaskInMainThread));
}

static void timerTraceProfilerInMainThread(const char* name, int status)
{
    if (!status) {
        TRACE_EVENT_BEGIN0("v8", name);
    } else {
        TRACE_EVENT_END0("v8", name);
    }
}

static void initializeV8Common(v8::Isolate* isolate)
{
    v8::V8::AddGCPrologueCallback(V8GCController::gcPrologue);
    v8::V8::AddGCEpilogueCallback(V8GCController::gcEpilogue);

    v8::Debug::SetLiveEditEnabled(isolate, false);

    isolate->SetAutorunMicrotasks(false);
}

namespace {

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
//     struct MemoryHead {
//         size_t magicNum;
//         size_t size;
//     };
//     static const size_t magicNum0 = 0x1122dd44;
//     static const size_t magicNum1 = 0x11227788;
// 
//     static MemoryHead* getPointerHead(void* pointer) { return ((MemoryHead*)pointer) - 1; }
//     static size_t getPointerMemSize(void* pointer) { return getPointerHead(pointer)->size; }
//     static void* getHeadToMemBegin(MemoryHead* head) { return head + 1; }

    void* allocate(size_t size, WTF::ArrayBufferContents::InitializationPolicy policy)
    {
         void* data;
         WTF::ArrayBufferContents::allocateMemory(size, policy, data);

//         WTF::ArrayBufferContents::allocateMemory(size + sizeof(MemoryHead), policy, data);
//         MemoryHead* head = (MemoryHead*)data;
//         head->magicNum = magicNum0;
//         head->size = size;
//         data = getHeadToMemBegin(head);
// 
         return data;
    }

    void* Allocate(size_t size) override
    {
        return allocate(size, WTF::ArrayBufferContents::ZeroInitialize);
    }

    void* AllocateUninitialized(size_t size) override
    {
        return allocate(size, WTF::ArrayBufferContents::DontInitialize);
    }

    void Free(void* data, size_t size) override
    {
//         MemoryHead* head = getPointerHead(data);
//         if (head->magicNum != magicNum0)
//             DebugBreak();
//         WTF::ArrayBufferContents::freeMemory(head, size);
        WTF::ArrayBufferContents::freeMemory(data, size);
    }
};

} // namespace

void V8Initializer::initializeMainThreadIfNeeded()
{
    ASSERT(isMainThread());

    static bool initialized = false;
    if (initialized)
        return;
    initialized = true;

    DEFINE_STATIC_LOCAL(ArrayBufferAllocator, arrayBufferAllocator, ());
    gin::IsolateHolder::Initialize(gin::IsolateHolder::kNonStrictMode, &arrayBufferAllocator);

    v8::Isolate* isolate = V8PerIsolateData::initialize();

    initializeV8Common(isolate);

    v8::V8::SetFatalErrorHandler(reportFatalErrorInMainThread);
    v8::V8::AddMessageListener(messageHandlerInMainThread);
    v8::V8::SetFailedAccessCheckCallbackFunction(failedAccessCheckCallbackInMainThread);
    v8::V8::SetAllowCodeGenerationFromStringsCallback(codeGenerationCheckCallbackInMainThread);

    if (RuntimeEnabledFeatures::v8IdleTasksEnabled())
        Platform::current()->currentThread()->scheduler()->postIdleTask(FROM_HERE, WTF::bind<double>(idleGCTaskInMainThread));

    isolate->SetEventLogger(timerTraceProfilerInMainThread);
    isolate->SetPromiseRejectCallback(promiseRejectHandlerInMainThread);

#ifdef MINIBLINK_NOT_IMPLEMENTED
    ScriptProfiler::initialize();
#endif // MINIBLINK_NOT_IMPLEMENTED
}

static void reportFatalErrorInWorker(const char* location, const char* message)
{
    // FIXME: We temporarily deal with V8 internal error situations such as out-of-memory by crashing the worker.
    CRASH();
}

static void messageHandlerInWorker(v8::Local<v8::Message> message, v8::Local<v8::Value> data)
{
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    V8PerIsolateData* perIsolateData = V8PerIsolateData::from(isolate);
    // Exceptions that occur in error handler should be ignored since in that case
    // WorkerGlobalScope::reportException will send the exception to the worker object.
    if (perIsolateData->isReportingException())
        return;
    perIsolateData->setReportingException(true);

    ScriptState* scriptState = ScriptState::current(isolate);
    // During the frame teardown, there may not be a valid context.
    if (ExecutionContext* context = scriptState->executionContext()) {
        String errorMessage = toCoreStringWithNullCheck(message->Get());
        TOSTRING_VOID(V8StringResource<>, sourceURL, message->GetScriptOrigin().ResourceName());
        int scriptId = 0;
        RefPtrWillBeRawPtr<ScriptCallStack> callStack = extractCallStack(isolate, message, &scriptId);

        int lineNumber = 0;
        int columnNumber = 0;
        if (v8Call(message->GetLineNumber(scriptState->context()), lineNumber)
            && v8Call(message->GetStartColumn(scriptState->context()), columnNumber))
            ++columnNumber;
        RefPtrWillBeRawPtr<ErrorEvent> event = ErrorEvent::create(errorMessage, sourceURL, lineNumber, columnNumber, &DOMWrapperWorld::current(isolate));
        AccessControlStatus corsStatus = message->IsSharedCrossOrigin() ? SharableCrossOrigin : NotSharableCrossOrigin;

        // If execution termination has been triggered as part of constructing
        // the error event from the v8::Message, quietly leave.
        if (!v8::V8::IsExecutionTerminating(isolate)) {
            V8ErrorHandler::storeExceptionOnErrorEventWrapper(isolate, event.get(), data, scriptState->context()->Global());
            context->reportException(event.release(), scriptId, callStack, corsStatus);
        }
    }

    perIsolateData->setReportingException(false);
}

static const int kWorkerMaxStackSize = 500 * 1024;

// This function uses a local stack variable to determine the isolate's stack limit. AddressSanitizer may
// relocate that local variable to a fake stack, which may lead to problems during JavaScript execution.
// Therefore we disable AddressSanitizer for V8Initializer::initializeWorker().
NO_SANITIZE_ADDRESS
void V8Initializer::initializeWorker(v8::Isolate* isolate)
{
    initializeV8Common(isolate);

    v8::V8::AddMessageListener(messageHandlerInWorker);
    v8::V8::SetFatalErrorHandler(reportFatalErrorInWorker);

    uint32_t here;
    isolate->SetStackLimit(reinterpret_cast<uintptr_t>(&here - kWorkerMaxStackSize / sizeof(uint32_t*)));
    isolate->SetPromiseRejectCallback(promiseRejectHandlerInWorker);
}

} // namespace blink
