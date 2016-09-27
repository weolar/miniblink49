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

#ifndef Frame_h
#define Frame_h

#include "core/CoreExport.h"
#include "core/frame/FrameTypes.h"
#include "core/loader/FrameLoaderTypes.h"
#include "core/page/FrameTree.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"

namespace blink {

class ChromeClient;
class DOMWindow;
class DOMWrapperWorld;
class Document;
class FrameClient;
class FrameHost;
class FrameOwner;
class HTMLFrameOwnerElement;
class LayoutPart;
class KURL;
class Page;
class SecurityContext;
class Settings;
class WindowProxy;
class WindowProxyManager;
struct FrameLoadRequest;

enum class FrameDetachType { Remove, Swap };

// Status of user gesture.
enum class UserGestureStatus { Active, None };

class CORE_EXPORT Frame : public RefCountedWillBeGarbageCollectedFinalized<Frame> {
public:
    virtual ~Frame();

    DECLARE_VIRTUAL_TRACE();

    virtual bool isLocalFrame() const { return false; }
    virtual bool isRemoteFrame() const { return false; }

    virtual DOMWindow* domWindow() const = 0;
    virtual WindowProxy* windowProxy(DOMWrapperWorld&) = 0;

    virtual void navigate(Document& originDocument, const KURL&, bool lockBackForwardList, UserGestureStatus) = 0;
    // This version of Frame::navigate assumes the resulting navigation is not
    // to be started on a timer. Use the method above in such cases.
    virtual void navigate(const FrameLoadRequest&) = 0;
    virtual void reload(FrameLoadType, ClientRedirectPolicy) = 0;

    virtual void detach(FrameDetachType);
    void detachChildren();
    virtual void disconnectOwnerElement();
    virtual bool shouldClose() = 0;

    FrameClient* client() const;

    // NOTE: Page is moving out of Blink up into the browser process as
    // part of the site-isolation (out of process iframes) work.
    // FrameHost should be used instead where possible.
    Page* page() const;
    FrameHost* host() const; // Null when the frame is detached.

    bool isMainFrame() const;
    bool isLocalRoot() const;

    FrameOwner* owner() const;
    void setOwner(FrameOwner* owner) { m_owner = owner; }
    HTMLFrameOwnerElement* deprecatedLocalOwner() const;

    FrameTree& tree() const;
    ChromeClient& chromeClient() const;

    virtual SecurityContext* securityContext() const = 0;

    Frame* findFrameForNavigation(const AtomicString& name, Frame& activeFrame);
    Frame* findUnsafeParentScrollPropagationBoundary();

    // This prepares the Frame for the next commit. It will detach children,
    // dispatch unload events, abort XHR requests and detach the documentm_loader
    virtual bool prepareForCommit() = 0;
    void finishSwapFrom(Frame*);

    bool canNavigate(const Frame&);
    virtual void printNavigationErrorMessage(const Frame&, const char* reason) = 0;

    LayoutPart* ownerLayoutObject() const; // LayoutObject for the element that contains this frame.

    int64_t frameID() const { return m_frameID; }

    Settings* settings() const; // can be null

    // isLoading() is true when the embedder should think a load is in progress.
    // In the case of LocalFrames, it means that the frame has sent a didStartLoading()
    // callback, but not the matching didStopLoading(). Inside blink, you probably
    // want Document::loadEventFinished() instead.
    void setIsLoading(bool isLoading) { m_isLoading = isLoading; }
    bool isLoading() const { return m_isLoading; }

protected:
    Frame(FrameClient*, FrameHost*, FrameOwner*);

    virtual WindowProxyManager* windowProxyManager() const = 0;

    mutable FrameTree m_treeNode;

    RawPtrWillBeMember<FrameHost> m_host;
    RawPtrWillBeMember<FrameOwner> m_owner;

private:
    FrameClient* m_client;
    // Needed to identify Frame Timing requests.
    int64_t m_frameID;
    bool m_isLoading;
};

inline FrameClient* Frame::client() const
{
    return m_client;
}

inline FrameOwner* Frame::owner() const
{
    return m_owner;
}

inline FrameTree& Frame::tree() const
{
    return m_treeNode;
}

// Allow equality comparisons of Frames by reference or pointer, interchangeably.
DEFINE_COMPARISON_OPERATORS_WITH_REFERENCES_REFCOUNTED(Frame)

} // namespace blink

#endif // Frame_h
