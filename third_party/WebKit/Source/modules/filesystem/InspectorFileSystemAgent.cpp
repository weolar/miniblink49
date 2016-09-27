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

#include "config.h"
#include "modules/filesystem/InspectorFileSystemAgent.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMImplementation.h"
#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "core/fileapi/File.h"
#include "core/fileapi/FileError.h"
#include "core/fileapi/FileReader.h"
#include "core/frame/LocalFrame.h"
#include "core/html/VoidCallback.h"
#include "core/html/parser/TextResourceDecoder.h"
#include "core/inspector/InspectorState.h"
#include "core/page/Page.h"
#include "modules/filesystem/DOMFileSystem.h"
#include "modules/filesystem/DirectoryEntry.h"
#include "modules/filesystem/DirectoryReader.h"
#include "modules/filesystem/EntriesCallback.h"
#include "modules/filesystem/Entry.h"
#include "modules/filesystem/EntryCallback.h"
#include "modules/filesystem/ErrorCallback.h"
#include "modules/filesystem/FileCallback.h"
#include "modules/filesystem/FileEntry.h"
#include "modules/filesystem/FileSystemCallbacks.h"
#include "modules/filesystem/LocalFileSystem.h"
#include "modules/filesystem/Metadata.h"
#include "modules/filesystem/MetadataCallback.h"
#include "platform/MIMETypeRegistry.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/text/Base64.h"
#include "wtf/text/TextEncoding.h"

using blink::TypeBuilder::Array;

typedef blink::InspectorBackendDispatcher::FileSystemCommandHandler::RequestFileSystemRootCallback RequestFileSystemRootCallback;
typedef blink::InspectorBackendDispatcher::FileSystemCommandHandler::RequestDirectoryContentCallback RequestDirectoryContentCallback;
typedef blink::InspectorBackendDispatcher::FileSystemCommandHandler::RequestMetadataCallback RequestMetadataCallback;
typedef blink::InspectorBackendDispatcher::FileSystemCommandHandler::RequestFileContentCallback RequestFileContentCallback;
typedef blink::InspectorBackendDispatcher::FileSystemCommandHandler::DeleteEntryCallback DeleteEntryCallback;

namespace blink {

namespace FileSystemAgentState {
static const char fileSystemAgentEnabled[] = "fileSystemAgentEnabled";
}

namespace {

template<typename BaseCallback, typename Handler, typename Argument>
class CallbackDispatcher final : public BaseCallback {
public:
    typedef bool (Handler::*HandlingMethod)(Argument);

    static CallbackDispatcher* create(PassRefPtr<Handler> handler, HandlingMethod handlingMethod)
    {
        return new CallbackDispatcher(handler, handlingMethod);
    }

    void handleEvent(Argument argument) override
    {
        (m_handler.get()->*m_handlingMethod)(argument);
    }

private:
    CallbackDispatcher(PassRefPtr<Handler> handler, HandlingMethod handlingMethod)
        : m_handler(handler)
        , m_handlingMethod(handlingMethod) { }

    RefPtr<Handler> m_handler;
    HandlingMethod m_handlingMethod;
};

template<typename BaseCallback>
class CallbackDispatcherFactory {
public:
    template<typename Handler, typename Argument>
    static CallbackDispatcher<BaseCallback, Handler, Argument>* create(Handler* handler, bool (Handler::*handlingMethod)(Argument))
    {
        return CallbackDispatcher<BaseCallback, Handler, Argument>::create(PassRefPtr<Handler>(handler), handlingMethod);
    }
};

class FileSystemRootRequest : public RefCounted<FileSystemRootRequest> {
    WTF_MAKE_NONCOPYABLE(FileSystemRootRequest);
public:
    static PassRefPtr<FileSystemRootRequest> create(PassRefPtrWillBeRawPtr<RequestFileSystemRootCallback> requestCallback, const String& type)
    {
        return adoptRef(new FileSystemRootRequest(requestCallback, type));
    }

    void start(ExecutionContext*);

private:
    bool didHitError(FileError* error)
    {
        reportResult(error->code());
        return true;
    }

    bool didGetEntry(Entry*);

    void reportResult(FileError::ErrorCode errorCode, PassRefPtr<TypeBuilder::FileSystem::Entry> entry = nullptr)
    {
        m_requestCallback->sendSuccess(static_cast<int>(errorCode), entry);
    }

    FileSystemRootRequest(PassRefPtrWillBeRawPtr<RequestFileSystemRootCallback> requestCallback, const String& type)
        : m_requestCallback(requestCallback)
        , m_type(type) { }

    RefPtrWillBePersistent<RequestFileSystemRootCallback> m_requestCallback;
    String m_type;
};

void FileSystemRootRequest::start(ExecutionContext* executionContext)
{
    ASSERT(executionContext);

    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &FileSystemRootRequest::didHitError);

    FileSystemType type;
    if (!DOMFileSystemBase::pathPrefixToFileSystemType(m_type, type)) {
        errorCallback->handleEvent(FileError::create(FileError::SYNTAX_ERR));
        return;
    }

    KURL rootURL = DOMFileSystemBase::createFileSystemRootURL(executionContext->securityOrigin()->toString(), type);
    if (!rootURL.isValid()) {
        errorCallback->handleEvent(FileError::create(FileError::SYNTAX_ERR));
        return;
    }

    EntryCallback* successCallback = CallbackDispatcherFactory<EntryCallback>::create(this, &FileSystemRootRequest::didGetEntry);
    OwnPtr<AsyncFileSystemCallbacks> fileSystemCallbacks = ResolveURICallbacks::create(successCallback, errorCallback, executionContext);
    LocalFileSystem::from(*executionContext)->resolveURL(executionContext, rootURL, fileSystemCallbacks.release());
}

bool FileSystemRootRequest::didGetEntry(Entry* entry)
{
    RefPtr<TypeBuilder::FileSystem::Entry> result = TypeBuilder::FileSystem::Entry::create()
        .setUrl(entry->toURL())
        .setName("/")
        .setIsDirectory(true);
    reportResult(static_cast<FileError::ErrorCode>(0), result);
    return true;
}

class DirectoryContentRequest final : public RefCounted<DirectoryContentRequest> {
    WTF_MAKE_NONCOPYABLE(DirectoryContentRequest);
public:
    static PassRefPtr<DirectoryContentRequest> create(PassRefPtrWillBeRawPtr<RequestDirectoryContentCallback> requestCallback, const String& url)
    {
        return adoptRef(new DirectoryContentRequest(requestCallback, url));
    }

    ~DirectoryContentRequest()
    {
        reportResult(FileError::ABORT_ERR);
    }

    void start(ExecutionContext*);

private:
    bool didHitError(FileError* error)
    {
        reportResult(error->code());
        return true;
    }

    bool didGetEntry(Entry*);
    bool didReadDirectoryEntries(const EntryHeapVector&);

    void reportResult(FileError::ErrorCode errorCode, PassRefPtr<Array<TypeBuilder::FileSystem::Entry>> entries = nullptr)
    {
        m_requestCallback->sendSuccess(static_cast<int>(errorCode), entries);
    }

    DirectoryContentRequest(PassRefPtrWillBeRawPtr<RequestDirectoryContentCallback> requestCallback, const String& url)
        : m_requestCallback(requestCallback)
        , m_url(ParsedURLString, url) { }

    void readDirectoryEntries();

    RefPtrWillBePersistent<RequestDirectoryContentCallback> m_requestCallback;
    KURL m_url;
    RefPtr<Array<TypeBuilder::FileSystem::Entry>> m_entries;
    Persistent<DirectoryReader> m_directoryReader;
};

void DirectoryContentRequest::start(ExecutionContext* executionContext)
{
    ASSERT(executionContext);

    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &DirectoryContentRequest::didHitError);
    EntryCallback* successCallback = CallbackDispatcherFactory<EntryCallback>::create(this, &DirectoryContentRequest::didGetEntry);

    OwnPtr<AsyncFileSystemCallbacks> fileSystemCallbacks = ResolveURICallbacks::create(successCallback, errorCallback, executionContext);

    LocalFileSystem::from(*executionContext)->resolveURL(executionContext, m_url, fileSystemCallbacks.release());
}

bool DirectoryContentRequest::didGetEntry(Entry* entry)
{
    if (!entry->isDirectory()) {
        reportResult(FileError::TYPE_MISMATCH_ERR);
        return true;
    }

    m_directoryReader = toDirectoryEntry(entry)->createReader();
    m_entries = Array<TypeBuilder::FileSystem::Entry>::create();
    readDirectoryEntries();
    return true;
}

void DirectoryContentRequest::readDirectoryEntries()
{
    if (!m_directoryReader->filesystem()->executionContext()) {
        reportResult(FileError::ABORT_ERR);
        return;
    }

    EntriesCallback* successCallback = CallbackDispatcherFactory<EntriesCallback>::create(this, &DirectoryContentRequest::didReadDirectoryEntries);
    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &DirectoryContentRequest::didHitError);
    m_directoryReader->readEntries(successCallback, errorCallback);
}

bool DirectoryContentRequest::didReadDirectoryEntries(const EntryHeapVector& entries)
{
    if (entries.isEmpty()) {
        reportResult(static_cast<FileError::ErrorCode>(0), m_entries);
        return true;
    }

    for (size_t i = 0; i < entries.size(); ++i) {
        Entry* entry = entries[i];
        RefPtr<TypeBuilder::FileSystem::Entry> entryForFrontend = TypeBuilder::FileSystem::Entry::create()
            .setUrl(entry->toURL())
            .setName(entry->name())
            .setIsDirectory(entry->isDirectory());

        using TypeBuilder::Page::ResourceType;
        if (!entry->isDirectory()) {
            String mimeType = MIMETypeRegistry::getMIMETypeForPath(entry->name());
            ResourceType::Enum resourceType;
            if (MIMETypeRegistry::isSupportedImageMIMEType(mimeType)) {
                resourceType = ResourceType::Image;
                entryForFrontend->setIsTextFile(false);
            } else if (MIMETypeRegistry::isSupportedJavaScriptMIMEType(mimeType)) {
                resourceType = ResourceType::Script;
                entryForFrontend->setIsTextFile(true);
            } else if (MIMETypeRegistry::isSupportedNonImageMIMEType(mimeType)) {
                resourceType = ResourceType::Document;
                entryForFrontend->setIsTextFile(true);
            } else {
                resourceType = ResourceType::Other;
                entryForFrontend->setIsTextFile(DOMImplementation::isXMLMIMEType(mimeType) || DOMImplementation::isTextMIMEType(mimeType));
            }

            entryForFrontend->setMimeType(mimeType);
            entryForFrontend->setResourceType(resourceType);
        }

        m_entries->addItem(entryForFrontend);
    }
    readDirectoryEntries();
    return true;
}

class MetadataRequest final : public RefCounted<MetadataRequest> {
    WTF_MAKE_NONCOPYABLE(MetadataRequest);
public:
    static PassRefPtr<MetadataRequest> create(PassRefPtrWillBeRawPtr<RequestMetadataCallback> requestCallback, const String& url)
    {
        return adoptRef(new MetadataRequest(requestCallback, url));
    }

    ~MetadataRequest()
    {
        reportResult(FileError::ABORT_ERR);
    }

    void start(ExecutionContext*);

private:
    bool didHitError(FileError* error)
    {
        reportResult(error->code());
        return true;
    }

    bool didGetEntry(Entry*);
    bool didGetMetadata(Metadata*);

    void reportResult(FileError::ErrorCode errorCode, PassRefPtr<TypeBuilder::FileSystem::Metadata> metadata = nullptr)
    {
        m_requestCallback->sendSuccess(static_cast<int>(errorCode), metadata);
    }

    MetadataRequest(PassRefPtrWillBeRawPtr<RequestMetadataCallback> requestCallback, const String& url)
        : m_requestCallback(requestCallback)
        , m_url(ParsedURLString, url) { }

    RefPtrWillBePersistent<RequestMetadataCallback> m_requestCallback;
    KURL m_url;
    bool m_isDirectory;
};

void MetadataRequest::start(ExecutionContext* executionContext)
{
    ASSERT(executionContext);

    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &MetadataRequest::didHitError);
    EntryCallback* successCallback = CallbackDispatcherFactory<EntryCallback>::create(this, &MetadataRequest::didGetEntry);
    OwnPtr<AsyncFileSystemCallbacks> fileSystemCallbacks = ResolveURICallbacks::create(successCallback, errorCallback, executionContext);
    LocalFileSystem::from(*executionContext)->resolveURL(executionContext, m_url, fileSystemCallbacks.release());
}

bool MetadataRequest::didGetEntry(Entry* entry)
{
    if (!entry->filesystem()->executionContext()) {
        reportResult(FileError::ABORT_ERR);
        return true;
    }

    MetadataCallback* successCallback = CallbackDispatcherFactory<MetadataCallback>::create(this, &MetadataRequest::didGetMetadata);
    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &MetadataRequest::didHitError);
    entry->getMetadata(successCallback, errorCallback);
    m_isDirectory = entry->isDirectory();
    return true;
}

bool MetadataRequest::didGetMetadata(Metadata* metadata)
{
    using TypeBuilder::FileSystem::Metadata;
    RefPtr<Metadata> result = Metadata::create()
        .setModificationTime(metadata->modificationTime())
        .setSize(metadata->size());
    reportResult(static_cast<FileError::ErrorCode>(0), result);
    return true;
}

class FileContentRequest final : public EventListener {
    WTF_MAKE_NONCOPYABLE(FileContentRequest);
public:
    static PassRefPtr<FileContentRequest> create(PassRefPtrWillBeRawPtr<RequestFileContentCallback> requestCallback, const String& url, bool readAsText, long long start, long long end, const String& charset)
    {
        return adoptRef(new FileContentRequest(requestCallback, url, readAsText, start, end, charset));
    }

    ~FileContentRequest() override
    {
        reportResult(FileError::ABORT_ERR);
    }

    void start(ExecutionContext*);

    bool operator==(const EventListener& other) override
    {
        return this == &other;
    }

    void handleEvent(ExecutionContext*, Event* event) override
    {
        if (event->type() == EventTypeNames::load)
            didRead();
        else if (event->type() == EventTypeNames::error)
            didHitError(m_reader->error());
    }

private:
    bool didHitError(FileError* error)
    {
        reportResult(error->code());
        return true;
    }

    bool didGetEntry(Entry*);
    bool didGetFile(File*);
    void didRead();

    void reportResult(FileError::ErrorCode errorCode, const String* result = 0, const String* charset = 0)
    {
        m_requestCallback->sendSuccess(static_cast<int>(errorCode), result, charset);
    }

    FileContentRequest(PassRefPtrWillBeRawPtr<RequestFileContentCallback> requestCallback, const String& url, bool readAsText, long long start, long long end, const String& charset)
        : EventListener(EventListener::CPPEventListenerType)
        , m_requestCallback(requestCallback)
        , m_url(ParsedURLString, url)
        , m_readAsText(readAsText)
        , m_start(start)
        , m_end(end)
        , m_charset(charset) { }

    RefPtrWillBePersistent<RequestFileContentCallback> m_requestCallback;
    KURL m_url;
    bool m_readAsText;
    int m_start;
    long long m_end;
    String m_mimeType;
    String m_charset;

    Persistent<FileReader> m_reader;
};

void FileContentRequest::start(ExecutionContext* executionContext)
{
    ASSERT(executionContext);

    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &FileContentRequest::didHitError);
    EntryCallback* successCallback = CallbackDispatcherFactory<EntryCallback>::create(this, &FileContentRequest::didGetEntry);

    OwnPtr<AsyncFileSystemCallbacks> fileSystemCallbacks = ResolveURICallbacks::create(successCallback, errorCallback, executionContext);
    LocalFileSystem::from(*executionContext)->resolveURL(executionContext, m_url, fileSystemCallbacks.release());
}

bool FileContentRequest::didGetEntry(Entry* entry)
{
    if (entry->isDirectory()) {
        reportResult(FileError::TYPE_MISMATCH_ERR);
        return true;
    }

    if (!entry->filesystem()->executionContext()) {
        reportResult(FileError::ABORT_ERR);
        return true;
    }

    FileCallback* successCallback = CallbackDispatcherFactory<FileCallback>::create(this, &FileContentRequest::didGetFile);
    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &FileContentRequest::didHitError);
    toFileEntry(entry)->file(successCallback, errorCallback);

    m_reader = FileReader::create(entry->filesystem()->executionContext());
    m_mimeType = MIMETypeRegistry::getMIMETypeForPath(entry->name());

    return true;
}

bool FileContentRequest::didGetFile(File* file)
{
    Blob* blob = file->Blob::slice(m_start, m_end, IGNORE_EXCEPTION);
    m_reader->setOnload(this);
    m_reader->setOnerror(this);

    m_reader->readAsArrayBuffer(blob, IGNORE_EXCEPTION);
    return true;
}

void FileContentRequest::didRead()
{
    StringOrArrayBuffer resultAttribute;
    m_reader->result(resultAttribute);
    RefPtr<DOMArrayBuffer> buffer = resultAttribute.getAsArrayBuffer();

    if (!m_readAsText) {
        String result = base64Encode(static_cast<char*>(buffer->data()), buffer->byteLength());
        reportResult(static_cast<FileError::ErrorCode>(0), &result, 0);
        return;
    }

    OwnPtr<TextResourceDecoder> decoder = TextResourceDecoder::create(m_mimeType, m_charset, true);
    String result = decoder->decode(static_cast<char*>(buffer->data()), buffer->byteLength());
    result = result + decoder->flush();
    m_charset = decoder->encoding().name();
    reportResult(static_cast<FileError::ErrorCode>(0), &result, &m_charset);
}

class DeleteEntryRequest final : public RefCounted<DeleteEntryRequest> {
public:
    static PassRefPtr<DeleteEntryRequest> create(PassRefPtrWillBeRawPtr<DeleteEntryCallback> requestCallback, const KURL& url)
    {
        return adoptRef(new DeleteEntryRequest(requestCallback, url));
    }

    ~DeleteEntryRequest()
    {
        reportResult(FileError::ABORT_ERR);
    }

    void start(ExecutionContext*);

private:
    // CallbackDispatcherFactory doesn't handle 0-arg handleEvent methods
    class VoidCallbackImpl final : public VoidCallback {
    public:
        explicit VoidCallbackImpl(PassRefPtr<DeleteEntryRequest> handler)
            : m_handler(handler)
        {
        }

        void handleEvent() override
        {
            m_handler->didDeleteEntry();
        }

    private:
        RefPtr<DeleteEntryRequest> m_handler;
    };

    bool didHitError(FileError* error)
    {
        reportResult(error->code());
        return true;
    }

    bool didGetEntry(Entry*);
    bool didDeleteEntry();

    void reportResult(FileError::ErrorCode errorCode)
    {
        m_requestCallback->sendSuccess(static_cast<int>(errorCode));
    }

    DeleteEntryRequest(PassRefPtrWillBeRawPtr<DeleteEntryCallback> requestCallback, const KURL& url)
        : m_requestCallback(requestCallback)
        , m_url(url) { }

    RefPtrWillBePersistent<DeleteEntryCallback> m_requestCallback;
    KURL m_url;
};

void DeleteEntryRequest::start(ExecutionContext* executionContext)
{
    ASSERT(executionContext);

    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &DeleteEntryRequest::didHitError);

    FileSystemType type;
    String path;
    if (!DOMFileSystemBase::crackFileSystemURL(m_url, type, path)) {
        errorCallback->handleEvent(FileError::create(FileError::SYNTAX_ERR));
        return;
    }

    if (path == "/") {
        VoidCallback* successCallback = new VoidCallbackImpl(this);
        OwnPtr<AsyncFileSystemCallbacks> fileSystemCallbacks = VoidCallbacks::create(successCallback, errorCallback, executionContext, nullptr);
        LocalFileSystem::from(*executionContext)->deleteFileSystem(executionContext, type, fileSystemCallbacks.release());
    } else {
        EntryCallback* successCallback = CallbackDispatcherFactory<EntryCallback>::create(this, &DeleteEntryRequest::didGetEntry);
        OwnPtr<AsyncFileSystemCallbacks> fileSystemCallbacks = ResolveURICallbacks::create(successCallback, errorCallback, executionContext);
        LocalFileSystem::from(*executionContext)->resolveURL(executionContext, m_url, fileSystemCallbacks.release());
    }
}

bool DeleteEntryRequest::didGetEntry(Entry* entry)
{
    VoidCallback* successCallback = new VoidCallbackImpl(this);
    ErrorCallback* errorCallback = CallbackDispatcherFactory<ErrorCallback>::create(this, &DeleteEntryRequest::didHitError);
    if (entry->isDirectory()) {
        DirectoryEntry* directoryEntry = toDirectoryEntry(entry);
        directoryEntry->removeRecursively(successCallback, errorCallback);
    } else {
        entry->remove(successCallback, errorCallback);
    }
    return true;
}

bool DeleteEntryRequest::didDeleteEntry()
{
    reportResult(static_cast<FileError::ErrorCode>(0));
    return true;
}

} // anonymous namespace

// static
PassOwnPtrWillBeRawPtr<InspectorFileSystemAgent> InspectorFileSystemAgent::create(Page* page)
{
    return adoptPtrWillBeNoop(new InspectorFileSystemAgent(page));
}

InspectorFileSystemAgent::~InspectorFileSystemAgent()
{
}

void InspectorFileSystemAgent::enable(ErrorString*)
{
    if (m_enabled)
        return;
    m_enabled = true;
    m_state->setBoolean(FileSystemAgentState::fileSystemAgentEnabled, m_enabled);
}

void InspectorFileSystemAgent::disable(ErrorString*)
{
    if (!m_enabled)
        return;
    m_enabled = false;
    m_state->setBoolean(FileSystemAgentState::fileSystemAgentEnabled, m_enabled);
}

void InspectorFileSystemAgent::requestFileSystemRoot(ErrorString* error, const String& origin, const String& type, PassRefPtrWillBeRawPtr<RequestFileSystemRootCallback> requestCallback)
{
    if (!assertEnabled(error))
        return;

    ExecutionContext* executionContext = assertExecutionContextForOrigin(error, SecurityOrigin::createFromString(origin).get());
    if (!executionContext)
        return;

    FileSystemRootRequest::create(requestCallback, type)->start(executionContext);
}

void InspectorFileSystemAgent::requestDirectoryContent(ErrorString* error, const String& url, PassRefPtrWillBeRawPtr<RequestDirectoryContentCallback> requestCallback)
{
    if (!assertEnabled(error))
        return;

    ExecutionContext* executionContext = assertExecutionContextForOrigin(error, SecurityOrigin::createFromString(url).get());
    if (!executionContext)
        return;

    DirectoryContentRequest::create(requestCallback, url)->start(executionContext);
}

void InspectorFileSystemAgent::requestMetadata(ErrorString* error, const String& url, PassRefPtrWillBeRawPtr<RequestMetadataCallback> requestCallback)
{
    if (!assertEnabled(error))
        return;

    ExecutionContext* executionContext = assertExecutionContextForOrigin(error, SecurityOrigin::createFromString(url).get());
    if (!executionContext)
        return;

    MetadataRequest::create(requestCallback, url)->start(executionContext);
}

void InspectorFileSystemAgent::requestFileContent(ErrorString* error, const String& url, bool readAsText, const int* start, const int* end, const String* charset, PassRefPtrWillBeRawPtr<RequestFileContentCallback> requestCallback)
{
    if (!assertEnabled(error))
        return;

    ExecutionContext* executionContext = assertExecutionContextForOrigin(error, SecurityOrigin::createFromString(url).get());
    if (!executionContext)
        return;

    long long startPosition = start ? *start : 0;
    long long endPosition = end ? *end : std::numeric_limits<long long>::max();
    FileContentRequest::create(requestCallback, url, readAsText, startPosition, endPosition, charset ? *charset : "")->start(executionContext);
}

void InspectorFileSystemAgent::deleteEntry(ErrorString* error, const String& urlString, PassRefPtrWillBeRawPtr<DeleteEntryCallback> requestCallback)
{
    if (!assertEnabled(error))
        return;

    KURL url(ParsedURLString, urlString);

    ExecutionContext* executionContext = assertExecutionContextForOrigin(error, SecurityOrigin::create(url).get());
    if (!executionContext)
        return;

    DeleteEntryRequest::create(requestCallback, url)->start(executionContext);
}

void InspectorFileSystemAgent::restore()
{
    m_enabled = m_state->getBoolean(FileSystemAgentState::fileSystemAgentEnabled);
}

InspectorFileSystemAgent::InspectorFileSystemAgent(Page* page)
    : InspectorBaseAgent<InspectorFileSystemAgent, InspectorFrontend::FileSystem>("FileSystem")
    , m_page(page)
    , m_enabled(false)
{
    ASSERT(m_page);
}

bool InspectorFileSystemAgent::assertEnabled(ErrorString* error)
{
    if (!m_enabled) {
        *error = "FileSystem agent is not enabled.";
        return false;
    }
    return true;
}

ExecutionContext* InspectorFileSystemAgent::assertExecutionContextForOrigin(ErrorString* error, SecurityOrigin* origin)
{
    for (Frame* frame = m_page->mainFrame(); frame; frame = frame->tree().traverseNext()) {
        if (!frame->isLocalFrame())
            continue;
        LocalFrame* localFrame = toLocalFrame(frame);
        if (localFrame->document() && localFrame->document()->securityOrigin()->isSameSchemeHostPort(origin))
            return localFrame->document();
    }

    *error = "No frame is available for the request";
    return 0;
}

DEFINE_TRACE(InspectorFileSystemAgent)
{
    visitor->trace(m_page);
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink
