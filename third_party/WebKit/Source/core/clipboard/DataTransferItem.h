/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef DataTransferItem_h
#define DataTransferItem_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"

namespace blink {

class Blob;
class DataObjectItem;
class DataTransfer;
class StringCallback;
class ExecutionContext;

class CORE_EXPORT DataTransferItem final : public GarbageCollected<DataTransferItem>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
    WTF_MAKE_NONCOPYABLE(DataTransferItem);
public:
    static DataTransferItem* create(DataTransfer*, DataObjectItem*);

    String kind() const;
    String type() const;

    void getAsString(ExecutionContext*, StringCallback*) const;
    Blob* getAsFile() const;

    DataTransfer* dataTransfer() { return m_dataTransfer.get(); }
    DataObjectItem* dataObjectItem() { return m_item.get(); }

    DECLARE_TRACE();

private:
    DataTransferItem(DataTransfer*, DataObjectItem*);

    Member<DataTransfer> m_dataTransfer;
    Member<DataObjectItem> m_item;
};

} // namespace blink

#endif // DataTransferItem_h
