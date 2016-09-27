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

#ifndef SmartClip_h
#define SmartClip_h

#include "core/CoreExport.h"
#include "core/dom/Node.h"
#include "core/frame/LocalFrame.h"
#include "platform/heap/Handle.h"

namespace blink {

class CORE_EXPORT SmartClipData {
public:
    SmartClipData()
        : m_isEmpty(true)
    {
    }

    SmartClipData(Node* node, IntRect rect, String string)
        : m_isEmpty(!node)
        , m_rectInViewport(rect)
        , m_string(string)
    {
    }

    IntRect rectInViewport() const;
    const String& clipData() const;

private:
    bool m_isEmpty;
    IntRect m_rectInViewport;
    String m_string;
};

// SmartClip implements support for the copy operation
// with an S-Pen on Samsung devices. The behavior of this
// class is quirky and poorly tested. It's approximately
// trying to do a poor-mans implementation of columnar
// selection followed by a copy operation.
class CORE_EXPORT SmartClip {
    STACK_ALLOCATED();
public:
    explicit SmartClip(PassRefPtrWillBeRawPtr<LocalFrame>);

    SmartClipData dataForRect(const IntRect&);

private:
    float pageScaleFactor();

    Node* minNodeContainsNodes(Node* minNode, Node* newNode);
    Node* findBestOverlappingNode(Node*, const IntRect& cropRectInViewport);
    bool shouldSkipBackgroundImage(Node*);
    void collectOverlappingChildNodes(Node* parentNode, const IntRect& cropRectInViewport, WillBeHeapVector<RawPtrWillBeMember<Node>>& overlappingNodeInfoTable);
    String extractTextFromNode(Node*);

    RefPtrWillBeMember<LocalFrame> m_frame;
};

} // namespace blink

#endif // SmartClip_h
