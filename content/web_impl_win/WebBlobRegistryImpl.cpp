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
#define INSIDE_BLINK 1

#include "content/web_impl_win/WebBlobRegistryImpl.h"

#include "platform/blob/BlobData.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include "net/BlobResourceLoader.h"

using namespace blink;

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

void WebBlobRegistryImpl::registerBlobData(const WebString& uuid, const WebBlobData& data)
{
    String uuidString = uuid;
    size_t i = 0;
    WebBlobData::Item dataItem;
    net::BlobDataWrap* dataWrap = new net::BlobDataWrap();
    dataWrap->m_contentType = data.contentType();
    dataWrap->m_ref = 1;

    while (data.itemAt(i++, dataItem)) {
        dataWrap->appendItem(new WebBlobData::Item(dataItem));
    }

    String out = String::format("WebBlobRegistryImpl::registerBlobData: %p, %s\n", dataWrap, uuidString.utf8().data());
    OutputDebugStringA(out.utf8().data());

    check();

    const Vector<blink::WebBlobData::Item*>& items = dataWrap->items();

    HashMap<String, net::BlobDataWrap*>::const_iterator it = m_datasSet.begin();
    for (; it != m_datasSet.end(); ++it) {
        net::BlobDataWrap* dataWrapValue = it->value;
        const Vector<blink::WebBlobData::Item*>& valueItems = dataWrapValue->items();
        for (size_t i = 0; i < valueItems.size(); ++i) {
            blink::WebBlobData::Item* valueiIem = valueItems[i];
            
            for (size_t i = 0; i < items.size(); ++i) {
                blink::WebBlobData::Item* item = items[i];

                if ((String)item->filePath == (String)valueiIem->filePath) {
                    item->length = valueiIem->length;
                    item->offset = valueiIem->offset;
                }
            }
        }
    }

    m_datasSet.add(uuidString, dataWrap);
}

WebBlobRegistry::Builder* WebBlobRegistryImpl::createBuilder(const WebString& uuid, const WebString& contentType)
{ 
    DebugBreak();
    return new WebBlobRegistryImpl::BuilderImpl();
}

void WebBlobRegistryImpl::check() const
{
#if 0
    HashMap<String, net::BlobDataWrap*>::const_iterator it = m_datasSet.begin();
    for (; it != m_datasSet.end(); ++it) {
        net::BlobDataWrap* dataWrapValue = it->value;
        const Vector<blink::WebBlobData::Item*>& valueItems = dataWrapValue->items();
        for (size_t i = 0; i < valueItems.size(); ++i) {
            blink::WebBlobData::Item* valueiIem = valueItems[i];
            if (!valueiIem)
                DebugBreak();
        }
    }
#endif
}

net::BlobDataWrap* WebBlobRegistryImpl::getBlobDataFromUUID(const String& url) const
{
    ASSERT(isMainThread());

    check();

    HashMap<String, net::BlobDataWrap*>::const_iterator it = m_datasSet.find(url);
    if (m_datasSet.end() == it)
        return nullptr;
    net::BlobDataWrap* dataWrap = it->value;
    return dataWrap;
}

// url is temp path
void WebBlobRegistryImpl::setBlobDataLengthByTempPath(const String& tempPath, size_t length) const
{
    ASSERT(isMainThread());

    net::BlobDataWrap* dataWrap = nullptr;
    HashMap<String, net::BlobDataWrap*>::const_iterator it = m_datasSet.begin();
    for (; it != m_datasSet.end(); ++it) {
        net::BlobDataWrap* dataWrap = (it->value);
        const Vector<blink::WebBlobData::Item*>& items = dataWrap->items();
        for (size_t i = 0; i < items.size(); ++i) {
            blink::WebBlobData::Item* item = items[i];
            if ((String)item->filePath == tempPath) {
                item->offset = 0;
                item->length = length;

//                 String out = String::format("WebBlobRegistryImpl::setBlobDataLengthByTempPath: %p %s\n", dataWrap, it->key.utf8().data());
//                 OutputDebugStringA(out.utf8().data());
            }
        }
    }

    check();
}

void WebBlobRegistryImpl::addBlobDataRef(const WebString& uuid)
{
    net::BlobDataWrap* dataWrap = getBlobDataFromUUID(uuid);
    if (!dataWrap)
        return;

    dataWrap->m_ref++;

    check();
}

void WebBlobRegistryImpl::removeBlobDataRef(const WebString& uuid)
{
    net::BlobDataWrap* dataWrap = getBlobDataFromUUID(uuid);
    if (!dataWrap)
        return;

    dataWrap->m_ref--;
    if (0 != dataWrap->m_ref)
        return;

    bool find = false;
    do {
        find = false;
        HashMap<String, net::BlobDataWrap*>::const_iterator it = m_datasSet.begin();
        for (; it != m_datasSet.end(); ++it) {
            if (it->value != dataWrap)
                continue;
            m_datasSet.remove(it->key);
            find = true;
            break;
        }
    } while (find);

    delete dataWrap;
    check();
}

// ��uuid��Ӧ��dataȡ�����ٽ���url��data�Ķ�Ӧ���൱��һ��data������uuid
void WebBlobRegistryImpl::registerPublicBlobURL(const WebURL& url, const WebString& uuid)
{
    HashMap<String, net::BlobDataWrap*>::iterator it = m_datasSet.find(uuid);
    if (m_datasSet.end() == it)
        return;
    net::BlobDataWrap* dataWrap = (it->value);
    dataWrap->m_ref++;

//     String out = String::format("WebBlobRegistryImpl::registerPublicBlobURL: %p %s\n", dataWrap, url.string().utf8().data());
//     OutputDebugStringA(out.utf8().data());

    m_datasSet.set(url.string(), dataWrap);

    check();
}

void WebBlobRegistryImpl::revokePublicBlobURL(const WebURL& url)
{
    removeBlobDataRef(url.string());
    m_datasSet.remove(url.string());

    check();
}

void WebBlobRegistryImpl::registerStreamURL(const WebURL& url, const WebString&) {  }

void WebBlobRegistryImpl::registerStreamURL(const WebURL& url, const WebURL& srcURL) {  }

void WebBlobRegistryImpl::addDataToStream(const WebURL& url, const char* data, size_t length) {  }

void WebBlobRegistryImpl::flushStream(const WebURL& url) {  }

void WebBlobRegistryImpl::finalizeStream(const WebURL& url) {  }

// Tell the registry that construction of this stream has been aborted and
// so it won't receive any more data.
void WebBlobRegistryImpl::abortStream(const WebURL& url)
{
//     HashMap<String, net::BlobDataWrap*>::const_iterator it = m_datasSet.begin();
//     for (; it != m_datasSet.end(); ++it) {
//         String out = it->key;
//         out.append("\n");
//         OutputDebugStringA(out.utf8().data());
// 
//         net::BlobDataWrap* dataWrap = it->value;
// 
//         Vector<blink::WebBlobData::Item*>& items = dataWrap->m_items;
//         for (size_t i = 0; i < items.size(); ++i) {
//             out = "  ";
//             blink::WebBlobData::Item* item = items[i];
//             out.append(item->filePath);
//             out.append(String::format("  %d\n", item->length));
//             OutputDebugStringA(out.utf8().data());
//         }
//     }
}

// Unregisters a stream referred by the URL.
void WebBlobRegistryImpl::unregisterStreamURL(const WebURL& url) {  }

} // namespace content
