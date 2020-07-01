/*
 * This file is part of the XSL implementation.
 *
 * Copyright (C) 2004, 2005, 2006, 2008, 2012 Apple Inc. All rights reserved.
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
#include "core/xml/XSLStyleSheet.h"

#include "core/dom/Document.h"
#include "core/dom/Node.h"
#include "core/dom/TransformSource.h"
#include "core/frame/FrameHost.h"
#include "core/frame/LocalFrame.h"
#include "core/xml/XSLImportRule.h"
#include "core/xml/XSLTProcessor.h"
#include "core/xml/parser/XMLDocumentParserScope.h"
#include "core/xml/parser/XMLParserInput.h"
#include "wtf/text/CString.h"
#include <libxml/uri.h>
#include "third_party/libxslt/libxslt/xsltutils.h"

namespace blink {

XSLStyleSheet::XSLStyleSheet(XSLImportRule* parentRule, const String& originalURL, const KURL& finalURL)
    : m_ownerNode(nullptr)
    , m_originalURL(originalURL)
    , m_finalURL(finalURL)
    , m_isDisabled(false)
    , m_embedded(false)
    // Child sheets get marked as processed when the libxslt engine has finally
    // seen them.
    , m_processed(false)
    , m_stylesheetDoc(0)
    , m_stylesheetDocTaken(false)
    , m_compilationFailed(false)
    , m_parentStyleSheet(parentRule ? parentRule->parentStyleSheet() : 0)
    , m_ownerDocument(nullptr)
{
}

XSLStyleSheet::XSLStyleSheet(Node* parentNode, const String& originalURL, const KURL& finalURL,  bool embedded)
    : m_ownerNode(parentNode)
    , m_originalURL(originalURL)
    , m_finalURL(finalURL)
    , m_isDisabled(false)
    , m_embedded(embedded)
    , m_processed(true) // The root sheet starts off processed.
    , m_stylesheetDoc(0)
    , m_stylesheetDocTaken(false)
    , m_compilationFailed(false)
    , m_parentStyleSheet(nullptr)
    , m_ownerDocument(nullptr)
{
}

XSLStyleSheet::XSLStyleSheet(Document* ownerDocument, Node* styleSheetRootNode, const String& originalURL, const KURL& finalURL,  bool embedded)
    : m_ownerNode(styleSheetRootNode)
    , m_originalURL(originalURL)
    , m_finalURL(finalURL)
    , m_isDisabled(false)
    , m_embedded(embedded)
    , m_processed(true) // The root sheet starts off processed.
    , m_stylesheetDoc(0)
    , m_stylesheetDocTaken(false)
    , m_compilationFailed(false)
    , m_parentStyleSheet(nullptr)
    , m_ownerDocument(ownerDocument)
{
}

XSLStyleSheet::~XSLStyleSheet()
{
    if (!m_stylesheetDocTaken)
        xmlFreeDoc(m_stylesheetDoc);
#if !ENABLE(OILPAN)
    for (unsigned i = 0; i < m_children.size(); ++i) {
        ASSERT(m_children.at(i)->parentStyleSheet() == this);
        m_children.at(i)->setParentStyleSheet(0);
    }
#endif
}

bool XSLStyleSheet::isLoading() const
{
    for (unsigned i = 0; i < m_children.size(); ++i) {
        if (m_children.at(i)->isLoading())
            return true;
    }
    return false;
}

void XSLStyleSheet::checkLoaded()
{
    if (isLoading())
        return;
    if (XSLStyleSheet* styleSheet = parentStyleSheet())
        styleSheet->checkLoaded();
    if (ownerNode())
        ownerNode()->sheetLoaded();
}

xmlDocPtr XSLStyleSheet::document()
{
    if (m_embedded && ownerDocument() && ownerDocument()->transformSource())
        return (xmlDocPtr)ownerDocument()->transformSource()->platformSource();
    return m_stylesheetDoc;
}

void XSLStyleSheet::clearDocuments()
{
    m_stylesheetDoc = 0;
    for (unsigned i = 0; i < m_children.size(); ++i) {
        XSLImportRule* import = m_children.at(i).get();
        if (import->styleSheet())
            import->styleSheet()->clearDocuments();
    }
}

bool XSLStyleSheet::parseString(const String& source)
{
    // Parse in a single chunk into an xmlDocPtr
    if (!m_stylesheetDocTaken)
        xmlFreeDoc(m_stylesheetDoc);
    m_stylesheetDocTaken = false;

    FrameConsole* console = nullptr;
    if (LocalFrame* frame = ownerDocument()->frame())
        console = &frame->console();

    XMLDocumentParserScope scope(ownerDocument(), XSLTProcessor::genericErrorFunc, XSLTProcessor::parseErrorFunc, console);
    XMLParserInput input(source);

    xmlParserCtxtPtr ctxt = xmlCreateMemoryParserCtxt(input.data(), input.size());
    if (!ctxt)
        return 0;

    if (m_parentStyleSheet) {
        // The XSL transform may leave the newly-transformed document
        // with references to the symbol dictionaries of the style sheet
        // and any of its children. XML document disposal can corrupt memory
        // if a document uses more than one symbol dictionary, so we
        // ensure that all child stylesheets use the same dictionaries as their
        // parents.
        xmlDictFree(ctxt->dict);
        ctxt->dict = m_parentStyleSheet->m_stylesheetDoc->dict;
        xmlDictReference(ctxt->dict);
    }

    m_stylesheetDoc = xmlCtxtReadMemory(ctxt, input.data(), input.size(),
        finalURL().string().utf8().data(), input.encoding(),
        XML_PARSE_NOENT | XML_PARSE_DTDATTR | XML_PARSE_NOWARNING | XML_PARSE_NOCDATA);

    xmlFreeParserCtxt(ctxt);
    loadChildSheets();
    return m_stylesheetDoc;
}

void XSLStyleSheet::loadChildSheets()
{
    if (!document())
        return;

    xmlNodePtr stylesheetRoot = document()->children;

    // Top level children may include other things such as DTD nodes, we ignore
    // those.
    while (stylesheetRoot && stylesheetRoot->type != XML_ELEMENT_NODE)
        stylesheetRoot = stylesheetRoot->next;

    if (m_embedded) {
        // We have to locate (by ID) the appropriate embedded stylesheet
        // element, so that we can walk the import/include list.
        xmlAttrPtr idNode = xmlGetID(document(), (const xmlChar*)(finalURL().string().utf8().data()));
        if (!idNode)
            return;
        stylesheetRoot = idNode->parent;
    } else {
        // FIXME: Need to handle an external URI with a # in it. This is a
        // pretty minor edge case, so we'll deal with it later.
    }

    if (stylesheetRoot) {
        // Walk the children of the root element and look for import/include
        // elements. Imports must occur first.
        xmlNodePtr curr = stylesheetRoot->children;
        while (curr) {
            if (curr->type != XML_ELEMENT_NODE) {
                curr = curr->next;
                continue;
            }
            if (IS_XSLT_ELEM(curr) && IS_XSLT_NAME(curr, "import")) {
                xmlChar* uriRef = xsltGetNsProp(curr, (const xmlChar*)"href", XSLT_NAMESPACE);
                loadChildSheet(String::fromUTF8((const char*)uriRef));
                xmlFree(uriRef);
            } else {
                break;
            }
            curr = curr->next;
        }

        // Now handle includes.
        while (curr) {
            if (curr->type == XML_ELEMENT_NODE && IS_XSLT_ELEM(curr) && IS_XSLT_NAME(curr, "include")) {
                xmlChar* uriRef = xsltGetNsProp(curr, (const xmlChar*)"href", XSLT_NAMESPACE);
                loadChildSheet(String::fromUTF8((const char*)uriRef));
                xmlFree(uriRef);
            }
            curr = curr->next;
        }
    }
}

void XSLStyleSheet::loadChildSheet(const String& href)
{
    XSLImportRule* childRule = XSLImportRule::create(this, href);
    m_children.append(childRule);
    childRule->loadSheet();
}

xsltStylesheetPtr XSLStyleSheet::compileStyleSheet()
{
    // FIXME: Hook up error reporting for the stylesheet compilation process.
    if (m_embedded)
        return xsltLoadStylesheetPI(document());

    // Certain libxslt versions are corrupting the xmlDoc on compilation
    // failures - hence attempting to recompile after a failure is unsafe.
    if (m_compilationFailed)
        return 0;

    // xsltParseStylesheetDoc makes the document part of the stylesheet
    // so we have to release our pointer to it.
    ASSERT(!m_stylesheetDocTaken);
    xsltStylesheetPtr result = xsltParseStylesheetDoc(m_stylesheetDoc);
    if (result)
        m_stylesheetDocTaken = true;
    else
        m_compilationFailed = true;
    return result;
}

void XSLStyleSheet::setParentStyleSheet(XSLStyleSheet* parent)
{
    m_parentStyleSheet = parent;
}

Document* XSLStyleSheet::ownerDocument()
{
    for (XSLStyleSheet* styleSheet = this; styleSheet; styleSheet = styleSheet->parentStyleSheet()) {
        if (styleSheet->m_ownerDocument)
            return styleSheet->m_ownerDocument.get();
        Node* node = styleSheet->ownerNode();
        if (node)
            return &node->document();
    }
    return nullptr;
}

xmlDocPtr XSLStyleSheet::locateStylesheetSubResource(xmlDocPtr parentDoc, const xmlChar* uri)
{
    bool matchedParent = (parentDoc == document());
    for (unsigned i = 0; i < m_children.size(); ++i) {
        XSLImportRule* import = m_children.at(i).get();
        XSLStyleSheet* child = import->styleSheet();
        if (!child)
            continue;
        if (matchedParent) {
            if (child->processed())
                continue; // libxslt has been given this sheet already.

            // Check the URI of the child stylesheet against the doc URI.
            // In order to ensure that libxml canonicalized both URLs, we get
            // the original href string from the import rule and canonicalize it
            // using libxml before comparing it with the URI argument.
            CString importHref = import->href().utf8();
            xmlChar* base = xmlNodeGetBase(parentDoc, (xmlNodePtr)parentDoc);
            xmlChar* childURI = xmlBuildURI((const xmlChar*)importHref.data(), base);
            bool equalURIs = xmlStrEqual(uri, childURI);
            xmlFree(base);
            xmlFree(childURI);
            if (equalURIs) {
                child->markAsProcessed();
                return child->document();
            }
            continue;
        }
        xmlDocPtr result = import->styleSheet()->locateStylesheetSubResource(parentDoc, uri);
        if (result)
            return result;
    }

    return 0;
}

void XSLStyleSheet::markAsProcessed()
{
    ASSERT(!m_processed);
    ASSERT(!m_stylesheetDocTaken);
    m_processed = true;
    m_stylesheetDocTaken = true;
}

DEFINE_TRACE(XSLStyleSheet)
{
    visitor->trace(m_ownerNode);
    visitor->trace(m_children);
    visitor->trace(m_parentStyleSheet);
    visitor->trace(m_ownerDocument);
    StyleSheet::trace(visitor);
}

} // namespace blink
