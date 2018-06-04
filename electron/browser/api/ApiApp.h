//#include "nodeblink.h"
#include "common/api/EventEmitter.h"

typedef struct HWND__ *HWND;
typedef struct tagCOPYDATASTRUCT COPYDATASTRUCT;
typedef void *HANDLE;

namespace atom {

class App : public mate::EventEmitter<App> {
public:
    explicit App(v8::Isolate* isolate, v8::Local<v8::Object> wrapper);

    ~App();

    static void init(v8::Local<v8::Object> target, v8::Isolate* isolate);
    static App* getInstance();

    void nullFunction();

    void quitApi();
    void exitApi();
    void focusApi();
    bool isReadyApi() const;
    void addRecentDocumentApi(const std::string& path);
    void clearRecentDocumentsApi();
    void setAppUserModelIdApi(const std::string& id);
    bool isDefaultProtocolClientApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    bool setAsDefaultProtocolClientApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    bool removeAsDefaultProtocolClientApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    bool setBadgeCountApi(int count);
    int getBadgeCountApi();
    int getLoginItemSettingsApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void setLoginItemSettingsApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    bool setUserTasksApi(const v8::FunctionCallbackInfo<v8::Value>& args);

    bool isAccessibilitySupportEnabled() { return false; }
    void disableHardwareAcceleration() {}

    void setVersionApi(const std::string& version) { m_version = version; }
    std::string getVersionApi() const { return m_version; }

    void setNameApi(const std::string& name) { m_name = name; }
    std::string getNameApi() const { return m_name; }

    void setPathApi(const std::string& name, const std::string& path);
    std::string getPathApi(const std::string& name) const;

    void setDesktopNameApi(const std::string& desktopName);

    v8::Local<v8::Value> getJumpListSettingsApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void setJumpListApi(const v8::FunctionCallbackInfo<v8::Value>& args);

    std::string getLocaleApi();

    bool makeSingleInstanceImplApi(const v8::FunctionCallbackInfo<v8::Value>& args);
    void releaseSingleInstanceApi();

    void relaunchApi(const base::DictionaryValue& options);

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

    void onWindowAllClosed();

public:
    void onCopyData(const COPYDATASTRUCT* copyData);

    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;

    v8::Persistent<v8::Value> m_singleInstanceCall;
    HWND m_hiddenWindow;
    HANDLE m_singleInstanceHandle;

private:
    static App* m_instance;

    std::string m_version;
    std::string m_name;
    std::map<std::string, std::string> m_pathMap;
};

} // atom