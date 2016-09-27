/*
 * Copyright (C) 2005, 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/editing/WrapContentsInDummySpanCommand.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/editing/ApplyStyleCommand.h"
#include "core/html/HTMLSpanElement.h"

namespace blink {

WrapContentsInDummySpanCommand::WrapContentsInDummySpanCommand(PassRefPtrWillBeRawPtr<Element> element)
    : SimpleEditCommand(element->document())
    , m_element(element)
{
    ASSERT(m_element);
}

void WrapContentsInDummySpanCommand::executeApply()
{
    NodeVector children;
    getChildNodes(*m_element, children);

    for (auto& child : children)
        m_dummySpan->appendChild(child.release(), IGNORE_EXCEPTION);

    m_element->appendChild(m_dummySpan.get(), IGNORE_EXCEPTION);
}

void WrapContentsInDummySpanCommand::doApply()
{
    m_dummySpan = createStyleSpanElement(document());

    executeApply();
}

void WrapContentsInDummySpanCommand::doUnapply()
{
    ASSERT(m_element);

    if (!m_dummySpan || !m_element->hasEditableStyle())
        return;

    NodeVector children;
    getChildNodes(*m_dummySpan, children);

    for (auto& child : children)
        m_element->appendChild(child.release(), IGNORE_EXCEPTION);

    m_dummySpan->remove(IGNORE_EXCEPTION);
}

void WrapContentsInDummySpanCommand::doReapply()
{
    ASSERT(m_element);

    if (!m_dummySpan || !m_element->hasEditableStyle())
        return;

    executeApply();
}

DEFINE_TRACE(WrapContentsInDummySpanCommand)
{
    visitor->trace(m_element);
    visitor->trace(m_dummySpan);
    SimpleEditCommand::trace(visitor);
}

} // namespace blink
