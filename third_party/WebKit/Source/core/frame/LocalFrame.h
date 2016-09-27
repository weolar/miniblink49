/*
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 *                     1999-2001 Lars Knoll <knoll@kde.org>
 *                     1999-2001 Antti Koivisto <koivisto@kde.org>
 *                     2000-2001 Simon Hausmann <hausmann@kde.org>
 *                     2000-2001 Dirk Mueller <mueller@kde.org>
 *                     2000 Stefan Schimanski <1Stein@gmx.de>
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
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
 */

#ifndef LocalFrame_h
#define LocalFrame_h

#include "core/CoreExport.h"
#include "core/dom/WeakIdentifierMap.h"
#include "core/frame/Frame.h"
#include "core/frame/LocalFrameLifecycleNotifier.h"
#include "core/frame/LocalFrameLifecycleObserver.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/NavigationScheduler.h"
#include "core/page/FrameTree.h"
#include "platform/Supplementable.h"
#include "platform/graphics/ImageOrientation.h"
#include "platform/graphics/paint/DisplayItem.h"
#include "platform/heap/Handle.h"
#include "platform/scroll/ScrollTypes.h"
#include "wtf/HashSet.h"

namespace blink {

    class Color;
    class Document;
    class DragImage;
    class Editor;
    class Element;
    class EventHandler;
    class FloatSize;
    class FrameConsole;
    class FrameSelection;
    class FrameView;
    class HTMLPlugInElement;
    class InputMethodController;
    class IntPoint;
    class IntSize;
    class InstrumentingAgents;
    class LocalDOMWindow;
    class Node;
    class Range;
    class LayoutView;
    class TreeScope;
    class ScriptController;
    class SpellChecker;
    class TreeScope;
    class VisiblePosition;

    class CORE_EXPORT LocalFrame : public Frame, public LocalFrameLifecycleNotifier, public WillBeHeapSupplementable<LocalFrame> {
        WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(LocalFrame);
    public:
        static PassRefPtrWillBeRawPtr<LocalFrame> create(FrameLoaderClient*, FrameHost*, FrameOwner*);

        void init();
        void setView(PassRefPtrWillBeRawPtr<FrameView>);
        void createView(const IntSize&, const Color&, bool,
            ScrollbarMode = ScrollbarAuto, bool horizontalLock = false,
            ScrollbarMode = ScrollbarAuto, bool verticalLock = false);

        // Frame overrides:
        virtual ~LocalFrame();
        DECLARE_VIRTUAL_TRACE();
        bool isLocalFrame() const override { return true; }
        DOMWindow* domWindow() const override;
        WindowProxy* windowProxy(DOMWrapperWorld&) override;
        void navigate(Document& originDocument, const KURL&, bool lockBackForwardList, UserGestureStatus) override;
        void navigate(const FrameLoadRequest&) override;
        void reload(FrameLoadType, ClientRedirectPolicy) override;
        void detach(FrameDetachType) override;
        void disconnectOwnerElement() override;
        bool shouldClose() override;
        SecurityContext* securityContext() const override;
        void printNavigationErrorMessage(const Frame&, const char* reason) override;
        bool prepareForCommit() override;

        void willDetachFrameHost();

        LocalDOMWindow* localDOMWindow() const;
        void setDOMWindow(PassRefPtrWillBeRawPtr<LocalDOMWindow>);
        FrameView* view() const;
        Document* document() const;
        void setPagePopupOwner(Element&);
        Element* pagePopupOwner() const { return m_pagePopupOwner.get(); }

        LayoutView* contentLayoutObject() const; // Root of the layout tree for the document contained in this frame.

        Editor& editor() const;
        EventHandler& eventHandler() const;
        FrameLoader& loader() const;
        NavigationScheduler& navigationScheduler() const;
        FrameSelection& selection() const;
        InputMethodController& inputMethodController() const;
        ScriptController& script() const;
        SpellChecker& spellChecker() const;
        FrameConsole& console() const;

        void didChangeVisibilityState();

        // This method is used to get the highest level LocalFrame in this
        // frame's in-process subtree.
        // FIXME: This is a temporary hack to support RemoteFrames, and callers
        // should be updated to avoid storing things on the main frame.
        LocalFrame* localFrameRoot();

        InstrumentingAgents* instrumentingAgents() const { return m_instrumentingAgents.get(); }

    // ======== All public functions below this point are candidates to move out of LocalFrame into another class. ========

        // See GraphicsLayerClient.h for accepted flags.
        String layerTreeAsText(unsigned flags = 0) const;

        void setPrinting(bool printing, const FloatSize& pageSize, const FloatSize& originalPageSize, float maximumShrinkRatio);
        bool shouldUsePrintingLayout() const;
        FloatSize resizePageRectsKeepingRatio(const FloatSize& originalSize, const FloatSize& expectedSize);

        bool inViewSourceMode() const;
        void setInViewSourceMode(bool = true);

        void setPageZoomFactor(float factor);
        float pageZoomFactor() const { return m_pageZoomFactor; }
        void setTextZoomFactor(float factor);
        float textZoomFactor() const { return m_textZoomFactor; }
        void setPageAndTextZoomFactors(float pageZoomFactor, float textZoomFactor);

        void deviceScaleFactorChanged();
        double devicePixelRatio() const;

        PassOwnPtr<DragImage> nodeImage(Node&);
        PassOwnPtr<DragImage> dragImageForSelection();

        String selectedText() const;
        String selectedTextForClipboard() const;

        VisiblePosition visiblePositionForPoint(const IntPoint& framePoint);
        Document* documentAtPoint(const IntPoint& windowPoint);
        PassRefPtrWillBeRawPtr<Range> rangeForPoint(const IntPoint& framePoint);

        bool isURLAllowed(const KURL&) const;
        bool shouldReuseDefaultView(const KURL&) const;
        void removeSpellingMarkersUnderWords(const Vector<String>& words);

        // FIXME: once scroll customization is enabled everywhere
        // (crbug.com/416862), this should take a ScrollState object.
        ScrollResult applyScrollDelta(const FloatSize& delta, bool isScrollBegin);
        bool shouldScrollTopControls(const FloatSize& delta) const;

#if ENABLE(OILPAN)
        void registerPluginElement(HTMLPlugInElement*);
        void unregisterPluginElement(HTMLPlugInElement*);
        void clearWeakMembers(Visitor*);
#endif
        DisplayItemClient displayItemClient() const { return toDisplayItemClient(this); }
        String debugName() const { return "LocalFrame"; }

    // ========

    private:
        LocalFrame(FrameLoaderClient*, FrameHost*, FrameOwner*);

        // Internal Frame helper overrides:
        WindowProxyManager* windowProxyManager() const override;

        String localLayerTreeAsText(unsigned flags) const;

        // Paints the area for the given rect into a DragImage, with the given displayItemClient id attached.
        // The rect is in the coordinate space of the frame.
        PassOwnPtr<DragImage> paintIntoDragImage(const DisplayItemClientWrapper&, DisplayItem::Type, RespectImageOrientationEnum shouldRespectImageOrientation, IntRect paintingRect);

        mutable FrameLoader m_loader;
        mutable NavigationScheduler m_navigationScheduler;

        RefPtrWillBeMember<FrameView> m_view;
        RefPtrWillBeMember<LocalDOMWindow> m_domWindow;
        // Usually 0. Non-null if this is the top frame of PagePopup.
        RefPtrWillBeMember<Element> m_pagePopupOwner;

        const OwnPtrWillBeMember<ScriptController> m_script;
        const OwnPtrWillBeMember<Editor> m_editor;
        const OwnPtrWillBeMember<SpellChecker> m_spellChecker;
        const OwnPtrWillBeMember<FrameSelection> m_selection;
        const OwnPtrWillBeMember<EventHandler> m_eventHandler;
        const OwnPtrWillBeMember<FrameConsole> m_console;
        const OwnPtrWillBeMember<InputMethodController> m_inputMethodController;

#if ENABLE(OILPAN)
        // Oilpan: in order to reliably finalize plugin elements with
        // renderer-less plugins, the frame keeps track of them. When
        // the frame is detached and disposed, these will be disposed
        // of in the process. This is needed as the plugin element
        // might not itself be attached to a DOM tree and be
        // explicitly detached&disposed of.
        //
        // A weak reference is all wanted; the plugin element must
        // otherwise be referenced and kept alive. So as to be able
        // to process the set of weak references during the LocalFrame's
        // weak callback, the set itself is not on the heap and the
        // references are bare pointers (rather than WeakMembers.)
        // See LocalFrame::clearWeakMembers().
        HashSet<HTMLPlugInElement*> m_pluginElements;
#endif

        float m_pageZoomFactor;
        float m_textZoomFactor;

        bool m_inViewSourceMode;

        RefPtrWillBeMember<InstrumentingAgents> m_instrumentingAgents;
    };

    inline void LocalFrame::init()
    {
        m_loader.init();
    }

    inline LocalDOMWindow* LocalFrame::localDOMWindow() const
    {
        return m_domWindow.get();
    }

    inline FrameLoader& LocalFrame::loader() const
    {
        return m_loader;
    }

    inline NavigationScheduler& LocalFrame::navigationScheduler() const
    {
        return m_navigationScheduler;
    }

    inline FrameView* LocalFrame::view() const
    {
        return m_view.get();
    }

    inline ScriptController& LocalFrame::script() const
    {
        return *m_script;
    }

    inline FrameSelection& LocalFrame::selection() const
    {
        return *m_selection;
    }

    inline Editor& LocalFrame::editor() const
    {
        return *m_editor;
    }

    inline SpellChecker& LocalFrame::spellChecker() const
    {
        return *m_spellChecker;
    }

    inline FrameConsole& LocalFrame::console() const
    {
        return *m_console;
    }

    inline InputMethodController& LocalFrame::inputMethodController() const
    {
        return *m_inputMethodController;
    }

    inline bool LocalFrame::inViewSourceMode() const
    {
        return m_inViewSourceMode;
    }

    inline void LocalFrame::setInViewSourceMode(bool mode)
    {
        m_inViewSourceMode = mode;
    }

    inline EventHandler& LocalFrame::eventHandler() const
    {
        ASSERT(m_eventHandler);
        return *m_eventHandler;
    }

    DEFINE_TYPE_CASTS(LocalFrame, Frame, localFrame, localFrame->isLocalFrame(), localFrame.isLocalFrame());

    DECLARE_WEAK_IDENTIFIER_MAP(LocalFrame);
} // namespace blink

// During refactoring, there are some places where we need to do type conversions that
// will not be needed once all instances of LocalFrame and RemoteFrame are sorted out.
// At that time this #define will be removed and all the uses of it will need to be corrected.
#define toLocalFrameTemporary toLocalFrame

#endif // LocalFrame_h
