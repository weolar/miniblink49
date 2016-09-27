/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/parser/HTMLConstructionSite.h"

#include "core/HTMLElementFactory.h"
#include "core/HTMLNames.h"
#include "core/dom/Comment.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/DocumentType.h"
#include "core/dom/Element.h"
#include "core/dom/ScriptLoader.h"
#include "core/dom/TemplateContentDocumentFragment.h"
#include "core/dom/Text.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/html/HTMLScriptElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/parser/AtomicHTMLToken.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/parser/HTMLStackItem.h"
#include "core/html/parser/HTMLToken.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/svg/SVGScriptElement.h"
#include "platform/NotImplemented.h"
#include "platform/text/TextBreakIterator.h"
#include <limits>

namespace blink {

using namespace HTMLNames;

static const unsigned maximumHTMLParserDOMTreeDepth = 512;

static inline void setAttributes(Element* element, AtomicHTMLToken* token, ParserContentPolicy parserContentPolicy)
{
    if (!scriptingContentIsAllowed(parserContentPolicy))
        element->stripScriptingAttributes(token->attributes());
    element->parserSetAttributes(token->attributes());
}

static bool hasImpliedEndTag(const HTMLStackItem* item)
{
    return item->hasTagName(ddTag)
        || item->hasTagName(dtTag)
        || item->hasTagName(liTag)
        || item->hasTagName(optionTag)
        || item->hasTagName(optgroupTag)
        || item->hasTagName(pTag)
        || item->hasTagName(rbTag)
        || item->hasTagName(rpTag)
        || item->hasTagName(rtTag)
        || item->hasTagName(rtcTag);
}

static bool shouldUseLengthLimit(const ContainerNode& node)
{
    return !isHTMLScriptElement(node)
        && !isHTMLStyleElement(node)
        && !isSVGScriptElement(node);
}

static unsigned textLengthLimitForContainer(const ContainerNode& node)
{
    return shouldUseLengthLimit(node) ? Text::defaultLengthLimit : std::numeric_limits<unsigned>::max();
}

static inline bool isAllWhitespace(const String& string)
{
    return string.isAllSpecialCharacters<isHTMLSpace<UChar>>();
}

static inline void insert(HTMLConstructionSiteTask& task)
{
    if (isHTMLTemplateElement(*task.parent))
        task.parent = toHTMLTemplateElement(task.parent.get())->content();

    if (task.nextChild)
        task.parent->parserInsertBefore(task.child.get(), *task.nextChild);
    else
        task.parent->parserAppendChild(task.child.get());
}

static inline void executeInsertTask(HTMLConstructionSiteTask& task)
{
    ASSERT(task.operation == HTMLConstructionSiteTask::Insert);

    insert(task);

    if (task.child->isElementNode()) {
        Element& child = toElement(*task.child);
        child.beginParsingChildren();
        if (task.selfClosing)
            child.finishParsingChildren();
    }
}

static inline void executeInsertTextTask(HTMLConstructionSiteTask& task)
{
    ASSERT(task.operation == HTMLConstructionSiteTask::InsertText);
    ASSERT(task.child->isTextNode());

    // Merge text nodes into previous ones if possible:
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/tree-construction.html#insert-a-character
    Text* newText = toText(task.child.get());
    Node* previousChild = task.nextChild ? task.nextChild->previousSibling() : task.parent->lastChild();
    if (previousChild && previousChild->isTextNode()) {
        Text* previousText = toText(previousChild);
        unsigned lengthLimit = textLengthLimitForContainer(*task.parent);
        if (previousText->length() + newText->length() < lengthLimit) {
            previousText->parserAppendData(newText->data());
            return;
        }
    }

    insert(task);
}

static inline void executeReparentTask(HTMLConstructionSiteTask& task)
{
    ASSERT(task.operation == HTMLConstructionSiteTask::Reparent);

    task.parent->parserAppendChild(task.child);
}

static inline void executeInsertAlreadyParsedChildTask(HTMLConstructionSiteTask& task)
{
    ASSERT(task.operation == HTMLConstructionSiteTask::InsertAlreadyParsedChild);

    insert(task);
}

static inline void executeTakeAllChildrenTask(HTMLConstructionSiteTask& task)
{
    ASSERT(task.operation == HTMLConstructionSiteTask::TakeAllChildren);

    task.parent->parserTakeAllChildrenFrom(*task.oldParent());
}

void HTMLConstructionSite::executeTask(HTMLConstructionSiteTask& task)
{
    ASSERT(m_taskQueue.isEmpty());
    if (task.operation == HTMLConstructionSiteTask::Insert)
        return executeInsertTask(task);

    if (task.operation == HTMLConstructionSiteTask::InsertText)
        return executeInsertTextTask(task);

    // All the cases below this point are only used by the adoption agency.

    if (task.operation == HTMLConstructionSiteTask::InsertAlreadyParsedChild)
        return executeInsertAlreadyParsedChildTask(task);

    if (task.operation == HTMLConstructionSiteTask::Reparent)
        return executeReparentTask(task);

    if (task.operation == HTMLConstructionSiteTask::TakeAllChildren)
        return executeTakeAllChildrenTask(task);

    ASSERT_NOT_REACHED();
}

// This is only needed for TextDocuments where we might have text nodes
// approaching the default length limit (~64k) and we don't want to
// break a text node in the middle of a combining character.
static unsigned findBreakIndexBetween(const StringBuilder& string, unsigned currentPosition, unsigned proposedBreakIndex)
{
    ASSERT(currentPosition < proposedBreakIndex);
    ASSERT(proposedBreakIndex <= string.length());
    // The end of the string is always a valid break.
    if (proposedBreakIndex == string.length())
        return proposedBreakIndex;

    // Latin-1 does not have breakable boundaries. If we ever moved to a differnet 8-bit encoding this could be wrong.
    if (string.is8Bit())
        return proposedBreakIndex;

    const UChar* breakSearchCharacters = string.characters16() + currentPosition;
    // We need at least two characters look-ahead to account for UTF-16 surrogates, but can't search off the end of the buffer!
    unsigned breakSearchLength = std::min(proposedBreakIndex - currentPosition + 2, string.length() - currentPosition);
    NonSharedCharacterBreakIterator it(breakSearchCharacters, breakSearchLength);

    if (it.isBreak(proposedBreakIndex - currentPosition))
        return proposedBreakIndex;

    int adjustedBreakIndexInSubstring = it.preceding(proposedBreakIndex - currentPosition);
    if (adjustedBreakIndexInSubstring > 0)
        return currentPosition + adjustedBreakIndexInSubstring;
    // We failed to find a breakable point, let the caller figure out what to do.
    return 0;
}

static String atomizeIfAllWhitespace(const String& string, WhitespaceMode whitespaceMode)
{
    // Strings composed entirely of whitespace are likely to be repeated.
    // Turn them into AtomicString so we share a single string for each.
    if (whitespaceMode == AllWhitespace || (whitespaceMode == WhitespaceUnknown && isAllWhitespace(string)))
        return AtomicString(string).string();
    return string;
}

void HTMLConstructionSite::flushPendingText(FlushMode mode)
{
    if (m_pendingText.isEmpty())
        return;

    if (mode == FlushIfAtTextLimit
        && !shouldUseLengthLimit(*m_pendingText.parent))
        return;

    PendingText pendingText;
    // Hold onto the current pending text on the stack so that queueTask doesn't recurse infinitely.
    m_pendingText.swap(pendingText);
    ASSERT(m_pendingText.isEmpty());

    // Splitting text nodes into smaller chunks contradicts HTML5 spec, but is necessary
    // for performance, see: https://bugs.webkit.org/show_bug.cgi?id=55898
    unsigned lengthLimit = textLengthLimitForContainer(*pendingText.parent);

    unsigned currentPosition = 0;
    const StringBuilder& string = pendingText.stringBuilder;
    while (currentPosition < string.length()) {
        unsigned proposedBreakIndex = std::min(currentPosition + lengthLimit, string.length());
        unsigned breakIndex = findBreakIndexBetween(string, currentPosition, proposedBreakIndex);
        ASSERT(breakIndex <= string.length());
        String substring = string.substring(currentPosition, breakIndex - currentPosition);
        substring = atomizeIfAllWhitespace(substring, pendingText.whitespaceMode);

        HTMLConstructionSiteTask task(HTMLConstructionSiteTask::InsertText);
        task.parent = pendingText.parent;
        task.nextChild = pendingText.nextChild;
        task.child = Text::create(task.parent->document(), substring);
        queueTask(task);

        ASSERT(breakIndex > currentPosition);
        ASSERT(breakIndex - currentPosition == substring.length());
        ASSERT(toText(task.child.get())->length() == substring.length());
        currentPosition = breakIndex;
    }
}

void HTMLConstructionSite::queueTask(const HTMLConstructionSiteTask& task)
{
    flushPendingText(FlushAlways);
    ASSERT(m_pendingText.isEmpty());
    m_taskQueue.append(task);
}

void HTMLConstructionSite::attachLater(ContainerNode* parent, PassRefPtrWillBeRawPtr<Node> prpChild, bool selfClosing)
{
    ASSERT(scriptingContentIsAllowed(m_parserContentPolicy) || !prpChild.get()->isElementNode() || !toScriptLoaderIfPossible(toElement(prpChild.get())));
    ASSERT(pluginContentIsAllowed(m_parserContentPolicy) || !isHTMLPlugInElement(prpChild));

    HTMLConstructionSiteTask task(HTMLConstructionSiteTask::Insert);
    task.parent = parent;
    task.child = prpChild;
    task.selfClosing = selfClosing;

    if (shouldFosterParent()) {
        fosterParent(task.child);
        return;
    }

    // Add as a sibling of the parent if we have reached the maximum depth allowed.
    if (m_openElements.stackDepth() > maximumHTMLParserDOMTreeDepth && task.parent->parentNode())
        task.parent = task.parent->parentNode();

    ASSERT(task.parent);
    queueTask(task);
}

void HTMLConstructionSite::executeQueuedTasks()
{
    // This has no affect on pendingText, and we may have pendingText
    // remaining after executing all other queued tasks.
    const size_t size = m_taskQueue.size();
    if (!size)
        return;

    // Copy the task queue into a local variable in case executeTask
    // re-enters the parser.
    TaskQueue queue;
    queue.swap(m_taskQueue);

    for (size_t i = 0; i < size; ++i)
        executeTask(queue[i]);

    // We might be detached now.
}

HTMLConstructionSite::HTMLConstructionSite(Document* document, ParserContentPolicy parserContentPolicy)
    : m_document(document)
    , m_attachmentRoot(document)
    , m_parserContentPolicy(parserContentPolicy)
    , m_isParsingFragment(false)
    , m_redirectAttachToFosterParent(false)
    , m_inQuirksMode(document->inQuirksMode())
{
    ASSERT(m_document->isHTMLDocument() || m_document->isXHTMLDocument());
}

HTMLConstructionSite::HTMLConstructionSite(DocumentFragment* fragment, ParserContentPolicy parserContentPolicy)
    : m_document(&fragment->document())
    , m_attachmentRoot(fragment)
    , m_parserContentPolicy(parserContentPolicy)
    , m_isParsingFragment(true)
    , m_redirectAttachToFosterParent(false)
    , m_inQuirksMode(fragment->document().inQuirksMode())
{
    ASSERT(m_document->isHTMLDocument() || m_document->isXHTMLDocument());
}

HTMLConstructionSite::~HTMLConstructionSite()
{
    // Depending on why we're being destroyed it might be OK
    // to forget queued tasks, but currently we don't expect to.
    ASSERT(m_taskQueue.isEmpty());
    // Currently we assume that text will never be the last token in the
    // document and that we'll always queue some additional task to cause it to flush.
    ASSERT(m_pendingText.isEmpty());
}

DEFINE_TRACE(HTMLConstructionSite)
{
    visitor->trace(m_document);
    visitor->trace(m_attachmentRoot);
    visitor->trace(m_head);
    visitor->trace(m_form);
    visitor->trace(m_openElements);
    visitor->trace(m_activeFormattingElements);
    visitor->trace(m_taskQueue);
    visitor->trace(m_pendingText);
}

void HTMLConstructionSite::detach()
{
    // FIXME: We'd like to ASSERT here that we're canceling and not just discarding
    // text that really should have made it into the DOM earlier, but there
    // doesn't seem to be a nice way to do that.
    m_pendingText.discard();
    m_document = nullptr;
    m_attachmentRoot = nullptr;
}

void HTMLConstructionSite::setForm(HTMLFormElement* form)
{
    // This method should only be needed for HTMLTreeBuilder in the fragment case.
    ASSERT(!m_form);
    m_form = form;
}

PassRefPtrWillBeRawPtr<HTMLFormElement> HTMLConstructionSite::takeForm()
{
    return m_form.release();
}

void HTMLConstructionSite::dispatchDocumentElementAvailableIfNeeded()
{
    ASSERT(m_document);
    if (m_document->frame() && !m_isParsingFragment)
        m_document->frame()->loader().dispatchDocumentElementAvailable();
}

void HTMLConstructionSite::insertHTMLHtmlStartTagBeforeHTML(AtomicHTMLToken* token)
{
    ASSERT(m_document);
    RefPtrWillBeRawPtr<HTMLHtmlElement> element = HTMLHtmlElement::create(*m_document);
    setAttributes(element.get(), token, m_parserContentPolicy);
    attachLater(m_attachmentRoot, element);
    m_openElements.pushHTMLHtmlElement(HTMLStackItem::create(element, token));

    executeQueuedTasks();
    element->insertedByParser();
    dispatchDocumentElementAvailableIfNeeded();
}

void HTMLConstructionSite::mergeAttributesFromTokenIntoElement(AtomicHTMLToken* token, Element* element)
{
    if (token->attributes().isEmpty())
        return;

    for (unsigned i = 0; i < token->attributes().size(); ++i) {
        const Attribute& tokenAttribute = token->attributes().at(i);
        if (element->attributesWithoutUpdate().findIndex(tokenAttribute.name()) == kNotFound)
            element->setAttribute(tokenAttribute.name(), tokenAttribute.value());
    }
}

void HTMLConstructionSite::insertHTMLHtmlStartTagInBody(AtomicHTMLToken* token)
{
    // Fragments do not have a root HTML element, so any additional HTML elements
    // encountered during fragment parsing should be ignored.
    if (m_isParsingFragment)
        return;

    mergeAttributesFromTokenIntoElement(token, m_openElements.htmlElement());
}

void HTMLConstructionSite::insertHTMLBodyStartTagInBody(AtomicHTMLToken* token)
{
    mergeAttributesFromTokenIntoElement(token, m_openElements.bodyElement());
}

void HTMLConstructionSite::setDefaultCompatibilityMode()
{
    if (m_isParsingFragment)
        return;
    setCompatibilityMode(Document::QuirksMode);
}

void HTMLConstructionSite::setCompatibilityMode(Document::CompatibilityMode mode)
{
    m_inQuirksMode = (mode == Document::QuirksMode);
    m_document->setCompatibilityMode(mode);
}

void HTMLConstructionSite::setCompatibilityModeFromDoctype(const String& name, const String& publicId, const String& systemId)
{
    // There are three possible compatibility modes:
    // Quirks - quirks mode emulates WinIE and NS4. CSS parsing is also relaxed in this mode, e.g., unit types can
    // be omitted from numbers.
    // Limited Quirks - This mode is identical to no-quirks mode except for its treatment of line-height in the inline box model.
    // No Quirks - no quirks apply. Web pages will obey the specifications to the letter.

    // Check for Quirks Mode.
    if (name != "html"
        || publicId.startsWith("+//Silmaril//dtd html Pro v0r11 19970101//", TextCaseInsensitive)
        || publicId.startsWith("-//AdvaSoft Ltd//DTD HTML 3.0 asWedit + extensions//", TextCaseInsensitive)
        || publicId.startsWith("-//AS//DTD HTML 3.0 asWedit + extensions//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 2.0 Level 1//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 2.0 Level 2//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 2.0 Strict Level 1//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 2.0 Strict Level 2//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 2.0 Strict//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 2.0//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 2.1E//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 3.0//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 3.2 Final//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 3.2//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML 3//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Level 0//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Level 1//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Level 2//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Level 3//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Strict Level 0//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Strict Level 1//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Strict Level 2//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Strict Level 3//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML Strict//", TextCaseInsensitive)
        || publicId.startsWith("-//IETF//DTD HTML//", TextCaseInsensitive)
        || publicId.startsWith("-//Metrius//DTD Metrius Presentational//", TextCaseInsensitive)
        || publicId.startsWith("-//Microsoft//DTD Internet Explorer 2.0 HTML Strict//", TextCaseInsensitive)
        || publicId.startsWith("-//Microsoft//DTD Internet Explorer 2.0 HTML//", TextCaseInsensitive)
        || publicId.startsWith("-//Microsoft//DTD Internet Explorer 2.0 Tables//", TextCaseInsensitive)
        || publicId.startsWith("-//Microsoft//DTD Internet Explorer 3.0 HTML Strict//", TextCaseInsensitive)
        || publicId.startsWith("-//Microsoft//DTD Internet Explorer 3.0 HTML//", TextCaseInsensitive)
        || publicId.startsWith("-//Microsoft//DTD Internet Explorer 3.0 Tables//", TextCaseInsensitive)
        || publicId.startsWith("-//Netscape Comm. Corp.//DTD HTML//", TextCaseInsensitive)
        || publicId.startsWith("-//Netscape Comm. Corp.//DTD Strict HTML//", TextCaseInsensitive)
        || publicId.startsWith("-//O'Reilly and Associates//DTD HTML 2.0//", TextCaseInsensitive)
        || publicId.startsWith("-//O'Reilly and Associates//DTD HTML Extended 1.0//", TextCaseInsensitive)
        || publicId.startsWith("-//O'Reilly and Associates//DTD HTML Extended Relaxed 1.0//", TextCaseInsensitive)
        || publicId.startsWith("-//SoftQuad Software//DTD HoTMetaL PRO 6.0::19990601::extensions to HTML 4.0//", TextCaseInsensitive)
        || publicId.startsWith("-//SoftQuad//DTD HoTMetaL PRO 4.0::19971010::extensions to HTML 4.0//", TextCaseInsensitive)
        || publicId.startsWith("-//Spyglass//DTD HTML 2.0 Extended//", TextCaseInsensitive)
        || publicId.startsWith("-//SQ//DTD HTML 2.0 HoTMetaL + extensions//", TextCaseInsensitive)
        || publicId.startsWith("-//Sun Microsystems Corp.//DTD HotJava HTML//", TextCaseInsensitive)
        || publicId.startsWith("-//Sun Microsystems Corp.//DTD HotJava Strict HTML//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML 3 1995-03-24//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML 3.2 Draft//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML 3.2 Final//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML 3.2//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML 3.2S Draft//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML 4.0 Frameset//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML 4.0 Transitional//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML Experimental 19960712//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD HTML Experimental 970421//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD W3 HTML//", TextCaseInsensitive)
        || publicId.startsWith("-//W3O//DTD W3 HTML 3.0//", TextCaseInsensitive)
        || equalIgnoringCase(publicId, "-//W3O//DTD W3 HTML Strict 3.0//EN//")
        || publicId.startsWith("-//WebTechs//DTD Mozilla HTML 2.0//", TextCaseInsensitive)
        || publicId.startsWith("-//WebTechs//DTD Mozilla HTML//", TextCaseInsensitive)
        || equalIgnoringCase(publicId, "-/W3C/DTD HTML 4.0 Transitional/EN")
        || equalIgnoringCase(publicId, "HTML")
        || equalIgnoringCase(systemId, "http://www.ibm.com/data/dtd/v11/ibmxhtml1-transitional.dtd")
        || (systemId.isEmpty() && publicId.startsWith("-//W3C//DTD HTML 4.01 Frameset//", TextCaseInsensitive))
        || (systemId.isEmpty() && publicId.startsWith("-//W3C//DTD HTML 4.01 Transitional//", TextCaseInsensitive))) {
        setCompatibilityMode(Document::QuirksMode);
        return;
    }

    // Check for Limited Quirks Mode.
    if (publicId.startsWith("-//W3C//DTD XHTML 1.0 Frameset//", TextCaseInsensitive)
        || publicId.startsWith("-//W3C//DTD XHTML 1.0 Transitional//", TextCaseInsensitive)
        || (!systemId.isEmpty() && publicId.startsWith("-//W3C//DTD HTML 4.01 Frameset//", TextCaseInsensitive))
        || (!systemId.isEmpty() && publicId.startsWith("-//W3C//DTD HTML 4.01 Transitional//", TextCaseInsensitive))) {
        setCompatibilityMode(Document::LimitedQuirksMode);
        return;
    }

    // Otherwise we are No Quirks Mode.
    setCompatibilityMode(Document::NoQuirksMode);
}

void HTMLConstructionSite::processEndOfFile()
{
    ASSERT(currentNode());
    flush(FlushAlways);
    openElements()->popAll();
}

void HTMLConstructionSite::finishedParsing()
{
    // We shouldn't have any queued tasks but we might have pending text which we need to promote to tasks and execute.
    ASSERT(m_taskQueue.isEmpty());
    flush(FlushAlways);
    m_document->finishedParsing();
}

void HTMLConstructionSite::insertDoctype(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::DOCTYPE);

    const String& publicId = StringImpl::create8BitIfPossible(token->publicIdentifier());
    const String& systemId = StringImpl::create8BitIfPossible(token->systemIdentifier());
    RefPtrWillBeRawPtr<DocumentType> doctype = DocumentType::create(m_document, token->name(), publicId, systemId);
    attachLater(m_attachmentRoot, doctype.release());

    // DOCTYPE nodes are only processed when parsing fragments w/o contextElements, which
    // never occurs.  However, if we ever chose to support such, this code is subtly wrong,
    // because context-less fragments can determine their own quirks mode, and thus change
    // parsing rules (like <p> inside <table>).  For now we ASSERT that we never hit this code
    // in a fragment, as changing the owning document's compatibility mode would be wrong.
    ASSERT(!m_isParsingFragment);
    if (m_isParsingFragment)
        return;

    if (token->forceQuirks())
        setCompatibilityMode(Document::QuirksMode);
    else {
        setCompatibilityModeFromDoctype(token->name(), publicId, systemId);
    }
}

void HTMLConstructionSite::insertComment(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::Comment);
    attachLater(currentNode(), Comment::create(ownerDocumentForCurrentNode(), token->comment()));
}

void HTMLConstructionSite::insertCommentOnDocument(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::Comment);
    ASSERT(m_document);
    attachLater(m_attachmentRoot, Comment::create(*m_document, token->comment()));
}

void HTMLConstructionSite::insertCommentOnHTMLHtmlElement(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::Comment);
    ContainerNode* parent = m_openElements.rootNode();
    attachLater(parent, Comment::create(parent->document(), token->comment()));
}

void HTMLConstructionSite::insertHTMLHeadElement(AtomicHTMLToken* token)
{
    ASSERT(!shouldFosterParent());
    m_head = HTMLStackItem::create(createHTMLElement(token), token);
    attachLater(currentNode(), m_head->element());
    m_openElements.pushHTMLHeadElement(m_head);
}

void HTMLConstructionSite::insertHTMLBodyElement(AtomicHTMLToken* token)
{
    ASSERT(!shouldFosterParent());
    RefPtrWillBeRawPtr<HTMLElement> body = createHTMLElement(token);
    attachLater(currentNode(), body);
    m_openElements.pushHTMLBodyElement(HTMLStackItem::create(body.release(), token));
    if (LocalFrame* frame = m_document->frame())
        frame->loader().client()->dispatchWillInsertBody();
}

void HTMLConstructionSite::insertHTMLFormElement(AtomicHTMLToken* token, bool isDemoted)
{
    RefPtrWillBeRawPtr<HTMLElement> element = createHTMLElement(token);
    ASSERT(isHTMLFormElement(element));
    m_form = static_pointer_cast<HTMLFormElement>(element.release());
    m_form->setDemoted(isDemoted);
    attachLater(currentNode(), m_form.get());
    m_openElements.push(HTMLStackItem::create(m_form.get(), token));
}

void HTMLConstructionSite::insertHTMLElement(AtomicHTMLToken* token)
{
    RefPtrWillBeRawPtr<HTMLElement> element = createHTMLElement(token);
    attachLater(currentNode(), element);
    m_openElements.push(HTMLStackItem::create(element.release(), token));
}

void HTMLConstructionSite::insertSelfClosingHTMLElement(AtomicHTMLToken* token)
{
    ASSERT(token->type() == HTMLToken::StartTag);
    // Normally HTMLElementStack is responsible for calling finishParsingChildren,
    // but self-closing elements are never in the element stack so the stack
    // doesn't get a chance to tell them that we're done parsing their children.
    attachLater(currentNode(), createHTMLElement(token), true);
    // FIXME: Do we want to acknowledge the token's self-closing flag?
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/tokenization.html#acknowledge-self-closing-flag
}

void HTMLConstructionSite::insertFormattingElement(AtomicHTMLToken* token)
{
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/parsing.html#the-stack-of-open-elements
    // Possible active formatting elements include:
    // a, b, big, code, em, font, i, nobr, s, small, strike, strong, tt, and u.
    insertHTMLElement(token);
    m_activeFormattingElements.append(currentElementRecord()->stackItem());
}

void HTMLConstructionSite::insertScriptElement(AtomicHTMLToken* token)
{
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/scripting-1.html#already-started
    // http://html5.org/specs/dom-parsing.html#dom-range-createcontextualfragment
    // For createContextualFragment, the specifications say to mark it parser-inserted and already-started and later unmark them.
    // However, we short circuit that logic to avoid the subtree traversal to find script elements since scripts can never see
    // those flags or effects thereof.
    const bool parserInserted = m_parserContentPolicy != AllowScriptingContentAndDoNotMarkAlreadyStarted;
    const bool alreadyStarted = m_isParsingFragment && parserInserted;
    RefPtrWillBeRawPtr<HTMLScriptElement> element = HTMLScriptElement::create(ownerDocumentForCurrentNode(), parserInserted, alreadyStarted);
    setAttributes(element.get(), token, m_parserContentPolicy);
    if (scriptingContentIsAllowed(m_parserContentPolicy))
        attachLater(currentNode(), element);
    m_openElements.push(HTMLStackItem::create(element.release(), token));
}

void HTMLConstructionSite::insertForeignElement(AtomicHTMLToken* token, const AtomicString& namespaceURI)
{
    ASSERT(token->type() == HTMLToken::StartTag);
#ifdef MINIBLINK_NOT_IMPLEMENTED // weolar
    notImplemented(); // parseError when xmlns or xmlns:xlink are wrong.
#endif // MINIBLINK_NOT_IMPLEMENTED
    RefPtrWillBeRawPtr<Element> element = createElement(token, namespaceURI);
    if (scriptingContentIsAllowed(m_parserContentPolicy) || !toScriptLoaderIfPossible(element.get()))
        attachLater(currentNode(), element, token->selfClosing());
    if (!token->selfClosing())
        m_openElements.push(HTMLStackItem::create(element.release(), token, namespaceURI));
}

void HTMLConstructionSite::insertTextNode(const String& string, WhitespaceMode whitespaceMode)
{
    HTMLConstructionSiteTask dummyTask(HTMLConstructionSiteTask::Insert);
    dummyTask.parent = currentNode();

    if (shouldFosterParent())
        findFosterSite(dummyTask);

    // FIXME: This probably doesn't need to be done both here and in insert(Task).
    if (isHTMLTemplateElement(*dummyTask.parent))
        dummyTask.parent = toHTMLTemplateElement(dummyTask.parent.get())->content();

    // Unclear when parent != case occurs. Somehow we insert text into two separate nodes while processing the same Token.
    // The nextChild != dummy.nextChild case occurs whenever foster parenting happened and we hit a new text node "<table>a</table>b"
    // In either case we have to flush the pending text into the task queue before making more.
    if (!m_pendingText.isEmpty() && (m_pendingText.parent != dummyTask.parent ||  m_pendingText.nextChild != dummyTask.nextChild))
        flushPendingText(FlushAlways);
    m_pendingText.append(dummyTask.parent, dummyTask.nextChild, string, whitespaceMode);
}

void HTMLConstructionSite::reparent(HTMLElementStack::ElementRecord* newParent, HTMLElementStack::ElementRecord* child)
{
    HTMLConstructionSiteTask task(HTMLConstructionSiteTask::Reparent);
    task.parent = newParent->node();
    task.child = child->node();
    queueTask(task);
}

void HTMLConstructionSite::reparent(HTMLElementStack::ElementRecord* newParent, HTMLStackItem* child)
{
    HTMLConstructionSiteTask task(HTMLConstructionSiteTask::Reparent);
    task.parent = newParent->node();
    task.child = child->node();
    queueTask(task);
}

void HTMLConstructionSite::insertAlreadyParsedChild(HTMLStackItem* newParent, HTMLElementStack::ElementRecord* child)
{
    if (newParent->causesFosterParenting()) {
        fosterParent(child->node());
        return;
    }

    HTMLConstructionSiteTask task(HTMLConstructionSiteTask::InsertAlreadyParsedChild);
    task.parent = newParent->node();
    task.child = child->node();
    queueTask(task);
}

void HTMLConstructionSite::takeAllChildren(HTMLStackItem* newParent, HTMLElementStack::ElementRecord* oldParent)
{
    HTMLConstructionSiteTask task(HTMLConstructionSiteTask::TakeAllChildren);
    task.parent = newParent->node();
    task.child = oldParent->node();
    queueTask(task);
}

PassRefPtrWillBeRawPtr<Element> HTMLConstructionSite::createElement(AtomicHTMLToken* token, const AtomicString& namespaceURI)
{
    QualifiedName tagName(nullAtom, token->name(), namespaceURI);
    RefPtrWillBeRawPtr<Element> element = ownerDocumentForCurrentNode().createElement(tagName, true);
    setAttributes(element.get(), token, m_parserContentPolicy);
    return element.release();
}

inline Document& HTMLConstructionSite::ownerDocumentForCurrentNode()
{
    if (isHTMLTemplateElement(*currentNode()))
        return toHTMLTemplateElement(currentElement())->content()->document();
    return currentNode()->document();
}

PassRefPtrWillBeRawPtr<HTMLElement> HTMLConstructionSite::createHTMLElement(AtomicHTMLToken* token)
{
    Document& document = ownerDocumentForCurrentNode();
    // Only associate the element with the current form if we're creating the new element
    // in a document with a browsing context (rather than in <template> contents).
    HTMLFormElement* form = document.frame() ? m_form.get() : 0;
    // FIXME: This can't use HTMLConstructionSite::createElement because we
    // have to pass the current form element.  We should rework form association
    // to occur after construction to allow better code sharing here.
    RefPtrWillBeRawPtr<HTMLElement> element = HTMLElementFactory::createHTMLElement(token->name(), document, form, true);
    setAttributes(element.get(), token, m_parserContentPolicy);
    return element.release();
}

PassRefPtrWillBeRawPtr<HTMLStackItem> HTMLConstructionSite::createElementFromSavedToken(HTMLStackItem* item)
{
    RefPtrWillBeRawPtr<Element> element;
    // NOTE: Moving from item -> token -> item copies the Attribute vector twice!
    AtomicHTMLToken fakeToken(HTMLToken::StartTag, item->localName(), item->attributes());
    if (item->namespaceURI() == HTMLNames::xhtmlNamespaceURI)
        element = createHTMLElement(&fakeToken);
    else
        element = createElement(&fakeToken, item->namespaceURI());
    return HTMLStackItem::create(element.release(), &fakeToken, item->namespaceURI());
}

bool HTMLConstructionSite::indexOfFirstUnopenFormattingElement(unsigned& firstUnopenElementIndex) const
{
    if (m_activeFormattingElements.isEmpty())
        return false;
    unsigned index = m_activeFormattingElements.size();
    do {
        --index;
        const HTMLFormattingElementList::Entry& entry = m_activeFormattingElements.at(index);
        if (entry.isMarker() || m_openElements.contains(entry.element())) {
            firstUnopenElementIndex = index + 1;
            return firstUnopenElementIndex < m_activeFormattingElements.size();
        }
    } while (index);
    firstUnopenElementIndex = index;
    return true;
}

void HTMLConstructionSite::reconstructTheActiveFormattingElements()
{
    unsigned firstUnopenElementIndex;
    if (!indexOfFirstUnopenFormattingElement(firstUnopenElementIndex))
        return;

    unsigned unopenEntryIndex = firstUnopenElementIndex;
    ASSERT(unopenEntryIndex < m_activeFormattingElements.size());
    for (; unopenEntryIndex < m_activeFormattingElements.size(); ++unopenEntryIndex) {
        HTMLFormattingElementList::Entry& unopenedEntry = m_activeFormattingElements.at(unopenEntryIndex);
        RefPtrWillBeRawPtr<HTMLStackItem> reconstructed = createElementFromSavedToken(unopenedEntry.stackItem().get());
        attachLater(currentNode(), reconstructed->node());
        m_openElements.push(reconstructed);
        unopenedEntry.replaceElement(reconstructed.release());
    }
}

void HTMLConstructionSite::generateImpliedEndTagsWithExclusion(const AtomicString& tagName)
{
    while (hasImpliedEndTag(currentStackItem()) && !currentStackItem()->matchesHTMLTag(tagName))
        m_openElements.pop();
}

void HTMLConstructionSite::generateImpliedEndTags()
{
    while (hasImpliedEndTag(currentStackItem()))
        m_openElements.pop();
}

bool HTMLConstructionSite::inQuirksMode()
{
    return m_inQuirksMode;
}


// Adjusts |task| to match the "adjusted insertion location" determined by the foster parenting algorithm,
// laid out as the substeps of step 2 of https://html.spec.whatwg.org/#appropriate-place-for-inserting-a-node
void HTMLConstructionSite::findFosterSite(HTMLConstructionSiteTask& task)
{
    // 2.1
    HTMLElementStack::ElementRecord* lastTemplate = m_openElements.topmost(templateTag.localName());

    // 2.2
    HTMLElementStack::ElementRecord* lastTable = m_openElements.topmost(tableTag.localName());

    // 2.3
    if (lastTemplate && (!lastTable || lastTemplate->isAbove(lastTable))) {
        task.parent = lastTemplate->element();
        return;
    }

    // 2.4
    if (!lastTable) {
        // Fragment case
        task.parent = m_openElements.rootNode(); // DocumentFragment
        return;
    }

    // 2.5
    if (ContainerNode* parent = lastTable->element()->parentNode()) {
        task.parent = parent;
        task.nextChild = lastTable->element();
        return;
    }

    // 2.6, 2.7
    task.parent = lastTable->next()->element();
}

bool HTMLConstructionSite::shouldFosterParent() const
{
    return m_redirectAttachToFosterParent
        && currentStackItem()->isElementNode()
        && currentStackItem()->causesFosterParenting();
}

void HTMLConstructionSite::fosterParent(PassRefPtrWillBeRawPtr<Node> node)
{
    HTMLConstructionSiteTask task(HTMLConstructionSiteTask::Insert);
    findFosterSite(task);
    task.child = node;
    ASSERT(task.parent);
    queueTask(task);
}

DEFINE_TRACE(HTMLConstructionSite::PendingText)
{
    visitor->trace(parent);
    visitor->trace(nextChild);
}


}
