// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef electron_browser_api_ApiDownloadItem_h
#define electron_browser_api_ApiDownloadItem_h

#include "common/api/EventEmitter.h"

#include "mbvip/core/mb.h"
#include <vector>
#include <map>

namespace atom {

class ApiDownloadItem : public mate::EventEmitter<ApiDownloadItem> {
public:
    static ApiDownloadItem* create(v8::Isolate* isolate);

    std::string getSavePath() const
    {
        return m_savePath;             
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target);
    static void MB_CALL_TYPE staticOnNetJobDataFinishCallback(void* ptr, mbNetJob job, mbLoadingResult result);
    static void MB_CALL_TYPE staticOnNetJobDataRecvCallback(void* ptr, mbNetJob job, const char* data, int length);
    static void MB_CALL_TYPE staticOnPopupDialogSaveNameCallback(void* ptr, const wchar_t* filePath);

private:
    ApiDownloadItem(v8::Isolate* isolate, v8::Local<v8::Object> wrapper);
    ~ApiDownloadItem();

    void setSavePathApi(const std::string path);
    std::string getSavePathApi() const;
    void setSaveDialogOptionsApi(const v8::FunctionCallbackInfo<v8::Value>& args/*options*/);
    void getSaveDialogOptionsApi(const v8::FunctionCallbackInfo<v8::Value>& args) const;
    void pauseApi();
    bool isPausedApi() const;
    void resumeApi();
    bool canResumeApi() const;
    void cancelsApi();
    std::string getURLApi() const;
    std::string getMimeTypeApi() const;
    bool hasUserGestureApi() const;
    std::string getFilenameApi() const;
    int getTotalBytesApi() const;
    int getReceivedBytesApi() const;
    std::string getContentDispositionApi() const;
    std::string getStateApi() const;
    std::vector<std::string> getURLChainApi() const;
    std::string getLastModifiedTimeApi() const;
    std::string getETagApi() const;
    std::string getStartTimeApi() const;

public:
    int m_id;
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
    std::string m_url;
    std::string m_mime;
    size_t m_recvSize;
    size_t m_allSize;

    std::string m_savePath; // 路径+文件名
    std::string m_disposition;

    v8::Persistent<v8::Object> m_liveSelf;

    enum State {
        kProgressing,
        kCompleted,
        kCancelled,
    };
    State m_state;
    bool m_isPaused;

private:
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
};

} // atom namespace

namespace gin {

v8::Local<v8::Value> ConvertToV8(v8::Isolate* isolate, const atom::ApiDownloadItem& item);

}

#endif // electron_browser_api_ApiDownloadItem_h

