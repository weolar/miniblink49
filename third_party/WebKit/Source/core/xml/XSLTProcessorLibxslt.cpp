/*
 * This file is part of the XSL implementation.
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple, Inc. All rights reserved.
 * Copyright (C) 2005, 2006 Alexey Proskuryakov <ap@webkit.org>
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
#include "core/xml/XSLTProcessor.h"

#include "core/dom/Document.h"
#include "core/dom/TransformSource.h"
#include "core/editing/markup.h"
#include "core/fetch/FetchInitiatorTypeNames.h"
#include "core/fetch/RawResource.h"
#include "core/fetch/Resource.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/frame/FrameConsole.h"
#include "core/frame/FrameHost.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/xml/XSLStyleSheet.h"
#include "core/xml/XSLTExtensions.h"
#include "core/xml/XSLTUnicodeSort.h"
#include "core/xml/parser/XMLDocumentParser.h"
#include "platform/SharedBuffer.h"
#include "platform/network/ResourceError.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/Assertions.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuffer.h"
#include "wtf/text/UTF8.h"
#include "third_party/libxslt/libxslt/imports.h"
#include "third_party/libxslt/libxslt/security.h"
#include "third_party/libxslt/libxslt/variables.h"
#include "third_party/libxslt/libxslt/xsltutils.h"

namespace blink {

void XSLTProcessor::genericErrorFunc(void*, const char*, ...)
{
    // It would be nice to do something with this error message.
}

void XSLTProcessor::parseErrorFunc(void* userData, xmlError* error)
{
    FrameConsole* console = static_cast<FrameConsole*>(userData);
    if (!console)
        return;

    MessageLevel level;
    switch (error->level) {
    case XML_ERR_NONE:
        level = DebugMessageLevel;
        break;
    case XML_ERR_WARNING:
        level = WarningMessageLevel;
        break;
    case XML_ERR_ERROR:
    case XML_ERR_FATAL:
    default:
        level = ErrorMessageLevel;
        break;
    }

    console->addMessage(ConsoleMessage::create(XMLMessageSource, level, error->message, error->file, error->line));
}

// FIXME: There seems to be no way to control the ctxt pointer for loading here, thus we have globals.
static XSLTProcessor* globalProcessor = nullptr;
static ResourceFetcher* globalResourceFetcher = nullptr;

static xmlDocPtr docLoaderFunc(
    const xmlChar* uri, xmlDictPtr, int options, void* ctxt, xsltLoadType type)
{
    if (!globalProcessor)
        return nullptr;

    switch (type) {
    case XSLT_LOAD_DOCUMENT: {
        xsltTransformContextPtr context = (xsltTransformContextPtr)ctxt;
        xmlChar* base = xmlNodeGetBase(context->document->doc, context->node);
        KURL url(KURL(ParsedURLString, reinterpret_cast<const char*>(base)), reinterpret_cast<const char*>(uri));
        xmlFree(base);

        ResourceLoaderOptions fetchOptions(ResourceFetcher::defaultResourceOptions());
        FetchRequest request(ResourceRequest(url), FetchInitiatorTypeNames::xml, fetchOptions);
        request.setOriginRestriction(FetchRequest::RestrictToSameOrigin);
        ResourcePtr<Resource> resource = RawResource::fetchSynchronously(request, globalResourceFetcher);
        if (!resource || !globalProcessor)
            return nullptr;

        FrameConsole* console = nullptr;
        LocalFrame* frame = globalProcessor->xslStylesheet()->ownerDocument()->frame();
        if (frame)
            console = &frame->console();
        xmlSetStructuredErrorFunc(console, XSLTProcessor::parseErrorFunc);
        xmlSetGenericErrorFunc(console, XSLTProcessor::genericErrorFunc);

        // We don't specify an encoding here. Neither Gecko nor WinIE respects
        // the encoding specified in the HTTP headers.
        SharedBuffer* data = resource->resourceBuffer();
        xmlDocPtr doc = data ? xmlReadMemory(data->data(), data->size(), (const char*)uri, 0, options) : nullptr;

        xmlSetStructuredErrorFunc(0, 0);
        xmlSetGenericErrorFunc(0, 0);

        return doc;
    }
    case XSLT_LOAD_STYLESHEET:
        return globalProcessor->xslStylesheet()->locateStylesheetSubResource(((xsltStylesheetPtr)ctxt)->doc, uri);
    default:
        break;
    }

    return nullptr;
}

static inline void setXSLTLoadCallBack(xsltDocLoaderFunc func, XSLTProcessor* processor, ResourceFetcher* fetcher)
{
    xsltSetLoaderFunc(func);
    globalProcessor = processor;
    globalResourceFetcher = fetcher;
}

static int writeToStringBuilder(void* context, const char* buffer, int len)
{
    StringBuilder& resultOutput = *static_cast<StringBuilder*>(context);

    if (!len)
        return 0;

    StringBuffer<UChar> stringBuffer(len);
    UChar* bufferUChar = stringBuffer.characters();
    UChar* bufferUCharEnd = bufferUChar + len;

    const char* stringCurrent = buffer;
    WTF::Unicode::ConversionResult result = WTF::Unicode::convertUTF8ToUTF16(&stringCurrent, buffer + len, &bufferUChar, bufferUCharEnd);
    if (result != WTF::Unicode::conversionOK && result != WTF::Unicode::sourceExhausted) {
        ASSERT_NOT_REACHED();
        return -1;
    }

    int utf16Length = bufferUChar - stringBuffer.characters();
    resultOutput.append(stringBuffer.characters(), utf16Length);
    return stringCurrent - buffer;
}

static bool saveResultToString(xmlDocPtr resultDoc, xsltStylesheetPtr sheet, String& resultString)
{
    xmlOutputBufferPtr outputBuf = xmlAllocOutputBuffer(0);
    if (!outputBuf)
        return false;

    StringBuilder resultBuilder;
    outputBuf->context = &resultBuilder;
    outputBuf->writecallback = writeToStringBuilder;

    int retval = xsltSaveResultTo(outputBuf, resultDoc, sheet);
    xmlOutputBufferClose(outputBuf);
    if (retval < 0)
        return false;

    // Workaround for <http://bugzilla.gnome.org/show_bug.cgi?id=495668>:
    // libxslt appends an extra line feed to the result.
    if (resultBuilder.length() > 0 && resultBuilder[resultBuilder.length() - 1] == '\n')
        resultBuilder.resize(resultBuilder.length() - 1);

    resultString = resultBuilder.toString();

    return true;
}

static const char** xsltParamArrayFromParameterMap(XSLTProcessor::ParameterMap& parameters)
{
    if (parameters.isEmpty())
        return nullptr;

    const char** parameterArray = static_cast<const char**>(fastMalloc(((parameters.size() * 2) + 1) * sizeof(char*)));

    unsigned index = 0;
    for (auto& parameter : parameters) {
        parameterArray[index++] = fastStrDup(parameter.key.utf8().data());
        parameterArray[index++] = fastStrDup(parameter.value.utf8().data());
    }
    parameterArray[index] = 0;

    return parameterArray;
}

static void freeXsltParamArray(const char** params)
{
    const char** temp = params;
    if (!params)
        return;

    while (*temp) {
        fastFree(const_cast<char*>(*(temp++)));
        fastFree(const_cast<char*>(*(temp++)));
    }
    fastFree(params);
}

static xsltStylesheetPtr xsltStylesheetPointer(Document* document, RefPtrWillBeMember<XSLStyleSheet>& cachedStylesheet, Node* stylesheetRootNode)
{
    if (!cachedStylesheet && stylesheetRootNode) {
        // When using importStylesheet, we will use the given document as the imported stylesheet's owner.
        cachedStylesheet = XSLStyleSheet::createForXSLTProcessor(
            stylesheetRootNode->parentNode() ? &stylesheetRootNode->parentNode()->document() : document,
            stylesheetRootNode,
            stylesheetRootNode->document().url().string(),
            stylesheetRootNode->document().url()); // FIXME: Should we use baseURL here?

        // According to Mozilla documentation, the node must be a Document node,
        // an xsl:stylesheet or xsl:transform element. But we just use text
        // content regardless of node type.
        cachedStylesheet->parseString(createMarkup(stylesheetRootNode));
    }

    if (!cachedStylesheet || !cachedStylesheet->document())
        return nullptr;

    return cachedStylesheet->compileStyleSheet();
}

static inline xmlDocPtr xmlDocPtrFromNode(Node* sourceNode, bool& shouldDelete)
{
    RefPtrWillBeRawPtr<Document> ownerDocument(sourceNode->document());
    bool sourceIsDocument = (sourceNode == ownerDocument.get());

    xmlDocPtr sourceDoc = nullptr;
    if (sourceIsDocument && ownerDocument->transformSource())
        sourceDoc = (xmlDocPtr)ownerDocument->transformSource()->platformSource();
    if (!sourceDoc) {
        sourceDoc = (xmlDocPtr)xmlDocPtrForString(ownerDocument.get(), createMarkup(sourceNode),
            sourceIsDocument ? ownerDocument->url().string() : String());
        shouldDelete = sourceDoc;
    }
    return sourceDoc;
}

static inline String resultMIMEType(xmlDocPtr resultDoc, xsltStylesheetPtr sheet)
{
    // There are three types of output we need to be able to deal with:
    // HTML (create an HTML document), XML (create an XML document),
    // and text (wrap in a <pre> and create an XML document).

    const xmlChar* resultType = nullptr;
    XSLT_GET_IMPORT_PTR(resultType, sheet, method);
    if (!resultType && resultDoc->type == XML_HTML_DOCUMENT_NODE)
        resultType = (const xmlChar*)"html";

    if (xmlStrEqual(resultType, (const xmlChar*)"html"))
        return "text/html";
    if (xmlStrEqual(resultType, (const xmlChar*)"text"))
        return "text/plain";

    return "application/xml";
}

bool XSLTProcessor::transformToString(Node* sourceNode, String& mimeType, String& resultString, String& resultEncoding)
{
    RefPtrWillBeRawPtr<Document> ownerDocument(sourceNode->document());

    setXSLTLoadCallBack(docLoaderFunc, this, ownerDocument->fetcher());
    xsltStylesheetPtr sheet = xsltStylesheetPointer(m_document.get(), m_stylesheet, m_stylesheetRootNode.get());
    if (!sheet) {
        setXSLTLoadCallBack(0, 0, 0);
        m_stylesheet = nullptr;
        return false;
    }
    m_stylesheet->clearDocuments();

    xmlChar* origMethod = sheet->method;
    if (!origMethod && mimeType == "text/html")
        sheet->method = (xmlChar*)"html";

    bool success = false;
    bool shouldFreeSourceDoc = false;
    if (xmlDocPtr sourceDoc = xmlDocPtrFromNode(sourceNode, shouldFreeSourceDoc)) {
        // The XML declaration would prevent parsing the result as a fragment,
        // and it's not needed even for documents, as the result of this
        // function is always immediately parsed.
        sheet->omitXmlDeclaration = true;

        xsltTransformContextPtr transformContext = xsltNewTransformContext(sheet, sourceDoc);
        registerXSLTExtensions(transformContext);

        xsltSecurityPrefsPtr securityPrefs = xsltNewSecurityPrefs();
        // Read permissions are checked by docLoaderFunc.
        if (0 != xsltSetSecurityPrefs(securityPrefs, XSLT_SECPREF_WRITE_FILE, xsltSecurityForbid))
            CRASH();
        if (0 != xsltSetSecurityPrefs(securityPrefs, XSLT_SECPREF_CREATE_DIRECTORY, xsltSecurityForbid))
            CRASH();
        if (0 != xsltSetSecurityPrefs(securityPrefs, XSLT_SECPREF_WRITE_NETWORK, xsltSecurityForbid))
            CRASH();
        if (0 != xsltSetCtxtSecurityPrefs(securityPrefs, transformContext))
            CRASH();

        // <http://bugs.webkit.org/show_bug.cgi?id=16077>: XSLT processor
        // <xsl:sort> algorithm only compares by code point.
        xsltSetCtxtSortFunc(transformContext, xsltUnicodeSortFunction);

        // This is a workaround for a bug in libxslt.
        // The bug has been fixed in version 1.1.13, so once we ship that this
        // can be removed.
        if (!transformContext->globalVars)
            transformContext->globalVars = xmlHashCreate(20);

        const char** params = xsltParamArrayFromParameterMap(m_parameters);
        xsltQuoteUserParams(transformContext, params);
        xmlDocPtr resultDoc = xsltApplyStylesheetUser(sheet, sourceDoc, 0, 0, 0, transformContext);

        xsltFreeTransformContext(transformContext);
        xsltFreeSecurityPrefs(securityPrefs);
        freeXsltParamArray(params);

        if (shouldFreeSourceDoc)
            xmlFreeDoc(sourceDoc);

        success = saveResultToString(resultDoc, sheet, resultString);
        if (success) {
            mimeType = resultMIMEType(resultDoc, sheet);
            resultEncoding = (char*)resultDoc->encoding;
        }
        xmlFreeDoc(resultDoc);
    }

    sheet->method = origMethod;
    setXSLTLoadCallBack(0, 0, 0);
    xsltFreeStylesheet(sheet);
    m_stylesheet = nullptr;

    return success;
}

} // namespace blink
