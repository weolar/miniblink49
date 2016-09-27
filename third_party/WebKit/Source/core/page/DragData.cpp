/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2013 Google Inc.
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
#include "core/page/DragData.h"

#include "core/clipboard/DataObject.h"
#include "core/dom/DocumentFragment.h"
#include "core/dom/Range.h"
#include "core/editing/markup.h"
#include "core/frame/LocalFrame.h"
#include "platform/FileMetadata.h"
#include "platform/clipboard/ClipboardMimeTypes.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/WTFString.h"

namespace blink {

DragData::DragData(DataObject* data, const IntPoint& clientPosition, const IntPoint& globalPosition,
    DragOperation sourceOperationMask, DragApplicationFlags flags)
    : m_clientPosition(clientPosition)
    , m_globalPosition(globalPosition)
    , m_platformDragData(data)
    , m_draggingSourceOperationMask(sourceOperationMask)
    , m_applicationFlags(flags)
{
}

static bool containsHTML(const DataObject* dropData)
{
    return dropData->types().contains(mimeTypeTextHTML);
}

bool DragData::containsURL(FilenameConversionPolicy filenamePolicy) const
{
    return m_platformDragData->types().contains(mimeTypeTextURIList)
        || (filenamePolicy == ConvertFilenames && m_platformDragData->containsFilenames());
}

String DragData::asURL(FilenameConversionPolicy filenamePolicy, String* title) const
{
    String url;
    if (m_platformDragData->types().contains(mimeTypeTextURIList))
        m_platformDragData->urlAndTitle(url, title);
    else if (filenamePolicy == ConvertFilenames && containsFiles())
        url = filePathToURL(m_platformDragData->filenames()[0]);
    return url;
}

bool DragData::containsFiles() const
{
    return m_platformDragData->containsFilenames();
}

int DragData::modifiers() const
{
    return m_platformDragData->modifiers();
}

void DragData::asFilePaths(Vector<String>& result) const
{
    const Vector<String>& filenames = m_platformDragData->filenames();
    for (size_t i = 0; i < filenames.size(); ++i) {
        if (!filenames[i].isEmpty())
            result.append(filenames[i]);
    }
}

bool DragData::containsPlainText() const
{
    return m_platformDragData->types().contains(mimeTypeTextPlain);
}

String DragData::asPlainText() const
{
    return m_platformDragData->getData(mimeTypeTextPlain);
}

bool DragData::canSmartReplace() const
{
    // Mimic the situations in which mac allows drag&drop to do a smart replace.
    // This is allowed whenever the drag data contains a 'range' (ie.,
    // ClipboardWin::writeRange is called). For example, dragging a link
    // should not result in a space being added.
    return m_platformDragData->types().contains(mimeTypeTextPlain)
        && !m_platformDragData->types().contains(mimeTypeTextURIList);
}

bool DragData::containsCompatibleContent() const
{
    return containsPlainText()
        || containsURL()
        || containsHTML(m_platformDragData)
        || containsFiles();
}

PassRefPtrWillBeRawPtr<DocumentFragment> DragData::asFragment(LocalFrame* frame, PassRefPtrWillBeRawPtr<Range>, bool, bool&) const
{
    /*
     * Order is richest format first. On OSX this is:
     * * Web Archive
     * * Filenames
     * * HTML
     * * RTF
     * * TIFF
     * * PICT
     */

    if (containsFiles()) {
        // FIXME: Implement this. Should be pretty simple to make some HTML
        // and call createFragmentFromMarkup.
    }

    if (m_platformDragData->types().contains(mimeTypeTextHTML)) {
        String html;
        KURL baseURL;
        m_platformDragData->htmlAndBaseURL(html, baseURL);
        ASSERT(frame->document());
        if (RefPtrWillBeRawPtr<DocumentFragment> fragment = createFragmentFromMarkup(*frame->document(), html, baseURL, DisallowScriptingAndPluginContent))
            return fragment.release();
    }

    return nullptr;
}

String DragData::droppedFileSystemId() const
{
    return m_platformDragData->filesystemId();
}

} // namespace blink
