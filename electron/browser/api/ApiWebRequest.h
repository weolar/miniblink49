// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/EventEmitter.h"

namespace atom {

class ApiWebRequest : public mate::EventEmitter<ApiWebRequest> {
public:
    static const char* kDefaultSessionName;
    static const char* kDefaultDir;
    
    static ApiWebRequest* create(v8::Isolate* isolate);

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target);
    
private:
    friend class ApiSession;
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    void onBeforeSendHeadersApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void onSendHeadersApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void onHeadersReceivedApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void onResponseStartedApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void onBeforeRedirectApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void onCompletedApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void onErrorOccurredApi(const v8::FunctionCallbackInfo<v8::Value>& args);

    ApiWebRequest(v8::Isolate* isolate, v8::Local<v8::Object> wrapper);
    v8::Persistent<v8::Object> m_liveSelf;

    v8::Persistent<v8::Value> m_beforeSendHeadersCb;
    v8::Persistent<v8::Value> m_sendHeadersCb;

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
};

} // atom namespace

