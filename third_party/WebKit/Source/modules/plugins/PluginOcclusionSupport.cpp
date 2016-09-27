/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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
#include "modules/plugins/PluginOcclusionSupport.h"

#include "core/HTMLNames.h"
#include "core/dom/Element.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutObject.h"
#include "platform/Widget.h"
#include "wtf/HashSet.h"

// This file provides a utility function to support rendering certain elements above plugins.

namespace blink {

static void getObjectStack(const LayoutObject* ro, Vector<const LayoutObject*>* roStack)
{
    roStack->clear();
    while (ro) {
        roStack->append(ro);
        ro = ro->parent();
    }
}

// Returns true if stack1 is at or above stack2
static bool iframeIsAbovePlugin(const Vector<const LayoutObject*>& iframeZstack, const Vector<const LayoutObject*>& pluginZstack)
{
    for (size_t i = 0; i < iframeZstack.size() && i < pluginZstack.size(); i++) {
        // The root is at the end of these stacks. We want to iterate
        // root-downwards so we index backwards from the end.
        const LayoutObject* ro1 = iframeZstack[iframeZstack.size() - 1 - i];
        const LayoutObject* ro2 = pluginZstack[pluginZstack.size() - 1 - i];

        if (ro1 != ro2) {
            // When we find nodes in the stack that are not the same, then
            // we've found the nodes just below the lowest comment ancestor.
            // Determine which should be on top.

            // See if z-index determines an order.
            if (ro1->style() && ro2->style()) {
                int z1 = ro1->style()->zIndex();
                int z2 = ro2->style()->zIndex();
                if (z1 > z2)
                    return true;
                if (z1 < z2)
                    return false;
            }

            // If the plugin does not have an explicit z-index it stacks behind the iframe.
            // This is for maintaining compatibility with IE.
            if (ro2->style()->position() == StaticPosition) {
                // The 0'th elements of these LayoutObject arrays represent the plugin node and
                // the iframe.
                const LayoutObject* pluginLayoutObject = pluginZstack[0];
                const LayoutObject* iframeLayoutObject = iframeZstack[0];

                if (pluginLayoutObject->style() && iframeLayoutObject->style()) {
                    if (pluginLayoutObject->style()->zIndex() > iframeLayoutObject->style()->zIndex())
                        return false;
                }
                return true;
            }

            // Inspect the document order. Later order means higher stacking.
            const LayoutObject* parent = ro1->parent();
            if (!parent)
                return false;
            ASSERT(parent == ro2->parent());

            for (const LayoutObject* ro = parent->slowFirstChild(); ro; ro = ro->nextSibling()) {
                if (ro == ro1)
                    return false;
                if (ro == ro2)
                    return true;
            }
            ASSERT(false); // We should have seen ro1 and ro2 by now.
            return false;
        }
    }
    return true;
}

static bool intersectsRect(const LayoutObject* renderer, const IntRect& rect)
{
    return renderer->absoluteBoundingBoxRectIgnoringTransforms().intersects(rect)
        && (!renderer->style() || renderer->style()->visibility() == VISIBLE);
}

static void addToOcclusions(const LayoutBox* renderer, Vector<IntRect>& occlusions)
{
    occlusions.append(IntRect(
        roundedIntPoint(renderer->localToAbsolute()),
        flooredIntSize(renderer->size())));
}

static void addTreeToOcclusions(const LayoutObject* renderer, const IntRect& frameRect, Vector<IntRect>& occlusions)
{
    if (!renderer)
        return;
    if (renderer->isBox() && intersectsRect(renderer, frameRect))
        addToOcclusions(toLayoutBox(renderer), occlusions);
    for (LayoutObject* child = renderer->slowFirstChild(); child; child = child->nextSibling())
        addTreeToOcclusions(child, frameRect, occlusions);
}

static const Element* topLayerAncestor(const Element* element)
{
    while (element && !element->isInTopLayer())
        element = element->parentOrShadowHostElement();
    return element;
}

// Return a set of rectangles that should not be overdrawn by the
// plugin ("cutouts"). This helps implement the "iframe shim"
// technique of overlaying a windowed plugin with content from the
// page. In a nutshell, iframe elements should occlude plugins when
// they occur higher in the stacking order.
void getPluginOcclusions(Element* element, Widget* parentWidget, const IntRect& frameRect, Vector<IntRect>& occlusions)
{
    LayoutObject* pluginNode = element->layoutObject();
    ASSERT(pluginNode);
    if (!pluginNode->style())
        return;
    Vector<const LayoutObject*> pluginZstack;
    Vector<const LayoutObject*> iframeZstack;
    getObjectStack(pluginNode, &pluginZstack);

    if (!parentWidget->isFrameView())
        return;

    FrameView* parentFrameView = toFrameView(parentWidget);

    // Occlusions by iframes.
    const FrameView::ChildrenWidgetSet* children = parentFrameView->children();
    for (FrameView::ChildrenWidgetSet::const_iterator it = children->begin(); it != children->end(); ++it) {
        // We only care about FrameView's because iframes show up as FrameViews.
        if (!(*it)->isFrameView())
            continue;

        const FrameView* frameView = toFrameView(it->get());
        // Check to make sure we can get both the element and the LayoutObject
        // for this FrameView, if we can't just move on to the next object.
        // FIXME: Plugin occlusion by remote frames is probably broken.
        HTMLElement* element = frameView->frame().deprecatedLocalOwner();
        if (!element || !element->layoutObject())
            continue;

        LayoutObject* iframeRenderer = element->layoutObject();

        if (isHTMLIFrameElement(*element) && intersectsRect(iframeRenderer, frameRect)) {
            getObjectStack(iframeRenderer, &iframeZstack);
            if (iframeIsAbovePlugin(iframeZstack, pluginZstack))
                addToOcclusions(toLayoutBox(iframeRenderer), occlusions);
        }
    }

    // Occlusions by top layer elements.
    // FIXME: There's no handling yet for the interaction between top layer and
    // iframes. For example, a plugin in the top layer will be occluded by an
    // iframe. And a plugin inside an iframe in the top layer won't be respected
    // as being in the top layer.
    const Element* ancestor = topLayerAncestor(element);
    Document* document = parentFrameView->frame().document();
    const WillBeHeapVector<RefPtrWillBeMember<Element>>& elements = document->topLayerElements();
    size_t start = ancestor ? elements.find(ancestor) + 1 : 0;
    for (size_t i = start; i < elements.size(); ++i)
        addTreeToOcclusions(elements[i]->layoutObject(), frameRect, occlusions);
}

} // namespace blink
