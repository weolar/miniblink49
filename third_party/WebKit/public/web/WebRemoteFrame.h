// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebRemoteFrame_h
#define WebRemoteFrame_h

#include "public/web/WebFrame.h"
#include "public/web/WebSandboxFlags.h"

namespace blink {

enum class WebTreeScopeType;
class WebFrameClient;
class WebRemoteFrameClient;

class WebRemoteFrame : public WebFrame {
public:
    BLINK_EXPORT static WebRemoteFrame* create(WebTreeScopeType, WebRemoteFrameClient*);

    // Functions for the embedder replicate the frame tree between processes.
    // TODO(dcheng): The embedder currently does not replicate local frames in
    // insertion order, so the local child version takes a previous sibling to
    // ensure that it is inserted into the correct location in the list of
    // children.
    virtual WebLocalFrame* createLocalChild(WebTreeScopeType, const WebString& name, WebSandboxFlags, WebFrameClient*, WebFrame* previousSibling) = 0;

    virtual WebRemoteFrame* createRemoteChild(WebTreeScopeType, const WebString& name, WebSandboxFlags, WebRemoteFrameClient*) = 0;

    // Transfer initial drawing parameters from a local frame.
    virtual void initializeFromFrame(WebLocalFrame*) const = 0;

    // Set security origin replicated from another process.
    virtual void setReplicatedOrigin(const WebSecurityOrigin&) const = 0;

    // Set sandbox flags replicated from another process.
    virtual void setReplicatedSandboxFlags(WebSandboxFlags) const = 0;

    // Set frame name replicated from another process.
    virtual void setReplicatedName(const WebString&) const = 0;

    virtual void DispatchLoadEventForFrameOwner() const = 0;

    virtual void didStartLoading() = 0;
    virtual void didStopLoading() = 0;

protected:
    explicit WebRemoteFrame(WebTreeScopeType scope) : WebFrame(scope) { }
};

} // namespace blink

#endif // WebRemoteFrame_h
