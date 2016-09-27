/*
 * Copyright (C) 2006, 2007, 2009 Apple Inc. All rights reserved.
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
 *
 */

#ifndef HTMLFrameOwnerElement_h
#define HTMLFrameOwnerElement_h

#include "core/CoreExport.h"
#include "core/dom/Document.h"
#include "core/frame/FrameOwner.h"
#include "core/html/HTMLElement.h"
#include "platform/heap/Handle.h"
#include "platform/scroll/ScrollTypes.h"
#include "wtf/HashCountedSet.h"

namespace blink {

class LocalDOMWindow;
class ExceptionState;
class Frame;
class LayoutPart;
class Widget;

class CORE_EXPORT HTMLFrameOwnerElement : public HTMLElement, public FrameOwner {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(HTMLFrameOwnerElement);
public:
    ~HTMLFrameOwnerElement() override;

    Frame* contentFrame() const { return m_contentFrame; }
    DOMWindow* contentWindow() const;
    Document* contentDocument() const;

    void setContentFrame(Frame&);
    void clearContentFrame();

    virtual void disconnectContentFrame();

    // Most subclasses use LayoutPart (either LayoutEmbeddedObject or LayoutIFrame)
    // except for HTMLObjectElement and HTMLEmbedElement which may return any
    // LayoutObject when using fallback content.
    LayoutPart* layoutPart() const;

    Document* getSVGDocument(ExceptionState&) const;

    virtual ScrollbarMode scrollingMode() const { return ScrollbarAuto; }

    virtual bool loadedNonEmptyDocument() const { return false; }
    virtual void didLoadNonEmptyDocument() { }

    void setWidget(PassRefPtrWillBeRawPtr<Widget>);
    Widget* ownedWidget() const;

    class UpdateSuspendScope {
    public:
        UpdateSuspendScope();
        ~UpdateSuspendScope();

    private:
        void performDeferredWidgetTreeOperations();
    };

    // FrameOwner overrides:
    bool isLocal() const override { return true; }
    void dispatchLoad() override;
    SandboxFlags sandboxFlags() const override { return m_sandboxFlags; }
    void renderFallbackContent() override { }

    DECLARE_VIRTUAL_TRACE();

protected:
    HTMLFrameOwnerElement(const QualifiedName& tagName, Document&);
    void setSandboxFlags(SandboxFlags);

    bool loadOrRedirectSubframe(const KURL&, const AtomicString& frameName, bool lockBackForwardList);

private:
    bool isKeyboardFocusable() const override;
    bool isFrameOwnerElement() const final { return true; }

    RawPtrWillBeMember<Frame> m_contentFrame;
    RefPtrWillBeMember<Widget> m_widget;
    SandboxFlags m_sandboxFlags;
};

DEFINE_ELEMENT_TYPE_CASTS(HTMLFrameOwnerElement, isFrameOwnerElement());

class SubframeLoadingDisabler {
    STACK_ALLOCATED();
public:
    explicit SubframeLoadingDisabler(Node& root)
        : m_root(root)
    {
        disabledSubtreeRoots().add(m_root);
    }

    ~SubframeLoadingDisabler()
    {
        disabledSubtreeRoots().remove(m_root);
    }

    static bool canLoadFrame(HTMLFrameOwnerElement& owner)
    {
        if (owner.document().unloadStarted())
            return false;
        for (Node* node = &owner; node; node = node->parentOrShadowHostNode()) {
            if (disabledSubtreeRoots().contains(node))
                return false;
        }
        return true;
    }

private:
    static WillBeHeapHashCountedSet<RawPtrWillBeMember<Node>>& disabledSubtreeRoots();

    RawPtrWillBeMember<Node> m_root;
};

DEFINE_TYPE_CASTS(HTMLFrameOwnerElement, FrameOwner, owner, owner->isLocal(), owner.isLocal());

} // namespace blink

#endif // HTMLFrameOwnerElement_h
