/*
 * Copyright (c) 2009 Google Inc. All rights reserved.
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

#ifndef ReplaceNodeWithSpanCommand_h
#define ReplaceNodeWithSpanCommand_h

#include "core/editing/CompositeEditCommand.h"

namespace blink {

class HTMLElement;
class HTMLSpanElement;

// More accurately, this is ReplaceElementWithSpanPreservingChildrenAndAttributesCommand
class ReplaceNodeWithSpanCommand final : public SimpleEditCommand {
public:
    static PassRefPtrWillBeRawPtr<ReplaceNodeWithSpanCommand> create(PassRefPtrWillBeRawPtr<HTMLElement> element)
    {
        return adoptRefWillBeNoop(new ReplaceNodeWithSpanCommand(element));
    }

    HTMLSpanElement* spanElement() { return m_spanElement.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit ReplaceNodeWithSpanCommand(PassRefPtrWillBeRawPtr<HTMLElement>);

    void doApply() override;
    void doUnapply() override;

    RefPtrWillBeMember<HTMLElement> m_elementToReplace;
    RefPtrWillBeMember<HTMLSpanElement> m_spanElement;
};

} // namespace blink

#endif // ReplaceNodeWithSpanCommand
