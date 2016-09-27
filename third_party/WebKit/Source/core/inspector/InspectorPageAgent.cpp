/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "core/inspector/InspectorPageAgent.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptRegexp.h"
#include "core/HTMLNames.h"
#include "core/dom/DOMImplementation.h"
#include "core/dom/Document.h"
#include "core/fetch/CSSStyleSheetResource.h"
#include "core/fetch/FontResource.h"
#include "core/fetch/ImageResource.h"
#include "core/fetch/MemoryCache.h"
#include "core/fetch/Resource.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/VoidCallback.h"
#include "core/html/imports/HTMLImportLoader.h"
#include "core/html/imports/HTMLImportsController.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/inspector/ContentSearchUtils.h"
#include "core/inspector/DOMPatchSupport.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/InspectorCSSAgent.h"
#include "core/inspector/InspectorDebuggerAgent.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorOverlay.h"
#include "core/inspector/InspectorResourceContentLoader.h"
#include "core/inspector/InspectorState.h"
#include "core/inspector/InstrumentingAgents.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoader.h"
#include "platform/JSONValues.h"
#include "platform/MIMETypeRegistry.h"
#include "platform/PlatformResourceLoader.h"
#include "platform/UserGestureIndicator.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/CurrentTime.h"
#include "wtf/ListHashSet.h"
#include "wtf/Vector.h"
#include "wtf/text/Base64.h"
#include "wtf/text/TextEncoding.h"

namespace blink {

namespace PageAgentState {
static const char pageAgentEnabled[] = "pageAgentEnabled";
static const char pageAgentScriptsToEvaluateOnLoad[] = "pageAgentScriptsToEvaluateOnLoad";
static const char showSizeOnResize[] = "showSizeOnResize";
static const char showGridOnResize[] = "showGridOnResize";
static const char screencastEnabled[] = "screencastEnabled";
}

namespace {

KURL urlWithoutFragment(const KURL& url)
{
    KURL result = url;
    result.removeFragmentIdentifier();
    return result;
}

String frameId(LocalFrame* frame)
{
    return frame ? IdentifiersFactory::frameId(frame) : "";
}

TypeBuilder::Page::DialogType::Enum dialogTypeToProtocol(ChromeClient::DialogType dialogType)
{
    switch (dialogType) {
    case ChromeClient::AlertDialog:
        return TypeBuilder::Page::DialogType::Alert;
    case ChromeClient::ConfirmDialog:
        return TypeBuilder::Page::DialogType::Confirm;
    case ChromeClient::PromptDialog:
        return TypeBuilder::Page::DialogType::Prompt;
    case ChromeClient::HTMLDialog:
        return TypeBuilder::Page::DialogType::Beforeunload;
    }
    return TypeBuilder::Page::DialogType::Alert;
}

}

class InspectorPageAgent::GetResourceContentLoadListener final : public VoidCallback {
public:
    GetResourceContentLoadListener(InspectorPageAgent*, const String& frameId, const String& url, PassRefPtrWillBeRawPtr<GetResourceContentCallback>);
    DECLARE_VIRTUAL_TRACE();
    virtual void handleEvent() override;
private:
    RawPtrWillBeMember<InspectorPageAgent> m_pageAgent;
    String m_frameId;
    String m_url;
    RefPtrWillBeMember<GetResourceContentCallback> m_callback;
};

InspectorPageAgent::GetResourceContentLoadListener::GetResourceContentLoadListener(InspectorPageAgent* pageAgent, const String& frameId, const String& url, PassRefPtrWillBeRawPtr<GetResourceContentCallback> callback)
    : m_pageAgent(pageAgent)
    , m_frameId(frameId)
    , m_url(url)
    , m_callback(callback)
{
}

DEFINE_TRACE(InspectorPageAgent::GetResourceContentLoadListener)
{
    visitor->trace(m_pageAgent);
    visitor->trace(m_callback);
    VoidCallback::trace(visitor);
}

void InspectorPageAgent::GetResourceContentLoadListener::handleEvent()
{
    if (!m_callback->isActive())
        return;
    m_pageAgent->getResourceContentAfterResourcesContentLoaded(m_frameId, m_url, m_callback);
}

static bool decodeBuffer(const char* buffer, unsigned size, const String& textEncodingName, String* result)
{
    if (buffer) {
        WTF::TextEncoding encoding(textEncodingName);
        if (!encoding.isValid())
            encoding = WindowsLatin1Encoding();
        *result = encoding.decode(buffer, size);
        return true;
    }
    return false;
}

static bool prepareResourceBuffer(Resource* cachedResource, bool* hasZeroSize)
{
    *hasZeroSize = false;
    if (!cachedResource)
        return false;

    if (cachedResource->dataBufferingPolicy() == DoNotBufferData)
        return false;

    // Zero-sized resources don't have data at all -- so fake the empty buffer, instead of indicating error by returning 0.
    if (!cachedResource->encodedSize()) {
        *hasZeroSize = true;
        return true;
    }

    if (cachedResource->isPurgeable()) {
        // If the resource is purgeable then make it unpurgeable to get
        // get its data. This might fail, in which case we return an
        // empty String.
        // FIXME: should we do something else in the case of a purged
        // resource that informs the user why there is no data in the
        // inspector?
        if (!cachedResource->lock())
            return false;
    }

    return true;
}

static bool hasTextContent(Resource* cachedResource)
{
    Resource::Type type = cachedResource->type();
    return type == Resource::CSSStyleSheet || type == Resource::XSLStyleSheet || type == Resource::Script || type == Resource::Raw || type == Resource::ImportResource || type == Resource::MainResource;
}

PassOwnPtr<TextResourceDecoder> InspectorPageAgent::createResourceTextDecoder(const String& mimeType, const String& textEncodingName)
{
    if (!textEncodingName.isEmpty())
        return TextResourceDecoder::create("text/plain", textEncodingName);
    if (DOMImplementation::isXMLMIMEType(mimeType)) {
        OwnPtr<TextResourceDecoder> decoder = TextResourceDecoder::create("application/xml");
        decoder->useLenientXMLDecoding();
        return decoder.release();
    }
    if (equalIgnoringCase(mimeType, "text/html"))
        return TextResourceDecoder::create("text/html", "UTF-8");
    if (MIMETypeRegistry::isSupportedJavaScriptMIMEType(mimeType) || DOMImplementation::isJSONMIMEType(mimeType))
        return TextResourceDecoder::create("text/plain", "UTF-8");
    if (DOMImplementation::isTextMIMEType(mimeType))
        return TextResourceDecoder::create("text/plain", "ISO-8859-1");
    return PassOwnPtr<TextResourceDecoder>();
}

static void resourceContent(ErrorString* errorString, LocalFrame* frame, const KURL& url, String* result, bool* base64Encoded)
{
    DocumentLoader* loader = InspectorPageAgent::assertDocumentLoader(errorString, frame);
    if (!loader)
        return;

    if (!InspectorPageAgent::cachedResourceContent(InspectorPageAgent::cachedResource(frame, url), result, base64Encoded))
        *errorString = "No resource with given URL found";
}

static bool encodeCachedResourceContent(Resource* cachedResource, bool hasZeroSize, String* result, bool* base64Encoded)
{
    *base64Encoded = true;
    RefPtr<SharedBuffer> buffer = hasZeroSize ? SharedBuffer::create() : cachedResource->resourceBuffer();

    if (!buffer)
        return false;

    *result = base64Encode(buffer->data(), buffer->size());
    return true;
}

bool InspectorPageAgent::cachedResourceContent(Resource* cachedResource, String* result, bool* base64Encoded)
{
    bool hasZeroSize;
    bool prepared = prepareResourceBuffer(cachedResource, &hasZeroSize);
    if (!prepared)
        return false;

    if (!hasTextContent(cachedResource))
        return encodeCachedResourceContent(cachedResource, hasZeroSize, result, base64Encoded);
    *base64Encoded = false;

    if (hasZeroSize) {
        *result = "";
        return true;
    }

    if (cachedResource) {
        switch (cachedResource->type()) {
        case Resource::CSSStyleSheet:
            *result = toCSSStyleSheetResource(cachedResource)->sheetText();
            return true;
        case Resource::Script:
            *result = cachedResource->resourceBuffer() ? toScriptResource(cachedResource)->decodedText() : toScriptResource(cachedResource)->script();
            return true;
        case Resource::ImportResource: // Fall through.
        case Resource::Raw: {
            SharedBuffer* buffer = cachedResource->resourceBuffer();
            if (!buffer)
                return false;
            OwnPtr<TextResourceDecoder> decoder = InspectorPageAgent::createResourceTextDecoder(cachedResource->response().mimeType(), cachedResource->response().textEncodingName());
            if (!decoder)
                return encodeCachedResourceContent(cachedResource, hasZeroSize, result, base64Encoded);
            String content = decoder->decode(buffer->data(), buffer->size());
            *result = content + decoder->flush();
            return true;
        }
        default:
            SharedBuffer* buffer = cachedResource->resourceBuffer();
            return decodeBuffer(buffer ? buffer->data() : nullptr, buffer ? buffer->size() : 0, cachedResource->response().textEncodingName(), result);
        }
    }
    return false;
}

// static
bool InspectorPageAgent::sharedBufferContent(PassRefPtr<SharedBuffer> buffer, const String& textEncodingName, bool withBase64Encode, String* result)
{
    return dataContent(buffer ? buffer->data() : nullptr, buffer ? buffer->size() : 0, textEncodingName, withBase64Encode, result);
}

bool InspectorPageAgent::dataContent(const char* data, unsigned size, const String& textEncodingName, bool withBase64Encode, String* result)
{
    if (withBase64Encode) {
        *result = base64Encode(data, size);
        return true;
    }

    return decodeBuffer(data, size, textEncodingName, result);
}

PassOwnPtrWillBeRawPtr<InspectorPageAgent> InspectorPageAgent::create(LocalFrame* inspectedFrame, InspectorOverlay* overlay, InspectorResourceContentLoader* resourceContentLoader)
{
    return adoptPtrWillBeNoop(new InspectorPageAgent(inspectedFrame, overlay, resourceContentLoader));
}

void InspectorPageAgent::setDeferredAgents(InspectorDebuggerAgent* debuggerAgent, InspectorCSSAgent* cssAgent)
{
    ASSERT(!m_debuggerAgent && !m_cssAgent);
    m_debuggerAgent = debuggerAgent;
    m_cssAgent = cssAgent;
}

Resource* InspectorPageAgent::cachedResource(LocalFrame* frame, const KURL& url)
{
    Document* document = frame->document();
    if (!document)
        return nullptr;
    Resource* cachedResource = document->fetcher()->cachedResource(url);
    if (!cachedResource) {
        Vector<Document*> allImports = InspectorPageAgent::importsForFrame(frame);
        for (Document* import : allImports) {
            cachedResource = import->fetcher()->cachedResource(url);
            if (cachedResource)
                break;
        }
    }
    if (!cachedResource)
        cachedResource = memoryCache()->resourceForURL(url, document->fetcher()->getCacheIdentifier());
    return cachedResource;
}

TypeBuilder::Page::ResourceType::Enum InspectorPageAgent::resourceTypeJson(InspectorPageAgent::ResourceType resourceType)
{
    switch (resourceType) {
    case DocumentResource:
        return TypeBuilder::Page::ResourceType::Document;
    case FontResource:
        return TypeBuilder::Page::ResourceType::Font;
    case ImageResource:
        return TypeBuilder::Page::ResourceType::Image;
    case MediaResource:
        return TypeBuilder::Page::ResourceType::Media;
    case ScriptResource:
        return TypeBuilder::Page::ResourceType::Script;
    case StylesheetResource:
        return TypeBuilder::Page::ResourceType::Stylesheet;
    case TextTrackResource:
        return TypeBuilder::Page::ResourceType::TextTrack;
    case XHRResource:
        return TypeBuilder::Page::ResourceType::XHR;
    case WebSocketResource:
        return TypeBuilder::Page::ResourceType::WebSocket;
    case OtherResource:
        return TypeBuilder::Page::ResourceType::Other;
    }
    return TypeBuilder::Page::ResourceType::Other;
}

InspectorPageAgent::ResourceType InspectorPageAgent::cachedResourceType(const Resource& cachedResource)
{
    switch (cachedResource.type()) {
    case Resource::Image:
        return InspectorPageAgent::ImageResource;
    case Resource::Font:
        return InspectorPageAgent::FontResource;
    case Resource::Media:
        return InspectorPageAgent::MediaResource;
    case Resource::TextTrack:
        return InspectorPageAgent::TextTrackResource;
    case Resource::CSSStyleSheet:
        // Fall through.
    case Resource::XSLStyleSheet:
        return InspectorPageAgent::StylesheetResource;
    case Resource::Script:
        return InspectorPageAgent::ScriptResource;
    case Resource::ImportResource:
        // Fall through.
    case Resource::MainResource:
        return InspectorPageAgent::DocumentResource;
    default:
        break;
    }
    return InspectorPageAgent::OtherResource;
}

TypeBuilder::Page::ResourceType::Enum InspectorPageAgent::cachedResourceTypeJson(const Resource& cachedResource)
{
    return resourceTypeJson(cachedResourceType(cachedResource));
}

InspectorPageAgent::InspectorPageAgent(LocalFrame* inspectedFrame, InspectorOverlay* overlay, InspectorResourceContentLoader* resourceContentLoader)
    : InspectorBaseAgent<InspectorPageAgent, InspectorFrontend::Page>("Page")
    , m_inspectedFrame(inspectedFrame)
    , m_debuggerAgent(nullptr)
    , m_cssAgent(nullptr)
    , m_overlay(overlay)
    , m_lastScriptIdentifier(0)
    , m_enabled(false)
    , m_reloading(false)
    , m_inspectorResourceContentLoader(resourceContentLoader)
{
}

void InspectorPageAgent::restore()
{
    if (m_state->getBoolean(PageAgentState::pageAgentEnabled)) {
        ErrorString error;
        enable(&error);
    }
}

void InspectorPageAgent::enable(ErrorString*)
{
    m_enabled = true;
    m_state->setBoolean(PageAgentState::pageAgentEnabled, true);
    m_instrumentingAgents->setInspectorPageAgent(this);
}

void InspectorPageAgent::disable(ErrorString*)
{
    m_enabled = false;
    m_state->setBoolean(PageAgentState::pageAgentEnabled, false);
    m_state->remove(PageAgentState::pageAgentScriptsToEvaluateOnLoad);
    m_scriptToEvaluateOnLoadOnce = String();
    m_pendingScriptToEvaluateOnLoadOnce = String();
    m_instrumentingAgents->setInspectorPageAgent(0);

    setShowViewportSizeOnResize(0, false, 0);
    stopScreencast(0);

    finishReload();
}

void InspectorPageAgent::addScriptToEvaluateOnLoad(ErrorString*, const String& source, String* identifier)
{
    RefPtr<JSONObject> scripts = m_state->getObject(PageAgentState::pageAgentScriptsToEvaluateOnLoad);
    if (!scripts) {
        scripts = JSONObject::create();
        m_state->setObject(PageAgentState::pageAgentScriptsToEvaluateOnLoad, scripts);
    }
    // Assure we don't override existing ids -- m_lastScriptIdentifier could get out of sync WRT actual
    // scripts once we restored the scripts from the cookie during navigation.
    do {
        *identifier = String::number(++m_lastScriptIdentifier);
    } while (scripts->find(*identifier) != scripts->end());
    scripts->setString(*identifier, source);

    // Force cookie serialization.
    m_state->setObject(PageAgentState::pageAgentScriptsToEvaluateOnLoad, scripts);
}

void InspectorPageAgent::removeScriptToEvaluateOnLoad(ErrorString* error, const String& identifier)
{
    RefPtr<JSONObject> scripts = m_state->getObject(PageAgentState::pageAgentScriptsToEvaluateOnLoad);
    if (!scripts || scripts->find(identifier) == scripts->end()) {
        *error = "Script not found";
        return;
    }
    scripts->remove(identifier);
}

void InspectorPageAgent::reload(ErrorString*, const bool* const optionalIgnoreCache, const String* optionalScriptToEvaluateOnLoad)
{
    m_pendingScriptToEvaluateOnLoadOnce = optionalScriptToEvaluateOnLoad ? *optionalScriptToEvaluateOnLoad : "";
    ErrorString unused;
    m_debuggerAgent->setSkipAllPauses(&unused, true);
    m_reloading = true;
    inspectedFrame()->reload(asBool(optionalIgnoreCache) ? FrameLoadTypeReloadFromOrigin : FrameLoadTypeReload, NotClientRedirect);
}

void InspectorPageAgent::navigate(ErrorString*, const String& url, String* outFrameId)
{
    *outFrameId = frameId(inspectedFrame());
}

static void cachedResourcesForDocument(Document* document, Vector<Resource*>& result, bool skipXHRs)
{
    const ResourceFetcher::DocumentResourceMap& allResources = document->fetcher()->allResources();
    for (const auto& resource : allResources) {
        Resource* cachedResource = resource.value.get();

        switch (cachedResource->type()) {
        case Resource::Image:
            // Skip images that were not auto loaded (images disabled in the user agent).
            if (toImageResource(cachedResource)->stillNeedsLoad())
                continue;
            break;
        case Resource::Font:
            // Skip fonts that were referenced in CSS but never used/downloaded.
            if (toFontResource(cachedResource)->stillNeedsLoad())
                continue;
            break;
        case Resource::Raw:
            if (skipXHRs)
                continue;
            break;
        default:
            // All other Resource types download immediately.
            break;
        }

        result.append(cachedResource);
    }
}

// static
Vector<Document*> InspectorPageAgent::importsForFrame(LocalFrame* frame)
{
    Vector<Document*> result;
    Document* rootDocument = frame->document();

    if (HTMLImportsController* controller = rootDocument->importsController()) {
        for (size_t i = 0; i < controller->loaderCount(); ++i) {
            if (Document* document = controller->loaderAt(i)->document())
                result.append(document);
        }
    }

    return result;
}

static Vector<Resource*> cachedResourcesForFrame(LocalFrame* frame, bool skipXHRs)
{
    Vector<Resource*> result;
    Document* rootDocument = frame->document();
    Vector<Document*> loaders = InspectorPageAgent::importsForFrame(frame);

    cachedResourcesForDocument(rootDocument, result, skipXHRs);
    for (size_t i = 0; i < loaders.size(); ++i)
        cachedResourcesForDocument(loaders[i], result, skipXHRs);

    return result;
}

void InspectorPageAgent::getResourceTree(ErrorString*, RefPtr<TypeBuilder::Page::FrameResourceTree>& object)
{
    object = buildObjectForFrameTree(inspectedFrame());
}

void InspectorPageAgent::finishReload()
{
    if (!m_reloading)
        return;
    m_reloading = false;
    ErrorString unused;
    m_debuggerAgent->setSkipAllPauses(&unused, false);
}

void InspectorPageAgent::getResourceContentAfterResourcesContentLoaded(const String& frameId, const String& url, PassRefPtrWillBeRawPtr<GetResourceContentCallback> callback)
{
    ErrorString errorString;
    LocalFrame* frame = assertFrame(&errorString, frameId);
    if (!frame) {
        callback->sendFailure(errorString);
        return;
    }
    String content;
    bool base64Encoded;
    resourceContent(&errorString, frame, KURL(ParsedURLString, url), &content, &base64Encoded);
    if (!errorString.isEmpty()) {
        callback->sendFailure(errorString);
        return;
    }
    callback->sendSuccess(content, base64Encoded);
}

void InspectorPageAgent::getResourceContent(ErrorString* errorString, const String& frameId, const String& url, PassRefPtrWillBeRawPtr<GetResourceContentCallback> callback)
{
    String content;
    if (m_debuggerAgent->getEditedScript(url, &content) || m_cssAgent->getEditedStyleSheet(url, &content)) {
        callback->sendSuccess(content, false);
        return;
    }
    if (!m_enabled) {
        callback->sendFailure("Agent is not enabled.");
        return;
    }
    m_inspectorResourceContentLoader->ensureResourcesContentLoaded(new GetResourceContentLoadListener(this, frameId, url, callback));
}

static bool textContentForResource(Resource* cachedResource, String* result)
{
    if (hasTextContent(cachedResource)) {
        String content;
        bool base64Encoded;
        if (InspectorPageAgent::cachedResourceContent(cachedResource, result, &base64Encoded)) {
            ASSERT(!base64Encoded);
            return true;
        }
    }
    return false;
}

void InspectorPageAgent::searchInResource(ErrorString*, const String& frameId, const String& url, const String& query, const bool* const optionalCaseSensitive, const bool* const optionalIsRegex, RefPtr<TypeBuilder::Array<TypeBuilder::Debugger::SearchMatch>>& results)
{
    results = TypeBuilder::Array<TypeBuilder::Debugger::SearchMatch>::create();

    LocalFrame* frame = frameForId(frameId);
    KURL kurl(ParsedURLString, url);

    FrameLoader* frameLoader = frame ? &frame->loader() : nullptr;
    DocumentLoader* loader = frameLoader ? frameLoader->documentLoader() : nullptr;
    if (!loader)
        return;

    String content;
    bool success = false;
    Resource* resource = cachedResource(frame, kurl);
    if (resource)
        success = textContentForResource(resource, &content);

    if (!success)
        return;

    results = ContentSearchUtils::searchInTextByLines(content, query, asBool(optionalCaseSensitive), asBool(optionalIsRegex));
}

void InspectorPageAgent::setDocumentContent(ErrorString* errorString, const String& frameId, const String& html)
{
    LocalFrame* frame = assertFrame(errorString, frameId);
    if (!frame)
        return;

    Document* document = frame->document();
    if (!document) {
        *errorString = "No Document instance to set HTML for";
        return;
    }
    DOMPatchSupport::patchDocument(*document, html);
}

void InspectorPageAgent::didClearDocumentOfWindowObject(LocalFrame* frame)
{
    if (!frontend())
        return;

    RefPtr<JSONObject> scripts = m_state->getObject(PageAgentState::pageAgentScriptsToEvaluateOnLoad);
    if (scripts) {
        for (const auto& script : *scripts) {
            String scriptText;
            if (script.value->asString(&scriptText))
                frame->script().executeScriptInMainWorld(scriptText);
        }
    }
    if (!m_scriptToEvaluateOnLoadOnce.isEmpty())
        frame->script().executeScriptInMainWorld(m_scriptToEvaluateOnLoadOnce);
}

void InspectorPageAgent::domContentLoadedEventFired(LocalFrame* frame)
{
    if (frame != inspectedFrame())
        return;
    frontend()->domContentEventFired(monotonicallyIncreasingTime());
}

void InspectorPageAgent::loadEventFired(LocalFrame* frame)
{
    if (frame != inspectedFrame())
        return;
    frontend()->loadEventFired(monotonicallyIncreasingTime());
}

void InspectorPageAgent::didCommitLoad(LocalFrame*, DocumentLoader* loader)
{
    if (loader->frame() == inspectedFrame()) {
        finishReload();
        m_scriptToEvaluateOnLoadOnce = m_pendingScriptToEvaluateOnLoadOnce;
        m_pendingScriptToEvaluateOnLoadOnce = String();
    }
    frontend()->frameNavigated(buildObjectForFrame(loader->frame()));
}

void InspectorPageAgent::frameAttachedToParent(LocalFrame* frame)
{
    Frame* parentFrame = frame->tree().parent();
    if (!parentFrame->isLocalFrame())
        parentFrame = 0;
    frontend()->frameAttached(frameId(frame), frameId(toLocalFrame(parentFrame)));
}

void InspectorPageAgent::frameDetachedFromParent(LocalFrame* frame)
{
    frontend()->frameDetached(frameId(frame));
}

FrameHost* InspectorPageAgent::frameHost()
{
    return m_inspectedFrame->host();
}

LocalFrame* InspectorPageAgent::frameForId(const String& frameId)
{
    LocalFrame* frame = IdentifiersFactory::frameById(frameId);
    return frame && frame->instrumentingAgents() == m_inspectedFrame->instrumentingAgents() ? frame : nullptr;
}

LocalFrame* InspectorPageAgent::findFrameWithSecurityOrigin(const String& originRawString)
{
    for (Frame* frame = inspectedFrame(); frame; frame = frame->tree().traverseNext(inspectedFrame())) {
        if (!frame->isLocalFrame())
            continue;
        RefPtr<SecurityOrigin> documentOrigin = toLocalFrame(frame)->document()->securityOrigin();
        if (documentOrigin->toRawString() == originRawString)
            return toLocalFrame(frame);
    }
    return nullptr;
}

LocalFrame* InspectorPageAgent::assertFrame(ErrorString* errorString, const String& frameId)
{
    LocalFrame* frame = frameForId(frameId);
    if (!frame)
        *errorString = "No frame for given id found";
    return frame;
}

bool InspectorPageAgent::screencastEnabled()
{
    return m_enabled && m_state->getBoolean(PageAgentState::screencastEnabled);
}

// static
DocumentLoader* InspectorPageAgent::assertDocumentLoader(ErrorString* errorString, LocalFrame* frame)
{
    DocumentLoader* documentLoader = frame->loader().documentLoader();
    if (!documentLoader)
        *errorString = "No documentLoader for given frame found";
    return documentLoader;
}

void InspectorPageAgent::frameStartedLoading(LocalFrame* frame)
{
    frontend()->frameStartedLoading(frameId(frame));
}

void InspectorPageAgent::frameStoppedLoading(LocalFrame* frame)
{
    frontend()->frameStoppedLoading(frameId(frame));
}

void InspectorPageAgent::frameScheduledNavigation(LocalFrame* frame, double delay)
{
    frontend()->frameScheduledNavigation(frameId(frame), delay);
}

void InspectorPageAgent::frameClearedScheduledNavigation(LocalFrame* frame)
{
    frontend()->frameClearedScheduledNavigation(frameId(frame));
}

void InspectorPageAgent::willRunJavaScriptDialog(const String& message, ChromeClient::DialogType dialogType)
{
    frontend()->javascriptDialogOpening(message, dialogTypeToProtocol(dialogType));
}

void InspectorPageAgent::didRunJavaScriptDialog(bool result)
{
    frontend()->javascriptDialogClosed(result);
}

void InspectorPageAgent::didLayout()
{
    if (!m_enabled)
        return;
    m_overlay->update();
}

void InspectorPageAgent::didScroll()
{
    if (m_enabled)
        m_overlay->update();
}

void InspectorPageAgent::didResizeMainFrame()
{
    if (!inspectedFrame()->isMainFrame())
        return;
#if !OS(ANDROID)
    if (m_enabled && m_state->getBoolean(PageAgentState::showSizeOnResize))
        m_overlay->showAndHideViewSize(m_state->getBoolean(PageAgentState::showGridOnResize));
#endif
    frontend()->frameResized();
}

PassRefPtr<TypeBuilder::Page::Frame> InspectorPageAgent::buildObjectForFrame(LocalFrame* frame)
{
    RefPtr<TypeBuilder::Page::Frame> frameObject = TypeBuilder::Page::Frame::create()
        .setId(frameId(frame))
        .setLoaderId(IdentifiersFactory::loaderId(frame->loader().documentLoader()))
        .setUrl(urlWithoutFragment(frame->document()->url()).string())
        .setMimeType(frame->loader().documentLoader()->responseMIMEType())
        .setSecurityOrigin(frame->document()->securityOrigin()->toRawString());
    // FIXME: This doesn't work for OOPI.
    Frame* parentFrame = frame->tree().parent();
    if (parentFrame && parentFrame->isLocalFrame())
        frameObject->setParentId(frameId(toLocalFrame(parentFrame)));
    if (frame->deprecatedLocalOwner()) {
        AtomicString name = frame->deprecatedLocalOwner()->getNameAttribute();
        if (name.isEmpty())
            name = frame->deprecatedLocalOwner()->getAttribute(HTMLNames::idAttr);
        frameObject->setName(name);
    }

    return frameObject;
}

PassRefPtr<TypeBuilder::Page::FrameResourceTree> InspectorPageAgent::buildObjectForFrameTree(LocalFrame* frame)
{
    RefPtr<TypeBuilder::Page::Frame> frameObject = buildObjectForFrame(frame);
    RefPtr<TypeBuilder::Array<TypeBuilder::Page::FrameResourceTree::Resources>> subresources = TypeBuilder::Array<TypeBuilder::Page::FrameResourceTree::Resources>::create();
    RefPtr<TypeBuilder::Page::FrameResourceTree> result = TypeBuilder::Page::FrameResourceTree::create()
        .setFrame(frameObject)
        .setResources(subresources);

    Vector<Resource*> allResources = cachedResourcesForFrame(frame, true);
    for (Resource* cachedResource : allResources) {
        RefPtr<TypeBuilder::Page::FrameResourceTree::Resources> resourceObject = TypeBuilder::Page::FrameResourceTree::Resources::create()
            .setUrl(urlWithoutFragment(cachedResource->url()).string())
            .setType(cachedResourceTypeJson(*cachedResource))
            .setMimeType(cachedResource->response().mimeType());
        if (cachedResource->wasCanceled())
            resourceObject->setCanceled(true);
        else if (cachedResource->status() == Resource::LoadError)
            resourceObject->setFailed(true);
        subresources->addItem(resourceObject);
    }

    Vector<Document*> allImports = InspectorPageAgent::importsForFrame(frame);
    for (Document* import : allImports) {
        RefPtr<TypeBuilder::Page::FrameResourceTree::Resources> resourceObject = TypeBuilder::Page::FrameResourceTree::Resources::create()
            .setUrl(urlWithoutFragment(import->url()).string())
            .setType(resourceTypeJson(InspectorPageAgent::DocumentResource))
            .setMimeType(import->suggestedMIMEType());
        subresources->addItem(resourceObject);
    }

    RefPtr<TypeBuilder::Array<TypeBuilder::Page::FrameResourceTree>> childrenArray;
    for (Frame* child = frame->tree().firstChild(); child; child = child->tree().nextSibling()) {
        if (!child->isLocalFrame())
            continue;
        if (!childrenArray) {
            childrenArray = TypeBuilder::Array<TypeBuilder::Page::FrameResourceTree>::create();
            result->setChildFrames(childrenArray);
        }
        childrenArray->addItem(buildObjectForFrameTree(toLocalFrame(child)));
    }
    return result;
}

void InspectorPageAgent::startScreencast(ErrorString*, const String* format, const int* quality, const int* maxWidth, const int* maxHeight)
{
    m_state->setBoolean(PageAgentState::screencastEnabled, true);
}

void InspectorPageAgent::stopScreencast(ErrorString*)
{
    m_state->setBoolean(PageAgentState::screencastEnabled, false);
}

void InspectorPageAgent::setShowViewportSizeOnResize(ErrorString*, bool show, const bool* showGrid)
{
    m_state->setBoolean(PageAgentState::showSizeOnResize, show);
    m_state->setBoolean(PageAgentState::showGridOnResize, asBool(showGrid));
}

void InspectorPageAgent::setOverlayMessage(ErrorString*, const String* message)
{
    m_overlay->setPausedInDebuggerMessage(message);
}

DEFINE_TRACE(InspectorPageAgent)
{
    visitor->trace(m_inspectedFrame);
    visitor->trace(m_debuggerAgent);
    visitor->trace(m_cssAgent);
    visitor->trace(m_overlay);
    visitor->trace(m_inspectorResourceContentLoader);
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink
