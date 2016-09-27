/*
 * Copyright (C) 2011, 2012 Google Inc. All rights reserved.
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

#ifndef InspectorFileSystemAgent_h
#define InspectorFileSystemAgent_h

#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "modules/ModulesExport.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class ExecutionContext;
class Page;
class SecurityOrigin;

class MODULES_EXPORT InspectorFileSystemAgent final : public InspectorBaseAgent<InspectorFileSystemAgent, InspectorFrontend::FileSystem>, public InspectorBackendDispatcher::FileSystemCommandHandler {
public:
    static PassOwnPtrWillBeRawPtr<InspectorFileSystemAgent> create(Page*);
    ~InspectorFileSystemAgent() override;
    DECLARE_VIRTUAL_TRACE();

    void enable(ErrorString*) override;

    void requestFileSystemRoot(ErrorString*, const String& origin, const String& typeString, PassRefPtrWillBeRawPtr<RequestFileSystemRootCallback>) override;
    void requestDirectoryContent(ErrorString*, const String& url, PassRefPtrWillBeRawPtr<RequestDirectoryContentCallback>) override;
    void requestMetadata(ErrorString*, const String& url, PassRefPtrWillBeRawPtr<RequestMetadataCallback>) override;
    void requestFileContent(ErrorString*, const String& url, bool readAsText, const int* start, const int* end, const String* charset, PassRefPtrWillBeRawPtr<RequestFileContentCallback>) override;
    void deleteEntry(ErrorString*, const String& url, PassRefPtrWillBeRawPtr<DeleteEntryCallback>) override;

    void disable(ErrorString*) override;
    void restore() override;

private:
    explicit InspectorFileSystemAgent(Page*);
    bool assertEnabled(ErrorString*);
    ExecutionContext* assertExecutionContextForOrigin(ErrorString*, SecurityOrigin*);

    RawPtrWillBeMember<Page> m_page;
    bool m_enabled;
};

} // namespace blink

#endif // InspectorFileSystemAgent_h
