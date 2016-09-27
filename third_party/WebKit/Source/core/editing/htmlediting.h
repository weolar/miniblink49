/*
 * Copyright (C) 2004, 2006, 2008 Apple Inc. All rights reserved.
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

#ifndef htmlediting_h
#define htmlediting_h

#include "core/CoreExport.h"
#include "core/dom/Position.h"
#include "core/editing/EditingBoundary.h"
#include "core/editing/PositionWithAffinity.h"
#include "platform/text/TextDirection.h"
#include "wtf/Forward.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

class Document;
class Element;
class HTMLBRElement;
class HTMLElement;
class HTMLLIElement;
class HTMLSpanElement;
class HTMLUListElement;
class Node;
class Range;
class VisiblePosition;
class VisibleSelection;


// This file contains a set of helper functions used by the editing commands

// -------------------------------------------------------------------------
// Node
// -------------------------------------------------------------------------

// Functions returning Node

CORE_EXPORT ContainerNode* highestEditableRoot(const Position&, EditableType = ContentIsEditable);
ContainerNode* highestEditableRoot(const PositionInComposedTree&, EditableType = ContentIsEditable);

Node* highestEnclosingNodeOfType(const Position&, bool (*nodeIsOfType)(const Node*),
    EditingBoundaryCrossingRule = CannotCrossEditingBoundary, Node* stayWithin = nullptr);
Node* highestNodeToRemoveInPruning(Node*, Node* excludeNode = nullptr);
Element* lowestEditableAncestor(Node*);

Element* enclosingBlock(Node*, EditingBoundaryCrossingRule = CannotCrossEditingBoundary);
Element* enclosingBlockFlowElement(Node&); // Deprecated, use enclosingBlock instead.
bool inSameContainingBlockFlowElement(Node*, Node*);
Element* enclosingTableCell(const Position&);
Node* enclosingEmptyListItem(const VisiblePosition&);
Element* enclosingAnchorElement(const Position&);
Element* enclosingElementWithTag(const Position&, const QualifiedName&);
Node* enclosingNodeOfType(const Position&, bool (*nodeIsOfType)(const Node*), EditingBoundaryCrossingRule = CannotCrossEditingBoundary);

HTMLSpanElement* tabSpanElement(const Node*);
Element* isLastPositionBeforeTable(const VisiblePosition&);
Element* isFirstPositionAfterTable(const VisiblePosition&);

// Returns the next leaf node or nullptr if there are no more.
// Delivers leaf nodes as if the whole DOM tree were a linear chain of its leaf nodes.
Node* nextAtomicLeafNode(const Node& start);

// Returns the previous leaf node or nullptr if there are no more.
// Delivers leaf nodes as if the whole DOM tree were a linear chain of its leaf nodes.
Node* previousAtomicLeafNode(const Node& start);

template <typename Strategy>
ContainerNode* parentCrossingShadowBoundaries(const Node&);
template <>
inline ContainerNode* parentCrossingShadowBoundaries<EditingStrategy>(const Node& node)
{
    return NodeTraversal::parentOrShadowHostNode(node);
}
template <>
inline ContainerNode* parentCrossingShadowBoundaries<EditingInComposedTreeStrategy>(const Node& node)
{
    return ComposedTreeTraversal::parent(node);
}

// offset functions on Node

int lastOffsetForEditing(const Node*);
int caretMinOffset(const Node*);
int caretMaxOffset(const Node*);

// boolean functions on Node

// FIXME: editingIgnoresContent, canHaveChildrenForEditing, and isAtomicNode
// should be renamed to reflect its usage.

// Returns true for nodes that either have no content, or have content that is ignored (skipped over) while editing.
// There are no VisiblePositions inside these nodes.
inline bool editingIgnoresContent(const Node* node)
{
    return EditingStrategy::editingIgnoresContent(node);
}

inline bool canHaveChildrenForEditing(const Node* node)
{
    return !node->isTextNode() && node->canContainRangeEndPoint();
}

bool isAtomicNode(const Node*);
bool isBlock(const Node*);
bool isInline(const Node*);
bool isTabHTMLSpanElement(const Node*);
bool isTabHTMLSpanElementTextNode(const Node*);
bool isMailHTMLBlockquoteElement(const Node*);
bool isRenderedTableElement(const Node*);
bool isRenderedHTMLTableElement(const Node*);
bool isTableCell(const Node*);
bool isEmptyTableCell(const Node*);
bool isTableStructureNode(const Node*);
bool isHTMLListElement(Node*);
bool isListItem(const Node*);
bool isNodeRendered(const Node&);
bool isNodeVisiblyContainedWithin(Node&, const Range&);
bool isRenderedAsNonInlineTableImageOrHR(const Node*);
bool areIdenticalElements(const Node*, const Node*);
bool isNonTableCellHTMLBlockElement(const Node*);
bool isBlockFlowElement(const Node&);
TextDirection directionOfEnclosingBlock(const Position&);

// -------------------------------------------------------------------------
// Position
// -------------------------------------------------------------------------

// Functions returning Position

Position nextCandidate(const Position&);
PositionInComposedTree nextCandidate(const PositionInComposedTree&);
Position previousCandidate(const Position&);
PositionInComposedTree previousCandidate(const PositionInComposedTree&);

Position nextVisuallyDistinctCandidate(const Position&);
Position previousVisuallyDistinctCandidate(const Position&);
PositionInComposedTree previousVisuallyDistinctCandidate(const PositionInComposedTree&);

Position positionBeforeContainingSpecialElement(const Position&, HTMLElement** containingSpecialElement = nullptr);
Position positionAfterContainingSpecialElement(const Position&, HTMLElement** containingSpecialElement = nullptr);

inline Position firstPositionInOrBeforeNode(Node* node)
{
    return Position::firstPositionInOrBeforeNode(node);
}

inline Position lastPositionInOrAfterNode(Node* node)
{
    return Position::lastPositionInOrAfterNode(node);
}

Position lastEditablePositionBeforePositionInRoot(const Position&, Node*);
PositionInComposedTree lastEditablePositionBeforePositionInRoot(const PositionInComposedTree&, Node*);

// comparision functions on Position

// |disconnected| is optional output parameter having true if specified
// positions don't have common ancestor.
int comparePositionsInDOMTree(Node* containerA, int offsetA, Node* containerB, int offsetB, bool* disconnected = nullptr);
int comparePositionsInComposedTree(Node* containerA, int offsetA, Node* containerB, int offsetB, bool* disconnected = nullptr);
int comparePositions(const Position&, const Position&);
int comparePositions(const PositionWithAffinity&, const PositionWithAffinity&);

// boolean functions on Position

enum EUpdateStyle { UpdateStyle, DoNotUpdateStyle };
// FIXME: Both isEditablePosition and isRichlyEditablePosition rely on up-to-date
// style to give proper results. They shouldn't update style by default, but
// should make it clear that that is the contract.
// FIXME: isRichlyEditablePosition should also take EUpdateStyle.
bool isEditablePosition(const Position&, EditableType = ContentIsEditable, EUpdateStyle = UpdateStyle);
bool isEditablePosition(const PositionInComposedTree&, EditableType = ContentIsEditable, EUpdateStyle = UpdateStyle);
bool isRichlyEditablePosition(const Position&, EditableType = ContentIsEditable);
bool lineBreakExistsAtPosition(const Position&);
bool isAtUnsplittableElement(const Position&);

// miscellaneous functions on Position

enum WhitespacePositionOption { NotConsiderNonCollapsibleWhitespace, ConsiderNonCollapsibleWhitespace };
Position leadingWhitespacePosition(const Position&, EAffinity, WhitespacePositionOption = NotConsiderNonCollapsibleWhitespace);
Position trailingWhitespacePosition(const Position&, EAffinity, WhitespacePositionOption = NotConsiderNonCollapsibleWhitespace);
unsigned numEnclosingMailBlockquotes(const Position&);
void updatePositionForNodeRemoval(Position&, Node&);

// -------------------------------------------------------------------------
// VisiblePosition
// -------------------------------------------------------------------------

// Functions returning VisiblePosition

VisiblePosition firstEditableVisiblePositionAfterPositionInRoot(const Position&, ContainerNode*);
VisiblePosition lastEditableVisiblePositionBeforePositionInRoot(const Position&, ContainerNode*);
VisiblePosition visiblePositionBeforeNode(Node&);
VisiblePosition visiblePositionAfterNode(Node&);

bool lineBreakExistsAtVisiblePosition(const VisiblePosition&);

int comparePositions(const VisiblePosition&, const VisiblePosition&);

int indexForVisiblePosition(const VisiblePosition&, RefPtrWillBeRawPtr<ContainerNode>& scope);
VisiblePosition visiblePositionForIndex(int index, ContainerNode* scope);

// -------------------------------------------------------------------------
// HTMLElement
// -------------------------------------------------------------------------

// Functions returning HTMLElement

PassRefPtrWillBeRawPtr<HTMLElement> createDefaultParagraphElement(Document&);
PassRefPtrWillBeRawPtr<HTMLBRElement> createBreakElement(Document&);
PassRefPtrWillBeRawPtr<HTMLUListElement> createUnorderedListElement(Document&);
PassRefPtrWillBeRawPtr<HTMLLIElement> createListItemElement(Document&);
PassRefPtrWillBeRawPtr<HTMLElement> createHTMLElement(Document&, const QualifiedName&);

HTMLElement* enclosingList(Node*);
HTMLElement* outermostEnclosingList(Node*, HTMLElement* rootList = nullptr);
Node* enclosingListChild(Node*);

// -------------------------------------------------------------------------
// Element
// -------------------------------------------------------------------------

// Functions returning Element

PassRefPtrWillBeRawPtr<HTMLSpanElement> createTabSpanElement(Document&);
PassRefPtrWillBeRawPtr<HTMLSpanElement> createTabSpanElement(Document&, const String& tabText);
PassRefPtrWillBeRawPtr<HTMLBRElement> createBlockPlaceholderElement(Document&);

Element* editableRootForPosition(const Position&, EditableType = ContentIsEditable);
Element* editableRootForPosition(const PositionInComposedTree&, EditableType = ContentIsEditable);
Element* unsplittableElementForPosition(const Position&);

// Boolean functions on Element

bool canMergeLists(Element* firstList, Element* secondList);

// -------------------------------------------------------------------------
// VisibleSelection
// -------------------------------------------------------------------------

// Functions returning VisibleSelection
VisibleSelection selectionForParagraphIteration(const VisibleSelection&);

Position adjustedSelectionStartForStyleComputation(const VisibleSelection&);


// Miscellaneous functions on Text
inline bool isWhitespace(UChar c)
{
    return c == noBreakSpaceCharacter || c == ' ' || c == '\n' || c == '\t';
}

// FIXME: Can't really answer this question correctly without knowing the white-space mode.
inline bool isCollapsibleWhitespace(UChar c)
{
    return c == ' ' || c == '\n';
}

inline bool isAmbiguousBoundaryCharacter(UChar character)
{
    // These are characters that can behave as word boundaries, but can appear within words.
    // If they are just typed, i.e. if they are immediately followed by a caret, we want to delay text checking until the next character has been typed.
    // FIXME: this is required until 6853027 is fixed and text checking can do this for us.
    return character == '\'' || character == rightSingleQuotationMarkCharacter || character == hebrewPunctuationGershayimCharacter;
}

String stringWithRebalancedWhitespace(const String&, bool startIsStartOfParagraph, bool endIsEndOfParagraph);
const String& nonBreakingSpaceString();

}

#endif
