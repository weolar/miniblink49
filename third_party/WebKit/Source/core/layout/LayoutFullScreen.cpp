/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutFullScreen.h"

#include "core/dom/Fullscreen.h"
#include "core/frame/FrameHost.h"
#include "core/frame/Settings.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/page/Page.h"

#include "public/platform/WebScreenInfo.h"

using namespace blink;

class LayoutFullScreenPlaceholder final : public LayoutBlockFlow {
public:
    LayoutFullScreenPlaceholder(LayoutFullScreen* owner)
        : LayoutBlockFlow(nullptr)
        , m_owner(owner)
    {
        setDocumentForAnonymous(&owner->document());
    }
private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectLayoutFullScreenPlaceholder || LayoutBlockFlow::isOfType(type); }
    virtual void willBeDestroyed() override;
    LayoutFullScreen* m_owner;
};

void LayoutFullScreenPlaceholder::willBeDestroyed()
{
    m_owner->setPlaceholder(nullptr);
    LayoutBlockFlow::willBeDestroyed();
}

LayoutFullScreen::LayoutFullScreen()
    : LayoutFlexibleBox(nullptr)
    , m_placeholder(nullptr)
{
    setReplaced(false);
}

LayoutFullScreen* LayoutFullScreen::createAnonymous(Document* document)
{
    LayoutFullScreen* layoutObject = new LayoutFullScreen();
    layoutObject->setDocumentForAnonymous(document);
    return layoutObject;
}

void LayoutFullScreen::willBeDestroyed()
{
    if (m_placeholder) {
        remove();
        if (!m_placeholder->beingDestroyed())
            m_placeholder->destroy();
        ASSERT(!m_placeholder);
    }

    // LayoutObjects are unretained, so notify the document (which holds a pointer to a LayoutFullScreen)
    // if its LayoutFullScreen is destroyed.
    Fullscreen& fullscreen = Fullscreen::from(document());
    if (fullscreen.fullScreenLayoutObject() == this)
        fullscreen.fullScreenLayoutObjectDestroyed();

    LayoutFlexibleBox::willBeDestroyed();
}

void LayoutFullScreen::updateStyle()
{
    RefPtr<ComputedStyle> fullscreenStyle = ComputedStyle::create();

    // Create a stacking context:
    fullscreenStyle->setZIndex(INT_MAX);

    fullscreenStyle->setFontDescription(FontDescription());
    fullscreenStyle->font().update(nullptr);

    fullscreenStyle->setDisplay(FLEX);
    fullscreenStyle->setJustifyContentPosition(ContentPositionCenter);
    fullscreenStyle->setAlignItemsPosition(ItemPositionCenter);
    fullscreenStyle->setFlexDirection(FlowColumn);

    fullscreenStyle->setPosition(FixedPosition);
    fullscreenStyle->setLeft(Length(0, blink::Fixed));
    fullscreenStyle->setTop(Length(0, blink::Fixed));
    IntSize viewportSize = document().page()->frameHost().pinchViewport().size();
    fullscreenStyle->setWidth(Length(viewportSize.width(), blink::Fixed));
    fullscreenStyle->setHeight(Length(viewportSize.height(), blink::Fixed));

    fullscreenStyle->setBackgroundColor(StyleColor(Color::black));

    setStyle(fullscreenStyle);
}

LayoutObject* LayoutFullScreen::wrapLayoutObject(LayoutObject* object, LayoutObject* parent, Document* document)
{
    // FIXME: We should not modify the structure of the layout tree during
    // layout. crbug.com/370459
    DeprecatedDisableModifyLayoutTreeStructureAsserts disabler;

    LayoutFullScreen* fullscreenLayoutObject = LayoutFullScreen::createAnonymous(document);
    fullscreenLayoutObject->updateStyle();
    if (parent && !parent->isChildAllowed(fullscreenLayoutObject, fullscreenLayoutObject->styleRef())) {
        fullscreenLayoutObject->destroy();
        return nullptr;
    }
    if (object) {
        // |object->parent()| can be null if the object is not yet attached
        // to |parent|.
        if (LayoutObject* parent = object->parent()) {
            LayoutBlock* containingBlock = object->containingBlock();
            ASSERT(containingBlock);
            // Since we are moving the |object| to a new parent |fullscreenLayoutObject|,
            // the line box tree underneath our |containingBlock| is not longer valid.
            containingBlock->deleteLineBoxTree();

            parent->addChild(fullscreenLayoutObject, object);
            object->remove();

            // Always just do a full layout to ensure that line boxes get deleted properly.
            // Because objects moved from |parent| to |fullscreenLayoutObject|, we want to
            // make new line boxes instead of leaving the old ones around.
            parent->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::Fullscreen);
            containingBlock->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::Fullscreen);
        }
        fullscreenLayoutObject->addChild(object);
        fullscreenLayoutObject->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::Fullscreen);
    }

    ASSERT(document);
    Fullscreen::from(*document).setFullScreenLayoutObject(fullscreenLayoutObject);
    return fullscreenLayoutObject;
}

void LayoutFullScreen::unwrapLayoutObject()
{
    // FIXME: We should not modify the structure of the layout tree during
    // layout. crbug.com/370459
    DeprecatedDisableModifyLayoutTreeStructureAsserts disabler;

    if (parent()) {
        for (LayoutObject* child = firstChild(); child; child = firstChild()) {
            // We have to clear the override size, because as a flexbox, we
            // may have set one on the child, and we don't want to leave that
            // lying around on the child.
            if (child->isBox())
                toLayoutBox(child)->clearOverrideSize();
            child->remove();
            parent()->addChild(child, this);
            parent()->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::Fullscreen);
        }
    }
    if (placeholder())
        placeholder()->remove();
    remove();
    destroy();
}

void LayoutFullScreen::setPlaceholder(LayoutBlock* placeholder)
{
    m_placeholder = placeholder;
}

void LayoutFullScreen::createPlaceholder(PassRefPtr<ComputedStyle> style, const LayoutRect& frameRect)
{
    if (style->width().isAuto())
        style->setWidth(Length(frameRect.width(), Fixed));
    if (style->height().isAuto())
        style->setHeight(Length(frameRect.height(), Fixed));

    if (!m_placeholder) {
        m_placeholder = new LayoutFullScreenPlaceholder(this);
        m_placeholder->setStyle(style);
        if (parent()) {
            parent()->addChild(m_placeholder, this);
            parent()->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::Fullscreen);
        }
    } else {
        m_placeholder->setStyle(style);
    }
}
