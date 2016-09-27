/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2005 Alexey Proskuryakov.
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
#include "core/editing/iterators/TextIterator.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/FirstLetterPseudoElement.h"
#include "core/dom/Position.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/EphemeralRange.h"
#include "core/editing/VisiblePosition.h"
#include "core/editing/VisibleUnits.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/CharacterIterator.h"
#include "core/editing/iterators/WordAwareIterator.h"
#include "core/frame/FrameView.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLTextFormControlElement.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutTableRow.h"
#include "core/layout/LayoutTextControl.h"
#include "core/layout/LayoutTextFragment.h"
#include "core/layout/line/InlineTextBox.h"
#include "platform/fonts/Font.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuilder.h"
//#include <unicode/utf16.h>

using namespace WTF::Unicode;

namespace blink {

using namespace HTMLNames;

namespace {

template <typename Strategy>
TextIteratorBehaviorFlags adjustBehaviorFlags(TextIteratorBehaviorFlags);

template <>
TextIteratorBehaviorFlags adjustBehaviorFlags<EditingStrategy>(TextIteratorBehaviorFlags flags)
{
    return flags;
}

template <>
TextIteratorBehaviorFlags adjustBehaviorFlags<EditingInComposedTreeStrategy>(TextIteratorBehaviorFlags flags)
{
    return flags & ~(TextIteratorEntersOpenShadowRoots | TextIteratorEntersTextControls);
}

// This function is like Range::pastLastNode, except for the fact that it can
// climb up out of shadow trees.
template <typename Strategy>
Node* pastLastNode(const Node& rangeEndContainer, int rangeEndOffset)
{
    if (rangeEndOffset >= 0 && !rangeEndContainer.offsetInCharacters()) {
        if (Node* next = Strategy::childAt(rangeEndContainer, rangeEndOffset))
            return next;
    }
    for (const Node* node = &rangeEndContainer; node; node = parentCrossingShadowBoundaries<Strategy>(*node)) {
        if (Node* next = Strategy::nextSibling(*node))
            return next;
    }
    return nullptr;
}

// Figure out the initial value of m_shadowDepth: the depth of startContainer's
// tree scope from the common ancestor tree scope.
template <typename Strategy>
int shadowDepthOf(const Node& startContainer, const Node& endContainer);

template <>
int shadowDepthOf<EditingStrategy>(const Node& startContainer, const Node& endContainer)
{
    const TreeScope* commonAncestorTreeScope = startContainer.treeScope().commonAncestorTreeScope(endContainer.treeScope());
    ASSERT(commonAncestorTreeScope);
    int shadowDepth = 0;
    for (const TreeScope* treeScope = &startContainer.treeScope(); treeScope != commonAncestorTreeScope; treeScope = treeScope->parentTreeScope())
        ++shadowDepth;
    return shadowDepth;
}

template <>
int shadowDepthOf<EditingInComposedTreeStrategy>(const Node& startContainer, const Node& endContainer)
{
    return 0;
}

} // namespace

template<typename Strategy>
TextIteratorAlgorithm<Strategy>::TextIteratorAlgorithm(const typename Strategy::PositionType& start, const typename Strategy::PositionType& end, TextIteratorBehaviorFlags behavior)
    : m_offset(0)
    , m_startContainer(nullptr)
    , m_startOffset(0)
    , m_endContainer(nullptr)
    , m_endOffset(0)
    , m_needsAnotherNewline(false)
    , m_textBox(nullptr)
    , m_remainingTextBox(nullptr)
    , m_firstLetterText(nullptr)
    , m_lastTextNode(nullptr)
    , m_lastTextNodeEndedWithCollapsedSpace(false)
    , m_sortedTextBoxesPosition(0)
    , m_behavior(adjustBehaviorFlags<Strategy>(behavior))
    , m_handledFirstLetter(false)
    , m_shouldStop(false)
    , m_handleShadowRoot(false)
    // The call to emitsOriginalText() must occur after m_behavior is initialized.
    , m_textState(emitsOriginalText())
{
    ASSERT(start.isNotNull());
    ASSERT(end.isNotNull());
    if (start.compareTo(end) > 0) {
        initialize(end.containerNode(), end.computeOffsetInContainerNode(), start.containerNode(), start.computeOffsetInContainerNode());
        return;
    }
    initialize(start.containerNode(), start.computeOffsetInContainerNode(), end.containerNode(), end.computeOffsetInContainerNode());
}

template<typename Strategy>
void TextIteratorAlgorithm<Strategy>::initialize(Node* startContainer, int startOffset, Node* endContainer, int endOffset)
{
    ASSERT(startContainer);
    ASSERT(endContainer);

    // Remember the range - this does not change.
    m_startContainer = startContainer;
    m_startOffset = startOffset;
    m_endContainer = endContainer;
    m_endOffset = endOffset;

    m_shadowDepth = shadowDepthOf<Strategy>(*startContainer, *endContainer);

    // Set up the current node for processing.
    if (startContainer->offsetInCharacters())
        m_node = startContainer;
    else if (Node* child = Strategy::childAt(*startContainer, startOffset))
        m_node = child;
    else if (!startOffset)
        m_node = startContainer;
    else
        m_node = Strategy::nextSkippingChildren(*startContainer);

    if (!m_node)
        return;

    m_node->document().updateLayoutIgnorePendingStylesheets();

    m_fullyClippedStack.setUpFullyClippedStack(m_node);
    m_offset = m_node == m_startContainer ? m_startOffset : 0;
    m_iterationProgress = HandledNone;

    // Calculate first out of bounds node.
    m_pastEndNode = endContainer? pastLastNode<Strategy>(*endContainer, endOffset) : nullptr;

    // Identify the first run.
    advance();
}

template<typename Strategy>
TextIteratorAlgorithm<Strategy>::~TextIteratorAlgorithm()
{
    if (!m_handleShadowRoot)
        return;
    Document* document = ownerDocument();
    if (!document)
        return;
    if (m_behavior & TextIteratorForInnerText)
        UseCounter::count(document, UseCounter::InnerTextWithShadowTree);
    if (m_behavior & TextIteratorForSelectionToString)
        UseCounter::count(document, UseCounter::SelectionToStringWithShadowTree);
    if (m_behavior & TextIteratorForWindowFind)
        UseCounter::count(document, UseCounter::WindowFindWithShadowTree);
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::isInsideReplacedElement() const
{
    if (atEnd() || length() != 1 || !m_node)
        return false;

    LayoutObject* layoutObject = m_node->layoutObject();
    return layoutObject && layoutObject->isReplaced();
}

template<typename Strategy>
void TextIteratorAlgorithm<Strategy>::advance()
{
    if (m_shouldStop)
        return;

    ASSERT(!m_node || !m_node->document().needsLayoutTreeUpdate());

    m_textState.resetRunInformation();

    // handle remembered node that needed a newline after the text node's newline
    if (m_needsAnotherNewline) {
        // Emit the extra newline, and position it *inside* m_node, after m_node's
        // contents, in case it's a block, in the same way that we position the first
        // newline. The range for the emitted newline should start where the line
        // break begins.
        // FIXME: It would be cleaner if we emitted two newlines during the last
        // iteration, instead of using m_needsAnotherNewline.
        Node* lastChild = Strategy::lastChild(*m_node);
        Node* baseNode = lastChild ? lastChild : m_node.get();
        emitCharacter('\n', Strategy::parent(*baseNode), baseNode, 1, 1);
        m_needsAnotherNewline = false;
        return;
    }

    if (!m_textBox && m_remainingTextBox) {
        m_textBox = m_remainingTextBox;
        m_remainingTextBox = 0;
        m_firstLetterText = nullptr;
        m_offset = 0;
    }
    // handle remembered text box
    if (m_textBox) {
        handleTextBox();
        if (m_textState.positionNode())
            return;
    }

    while (m_node && (m_node != m_pastEndNode || m_shadowDepth > 0)) {
        if (!m_shouldStop && stopsOnFormControls() && HTMLFormControlElement::enclosingFormControlElement(m_node))
            m_shouldStop = true;

        // if the range ends at offset 0 of an element, represent the
        // position, but not the content, of that element e.g. if the
        // node is a blockflow element, emit a newline that
        // precedes the element
        if (m_node == m_endContainer && !m_endOffset) {
            representNodeOffsetZero();
            m_node = nullptr;
            return;
        }

        LayoutObject* layoutObject = m_node->layoutObject();
        if (!layoutObject) {
            if (m_node->isShadowRoot()) {
                // A shadow root doesn't have a layoutObject, but we want to visit children anyway.
                m_iterationProgress = m_iterationProgress < HandledNode ? HandledNode : m_iterationProgress;
                m_handleShadowRoot = true;
            } else {
                m_iterationProgress = HandledChildren;
            }
        } else {
            // Enter author shadow roots, from youngest, if any and if necessary.
            if (m_iterationProgress < HandledOpenShadowRoots) {
                if (entersOpenShadowRoots() && m_node->isElementNode() && toElement(m_node)->hasOpenShadowRoot()) {
                    ShadowRoot* youngestShadowRoot = toElement(m_node)->shadowRoot();
                    ASSERT(youngestShadowRoot->type() == ShadowRootType::Open);
                    m_node = youngestShadowRoot;
                    m_iterationProgress = HandledNone;
                    ++m_shadowDepth;
                    m_fullyClippedStack.pushFullyClippedState(m_node);
                    continue;
                }

                m_iterationProgress = HandledOpenShadowRoots;
            }

            // Enter user-agent shadow root, if necessary.
            if (m_iterationProgress < HandledUserAgentShadowRoot) {
                if (entersTextControls() && layoutObject->isTextControl()) {
                    ShadowRoot* userAgentShadowRoot = toElement(m_node)->userAgentShadowRoot();
                    ASSERT(userAgentShadowRoot->type() == ShadowRootType::UserAgent);
                    m_node = userAgentShadowRoot;
                    m_iterationProgress = HandledNone;
                    ++m_shadowDepth;
                    m_fullyClippedStack.pushFullyClippedState(m_node);
                    continue;
                }
                m_iterationProgress = HandledUserAgentShadowRoot;
            }

            // Handle the current node according to its type.
            if (m_iterationProgress < HandledNode) {
                bool handledNode = false;
                if (layoutObject->isText() && m_node->nodeType() == Node::TEXT_NODE) { // FIXME: What about CDATA_SECTION_NODE?
                    if (!m_fullyClippedStack.top() || ignoresStyleVisibility())
                        handledNode = handleTextNode();
                } else if (layoutObject && (layoutObject->isImage() || layoutObject->isLayoutPart()
                    || (m_node && m_node->isHTMLElement()
                    && (isHTMLFormControlElement(toHTMLElement(*m_node))
                    || isHTMLLegendElement(toHTMLElement(*m_node))
                    || isHTMLImageElement(toHTMLElement(*m_node))
                    || isHTMLMeterElement(toHTMLElement(*m_node))
                    || isHTMLProgressElement(toHTMLElement(*m_node)))))) {
                    handledNode = handleReplacedElement();
                } else {
                    handledNode = handleNonTextNode();
                }
                if (handledNode)
                    m_iterationProgress = HandledNode;
                if (m_textState.positionNode())
                    return;
            }
        }

        // Find a new current node to handle in depth-first manner,
        // calling exitNode() as we come back thru a parent node.
        //
        // 1. Iterate over child nodes, if we haven't done yet.
        // To support |TextIteratorEmitsImageAltText|, we don't traversal child
        // nodes, in composed tree.
        Node* next = m_iterationProgress < HandledChildren && !isHTMLImageElement(*m_node) ? Strategy::firstChild(*m_node) : nullptr;
        m_offset = 0;
        if (!next) {
            // 2. If we've already iterated children or they are not available, go to the next sibling node.
            next = Strategy::nextSibling(*m_node);
            if (!next) {
                // 3. If we are at the last child, go up the node tree until we find a next sibling.
                bool pastEnd = Strategy::next(*m_node) == m_pastEndNode;
                ContainerNode* parentNode = Strategy::parent(*m_node);
                while (!next && parentNode) {
                    if ((pastEnd && parentNode == m_endContainer) || Strategy::isDescendantOf(*m_endContainer, *parentNode))
                        return;
                    bool haveLayoutObject = m_node->layoutObject();
                    m_node = parentNode;
                    m_fullyClippedStack.pop();
                    parentNode = Strategy::parent(*m_node);
                    if (haveLayoutObject)
                        exitNode();
                    if (m_textState.positionNode()) {
                        m_iterationProgress = HandledChildren;
                        return;
                    }
                    next = Strategy::nextSibling(*m_node);
                }

                if (!next && !parentNode && m_shadowDepth > 0) {
                    // 4. Reached the top of a shadow root. If it's created by author, then try to visit the next
                    // sibling shadow root, if any.
                    ShadowRoot* shadowRoot = toShadowRoot(m_node);
                    if (shadowRoot->type() == ShadowRootType::Open) {
                        ShadowRoot* nextShadowRoot = shadowRoot->olderShadowRoot();
                        if (nextShadowRoot && nextShadowRoot->type() == ShadowRootType::Open) {
                            m_fullyClippedStack.pop();
                            m_node = nextShadowRoot;
                            m_iterationProgress = HandledNone;
                            // m_shadowDepth is unchanged since we exit from a shadow root and enter another.
                            m_fullyClippedStack.pushFullyClippedState(m_node);
                        } else {
                            // We are the last shadow root; exit from here and go back to where we were.
                            m_node = shadowRoot->host();
                            m_iterationProgress = HandledOpenShadowRoots;
                            --m_shadowDepth;
                            m_fullyClippedStack.pop();
                        }
                    } else {
                        // If we are in a user-agent shadow root, then go back to the host.
                        ASSERT(shadowRoot->type() == ShadowRootType::UserAgent);
                        m_node = shadowRoot->host();
                        m_iterationProgress = HandledUserAgentShadowRoot;
                        --m_shadowDepth;
                        m_fullyClippedStack.pop();
                    }
                    m_handledFirstLetter = false;
                    m_firstLetterText = nullptr;
                    continue;
                }
            }
            m_fullyClippedStack.pop();
        }

        // set the new current node
        m_node = next;
        if (m_node)
            m_fullyClippedStack.pushFullyClippedState(m_node);
        m_iterationProgress = HandledNone;
        m_handledFirstLetter = false;
        m_firstLetterText = nullptr;

        // how would this ever be?
        if (m_textState.positionNode())
            return;
    }
}

static bool hasVisibleTextNode(LayoutText* layoutObject)
{
    if (layoutObject->style()->visibility() == VISIBLE)
        return true;

    if (!layoutObject->isTextFragment())
        return false;

    LayoutTextFragment* fragment = toLayoutTextFragment(layoutObject);
    if (!fragment->isRemainingTextLayoutObject())
        return false;

    ASSERT(fragment->firstLetterPseudoElement());
    LayoutObject* pseudoElementLayoutObject = fragment->firstLetterPseudoElement()->layoutObject();
    return pseudoElementLayoutObject && pseudoElementLayoutObject->style()->visibility() == VISIBLE;
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::handleTextNode()
{
    Text* textNode = toText(m_node);
    LayoutText* layoutObject = textNode->layoutObject();

    m_lastTextNode = textNode;
    String str = layoutObject->text();

    // handle pre-formatted text
    if (!layoutObject->style()->collapseWhiteSpace()) {
        int runStart = m_offset;
        if (m_lastTextNodeEndedWithCollapsedSpace && hasVisibleTextNode(layoutObject)) {
            emitCharacter(spaceCharacter, textNode, 0, runStart, runStart);
            return false;
        }
        if (!m_handledFirstLetter && layoutObject->isTextFragment() && !m_offset) {
            handleTextNodeFirstLetter(toLayoutTextFragment(layoutObject));
            if (m_firstLetterText) {
                String firstLetter = m_firstLetterText->text();
                emitText(textNode, m_firstLetterText, m_offset, m_offset + firstLetter.length());
                m_firstLetterText = nullptr;
                m_textBox = 0;
                return false;
            }
        }
        if (layoutObject->style()->visibility() != VISIBLE && !ignoresStyleVisibility())
            return false;
        int strLength = str.length();
        int end = (textNode == m_endContainer) ? m_endOffset : INT_MAX;
        int runEnd = std::min(strLength, end);

        if (runStart >= runEnd)
            return true;

        emitText(textNode, textNode->layoutObject(), runStart, runEnd);
        return true;
    }

    if (layoutObject->firstTextBox())
        m_textBox = layoutObject->firstTextBox();

    bool shouldHandleFirstLetter = !m_handledFirstLetter && layoutObject->isTextFragment() && !m_offset;
    if (shouldHandleFirstLetter)
        handleTextNodeFirstLetter(toLayoutTextFragment(layoutObject));

    if (!layoutObject->firstTextBox() && str.length() > 0 && !shouldHandleFirstLetter) {
        if (layoutObject->style()->visibility() != VISIBLE && !ignoresStyleVisibility())
            return false;
        m_lastTextNodeEndedWithCollapsedSpace = true; // entire block is collapsed space
        return true;
    }

    if (m_firstLetterText)
        layoutObject = m_firstLetterText;

    // Used when text boxes are out of order (Hebrew/Arabic w/ embeded LTR text)
    if (layoutObject->containsReversedText()) {
        m_sortedTextBoxes.clear();
        for (InlineTextBox* textBox = layoutObject->firstTextBox(); textBox; textBox = textBox->nextTextBox()) {
            m_sortedTextBoxes.append(textBox);
        }
        std::sort(m_sortedTextBoxes.begin(), m_sortedTextBoxes.end(), InlineTextBox::compareByStart);
        m_sortedTextBoxesPosition = 0;
        m_textBox = m_sortedTextBoxes.isEmpty() ? 0 : m_sortedTextBoxes[0];
    }

    handleTextBox();
    return true;
}

template<typename Strategy>
void TextIteratorAlgorithm<Strategy>::handleTextBox()
{
    LayoutText* layoutObject = m_firstLetterText ? m_firstLetterText : toLayoutText(m_node->layoutObject());

    if (layoutObject->style()->visibility() != VISIBLE && !ignoresStyleVisibility()) {
        m_textBox = nullptr;
    } else {
        String str = layoutObject->text();
        unsigned start = m_offset;
        unsigned end = (m_node == m_endContainer) ? static_cast<unsigned>(m_endOffset) : INT_MAX;
        while (m_textBox) {
            unsigned textBoxStart = m_textBox->start();
            unsigned runStart = std::max(textBoxStart, start);

            // Check for collapsed space at the start of this run.
            InlineTextBox* firstTextBox = layoutObject->containsReversedText() ? (m_sortedTextBoxes.isEmpty() ? 0 : m_sortedTextBoxes[0]) : layoutObject->firstTextBox();
            bool needSpace = m_lastTextNodeEndedWithCollapsedSpace
                || (m_textBox == firstTextBox && textBoxStart == runStart && runStart > 0);
            if (needSpace && !layoutObject->style()->isCollapsibleWhiteSpace(m_textState.lastCharacter()) && m_textState.lastCharacter()) {
                if (m_lastTextNode == m_node && runStart > 0 && str[runStart - 1] == ' ') {
                    unsigned spaceRunStart = runStart - 1;
                    while (spaceRunStart > 0 && str[spaceRunStart - 1] == ' ')
                        --spaceRunStart;
                    emitText(m_node, layoutObject, spaceRunStart, spaceRunStart + 1);
                } else {
                    emitCharacter(spaceCharacter, m_node, 0, runStart, runStart);
                }
                return;
            }
            unsigned textBoxEnd = textBoxStart + m_textBox->len();
            unsigned runEnd = std::min(textBoxEnd, end);

            // Determine what the next text box will be, but don't advance yet
            InlineTextBox* nextTextBox = nullptr;
            if (layoutObject->containsReversedText()) {
                if (m_sortedTextBoxesPosition + 1 < m_sortedTextBoxes.size())
                    nextTextBox = m_sortedTextBoxes[m_sortedTextBoxesPosition + 1];
            } else {
                nextTextBox = m_textBox->nextTextBox();
            }

            // FIXME: Based on the outcome of crbug.com/446502 it's possible we can
            //   remove this block. The reason we new it now is because BIDI and
            //   FirstLetter seem to have different ideas of where things can split.
            //   FirstLetter takes the punctuation + first letter, and BIDI will
            //   split out the punctuation and possibly reorder it.
            if (nextTextBox && nextTextBox->layoutObject() != layoutObject) {
                m_textBox = 0;
                return;
            }
            ASSERT(!nextTextBox || nextTextBox->layoutObject() == layoutObject);

            if (runStart < runEnd) {
                // Handle either a single newline character (which becomes a space),
                // or a run of characters that does not include a newline.
                // This effectively translates newlines to spaces without copying the text.
                if (str[runStart] == '\n') {
                    emitCharacter(spaceCharacter, m_node, 0, runStart, runStart + 1);
                    m_offset = runStart + 1;
                } else {
                    size_t subrunEnd = str.find('\n', runStart);
                    if (subrunEnd == kNotFound || subrunEnd > runEnd)
                        subrunEnd = runEnd;

                    m_offset = subrunEnd;
                    emitText(m_node, layoutObject, runStart, subrunEnd);
                }

                // If we are doing a subrun that doesn't go to the end of the text box,
                // come back again to finish handling this text box; don't advance to the next one.
                if (static_cast<unsigned>(m_textState.positionEndOffset()) < textBoxEnd)
                    return;

                // Advance and return
                unsigned nextRunStart = nextTextBox ? nextTextBox->start() : str.length();
                if (nextRunStart > runEnd)
                    m_lastTextNodeEndedWithCollapsedSpace = true; // collapsed space between runs or at the end

                m_textBox = nextTextBox;
                if (layoutObject->containsReversedText())
                    ++m_sortedTextBoxesPosition;
                return;
            }
            // Advance and continue
            m_textBox = nextTextBox;
            if (layoutObject->containsReversedText())
                ++m_sortedTextBoxesPosition;
        }
    }

    if (!m_textBox && m_remainingTextBox) {
        m_textBox = m_remainingTextBox;
        m_remainingTextBox = 0;
        m_firstLetterText = nullptr;
        m_offset = 0;
        handleTextBox();
    }
}

template<typename Strategy>
void TextIteratorAlgorithm<Strategy>::handleTextNodeFirstLetter(LayoutTextFragment* layoutObject)
{
    m_handledFirstLetter = true;

    if (!layoutObject->isRemainingTextLayoutObject())
        return;

    FirstLetterPseudoElement* firstLetterElement = layoutObject->firstLetterPseudoElement();
    if (!firstLetterElement)
        return;

    LayoutObject* pseudoLayoutObject = firstLetterElement->layoutObject();
    if (pseudoLayoutObject->style()->visibility() != VISIBLE && !ignoresStyleVisibility())
        return;

    LayoutObject* firstLetter = pseudoLayoutObject->slowFirstChild();
    ASSERT(firstLetter);

    m_remainingTextBox = m_textBox;
    m_textBox = toLayoutText(firstLetter)->firstTextBox();
    m_sortedTextBoxes.clear();
    m_firstLetterText = toLayoutText(firstLetter);
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::supportsAltText(Node* node)
{
    if (!node->isHTMLElement())
        return false;
    HTMLElement& element = toHTMLElement(*node);

    // FIXME: Add isSVGImageElement.
    if (isHTMLImageElement(element))
        return true;
    if (isHTMLInputElement(toHTMLElement(*node)) && toHTMLInputElement(*node).isImage())
        return true;
    return false;
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::handleReplacedElement()
{
    if (m_fullyClippedStack.top())
        return false;

    LayoutObject* layoutObject = m_node->layoutObject();
    if (layoutObject->style()->visibility() != VISIBLE && !ignoresStyleVisibility())
        return false;

    if (emitsObjectReplacementCharacter()) {
        emitCharacter(objectReplacementCharacter, Strategy::parent(*m_node), m_node, 0, 1);
        return true;
    }

    if (m_lastTextNodeEndedWithCollapsedSpace) {
        emitCharacter(spaceCharacter, Strategy::parent(*m_lastTextNode), m_lastTextNode, 1, 1);
        return false;
    }

    if (entersTextControls() && layoutObject->isTextControl()) {
        // The shadow tree should be already visited.
        return true;
    }

    if (emitsCharactersBetweenAllVisiblePositions()) {
        // We want replaced elements to behave like punctuation for boundary
        // finding, and to simply take up space for the selection preservation
        // code in moveParagraphs, so we use a comma.
        emitCharacter(',', Strategy::parent(*m_node), m_node, 0, 1);
        return true;
    }

    m_textState.updateForReplacedElement(m_node);

    if (emitsImageAltText() && TextIterator::supportsAltText(m_node)) {
        m_textState.emitAltText(m_node);
        if (m_textState.length())
            return true;
    }

    return true;
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::shouldEmitTabBeforeNode(Node* node)
{
    LayoutObject* r = node->layoutObject();

    // Table cells are delimited by tabs.
    if (!r || !isTableCell(node))
        return false;

    // Want a tab before every cell other than the first one
    LayoutTableCell* rc = toLayoutTableCell(r);
    LayoutTable* t = rc->table();
    return t && (t->cellBefore(rc) || t->cellAbove(rc));
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::shouldEmitNewlineForNode(Node* node, bool emitsOriginalText)
{
    LayoutObject* layoutObject = node->layoutObject();

    if (layoutObject ? !layoutObject->isBR() : !isHTMLBRElement(node))
        return false;
    return emitsOriginalText || !(node->isInShadowTree() && isHTMLInputElement(*node->shadowHost()));
}

static bool shouldEmitNewlinesBeforeAndAfterNode(Node& node)
{
    // Block flow (versus inline flow) is represented by having
    // a newline both before and after the element.
    LayoutObject* r = node.layoutObject();
    if (!r) {
        return (node.hasTagName(blockquoteTag)
            || node.hasTagName(ddTag)
            || node.hasTagName(divTag)
            || node.hasTagName(dlTag)
            || node.hasTagName(dtTag)
            || node.hasTagName(h1Tag)
            || node.hasTagName(h2Tag)
            || node.hasTagName(h3Tag)
            || node.hasTagName(h4Tag)
            || node.hasTagName(h5Tag)
            || node.hasTagName(h6Tag)
            || node.hasTagName(hrTag)
            || node.hasTagName(liTag)
            || node.hasTagName(listingTag)
            || node.hasTagName(olTag)
            || node.hasTagName(pTag)
            || node.hasTagName(preTag)
            || node.hasTagName(trTag)
            || node.hasTagName(ulTag));
    }

    // Need to make an exception for option and optgroup, because we want to
    // keep the legacy behavior before we added layoutObjects to them.
    if (isHTMLOptionElement(node) || isHTMLOptGroupElement(node))
        return false;

    // Need to make an exception for table cells, because they are blocks, but we
    // want them tab-delimited rather than having newlines before and after.
    if (isTableCell(&node))
        return false;

    // Need to make an exception for table row elements, because they are neither
    // "inline" or "LayoutBlock", but we want newlines for them.
    if (r->isTableRow()) {
        LayoutTable* t = toLayoutTableRow(r)->table();
        if (t && !t->isInline())
            return true;
    }

    return !r->isInline() && r->isLayoutBlock()
        && !r->isFloatingOrOutOfFlowPositioned() && !r->isBody() && !r->isRubyText();
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::shouldEmitNewlineAfterNode(Node& node)
{
    // FIXME: It should be better but slower to create a VisiblePosition here.
    if (!shouldEmitNewlinesBeforeAndAfterNode(node))
        return false;
    // Check if this is the very last layoutObject in the document.
    // If so, then we should not emit a newline.
    Node* next = &node;
    do {
        next = Strategy::nextSkippingChildren(*next);
        if (next && next->layoutObject())
            return true;
    } while (next);
    return false;
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::shouldEmitNewlineBeforeNode(Node& node)
{
    return shouldEmitNewlinesBeforeAndAfterNode(node);
}

static bool shouldEmitExtraNewlineForNode(Node* node)
{
    // When there is a significant collapsed bottom margin, emit an extra
    // newline for a more realistic result. We end up getting the right
    // result even without margin collapsing. For example: <div><p>text</p></div>
    // will work right even if both the <div> and the <p> have bottom margins.
    LayoutObject* r = node->layoutObject();
    if (!r || !r->isBox())
        return false;

    // NOTE: We only do this for a select set of nodes, and fwiw WinIE appears
    // not to do this at all
    if (node->hasTagName(h1Tag)
        || node->hasTagName(h2Tag)
        || node->hasTagName(h3Tag)
        || node->hasTagName(h4Tag)
        || node->hasTagName(h5Tag)
        || node->hasTagName(h6Tag)
        || node->hasTagName(pTag)) {
        const ComputedStyle* style = r->style();
        if (style) {
            int bottomMargin = toLayoutBox(r)->collapsedMarginAfter();
            int fontSize = style->fontDescription().computedPixelSize();
            if (bottomMargin * 2 >= fontSize)
                return true;
        }
    }

    return false;
}

// Whether or not we should emit a character as we enter m_node (if it's a container) or as we hit it (if it's atomic).
template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::shouldRepresentNodeOffsetZero()
{
    if (emitsCharactersBetweenAllVisiblePositions() && isRenderedTableElement(m_node))
        return true;

    // Leave element positioned flush with start of a paragraph
    // (e.g. do not insert tab before a table cell at the start of a paragraph)
    if (m_textState.lastCharacter() == '\n')
        return false;

    // Otherwise, show the position if we have emitted any characters
    if (m_textState.hasEmitted())
        return true;

    // We've not emitted anything yet. Generally, there is no need for any positioning then.
    // The only exception is when the element is visually not in the same line as
    // the start of the range (e.g. the range starts at the end of the previous paragraph).
    // NOTE: Creating VisiblePositions and comparing them is relatively expensive, so we
    // make quicker checks to possibly avoid that. Another check that we could make is
    // is whether the inline vs block flow changed since the previous visible element.
    // I think we're already in a special enough case that that won't be needed, tho.

    // No character needed if this is the first node in the range.
    if (m_node == m_startContainer)
        return false;

    // If we are outside the start container's subtree, assume we need to emit.
    // FIXME: m_startContainer could be an inline block
    if (!Strategy::isDescendantOf(*m_node, *m_startContainer))
        return true;

    // If we started as m_startContainer offset 0 and the current node is a descendant of
    // the start container, we already had enough context to correctly decide whether to
    // emit after a preceding block. We chose not to emit (m_hasEmitted is false),
    // so don't second guess that now.
    // NOTE: Is this really correct when m_node is not a leftmost descendant? Probably
    // immaterial since we likely would have already emitted something by now.
    if (!m_startOffset)
        return false;

    // If this node is unrendered or invisible the VisiblePosition checks below won't have much meaning.
    // Additionally, if the range we are iterating over contains huge sections of unrendered content,
    // we would create VisiblePositions on every call to this function without this check.
    if (!m_node->layoutObject() || m_node->layoutObject()->style()->visibility() != VISIBLE
        || (m_node->layoutObject()->isLayoutBlockFlow() && !toLayoutBlock(m_node->layoutObject())->size().height() && !isHTMLBodyElement(*m_node)))
        return false;

    // The startPos.isNotNull() check is needed because the start could be before the body,
    // and in that case we'll get null. We don't want to put in newlines at the start in that case.
    // The currPos.isNotNull() check is needed because positions in non-HTML content
    // (like SVG) do not have visible positions, and we don't want to emit for them either.
    VisiblePosition startPos = VisiblePosition(Position(m_startContainer, m_startOffset), DOWNSTREAM);
    VisiblePosition currPos = VisiblePosition(positionBeforeNode(m_node), DOWNSTREAM);
    return startPos.isNotNull() && currPos.isNotNull() && !inSameLine(startPos, currPos);
}

template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::shouldEmitSpaceBeforeAndAfterNode(Node* node)
{
    return isRenderedTableElement(node) && (node->layoutObject()->isInline() || emitsCharactersBetweenAllVisiblePositions());
}

template<typename Strategy>
void TextIteratorAlgorithm<Strategy>::representNodeOffsetZero()
{
    // Emit a character to show the positioning of m_node.

    // When we haven't been emitting any characters, shouldRepresentNodeOffsetZero() can
    // create VisiblePositions, which is expensive. So, we perform the inexpensive checks
    // on m_node to see if it necessitates emitting a character first and will early return
    // before encountering shouldRepresentNodeOffsetZero()s worse case behavior.
    if (shouldEmitTabBeforeNode(m_node)) {
        if (shouldRepresentNodeOffsetZero())
            emitCharacter('\t', Strategy::parent(*m_node), m_node, 0, 0);
    } else if (shouldEmitNewlineBeforeNode(*m_node)) {
        if (shouldRepresentNodeOffsetZero())
            emitCharacter('\n', Strategy::parent(*m_node), m_node, 0, 0);
    } else if (shouldEmitSpaceBeforeAndAfterNode(m_node)) {
        if (shouldRepresentNodeOffsetZero())
            emitCharacter(spaceCharacter, Strategy::parent(*m_node), m_node, 0, 0);
    }
}


template<typename Strategy>
bool TextIteratorAlgorithm<Strategy>::handleNonTextNode()
{
    if (shouldEmitNewlineForNode(m_node, emitsOriginalText()))
        emitCharacter('\n', Strategy::parent(*m_node), m_node, 0, 1);
    else if (emitsCharactersBetweenAllVisiblePositions() && m_node->layoutObject() && m_node->layoutObject()->isHR())
        emitCharacter(spaceCharacter, Strategy::parent(*m_node), m_node, 0, 1);
    else
        representNodeOffsetZero();

    return true;
}

template<typename Strategy>
void TextIteratorAlgorithm<Strategy>::exitNode()
{
    // prevent emitting a newline when exiting a collapsed block at beginning of the range
    // FIXME: !m_hasEmitted does not necessarily mean there was a collapsed block... it could
    // have been an hr (e.g.). Also, a collapsed block could have height (e.g. a table) and
    // therefore look like a blank line.
    if (!m_textState.hasEmitted())
        return;

    // Emit with a position *inside* m_node, after m_node's contents, in
    // case it is a block, because the run should start where the
    // emitted character is positioned visually.
    Node* lastChild = Strategy::lastChild(*m_node);
    Node* baseNode = lastChild ? lastChild : m_node.get();
    // FIXME: This shouldn't require the m_lastTextNode to be true, but we can't change that without making
    // the logic in _web_attributedStringFromRange match. We'll get that for free when we switch to use
    // TextIterator in _web_attributedStringFromRange.
    // See <rdar://problem/5428427> for an example of how this mismatch will cause problems.
    if (m_lastTextNode && shouldEmitNewlineAfterNode(*m_node)) {
        // use extra newline to represent margin bottom, as needed
        bool addNewline = shouldEmitExtraNewlineForNode(m_node);

        // FIXME: We need to emit a '\n' as we leave an empty block(s) that
        // contain a VisiblePosition when doing selection preservation.
        if (m_textState.lastCharacter() != '\n') {
            // insert a newline with a position following this block's contents.
            emitCharacter(newlineCharacter, Strategy::parent(*baseNode), baseNode, 1, 1);
            // remember whether to later add a newline for the current node
            ASSERT(!m_needsAnotherNewline);
            m_needsAnotherNewline = addNewline;
        } else if (addNewline) {
            // insert a newline with a position following this block's contents.
            emitCharacter(newlineCharacter, Strategy::parent(*baseNode), baseNode, 1, 1);
        }
    }

    // If nothing was emitted, see if we need to emit a space.
    if (!m_textState.positionNode() && shouldEmitSpaceBeforeAndAfterNode(m_node))
        emitCharacter(spaceCharacter, Strategy::parent(*baseNode), baseNode, 1, 1);
}

template<typename Strategy>
void TextIteratorAlgorithm<Strategy>::emitCharacter(UChar c, Node* textNode, Node* offsetBaseNode, int textStartOffset, int textEndOffset)
{
    // Since m_lastTextNodeEndedWithCollapsedSpace seems better placed in
    // TextIterator, but is always reset when we call emitCharacter, we
    // wrap TextIteratorTextState::emitCharacter() with this function.
    m_textState.emitCharacter(c, textNode, offsetBaseNode, textStartOffset, textEndOffset);
    m_lastTextNodeEndedWithCollapsedSpace = false;
}

template<typename Strategy>
void TextIteratorAlgorithm<Strategy>::emitText(Node* textNode, LayoutText* layoutObject, int textStartOffset, int textEndOffset)
{
    // Since m_lastTextNodeEndedWithCollapsedSpace seems better placed in
    // TextIterator, but is always reset when we call emitCharacter, we
    // wrap TextIteratorTextState::emitCharacter() with this function.
    m_textState.emitText(textNode, layoutObject, textStartOffset, textEndOffset);
    m_lastTextNodeEndedWithCollapsedSpace = false;
}

template<typename Strategy>
EphemeralRange TextIteratorAlgorithm<Strategy>::range() const
{
    // use the current run information, if we have it
    if (m_textState.positionNode())
        return m_textState.range();

    // otherwise, return the end of the overall range we were given
    if (m_endContainer)
        return EphemeralRange(Position(m_endContainer, m_endOffset));

    return EphemeralRange();
}

template<typename Strategy>
Document* TextIteratorAlgorithm<Strategy>::ownerDocument() const
{
    if (m_textState.positionNode())
        return &m_textState.positionNode()->document();
    if (m_endContainer)
        return &m_endContainer->document();
    return 0;
}

template<typename Strategy>
Node* TextIteratorAlgorithm<Strategy>::node() const
{
    if (m_textState.positionNode() || m_endContainer) {
        Node* node = currentContainer();
        if (node->offsetInCharacters())
            return node;
        return Strategy::childAt(*node, startOffsetInCurrentContainer());
    }
    return 0;
}

template<typename Strategy>
int TextIteratorAlgorithm<Strategy>::startOffsetInCurrentContainer() const
{
    if (m_textState.positionNode()) {
        m_textState.flushPositionOffsets();
        return m_textState.positionStartOffset();
    }
    ASSERT(m_endContainer);
    return m_endOffset;
}

template<typename Strategy>
int TextIteratorAlgorithm<Strategy>::endOffsetInCurrentContainer() const
{
    if (m_textState.positionNode()) {
        m_textState.flushPositionOffsets();
        return m_textState.positionEndOffset();
    }
    ASSERT(m_endContainer);
    return m_endOffset;
}

template<typename Strategy>
Node* TextIteratorAlgorithm<Strategy>::currentContainer() const
{
    if (m_textState.positionNode()) {
        return m_textState.positionNode();
    }
    ASSERT(m_endContainer);
    return m_endContainer;
}

template<typename Strategy>
typename Strategy::PositionType TextIteratorAlgorithm<Strategy>::startPositionInCurrentContainer() const
{
    return Strategy::PositionType::createLegacyEditingPosition(currentContainer(), startOffsetInCurrentContainer());
}

template<typename Strategy>
typename Strategy::PositionType TextIteratorAlgorithm<Strategy>::endPositionInCurrentContainer() const
{
    return Strategy::PositionType::createLegacyEditingPosition(currentContainer(), endOffsetInCurrentContainer());
}

template<typename Strategy>
int TextIteratorAlgorithm<Strategy>::rangeLength(const typename Strategy::PositionType& start, const typename Strategy::PositionType& end, bool forSelectionPreservation)
{
    int length = 0;
    TextIteratorBehaviorFlags behaviorFlags = TextIteratorEmitsObjectReplacementCharacter;
    if (forSelectionPreservation)
        behaviorFlags |= TextIteratorEmitsCharactersBetweenAllVisiblePositions;
    for (TextIteratorAlgorithm<Strategy> it(start, end, behaviorFlags); !it.atEnd(); it.advance())
        length += it.length();

    return length;
}

template<typename Strategy>
EphemeralRange TextIteratorAlgorithm<Strategy>::subrange(const Position& start, const Position& end, int characterOffset, int characterCount)
{
    CharacterIterator entireRangeIterator(start, end, TextIteratorEmitsObjectReplacementCharacter);
    return entireRangeIterator.calculateCharacterSubrange(characterOffset, characterCount);
}

// --------

template <typename Strategy>
static String createPlainText(const typename Strategy::PositionType& start, const typename Strategy::PositionType& end, TextIteratorBehaviorFlags behavior)
{
    TextIteratorAlgorithm<Strategy> it(start, end, behavior);
    // The initial buffer size can be critical for performance: https://bugs.webkit.org/show_bug.cgi?id=81192
    static const unsigned initialCapacity = 1 << 15;

    unsigned bufferLength = 0;
    StringBuilder builder;
    builder.reserveCapacity(initialCapacity);

    for (; !it.atEnd(); it.advance()) {
        it.text().appendTextToStringBuilder(builder);
        bufferLength += it.length();
    }

    if (!bufferLength)
        return emptyString();

    return builder.toString();
}

String plainText(const Position& start, const Position& end, TextIteratorBehaviorFlags behavior)
{
    return createPlainText<EditingStrategy>(start, end, behavior);
}

String plainText(const PositionInComposedTree& start, const PositionInComposedTree& end, TextIteratorBehaviorFlags behavior)
{
    return createPlainText<EditingInComposedTreeStrategy>(start, end, behavior);
}

template class CORE_TEMPLATE_EXPORT TextIteratorAlgorithm<EditingStrategy>;
template class CORE_TEMPLATE_EXPORT TextIteratorAlgorithm<EditingInComposedTreeStrategy>;

} // namespace blink
