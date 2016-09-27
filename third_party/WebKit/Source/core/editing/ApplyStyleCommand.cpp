/*
 * Copyright (C) 2005, 2006, 2008, 2009 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/editing/ApplyStyleCommand.h"

#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/css/CSSComputedStyleDeclaration.h"
#include "core/css/CSSValuePool.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/Document.h"
#include "core/dom/NodeList.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/Range.h"
#include "core/dom/Text.h"
#include "core/editing/EditingStyle.h"
#include "core/editing/HTMLInterchange.h"
#include "core/editing/PlainTextRange.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLFontElement.h"
#include "core/html/HTMLSpanElement.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutText.h"
#include "platform/heap/Handle.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

static String& styleSpanClassString()
{
    DEFINE_STATIC_LOCAL(String, styleSpanClassString, ((AppleStyleSpanClass)));
    return styleSpanClassString;
}

bool isLegacyAppleHTMLSpanElement(const Node* node)
{
    if (!isHTMLSpanElement(node))
        return false;

    const HTMLSpanElement& span = toHTMLSpanElement(*node);
    if (span.getAttribute(classAttr) != styleSpanClassString())
        return false;
    UseCounter::count(span.document(), UseCounter::EditingAppleStyleSpanClass);
    return true;
}

static bool hasNoAttributeOrOnlyStyleAttribute(const HTMLElement* element, ShouldStyleAttributeBeEmpty shouldStyleAttributeBeEmpty)
{
    AttributeCollection attributes = element->attributes();
    if (attributes.isEmpty())
        return true;

    unsigned matchedAttributes = 0;
    if (element->getAttribute(classAttr) == styleSpanClassString())
        matchedAttributes++;
    if (element->hasAttribute(styleAttr) && (shouldStyleAttributeBeEmpty == AllowNonEmptyStyleAttribute
        || !element->inlineStyle() || element->inlineStyle()->isEmpty()))
        matchedAttributes++;

    ASSERT(matchedAttributes <= attributes.size());
    return matchedAttributes == attributes.size();
}

bool isStyleSpanOrSpanWithOnlyStyleAttribute(const Element* element)
{
    if (!isHTMLSpanElement(element))
        return false;
    return hasNoAttributeOrOnlyStyleAttribute(toHTMLSpanElement(element), AllowNonEmptyStyleAttribute);
}

static inline bool isSpanWithoutAttributesOrUnstyledStyleSpan(const Node* node)
{
    if (!isHTMLSpanElement(node))
        return false;
    return hasNoAttributeOrOnlyStyleAttribute(toHTMLSpanElement(node), StyleAttributeShouldBeEmpty);
}

bool isEmptyFontTag(const Element* element, ShouldStyleAttributeBeEmpty shouldStyleAttributeBeEmpty)
{
    if (!isHTMLFontElement(element))
        return false;

    return hasNoAttributeOrOnlyStyleAttribute(toHTMLFontElement(element), shouldStyleAttributeBeEmpty);
}

static PassRefPtrWillBeRawPtr<HTMLFontElement> createFontElement(Document& document)
{
    return toHTMLFontElement(createHTMLElement(document, fontTag).get());
}

PassRefPtrWillBeRawPtr<HTMLSpanElement> createStyleSpanElement(Document& document)
{
    return toHTMLSpanElement(createHTMLElement(document, spanTag).get());
}

ApplyStyleCommand::ApplyStyleCommand(Document& document, const EditingStyle* style, EditAction editingAction, EPropertyLevel propertyLevel)
    : CompositeEditCommand(document)
    , m_style(style->copy())
    , m_editingAction(editingAction)
    , m_propertyLevel(propertyLevel)
    , m_start(endingSelection().start().downstream())
    , m_end(endingSelection().end().upstream())
    , m_useEndingSelection(true)
    , m_styledInlineElement(nullptr)
    , m_removeOnly(false)
    , m_isInlineElementToRemoveFunction(0)
{
}

ApplyStyleCommand::ApplyStyleCommand(Document& document, const EditingStyle* style, const Position& start, const Position& end, EditAction editingAction, EPropertyLevel propertyLevel)
    : CompositeEditCommand(document)
    , m_style(style->copy())
    , m_editingAction(editingAction)
    , m_propertyLevel(propertyLevel)
    , m_start(start)
    , m_end(end)
    , m_useEndingSelection(false)
    , m_styledInlineElement(nullptr)
    , m_removeOnly(false)
    , m_isInlineElementToRemoveFunction(0)
{
}

ApplyStyleCommand::ApplyStyleCommand(PassRefPtrWillBeRawPtr<Element> element, bool removeOnly, EditAction editingAction)
    : CompositeEditCommand(element->document())
    , m_style(EditingStyle::create())
    , m_editingAction(editingAction)
    , m_propertyLevel(PropertyDefault)
    , m_start(endingSelection().start().downstream())
    , m_end(endingSelection().end().upstream())
    , m_useEndingSelection(true)
    , m_styledInlineElement(element)
    , m_removeOnly(removeOnly)
    , m_isInlineElementToRemoveFunction(0)
{
}

ApplyStyleCommand::ApplyStyleCommand(Document& document, const EditingStyle* style, IsInlineElementToRemoveFunction isInlineElementToRemoveFunction, EditAction editingAction)
    : CompositeEditCommand(document)
    , m_style(style->copy())
    , m_editingAction(editingAction)
    , m_propertyLevel(PropertyDefault)
    , m_start(endingSelection().start().downstream())
    , m_end(endingSelection().end().upstream())
    , m_useEndingSelection(true)
    , m_styledInlineElement(nullptr)
    , m_removeOnly(true)
    , m_isInlineElementToRemoveFunction(isInlineElementToRemoveFunction)
{
}

void ApplyStyleCommand::updateStartEnd(const Position& newStart, const Position& newEnd)
{
    ASSERT(comparePositions(newEnd, newStart) >= 0);

    if (!m_useEndingSelection && (newStart != m_start || newEnd != m_end))
        m_useEndingSelection = true;

    setEndingSelection(VisibleSelection(newStart, newEnd, VP_DEFAULT_AFFINITY, endingSelection().isDirectional()));
    m_start = newStart;
    m_end = newEnd;
}

Position ApplyStyleCommand::startPosition()
{
    if (m_useEndingSelection)
        return endingSelection().start();

    return m_start;
}

Position ApplyStyleCommand::endPosition()
{
    if (m_useEndingSelection)
        return endingSelection().end();

    return m_end;
}

void ApplyStyleCommand::doApply()
{
    switch (m_propertyLevel) {
    case PropertyDefault: {
        // Apply the block-centric properties of the style.
        RefPtrWillBeRawPtr<EditingStyle> blockStyle = m_style->extractAndRemoveBlockProperties();
        if (!blockStyle->isEmpty())
            applyBlockStyle(blockStyle.get());
        // Apply any remaining styles to the inline elements.
        if (!m_style->isEmpty() || m_styledInlineElement || m_isInlineElementToRemoveFunction) {
            applyRelativeFontStyleChange(m_style.get());
            applyInlineStyle(m_style.get());
        }
        break;
    }
    case ForceBlockProperties:
        // Force all properties to be applied as block styles.
        applyBlockStyle(m_style.get());
        break;
    }
}

EditAction ApplyStyleCommand::editingAction() const
{
    return m_editingAction;
}

void ApplyStyleCommand::applyBlockStyle(EditingStyle *style)
{
    // update document layout once before removing styles
    // so that we avoid the expense of updating before each and every call
    // to check a computed style
    document().updateLayoutIgnorePendingStylesheets();

    // get positions we want to use for applying style
    Position start = startPosition();
    Position end = endPosition();
    if (comparePositions(end, start) < 0) {
        Position swap = start;
        start = end;
        end = swap;
    }

    VisiblePosition visibleStart(start);
    VisiblePosition visibleEnd(end);

    if (visibleStart.isNull() || visibleStart.isOrphan() || visibleEnd.isNull() || visibleEnd.isOrphan())
        return;

    // Save and restore the selection endpoints using their indices in the document, since
    // addBlockStyleIfNeeded may moveParagraphs, which can remove these endpoints.
    // Calculate start and end indices from the start of the tree that they're in.
    Node& scope = NodeTraversal::highestAncestorOrSelf(*visibleStart.deepEquivalent().deprecatedNode());
    RefPtrWillBeRawPtr<Range> startRange = Range::create(document(), firstPositionInNode(&scope), visibleStart.deepEquivalent().parentAnchoredEquivalent());
    RefPtrWillBeRawPtr<Range> endRange = Range::create(document(), firstPositionInNode(&scope), visibleEnd.deepEquivalent().parentAnchoredEquivalent());
    int startIndex = TextIterator::rangeLength(startRange->startPosition(), startRange->endPosition(), true);
    int endIndex = TextIterator::rangeLength(endRange->startPosition(), endRange->endPosition(), true);

    VisiblePosition paragraphStart(startOfParagraph(visibleStart));
    VisiblePosition nextParagraphStart(endOfParagraph(paragraphStart).next());
    VisiblePosition beyondEnd(endOfParagraph(visibleEnd).next());
    while (paragraphStart.isNotNull() && paragraphStart != beyondEnd) {
        StyleChange styleChange(style, paragraphStart.deepEquivalent());
        if (styleChange.cssStyle().length() || m_removeOnly) {
            RefPtrWillBeRawPtr<Element> block = enclosingBlock(paragraphStart.deepEquivalent().deprecatedNode());
            const Position& paragraphStartToMove = paragraphStart.deepEquivalent();
            if (!m_removeOnly && isEditablePosition(paragraphStartToMove)) {
                RefPtrWillBeRawPtr<HTMLElement> newBlock = moveParagraphContentsToNewBlockIfNecessary(paragraphStartToMove);
                if (newBlock)
                    block = newBlock;
            }
            if (block && block->isHTMLElement()) {
                removeCSSStyle(style, toHTMLElement(block));
                if (!m_removeOnly)
                    addBlockStyle(styleChange, toHTMLElement(block));
            }

            if (nextParagraphStart.isOrphan())
                nextParagraphStart = endOfParagraph(paragraphStart).next();
        }

        paragraphStart = nextParagraphStart;
        nextParagraphStart = endOfParagraph(paragraphStart).next();
    }

    startRange = PlainTextRange(startIndex).createRangeForSelection(toContainerNode(scope));
    endRange = PlainTextRange(endIndex).createRangeForSelection(toContainerNode(scope));
    if (startRange && endRange)
        updateStartEnd(startRange->startPosition(), endRange->startPosition());
}

static PassRefPtrWillBeRawPtr<MutableStylePropertySet> copyStyleOrCreateEmpty(const StylePropertySet* style)
{
    if (!style)
        return MutableStylePropertySet::create();
    return style->mutableCopy();
}

void ApplyStyleCommand::applyRelativeFontStyleChange(EditingStyle* style)
{
    static const float MinimumFontSize = 0.1f;

    if (!style || !style->hasFontSizeDelta())
        return;

    Position start = startPosition();
    Position end = endPosition();
    if (comparePositions(end, start) < 0) {
        Position swap = start;
        start = end;
        end = swap;
    }

    // Join up any adjacent text nodes.
    if (start.deprecatedNode()->isTextNode()) {
        joinChildTextNodes(start.deprecatedNode()->parentNode(), start, end);
        start = startPosition();
        end = endPosition();
    }

    if (start.isNull() || end.isNull())
        return;

    if (end.deprecatedNode()->isTextNode() && start.deprecatedNode()->parentNode() != end.deprecatedNode()->parentNode()) {
        joinChildTextNodes(end.deprecatedNode()->parentNode(), start, end);
        start = startPosition();
        end = endPosition();
    }

    if (start.isNull() || end.isNull())
        return;

    // Split the start text nodes if needed to apply style.
    if (isValidCaretPositionInTextNode(start)) {
        splitTextAtStart(start, end);
        start = startPosition();
        end = endPosition();
    }

    if (isValidCaretPositionInTextNode(end)) {
        splitTextAtEnd(start, end);
        start = startPosition();
        end = endPosition();
    }

    // Calculate loop end point.
    // If the end node is before the start node (can only happen if the end node is
    // an ancestor of the start node), we gather nodes up to the next sibling of the end node
    Node* beyondEnd;
    ASSERT(start.deprecatedNode());
    ASSERT(end.deprecatedNode());
    if (start.deprecatedNode()->isDescendantOf(end.deprecatedNode()))
        beyondEnd = NodeTraversal::nextSkippingChildren(*end.deprecatedNode());
    else
        beyondEnd = NodeTraversal::next(*end.deprecatedNode());

    start = start.upstream(); // Move upstream to ensure we do not add redundant spans.
    Node* startNode = start.deprecatedNode();
    ASSERT(startNode);

    // Make sure we're not already at the end or the next NodeTraversal::next() will traverse
    // past it.
    if (startNode == beyondEnd)
        return;

    if (startNode->isTextNode() && start.deprecatedEditingOffset() >= caretMaxOffset(startNode)) {
        // Move out of text node if range does not include its characters.
        startNode = NodeTraversal::next(*startNode);
        if (!startNode)
            return;
    }

    // Store away font size before making any changes to the document.
    // This ensures that changes to one node won't effect another.
    WillBeHeapHashMap<RawPtrWillBeMember<Node>, float> startingFontSizes;
    for (Node* node = startNode; node != beyondEnd; node = NodeTraversal::next(*node)) {
        ASSERT(node);
        startingFontSizes.set(node, computedFontSize(node));
    }

    // These spans were added by us. If empty after font size changes, they can be removed.
    WillBeHeapVector<RefPtrWillBeMember<HTMLElement>> unstyledSpans;

    Node* lastStyledNode = nullptr;
    for (Node* node = startNode; node != beyondEnd; node = NodeTraversal::next(*node)) {
        ASSERT(node);
        RefPtrWillBeRawPtr<HTMLElement> element = nullptr;
        if (node->isHTMLElement()) {
            // Only work on fully selected nodes.
            if (!elementFullySelected(toHTMLElement(*node), start, end))
                continue;
            element = toHTMLElement(node);
        } else if (node->isTextNode() && node->layoutObject() && node->parentNode() != lastStyledNode) {
            // Last styled node was not parent node of this text node, but we wish to style this
            // text node. To make this possible, add a style span to surround this text node.
            RefPtrWillBeRawPtr<HTMLSpanElement> span = createStyleSpanElement(document());
            surroundNodeRangeWithElement(node, node, span.get());
            element = span.release();
        }  else {
            // Only handle HTML elements and text nodes.
            continue;
        }
        lastStyledNode = node;

        RefPtrWillBeRawPtr<MutableStylePropertySet> inlineStyle = copyStyleOrCreateEmpty(element->inlineStyle());
        float currentFontSize = computedFontSize(node);
        float desiredFontSize = max(MinimumFontSize, startingFontSizes.get(node) + style->fontSizeDelta());
        RefPtrWillBeRawPtr<CSSValue> value = inlineStyle->getPropertyCSSValue(CSSPropertyFontSize);
        if (value) {
            element->removeInlineStyleProperty(CSSPropertyFontSize);
            currentFontSize = computedFontSize(node);
        }
        if (currentFontSize != desiredFontSize) {
            inlineStyle->setProperty(CSSPropertyFontSize, cssValuePool().createValue(desiredFontSize, CSSPrimitiveValue::CSS_PX), false);
            setNodeAttribute(element.get(), styleAttr, AtomicString(inlineStyle->asText()));
        }
        if (inlineStyle->isEmpty()) {
            removeElementAttribute(element.get(), styleAttr);
            if (isSpanWithoutAttributesOrUnstyledStyleSpan(element.get()))
                unstyledSpans.append(element.release());
        }
    }

    for (const auto& unstyledSpan : unstyledSpans)
        removeNodePreservingChildren(unstyledSpan.get());
}

static ContainerNode* dummySpanAncestorForNode(const Node* node)
{
    while (node && (!node->isElementNode() || !isStyleSpanOrSpanWithOnlyStyleAttribute(toElement(node))))
        node = node->parentNode();

    return node ? node->parentNode() : 0;
}

void ApplyStyleCommand::cleanupUnstyledAppleStyleSpans(ContainerNode* dummySpanAncestor)
{
    if (!dummySpanAncestor)
        return;

    // Dummy spans are created when text node is split, so that style information
    // can be propagated, which can result in more splitting. If a dummy span gets
    // cloned/split, the new node is always a sibling of it. Therefore, we scan
    // all the children of the dummy's parent
    Node* next;
    for (Node* node = dummySpanAncestor->firstChild(); node; node = next) {
        next = node->nextSibling();
        if (isSpanWithoutAttributesOrUnstyledStyleSpan(node))
            removeNodePreservingChildren(node);
    }
}

HTMLElement* ApplyStyleCommand::splitAncestorsWithUnicodeBidi(Node* node, bool before, WritingDirection allowedDirection)
{
    // We are allowed to leave the highest ancestor with unicode-bidi unsplit if it is unicode-bidi: embed and direction: allowedDirection.
    // In that case, we return the unsplit ancestor. Otherwise, we return 0.
    Element* block = enclosingBlock(node);
    if (!block)
        return 0;

    ContainerNode* highestAncestorWithUnicodeBidi = nullptr;
    ContainerNode* nextHighestAncestorWithUnicodeBidi = nullptr;
    int highestAncestorUnicodeBidi = 0;
    for (ContainerNode* n = node->parentNode(); n != block; n = n->parentNode()) {
        int unicodeBidi = getIdentifierValue(CSSComputedStyleDeclaration::create(n).get(), CSSPropertyUnicodeBidi);
        if (unicodeBidi && unicodeBidi != CSSValueNormal) {
            highestAncestorUnicodeBidi = unicodeBidi;
            nextHighestAncestorWithUnicodeBidi = highestAncestorWithUnicodeBidi;
            highestAncestorWithUnicodeBidi = n;
        }
    }

    if (!highestAncestorWithUnicodeBidi)
        return 0;

    HTMLElement* unsplitAncestor = 0;

    WritingDirection highestAncestorDirection;
    if (allowedDirection != NaturalWritingDirection
        && highestAncestorUnicodeBidi != CSSValueBidiOverride
        && highestAncestorWithUnicodeBidi->isHTMLElement()
        && EditingStyle::create(highestAncestorWithUnicodeBidi, EditingStyle::AllProperties)->textDirection(highestAncestorDirection)
        && highestAncestorDirection == allowedDirection) {
        if (!nextHighestAncestorWithUnicodeBidi)
            return toHTMLElement(highestAncestorWithUnicodeBidi);

        unsplitAncestor = toHTMLElement(highestAncestorWithUnicodeBidi);
        highestAncestorWithUnicodeBidi = nextHighestAncestorWithUnicodeBidi;
    }

    // Split every ancestor through highest ancestor with embedding.
    RefPtrWillBeRawPtr<Node> currentNode = node;
    while (currentNode) {
        RefPtrWillBeRawPtr<Element> parent = toElement(currentNode->parentNode());
        if (before ? currentNode->previousSibling() : currentNode->nextSibling())
            splitElement(parent, before ? currentNode.get() : currentNode->nextSibling());
        if (parent == highestAncestorWithUnicodeBidi)
            break;
        currentNode = parent;
    }
    return unsplitAncestor;
}

void ApplyStyleCommand::removeEmbeddingUpToEnclosingBlock(Node* node, HTMLElement* unsplitAncestor)
{
    Element* block = enclosingBlock(node);
    if (!block)
        return;

    for (ContainerNode* n = node->parentNode(); n != block && n != unsplitAncestor; n = n->parentNode()) {
        if (!n->isStyledElement())
            continue;

        Element* element = toElement(n);
        int unicodeBidi = getIdentifierValue(CSSComputedStyleDeclaration::create(element).get(), CSSPropertyUnicodeBidi);
        if (!unicodeBidi || unicodeBidi == CSSValueNormal)
            continue;

        // FIXME: This code should really consider the mapped attribute 'dir', the inline style declaration,
        // and all matching style rules in order to determine how to best set the unicode-bidi property to 'normal'.
        // For now, it assumes that if the 'dir' attribute is present, then removing it will suffice, and
        // otherwise it sets the property in the inline style declaration.
        if (element->hasAttribute(dirAttr)) {
            // FIXME: If this is a BDO element, we should probably just remove it if it has no
            // other attributes, like we (should) do with B and I elements.
            removeElementAttribute(element, dirAttr);
        } else {
            RefPtrWillBeRawPtr<MutableStylePropertySet> inlineStyle = copyStyleOrCreateEmpty(element->inlineStyle());
            inlineStyle->setProperty(CSSPropertyUnicodeBidi, CSSValueNormal);
            inlineStyle->removeProperty(CSSPropertyDirection);
            setNodeAttribute(element, styleAttr, AtomicString(inlineStyle->asText()));
            if (isSpanWithoutAttributesOrUnstyledStyleSpan(element))
                removeNodePreservingChildren(element);
        }
    }
}

static HTMLElement* highestEmbeddingAncestor(Node* startNode, Node* enclosingNode)
{
    for (Node* n = startNode; n && n != enclosingNode; n = n->parentNode()) {
        if (n->isHTMLElement() && getIdentifierValue(CSSComputedStyleDeclaration::create(n).get(), CSSPropertyUnicodeBidi) == CSSValueEmbed)
            return toHTMLElement(n);
    }

    return 0;
}

void ApplyStyleCommand::applyInlineStyle(EditingStyle* style)
{
    RefPtrWillBeRawPtr<ContainerNode> startDummySpanAncestor = nullptr;
    RefPtrWillBeRawPtr<ContainerNode> endDummySpanAncestor = nullptr;

    // update document layout once before removing styles
    // so that we avoid the expense of updating before each and every call
    // to check a computed style
    document().updateLayoutIgnorePendingStylesheets();

    // adjust to the positions we want to use for applying style
    Position start = startPosition();
    Position end = endPosition();

    if (start.isNull() || end.isNull())
        return;

    if (comparePositions(end, start) < 0) {
        Position swap = start;
        start = end;
        end = swap;
    }

    // split the start node and containing element if the selection starts inside of it
    bool splitStart = isValidCaretPositionInTextNode(start);
    if (splitStart) {
        if (shouldSplitTextElement(start.deprecatedNode()->parentElement(), style))
            splitTextElementAtStart(start, end);
        else
            splitTextAtStart(start, end);
        start = startPosition();
        end = endPosition();
        if (start.isNull() || end.isNull())
            return;
        startDummySpanAncestor = dummySpanAncestorForNode(start.deprecatedNode());
    }

    // split the end node and containing element if the selection ends inside of it
    bool splitEnd = isValidCaretPositionInTextNode(end);
    if (splitEnd) {
        if (shouldSplitTextElement(end.deprecatedNode()->parentElement(), style))
            splitTextElementAtEnd(start, end);
        else
            splitTextAtEnd(start, end);
        start = startPosition();
        end = endPosition();
        if (start.isNull() || end.isNull())
            return;
        endDummySpanAncestor = dummySpanAncestorForNode(end.deprecatedNode());
    }

    // Remove style from the selection.
    // Use the upstream position of the start for removing style.
    // This will ensure we remove all traces of the relevant styles from the selection
    // and prevent us from adding redundant ones, as described in:
    // <rdar://problem/3724344> Bolding and unbolding creates extraneous tags
    Position removeStart = start.upstream();
    WritingDirection textDirection = NaturalWritingDirection;
    bool hasTextDirection = style->textDirection(textDirection);
    RefPtrWillBeRawPtr<EditingStyle> styleWithoutEmbedding = nullptr;
    RefPtrWillBeRawPtr<EditingStyle> embeddingStyle = nullptr;
    if (hasTextDirection) {
        // Leave alone an ancestor that provides the desired single level embedding, if there is one.
        HTMLElement* startUnsplitAncestor = splitAncestorsWithUnicodeBidi(start.deprecatedNode(), true, textDirection);
        HTMLElement* endUnsplitAncestor = splitAncestorsWithUnicodeBidi(end.deprecatedNode(), false, textDirection);
        removeEmbeddingUpToEnclosingBlock(start.deprecatedNode(), startUnsplitAncestor);
        removeEmbeddingUpToEnclosingBlock(end.deprecatedNode(), endUnsplitAncestor);

        // Avoid removing the dir attribute and the unicode-bidi and direction properties from the unsplit ancestors.
        Position embeddingRemoveStart = removeStart;
        if (startUnsplitAncestor && elementFullySelected(*startUnsplitAncestor, removeStart, end))
            embeddingRemoveStart = positionInParentAfterNode(*startUnsplitAncestor);

        Position embeddingRemoveEnd = end;
        if (endUnsplitAncestor && elementFullySelected(*endUnsplitAncestor, removeStart, end))
            embeddingRemoveEnd = positionInParentBeforeNode(*endUnsplitAncestor).downstream();

        if (embeddingRemoveEnd != removeStart || embeddingRemoveEnd != end) {
            styleWithoutEmbedding = style->copy();
            embeddingStyle = styleWithoutEmbedding->extractAndRemoveTextDirection();

            if (comparePositions(embeddingRemoveStart, embeddingRemoveEnd) <= 0)
                removeInlineStyle(embeddingStyle.get(), embeddingRemoveStart, embeddingRemoveEnd);
        }
    }

    removeInlineStyle(styleWithoutEmbedding ? styleWithoutEmbedding.get() : style, removeStart, end);
    start = startPosition();
    end = endPosition();
    if (start.isNull() || start.isOrphan() || end.isNull() || end.isOrphan())
        return;

    if (splitStart && mergeStartWithPreviousIfIdentical(start, end)) {
        start = startPosition();
        end = endPosition();
    }

    if (splitEnd) {
        mergeEndWithNextIfIdentical(start, end);
        start = startPosition();
        end = endPosition();
    }

    // update document layout once before running the rest of the function
    // so that we avoid the expense of updating before each and every call
    // to check a computed style
    document().updateLayoutIgnorePendingStylesheets();

    RefPtrWillBeRawPtr<EditingStyle> styleToApply = style;
    if (hasTextDirection) {
        // Avoid applying the unicode-bidi and direction properties beneath ancestors that already have them.
        HTMLElement* embeddingStartElement = highestEmbeddingAncestor(start.deprecatedNode(), enclosingBlock(start.deprecatedNode()));
        HTMLElement* embeddingEndElement = highestEmbeddingAncestor(end.deprecatedNode(), enclosingBlock(end.deprecatedNode()));

        if (embeddingStartElement || embeddingEndElement) {
            Position embeddingApplyStart = embeddingStartElement ? positionInParentAfterNode(*embeddingStartElement) : start;
            Position embeddingApplyEnd = embeddingEndElement ? positionInParentBeforeNode(*embeddingEndElement) : end;
            ASSERT(embeddingApplyStart.isNotNull() && embeddingApplyEnd.isNotNull());

            if (!embeddingStyle) {
                styleWithoutEmbedding = style->copy();
                embeddingStyle = styleWithoutEmbedding->extractAndRemoveTextDirection();
            }
            fixRangeAndApplyInlineStyle(embeddingStyle.get(), embeddingApplyStart, embeddingApplyEnd);

            styleToApply = styleWithoutEmbedding;
        }
    }

    fixRangeAndApplyInlineStyle(styleToApply.get(), start, end);

    // Remove dummy style spans created by splitting text elements.
    cleanupUnstyledAppleStyleSpans(startDummySpanAncestor.get());
    if (endDummySpanAncestor != startDummySpanAncestor)
        cleanupUnstyledAppleStyleSpans(endDummySpanAncestor.get());
}

void ApplyStyleCommand::fixRangeAndApplyInlineStyle(EditingStyle* style, const Position& start, const Position& end)
{
    Node* startNode = start.deprecatedNode();
    ASSERT(startNode);

    if (start.deprecatedEditingOffset() >= caretMaxOffset(start.deprecatedNode())) {
        startNode = NodeTraversal::next(*startNode);
        if (!startNode || comparePositions(end, firstPositionInOrBeforeNode(startNode)) < 0)
            return;
    }

    Node* pastEndNode = end.deprecatedNode();
    if (end.deprecatedEditingOffset() >= caretMaxOffset(end.deprecatedNode()))
        pastEndNode = NodeTraversal::nextSkippingChildren(*end.deprecatedNode());

    // FIXME: Callers should perform this operation on a Range that includes the br
    // if they want style applied to the empty line.
    if (start == end && isHTMLBRElement(*start.deprecatedNode()))
        pastEndNode = NodeTraversal::next(*start.deprecatedNode());

    // Start from the highest fully selected ancestor so that we can modify the fully selected node.
    // e.g. When applying font-size: large on <font color="blue">hello</font>, we need to include the font element in our run
    // to generate <font color="blue" size="4">hello</font> instead of <font color="blue"><font size="4">hello</font></font>
    RefPtrWillBeRawPtr<Range> range = Range::create(startNode->document(), start, end);
    Element* editableRoot = startNode->rootEditableElement();
    if (startNode != editableRoot) {
        while (editableRoot && startNode->parentNode() != editableRoot && isNodeVisiblyContainedWithin(*startNode->parentNode(), *range))
            startNode = startNode->parentNode();
    }

    applyInlineStyleToNodeRange(style, startNode, pastEndNode);
}

static bool containsNonEditableRegion(Node& node)
{
    if (!node.hasEditableStyle())
        return true;

    Node* sibling = NodeTraversal::nextSkippingChildren(node);
    for (Node* descendent = node.firstChild(); descendent && descendent != sibling; descendent = NodeTraversal::next(*descendent)) {
        if (!descendent->hasEditableStyle())
            return true;
    }

    return false;
}

class InlineRunToApplyStyle {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    InlineRunToApplyStyle(Node* start, Node* end, Node* pastEndNode)
        : start(start)
        , end(end)
        , pastEndNode(pastEndNode)
    {
        ASSERT(start->parentNode() == end->parentNode());
    }

    bool startAndEndAreStillInDocument()
    {
        return start && end && start->inDocument() && end->inDocument();
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(start);
        visitor->trace(end);
        visitor->trace(pastEndNode);
        visitor->trace(positionForStyleComputation);
        visitor->trace(dummyElement);
    }

    RefPtrWillBeMember<Node> start;
    RefPtrWillBeMember<Node> end;
    RefPtrWillBeMember<Node> pastEndNode;
    Position positionForStyleComputation;
    RefPtrWillBeMember<HTMLSpanElement> dummyElement;
    StyleChange change;
};

} // namespace blink

WTF_ALLOW_INIT_WITH_MEM_FUNCTIONS(blink::InlineRunToApplyStyle);

namespace blink {

void ApplyStyleCommand::applyInlineStyleToNodeRange(EditingStyle* style, PassRefPtrWillBeRawPtr<Node> startNode, PassRefPtrWillBeRawPtr<Node> pastEndNode)
{
    if (m_removeOnly)
        return;

    document().updateLayoutIgnorePendingStylesheets();

    WillBeHeapVector<InlineRunToApplyStyle> runs;
    RefPtrWillBeRawPtr<Node> node = startNode;
    for (RefPtrWillBeRawPtr<Node> next; node && node != pastEndNode; node = next) {
        next = NodeTraversal::next(*node);

        if (!node->layoutObject() || !node->hasEditableStyle())
            continue;

        if (!node->layoutObjectIsRichlyEditable() && node->isHTMLElement()) {
            HTMLElement* element = toHTMLElement(node);
            // This is a plaintext-only region. Only proceed if it's fully selected.
            // pastEndNode is the node after the last fully selected node, so if it's inside node then
            // node isn't fully selected.
            if (pastEndNode && pastEndNode->isDescendantOf(element))
                break;
            // Add to this element's inline style and skip over its contents.
            next = NodeTraversal::nextSkippingChildren(*node);
            if (!style->style())
                continue;
            RefPtrWillBeRawPtr<MutableStylePropertySet> inlineStyle = copyStyleOrCreateEmpty(element->inlineStyle());
            inlineStyle->mergeAndOverrideOnConflict(style->style());
            setNodeAttribute(element, styleAttr, AtomicString(inlineStyle->asText()));
            continue;
        }

        if (isBlock(node.get()))
            continue;

        if (node->hasChildren()) {
            if (node->contains(pastEndNode.get()) || containsNonEditableRegion(*node) || !node->parentNode()->hasEditableStyle())
                continue;
            if (editingIgnoresContent(node.get())) {
                next = NodeTraversal::nextSkippingChildren(*node);
                continue;
            }
        }

        Node* runStart = node.get();
        Node* runEnd = node.get();
        Node* sibling = node->nextSibling();
        while (sibling && sibling != pastEndNode && !sibling->contains(pastEndNode.get())
            && (!isBlock(sibling) || isHTMLBRElement(*sibling))
            && !containsNonEditableRegion(*sibling)) {
            runEnd = sibling;
            sibling = runEnd->nextSibling();
        }
        ASSERT(runEnd);
        next = NodeTraversal::nextSkippingChildren(*runEnd);

        Node* pastEndNode = NodeTraversal::nextSkippingChildren(*runEnd);
        if (!shouldApplyInlineStyleToRun(style, runStart, pastEndNode))
            continue;

        runs.append(InlineRunToApplyStyle(runStart, runEnd, pastEndNode));
    }

    for (auto& run : runs) {
        removeConflictingInlineStyleFromRun(style, run.start, run.end, run.pastEndNode);
        if (run.startAndEndAreStillInDocument())
            run.positionForStyleComputation = positionToComputeInlineStyleChange(run.start, run.dummyElement);
    }

    document().updateLayoutIgnorePendingStylesheets();

    for (auto& run : runs) {
        if (run.positionForStyleComputation.isNotNull())
            run.change = StyleChange(style, run.positionForStyleComputation);
    }

    for (auto& run : runs) {
        if (run.dummyElement)
            removeNode(run.dummyElement);
        if (run.startAndEndAreStillInDocument())
            applyInlineStyleChange(run.start.release(), run.end.release(), run.change, AddStyledElement);
    }
}

bool ApplyStyleCommand::isStyledInlineElementToRemove(Element* element) const
{
    return (m_styledInlineElement && element->hasTagName(m_styledInlineElement->tagQName()))
        || (m_isInlineElementToRemoveFunction && m_isInlineElementToRemoveFunction(element));
}

bool ApplyStyleCommand::shouldApplyInlineStyleToRun(EditingStyle* style, Node* runStart, Node* pastEndNode)
{
    ASSERT(style && runStart);

    for (Node* node = runStart; node && node != pastEndNode; node = NodeTraversal::next(*node)) {
        if (node->hasChildren())
            continue;
        // We don't consider m_isInlineElementToRemoveFunction here because we never apply style when m_isInlineElementToRemoveFunction is specified
        if (!style->styleIsPresentInComputedStyleOfNode(node))
            return true;
        if (m_styledInlineElement && !enclosingElementWithTag(positionBeforeNode(node), m_styledInlineElement->tagQName()))
            return true;
    }
    return false;
}

void ApplyStyleCommand::removeConflictingInlineStyleFromRun(EditingStyle* style, RefPtrWillBeMember<Node>& runStart, RefPtrWillBeMember<Node>& runEnd, PassRefPtrWillBeRawPtr<Node> pastEndNode)
{
    ASSERT(runStart && runEnd);
    RefPtrWillBeRawPtr<Node> next = runStart;
    for (RefPtrWillBeRawPtr<Node> node = next; node && node->inDocument() && node != pastEndNode; node = next) {
        if (editingIgnoresContent(node.get())) {
            ASSERT(!node->contains(pastEndNode.get()));
            next = NodeTraversal::nextSkippingChildren(*node);
        } else {
            next = NodeTraversal::next(*node);
        }
        if (!node->isHTMLElement())
            continue;

        HTMLElement& element = toHTMLElement(*node);
        RefPtrWillBeRawPtr<Node> previousSibling = element.previousSibling();
        RefPtrWillBeRawPtr<Node> nextSibling = element.nextSibling();
        RefPtrWillBeRawPtr<ContainerNode> parent = element.parentNode();
        removeInlineStyleFromElement(style, &element, RemoveAlways);
        if (!element.inDocument()) {
            // FIXME: We might need to update the start and the end of current selection here but need a test.
            if (runStart == element)
                runStart = previousSibling ? previousSibling->nextSibling() : parent->firstChild();
            if (runEnd == element)
                runEnd = nextSibling ? nextSibling->previousSibling() : parent->lastChild();
        }
    }
}

bool ApplyStyleCommand::removeInlineStyleFromElement(EditingStyle* style, PassRefPtrWillBeRawPtr<HTMLElement> element, InlineStyleRemovalMode mode, EditingStyle* extractedStyle)
{
    ASSERT(element);

    if (!element->parentNode() || !element->parentNode()->isContentEditable(Node::UserSelectAllIsAlwaysNonEditable))
        return false;

    if (isStyledInlineElementToRemove(element.get())) {
        if (mode == RemoveNone)
            return true;
        if (extractedStyle)
            extractedStyle->mergeInlineStyleOfElement(element.get(), EditingStyle::OverrideValues);
        removeNodePreservingChildren(element);
        return true;
    }

    bool removed = false;
    if (removeImplicitlyStyledElement(style, element.get(), mode, extractedStyle))
        removed = true;

    if (!element->inDocument())
        return removed;

    // If the node was converted to a span, the span may still contain relevant
    // styles which must be removed (e.g. <b style='font-weight: bold'>)
    if (removeCSSStyle(style, element.get(), mode, extractedStyle))
        removed = true;

    return removed;
}

void ApplyStyleCommand::replaceWithSpanOrRemoveIfWithoutAttributes(HTMLElement* elem)
{
    if (hasNoAttributeOrOnlyStyleAttribute(elem, StyleAttributeShouldBeEmpty))
        removeNodePreservingChildren(elem);
    else
        replaceElementWithSpanPreservingChildrenAndAttributes(elem);
}

bool ApplyStyleCommand::removeImplicitlyStyledElement(EditingStyle* style, HTMLElement* element, InlineStyleRemovalMode mode, EditingStyle* extractedStyle)
{
    ASSERT(style);
    if (mode == RemoveNone) {
        ASSERT(!extractedStyle);
        return style->conflictsWithImplicitStyleOfElement(element) || style->conflictsWithImplicitStyleOfAttributes(element);
    }

    ASSERT(mode == RemoveIfNeeded || mode == RemoveAlways);
    if (style->conflictsWithImplicitStyleOfElement(element, extractedStyle, mode == RemoveAlways ? EditingStyle::ExtractMatchingStyle : EditingStyle::DoNotExtractMatchingStyle)) {
        replaceWithSpanOrRemoveIfWithoutAttributes(element);
        return true;
    }

    // unicode-bidi and direction are pushed down separately so don't push down with other styles
    Vector<QualifiedName> attributes;
    if (!style->extractConflictingImplicitStyleOfAttributes(element, extractedStyle ? EditingStyle::PreserveWritingDirection : EditingStyle::DoNotPreserveWritingDirection,
        extractedStyle, attributes, mode == RemoveAlways ? EditingStyle::ExtractMatchingStyle : EditingStyle::DoNotExtractMatchingStyle))
        return false;

    for (const auto& attribute : attributes)
        removeElementAttribute(element, attribute);

    if (isEmptyFontTag(element) || isSpanWithoutAttributesOrUnstyledStyleSpan(element))
        removeNodePreservingChildren(element);

    return true;
}

bool ApplyStyleCommand::removeCSSStyle(EditingStyle* style, HTMLElement* element, InlineStyleRemovalMode mode, EditingStyle* extractedStyle)
{
    ASSERT(style);
    ASSERT(element);

    if (mode == RemoveNone)
        return style->conflictsWithInlineStyleOfElement(element);

    Vector<CSSPropertyID> properties;
    if (!style->conflictsWithInlineStyleOfElement(element, extractedStyle, properties))
        return false;

    // FIXME: We should use a mass-removal function here but we don't have an undoable one yet.
    for (const auto& property : properties)
        removeCSSProperty(element, property);

    if (isSpanWithoutAttributesOrUnstyledStyleSpan(element))
        removeNodePreservingChildren(element);

    return true;
}

HTMLElement* ApplyStyleCommand::highestAncestorWithConflictingInlineStyle(EditingStyle* style, Node* node)
{
    if (!node)
        return 0;

    HTMLElement* result = nullptr;
    Node* unsplittableElement = unsplittableElementForPosition(firstPositionInOrBeforeNode(node));

    for (Node *n = node; n; n = n->parentNode()) {
        if (n->isHTMLElement() && shouldRemoveInlineStyleFromElement(style, toHTMLElement(n)))
            result = toHTMLElement(n);
        // Should stop at the editable root (cannot cross editing boundary) and
        // also stop at the unsplittable element to be consistent with other UAs
        if (n == unsplittableElement)
            break;
    }

    return result;
}

void ApplyStyleCommand::applyInlineStyleToPushDown(Node* node, EditingStyle* style)
{
    ASSERT(node);

    node->document().updateLayoutTreeIfNeeded();

    if (!style || style->isEmpty() || !node->layoutObject() || isHTMLIFrameElement(*node))
        return;

    RefPtrWillBeRawPtr<EditingStyle> newInlineStyle = style;
    if (node->isHTMLElement() && toHTMLElement(node)->inlineStyle()) {
        newInlineStyle = style->copy();
        newInlineStyle->mergeInlineStyleOfElement(toHTMLElement(node), EditingStyle::OverrideValues);
    }

    // Since addInlineStyleIfNeeded can't add styles to block-flow layout objects, add style attribute instead.
    // FIXME: applyInlineStyleToRange should be used here instead.
    if ((node->layoutObject()->isLayoutBlockFlow() || node->hasChildren()) && node->isHTMLElement()) {
        setNodeAttribute(toHTMLElement(node), styleAttr, AtomicString(newInlineStyle->style()->asText()));
        return;
    }

    if (node->layoutObject()->isText() && toLayoutText(node->layoutObject())->isAllCollapsibleWhitespace())
        return;

    // We can't wrap node with the styled element here because new styled element will never be removed if we did.
    // If we modified the child pointer in pushDownInlineStyleAroundNode to point to new style element
    // then we fall into an infinite loop where we keep removing and adding styled element wrapping node.
    addInlineStyleIfNeeded(newInlineStyle.get(), node, node, DoNotAddStyledElement);
}

void ApplyStyleCommand::pushDownInlineStyleAroundNode(EditingStyle* style, Node* targetNode)
{
    HTMLElement* highestAncestor = highestAncestorWithConflictingInlineStyle(style, targetNode);
    if (!highestAncestor)
        return;

    // The outer loop is traversing the tree vertically from highestAncestor to targetNode
    RefPtrWillBeRawPtr<Node> current = highestAncestor;
    // Along the way, styled elements that contain targetNode are removed and accumulated into elementsToPushDown.
    // Each child of the removed element, exclusing ancestors of targetNode, is then wrapped by clones of elements in elementsToPushDown.
    WillBeHeapVector<RefPtrWillBeMember<Element>> elementsToPushDown;
    while (current && current != targetNode && current->contains(targetNode)) {
        NodeVector currentChildren;
        getChildNodes(toContainerNode(*current), currentChildren);
        RefPtrWillBeRawPtr<Element> styledElement = nullptr;
        if (current->isStyledElement() && isStyledInlineElementToRemove(toElement(current))) {
            styledElement = toElement(current);
            elementsToPushDown.append(styledElement);
        }

        RefPtrWillBeRawPtr<EditingStyle> styleToPushDown = EditingStyle::create();
        if (current->isHTMLElement())
            removeInlineStyleFromElement(style, toHTMLElement(current), RemoveIfNeeded, styleToPushDown.get());

        // The inner loop will go through children on each level
        // FIXME: we should aggregate inline child elements together so that we don't wrap each child separately.
        for (const auto& currentChild : currentChildren) {
            Node* child = currentChild.get();
            if (!child->parentNode())
                continue;
            if (!child->contains(targetNode) && elementsToPushDown.size()) {
                for (const auto& element : elementsToPushDown) {
                    RefPtrWillBeRawPtr<Element> wrapper = element->cloneElementWithoutChildren();
                    wrapper->removeAttribute(styleAttr);
                    // Delete id attribute from the second element because the same id cannot be used for more than one element
                    element->removeAttribute(HTMLNames::idAttr);
                    if (isHTMLAnchorElement(element))
                        element->removeAttribute(HTMLNames::nameAttr);
                    surroundNodeRangeWithElement(child, child, wrapper);
                }
            }

            // Apply style to all nodes containing targetNode and their siblings but NOT to targetNode
            // But if we've removed styledElement then go ahead and always apply the style.
            if (child != targetNode || styledElement)
                applyInlineStyleToPushDown(child, styleToPushDown.get());

            // We found the next node for the outer loop (contains targetNode)
            // When reached targetNode, stop the outer loop upon the completion of the current inner loop
            if (child == targetNode || child->contains(targetNode))
                current = child;
        }
    }
}

void ApplyStyleCommand::removeInlineStyle(EditingStyle* style, const Position &start, const Position &end)
{
    ASSERT(start.isNotNull());
    ASSERT(end.isNotNull());
    ASSERT(start.inDocument());
    ASSERT(end.inDocument());
    ASSERT(Position::commonAncestorTreeScope(start, end));
    ASSERT(comparePositions(start, end) <= 0);
    // FIXME: We should assert that start/end are not in the middle of a text node.

    Position pushDownStart = start.downstream();
    // If the pushDownStart is at the end of a text node, then this node is not fully selected.
    // Move it to the next deep quivalent position to avoid removing the style from this node.
    // e.g. if pushDownStart was at Position("hello", 5) in <b>hello<div>world</div></b>, we want Position("world", 0) instead.
    Node* pushDownStartContainer = pushDownStart.containerNode();
    if (pushDownStartContainer && pushDownStartContainer->isTextNode()
        && pushDownStart.computeOffsetInContainerNode() == pushDownStartContainer->maxCharacterOffset())
        pushDownStart = nextVisuallyDistinctCandidate(pushDownStart);
    Position pushDownEnd = end.upstream();
    // If pushDownEnd is at the start of a text node, then this node is not fully selected.
    // Move it to the previous deep equivalent position to avoid removing the style from this node.
    Node* pushDownEndContainer = pushDownEnd.containerNode();
    if (pushDownEndContainer && pushDownEndContainer->isTextNode() && !pushDownEnd.computeOffsetInContainerNode())
        pushDownEnd = previousVisuallyDistinctCandidate(pushDownEnd);

    pushDownInlineStyleAroundNode(style, pushDownStart.deprecatedNode());
    pushDownInlineStyleAroundNode(style, pushDownEnd.deprecatedNode());

    // The s and e variables store the positions used to set the ending selection after style removal
    // takes place. This will help callers to recognize when either the start node or the end node
    // are removed from the document during the work of this function.
    // If pushDownInlineStyleAroundNode has pruned start.deprecatedNode() or end.deprecatedNode(),
    // use pushDownStart or pushDownEnd instead, which pushDownInlineStyleAroundNode won't prune.
    Position s = start.isNull() || start.isOrphan() ? pushDownStart : start;
    Position e = end.isNull() || end.isOrphan() ? pushDownEnd : end;

    // Current ending selection resetting algorithm assumes |start| and |end|
    // are in a same DOM tree even if they are not in document.
    if (!Position::commonAncestorTreeScope(start, end))
        return;

    RefPtrWillBeRawPtr<Node> node = start.deprecatedNode();
    while (node) {
        RefPtrWillBeRawPtr<Node> next = nullptr;
        if (editingIgnoresContent(node.get())) {
            ASSERT(node == end.deprecatedNode() || !node->contains(end.deprecatedNode()));
            next = NodeTraversal::nextSkippingChildren(*node);
        } else {
            next = NodeTraversal::next(*node);
        }
        if (node->isHTMLElement() && elementFullySelected(toHTMLElement(*node), start, end)) {
            RefPtrWillBeRawPtr<HTMLElement> elem = toHTMLElement(node);
            RefPtrWillBeRawPtr<Node> prev = NodeTraversal::previousPostOrder(*elem);
            RefPtrWillBeRawPtr<Node> next = NodeTraversal::next(*elem);
            RefPtrWillBeRawPtr<EditingStyle> styleToPushDown = nullptr;
            RefPtrWillBeRawPtr<Node> childNode = nullptr;
            if (isStyledInlineElementToRemove(elem.get())) {
                styleToPushDown = EditingStyle::create();
                childNode = elem->firstChild();
            }

            removeInlineStyleFromElement(style, elem.get(), RemoveIfNeeded, styleToPushDown.get());
            if (!elem->inDocument()) {
                if (s.deprecatedNode() == elem) {
                    // Since elem must have been fully selected, and it is at the start
                    // of the selection, it is clear we can set the new s offset to 0.
                    ASSERT(s.anchorType() == PositionAnchorType::BeforeAnchor || s.anchorType() == PositionAnchorType::BeforeChildren || s.offsetInContainerNode() <= 0);
                    s = firstPositionInOrBeforeNode(next.get());
                }
                if (e.deprecatedNode() == elem) {
                    // Since elem must have been fully selected, and it is at the end
                    // of the selection, it is clear we can set the new e offset to
                    // the max range offset of prev.
                    ASSERT(s.anchorType() == PositionAnchorType::AfterAnchor || !offsetIsBeforeLastNodeOffset(s.offsetInContainerNode(), s.containerNode()));
                    e = lastPositionInOrAfterNode(prev.get());
                }
            }

            if (styleToPushDown) {
                for (; childNode; childNode = childNode->nextSibling())
                    applyInlineStyleToPushDown(childNode.get(), styleToPushDown.get());
            }
        }
        if (node == end.deprecatedNode())
            break;
        node = next;
    }

    updateStartEnd(s, e);
}

bool ApplyStyleCommand::elementFullySelected(HTMLElement& element, const Position& start, const Position& end) const
{
    // The tree may have changed and Position::upstream() relies on an up-to-date layout.
    element.document().updateLayoutIgnorePendingStylesheets();

    return comparePositions(firstPositionInOrBeforeNode(&element), start) >= 0
        && comparePositions(lastPositionInOrAfterNode(&element).upstream(), end) <= 0;
}

void ApplyStyleCommand::splitTextAtStart(const Position& start, const Position& end)
{
    ASSERT(start.containerNode()->isTextNode());

    Position newEnd;
    if (end.anchorType() == PositionAnchorType::OffsetInAnchor && start.containerNode() == end.containerNode())
        newEnd = Position(end.containerText(), end.offsetInContainerNode() - start.offsetInContainerNode());
    else
        newEnd = end;

    RefPtrWillBeRawPtr<Text> text = start.containerText();
    splitTextNode(text, start.offsetInContainerNode());
    updateStartEnd(firstPositionInNode(text.get()), newEnd);
}

void ApplyStyleCommand::splitTextAtEnd(const Position& start, const Position& end)
{
    ASSERT(end.containerNode()->isTextNode());

    bool shouldUpdateStart = start.anchorType() == PositionAnchorType::OffsetInAnchor && start.containerNode() == end.containerNode();
    Text* text = toText(end.deprecatedNode());
    splitTextNode(text, end.offsetInContainerNode());

    Node* prevNode = text->previousSibling();
    if (!prevNode || !prevNode->isTextNode())
        return;

    Position newStart = shouldUpdateStart ? Position(toText(prevNode), start.offsetInContainerNode()) : start;
    updateStartEnd(newStart, lastPositionInNode(prevNode));
}

void ApplyStyleCommand::splitTextElementAtStart(const Position& start, const Position& end)
{
    ASSERT(start.containerNode()->isTextNode());

    Position newEnd;
    if (start.containerNode() == end.containerNode())
        newEnd = Position(end.containerText(), end.offsetInContainerNode() - start.offsetInContainerNode());
    else
        newEnd = end;

    splitTextNodeContainingElement(start.containerText(), start.offsetInContainerNode());
    updateStartEnd(positionBeforeNode(start.containerNode()), newEnd);
}

void ApplyStyleCommand::splitTextElementAtEnd(const Position& start, const Position& end)
{
    ASSERT(end.containerNode()->isTextNode());

    bool shouldUpdateStart = start.containerNode() == end.containerNode();
    splitTextNodeContainingElement(end.containerText(), end.offsetInContainerNode());

    Node* parentElement = end.containerNode()->parentNode();
    if (!parentElement || !parentElement->previousSibling())
        return;
    Node* firstTextNode = parentElement->previousSibling()->lastChild();
    if (!firstTextNode || !firstTextNode->isTextNode())
        return;

    Position newStart = shouldUpdateStart ? Position(toText(firstTextNode), start.offsetInContainerNode()) : start;
    updateStartEnd(newStart, positionAfterNode(firstTextNode));
}

bool ApplyStyleCommand::shouldSplitTextElement(Element* element, EditingStyle* style)
{
    if (!element || !element->isHTMLElement())
        return false;

    return shouldRemoveInlineStyleFromElement(style, toHTMLElement(element));
}

bool ApplyStyleCommand::isValidCaretPositionInTextNode(const Position& position)
{
    ASSERT(position.isNotNull());

    Node* node = position.containerNode();
    if (position.anchorType() != PositionAnchorType::OffsetInAnchor || !node->isTextNode())
        return false;
    int offsetInText = position.offsetInContainerNode();
    return offsetInText > caretMinOffset(node) && offsetInText < caretMaxOffset(node);
}

bool ApplyStyleCommand::mergeStartWithPreviousIfIdentical(const Position& start, const Position& end)
{
    Node* startNode = start.containerNode();
    int startOffset = start.computeOffsetInContainerNode();
    if (startOffset)
        return false;

    if (isAtomicNode(startNode)) {
        // note: prior siblings could be unrendered elements. it's silly to miss the
        // merge opportunity just for that.
        if (startNode->previousSibling())
            return false;

        startNode = startNode->parentNode();
    }

    if (!startNode->isElementNode())
        return false;

    Node* previousSibling = startNode->previousSibling();

    if (previousSibling && areIdenticalElements(startNode, previousSibling)) {
        Element* previousElement = toElement(previousSibling);
        Element* element = toElement(startNode);
        Node* startChild = element->firstChild();
        ASSERT(startChild);
        mergeIdenticalElements(previousElement, element);

        int startOffsetAdjustment = startChild->nodeIndex();
        int endOffsetAdjustment = startNode == end.deprecatedNode() ? startOffsetAdjustment : 0;
        updateStartEnd(Position(startNode, startOffsetAdjustment),
            Position(end.deprecatedNode(), end.deprecatedEditingOffset() + endOffsetAdjustment));
        return true;
    }

    return false;
}

bool ApplyStyleCommand::mergeEndWithNextIfIdentical(const Position& start, const Position& end)
{
    Node* endNode = end.containerNode();

    if (isAtomicNode(endNode)) {
        int endOffset = end.computeOffsetInContainerNode();
        if (offsetIsBeforeLastNodeOffset(endOffset, endNode))
            return false;

        if (end.deprecatedNode()->nextSibling())
            return false;

        endNode = end.deprecatedNode()->parentNode();
    }

    if (!endNode->isElementNode() || isHTMLBRElement(*endNode))
        return false;

    Node* nextSibling = endNode->nextSibling();
    if (nextSibling && areIdenticalElements(endNode, nextSibling)) {
        Element* nextElement = toElement(nextSibling);
        Element* element = toElement(endNode);
        Node* nextChild = nextElement->firstChild();

        mergeIdenticalElements(element, nextElement);

        bool shouldUpdateStart = start.containerNode() == endNode;
        int endOffset = nextChild ? nextChild->nodeIndex() : nextElement->childNodes()->length();
        updateStartEnd(shouldUpdateStart ? Position(nextElement, start.offsetInContainerNode()) : start,
            Position(nextElement, endOffset));
        return true;
    }

    return false;
}

void ApplyStyleCommand::surroundNodeRangeWithElement(PassRefPtrWillBeRawPtr<Node> passedStartNode, PassRefPtrWillBeRawPtr<Node> endNode, PassRefPtrWillBeRawPtr<Element> elementToInsert)
{
    ASSERT(passedStartNode);
    ASSERT(endNode);
    ASSERT(elementToInsert);
    RefPtrWillBeRawPtr<Node> node = passedStartNode;
    RefPtrWillBeRawPtr<Element> element = elementToInsert;

    insertNodeBefore(element, node);

    while (node) {
        RefPtrWillBeRawPtr<Node> next = node->nextSibling();
        if (node->isContentEditable(Node::UserSelectAllIsAlwaysNonEditable)) {
            removeNode(node);
            appendNode(node, element);
        }
        if (node == endNode)
            break;
        node = next;
    }

    RefPtrWillBeRawPtr<Node> nextSibling = element->nextSibling();
    RefPtrWillBeRawPtr<Node> previousSibling = element->previousSibling();
    if (nextSibling && nextSibling->isElementNode() && nextSibling->hasEditableStyle()
        && areIdenticalElements(element.get(), toElement(nextSibling)))
        mergeIdenticalElements(element.get(), toElement(nextSibling));

    if (previousSibling && previousSibling->isElementNode() && previousSibling->hasEditableStyle()) {
        Node* mergedElement = previousSibling->nextSibling();
        if (mergedElement->isElementNode() && mergedElement->hasEditableStyle()
            && areIdenticalElements(toElement(previousSibling), toElement(mergedElement)))
            mergeIdenticalElements(toElement(previousSibling), toElement(mergedElement));
    }

    // FIXME: We should probably call updateStartEnd if the start or end was in the node
    // range so that the endingSelection() is canonicalized.  See the comments at the end of
    // VisibleSelection::validate().
}

void ApplyStyleCommand::addBlockStyle(const StyleChange& styleChange, HTMLElement* block)
{
    // Do not check for legacy styles here. Those styles, like <B> and <I>, only apply for
    // inline content.
    if (!block)
        return;

    String cssStyle = styleChange.cssStyle();
    StringBuilder cssText;
    cssText.append(cssStyle);
    if (const StylePropertySet* decl = block->inlineStyle()) {
        if (!cssStyle.isEmpty())
            cssText.append(' ');
        cssText.append(decl->asText());
    }
    setNodeAttribute(block, styleAttr, cssText.toAtomicString());
}

void ApplyStyleCommand::addInlineStyleIfNeeded(EditingStyle* style, PassRefPtrWillBeRawPtr<Node> passedStart, PassRefPtrWillBeRawPtr<Node> passedEnd, EAddStyledElement addStyledElement)
{
    if (!passedStart || !passedEnd || !passedStart->inDocument() || !passedEnd->inDocument())
        return;

    RefPtrWillBeRawPtr<Node> start = passedStart;
    RefPtrWillBeMember<HTMLSpanElement> dummyElement = nullptr;
    StyleChange styleChange(style, positionToComputeInlineStyleChange(start, dummyElement));

    if (dummyElement)
        removeNode(dummyElement);

    applyInlineStyleChange(start, passedEnd, styleChange, addStyledElement);
}

Position ApplyStyleCommand::positionToComputeInlineStyleChange(PassRefPtrWillBeRawPtr<Node> startNode, RefPtrWillBeMember<HTMLSpanElement>& dummyElement)
{
    // It's okay to obtain the style at the startNode because we've removed all relevant styles from the current run.
    if (!startNode->isElementNode()) {
        dummyElement = createStyleSpanElement(document());
        insertNodeAt(dummyElement, positionBeforeNode(startNode.get()));
        return positionBeforeNode(dummyElement.get());
    }

    return firstPositionInOrBeforeNode(startNode.get());
}

void ApplyStyleCommand::applyInlineStyleChange(PassRefPtrWillBeRawPtr<Node> passedStart, PassRefPtrWillBeRawPtr<Node> passedEnd, StyleChange& styleChange, EAddStyledElement addStyledElement)
{
    RefPtrWillBeRawPtr<Node> startNode = passedStart;
    RefPtrWillBeRawPtr<Node> endNode = passedEnd;
    ASSERT(startNode->inDocument());
    ASSERT(endNode->inDocument());

    // Find appropriate font and span elements top-down.
    HTMLFontElement* fontContainer = nullptr;
    HTMLElement* styleContainer = nullptr;
    for (Node* container = startNode.get(); container && startNode == endNode; container = container->firstChild()) {
        if (isHTMLFontElement(*container))
            fontContainer = toHTMLFontElement(container);
        bool styleContainerIsNotSpan = !isHTMLSpanElement(styleContainer);
        if (container->isHTMLElement()) {
            HTMLElement* containerElement = toHTMLElement(container);
            if (isHTMLSpanElement(*containerElement) || (styleContainerIsNotSpan && containerElement->hasChildren()))
                styleContainer = toHTMLElement(container);
        }
        if (!container->hasChildren())
            break;
        startNode = container->firstChild();
        endNode = container->lastChild();
    }

    // Font tags need to go outside of CSS so that CSS font sizes override leagcy font sizes.
    if (styleChange.applyFontColor() || styleChange.applyFontFace() || styleChange.applyFontSize()) {
        if (fontContainer) {
            if (styleChange.applyFontColor())
                setNodeAttribute(fontContainer, colorAttr, AtomicString(styleChange.fontColor()));
            if (styleChange.applyFontFace())
                setNodeAttribute(fontContainer, faceAttr, AtomicString(styleChange.fontFace()));
            if (styleChange.applyFontSize())
                setNodeAttribute(fontContainer, sizeAttr, AtomicString(styleChange.fontSize()));
        } else {
            RefPtrWillBeRawPtr<HTMLFontElement> fontElement = createFontElement(document());
            if (styleChange.applyFontColor())
                fontElement->setAttribute(colorAttr, AtomicString(styleChange.fontColor()));
            if (styleChange.applyFontFace())
                fontElement->setAttribute(faceAttr, AtomicString(styleChange.fontFace()));
            if (styleChange.applyFontSize())
                fontElement->setAttribute(sizeAttr, AtomicString(styleChange.fontSize()));
            surroundNodeRangeWithElement(startNode, endNode, fontElement.get());
        }
    }

    if (styleChange.cssStyle().length()) {
        if (styleContainer) {
            if (const StylePropertySet* existingStyle = styleContainer->inlineStyle()) {
                String existingText = existingStyle->asText();
                StringBuilder cssText;
                cssText.append(existingText);
                if (!existingText.isEmpty())
                    cssText.append(' ');
                cssText.append(styleChange.cssStyle());
                setNodeAttribute(styleContainer, styleAttr, cssText.toAtomicString());
            } else {
                setNodeAttribute(styleContainer, styleAttr, AtomicString(styleChange.cssStyle()));
            }
        } else {
            RefPtrWillBeRawPtr<HTMLSpanElement> styleElement = createStyleSpanElement(document());
            styleElement->setAttribute(styleAttr, AtomicString(styleChange.cssStyle()));
            surroundNodeRangeWithElement(startNode, endNode, styleElement.release());
        }
    }

    if (styleChange.applyBold())
        surroundNodeRangeWithElement(startNode, endNode, createHTMLElement(document(), bTag));

    if (styleChange.applyItalic())
        surroundNodeRangeWithElement(startNode, endNode, createHTMLElement(document(), iTag));

    if (styleChange.applyUnderline())
        surroundNodeRangeWithElement(startNode, endNode, createHTMLElement(document(), uTag));

    if (styleChange.applyLineThrough())
        surroundNodeRangeWithElement(startNode, endNode, createHTMLElement(document(), strikeTag));

    if (styleChange.applySubscript())
        surroundNodeRangeWithElement(startNode, endNode, createHTMLElement(document(), subTag));
    else if (styleChange.applySuperscript())
        surroundNodeRangeWithElement(startNode, endNode, createHTMLElement(document(), supTag));

    if (m_styledInlineElement && addStyledElement == AddStyledElement)
        surroundNodeRangeWithElement(startNode, endNode, m_styledInlineElement->cloneElementWithoutChildren());
}

float ApplyStyleCommand::computedFontSize(Node* node)
{
    if (!node)
        return 0;

    RefPtrWillBeRawPtr<CSSComputedStyleDeclaration> style = CSSComputedStyleDeclaration::create(node);
    if (!style)
        return 0;

    RefPtrWillBeRawPtr<CSSPrimitiveValue> value = static_pointer_cast<CSSPrimitiveValue>(style->getPropertyCSSValue(CSSPropertyFontSize));
    if (!value)
        return 0;

    ASSERT(value->primitiveType() == CSSPrimitiveValue::CSS_PX);
    return value->getFloatValue();
}

void ApplyStyleCommand::joinChildTextNodes(ContainerNode* node, const Position& start, const Position& end)
{
    if (!node)
        return;

    Position newStart = start;
    Position newEnd = end;

    WillBeHeapVector<RefPtrWillBeMember<Text>> textNodes;
    for (Node* curr = node->firstChild(); curr; curr = curr->nextSibling()) {
        if (!curr->isTextNode())
            continue;

        textNodes.append(toText(curr));
    }

    for (const auto& textNode : textNodes) {
        Text* childText = textNode.get();
        Node* next = childText->nextSibling();
        if (!next || !next->isTextNode())
            continue;

        Text* nextText = toText(next);
        if (start.anchorType() == PositionAnchorType::OffsetInAnchor && next == start.containerNode())
            newStart = Position(childText, childText->length() + start.offsetInContainerNode());
        if (end.anchorType() == PositionAnchorType::OffsetInAnchor && next == end.containerNode())
            newEnd = Position(childText, childText->length() + end.offsetInContainerNode());
        String textToMove = nextText->data();
        insertTextIntoNode(childText, childText->length(), textToMove);
        removeNode(next);
        // don't move child node pointer. it may want to merge with more text nodes.
    }

    updateStartEnd(newStart, newEnd);
}

DEFINE_TRACE(ApplyStyleCommand)
{
    visitor->trace(m_style);
    visitor->trace(m_start);
    visitor->trace(m_end);
    visitor->trace(m_styledInlineElement);
    CompositeEditCommand::trace(visitor);
}

}
