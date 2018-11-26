// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/DOMWindow.h"

#include "bindings/core/v8/ScriptCallStackFactory.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/SecurityContext.h"
#include "core/events/MessageEvent.h"
#include "core/frame/External.h"
#include "core/frame/Frame.h"
#include "core/frame/FrameClient.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/Location.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "core/inspector/ConsoleMessageStorage.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/MixedContentChecker.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"

namespace blink {

DOMWindow::DOMWindow()
    : m_windowIsClosing(false)
{
}

DOMWindow::~DOMWindow()
{
}

v8::Local<v8::Object> DOMWindow::wrap(v8::Isolate*, v8::Local<v8::Object> creationContext)
{
    // DOMWindow must never be wrapped with wrap method.  The wrappers must be
    // created at WindowProxy::installDOMWindow().
    RELEASE_ASSERT_NOT_REACHED();
    return v8::Local<v8::Object>();
}

v8::Local<v8::Object> DOMWindow::associateWithWrapper(v8::Isolate*, const WrapperTypeInfo*, v8::Local<v8::Object> wrapper)
{
    RELEASE_ASSERT_NOT_REACHED(); // same as wrap method
    return v8::Local<v8::Object>();
}

const AtomicString& DOMWindow::interfaceName() const
{
    return EventTargetNames::DOMWindow;
}

Location* DOMWindow::location() const
{
    if (!m_location)
        m_location = Location::create(frame());
    return m_location.get();
}

bool DOMWindow::closed() const
{
    return m_windowIsClosing || !frame() || !frame()->host();
}

unsigned DOMWindow::length() const
{
    return frame() ? frame()->tree().scopedChildCount() : 0;
}

DOMWindow* DOMWindow::self() const
{
    if (!frame())
        return nullptr;

    return frame()->domWindow();
}

DOMWindow* DOMWindow::opener() const
{
    // FIXME: Use FrameTree to get opener as well, to simplify logic here.
    if (!frame() || !frame()->client())
        return nullptr;

    Frame* opener = frame()->client()->opener();
    return opener ? opener->domWindow() : nullptr;
}

DOMWindow* DOMWindow::parent() const
{
    if (!frame())
        return nullptr;

    Frame* parent = frame()->tree().parent();
    return parent ? parent->domWindow() : frame()->domWindow();
}

DOMWindow* DOMWindow::top() const
{
    if (!frame())
        return nullptr;

    return frame()->tree().top()->domWindow();
}

External* DOMWindow::external()
{
    if (!m_external)
        m_external = new External();
    return m_external;
}

DOMWindow* DOMWindow::anonymousIndexedGetter(uint32_t index) const
{
    if (!frame())
        return nullptr;

    Frame* child = frame()->tree().scopedChild(index);
    return child ? child->domWindow() : nullptr;
}

bool DOMWindow::isCurrentlyDisplayedInFrame() const
{
    return frame() && frame()->domWindow() == this && frame()->host();
}

bool DOMWindow::isInsecureScriptAccess(LocalDOMWindow& callingWindow, const String& urlString)
{
    if (!protocolIsJavaScript(urlString))
        return false;

    // If this DOMWindow isn't currently active in the Frame, then there's no
    // way we should allow the access.
    if (isCurrentlyDisplayedInFrame()) {
        // FIXME: Is there some way to eliminate the need for a separate "callingWindow == this" check?
        if (&callingWindow == this)
            return false;

        // FIXME: The name canAccess seems to be a roundabout way to ask "can execute script".
        // Can we name the SecurityOrigin function better to make this more clear?
        if (callingWindow.document()->securityOrigin()->canAccessCheckSuborigins(frame()->securityContext()->securityOrigin()))
            return false;
    }

    callingWindow.printErrorMessage(crossDomainAccessErrorMessage(&callingWindow));
    return true;
}

void DOMWindow::resetLocation()
{
    // Location needs to be reset manually because it doesn't inherit from DOMWindowProperty.
    // DOMWindowProperty is local-only, and Location needs to support remote windows, too.
    if (m_location) {
        m_location->reset();
        m_location = nullptr;
    }
}

void DOMWindow::postMessage(PassRefPtr<SerializedScriptValue> message, const MessagePortArray* ports, const String& targetOrigin, LocalDOMWindow* source, ExceptionState& exceptionState)
{
    if (!isCurrentlyDisplayedInFrame())
        return;

    Document* sourceDocument = source->document();

    // Compute the target origin.  We need to do this synchronously in order
    // to generate the SyntaxError exception correctly.
    RefPtr<SecurityOrigin> target;
    if (targetOrigin == "/") {
        if (!sourceDocument)
            return;
        target = sourceDocument->securityOrigin();
    } else if (targetOrigin != "*") {
        target = SecurityOrigin::createFromString(targetOrigin);
        // It doesn't make sense target a postMessage at a unique origin
        // because there's no way to represent a unique origin in a string.
        if (target->isUnique()) {
            exceptionState.throwDOMException(SyntaxError, "Invalid target origin '" + targetOrigin + "' in a call to 'postMessage'.");
            return;
        }
    }

    OwnPtr<MessagePortChannelArray> channels = MessagePort::disentanglePorts(executionContext(), ports, exceptionState);
    if (exceptionState.hadException())
        return;

    // Capture the source of the message.  We need to do this synchronously
    // in order to capture the source of the message correctly.
    if (!sourceDocument)
        return;
    String sourceOrigin = sourceDocument->securityOrigin()->toString();

    // FIXME: MixedContentChecker needs to be refactored for OOPIF.  For now,
    // create the url using replicated origins for remote frames.
    KURL targetUrl = isLocalDOMWindow() ? document()->url() : KURL(KURL(), frame()->securityContext()->securityOrigin()->toString());
    if (MixedContentChecker::isMixedContent(sourceDocument->securityOrigin(), targetUrl))
        UseCounter::count(frame(), UseCounter::PostMessageFromSecureToInsecure);
    else if (MixedContentChecker::isMixedContent(frame()->securityContext()->securityOrigin(), sourceDocument->url()))
        UseCounter::count(frame(), UseCounter::PostMessageFromInsecureToSecure);

    // Give the embedder a chance to intercept this postMessage.  If the
    // target is a remote frame, the message will be forwarded through the
    // browser process.
    RefPtrWillBeRawPtr<MessageEvent> event = MessageEvent::create(channels.release(), message, sourceOrigin, String(), source);
    bool didHandleMessageEvent = frame()->client()->willCheckAndDispatchMessageEvent(target.get(), event.get(), source->document()->frame());
    if (!didHandleMessageEvent) {
        // Capture stack trace only when inspector front-end is loaded as it may be time consuming.
        RefPtrWillBeRawPtr<ScriptCallStack> stackTrace = nullptr;
        if (InspectorInstrumentation::consoleAgentEnabled(sourceDocument))
            stackTrace = createScriptCallStack(ScriptCallStack::maxCallStackSizeToCapture, true);

        toLocalDOMWindow(this)->schedulePostMessage(event, source, target.get(), stackTrace.release());
    }
}

// FIXME: Once we're throwing exceptions for cross-origin access violations, we will always sanitize the target
// frame details, so we can safely combine 'crossDomainAccessErrorMessage' with this method after considering
// exactly which details may be exposed to JavaScript.
//
// http://crbug.com/17325
String DOMWindow::sanitizedCrossDomainAccessErrorMessage(LocalDOMWindow* callingWindow)
{
    if (!callingWindow || !callingWindow->document() || !frame())
        return String();

    const KURL& callingWindowURL = callingWindow->document()->url();
    if (callingWindowURL.isNull())
        return String();

    ASSERT(!callingWindow->document()->securityOrigin()->canAccessCheckSuborigins(frame()->securityContext()->securityOrigin()));

    SecurityOrigin* activeOrigin = callingWindow->document()->securityOrigin();
    String message = "Blocked a frame with origin \"" + activeOrigin->toString() + "\" from accessing a cross-origin frame.";

    // FIXME: Evaluate which details from 'crossDomainAccessErrorMessage' may safely be reported to JavaScript.

    return message;
}

String DOMWindow::crossDomainAccessErrorMessage(LocalDOMWindow* callingWindow)
{
    if (!callingWindow || !callingWindow->document() || !frame())
        return String();

    const KURL& callingWindowURL = callingWindow->document()->url();
    if (callingWindowURL.isNull())
        return String();

    // FIXME: This message, and other console messages, have extra newlines. Should remove them.
    SecurityOrigin* activeOrigin = callingWindow->document()->securityOrigin();
    SecurityOrigin* targetOrigin = frame()->securityContext()->securityOrigin();
    ASSERT(!activeOrigin->canAccessCheckSuborigins(targetOrigin));

    String message = "Blocked a frame with origin \"" + activeOrigin->toString() + "\" from accessing a frame with origin \"" + targetOrigin->toString() + "\". ";

    // Sandbox errors: Use the origin of the frames' location, rather than their actual origin (since we know that at least one will be "null").
    KURL activeURL = callingWindow->document()->url();
    // TODO(alexmos): RemoteFrames do not have a document, and their URLs
    // aren't replicated.  For now, construct the URL using the replicated
    // origin for RemoteFrames. If the target frame is remote and sandboxed,
    // there isn't anything else to show other than "null" for its origin.
    KURL targetURL = isLocalDOMWindow() ? document()->url() : KURL(KURL(), targetOrigin->toString());
    if (frame()->securityContext()->isSandboxed(SandboxOrigin) || callingWindow->document()->isSandboxed(SandboxOrigin)) {
        message = "Blocked a frame at \"" + SecurityOrigin::create(activeURL)->toString() + "\" from accessing a frame at \"" + SecurityOrigin::create(targetURL)->toString() + "\". ";
        if (frame()->securityContext()->isSandboxed(SandboxOrigin) && callingWindow->document()->isSandboxed(SandboxOrigin))
            return "Sandbox access violation: " + message + " Both frames are sandboxed and lack the \"allow-same-origin\" flag.";
        if (frame()->securityContext()->isSandboxed(SandboxOrigin))
            return "Sandbox access violation: " + message + " The frame being accessed is sandboxed and lacks the \"allow-same-origin\" flag.";
        return "Sandbox access violation: " + message + " The frame requesting access is sandboxed and lacks the \"allow-same-origin\" flag.";
    }

    // Protocol errors: Use the URL's protocol rather than the origin's protocol so that we get a useful message for non-heirarchal URLs like 'data:'.
    if (targetOrigin->protocol() != activeOrigin->protocol())
        return message + " The frame requesting access has a protocol of \"" + activeURL.protocol() + "\", the frame being accessed has a protocol of \"" + targetURL.protocol() + "\". Protocols must match.\n";

    // 'document.domain' errors.
    if (targetOrigin->domainWasSetInDOM() && activeOrigin->domainWasSetInDOM())
        return message + "The frame requesting access set \"document.domain\" to \"" + activeOrigin->domain() + "\", the frame being accessed set it to \"" + targetOrigin->domain() + "\". Both must set \"document.domain\" to the same value to allow access.";
    if (activeOrigin->domainWasSetInDOM())
        return message + "The frame requesting access set \"document.domain\" to \"" + activeOrigin->domain() + "\", but the frame being accessed did not. Both must set \"document.domain\" to the same value to allow access.";
    if (targetOrigin->domainWasSetInDOM())
        return message + "The frame being accessed set \"document.domain\" to \"" + targetOrigin->domain() + "\", but the frame requesting access did not. Both must set \"document.domain\" to the same value to allow access.";

    // Default.
    return message + "Protocols, domains, and ports must match.";
}

void DOMWindow::close(ExecutionContext* context)
{
    if (!frame() || !frame()->isMainFrame())
        return;

    Page* page = frame()->page();
    if (!page)
        return;

    Document* activeDocument = nullptr;
    if (context) {
        ASSERT(isMainThread());
        activeDocument = toDocument(context);
        if (!activeDocument)
            return;

        if (!activeDocument->frame() || !activeDocument->frame()->canNavigate(*frame()))
            return;
    }

    Settings* settings = frame()->settings();
    bool allowScriptsToCloseWindows = settings && settings->allowScriptsToCloseWindows();

    if (!page->openedByDOM() && frame()->client()->backForwardLength() > 1 && !allowScriptsToCloseWindows) {
        if (activeDocument) {
            activeDocument->domWindow()->frameConsole()->addMessage(ConsoleMessage::create(JSMessageSource, WarningMessageLevel, "Scripts may close only the windows that were opened by it."));
        }
        return;
    }

    if (!frame()->shouldClose())
        return;

    InspectorInstrumentation::willCloseWindow(context);

    page->chromeClient().closeWindowSoon();

    // So as to make window.closed return the expected result
    // after window.close(), separately record the to-be-closed
    // state of this window. Scripts may access window.closed
    // before the deferred close operation has gone ahead.
    m_windowIsClosing = true;
}

DEFINE_TRACE(DOMWindow)
{
    visitor->trace(m_location);
    visitor->trace(m_external);
    EventTargetWithInlineData::trace(visitor);
}

} // namespace blink
