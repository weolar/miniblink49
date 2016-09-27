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

#include "config.h"
#include "core/editing/ReplaceNodeWithSpanCommand.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/editing/htmlediting.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLSpanElement.h"
#include "wtf/Assertions.h"

namespace blink {

using namespace HTMLNames;

ReplaceNodeWithSpanCommand::ReplaceNodeWithSpanCommand(PassRefPtrWillBeRawPtr<HTMLElement> element)
    : SimpleEditCommand(element->document())
    , m_elementToReplace(element)
{
    ASSERT(m_elementToReplace);
}

static void swapInNodePreservingAttributesAndChildren(HTMLElement* newElement, HTMLElement& elementToReplace)
{
    ASSERT(elementToReplace.inDocument());
    RefPtrWillBeRawPtr<ContainerNode> parentNode = elementToReplace.parentNode();
    parentNode->insertBefore(newElement, &elementToReplace);

    NodeVector children;
    getChildNodes(elementToReplace, children);
    for (const auto& child : children)
        newElement->appendChild(child);

    // FIXME: Fix this to send the proper MutationRecords when MutationObservers are present.
    newElement->cloneDataFromElement(elementToReplace);

    parentNode->removeChild(&elementToReplace, ASSERT_NO_EXCEPTION);
}

void ReplaceNodeWithSpanCommand::doApply()
{
    if (!m_elementToReplace->inDocument())
        return;
    if (!m_spanElement)
        m_spanElement = toHTMLSpanElement(createHTMLElement(m_elementToReplace->document(), spanTag).get());
    swapInNodePreservingAttributesAndChildren(m_spanElement.get(), *m_elementToReplace);
}

void ReplaceNodeWithSpanCommand::doUnapply()
{
    if (!m_spanElement->inDocument())
        return;
    swapInNodePreservingAttributesAndChildren(m_elementToReplace.get(), *m_spanElement);
}

DEFINE_TRACE(ReplaceNodeWithSpanCommand)
{
    visitor->trace(m_elementToReplace);
    visitor->trace(m_spanElement);
    SimpleEditCommand::trace(visitor);
}

} // namespace blink
