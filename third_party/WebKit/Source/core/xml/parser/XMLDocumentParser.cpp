/*
 * Copyright (C) 2000 Peter Kelly (pmk@post.com)
 * Copyright (C) 2005, 2006, 2008, 2014 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2007 Samuel Weinig (sam@webkit.org)
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Holger Hans Peter Freyther
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/xml/parser/XMLDocumentParser.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/V8Document.h"
#include "core/HTMLNames.h"
#include "core/XMLNSNames.h"
#include "core/dom/CDATASection.h"
#include "core/dom/Comment.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentType.h"
#include "core/dom/ProcessingInstruction.h"
#include "core/dom/ScriptLoader.h"
#include "core/dom/TransformSource.h"
#include "core/fetch/FetchInitiatorTypeNames.h"
#include "core/fetch/RawResource.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/fetch/ScriptResource.h"
#include "core/frame/ConsoleTypes.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/parser/HTMLEntityParser.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/ImageLoader.h"
#include "core/svg/graphics/SVGImage.h"
#include "core/xml/DocumentXSLT.h"
#include "core/xml/parser/SharedBufferReader.h"
#include "core/xml/parser/XMLDocumentParserScope.h"
#include "core/xml/parser/XMLParserInput.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/SharedBuffer.h"
#include "platform/TraceEvent.h"
#include "platform/network/ResourceError.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/StringExtras.h"
#include "wtf/TemporaryChange.h"
#include "wtf/Threading.h"
#include "wtf/Vector.h"
#include "wtf/text/UTF8.h"
#include <libxml/catalog.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
//#include <libxslt/xslt.h>

namespace blink {

using namespace HTMLNames;

// FIXME: HTMLConstructionSite has a limit of 512, should these match?
static const unsigned maxXMLTreeDepth = 5000;

static inline String toString(const xmlChar* string, size_t length)
{
    return String::fromUTF8(reinterpret_cast<const char*>(string), length);
}

static inline String toString(const xmlChar* string)
{
    return String::fromUTF8(reinterpret_cast<const char*>(string));
}

static inline AtomicString toAtomicString(const xmlChar* string, size_t length)
{
    return AtomicString::fromUTF8(reinterpret_cast<const char*>(string), length);
}

static inline AtomicString toAtomicString(const xmlChar* string)
{
    return AtomicString::fromUTF8(reinterpret_cast<const char*>(string));
}

static inline bool hasNoStyleInformation(Document* document)
{
    if (document->sawElementsInKnownNamespaces() /*|| DocumentXSLT::hasTransformSourceDocument(*document)*/)
        return false;

    if (!document->frame() || !document->frame()->page())
        return false;

    if (document->frame()->tree().parent())
        return false; // This document is not in a top frame

    if (SVGImage::isInSVGImage(document))
        return false;

    return true;
}

class PendingStartElementNSCallback final : public XMLDocumentParser::PendingCallback {
public:
    PendingStartElementNSCallback(const AtomicString& localName, const AtomicString& prefix, const AtomicString& uri,
        int namespaceCount, const xmlChar** namespaces, int attributeCount, int defaultedCount, const xmlChar** attributes)
        : m_localName(localName)
        , m_prefix(prefix)
        , m_uri(uri)
        , m_namespaceCount(namespaceCount)
        , m_attributeCount(attributeCount)
        , m_defaultedCount(defaultedCount)
    {
        m_namespaces = static_cast<xmlChar**>(xmlMalloc(sizeof(xmlChar*) * namespaceCount * 2));
        for (int i = 0; i < namespaceCount * 2 ; ++i)
            m_namespaces[i] = xmlStrdup(namespaces[i]);
        m_attributes = static_cast<xmlChar**>(xmlMalloc(sizeof(xmlChar*) * attributeCount * 5));
        for (int i = 0; i < attributeCount; ++i) {
            // Each attribute has 5 elements in the array:
            // name, prefix, uri, value and an end pointer.
            for (int j = 0; j < 3; ++j)
                m_attributes[i * 5 + j] = xmlStrdup(attributes[i * 5 + j]);
            int length = attributes[i * 5 + 4] - attributes[i * 5 + 3];
            m_attributes[i * 5 + 3] = xmlStrndup(attributes[i * 5 + 3], length);
            m_attributes[i * 5 + 4] = m_attributes[i * 5 + 3] + length;
        }
    }

    ~PendingStartElementNSCallback() override
    {
        for (int i = 0; i < m_namespaceCount * 2; ++i)
            xmlFree(m_namespaces[i]);
        xmlFree(m_namespaces);
        for (int i = 0; i < m_attributeCount; ++i)
            for (int j = 0; j < 4; ++j)
                xmlFree(m_attributes[i * 5 + j]);
        xmlFree(m_attributes);
    }

    void call(XMLDocumentParser* parser) override
    {
        parser->startElementNs(m_localName, m_prefix, m_uri,
            m_namespaceCount, const_cast<const xmlChar**>(m_namespaces),
            m_attributeCount, m_defaultedCount, const_cast<const xmlChar**>(m_attributes));
    }

private:
    AtomicString m_localName;
    AtomicString m_prefix;
    AtomicString m_uri;
    int m_namespaceCount;
    xmlChar** m_namespaces;
    int m_attributeCount;
    int m_defaultedCount;
    xmlChar** m_attributes;
};

class PendingEndElementNSCallback final : public XMLDocumentParser::PendingCallback {
public:
    void call(XMLDocumentParser* parser) override
    {
        parser->endElementNs();
    }
};

class PendingCharactersCallback final : public XMLDocumentParser::PendingCallback {
public:
    PendingCharactersCallback(const xmlChar* chars, int length)
        : m_chars(xmlStrndup(chars, length))
        , m_length(length)
    {
    }

    ~PendingCharactersCallback() override
    {
        xmlFree(m_chars);
    }

    void call(XMLDocumentParser* parser) override
    {
        parser->characters(m_chars, m_length);
    }

private:
    xmlChar* m_chars;
    int m_length;
};

class PendingProcessingInstructionCallback final : public XMLDocumentParser::PendingCallback {
public:
    PendingProcessingInstructionCallback(const String& target, const String& data)
        : m_target(target)
        , m_data(data)
    {
    }

    void call(XMLDocumentParser* parser) override
    {
        parser->processingInstruction(m_target, m_data);
    }

private:
    String m_target;
    String m_data;
};

class PendingCDATABlockCallback final : public XMLDocumentParser::PendingCallback {
public:
    explicit PendingCDATABlockCallback(const String& text) : m_text(text) { }

    void call(XMLDocumentParser* parser) override
    {
        parser->cdataBlock(m_text);
    }

private:
    String m_text;
};

class PendingCommentCallback final : public XMLDocumentParser::PendingCallback {
public:
    explicit PendingCommentCallback(const String& text) : m_text(text) { }

    void call(XMLDocumentParser* parser) override
    {
        parser->comment(m_text);
    }

private:
    String m_text;
};

class PendingInternalSubsetCallback final : public XMLDocumentParser::PendingCallback {
public:
    PendingInternalSubsetCallback(const String& name, const String& externalID, const String& systemID)
        : m_name(name)
        , m_externalID(externalID)
        , m_systemID(systemID)
    {
    }

    void call(XMLDocumentParser* parser) override
    {
        parser->internalSubset(m_name, m_externalID, m_systemID);
    }

private:
    String m_name;
    String m_externalID;
    String m_systemID;
};

class PendingErrorCallback final : public XMLDocumentParser::PendingCallback {
public:
    PendingErrorCallback(XMLErrors::ErrorType type, const xmlChar* message, OrdinalNumber lineNumber, OrdinalNumber columnNumber)
        : m_type(type)
        , m_message(xmlStrdup(message))
        , m_lineNumber(lineNumber)
        , m_columnNumber(columnNumber)
    {
    }

    ~PendingErrorCallback() override
    {
        xmlFree(m_message);
    }

    void call(XMLDocumentParser* parser) override
    {
        parser->handleError(m_type, reinterpret_cast<char*>(m_message), TextPosition(m_lineNumber, m_columnNumber));
    }

private:
    XMLErrors::ErrorType m_type;
    xmlChar* m_message;
    OrdinalNumber m_lineNumber;
    OrdinalNumber m_columnNumber;
};

void XMLDocumentParser::pushCurrentNode(ContainerNode* n)
{
    ASSERT(n);
    ASSERT(m_currentNode);
#if !ENABLE(OILPAN)
    if (n != document())
        n->ref();
#endif
    m_currentNodeStack.append(m_currentNode);
    m_currentNode = n;
    if (m_currentNodeStack.size() > maxXMLTreeDepth)
        handleError(XMLErrors::ErrorTypeFatal, "Excessive node nesting.", textPosition());
}

void XMLDocumentParser::popCurrentNode()
{
    if (!m_currentNode)
        return;
    ASSERT(m_currentNodeStack.size());
#if !ENABLE(OILPAN)
    if (m_currentNode != document())
        m_currentNode->deref();
#endif
    m_currentNode = m_currentNodeStack.last();
    m_currentNodeStack.removeLast();
}

void XMLDocumentParser::clearCurrentNodeStack()
{
#if !ENABLE(OILPAN)
    if (m_currentNode && m_currentNode != document())
        m_currentNode->deref();
#endif
    m_currentNode = nullptr;
    m_leafTextNode = nullptr;

    if (m_currentNodeStack.size()) { // Aborted parsing.
#if !ENABLE(OILPAN)
        for (size_t i = m_currentNodeStack.size() - 1; i != 0; --i)
            m_currentNodeStack[i]->deref();
        if (m_currentNodeStack[0] && m_currentNodeStack[0] != document())
            m_currentNodeStack[0]->deref();
#endif
        m_currentNodeStack.clear();
    }
}

void XMLDocumentParser::insert(const SegmentedString&)
{
    ASSERT_NOT_REACHED();
}

void XMLDocumentParser::append(const String& inputSource)
{
    const SegmentedString source(inputSource);
    if (m_sawXSLTransform || !m_sawFirstElement)
        m_originalSourceForTransform.append(source);

    if (isStopped() || m_sawXSLTransform)
        return;

    if (m_parserPaused) {
        m_pendingSrc.append(source);
        return;
    }

    // JavaScript can detach the parser. Make sure this is not released
    // before the end of this method.
    RefPtrWillBeRawPtr<XMLDocumentParser> protect(this);

    doWrite(source.toString());
}

void XMLDocumentParser::handleError(XMLErrors::ErrorType type, const char* formattedMessage, TextPosition position)
{
    m_xmlErrors.handleError(type, formattedMessage, position);
    if (type != XMLErrors::ErrorTypeWarning)
        m_sawError = true;
    if (type == XMLErrors::ErrorTypeFatal)
        stopParsing();
}

void XMLDocumentParser::enterText()
{
    ASSERT(m_bufferedText.size() == 0);
    ASSERT(!m_leafTextNode);
    m_leafTextNode = Text::create(m_currentNode->document(), "");
    m_currentNode->parserAppendChild(m_leafTextNode.get());
}

void XMLDocumentParser::exitText()
{
    if (isStopped())
        return;

    if (!m_leafTextNode)
        return;

    m_leafTextNode->appendData(toString(m_bufferedText.data(), m_bufferedText.size()));
    m_bufferedText.clear();
    m_leafTextNode = nullptr;
}

void XMLDocumentParser::detach()
{
    clearCurrentNodeStack();
    ScriptableDocumentParser::detach();
}

void XMLDocumentParser::end()
{
    TRACE_EVENT0("blink", "XMLDocumentParser::end");
    // XMLDocumentParserLibxml2 will do bad things to the document if doEnd() is called.
    // I don't believe XMLDocumentParserQt needs doEnd called in the fragment case.
    ASSERT(!m_parsingFragment);

    doEnd();

    // doEnd() call above can detach the parser and null out its document.
    // In that case, we just bail out.
    if (isDetached())
        return;

    // doEnd() could process a script tag, thus pausing parsing.
    if (m_parserPaused)
        return;

    if (m_sawError) {
        insertErrorMessageBlock();
    } else {
        exitText();
        document()->styleResolverChanged();
    }

    if (isParsing())
        prepareToStopParsing();
    document()->setReadyState(Document::Interactive);
    clearCurrentNodeStack();
    document()->finishedParsing();
}

void XMLDocumentParser::finish()
{
    // FIXME: We should ASSERT(!m_parserStopped) here, since it does not
    // makes sense to call any methods on DocumentParser once it's been stopped.
    // However, FrameLoader::stop calls DocumentParser::finish unconditionally.

    // flush may ending up executing arbitrary script, and possibly detach the parser.
    RefPtrWillBeRawPtr<XMLDocumentParser> protect(this);
    flush();
    if (isDetached())
        return;

    if (m_parserPaused)
        m_finishCalled = true;
    else
        end();
}

void XMLDocumentParser::insertErrorMessageBlock()
{
    m_xmlErrors.insertErrorMessageBlock();
}

void XMLDocumentParser::notifyFinished(Resource* unusedResource)
{
    ASSERT_UNUSED(unusedResource, unusedResource == m_pendingScript);

    ScriptSourceCode sourceCode(m_pendingScript.get());
    bool errorOccurred = m_pendingScript->errorOccurred();
    bool wasCanceled = m_pendingScript->wasCanceled();

    m_pendingScript->removeClient(this);
    m_pendingScript = 0;

    RefPtrWillBeRawPtr<Element> e = m_scriptElement;
    m_scriptElement = nullptr;

    ScriptLoader* scriptLoader = toScriptLoaderIfPossible(e.get());
    ASSERT(scriptLoader);

    // JavaScript can detach this parser, make sure it's kept alive even if
    // detached.
    RefPtrWillBeRawPtr<XMLDocumentParser> protect(this);

    if (errorOccurred) {
        scriptLoader->dispatchErrorEvent();
    } else if (!wasCanceled) {
        if (!scriptLoader->executeScript(sourceCode))
            scriptLoader->dispatchErrorEvent();
        else
            scriptLoader->dispatchLoadEvent();
    }

    m_scriptElement = nullptr;

    if (!isDetached() && !m_requestingScript)
        resumeParsing();
}

bool XMLDocumentParser::isWaitingForScripts() const
{
    return m_pendingScript;
}

void XMLDocumentParser::pauseParsing()
{
    if (!m_parsingFragment)
        m_parserPaused = true;
}

bool XMLDocumentParser::parseDocumentFragment(const String& chunk, DocumentFragment* fragment, Element* contextElement, ParserContentPolicy parserContentPolicy)
{
    if (!chunk.length())
        return true;

    // FIXME: We need to implement the HTML5 XML Fragment parsing algorithm:
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/the-xhtml-syntax.html#xml-fragment-parsing-algorithm
    // For now we have a hack for script/style innerHTML support:
    if (contextElement && (contextElement->hasLocalName(scriptTag.localName()) || contextElement->hasLocalName(styleTag.localName()))) {
        fragment->parserAppendChild(fragment->document().createTextNode(chunk));
        return true;
    }

    RefPtrWillBeRawPtr<XMLDocumentParser> parser = XMLDocumentParser::create(fragment, contextElement, parserContentPolicy);
    bool wellFormed = parser->appendFragmentSource(chunk);

    // Do not call finish(). Current finish() and doEnd() implementations touch
    // the main Document/loader and can cause crashes in the fragment case.

    // Allows ~DocumentParser to assert it was detached before destruction.
    parser->detach();
    // appendFragmentSource()'s wellFormed is more permissive than wellFormed().
    return wellFormed;
}

static int globalDescriptor = 0;
static ThreadIdentifier libxmlLoaderThread = 0;

static int matchFunc(const char*)
{
    // Only match loads initiated due to uses of libxml2 from within
    // XMLDocumentParser to avoid interfering with client applications that also
    // use libxml2. http://bugs.webkit.org/show_bug.cgi?id=17353
    return XMLDocumentParserScope::currentDocument && currentThread() == libxmlLoaderThread;
}

static inline void setAttributes(Element* element, Vector<Attribute>& attributeVector, ParserContentPolicy parserContentPolicy)
{
    if (!scriptingContentIsAllowed(parserContentPolicy))
        element->stripScriptingAttributes(attributeVector);
    element->parserSetAttributes(attributeVector);
}

static void switchEncoding(xmlParserCtxtPtr ctxt, bool is8Bit)
{
    // Hack around libxml2's lack of encoding overide support by manually
    // resetting the encoding to UTF-16 before every chunk. Otherwise libxml
    // will detect <?xml version="1.0" encoding="<encoding name>"?> blocks and
    // switch encodings, causing the parse to fail.
    if (is8Bit) {
        xmlSwitchEncoding(ctxt, XML_CHAR_ENCODING_8859_1);
        return;
    }

    const UChar BOM = 0xFEFF;
    const unsigned char BOMHighByte = *reinterpret_cast<const unsigned char*>(&BOM);
    xmlSwitchEncoding(ctxt, BOMHighByte == 0xFF ? XML_CHAR_ENCODING_UTF16LE : XML_CHAR_ENCODING_UTF16BE);
}

static void parseChunk(xmlParserCtxtPtr ctxt, const String& chunk)
{
    bool is8Bit = chunk.is8Bit();
    switchEncoding(ctxt, is8Bit);
    if (is8Bit)
        xmlParseChunk(ctxt, reinterpret_cast<const char*>(chunk.characters8()), sizeof(LChar) * chunk.length(), 0);
    else
        xmlParseChunk(ctxt, reinterpret_cast<const char*>(chunk.characters16()), sizeof(UChar) * chunk.length(), 0);
}

static void finishParsing(xmlParserCtxtPtr ctxt)
{
    xmlParseChunk(ctxt, 0, 0, 1);
}

#define xmlParseChunk #error "Use parseChunk instead to select the correct encoding."

static bool isLibxmlDefaultCatalogFile(const String& urlString)
{
    // On non-Windows platforms libxml asks for this URL, the
    // "XML_XML_DEFAULT_CATALOG", on initialization.
    if (urlString == "file:///etc/xml/catalog")
        return true;

    // On Windows, libxml computes a URL relative to where its DLL resides.
    if (urlString.startsWith("file:///", TextCaseInsensitive) && urlString.endsWith("/etc/catalog", TextCaseInsensitive))
        return true;
    return false;
}

static bool shouldAllowExternalLoad(const KURL& url)
{
    String urlString = url.string();

    // This isn't really necessary now that initializeLibXMLIfNecessary
    // disables catalog support in libxml, but keeping it for defense in depth.
    if (isLibxmlDefaultCatalogFile(url))
        return false;

    // The most common DTD. There isn't much point in hammering www.w3c.org by
    // requesting this URL for every XHTML document.
    if (urlString.startsWith("http://www.w3.org/TR/xhtml", TextCaseInsensitive))
        return false;

    // Similarly, there isn't much point in requesting the SVG DTD.
    if (urlString.startsWith("http://www.w3.org/Graphics/SVG", TextCaseInsensitive))
        return false;

    // The libxml doesn't give us a lot of context for deciding whether to allow
    // this request. In the worst case, this load could be for an external
    // entity and the resulting document could simply read the retrieved
    // content. If we had more context, we could potentially allow the parser to
    // load a DTD. As things stand, we take the conservative route and allow
    // same-origin requests only.
    if (!XMLDocumentParserScope::currentDocument->securityOrigin()->canRequest(url)) {
        // FIXME: This is copy/pasted. We should probably build console logging into canRequest().
        if (!url.isNull()) {
            String message = "Unsafe attempt to load URL " + url.elidedString() +
                " from frame with URL " + XMLDocumentParserScope::currentDocument->url().elidedString() +
                ". Domains, protocols and ports must match.\n";
            XMLDocumentParserScope::currentDocument->addConsoleMessage(ConsoleMessage::create(SecurityMessageSource, ErrorMessageLevel, message));
        }
        return false;
    }

    return true;
}

static void* openFunc(const char* uri)
{
    ASSERT(XMLDocumentParserScope::currentDocument);
    ASSERT(currentThread() == libxmlLoaderThread);

    KURL url(KURL(), uri);

    if (!shouldAllowExternalLoad(url))
        return &globalDescriptor;

    KURL finalURL;
    RefPtr<SharedBuffer> data;

    {
        Document* document = XMLDocumentParserScope::currentDocument;
        XMLDocumentParserScope scope(0);
        // FIXME: We should restore the original global error handler as well.
        FetchRequest request(ResourceRequest(url), FetchInitiatorTypeNames::xml, ResourceFetcher::defaultResourceOptions());
        ResourcePtr<Resource> resource = RawResource::fetchSynchronously(request, document->fetcher());
        if (resource && !resource->errorOccurred()) {
            data = resource->resourceBuffer();
            finalURL = resource->response().url();
        }
    }

    // We have to check the URL again after the load to catch redirects.
    // See <https://bugs.webkit.org/show_bug.cgi?id=21963>.
    if (!shouldAllowExternalLoad(finalURL))
        return &globalDescriptor;

    UseCounter::count(XMLDocumentParserScope::currentDocument, UseCounter::XMLExternalResourceLoad);

    return new SharedBufferReader(data);
}

static int readFunc(void* context, char* buffer, int len)
{
    // Do 0-byte reads in case of a null descriptor
    if (context == &globalDescriptor)
        return 0;

    SharedBufferReader* data = static_cast<SharedBufferReader*>(context);
    return data->readData(buffer, len);
}

static int writeFunc(void*, const char*, int)
{
    // Always just do 0-byte writes
    return 0;
}

static int closeFunc(void* context)
{
    if (context != &globalDescriptor) {
        SharedBufferReader* data = static_cast<SharedBufferReader*>(context);
        delete data;
    }
    return 0;
}

static void errorFunc(void*, const char*, ...)
{
    // FIXME: It would be nice to display error messages somewhere.
}

static void initializeLibXMLIfNecessary()
{
    static bool didInit = false;
    if (didInit)
        return;

    // We don't want libxml to try and load catalogs.
    // FIXME: It's not nice to set global settings in libxml, embedders of Blink
    // could be trying to use libxml themselves.
#ifdef MINIBLINK_NOT_IMPLEMENTED
    xmlCatalogSetDefaults(XML_CATA_ALLOW_NONE);
#endif
    xmlInitParser();
    xmlRegisterInputCallbacks(matchFunc, openFunc, readFunc, closeFunc);
#ifdef MINIBLINK_NOT_IMPLEMENTED
    xmlRegisterOutputCallbacks(matchFunc, openFunc, writeFunc, closeFunc);
#endif
    libxmlLoaderThread = currentThread();
    didInit = true;
}


PassRefPtr<XMLParserContext> XMLParserContext::createStringParser(xmlSAXHandlerPtr handlers, void* userData)
{
    initializeLibXMLIfNecessary();
    xmlParserCtxtPtr parser = xmlCreatePushParserCtxt(handlers, 0, 0, 0, 0);
    parser->_private = userData;
    parser->replaceEntities = true;
    return adoptRef(new XMLParserContext(parser));
}

// Chunk should be encoded in UTF-8
PassRefPtr<XMLParserContext> XMLParserContext::createMemoryParser(xmlSAXHandlerPtr handlers, void* userData, const CString& chunk)
{
    initializeLibXMLIfNecessary();

    // appendFragmentSource() checks that the length doesn't overflow an int.
    xmlParserCtxtPtr parser = xmlCreateMemoryParserCtxt(chunk.data(), chunk.length());

    if (!parser)
        return nullptr;

    // Copy the sax handler
    memcpy(parser->sax, handlers, sizeof(xmlSAXHandler));

    // Set parser options.
    // XML_PARSE_NODICT: default dictionary option.
    // XML_PARSE_NOENT: force entities substitutions.
    xmlCtxtUseOptions(parser, XML_PARSE_NODICT | XML_PARSE_NOENT);

    // Internal initialization
    parser->sax2 = 1;
    parser->instate = XML_PARSER_CONTENT; // We are parsing a CONTENT
    parser->depth = 0;
    parser->str_xml = xmlDictLookup(parser->dict, BAD_CAST "xml", 3);
    parser->str_xmlns = xmlDictLookup(parser->dict, BAD_CAST "xmlns", 5);
    parser->str_xml_ns = xmlDictLookup(parser->dict, XML_XML_NAMESPACE, 36);
    parser->_private = userData;

    return adoptRef(new XMLParserContext(parser));
}

// --------------------------------

bool XMLDocumentParser::supportsXMLVersion(const String& version)
{
    return version == "1.0";
}

XMLDocumentParser::XMLDocumentParser(Document& document, FrameView* frameView)
    : ScriptableDocumentParser(document)
    , m_hasView(frameView)
    , m_context(nullptr)
    , m_currentNode(&document)
    , m_isCurrentlyParsing8BitChunk(false)
    , m_sawError(false)
    , m_sawCSS(false)
    , m_sawXSLTransform(false)
    , m_sawFirstElement(false)
    , m_isXHTMLDocument(false)
    , m_parserPaused(false)
    , m_requestingScript(false)
    , m_finishCalled(false)
    , m_xmlErrors(&document)
    , m_pendingScript(0)
    , m_scriptStartPosition(TextPosition::belowRangePosition())
    , m_parsingFragment(false)
{
    // This is XML being used as a document resource.
    if (frameView && document.isXMLDocument())
        UseCounter::count(document, UseCounter::XMLDocument);
}

XMLDocumentParser::XMLDocumentParser(DocumentFragment* fragment, Element* parentElement, ParserContentPolicy parserContentPolicy)
    : ScriptableDocumentParser(fragment->document(), parserContentPolicy)
    , m_hasView(false)
    , m_context(nullptr)
    , m_currentNode(fragment)
    , m_isCurrentlyParsing8BitChunk(false)
    , m_sawError(false)
    , m_sawCSS(false)
    , m_sawXSLTransform(false)
    , m_sawFirstElement(false)
    , m_isXHTMLDocument(false)
    , m_parserPaused(false)
    , m_requestingScript(false)
    , m_finishCalled(false)
    , m_xmlErrors(&fragment->document())
    , m_pendingScript(0)
    , m_scriptStartPosition(TextPosition::belowRangePosition())
    , m_parsingFragment(true)
{
#if !ENABLE(OILPAN)
    fragment->ref();
#endif

    // Add namespaces based on the parent node
    WillBeHeapVector<RawPtrWillBeMember<Element>> elemStack;
    while (parentElement) {
        elemStack.append(parentElement);

        Element* grandParentElement = parentElement->parentElement();
        if (!grandParentElement)
            break;
        parentElement = grandParentElement;
    }

    if (elemStack.isEmpty())
        return;

    for (; !elemStack.isEmpty(); elemStack.removeLast()) {
        Element* element = elemStack.last();
        AttributeCollection attributes = element->attributes();
        for (auto& attribute : attributes) {
            if (attribute.localName() == xmlnsAtom)
                m_defaultNamespaceURI = attribute.value();
            else if (attribute.prefix() == xmlnsAtom)
                m_prefixToNamespaceMap.set(attribute.localName(), attribute.value());
        }
    }

    // If the parent element is not in document tree, there may be no xmlns attribute; just default to the parent's namespace.
    if (m_defaultNamespaceURI.isNull() && !parentElement->inDocument())
        m_defaultNamespaceURI = parentElement->namespaceURI();
}

XMLParserContext::~XMLParserContext()
{
    if (m_context->myDoc)
        xmlFreeDoc(m_context->myDoc);
    xmlFreeParserCtxt(m_context);
}

XMLDocumentParser::~XMLDocumentParser()
{
#if !ENABLE(OILPAN)
    // The XMLDocumentParser will always be detached before being destroyed.
    ASSERT(m_currentNodeStack.isEmpty());
    ASSERT(!m_currentNode);
#endif

    // FIXME: m_pendingScript handling should be moved into XMLDocumentParser.cpp!
    if (m_pendingScript)
        m_pendingScript->removeClient(this);
}

DEFINE_TRACE(XMLDocumentParser)
{
    visitor->trace(m_currentNode);
#if ENABLE(OILPAN)
    visitor->trace(m_currentNodeStack);
#endif
    visitor->trace(m_leafTextNode);
    visitor->trace(m_xmlErrors);
    visitor->trace(m_scriptElement);
    ScriptableDocumentParser::trace(visitor);
}

void XMLDocumentParser::doWrite(const String& parseString)
{
    TRACE_EVENT0("blink", "XMLDocumentParser::doWrite");
    ASSERT(!isDetached());
    if (!m_context)
        initializeParserContext();

    // Protect the libxml context from deletion during a callback
    RefPtr<XMLParserContext> context = m_context;

    // libXML throws an error if you try to switch the encoding for an empty
    // string.
    if (parseString.length()) {
        // JavaScript may cause the parser to detach during parseChunk
        // keep this alive until this function is done.
        RefPtrWillBeRawPtr<XMLDocumentParser> protect(this);

        XMLDocumentParserScope scope(document());
        TemporaryChange<bool> encodingScope(m_isCurrentlyParsing8BitChunk, parseString.is8Bit());
        parseChunk(context->context(), parseString);

        // JavaScript (which may be run under the parseChunk callstack) may
        // cause the parser to be stopped or detached.
        if (isStopped())
            return;
    }

    // FIXME: Why is this here? And why is it after we process the passed
    // source?
    if (document()->sawDecodingError()) {
        // If the decoder saw an error, report it as fatal (stops parsing)
        TextPosition position(OrdinalNumber::fromOneBasedInt(context->context()->input->line), OrdinalNumber::fromOneBasedInt(context->context()->input->col));
        handleError(XMLErrors::ErrorTypeFatal, "Encoding error", position);
    }
}

struct xmlSAX2Namespace {
    const xmlChar* prefix;
    const xmlChar* uri;
};

static inline void handleNamespaceAttributes(Vector<Attribute>& prefixedAttributes, const xmlChar** libxmlNamespaces, int nbNamespaces, ExceptionState& exceptionState)
{
    xmlSAX2Namespace* namespaces = reinterpret_cast<xmlSAX2Namespace*>(libxmlNamespaces);
    for (int i = 0; i < nbNamespaces; ++i) {
        AtomicString namespaceQName = xmlnsAtom;
        AtomicString namespaceURI = toAtomicString(namespaces[i].uri);
        if (namespaces[i].prefix)
            namespaceQName = WTF::xmlnsWithColon + toAtomicString(namespaces[i].prefix);

        QualifiedName parsedName = anyName;
        if (!Element::parseAttributeName(parsedName, XMLNSNames::xmlnsNamespaceURI, namespaceQName, exceptionState))
            return;

        prefixedAttributes.append(Attribute(parsedName, namespaceURI));
    }
}

struct xmlSAX2Attributes {
    const xmlChar* localname;
    const xmlChar* prefix;
    const xmlChar* uri;
    const xmlChar* value;
    const xmlChar* end;
};

static inline void handleElementAttributes(Vector<Attribute>& prefixedAttributes, const xmlChar** libxmlAttributes, int nbAttributes,
    const HashMap<AtomicString, AtomicString>& initialPrefixToNamespaceMap, ExceptionState& exceptionState)
{
    xmlSAX2Attributes* attributes = reinterpret_cast<xmlSAX2Attributes*>(libxmlAttributes);
    for (int i = 0; i < nbAttributes; ++i) {
        int valueLength = static_cast<int>(attributes[i].end - attributes[i].value);
        AtomicString attrValue = toAtomicString(attributes[i].value, valueLength);
        AtomicString attrPrefix = toAtomicString(attributes[i].prefix);
        AtomicString attrURI;
        if (!attrPrefix.isEmpty()) {
            // If provided, use the namespace URI from libxml2 because libxml2
            // updates its namespace table as it parses whereas the
            // initialPrefixToNamespaceMap is the initial map from namespace
            // prefixes to namespace URIs created by the XMLDocumentParser
            // constructor (in the case where we are parsing an XML fragment).
            if (attributes[i].uri) {
                attrURI = toAtomicString(attributes[i].uri);
            } else {
                const HashMap<AtomicString, AtomicString>::const_iterator it = initialPrefixToNamespaceMap.find(attrPrefix);
                if (it != initialPrefixToNamespaceMap.end())
                    attrURI = it->value;
                else
                    attrURI = AtomicString();
            }
        }
        AtomicString attrQName = attrPrefix.isEmpty() ? toAtomicString(attributes[i].localname) : attrPrefix + ":" + toString(attributes[i].localname);

        QualifiedName parsedName = anyName;
        if (!Element::parseAttributeName(parsedName, attrURI, attrQName, exceptionState))
            return;

        prefixedAttributes.append(Attribute(parsedName, attrValue));
    }
}

void XMLDocumentParser::startElementNs(const AtomicString& localName, const AtomicString& prefix, const AtomicString& uri, int nbNamespaces,
    const xmlChar** libxmlNamespaces, int nbAttributes, int nbDefaulted, const xmlChar** libxmlAttributes)
{
    if (isStopped())
        return;

    if (m_parserPaused) {
        m_pendingCallbacks.append(adoptPtr(new PendingStartElementNSCallback(localName, prefix, uri, nbNamespaces, libxmlNamespaces,
            nbAttributes, nbDefaulted, libxmlAttributes)));
        return;
    }

    exitText();

    AtomicString adjustedURI = uri;
    if (m_parsingFragment && adjustedURI.isNull()) {
        if (!prefix.isNull())
            adjustedURI = m_prefixToNamespaceMap.get(prefix);
        else
            adjustedURI = m_defaultNamespaceURI;
    }

    bool isFirstElement = !m_sawFirstElement;
    m_sawFirstElement = true;

    QualifiedName qName(prefix, localName, adjustedURI);
    RefPtrWillBeRawPtr<Element> newElement = m_currentNode->document().createElement(qName, true);
    if (!newElement) {
        stopParsing();
        return;
    }

    Vector<Attribute> prefixedAttributes;
    TrackExceptionState exceptionState;
    handleNamespaceAttributes(prefixedAttributes, libxmlNamespaces, nbNamespaces, exceptionState);
    if (exceptionState.hadException()) {
        setAttributes(newElement.get(), prefixedAttributes, parserContentPolicy());
        stopParsing();
        return;
    }

    handleElementAttributes(prefixedAttributes, libxmlAttributes, nbAttributes, m_prefixToNamespaceMap, exceptionState);
    setAttributes(newElement.get(), prefixedAttributes, parserContentPolicy());
    if (exceptionState.hadException()) {
        stopParsing();
        return;
    }

    newElement->beginParsingChildren();

    ScriptLoader* scriptLoader = toScriptLoaderIfPossible(newElement.get());
    if (scriptLoader)
        m_scriptStartPosition = textPosition();

    m_currentNode->parserAppendChild(newElement.get());

    // Event handlers may synchronously trigger removal of the
    // document and cancellation of this parser.
    if (isStopped()) {
        stopParsing();
        return;
    }

    if (isHTMLTemplateElement(*newElement))
        pushCurrentNode(toHTMLTemplateElement(*newElement).content());
    else
        pushCurrentNode(newElement.get());

    if (isHTMLHtmlElement(*newElement))
        toHTMLHtmlElement(*newElement).insertedByParser();

    if (!m_parsingFragment && isFirstElement && document()->frame())
        document()->frame()->loader().dispatchDocumentElementAvailable();
}

void XMLDocumentParser::endElementNs()
{
    if (isStopped())
        return;

    if (m_parserPaused) {
        m_pendingCallbacks.append(adoptPtr(new PendingEndElementNSCallback()));
        return;
    }

    // JavaScript can detach the parser. Make sure this is not released before
    // the end of this method.
    RefPtrWillBeRawPtr<XMLDocumentParser> protect(this);

    exitText();

    RefPtrWillBeRawPtr<ContainerNode> n = m_currentNode;
    if (m_currentNode->isElementNode())
        toElement(n.get())->finishParsingChildren();

    if (!scriptingContentIsAllowed(parserContentPolicy()) && n->isElementNode() && toScriptLoaderIfPossible(toElement(n))) {
        popCurrentNode();
        n->remove(IGNORE_EXCEPTION);
        return;
    }

    if (!n->isElementNode() || !m_hasView) {
        popCurrentNode();
        return;
    }

    Element* element = toElement(n);

    // The element's parent may have already been removed from document.
    // Parsing continues in this case, but scripts aren't executed.
    if (!element->inDocument()) {
        popCurrentNode();
        return;
    }

    ScriptLoader* scriptLoader = toScriptLoaderIfPossible(element);
    if (!scriptLoader) {
        popCurrentNode();
        return;
    }

    // Don't load external scripts for standalone documents (for now).
    ASSERT(!m_pendingScript);
    m_requestingScript = true;

    if (scriptLoader->prepareScript(m_scriptStartPosition, ScriptLoader::AllowLegacyTypeInTypeAttribute)) {
        // FIXME: Script execution should be shared between
        // the libxml2 and Qt XMLDocumentParser implementations.


        if (scriptLoader->readyToBeParserExecuted()) {
            if (!scriptLoader->executeScript(ScriptSourceCode(scriptLoader->scriptContent(), document()->url(), m_scriptStartPosition))) {
                scriptLoader->dispatchErrorEvent();
                return;
            }
        } else if (scriptLoader->willBeParserExecuted()) {
            m_pendingScript = scriptLoader->resource();
            m_scriptElement = element;
            m_pendingScript->addClient(this);
            // m_pendingScript will be 0 if script was already loaded and
            // addClient() executed it.
            if (m_pendingScript)
                pauseParsing();
        } else {
            m_scriptElement = nullptr;
        }

        // JavaScript may have detached the parser
        if (isDetached())
            return;
    }
    m_requestingScript = false;
    popCurrentNode();
}

void XMLDocumentParser::characters(const xmlChar* chars, int length)
{
    if (isStopped())
        return;

    if (m_parserPaused) {
        m_pendingCallbacks.append(adoptPtr(new PendingCharactersCallback(chars, length)));
        return;
    }

    if (!m_leafTextNode)
        enterText();
    m_bufferedText.append(chars, length);
}

void XMLDocumentParser::error(XMLErrors::ErrorType type, const char* message, va_list args)
{
    if (isStopped())
        return;

    char formattedMessage[1024];
    vsnprintf(formattedMessage, sizeof(formattedMessage) - 1, message, args);

    if (m_parserPaused) {
        m_pendingCallbacks.append(adoptPtr(new PendingErrorCallback(type, reinterpret_cast<const xmlChar*>(formattedMessage), lineNumber(), columnNumber())));
        return;
    }

    handleError(type, formattedMessage, textPosition());
}

void XMLDocumentParser::processingInstruction(const String& target, const String& data)
{
    if (isStopped())
        return;

    if (m_parserPaused) {
        m_pendingCallbacks.append(adoptPtr(new PendingProcessingInstructionCallback(target, data)));
        return;
    }

    exitText();

    // ### handle exceptions
    TrackExceptionState exceptionState;
    RefPtrWillBeRawPtr<ProcessingInstruction> pi = m_currentNode->document().createProcessingInstruction(target, data, exceptionState);
    if (exceptionState.hadException())
        return;

    pi->setCreatedByParser(true);

    m_currentNode->parserAppendChild(pi.get());

    pi->setCreatedByParser(false);

    if (pi->isCSS())
        m_sawCSS = true;

    if (!RuntimeEnabledFeatures::xsltEnabled())
        return;
#ifdef MINIBLINK_NOT_IMPLEMENTED
    m_sawXSLTransform = !m_sawFirstElement && pi->isXSL();
    if (m_sawXSLTransform && !DocumentXSLT::hasTransformSourceDocument(*document())) {
        // This behavior is very tricky. We call stopParsing() here because we
        // want to stop processing the document until we're ready to apply the
        // transform, but we actually still want to be fed decoded string pieces
        // to accumulate in m_originalSourceForTransform. So, we call
        // stopParsing() here and check isStopped() in element callbacks.
        // FIXME: This contradicts the contract of DocumentParser.
        stopParsing();
    }
#endif
}

void XMLDocumentParser::cdataBlock(const String& text)
{
    if (isStopped())
        return;

    if (m_parserPaused) {
        m_pendingCallbacks.append(adoptPtr(new PendingCDATABlockCallback(text)));
        return;
    }

    exitText();

    m_currentNode->parserAppendChild(CDATASection::create(m_currentNode->document(), text));
}

void XMLDocumentParser::comment(const String& text)
{
    if (isStopped())
        return;

    if (m_parserPaused) {
        m_pendingCallbacks.append(adoptPtr(new PendingCommentCallback(text)));
        return;
    }

    exitText();

    m_currentNode->parserAppendChild(Comment::create(m_currentNode->document(), text));
}

enum StandaloneInfo {
    StandaloneUnspecified = -2,
    NoXMlDeclaration,
    StandaloneNo,
    StandaloneYes
};

void XMLDocumentParser::startDocument(const String& version, const String& encoding, int standalone)
{
    StandaloneInfo standaloneInfo = static_cast<StandaloneInfo>(standalone);
    if (standaloneInfo == NoXMlDeclaration) {
        document()->setHasXMLDeclaration(false);
        return;
    }

    if (!version.isNull())
        document()->setXMLVersion(version, ASSERT_NO_EXCEPTION);
    if (standalone != StandaloneUnspecified)
        document()->setXMLStandalone(standaloneInfo == StandaloneYes, ASSERT_NO_EXCEPTION);
    if (!encoding.isNull())
        document()->setXMLEncoding(encoding);
    document()->setHasXMLDeclaration(true);
}

void XMLDocumentParser::endDocument()
{
    exitText();
}

void XMLDocumentParser::internalSubset(const String& name, const String& externalID, const String& systemID)
{
    if (isStopped())
        return;

    if (m_parserPaused) {
        m_pendingCallbacks.append(adoptPtr(new PendingInternalSubsetCallback(name, externalID, systemID)));
        return;
    }

    if (document())
        document()->parserAppendChild(DocumentType::create(document(), name, externalID, systemID));
}

static inline XMLDocumentParser* getParser(void* closure)
{
    xmlParserCtxtPtr ctxt = static_cast<xmlParserCtxtPtr>(closure);
    return static_cast<XMLDocumentParser*>(ctxt->_private);
}

static void startElementNsHandler(void* closure, const xmlChar* localName, const xmlChar* prefix, const xmlChar* uri, int nbNamespaces, const xmlChar** namespaces, int nbAttributes, int nbDefaulted, const xmlChar** libxmlAttributes)
{
    getParser(closure)->startElementNs(toAtomicString(localName), toAtomicString(prefix), toAtomicString(uri), nbNamespaces, namespaces, nbAttributes, nbDefaulted, libxmlAttributes);
}

static void endElementNsHandler(void* closure, const xmlChar*, const xmlChar*, const xmlChar*)
{
    getParser(closure)->endElementNs();
}

static void charactersHandler(void* closure, const xmlChar* chars, int length)
{
    getParser(closure)->characters(chars, length);
}

static void processingInstructionHandler(void* closure, const xmlChar* target, const xmlChar* data)
{
    getParser(closure)->processingInstruction(toString(target), toString(data));
}

static void cdataBlockHandler(void* closure, const xmlChar* text, int length)
{
    getParser(closure)->cdataBlock(toString(text, length));
}

static void commentHandler(void* closure, const xmlChar* text)
{
    getParser(closure)->comment(toString(text));
}

WTF_ATTRIBUTE_PRINTF(2, 3)
static void warningHandler(void* closure, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    getParser(closure)->error(XMLErrors::ErrorTypeWarning, message, args);
    va_end(args);
}

WTF_ATTRIBUTE_PRINTF(2, 3)
static void normalErrorHandler(void* closure, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    getParser(closure)->error(XMLErrors::ErrorTypeNonFatal, message, args);
    va_end(args);
}

// Using a static entity and marking it XML_INTERNAL_PREDEFINED_ENTITY is a hack
// to avoid malloc/free. Using a global variable like this could cause trouble
// if libxml implementation details were to change
static xmlChar sharedXHTMLEntityResult[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

static xmlEntityPtr sharedXHTMLEntity()
{
    static xmlEntity entity;
    if (!entity.type) {
        entity.type = XML_ENTITY_DECL;
        entity.orig = sharedXHTMLEntityResult;
        entity.content = sharedXHTMLEntityResult;
        entity.etype = XML_INTERNAL_PREDEFINED_ENTITY;
    }
    return &entity;
}

static size_t convertUTF16EntityToUTF8(const UChar* utf16Entity, size_t numberOfCodeUnits, char* target, size_t targetSize)
{
    const char* originalTarget = target;
    WTF::Unicode::ConversionResult conversionResult = WTF::Unicode::convertUTF16ToUTF8(&utf16Entity,
        utf16Entity + numberOfCodeUnits, &target, target + targetSize);
    if (conversionResult != WTF::Unicode::conversionOK)
        return 0;

    // Even though we must pass the length, libxml expects the entity string to be null terminated.
    ASSERT(target > originalTarget + 1);
    *target = '\0';
    return target - originalTarget;
}

static xmlEntityPtr getXHTMLEntity(const xmlChar* name)
{
    UChar utf16DecodedEntity[4];
    size_t numberOfCodeUnits = decodeNamedEntityToUCharArray(reinterpret_cast<const char*>(name), utf16DecodedEntity);
    if (!numberOfCodeUnits)
        return 0;

    ASSERT(numberOfCodeUnits <= 4);
    size_t entityLengthInUTF8 = convertUTF16EntityToUTF8(utf16DecodedEntity, numberOfCodeUnits,
        reinterpret_cast<char*>(sharedXHTMLEntityResult), WTF_ARRAY_LENGTH(sharedXHTMLEntityResult));
    if (!entityLengthInUTF8)
        return 0;

    xmlEntityPtr entity = sharedXHTMLEntity();
    entity->length = entityLengthInUTF8;
    entity->name = name;
    return entity;
}

static xmlEntityPtr getEntityHandler(void* closure, const xmlChar* name)
{
    xmlParserCtxtPtr ctxt = static_cast<xmlParserCtxtPtr>(closure);
    xmlEntityPtr ent = xmlGetPredefinedEntity(name);
    if (ent) {
        ent->etype = XML_INTERNAL_PREDEFINED_ENTITY;
        return ent;
    }

    ent = xmlGetDocEntity(ctxt->myDoc, name);
    if (!ent && getParser(closure)->isXHTMLDocument()) {
        ent = getXHTMLEntity(name);
        if (ent)
            ent->etype = XML_INTERNAL_GENERAL_ENTITY;
    }

    return ent;
}

static void startDocumentHandler(void* closure)
{
    xmlParserCtxt* ctxt = static_cast<xmlParserCtxt*>(closure);
    XMLDocumentParser* parser = getParser(closure);
    switchEncoding(ctxt, parser->isCurrentlyParsing8BitChunk());
    parser->startDocument(toString(ctxt->version), toString(ctxt->encoding), ctxt->standalone);
    xmlSAX2StartDocument(closure);
}

static void endDocumentHandler(void* closure)
{
    getParser(closure)->endDocument();
    xmlSAX2EndDocument(closure);
}

static void internalSubsetHandler(void* closure, const xmlChar* name, const xmlChar* externalID, const xmlChar* systemID)
{
    getParser(closure)->internalSubset(toString(name), toString(externalID), toString(systemID));
    xmlSAX2InternalSubset(closure, name, externalID, systemID);
}

static void externalSubsetHandler(void* closure, const xmlChar*, const xmlChar* externalId, const xmlChar*)
{
    String extId = toString(externalId);
    if (extId == "-//W3C//DTD XHTML 1.0 Transitional//EN"
        || extId == "-//W3C//DTD XHTML 1.1//EN"
        || extId == "-//W3C//DTD XHTML 1.0 Strict//EN"
        || extId == "-//W3C//DTD XHTML 1.0 Frameset//EN"
        || extId == "-//W3C//DTD XHTML Basic 1.0//EN"
        || extId == "-//W3C//DTD XHTML 1.1 plus MathML 2.0//EN"
        || extId == "-//W3C//DTD XHTML 1.1 plus MathML 2.0 plus SVG 1.1//EN"
        || extId == "-//WAPFORUM//DTD XHTML Mobile 1.0//EN"
        || extId == "-//WAPFORUM//DTD XHTML Mobile 1.1//EN"
        || extId == "-//WAPFORUM//DTD XHTML Mobile 1.2//EN") {
        // Controls if we replace entities or not.
        getParser(closure)->setIsXHTMLDocument(true);
    }
}

static void ignorableWhitespaceHandler(void*, const xmlChar*, int)
{
    // Nothing to do, but we need this to work around a crasher.
    // http://bugzilla.gnome.org/show_bug.cgi?id=172255
    // http://bugs.webkit.org/show_bug.cgi?id=5792
}

void XMLDocumentParser::initializeParserContext(const CString& chunk)
{
    xmlSAXHandler sax;
    memset(&sax, 0, sizeof(sax));

    // According to http://xmlsoft.org/html/libxml-tree.html#xmlSAXHandler and
    // http://xmlsoft.org/html/libxml-parser.html#fatalErrorSAXFunc the SAX
    // fatalError callback is unused; error gets all the errors. Use normalErrorHandler
    // for both the error and fatalError callbacks.
    sax.error = normalErrorHandler;
    sax.fatalError = normalErrorHandler;
    sax.characters = charactersHandler;
    sax.processingInstruction = processingInstructionHandler;
    sax.cdataBlock = cdataBlockHandler;
    sax.comment = commentHandler;
    sax.warning = warningHandler;
    sax.startElementNs = startElementNsHandler;
    sax.endElementNs = endElementNsHandler;
    sax.getEntity = getEntityHandler;
    sax.startDocument = startDocumentHandler;
    sax.endDocument = endDocumentHandler;
    sax.internalSubset = internalSubsetHandler;
    sax.externalSubset = externalSubsetHandler;
    sax.ignorableWhitespace = ignorableWhitespaceHandler;
    sax.entityDecl = xmlSAX2EntityDecl;
    sax.initialized = XML_SAX2_MAGIC;
    m_sawError = false;
    m_sawCSS = false;
    m_sawXSLTransform = false;
    m_sawFirstElement = false;

    XMLDocumentParserScope scope(document());
    if (m_parsingFragment) {
        m_context = XMLParserContext::createMemoryParser(&sax, this, chunk);
    } else {
        ASSERT(!chunk.data());
        m_context = XMLParserContext::createStringParser(&sax, this);
    }
}

void XMLDocumentParser::doEnd()
{
    if (!isStopped()) {
        if (m_context) {
            // Tell libxml we're done.
            {
                XMLDocumentParserScope scope(document());
                finishParsing(context());
            }

            m_context = nullptr;
        }
    }

    bool xmlViewerMode = !m_sawError && !m_sawCSS && !m_sawXSLTransform && hasNoStyleInformation(document());
    if (xmlViewerMode) {
        const char noStyleMessage[] = "This XML file does not appear to have any style information associated with it. The document tree is shown below.";
        document()->setIsViewSource(true);
        V8Document::PrivateScript::transformDocumentToTreeViewMethod(document()->frame(), document(), noStyleMessage);
    } else if (m_sawXSLTransform) {
        xmlDocPtr doc = xmlDocPtrForString(document(), m_originalSourceForTransform.toString(), document()->url().string());
        document()->setTransformSource(adoptPtr(new TransformSource(doc)));
        // Make the document think it's done, so it will apply XSL stylesheets.
        document()->setParsingState(Document::FinishedParsing);
        document()->styleResolverChanged();

        // styleResolverChanged() call can detach the parser and null out its
        // document. In that case, we just bail out.
        if (isDetached())
            return;

        document()->setParsingState(Document::Parsing);
        DocumentParser::stopParsing();
    }
}

xmlDocPtr xmlDocPtrForString(Document* document, const String& source, const String& url)
{
    if (source.isEmpty())
        return 0;
#ifdef MINIBLINK_NOT_IMPLEMENTED
    // Parse in a single chunk into an xmlDocPtr
    // FIXME: Hook up error handlers so that a failure to parse the main
    // document results in good error messages.
    XMLDocumentParserScope scope(document, errorFunc, 0);
    XMLParserInput input(source);
    return xmlReadMemory(input.data(), input.size(), url.latin1().data(), input.encoding(), XSLT_PARSE_OPTIONS);
#else
    notImplemented();
    return 0;
#endif
}

OrdinalNumber XMLDocumentParser::lineNumber() const
{
    return OrdinalNumber::fromOneBasedInt(context() ? context()->input->line : 1);
}

OrdinalNumber XMLDocumentParser::columnNumber() const
{
    return OrdinalNumber::fromOneBasedInt(context() ? context()->input->col : 1);
}

TextPosition XMLDocumentParser::textPosition() const
{
    xmlParserCtxtPtr context = this->context();
    if (!context)
        return TextPosition::minimumPosition();
    return TextPosition(OrdinalNumber::fromOneBasedInt(context->input->line), OrdinalNumber::fromOneBasedInt(context->input->col));
}

void XMLDocumentParser::stopParsing()
{
    DocumentParser::stopParsing();
    if (context())
        xmlStopParser(context());
}

void XMLDocumentParser::resumeParsing()
{
    ASSERT(!isDetached());
    ASSERT(m_parserPaused);

    m_parserPaused = false;

    // First, execute any pending callbacks
    while (!m_pendingCallbacks.isEmpty()) {
        OwnPtr<PendingCallback> callback = m_pendingCallbacks.takeFirst();
        callback->call(this);

        // A callback paused the parser
        if (m_parserPaused)
            return;
    }

    // Then, write any pending data
    SegmentedString rest = m_pendingSrc;
    m_pendingSrc.clear();
    // There is normally only one string left, so toString() shouldn't copy.
    // In any case, the XML parser runs on the main thread and it's OK if
    // the passed string has more than one reference.
    append(rest.toString().impl());

    // Finally, if finish() has been called and write() didn't result
    // in any further callbacks being queued, call end()
    if (m_finishCalled && m_pendingCallbacks.isEmpty())
        end();
}

bool XMLDocumentParser::appendFragmentSource(const String& chunk)
{
    ASSERT(!m_context);
    ASSERT(m_parsingFragment);

    CString chunkAsUtf8 = chunk.utf8();

    // libxml2 takes an int for a length, and therefore can't handle XML chunks
    // larger than 2 GiB.
    if (chunkAsUtf8.length() > INT_MAX)
        return false;

    TRACE_EVENT0("blink", "XMLDocumentParser::appendFragmentSource");
    initializeParserContext(chunkAsUtf8);
    xmlParseContent(context());
    endDocument(); // Close any open text nodes.

    // FIXME: If this code is actually needed, it should probably move to
    // finish()
    // XMLDocumentParserQt has a similar check (m_stream.error() ==
    // QXmlStreamReader::PrematureEndOfDocumentError) in doEnd(). Check if all
    // the chunk has been processed.
    long bytesProcessed = xmlByteConsumed(context());
    if (bytesProcessed == -1 || static_cast<unsigned long>(bytesProcessed) != chunkAsUtf8.length()) {
        // FIXME: I don't believe we can hit this case without also having seen
        // an error or a null byte. If we hit this ASSERT, we've found a test
        // case which demonstrates the need for this code.
        ASSERT(m_sawError || (bytesProcessed >= 0 && !chunkAsUtf8.data()[bytesProcessed]));
        return false;
    }

    // No error if the chunk is well formed or it is not but we have no error.
    return context()->wellFormed || !xmlCtxtGetLastError(context());
}

// --------------------------------

struct AttributeParseState {
    HashMap<String, String> attributes;
    bool gotAttributes;
};

static void attributesStartElementNsHandler(void* closure, const xmlChar* xmlLocalName, const xmlChar* /*xmlPrefix*/,
    const xmlChar* /*xmlURI*/, int /*nbNamespaces*/, const xmlChar** /*namespaces*/,
    int nbAttributes, int /*nbDefaulted*/, const xmlChar** libxmlAttributes)
{
    if (strcmp(reinterpret_cast<const char*>(xmlLocalName), "attrs") != 0)
        return;

    xmlParserCtxtPtr ctxt = static_cast<xmlParserCtxtPtr>(closure);
    AttributeParseState* state = static_cast<AttributeParseState*>(ctxt->_private);

    state->gotAttributes = true;

    xmlSAX2Attributes* attributes = reinterpret_cast<xmlSAX2Attributes*>(libxmlAttributes);
    for (int i = 0; i < nbAttributes; ++i) {
        String attrLocalName = toString(attributes[i].localname);
        int valueLength = (int) (attributes[i].end - attributes[i].value);
        String attrValue = toString(attributes[i].value, valueLength);
        String attrPrefix = toString(attributes[i].prefix);
        String attrQName = attrPrefix.isEmpty() ? attrLocalName : attrPrefix + ":" + attrLocalName;

        state->attributes.set(attrQName, attrValue);
    }
}

HashMap<String, String> parseAttributes(const String& string, bool& attrsOK)
{
    AttributeParseState state;
    state.gotAttributes = false;

    xmlSAXHandler sax;
    memset(&sax, 0, sizeof(sax));
    sax.startElementNs = attributesStartElementNsHandler;
    sax.initialized = XML_SAX2_MAGIC;
    RefPtr<XMLParserContext> parser = XMLParserContext::createStringParser(&sax, &state);
    String parseString = "<?xml version=\"1.0\"?><attrs " + string + " />";
    parseChunk(parser->context(), parseString);
    finishParsing(parser->context());
    attrsOK = state.gotAttributes;
    return state.attributes;
}

} // namespace blink
