/*
 * Copyright (C) 2010 weolar Inc. All rights reserved.
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
 *     * Neither the name of weolar Inc. nor the names of its
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

#include "content\web_impl_win\WebBlobRegistryImpl.h"

namespace content {

WebBlobRegistryImpl::BuilderImpl::BuilderImpl()
{
}

WebBlobRegistryImpl::BuilderImpl::~BuilderImpl() 
{ 
}

void WebBlobRegistryImpl::BuilderImpl::appendData(const WebThreadSafeData&)
{

}
void WebBlobRegistryImpl::BuilderImpl::appendFile(const WebString& path, uint64_t offset, uint64_t length, double expectedModificationTime)
{

}

void WebBlobRegistryImpl::BuilderImpl::appendBlob(const WebString& uuid, uint64_t offset, uint64_t length)
{

}
void WebBlobRegistryImpl::BuilderImpl::appendFileSystemURL(const WebURL&, uint64_t offset, uint64_t length, double expectedModificationTime)
{

}

void WebBlobRegistryImpl::BuilderImpl::build()
{

}

WebBlobRegistryImpl::~WebBlobRegistryImpl() { }

void WebBlobRegistryImpl::registerBlobData(const WebString& uuid, const WebBlobData&) { }

WebBlobRegistry::Builder* WebBlobRegistryImpl::createBuilder(const WebString& uuid, const WebString& contentType)
{ 
    return new WebBlobRegistryImpl::BuilderImpl();
}

void WebBlobRegistryImpl::addBlobDataRef(const WebString& uuid) { }
void WebBlobRegistryImpl::removeBlobDataRef(const WebString& uuid) { }
void WebBlobRegistryImpl::registerPublicBlobURL(const WebURL&, const WebString& uuid) { }
void WebBlobRegistryImpl::revokePublicBlobURL(const WebURL&) { }

void WebBlobRegistryImpl::registerStreamURL(const WebURL&, const WebString&) {  }

void WebBlobRegistryImpl::registerStreamURL(const WebURL&, const WebURL& srcURL) {  }

void WebBlobRegistryImpl::addDataToStream(const WebURL&, const char* data, size_t length) {  }

void WebBlobRegistryImpl::flushStream(const WebURL&) {  }

void WebBlobRegistryImpl::finalizeStream(const WebURL&) {  }

// Tell the registry that construction of this stream has been aborted and
// so it won't receive any more data.
void WebBlobRegistryImpl::abortStream(const WebURL&) {  }

// Unregisters a stream referred by the URL.
void WebBlobRegistryImpl::unregisterStreamURL(const WebURL&) {  }

} // namespace content
