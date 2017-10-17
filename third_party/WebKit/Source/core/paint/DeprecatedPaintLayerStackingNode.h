/*
 * Copyright (C) 2003, 2009, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
 *
 * Portions are Copyright (C) 1998 Netscape Communications Corporation.
 *
 * Other contributors:
 *   Robert O'Callahan <roc+@cs.cmu.edu>
 *   David Baron <dbaron@fas.harvard.edu>
 *   Christian Biesinger <cbiesinger@web.de>
 *   Randall Jesup <rjesup@wgate.com>
 *   Roland Mainz <roland.mainz@informatik.med.uni-giessen.de>
 *   Josh Soref <timeless@mac.com>
 *   Boris Zbarsky <bzbarsky@mit.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Alternatively, the contents of this file may be used under the terms
 * of either the Mozilla Public License Version 1.1, found at
 * http://www.mozilla.org/MPL/ (the "MPL") or the GNU General Public
 * License Version 2.0, found at http://www.fsf.org/copyleft/gpl.html
 * (the "GPL"), in which case the provisions of the MPL or the GPL are
 * applicable instead of those above.  If you wish to allow use of your
 * version of this file only under the terms of one of those two
 * licenses (the MPL or the GPL) and not to allow others to use your
 * version of this file under the LGPL, indicate your decision by
 * deletingthe provisions above and replace them with the notice and
 * other provisions required by the MPL or the GPL, as the case may be.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under any of the LGPL, the MPL or the GPL.
 */

#ifndef DeprecatedPaintLayerStackingNode_h
#define DeprecatedPaintLayerStackingNode_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBoxModelObject.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

class DeprecatedPaintLayer;
class DeprecatedPaintLayerCompositor;
class ComputedStyle;
class LayoutBoxModelObject;

class CORE_EXPORT DeprecatedPaintLayerStackingNode {
    WTF_MAKE_NONCOPYABLE(DeprecatedPaintLayerStackingNode);
public:
    explicit DeprecatedPaintLayerStackingNode(DeprecatedPaintLayer* layer);
    ~DeprecatedPaintLayerStackingNode();

    int zIndex() const { return layoutObject()->style()->zIndex(); }

    bool isStackingContext() const { return layoutObject()->style()->isStackingContext(); }

    // Update our normal and z-index lists.
    void updateLayerListsIfNeeded();

    bool zOrderListsDirty() const { return m_zOrderListsDirty; }
    void dirtyZOrderLists();
    void updateZOrderLists();
    void clearZOrderLists();
    void dirtyStackingContextZOrderLists();

    bool hasPositiveZOrderList() const { return posZOrderList() && posZOrderList()->size(); }
    bool hasNegativeZOrderList() const { return negZOrderList() && negZOrderList()->size(); }

    bool isTreatedAsStackingContextForPainting() const { return m_isTreatedAsStackingContextForPainting; }
    void updateIsTreatedAsStackingContextForPainting();

    void updateStackingNodesAfterStyleChange(const ComputedStyle* oldStyle);

    DeprecatedPaintLayerStackingNode* ancestorStackingContextNode() const;

    // FIXME: A lot of code depends on this function but shouldn't. We should
    // build our code on top of LayoutBoxModelObject, not DeprecatedPaintLayer.
    DeprecatedPaintLayer* layer() const { return m_layer; }

#if ENABLE(ASSERT)
    bool layerListMutationAllowed() const { return m_layerListMutationAllowed; }
    void setLayerListMutationAllowed(bool flag) { m_layerListMutationAllowed = flag; }
#endif

private:
    friend class DeprecatedPaintLayerStackingNodeIterator;
    friend class DeprecatedPaintLayerStackingNodeReverseIterator;
    friend class LayoutTreeAsText;

    Vector<DeprecatedPaintLayerStackingNode*>* posZOrderList() const
    {
        ASSERT(!m_zOrderListsDirty);
        ASSERT(isStackingContext() || !m_posZOrderList);
        return m_posZOrderList.get();
    }

    Vector<DeprecatedPaintLayerStackingNode*>* negZOrderList() const
    {
        ASSERT(!m_zOrderListsDirty);
        ASSERT(isStackingContext() || !m_negZOrderList);
        return m_negZOrderList.get();
    }

    void rebuildZOrderLists();
    void collectLayers(OwnPtr<Vector<DeprecatedPaintLayerStackingNode*>>& posBuffer, OwnPtr<Vector<DeprecatedPaintLayerStackingNode*>>& negBuffer);

#if ENABLE(ASSERT)
    bool isInStackingParentZOrderLists() const;
    void updateStackingParentForZOrderLists(DeprecatedPaintLayerStackingNode* stackingParent);
    void setStackingParent(DeprecatedPaintLayerStackingNode* stackingParent) { m_stackingParent = stackingParent; }
#endif

    bool shouldBeTreatedAsStackingContextForPainting() const { return layoutObject()->style()->isTreatedAsStackingContextForPainting(); }

    bool isDirtyStackingContext() const { return m_zOrderListsDirty && isStackingContext(); }

    DeprecatedPaintLayerCompositor* compositor() const;
    // We can't return a LayoutBox as LayoutInline can be a stacking context.
    LayoutBoxModelObject* layoutObject() const;

    DeprecatedPaintLayer* m_layer;

    // m_posZOrderList holds a sorted list of all the descendant nodes within
    // that have z-indices of 0 or greater (auto will count as 0).
    // m_negZOrderList holds descendants within our stacking context with
    // negative z-indices.
    OwnPtr<Vector<DeprecatedPaintLayerStackingNode*>> m_posZOrderList;
    OwnPtr<Vector<DeprecatedPaintLayerStackingNode*>> m_negZOrderList;

    unsigned m_zOrderListsDirty : 1;
    unsigned m_isTreatedAsStackingContextForPainting : 1;

#if ENABLE(ASSERT)
    unsigned m_layerListMutationAllowed : 1;
    DeprecatedPaintLayerStackingNode* m_stackingParent;
#endif
};

inline void DeprecatedPaintLayerStackingNode::clearZOrderLists()
{
    ASSERT(!isStackingContext());

#if ENABLE(ASSERT)
    updateStackingParentForZOrderLists(0);
#endif

    m_posZOrderList.clear();
    m_negZOrderList.clear();
}

inline void DeprecatedPaintLayerStackingNode::updateZOrderLists()
{
    if (!m_zOrderListsDirty)
        return;

    if (!isStackingContext()) {
        clearZOrderLists();
        m_zOrderListsDirty = false;
        return;
    }

    rebuildZOrderLists();
}

#if ENABLE(ASSERT)
class LayerListMutationDetector {
public:
    explicit LayerListMutationDetector(DeprecatedPaintLayerStackingNode* stackingNode)
        : m_stackingNode(stackingNode)
        , m_previousMutationAllowedState(stackingNode->layerListMutationAllowed())
    {
        m_stackingNode->setLayerListMutationAllowed(false);
    }

    ~LayerListMutationDetector()
    {
        m_stackingNode->setLayerListMutationAllowed(m_previousMutationAllowedState);
    }

private:
    DeprecatedPaintLayerStackingNode* m_stackingNode;
    bool m_previousMutationAllowedState;
};
#endif

} // namespace blink

#endif // DeprecatedPaintLayerStackingNode_h
