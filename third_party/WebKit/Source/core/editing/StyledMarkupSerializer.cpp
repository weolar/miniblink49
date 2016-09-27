/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009, 2010, 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011 Igalia S.L.
 * Copyright (C) 2011 Motorola Mobility. All rights reserved.
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
#include "core/editing/StyledMarkupSerializer.h"

#include "core/css/StylePropertySet.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/editing/EditingStyle.h"
#include "core/editing/VisibleSelection.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/editing/markup.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLElement.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

namespace {

template<typename Strategy>
TextOffset toTextOffset(const PositionAlgorithm<Strategy>& position)
{
    if (position.isNull())
        return TextOffset();

    if (!position.containerNode()->isTextNode())
        return TextOffset();

    return TextOffset(toText(position.containerNode()), position.offsetInContainerNode());
}

template<typename EditingStrategy>
static bool handleSelectionBoundary(const Node&);

template<>
bool handleSelectionBoundary<EditingStrategy>(const Node&)
{
    return false;
}

template<>
bool handleSelectionBoundary<EditingInComposedTreeStrategy>(const Node& node)
{
    if (!node.isElementNode())
        return false;
    ElementShadow* shadow = toElement(node).shadow();
    if (!shadow)
        return false;
    return shadow->youngestShadowRoot()->type() == ShadowRootType::UserAgent;
}

} // namespace

using namespace HTMLNames;

template<typename Strategy>
class StyledMarkupTraverser {
    WTF_MAKE_NONCOPYABLE(StyledMarkupTraverser);
    STACK_ALLOCATED();
public:
    StyledMarkupTraverser();
    StyledMarkupTraverser(StyledMarkupAccumulator*, Node*);

    Node* traverse(Node*, Node*);
    void wrapWithNode(ContainerNode&, PassRefPtrWillBeRawPtr<EditingStyle>);
    RefPtrWillBeRawPtr<EditingStyle> createInlineStyleIfNeeded(Node&);

private:
    bool shouldAnnotate() const;
    bool convertBlocksToInlines() const;
    void appendStartMarkup(Node&);
    void appendEndMarkup(Node&);
    RefPtrWillBeRawPtr<EditingStyle> createInlineStyle(Element&);
    bool needsInlineStyle(const Element&);
    bool shouldApplyWrappingStyle(const Node&) const;

    StyledMarkupAccumulator* m_accumulator;
    RefPtrWillBeMember<Node> m_lastClosed;
    RefPtrWillBeMember<EditingStyle> m_wrappingStyle;
};

template<typename Strategy>
bool StyledMarkupTraverser<Strategy>::shouldAnnotate() const
{
    return m_accumulator->shouldAnnotate();
}

template<typename Strategy>
bool StyledMarkupTraverser<Strategy>::convertBlocksToInlines() const
{
    return m_accumulator->convertBlocksToInlines();
}

template<typename Strategy>
StyledMarkupSerializer<Strategy>::StyledMarkupSerializer(EAbsoluteURLs shouldResolveURLs, EAnnotateForInterchange shouldAnnotate, const PositionAlgorithm<Strategy>& start, const PositionAlgorithm<Strategy>& end, Node* highestNodeToBeSerialized, ConvertBlocksToInlines convertBlocksToInlines)
    : m_start(start)
    , m_end(end)
    , m_shouldResolveURLs(shouldResolveURLs)
    , m_shouldAnnotate(shouldAnnotate)
    , m_highestNodeToBeSerialized(highestNodeToBeSerialized)
    , m_convertBlocksToInlines(convertBlocksToInlines)
    , m_lastClosed(highestNodeToBeSerialized)
{
}

static bool needInterchangeNewlineAfter(const VisiblePosition& v)
{
    VisiblePosition next = v.next();
    Node* upstreamNode = next.deepEquivalent().upstream().deprecatedNode();
    Node* downstreamNode = v.deepEquivalent().downstream().deprecatedNode();
    // Add an interchange newline if a paragraph break is selected and a br won't already be added to the markup to represent it.
    return isEndOfParagraph(v) && isStartOfParagraph(next) && !(isHTMLBRElement(*upstreamNode) && upstreamNode == downstreamNode);
}

static bool needInterchangeNewlineAt(const VisiblePosition& v)
{
    // FIXME: |v.previous()| works on a DOM tree. We need to fix this to work on
    // a composed tree.
    return needInterchangeNewlineAfter(v.previous());
}

template<typename Strategy>
static bool areSameRanges(Node* node, const PositionAlgorithm<Strategy>& startPosition, const PositionAlgorithm<Strategy>& endPosition)
{
    ASSERT(node);
    Position otherStartPosition;
    Position otherEndPosition;
    VisibleSelection::selectionFromContentsOfNode(node).toNormalizedPositions(otherStartPosition, otherEndPosition);
    return toPositionInDOMTree(startPosition) == otherStartPosition && toPositionInDOMTree(endPosition) == otherEndPosition;
}

static PassRefPtrWillBeRawPtr<EditingStyle> styleFromMatchedRulesAndInlineDecl(const HTMLElement* element)
{
    RefPtrWillBeRawPtr<EditingStyle> style = EditingStyle::create(element->inlineStyle());
    // FIXME: Having to const_cast here is ugly, but it is quite a bit of work to untangle
    // the non-const-ness of styleFromMatchedRulesForElement.
    style->mergeStyleFromRules(const_cast<HTMLElement*>(element));
    return style.release();
}

template<typename Strategy>
String StyledMarkupSerializer<Strategy>::createMarkup()
{
    StyledMarkupAccumulator markupAccumulator(m_shouldResolveURLs, toTextOffset(m_start.parentAnchoredEquivalent()), toTextOffset(m_end.parentAnchoredEquivalent()), m_start.document(), m_shouldAnnotate, m_convertBlocksToInlines);

    Node* pastEnd = m_end.nodeAsRangePastLastNode();

    Node* firstNode = m_start.nodeAsRangeFirstNode();
    VisiblePosition visibleStart(m_start);
    VisiblePosition visibleEnd(m_end);
    if (shouldAnnotate() && needInterchangeNewlineAfter(visibleStart)) {
        markupAccumulator.appendInterchangeNewline();
        if (visibleStart == visibleEnd.previous())
            return markupAccumulator.takeResults();

        firstNode = visibleStart.next().deepEquivalent().deprecatedNode();

        if (pastEnd && PositionAlgorithm<Strategy>::beforeNode(firstNode).compareTo(PositionAlgorithm<Strategy>::beforeNode(pastEnd)) >= 0) {
            // This condition hits in editing/pasteboard/copy-display-none.html.
            return markupAccumulator.takeResults();
        }
    }

    if (!m_lastClosed)
        m_lastClosed = StyledMarkupTraverser<Strategy>().traverse(firstNode, pastEnd);
    StyledMarkupTraverser<Strategy> traverser(&markupAccumulator, m_lastClosed);
    Node* lastClosed = traverser.traverse(firstNode, pastEnd);

    if (m_highestNodeToBeSerialized && lastClosed) {
        // TODO(hajimehoshi): This is calculated at createMarkupInternal too.
        Node* commonAncestor = Strategy::commonAncestor(*m_start.containerNode(), *m_end.containerNode());
        ASSERT(commonAncestor);
        HTMLBodyElement* body = toHTMLBodyElement(enclosingElementWithTag(firstPositionInNode(commonAncestor), bodyTag));
        HTMLBodyElement* fullySelectedRoot = nullptr;
        // FIXME: Do this for all fully selected blocks, not just the body.
        if (body && areSameRanges(body, m_start, m_end))
            fullySelectedRoot = body;

        // Also include all of the ancestors of lastClosed up to this special ancestor.
        // FIXME: What is ancestor?
        for (ContainerNode* ancestor = Strategy::parent(*lastClosed); ancestor; ancestor = Strategy::parent(*ancestor)) {
            if (ancestor == fullySelectedRoot && !markupAccumulator.convertBlocksToInlines()) {
                RefPtrWillBeRawPtr<EditingStyle> fullySelectedRootStyle = styleFromMatchedRulesAndInlineDecl(fullySelectedRoot);

                // Bring the background attribute over, but not as an attribute because a background attribute on a div
                // appears to have no effect.
                if ((!fullySelectedRootStyle || !fullySelectedRootStyle->style() || !fullySelectedRootStyle->style()->getPropertyCSSValue(CSSPropertyBackgroundImage))
                    && fullySelectedRoot->hasAttribute(backgroundAttr))
                    fullySelectedRootStyle->style()->setProperty(CSSPropertyBackgroundImage, "url('" + fullySelectedRoot->getAttribute(backgroundAttr) + "')");

                if (fullySelectedRootStyle->style()) {
                    // Reset the CSS properties to avoid an assertion error in addStyleMarkup().
                    // This assertion is caused at least when we select all text of a <body> element whose
                    // 'text-decoration' property is "inherit", and copy it.
                    if (!propertyMissingOrEqualToNone(fullySelectedRootStyle->style(), CSSPropertyTextDecoration))
                        fullySelectedRootStyle->style()->setProperty(CSSPropertyTextDecoration, CSSValueNone);
                    if (!propertyMissingOrEqualToNone(fullySelectedRootStyle->style(), CSSPropertyWebkitTextDecorationsInEffect))
                        fullySelectedRootStyle->style()->setProperty(CSSPropertyWebkitTextDecorationsInEffect, CSSValueNone);
                    markupAccumulator.wrapWithStyleNode(fullySelectedRootStyle->style());
                }
            } else {
                RefPtrWillBeRawPtr<EditingStyle> style = traverser.createInlineStyleIfNeeded(*ancestor);
                // Since this node and all the other ancestors are not in the selection we want
                // styles that affect the exterior of the node not to be not included.
                // If the node is not fully selected by the range, then we don't want to keep styles that affect its relationship to the nodes around it
                // only the ones that affect it and the nodes within it.
                if (style && style->style())
                    style->style()->removeProperty(CSSPropertyFloat);
                traverser.wrapWithNode(*ancestor, style);
            }

            if (ancestor == m_highestNodeToBeSerialized)
                break;
        }
    }

    // FIXME: The interchange newline should be placed in the block that it's in, not after all of the content, unconditionally.
    if (shouldAnnotate() && needInterchangeNewlineAt(visibleEnd))
        markupAccumulator.appendInterchangeNewline();

    return markupAccumulator.takeResults();
}

template<typename Strategy>
StyledMarkupTraverser<Strategy>::StyledMarkupTraverser()
    : StyledMarkupTraverser(nullptr, nullptr)
{
}

template<typename Strategy>
StyledMarkupTraverser<Strategy>::StyledMarkupTraverser(StyledMarkupAccumulator* accumulator, Node* lastClosed)
    : m_accumulator(accumulator)
    , m_lastClosed(lastClosed)
    , m_wrappingStyle(nullptr)
{
    if (!m_accumulator) {
        ASSERT(!m_lastClosed);
        return;
    }
    if (!m_lastClosed)
        return;
    ContainerNode* parent = Strategy::parent(*m_lastClosed);
    if (!parent)
        return;
    if (shouldAnnotate()) {
        m_wrappingStyle = EditingStyle::wrappingStyleForAnnotatedSerialization(parent);
        return;
    }
    m_wrappingStyle = EditingStyle::wrappingStyleForSerialization(parent);
}

template<typename Strategy>
Node* StyledMarkupTraverser<Strategy>::traverse(Node* startNode, Node* pastEnd)
{
    WillBeHeapVector<RawPtrWillBeMember<ContainerNode>> ancestorsToClose;
    Node* next;
    Node* lastClosed = nullptr;
    for (Node* n = startNode; n != pastEnd; n = next) {
        // According to <rdar://problem/5730668>, it is possible for n to blow
        // past pastEnd and become null here. This shouldn't be possible.
        // This null check will prevent crashes (but create too much markup)
        // and the ASSERT will hopefully lead us to understanding the problem.
        ASSERT(n);
        if (!n)
            break;

        // If |n| is a selection boundary such as <input>, traverse the child
        // nodes in the DOM tree instead of the composed tree.
        if (handleSelectionBoundary<Strategy>(*n)) {
            lastClosed = StyledMarkupTraverser<EditingStrategy>(m_accumulator, m_lastClosed.get()).traverse(n, EditingStrategy::nextSkippingChildren(*n));
            next = EditingInComposedTreeStrategy::nextSkippingChildren(*n);
        } else {
            next = Strategy::next(*n);
            if (isBlock(n) && canHaveChildrenForEditing(n) && next == pastEnd) {
                // Don't write out empty block containers that aren't fully selected.
                continue;
            }

            if (!n->layoutObject() && !enclosingElementWithTag(firstPositionInOrBeforeNode(n), selectTag)) {
                next = Strategy::nextSkippingChildren(*n);
                // Don't skip over pastEnd.
                if (pastEnd && Strategy::isDescendantOf(*pastEnd, *n))
                    next = pastEnd;
            } else {
                // Add the node to the markup if we're not skipping the descendants
                appendStartMarkup(*n);

                // If node has no children, close the tag now.
                if (Strategy::hasChildren(*n)) {
                    ancestorsToClose.append(toContainerNode(n));
                    continue;
                }
                appendEndMarkup(*n);
                lastClosed = n;
            }
        }

        // If we didn't insert open tag and there's no more siblings or we're at the end of the traversal, take care of ancestors.
        // FIXME: What happens if we just inserted open tag and reached the end?
        if (Strategy::nextSibling(*n) && next != pastEnd)
            continue;

        // Close up the ancestors.
        while (!ancestorsToClose.isEmpty()) {
            ContainerNode* ancestor = ancestorsToClose.last();
            ASSERT(ancestor);
            if (next != pastEnd && Strategy::isDescendantOf(*next, *ancestor))
                break;
            // Not at the end of the range, close ancestors up to sibling of next node.
            appendEndMarkup(*ancestor);
            lastClosed = ancestor;
            ancestorsToClose.removeLast();
        }

        // Surround the currently accumulated markup with markup for ancestors we never opened as we leave the subtree(s) rooted at those ancestors.
        ContainerNode* nextParent = next ? Strategy::parent(*next) : nullptr;
        if (next == pastEnd || n == nextParent)
            continue;

        ASSERT(n);
        Node* lastAncestorClosedOrSelf = (lastClosed && Strategy::isDescendantOf(*n, *lastClosed)) ? lastClosed : n;
        for (ContainerNode* parent = Strategy::parent(*lastAncestorClosedOrSelf); parent && parent != nextParent; parent = Strategy::parent(*parent)) {
            // All ancestors that aren't in the ancestorsToClose list should either be a) unrendered:
            if (!parent->layoutObject())
                continue;
            // or b) ancestors that we never encountered during a pre-order traversal starting at startNode:
            ASSERT(startNode);
            ASSERT(Strategy::isDescendantOf(*startNode, *parent));
            RefPtrWillBeRawPtr<EditingStyle> style = createInlineStyleIfNeeded(*parent);
            wrapWithNode(*parent, style);
            lastClosed = parent;
        }
    }

    return lastClosed;
}

template<typename Strategy>
bool StyledMarkupTraverser<Strategy>::needsInlineStyle(const Element& element)
{
    if (!element.isHTMLElement())
        return false;
    if (shouldAnnotate())
        return true;
    return convertBlocksToInlines() && isBlock(&element);
}

template<typename Strategy>
void StyledMarkupTraverser<Strategy>::wrapWithNode(ContainerNode& node, PassRefPtrWillBeRawPtr<EditingStyle> style)
{
    if (!m_accumulator)
        return;
    StringBuilder markup;
    if (node.isDocumentNode()) {
        MarkupFormatter::appendXMLDeclaration(markup, toDocument(node));
        m_accumulator->pushMarkup(markup.toString());
        return;
    }
    if (!node.isElementNode())
        return;
    Element& element = toElement(node);
    if (shouldApplyWrappingStyle(element) || needsInlineStyle(element))
        m_accumulator->appendElementWithInlineStyle(markup, element, style);
    else
        m_accumulator->appendElement(markup, element);
    m_accumulator->pushMarkup(markup.toString());
    m_accumulator->appendEndTag(toElement(node));
}

template<typename Strategy>
RefPtrWillBeRawPtr<EditingStyle> StyledMarkupTraverser<Strategy>::createInlineStyleIfNeeded(Node& node)
{
    if (!m_accumulator)
        return nullptr;
    if (!node.isElementNode())
        return nullptr;
    RefPtrWillBeRawPtr<EditingStyle> inlineStyle = createInlineStyle(toElement(node));
    if (convertBlocksToInlines() && isBlock(&node))
        inlineStyle->forceInline();
    return inlineStyle;
}

template<typename Strategy>
void StyledMarkupTraverser<Strategy>::appendStartMarkup(Node& node)
{
    if (!m_accumulator)
        return;
    switch (node.nodeType()) {
    case Node::TEXT_NODE: {
        Text& text = toText(node);
        if (text.parentElement() && isHTMLTextAreaElement(text.parentElement())) {
            m_accumulator->appendText(text);
            break;
        }
        RefPtrWillBeRawPtr<EditingStyle> inlineStyle = nullptr;
        if (shouldApplyWrappingStyle(text)) {
            inlineStyle = m_wrappingStyle->copy();
            // FIXME: <rdar://problem/5371536> Style rules that match pasted content can change it's appearance
            // Make sure spans are inline style in paste side e.g. span { display: block }.
            inlineStyle->forceInline();
            // FIXME: Should this be included in forceInline?
            inlineStyle->style()->setProperty(CSSPropertyFloat, CSSValueNone);
        }
        m_accumulator->appendTextWithInlineStyle(text, inlineStyle);
        break;
    }
    case Node::ELEMENT_NODE: {
        Element& element = toElement(node);
        if ((element.isHTMLElement() && shouldAnnotate()) || shouldApplyWrappingStyle(element)) {
            RefPtrWillBeRawPtr<EditingStyle> inlineStyle = createInlineStyle(element);
            m_accumulator->appendElementWithInlineStyle(element, inlineStyle);
            break;
        }
        m_accumulator->appendElement(element);
        break;
    }
    default:
        m_accumulator->appendStartMarkup(node);
        break;
    }
}

template<typename Strategy>
void StyledMarkupTraverser<Strategy>::appendEndMarkup(Node& node)
{
    if (!m_accumulator || !node.isElementNode())
        return;
    m_accumulator->appendEndTag(toElement(node));
}

template<typename Strategy>
bool StyledMarkupTraverser<Strategy>::shouldApplyWrappingStyle(const Node& node) const
{
    return m_lastClosed && Strategy::parent(*m_lastClosed) == Strategy::parent(node)
        && m_wrappingStyle && m_wrappingStyle->style();
}

template<typename Strategy>
RefPtrWillBeRawPtr<EditingStyle> StyledMarkupTraverser<Strategy>::createInlineStyle(Element& element)
{
    RefPtrWillBeRawPtr<EditingStyle> inlineStyle = nullptr;

    if (shouldApplyWrappingStyle(element)) {
        inlineStyle = m_wrappingStyle->copy();
        inlineStyle->removePropertiesInElementDefaultStyle(&element);
        inlineStyle->removeStyleConflictingWithStyleOfElement(&element);
    } else {
        inlineStyle = EditingStyle::create();
    }

    if (element.isStyledElement() && element.inlineStyle())
        inlineStyle->overrideWithStyle(element.inlineStyle());

    if (element.isHTMLElement() && shouldAnnotate())
        inlineStyle->mergeStyleFromRulesForSerialization(&toHTMLElement(element));

    return inlineStyle;
}

template class StyledMarkupSerializer<EditingStrategy>;
template class StyledMarkupSerializer<EditingInComposedTreeStrategy>;

} // namespace blink
