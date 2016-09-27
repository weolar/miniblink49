/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/webdatabase/SQLResultSetRowList.h"

#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ToV8.h"
#include "bindings/modules/v8/ToV8ForModules.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

unsigned SQLResultSetRowList::length() const
{
    if (m_result.size() == 0)
        return 0;

    ASSERT(m_result.size() % m_columns.size() == 0);

    return m_result.size() / m_columns.size();
}

ScriptValue SQLResultSetRowList::item(ScriptState* scriptState, unsigned index, ExceptionState& exceptionState)
{
    if (index >= length()) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound<unsigned>("index", index, length()));
        return ScriptValue();
    }

    unsigned numColumns = m_columns.size();
    unsigned valuesIndex = index * numColumns;

    Vector<std::pair<String, SQLValue>> dataArray;
    for (unsigned i = 0; i < numColumns; ++i)
        dataArray.append(std::make_pair(m_columns[i], m_result[valuesIndex + i]));

    return ScriptValue::from(scriptState, dataArray);
}

} // namespace blink
