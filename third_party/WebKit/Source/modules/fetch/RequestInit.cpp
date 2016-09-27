// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/fetch/RequestInit.h"

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/V8ArrayBuffer.h"
#include "bindings/core/v8/V8ArrayBufferView.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8Blob.h"
#include "bindings/core/v8/V8FormData.h"
#include "core/fileapi/Blob.h"
#include "modules/fetch/Headers.h"
#include "platform/blob/BlobData.h"
#include "platform/network/FormData.h"

namespace blink {

RequestInit::RequestInit(ExecutionContext* context, const Dictionary& options, ExceptionState& exceptionState)
{
    DictionaryHelper::get(options, "method", method);
    DictionaryHelper::get(options, "headers", headers);
    if (!headers) {
        Vector<Vector<String>> headersVector;
        if (DictionaryHelper::get(options, "headers", headersVector, exceptionState))
            headers = Headers::create(headersVector, exceptionState);
        else
            DictionaryHelper::get(options, "headers", headersDictionary);
    }
    DictionaryHelper::get(options, "mode", mode);
    DictionaryHelper::get(options, "credentials", credentials);

    v8::Local<v8::Value> body;
    if (!DictionaryHelper::get(options, "body", body) || body->IsUndefined() || body->IsNull())
        return;
    OwnPtr<BlobData> blobData = BlobData::create();
    v8::Isolate* isolate = toIsolate(context);
    if (body->IsArrayBuffer()) {
        DOMArrayBuffer* arrayBuffer = V8ArrayBuffer::toImpl(v8::Local<v8::Object>::Cast(body));
        ASSERT(arrayBuffer);
        blobData->appendBytes(arrayBuffer->data(), arrayBuffer->byteLength());
    } else if (body->IsArrayBufferView()) {
        DOMArrayBufferView* arrayBufferView = V8ArrayBufferView::toImpl(v8::Local<v8::Object>::Cast(body));
        ASSERT(arrayBufferView);
        blobData->appendBytes(arrayBufferView->baseAddress(), arrayBufferView->byteLength());
    } else if (V8Blob::hasInstance(body, isolate)) {
        Blob* blob = V8Blob::toImpl(v8::Local<v8::Object>::Cast(body));
        ASSERT(blob);
        blob->appendTo(*blobData);
        blobData->setContentType(blob->type());
    } else if (V8FormData::hasInstance(body, isolate)) {
        DOMFormData* domFormData = V8FormData::toImpl(v8::Local<v8::Object>::Cast(body));
        ASSERT(domFormData);
        RefPtr<FormData> httpBody = domFormData->createMultiPartFormData();
        for (size_t i = 0; i < httpBody->elements().size(); ++i) {
            const FormDataElement& element = httpBody->elements()[i];
            switch (element.m_type) {
            case FormDataElement::data: {
                blobData->appendBytes(element.m_data.data(), element.m_data.size());
                break;
            }
            case FormDataElement::encodedFile:
                blobData->appendFile(element.m_filename, element.m_fileStart, element.m_fileLength, element.m_expectedFileModificationTime);
                break;
            case FormDataElement::encodedBlob:
                if (element.m_optionalBlobDataHandle)
                    blobData->appendBlob(element.m_optionalBlobDataHandle, 0, element.m_optionalBlobDataHandle->size());
                break;
            case FormDataElement::encodedFileSystemURL:
                blobData->appendFileSystemURL(element.m_fileSystemURL, element.m_fileStart, element.m_fileLength, element.m_expectedFileModificationTime);
                break;
            default:
                ASSERT_NOT_REACHED();
            }
        }
        blobData->setContentType(AtomicString("multipart/form-data; boundary=", AtomicString::ConstructFromLiteral) + httpBody->boundary().data());
    } else if (body->IsString()) {
        String stringValue(toUSVString(isolate, body, exceptionState));
        blobData->appendText(stringValue, false);
        blobData->setContentType("text/plain;charset=UTF-8");
    } else {
        return;
    }
    const long long blobSize = blobData->length();
    bodyBlobHandle = BlobDataHandle::create(blobData.release(), blobSize);
}

}
