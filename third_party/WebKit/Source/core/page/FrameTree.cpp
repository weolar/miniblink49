/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 * Copyright (C) 2006 Apple Computer, Inc.
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

#include "config.h"
#include "core/page/FrameTree.h"

#include "core/dom/Document.h"
#include "core/frame/FrameClient.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/RemoteFrame.h"
#include "core/frame/RemoteFrameView.h"
#include "core/page/Page.h"
#include "wtf/Vector.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuilder.h"

using std::swap;

namespace blink {

namespace {

const unsigned invalidChildCount = ~0U;

} // namespace

FrameTree::FrameTree(Frame* thisFrame)
    : m_thisFrame(thisFrame)
    , m_scopedChildCount(invalidChildCount)
{
}

FrameTree::~FrameTree()
{
#if !ENABLE(OILPAN)
    // FIXME: Why is this here? Doesn't this parallel what we already do in ~LocalFrame?
    for (Frame* child = firstChild(); child; child = child->tree().nextSibling()) {
        if (child->isLocalFrame())
            toLocalFrame(child)->setView(nullptr);
        else if (child->isRemoteFrame())
            toRemoteFrame(child)->setView(nullptr);
    }
#endif
}

void FrameTree::setName(const AtomicString& name, const AtomicString& fallbackName)
{
    m_name = name;
    if (!parent()) {
        m_uniqueName = name;
        return;
    }
    m_uniqueName = AtomicString(); // Remove our old frame name so it's not considered in uniqueChildName.
    m_uniqueName = parent()->tree().uniqueChildName(name.isEmpty() ? fallbackName : name);
}

Frame* FrameTree::parent() const
{
    if (!m_thisFrame->client())
        return nullptr;
    return m_thisFrame->client()->parent();
}

Frame* FrameTree::top() const
{
    // FIXME: top() should never return null, so here are some hacks to deal
    // with EmptyFrameLoaderClient and cases where the frame is detached
    // already...
    if (!m_thisFrame->client())
        return m_thisFrame;
    Frame* candidate = m_thisFrame->client()->top();
    return candidate ? candidate : m_thisFrame.get();
}

Frame* FrameTree::previousSibling() const
{
    if (!m_thisFrame->client())
        return nullptr;
    return m_thisFrame->client()->previousSibling();
}

Frame* FrameTree::nextSibling() const
{
    if (!m_thisFrame->client())
        return nullptr;
    return m_thisFrame->client()->nextSibling();
}

Frame* FrameTree::firstChild() const
{
    if (!m_thisFrame->client())
        return nullptr;
    return m_thisFrame->client()->firstChild();
}

Frame* FrameTree::lastChild() const
{
    if (!m_thisFrame->client())
        return nullptr;
    return m_thisFrame->client()->lastChild();
}

bool FrameTree::uniqueNameExists(const AtomicString& name) const
{
    for (Frame* frame = top(); frame; frame = frame->tree().traverseNext()) {
        if (frame->tree().uniqueName() == name)
            return true;
    }
    return false;
}

AtomicString FrameTree::uniqueChildName(const AtomicString& requestedName) const
{
    if (!requestedName.isEmpty() && !uniqueNameExists(requestedName) && requestedName != "_blank")
        return requestedName;

    // Create a repeatable name for a child about to be added to us. The name must be
    // unique within the frame tree. The string we generate includes a "path" of names
    // from the root frame down to us. For this path to be unique, each set of siblings must
    // contribute a unique name to the path, which can't collide with any HTML-assigned names.
    // We generate this path component by index in the child list along with an unlikely
    // frame name that can't be set in HTML because it collides with comment syntax.

    const char framePathPrefix[] = "<!--framePath ";
    const int framePathPrefixLength = 14;
    const int framePathSuffixLength = 3;

    // Find the nearest parent that has a frame with a path in it.
    Vector<Frame*, 16> chain;
    Frame* frame;
    for (frame = m_thisFrame; frame; frame = frame->tree().parent()) {
        if (frame->tree().uniqueName().startsWith(framePathPrefix))
            break;
        chain.append(frame);
    }
    StringBuilder name;
    name.append(framePathPrefix);
    if (frame) {
        name.append(frame->tree().uniqueName().string().substring(framePathPrefixLength,
            frame->tree().uniqueName().length() - framePathPrefixLength - framePathSuffixLength));
    }
    for (int i = chain.size() - 1; i >= 0; --i) {
        frame = chain[i];
        name.append('/');
        name.append(frame->tree().uniqueName());
    }

    name.appendLiteral("/<!--frame");
    name.appendNumber(childCount() - 1);
    name.appendLiteral("-->-->");

    return name.toAtomicString();
}

Frame* FrameTree::scopedChild(unsigned index) const
{
    unsigned scopedIndex = 0;
    for (Frame* child = firstChild(); child; child = child->tree().nextSibling()) {
        if (child->client()->inShadowTree())
            continue;
        if (scopedIndex == index)
            return child;
        scopedIndex++;
    }

    return nullptr;
}

Frame* FrameTree::scopedChild(const AtomicString& name) const
{
    for (Frame* child = firstChild(); child; child = child->tree().nextSibling()) {
        if (child->client()->inShadowTree())
            continue;
        if (child->tree().name() == name)
            return child;
    }
    return nullptr;
}

inline unsigned FrameTree::scopedChildCount(TreeScope* scope) const
{
    unsigned scopedCount = 0;
    for (Frame* child = firstChild(); child; child = child->tree().nextSibling()) {
        if (child->client()->inShadowTree())
            continue;
        scopedCount++;
    }

    return scopedCount;
}

unsigned FrameTree::scopedChildCount() const
{
    if (m_scopedChildCount == invalidChildCount) {
        // FIXME: implement a TreeScope for RemoteFrames.
        TreeScope* scope = m_thisFrame->isLocalFrame() ? toLocalFrame(m_thisFrame)->document() : nullptr;
        m_scopedChildCount = scopedChildCount(scope);
    }
    return m_scopedChildCount;
}

void FrameTree::invalidateScopedChildCount()
{
    m_scopedChildCount = invalidChildCount;
}

unsigned FrameTree::childCount() const
{
    unsigned count = 0;
    for (Frame* result = firstChild(); result; result = result->tree().nextSibling())
        ++count;
    return count;
}

Frame* FrameTree::child(const AtomicString& name) const
{
    for (Frame* child = firstChild(); child; child = child->tree().nextSibling())
        if (child->tree().name() == name)
            return child;
    return nullptr;
}

Frame* FrameTree::find(const AtomicString& name) const
{
    if (name == "_self" || name == "_current" || name.isEmpty())
        return m_thisFrame;

    if (name == "_top")
        return top();

    if (name == "_parent")
        return parent() ? parent() : m_thisFrame.get();

    // Since "_blank" should never be any frame's name, the following just amounts to an optimization.
    if (name == "_blank")
        return nullptr;

    // Search subtree starting with this frame first.
    for (Frame* frame = m_thisFrame; frame; frame = frame->tree().traverseNext(m_thisFrame))
        if (frame->tree().name() == name)
            return frame;

    // Search the entire tree for this page next.
    Page* page = m_thisFrame->page();

    // The frame could have been detached from the page, so check it.
    if (!page)
        return nullptr;

    for (Frame* frame = page->mainFrame(); frame; frame = frame->tree().traverseNext())
        if (frame->tree().name() == name)
            return frame;

    // Search the entire tree of each of the other pages in this namespace.
    // FIXME: Is random order OK?
    const HashSet<Page*>& pages = Page::ordinaryPages();
    for (const Page* otherPage : pages) {
        if (otherPage != page) {
            for (Frame* frame = otherPage->mainFrame(); frame; frame = frame->tree().traverseNext()) {
                if (frame->tree().name() == name)
                    return frame;
            }
        }
    }

    return nullptr;
}

bool FrameTree::isDescendantOf(const Frame* ancestor) const
{
    if (!ancestor)
        return false;

    if (m_thisFrame->page() != ancestor->page())
        return false;

    for (Frame* frame = m_thisFrame; frame; frame = frame->tree().parent())
        if (frame == ancestor)
            return true;
    return false;
}

Frame* FrameTree::traverseNext(const Frame* stayWithin) const
{
    Frame* child = firstChild();
    if (child) {
        ASSERT(!stayWithin || child->tree().isDescendantOf(stayWithin));
        return child;
    }

    if (m_thisFrame == stayWithin)
        return nullptr;

    Frame* sibling = nextSibling();
    if (sibling) {
        ASSERT(!stayWithin || sibling->tree().isDescendantOf(stayWithin));
        return sibling;
    }

    Frame* frame = m_thisFrame;
    while (!sibling && (!stayWithin || frame->tree().parent() != stayWithin)) {
        frame = frame->tree().parent();
        if (!frame)
            return nullptr;
        sibling = frame->tree().nextSibling();
    }

    if (frame) {
        ASSERT(!stayWithin || !sibling || sibling->tree().isDescendantOf(stayWithin));
        return sibling;
    }

    return nullptr;
}

Frame* FrameTree::traverseNextWithWrap(bool wrap) const
{
    if (Frame* result = traverseNext())
        return result;

    if (wrap)
        return m_thisFrame->page()->mainFrame();

    return nullptr;
}

Frame* FrameTree::traversePreviousWithWrap(bool wrap) const
{
    // FIXME: besides the wrap feature, this is just the traversePreviousNode algorithm

    if (Frame* prevSibling = previousSibling())
        return prevSibling->tree().deepLastChild();
    if (Frame* parentFrame = parent())
        return parentFrame;

    // no siblings, no parent, self==top
    if (wrap)
        return deepLastChild();

    // top view is always the last one in this ordering, so prev is nil without wrap
    return nullptr;
}

Frame* FrameTree::deepLastChild() const
{
    Frame* result = m_thisFrame;
    for (Frame* last = lastChild(); last; last = last->tree().lastChild())
        result = last;

    return result;
}

DEFINE_TRACE(FrameTree)
{
    visitor->trace(m_thisFrame);
}

} // namespace blink

#ifndef NDEBUG

static void printIndent(int indent)
{
    for (int i = 0; i < indent; ++i)
        printf("    ");
}

static void printFrames(const blink::Frame* frame, const blink::Frame* targetFrame, int indent)
{
    if (frame == targetFrame) {
        printf("--> ");
        printIndent(indent - 1);
    } else
        printIndent(indent);

    blink::FrameView* view = frame->isLocalFrame() ? toLocalFrame(frame)->view() : 0;
    printf("Frame %p %dx%d\n", frame, view ? view->width() : 0, view ? view->height() : 0);
    printIndent(indent);
    printf("  owner=%p\n", frame->owner());
    printIndent(indent);
    printf("  frameView=%p\n", view);
    printIndent(indent);
    printf("  document=%p\n", frame->isLocalFrame() ? toLocalFrame(frame)->document() : 0);
    printIndent(indent);
    printf("  uri=%s\n\n", frame->isLocalFrame() ? toLocalFrame(frame)->document()->url().string().utf8().data() : 0);

    for (blink::Frame* child = frame->tree().firstChild(); child; child = child->tree().nextSibling())
        printFrames(child, targetFrame, indent + 1);
}

void showFrameTree(const blink::Frame* frame)
{
    if (!frame) {
        printf("Null input frame\n");
        return;
    }

    printFrames(frame->tree().top(), frame, 0);
}

#endif
