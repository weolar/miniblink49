/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "core/layout/TextAutosizer.h"

#include "core/dom/Document.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutListItem.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutView.h"
#include "core/layout/line/InlineIterator.h"
#include "core/page/Page.h"

#ifdef AUTOSIZING_DOM_DEBUG_INFO
#include "core/dom/ExecutionContextTask.h"
#endif

namespace blink {

#ifdef AUTOSIZING_DOM_DEBUG_INFO
class WriteDebugInfoTask : public ExecutionContextTask {
public:
    WriteDebugInfoTask(PassRefPtrWillBeRawPtr<Element> element, AtomicString value)
        : m_element(element)
        , m_value(value)
    {
    }

    virtual void performTask(ExecutionContext*)
    {
        m_element->setAttribute("data-autosizing", m_value, ASSERT_NO_EXCEPTION);
    }

private:
    RefPtrWillBePersistent<Element> m_element;
    AtomicString m_value;
};

static void writeDebugInfo(LayoutObject* layoutObject, const AtomicString& output)
{
    Node* node = layoutObject->node();
    if (!node)
        return;
    if (node->isDocumentNode())
        node = toDocument(node)->documentElement();
    if (!node->isElementNode())
        return;
    node->document().postTask(adoptPtr(new WriteDebugInfoTask(toElement(node), output)));
}

void TextAutosizer::writeClusterDebugInfo(Cluster* cluster)
{
    String explanation = "";
    if (cluster->m_flags & SUPPRESSING) {
        explanation = "[suppressed]";
    } else if (!(cluster->m_flags & (INDEPENDENT | WIDER_OR_NARROWER))) {
        explanation = "[inherited]";
    } else if (cluster->m_supercluster) {
        explanation = "[supercluster]";
    } else if (!clusterHasEnoughTextToAutosize(cluster)) {
        explanation = "[insufficient-text]";
    } else {
        const LayoutBlock* widthProvider = clusterWidthProvider(cluster->m_root);
        if (cluster->m_hasTableAncestor && cluster->m_multiplier < multiplierFromBlock(widthProvider)) {
            explanation = "[table-ancestor-limited]";
        } else {
            explanation = String::format("[from width %d of %s]",
                static_cast<int>(widthFromBlock(widthProvider)), widthProvider->debugName().utf8().data());
        }
    }
    String pageInfo = "";
    if (cluster->m_root->isLayoutView()) {
        pageInfo = String::format("; pageinfo: bm %f * (lw %d / fw %d)",
            m_pageInfo.m_baseMultiplier, m_pageInfo.m_layoutWidth, m_pageInfo.m_frameWidth);
    }
    float multiplier = cluster->m_flags & SUPPRESSING ? 1.0 : cluster->m_multiplier;
    writeDebugInfo(const_cast<LayoutBlock*>(cluster->m_root),
        AtomicString(String::format("cluster: %f %s%s", multiplier,
            explanation.utf8().data(), pageInfo.utf8().data())));
}
#endif

static const LayoutObject* parentElementLayoutObject(const LayoutObject* layoutObject)
{
    // At style recalc, the layoutObject's parent may not be attached,
    // so we need to obtain this from the DOM tree.
    const Node* node = layoutObject->node();
    if (!node)
        return nullptr;

    // FIXME: This should be using LayoutTreeBuilderTraversal::parent().
    if (Element* parent = node->parentElement())
        return parent->layoutObject();
    return nullptr;
}

static bool isNonTextAreaFormControl(const LayoutObject* layoutObject)
{
    const Node* node = layoutObject ? layoutObject->node() : nullptr;
    if (!node || !node->isElementNode())
        return false;
    const Element* element = toElement(node);

    return (element->isFormControlElement() && !isHTMLTextAreaElement(element));
}

static bool isPotentialClusterRoot(const LayoutObject* layoutObject)
{
    // "Potential cluster roots" are the smallest unit for which we can
    // enable/disable text autosizing.
    // - Must not be inline, as different multipliers on one line looks terrible.
    //   Exceptions are inline-block and alike elements (inline-table, -webkit-inline-*),
    //   as they often contain entire multi-line columns of text.
    // - Must not be normal list items, as items in the same list should look
    //   consistent, unless they are floating or position:absolute/fixed.
    Node* node = layoutObject->generatingNode();
    if (node && !node->hasChildren())
        return false;
    if (!layoutObject->isLayoutBlock())
        return false;
    if (layoutObject->isInline() && !layoutObject->style()->isDisplayReplacedType())
        return false;
    if (layoutObject->isListItem())
        return (layoutObject->isFloating() || layoutObject->isOutOfFlowPositioned());

    return true;
}

static bool isIndependentDescendant(const LayoutBlock* layoutObject)
{
    ASSERT(isPotentialClusterRoot(layoutObject));

    LayoutBlock* containingBlock = layoutObject->containingBlock();
    return layoutObject->isLayoutView()
        || layoutObject->isFloating()
        || layoutObject->isOutOfFlowPositioned()
        || layoutObject->isTableCell()
        || layoutObject->isTableCaption()
        || layoutObject->isFlexibleBoxIncludingDeprecated()
        || (containingBlock && containingBlock->isHorizontalWritingMode() != layoutObject->isHorizontalWritingMode())
        || layoutObject->style()->isDisplayReplacedType()
        || layoutObject->isTextArea()
        || layoutObject->style()->userModify() != READ_ONLY;
}

static bool blockIsRowOfLinks(const LayoutBlock* block)
{
    // A "row of links" is a block for which:
    //  1. It does not contain non-link text elements longer than 3 characters
    //  2. It contains a minimum of 3 inline links and all links should
    //     have the same specified font size.
    //  3. It should not contain <br> elements.
    //  4. It should contain only inline elements unless they are containers,
    //     children of link elements or children of sub-containers.
    int linkCount = 0;
    LayoutObject* layoutObject = block->firstChild();
    float matchingFontSize = -1;

    while (layoutObject) {
        if (!isPotentialClusterRoot(layoutObject)) {
            if (layoutObject->isText() && toLayoutText(layoutObject)->text().stripWhiteSpace().length() > 3)
                return false;
            if (!layoutObject->isInline() || layoutObject->isBR())
                return false;
        }
        if (layoutObject->style()->isLink()) {
            linkCount++;
            if (matchingFontSize < 0)
                matchingFontSize = layoutObject->style()->specifiedFontSize();
            else if (matchingFontSize != layoutObject->style()->specifiedFontSize())
                return false;

            // Skip traversing descendants of the link.
            layoutObject = layoutObject->nextInPreOrderAfterChildren(block);
            continue;
        }
        layoutObject = layoutObject->nextInPreOrder(block);
    }

    return (linkCount >= 3);
}

static bool blockHeightConstrained(const LayoutBlock* block)
{
    // FIXME: Propagate constrainedness down the tree, to avoid inefficiently walking back up from each box.
    // FIXME: This code needs to take into account vertical writing modes.
    // FIXME: Consider additional heuristics, such as ignoring fixed heights if the content is already overflowing before autosizing kicks in.
    for (; block; block = block->containingBlock()) {
        const ComputedStyle& style = block->styleRef();
        if (style.overflowY() >= OSCROLL)
            return false;
        if (style.height().isSpecified() || style.maxHeight().isSpecified() || block->isOutOfFlowPositioned()) {
            // Some sites (e.g. wikipedia) set their html and/or body elements to height:100%,
            // without intending to constrain the height of the content within them.
            return !block->isDocumentElement() && !block->isBody() && !block->isLayoutView();
        }
        if (block->isFloating())
            return false;
    }
    return false;
}

static bool blockOrImmediateChildrenAreFormControls(const LayoutBlock* block)
{
    if (isNonTextAreaFormControl(block))
        return true;
    const LayoutObject* layoutObject = block->firstChild();
    while (layoutObject) {
        if (isNonTextAreaFormControl(layoutObject))
            return true;
        layoutObject = layoutObject->nextSibling();
    }

    return false;
}

// Some blocks are not autosized even if their parent cluster wants them to.
static bool blockSuppressesAutosizing(const LayoutBlock* block)
{
    if (blockOrImmediateChildrenAreFormControls(block))
        return true;

    if (blockIsRowOfLinks(block))
        return true;

    // Don't autosize block-level text that can't wrap (as it's likely to
    // expand sideways and break the page's layout).
    if (!block->style()->autoWrap())
        return true;

    if (blockHeightConstrained(block))
        return true;

    return false;
}

static bool hasExplicitWidth(const LayoutBlock* block)
{
    // FIXME: This heuristic may need to be expanded to other ways a block can be wider or narrower
    //        than its parent containing block.
    return block->style() && block->style()->width().isSpecified();
}

TextAutosizer::TextAutosizer(const Document* document)
    : m_document(document)
    , m_firstBlockToBeginLayout(nullptr)
#if ENABLE(ASSERT)
    , m_blocksThatHaveBegunLayout()
#endif
    , m_superclusters()
    , m_clusterStack()
    , m_fingerprintMapper()
    , m_pageInfo()
    , m_updatePageInfoDeferred(false)
{
}

void TextAutosizer::record(const LayoutBlock* block)
{
    if (!m_pageInfo.m_settingEnabled)
        return;

    ASSERT(!m_blocksThatHaveBegunLayout.contains(block));

    if (!classifyBlock(block, INDEPENDENT | EXPLICIT_WIDTH))
        return;

    if (Fingerprint fingerprint = computeFingerprint(block))
        m_fingerprintMapper.addTentativeClusterRoot(block, fingerprint);
}

void TextAutosizer::destroy(const LayoutBlock* block)
{
    if (!m_pageInfo.m_settingEnabled && !m_fingerprintMapper.hasFingerprints())
        return;

    ASSERT(!m_blocksThatHaveBegunLayout.contains(block));

    if (m_fingerprintMapper.remove(block) && m_firstBlockToBeginLayout) {
        // LayoutBlock with a fingerprint was destroyed during layout.
        // Clear the cluster stack and the supercluster map to avoid stale pointers.
        // Speculative fix for http://crbug.com/369485.
        m_firstBlockToBeginLayout = 0;
        m_clusterStack.clear();
        m_superclusters.clear();
    }
}

TextAutosizer::BeginLayoutBehavior TextAutosizer::prepareForLayout(const LayoutBlock* block)
{
#if ENABLE(ASSERT)
    m_blocksThatHaveBegunLayout.add(block);
#endif

    if (!m_firstBlockToBeginLayout) {
        m_firstBlockToBeginLayout = block;
        prepareClusterStack(block->parent());
    } else if (block == currentCluster()->m_root) {
        // Ignore beginLayout on the same block twice.
        // This can happen with paginated overflow.
        return StopLayout;
    }

    return ContinueLayout;
}

void TextAutosizer::prepareClusterStack(const LayoutObject* layoutObject)
{
    if (!layoutObject)
        return;
    prepareClusterStack(layoutObject->parent());

    if (layoutObject->isLayoutBlock()) {
        const LayoutBlock* block = toLayoutBlock(layoutObject);
#if ENABLE(ASSERT)
        m_blocksThatHaveBegunLayout.add(block);
#endif
        if (Cluster* cluster = maybeCreateCluster(block))
            m_clusterStack.append(adoptPtr(cluster));
    }
}

void TextAutosizer::beginLayout(LayoutBlock* block)
{
    ASSERT(shouldHandleLayout());

    if (prepareForLayout(block) == StopLayout)
        return;

    if (Cluster* cluster = maybeCreateCluster(block))
        m_clusterStack.append(adoptPtr(cluster));

    // Cells in auto-layout tables are handled separately by inflateAutoTable.
    bool isAutoTableCell = block->isTableCell() && !toLayoutTableCell(block)->table()->style()->isFixedTableLayout();
    if (!isAutoTableCell && !m_clusterStack.isEmpty())
        inflate(block);
}

void TextAutosizer::inflateAutoTable(LayoutTable* table)
{
    ASSERT(table);
    ASSERT(!table->style()->isFixedTableLayout());
    ASSERT(table->containingBlock());

    Cluster* cluster = currentCluster();
    if (cluster->m_root != table)
        return;

    // Pre-inflate cells that have enough text so that their inflated preferred widths will be used
    // for column sizing.
    for (LayoutObject* section = table->firstChild(); section; section = section->nextSibling()) {
        if (!section->isTableSection())
            continue;
        for (LayoutTableRow* row = toLayoutTableSection(section)->firstRow(); row; row = row->nextRow()) {
            for (LayoutTableCell* cell = row->firstCell(); cell; cell = cell->nextCell()) {
                if (!cell->needsLayout())
                    continue;

                beginLayout(cell);
                inflate(cell, DescendToInnerBlocks);
                endLayout(cell);
            }
        }
    }
}

void TextAutosizer::endLayout(LayoutBlock* block)
{
    ASSERT(shouldHandleLayout());

    if (block == m_firstBlockToBeginLayout) {
        m_firstBlockToBeginLayout = 0;
        m_clusterStack.clear();
        m_superclusters.clear();
        m_stylesRetainedDuringLayout.clear();
#if ENABLE(ASSERT)
        m_blocksThatHaveBegunLayout.clear();
#endif
    // Tables can create two layout scopes for the same block so the isEmpty
    // check below is needed to guard against endLayout being called twice.
    } else if (!m_clusterStack.isEmpty() && currentCluster()->m_root == block) {
        m_clusterStack.removeLast();
    }
}

float TextAutosizer::inflate(LayoutObject* parent, InflateBehavior behavior, float multiplier)
{
    Cluster* cluster = currentCluster();
    bool hasTextChild = false;

    LayoutObject* child = nullptr;
    if (parent->isLayoutBlock() && (parent->childrenInline() || behavior == DescendToInnerBlocks))
        child = toLayoutBlock(parent)->firstChild();
    else if (parent->isLayoutInline())
        child = toLayoutInline(parent)->firstChild();

    while (child) {
        if (child->isText()) {
            hasTextChild = true;
            // We only calculate this multiplier on-demand to ensure the parent block of this text
            // has entered layout.
            if (!multiplier)
                multiplier = cluster->m_flags & SUPPRESSING ? 1.0f : clusterMultiplier(cluster);
            applyMultiplier(child, multiplier);

            // FIXME: Investigate why MarkOnlyThis is sufficient.
            if (parent->isLayoutInline())
                child->setPreferredLogicalWidthsDirty(MarkOnlyThis);
        } else if (child->isLayoutInline()) {
            multiplier = inflate(child, behavior, multiplier);
        } else if (child->isLayoutBlock() && behavior == DescendToInnerBlocks
            && !classifyBlock(child, INDEPENDENT | EXPLICIT_WIDTH | SUPPRESSING)) {
            multiplier = inflate(child, behavior, multiplier);
        }
        child = child->nextSibling();
    }

    if (hasTextChild) {
        applyMultiplier(parent, multiplier); // Parent handles line spacing.
    } else if (!parent->isListItem()) {
        // For consistency, a block with no immediate text child should always have a
        // multiplier of 1.
        applyMultiplier(parent, 1);
    }

    if (parent->isListItem()) {
        float multiplier = clusterMultiplier(cluster);
        applyMultiplier(parent, multiplier);

        // The list item has to be treated special because we can have a tree such that you have
        // a list item for a form inside it. The list marker then ends up inside the form and when
        // we try to get the clusterMultiplier we have the wrong cluster root to work from and get
        // the wrong value.
        LayoutListItem* item = toLayoutListItem(parent);
        if (LayoutListMarker* marker = item->marker()) {
            applyMultiplier(marker, multiplier);
            marker->setPreferredLogicalWidthsDirty(MarkOnlyThis);
        }
    }

    return multiplier;
}

bool TextAutosizer::shouldHandleLayout() const
{
    return m_pageInfo.m_settingEnabled && m_pageInfo.m_pageNeedsAutosizing && !m_updatePageInfoDeferred;
}

void TextAutosizer::updatePageInfoInAllFrames()
{
    ASSERT(!m_document->frame() || m_document->frame()->isMainFrame());

    for (Frame* frame = m_document->frame(); frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;

        Document* document = toLocalFrame(frame)->document();
        // If document is being detached, skip updatePageInfo.
        if (!document || !document->isActive())
            continue;
        if (TextAutosizer* textAutosizer = document->textAutosizer())
            textAutosizer->updatePageInfo();
    }
}

void TextAutosizer::updatePageInfo()
{
    if (m_updatePageInfoDeferred || !m_document->page() || !m_document->settings())
        return;

    PageInfo previousPageInfo(m_pageInfo);
    m_pageInfo.m_settingEnabled = m_document->settings()->textAutosizingEnabled();

    if (!m_pageInfo.m_settingEnabled || m_document->printing()) {
        m_pageInfo.m_pageNeedsAutosizing = false;
    } else {
        LayoutView* layoutView = m_document->layoutView();
        bool horizontalWritingMode = isHorizontalWritingMode(layoutView->style()->writingMode());

        // FIXME: With out-of-process iframes, the top frame can be remote and
        // doesn't have sizing information. Just return if this is the case.
        Frame* frame = m_document->frame()->tree().top();
        if (frame->isRemoteFrame())
            return;

        LocalFrame* mainFrame = m_document->page()->deprecatedLocalMainFrame();
        IntSize frameSize = m_document->settings()->textAutosizingWindowSizeOverride();
        if (frameSize.isEmpty())
            frameSize = windowSize();

        m_pageInfo.m_frameWidth = horizontalWritingMode ? frameSize.width() : frameSize.height();

        IntSize layoutSize = mainFrame->view()->layoutSize();
        m_pageInfo.m_layoutWidth = horizontalWritingMode ? layoutSize.width() : layoutSize.height();

        // Compute the base font scale multiplier based on device and accessibility settings.
        m_pageInfo.m_baseMultiplier = m_document->settings()->accessibilityFontScaleFactor();
        // If the page has a meta viewport or @viewport, don't apply the device scale adjustment.
        const ViewportDescription& viewportDescription = mainFrame->document()->viewportDescription();
        if (!viewportDescription.isSpecifiedByAuthor()) {
            float deviceScaleAdjustment = m_document->settings()->deviceScaleAdjustment();
            m_pageInfo.m_baseMultiplier *= deviceScaleAdjustment;
        }

        m_pageInfo.m_pageNeedsAutosizing = !!m_pageInfo.m_frameWidth
            && (m_pageInfo.m_baseMultiplier * (static_cast<float>(m_pageInfo.m_layoutWidth) / m_pageInfo.m_frameWidth) > 1.0f);
    }

    if (m_pageInfo.m_pageNeedsAutosizing) {
        // If page info has changed, multipliers may have changed. Force a layout to recompute them.
        if (m_pageInfo.m_frameWidth != previousPageInfo.m_frameWidth
            || m_pageInfo.m_layoutWidth != previousPageInfo.m_layoutWidth
            || m_pageInfo.m_baseMultiplier != previousPageInfo.m_baseMultiplier
            || m_pageInfo.m_settingEnabled != previousPageInfo.m_settingEnabled)
            setAllTextNeedsLayout();
    } else if (previousPageInfo.m_hasAutosized) {
        // If we are no longer autosizing the page, we won't do anything during the next layout.
        // Set all the multipliers back to 1 now.
        resetMultipliers();
        m_pageInfo.m_hasAutosized = false;
    }
}

IntSize TextAutosizer::windowSize() const
{
    Page * page = m_document->page();
    ASSERT(page);
    return page->frameHost().pinchViewport().size();
}

void TextAutosizer::resetMultipliers()
{
    LayoutObject* layoutObject = m_document->layoutView();
    while (layoutObject) {
        if (const ComputedStyle* style = layoutObject->style()) {
            if (style->textAutosizingMultiplier() != 1)
                applyMultiplier(layoutObject, 1, LayoutNeeded);
        }
        layoutObject = layoutObject->nextInPreOrder();
    }
}

void TextAutosizer::setAllTextNeedsLayout()
{
    LayoutObject* layoutObject = m_document->layoutView();
    while (layoutObject) {
        if (layoutObject->isText())
            layoutObject->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::TextAutosizing);
        layoutObject = layoutObject->nextInPreOrder();
    }
}

TextAutosizer::BlockFlags TextAutosizer::classifyBlock(const LayoutObject* layoutObject, BlockFlags mask) const
{
    if (!layoutObject->isLayoutBlock())
        return 0;

    const LayoutBlock* block = toLayoutBlock(layoutObject);
    BlockFlags flags = 0;

    if (isPotentialClusterRoot(block)) {
        if (mask & POTENTIAL_ROOT)
            flags |= POTENTIAL_ROOT;

        if ((mask & INDEPENDENT) && (isIndependentDescendant(block) || block->isTable()))
            flags |= INDEPENDENT;

        if ((mask & EXPLICIT_WIDTH) && hasExplicitWidth(block))
            flags |= EXPLICIT_WIDTH;

        if ((mask & SUPPRESSING) && blockSuppressesAutosizing(block))
            flags |= SUPPRESSING;
    }
    return flags;
}

bool TextAutosizer::clusterWouldHaveEnoughTextToAutosize(const LayoutBlock* root, const LayoutBlock* widthProvider)
{
    Cluster hypotheticalCluster(root, classifyBlock(root), nullptr);
    return clusterHasEnoughTextToAutosize(&hypotheticalCluster, widthProvider);
}

bool TextAutosizer::clusterHasEnoughTextToAutosize(Cluster* cluster, const LayoutBlock* widthProvider)
{
    if (cluster->m_hasEnoughTextToAutosize != UnknownAmountOfText)
        return cluster->m_hasEnoughTextToAutosize == HasEnoughText;

    const LayoutBlock* root = cluster->m_root;
    if (!widthProvider)
        widthProvider = clusterWidthProvider(root);

    // TextAreas and user-modifiable areas get a free pass to autosize regardless of text content.
    if (root->isTextArea() || (root->style() && root->style()->userModify() != READ_ONLY)) {
        cluster->m_hasEnoughTextToAutosize = HasEnoughText;
        return true;
    }

    if (cluster->m_flags & SUPPRESSING) {
        cluster->m_hasEnoughTextToAutosize = NotEnoughText;
        return false;
    }

    // 4 lines of text is considered enough to autosize.
    float minimumTextLengthToAutosize = widthFromBlock(widthProvider) * 4;

    float length = 0;
    LayoutObject* descendant = root->firstChild();
    while (descendant) {
        if (descendant->isLayoutBlock()) {
            if (classifyBlock(descendant, INDEPENDENT | SUPPRESSING)) {
                descendant = descendant->nextInPreOrderAfterChildren(root);
                continue;
            }
        } else if (descendant->isText()) {
            // Note: Using text().stripWhiteSpace().length() instead of resolvedTextLength() because
            // the lineboxes will not be built until layout. These values can be different.
            // Note: This is an approximation assuming each character is 1em wide.
            length += toLayoutText(descendant)->text().stripWhiteSpace().length() * descendant->style()->specifiedFontSize();

            if (length >= minimumTextLengthToAutosize) {
                cluster->m_hasEnoughTextToAutosize = HasEnoughText;
                return true;
            }
        }
        descendant = descendant->nextInPreOrder(root);
    }

    cluster->m_hasEnoughTextToAutosize = NotEnoughText;
    return false;
}

TextAutosizer::Fingerprint TextAutosizer::getFingerprint(const LayoutObject* layoutObject)
{
    Fingerprint result = m_fingerprintMapper.get(layoutObject);
    if (!result) {
        result = computeFingerprint(layoutObject);
        m_fingerprintMapper.add(layoutObject, result);
    }
    return result;
}

TextAutosizer::Fingerprint TextAutosizer::computeFingerprint(const LayoutObject* layoutObject)
{
    Node* node = layoutObject->generatingNode();
    if (!node || !node->isElementNode())
        return 0;

    FingerprintSourceData data;
    if (const LayoutObject* parent = parentElementLayoutObject(layoutObject))
        data.m_parentHash = getFingerprint(parent);

    data.m_qualifiedNameHash = QualifiedNameHash::hash(toElement(node)->tagQName());

    if (const ComputedStyle* style = layoutObject->style()) {
        data.m_packedStyleProperties = style->direction();
        data.m_packedStyleProperties |= (style->position() << 1);
        data.m_packedStyleProperties |= (style->floating() << 4);
        data.m_packedStyleProperties |= (style->display() << 6);
        data.m_packedStyleProperties |= (style->width().type() << 11);
        // packedStyleProperties effectively using 15 bits now.

        // consider for adding: writing mode, padding.

        data.m_width = style->width().getFloatValue();
    }

    // Use nodeIndex as a rough approximation of column number
    // (it's too early to call LayoutTableCell::col).
    // FIXME: account for colspan
    if (layoutObject->isTableCell())
        data.m_column = layoutObject->node()->nodeIndex();

    return StringHasher::computeHash<UChar>(
        static_cast<const UChar*>(static_cast<const void*>(&data)),
        sizeof data / sizeof(UChar));
}

TextAutosizer::Cluster* TextAutosizer::maybeCreateCluster(const LayoutBlock* block)
{
    BlockFlags flags = classifyBlock(block);
    if (!(flags & POTENTIAL_ROOT))
        return nullptr;

    Cluster* parentCluster = m_clusterStack.isEmpty() ? nullptr : currentCluster();
    ASSERT(parentCluster || block->isLayoutView());

    // If a non-independent block would not alter the SUPPRESSING flag, it doesn't need to be a cluster.
    bool parentSuppresses = parentCluster && (parentCluster->m_flags & SUPPRESSING);
    if (!(flags & INDEPENDENT) && !(flags & EXPLICIT_WIDTH) && !!(flags & SUPPRESSING) == parentSuppresses)
        return nullptr;

    Cluster* cluster = new Cluster(block, flags, parentCluster, getSupercluster(block));
#ifdef AUTOSIZING_DOM_DEBUG_INFO
    // Non-SUPPRESSING clusters are annotated in clusterMultiplier.
    if (flags & SUPPRESSING)
        writeClusterDebugInfo(cluster);
#endif
    return cluster;
}

TextAutosizer::Supercluster* TextAutosizer::getSupercluster(const LayoutBlock* block)
{
    Fingerprint fingerprint = m_fingerprintMapper.get(block);
    if (!fingerprint)
        return nullptr;

    BlockSet* roots = m_fingerprintMapper.getTentativeClusterRoots(fingerprint);
    if (!roots || roots->size() < 2 || !roots->contains(block))
        return nullptr;

    SuperclusterMap::AddResult addResult = m_superclusters.add(fingerprint, PassOwnPtr<Supercluster>());
    if (!addResult.isNewEntry)
        return addResult.storedValue->value.get();

    Supercluster* supercluster = new Supercluster(roots);
    addResult.storedValue->value = adoptPtr(supercluster);
    return supercluster;
}

float TextAutosizer::clusterMultiplier(Cluster* cluster)
{
    if (cluster->m_multiplier)
        return cluster->m_multiplier;

    // FIXME: why does isWiderOrNarrowerDescendant crash on independent clusters?
    if (!(cluster->m_flags & INDEPENDENT) && isWiderOrNarrowerDescendant(cluster))
        cluster->m_flags |= WIDER_OR_NARROWER;

    if (cluster->m_flags & (INDEPENDENT | WIDER_OR_NARROWER)) {
        if (cluster->m_supercluster)
            cluster->m_multiplier = superclusterMultiplier(cluster);
        else if (clusterHasEnoughTextToAutosize(cluster))
            cluster->m_multiplier = multiplierFromBlock(clusterWidthProvider(cluster->m_root));
        else
            cluster->m_multiplier = 1.0f;
    } else {
        cluster->m_multiplier = cluster->m_parent ? clusterMultiplier(cluster->m_parent) : 1.0f;
    }

#ifdef AUTOSIZING_DOM_DEBUG_INFO
    writeClusterDebugInfo(cluster);
#endif

    ASSERT(cluster->m_multiplier);
    return cluster->m_multiplier;
}

bool TextAutosizer::superclusterHasEnoughTextToAutosize(Supercluster* supercluster, const LayoutBlock* widthProvider)
{
    if (supercluster->m_hasEnoughTextToAutosize != UnknownAmountOfText)
        return supercluster->m_hasEnoughTextToAutosize == HasEnoughText;

    for (auto* root : *supercluster->m_roots) {
        if (clusterWouldHaveEnoughTextToAutosize(root, widthProvider)) {
            supercluster->m_hasEnoughTextToAutosize = HasEnoughText;
            return true;
        }
    }
    supercluster->m_hasEnoughTextToAutosize = NotEnoughText;
    return false;
}

float TextAutosizer::superclusterMultiplier(Cluster* cluster)
{
    Supercluster* supercluster = cluster->m_supercluster;
    if (!supercluster->m_multiplier) {
        const LayoutBlock* widthProvider = maxClusterWidthProvider(cluster->m_supercluster, cluster->m_root);
        supercluster->m_multiplier = superclusterHasEnoughTextToAutosize(supercluster, widthProvider)
            ? multiplierFromBlock(widthProvider) : 1.0f;
    }
    ASSERT(supercluster->m_multiplier);
    return supercluster->m_multiplier;
}

const LayoutBlock* TextAutosizer::clusterWidthProvider(const LayoutBlock* root) const
{
    if (root->isTable() || root->isTableCell())
        return root;

    return deepestBlockContainingAllText(root);
}

const LayoutBlock* TextAutosizer::maxClusterWidthProvider(const Supercluster* supercluster, const LayoutBlock* currentRoot) const
{
    const LayoutBlock* result = clusterWidthProvider(currentRoot);
    float maxWidth = widthFromBlock(result);

    const BlockSet* roots = supercluster->m_roots;
    for (const auto* root : *roots) {
        const LayoutBlock* widthProvider = clusterWidthProvider(root);
        if (widthProvider->needsLayout())
            continue;
        float width = widthFromBlock(widthProvider);
        if (width > maxWidth) {
            maxWidth = width;
            result = widthProvider;
        }
    }
    RELEASE_ASSERT(result);
    return result;
}

float TextAutosizer::widthFromBlock(const LayoutBlock* block) const
{
    RELEASE_ASSERT(block);
    RELEASE_ASSERT(block->style());

    if (!(block->isTable() || block->isTableCell() || block->isListItem()))
        return block->contentLogicalWidth().toFloat();

    if (!block->containingBlock())
        return 0;

    // Tables may be inflated before computing their preferred widths. Try several methods to
    // obtain a width, and fall back on a containing block's width.
    for (; block; block = block->containingBlock()) {
        float width;
        Length specifiedWidth = block->isTableCell()
            ? toLayoutTableCell(block)->styleOrColLogicalWidth() : block->style()->logicalWidth();
        if (specifiedWidth.isFixed()) {
            if ((width = specifiedWidth.value()) > 0)
                return width;
        }
        if (specifiedWidth.hasPercent()) {
            if (float containerWidth = block->containingBlock()->contentLogicalWidth().toFloat()) {
                if ((width = floatValueForLength(specifiedWidth, containerWidth)) > 0)
                    return width;
            }
        }
        if ((width = block->contentLogicalWidth().toFloat()) > 0)
            return width;
    }
    return 0;
}

float TextAutosizer::multiplierFromBlock(const LayoutBlock* block)
{
    // If block->needsLayout() is false, it does not need to be in m_blocksThatHaveBegunLayout.
    // This can happen during layout of a positioned object if the cluster's DBCAT is deeper
    // than the positioned object's containing block, and wasn't marked as needing layout.
    ASSERT(m_blocksThatHaveBegunLayout.contains(block) || !block->needsLayout());

    // Block width, in CSS pixels.
    float blockWidth = widthFromBlock(block);
    float multiplier = m_pageInfo.m_frameWidth ? std::min(blockWidth, static_cast<float>(m_pageInfo.m_layoutWidth)) / m_pageInfo.m_frameWidth : 1.0f;

    return std::max(m_pageInfo.m_baseMultiplier * multiplier, 1.0f);
}

const LayoutBlock* TextAutosizer::deepestBlockContainingAllText(Cluster* cluster)
{
    if (!cluster->m_deepestBlockContainingAllText)
        cluster->m_deepestBlockContainingAllText = deepestBlockContainingAllText(cluster->m_root);

    return cluster->m_deepestBlockContainingAllText;
}

// FIXME: Refactor this to look more like TextAutosizer::deepestCommonAncestor.
const LayoutBlock* TextAutosizer::deepestBlockContainingAllText(const LayoutBlock* root) const
{
    size_t firstDepth = 0;
    const LayoutObject* firstTextLeaf = findTextLeaf(root, firstDepth, First);
    if (!firstTextLeaf)
        return root;

    size_t lastDepth = 0;
    const LayoutObject* lastTextLeaf = findTextLeaf(root, lastDepth, Last);
    ASSERT(lastTextLeaf);

    // Equalize the depths if necessary. Only one of the while loops below will get executed.
    const LayoutObject* firstNode = firstTextLeaf;
    const LayoutObject* lastNode = lastTextLeaf;
    while (firstDepth > lastDepth) {
        firstNode = firstNode->parent();
        --firstDepth;
    }
    while (lastDepth > firstDepth) {
        lastNode = lastNode->parent();
        --lastDepth;
    }

    // Go up from both nodes until the parent is the same. Both pointers will point to the LCA then.
    while (firstNode != lastNode) {
        firstNode = firstNode->parent();
        lastNode = lastNode->parent();
    }

    if (firstNode->isLayoutBlock())
        return toLayoutBlock(firstNode);

    // containingBlock() should never leave the cluster, since it only skips ancestors when finding
    // the container of position:absolute/fixed blocks, and those cannot exist between a cluster and
    // its text node's lowest common ancestor as isAutosizingCluster would have made them into their
    // own independent cluster.
    const LayoutBlock* containingBlock = firstNode->containingBlock();
    if (!containingBlock)
        return root;

    ASSERT(containingBlock->isDescendantOf(root));
    return containingBlock;
}

const LayoutObject* TextAutosizer::findTextLeaf(const LayoutObject* parent, size_t& depth, TextLeafSearch firstOrLast) const
{
    // List items are treated as text due to the marker.
    if (parent->isListItem())
        return parent;

    if (parent->isText())
        return parent;

    ++depth;
    const LayoutObject* child = (firstOrLast == First) ? parent->slowFirstChild() : parent->slowLastChild();
    while (child) {
        // Note: At this point clusters may not have been created for these blocks so we cannot rely
        //       on m_clusters. Instead, we use a best-guess about whether the block will become a cluster.
        if (!classifyBlock(child, INDEPENDENT)) {
            if (const LayoutObject* leaf = findTextLeaf(child, depth, firstOrLast))
                return leaf;
        }
        child = (firstOrLast == First) ? child->nextSibling() : child->previousSibling();
    }
    --depth;

    return nullptr;
}

void TextAutosizer::applyMultiplier(LayoutObject* layoutObject, float multiplier, RelayoutBehavior relayoutBehavior)
{
    ASSERT(layoutObject);
    ComputedStyle& currentStyle = layoutObject->mutableStyleRef();
    if (currentStyle.textAutosizingMultiplier() == multiplier)
        return;

    // We need to clone the layoutObject style to avoid breaking style sharing.
    RefPtr<ComputedStyle> style = ComputedStyle::clone(currentStyle);
    style->setTextAutosizingMultiplier(multiplier);
    style->setUnique();

    switch (relayoutBehavior) {
    case AlreadyInLayout:
        // Don't free currentStyle until the end of the layout pass. This allows other parts of the system
        // to safely hold raw ComputedStyle* pointers during layout, e.g. BreakingContext::m_currentStyle.
        m_stylesRetainedDuringLayout.append(&currentStyle);

        layoutObject->setStyleInternal(style.release());
        layoutObject->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::TextAutosizing);
        break;

    case LayoutNeeded:
        layoutObject->setStyle(style.release());
        break;
    }

    if (multiplier != 1)
        m_pageInfo.m_hasAutosized = true;
}

bool TextAutosizer::isWiderOrNarrowerDescendant(Cluster* cluster)
{
    // FIXME: Why do we return true when hasExplicitWidth returns false??
    if (!cluster->m_parent || !hasExplicitWidth(cluster->m_root))
        return true;

    const LayoutBlock* parentDeepestBlockContainingAllText = deepestBlockContainingAllText(cluster->m_parent);
    ASSERT(m_blocksThatHaveBegunLayout.contains(cluster->m_root));
    ASSERT(m_blocksThatHaveBegunLayout.contains(parentDeepestBlockContainingAllText));

    float contentWidth = cluster->m_root->contentLogicalWidth().toFloat();
    float clusterTextWidth = parentDeepestBlockContainingAllText->contentLogicalWidth().toFloat();

    // Clusters with a root that is wider than the deepestBlockContainingAllText of their parent
    // autosize independently of their parent.
    if (contentWidth > clusterTextWidth)
        return true;

    // Clusters with a root that is significantly narrower than the deepestBlockContainingAllText of
    // their parent autosize independently of their parent.
    static float narrowWidthDifference = 200;
    if (clusterTextWidth - contentWidth > narrowWidthDifference)
        return true;

    return false;
}

TextAutosizer::Cluster* TextAutosizer::currentCluster() const
{
    ASSERT_WITH_SECURITY_IMPLICATION(!m_clusterStack.isEmpty());
    return m_clusterStack.last().get();
}

#if ENABLE(ASSERT)
void TextAutosizer::FingerprintMapper::assertMapsAreConsistent()
{
    // For each fingerprint -> block mapping in m_blocksForFingerprint we should have an associated
    // map from block -> fingerprint in m_fingerprints.
    ReverseFingerprintMap::iterator end = m_blocksForFingerprint.end();
    for (ReverseFingerprintMap::iterator fingerprintIt = m_blocksForFingerprint.begin(); fingerprintIt != end; ++fingerprintIt) {
        Fingerprint fingerprint = fingerprintIt->key;
        BlockSet* blocks = fingerprintIt->value.get();
        for (BlockSet::iterator blockIt = blocks->begin(); blockIt != blocks->end(); ++blockIt) {
            const LayoutBlock* block = (*blockIt);
            ASSERT(m_fingerprints.get(block) == fingerprint);
        }
    }
}
#endif

void TextAutosizer::FingerprintMapper::add(const LayoutObject* layoutObject, Fingerprint fingerprint)
{
    remove(layoutObject);

    m_fingerprints.set(layoutObject, fingerprint);
#if ENABLE(ASSERT)
    assertMapsAreConsistent();
#endif
}

void TextAutosizer::FingerprintMapper::addTentativeClusterRoot(const LayoutBlock* block, Fingerprint fingerprint)
{
    add(block, fingerprint);

    ReverseFingerprintMap::AddResult addResult = m_blocksForFingerprint.add(fingerprint, PassOwnPtr<BlockSet>());
    if (addResult.isNewEntry)
        addResult.storedValue->value = adoptPtr(new BlockSet);
    addResult.storedValue->value->add(block);
#if ENABLE(ASSERT)
    assertMapsAreConsistent();
#endif
}

bool TextAutosizer::FingerprintMapper::remove(const LayoutObject* layoutObject)
{
    Fingerprint fingerprint = m_fingerprints.take(layoutObject);
    if (!fingerprint || !layoutObject->isLayoutBlock())
        return false;

    ReverseFingerprintMap::iterator blocksIter = m_blocksForFingerprint.find(fingerprint);
    if (blocksIter == m_blocksForFingerprint.end())
        return false;

    BlockSet& blocks = *blocksIter->value;
    blocks.remove(toLayoutBlock(layoutObject));
    if (blocks.isEmpty())
        m_blocksForFingerprint.remove(blocksIter);
#if ENABLE(ASSERT)
    assertMapsAreConsistent();
#endif
    return true;
}

TextAutosizer::Fingerprint TextAutosizer::FingerprintMapper::get(const LayoutObject* layoutObject)
{
    return m_fingerprints.get(layoutObject);
}

TextAutosizer::BlockSet* TextAutosizer::FingerprintMapper::getTentativeClusterRoots(Fingerprint fingerprint)
{
    return m_blocksForFingerprint.get(fingerprint);
}

TextAutosizer::LayoutScope::LayoutScope(LayoutBlock* block)
    : m_textAutosizer(block->document().textAutosizer())
    , m_block(block)
{
    if (!m_textAutosizer)
        return;

    if (m_textAutosizer->shouldHandleLayout())
        m_textAutosizer->beginLayout(m_block);
    else
        m_textAutosizer = 0;
}

TextAutosizer::LayoutScope::~LayoutScope()
{
    if (m_textAutosizer)
        m_textAutosizer->endLayout(m_block);
}


TextAutosizer::TableLayoutScope::TableLayoutScope(LayoutTable* table)
    : LayoutScope(table)
{
    if (m_textAutosizer) {
        ASSERT(m_textAutosizer->shouldHandleLayout());
        m_textAutosizer->inflateAutoTable(table);
    }
}

TextAutosizer::DeferUpdatePageInfo::DeferUpdatePageInfo(Page* page)
    : m_mainFrame(page->deprecatedLocalMainFrame())
{
    if (TextAutosizer* textAutosizer = m_mainFrame->document()->textAutosizer()) {
        ASSERT(!textAutosizer->m_updatePageInfoDeferred);
        textAutosizer->m_updatePageInfoDeferred = true;
    }
}

TextAutosizer::DeferUpdatePageInfo::~DeferUpdatePageInfo()
{
    if (TextAutosizer* textAutosizer = m_mainFrame->document()->textAutosizer()) {
        ASSERT(textAutosizer->m_updatePageInfoDeferred);
        textAutosizer->m_updatePageInfoDeferred = false;
        textAutosizer->updatePageInfoInAllFrames();
    }
}

float TextAutosizer::computeAutosizedFontSize(float specifiedSize, float multiplier)
{
    // Somewhat arbitrary "pleasant" font size.
    const float pleasantSize = 16;

    // Multiply fonts that the page author has specified to be larger than
    // pleasantSize by less and less, until huge fonts are not increased at all.
    // For specifiedSize between 0 and pleasantSize we directly apply the
    // multiplier; hence for specifiedSize == pleasantSize, computedSize will be
    // multiplier * pleasantSize. For greater specifiedSizes we want to
    // gradually fade out the multiplier, so for every 1px increase in
    // specifiedSize beyond pleasantSize we will only increase computedSize
    // by gradientAfterPleasantSize px until we meet the
    // computedSize = specifiedSize line, after which we stay on that line (so
    // then every 1px increase in specifiedSize increases computedSize by 1px).
    const float gradientAfterPleasantSize = 0.5;

    float computedSize;
    if (specifiedSize <= pleasantSize) {
        computedSize = multiplier * specifiedSize;
    } else {
        computedSize = multiplier * pleasantSize + gradientAfterPleasantSize * (specifiedSize - pleasantSize);
        if (computedSize < specifiedSize)
            computedSize = specifiedSize;
    }
    return computedSize;
}

DEFINE_TRACE(TextAutosizer)
{
    visitor->trace(m_document);
}

} // namespace blink
