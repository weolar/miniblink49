/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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

#ifndef DragData_h
#define DragData_h

#include "core/CoreExport.h"
#include "core/page/DragActions.h"
#include "platform/geometry/IntPoint.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"

namespace blink {

class DataObject;
class DocumentFragment;
class LocalFrame;
class Range;

enum DragApplicationFlags {
    DragApplicationNone = 0,
    DragApplicationIsModal = 1,
    DragApplicationIsSource = 2,
    DragApplicationHasAttachedSheet = 4,
    DragApplicationIsCopyKeyDown = 8
};

class CORE_EXPORT DragData {
    STACK_ALLOCATED();
public:
    enum FilenameConversionPolicy { DoNotConvertFilenames, ConvertFilenames };

    // clientPosition is taken to be the position of the drag event within the target window, with (0,0) at the top left
    DragData(DataObject*, const IntPoint& clientPosition, const IntPoint& globalPosition, DragOperation, DragApplicationFlags = DragApplicationNone);
    const IntPoint& clientPosition() const { return m_clientPosition; }
    const IntPoint& globalPosition() const { return m_globalPosition; }
    DragApplicationFlags flags() const { return m_applicationFlags; }
    DataObject* platformData() const { return m_platformDragData; }
    DragOperation draggingSourceOperationMask() const { return m_draggingSourceOperationMask; }
    bool containsURL(FilenameConversionPolicy filenamePolicy = ConvertFilenames) const;
    bool containsPlainText() const;
    bool containsCompatibleContent() const;
    String asURL(FilenameConversionPolicy filenamePolicy = ConvertFilenames, String* title = nullptr) const;
    String asPlainText() const;
    void asFilePaths(Vector<String>&) const;
    PassRefPtrWillBeRawPtr<DocumentFragment> asFragment(LocalFrame*, PassRefPtrWillBeRawPtr<Range> context, bool allowPlainText, bool& chosePlainText) const;
    bool canSmartReplace() const;
    bool containsFiles() const;
    int modifiers() const;

    String droppedFileSystemId() const;

private:
    IntPoint m_clientPosition;
    IntPoint m_globalPosition;
    Member<DataObject> m_platformDragData;
    DragOperation m_draggingSourceOperationMask;
    DragApplicationFlags m_applicationFlags;
};

}

#endif // !DragData_h
