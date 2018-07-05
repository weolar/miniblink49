
#include "node/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/StringUtil.h"
#include "common/api/EventEmitter.h"
#include "gin/object_template_builder.h"
#include "gin/dictionary.h"
#include "base/strings/string_util.h"

#include <Shobjidl.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <CommDlg.h>
#include <cderr.h>
#include <vector>

//struct __declspec(uuid("00000000-0000-0000-c000-000000000046")) IFileOpenDialog;

namespace atom {

class Dialog : public mate::EventEmitter<Dialog> {
public:
    explicit Dialog(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<Dialog>::InitWith(isolate, wrapper);
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Dialog"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("_showOpenDialog", &Dialog::_showOpenDialogApi);
        builder.SetMethod("_showSaveDialog", &Dialog::_showSaveDialogApi);
        builder.SetMethod("_showMessageBox", &Dialog::_showMessageBoxApi);
        builder.SetMethod("_showErrorBox", &Dialog::_showErrorBoxApi);
        
        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Dialog"), prototype->GetFunction());
    }

    void nullFunction() {
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            new Dialog(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

    // showSaveDialog([browserWindow, ]options[, callback])
    void _showSaveDialogApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        _showOpenOrSaveDialogApi(false, args);
    }

    void _showOpenDialogApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        _showOpenOrSaveDialogApi(true, args);
    }

    // showOpenDialog([browserWindow, ]options[, callback])
    void _showOpenOrSaveDialogApi(bool isOpenOrSave, const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (3 != args.Length())
            return;

        int browserWindowId = -1;
        if (args[0]->IsInt32())
            browserWindowId = args[0]->ToInt32()->Value();

        v8::Local<v8::Object> options;
        if (args[1]->IsObject())
            options = args[1]->ToObject();

        v8::Function* callback = nullptr;
        if (args[2]->IsFunction())
            callback = v8::Function::Cast(*(args[2]));

        std::string title;
        std::string defaultPath;
        std::string buttonLabel;
        Filters filters;
        base::ListValue* properties = nullptr;
        int fileDialogProperty;
        getOptions(args.GetIsolate(), options, &title, &defaultPath, &filters, &buttonLabel, &properties, &fileDialogProperty);

        base::ListValue paths;
        showOpenOrSaveDialog(isOpenOrSave, NULL, title, buttonLabel, defaultPath, filters, fileDialogProperty, &paths);

        v8::Local<v8::Value> result = v8::Undefined(args.GetIsolate());
        if (0 != paths.GetSize())
            result = gin::Converter<base::ListValue>::ToV8(args.GetIsolate(), paths);

        if (callback) {
            v8::Local<v8::Object> recv = args.GetIsolate()->GetCurrentContext()->Global();
            v8::Local<v8::Value> argv[1];
            argv[0] = result;
            callback->Call(recv, 1, argv);
        } else
            args.GetReturnValue().Set(result);
    }

    void _showErrorBoxApi(const std::string& title, const std::string& content) {
        std::wstring titleW = StringUtil::UTF8ToUTF16(title);
        std::wstring contentW = StringUtil::UTF8ToUTF16(content);
        ::MessageBoxW(nullptr, contentW.c_str(), titleW.c_str(), MB_OK);
    }

    void _showMessageBoxApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        if (3 != args.Length())
            return;

        int browserWindowId = -1;
        v8::Local<v8::Object> options;
        if (args[0]->IsInt32())
            browserWindowId = args[0]->ToInt32()->Value();

        if (args[1]->IsObject())
            options = args[1]->ToObject();

        v8::Function* callback = nullptr;
        if (args[2]->IsFunction())
            callback = v8::Function::Cast(*(args[2]));

        std::string title;
        std::string message;
        std::string type;
        std::vector<std::string> buttons;
        getMessageOptions(args.GetIsolate(), options, &title, &message, &type, &buttons);

        UINT uType = MB_ICONINFORMATION;
        if ("error" == type)
            uType = MB_ICONERROR;
        else if ("question" == type)
            uType = MB_ICONQUESTION;
        else if ("warning" == type)
            uType = MB_ICONWARNING;

        if (1 == buttons.size())
            uType |= MB_OK;
        else if (2 == buttons.size())
            uType |= MB_YESNO;
        else if (3 == buttons.size())
            uType |= MB_YESNOCANCEL;

        int result = ::MessageBoxW(nullptr, base::UTF8ToWide(message).c_str(), base::UTF8ToWide(title).c_str(), uType);
        if (1 == buttons.size()) {
            result = 0;
        } else if (2 == buttons.size()) {
            if (IDYES == result)
                result = 0;
            else
                result = 1;
        } else if (3 == buttons.size()) {
            if (IDYES == result)
                result = 0;
            else if (IDNO == result)
                result = 1;
            else
                result = 2;
        }

        v8::Local<v8::Value> v8Result = v8::Integer::New(args.GetIsolate(), result).As<v8::Value>();
        if (callback) {
            v8::Local<v8::Object> recv = args.GetIsolate()->GetCurrentContext()->Global();
            v8::Local<v8::Value> argv[1];
            argv[0] = v8Result;
            callback->Call(recv, 1, argv);
        } else
            args.GetReturnValue().Set(v8Result);
    }

private:
    enum FileDialogProperty {
        FILE_DIALOG_OPEN_FILE = 1 << 0,
        FILE_DIALOG_OPEN_DIRECTORY = 1 << 1,
        FILE_DIALOG_MULTI_SELECTIONS = 1 << 2,
        FILE_DIALOG_CREATE_DIRECTORY = 1 << 3,
        FILE_DIALOG_SHOW_HIDDEN_FILES = 1 << 4,
        FILE_DIALOG_PROMPT_TO_CREATE = 1 << 5,
    };

    // <description, extensions>
    typedef std::pair<std::string, std::vector<std::string> > Filter;
    typedef std::vector<Filter> Filters;

    void getMessageOptions(v8::Isolate* isolate,
        v8::Local<v8::Object> options,
        std::string* title,
        std::string* message,
        std::string* type,
        std::vector<std::string>* buttons) {
        base::DictionaryValue optionsDict;
        if (options.IsEmpty() || !gin::Converter<base::DictionaryValue>::FromV8(isolate, options, &optionsDict))
            return;

        optionsDict.GetString("title", title);
        optionsDict.GetString("message", message);
        optionsDict.GetString("type", type);

        base::ListValue* buttonsList;
        optionsDict.GetList("buttons", &buttonsList);
        for (size_t i = 0; i < buttonsList->GetSize(); ++i) {
            std::string button;
            buttonsList->GetString(i, &button);
            if (!button.empty())
                buttons->push_back(button);
        }        
    }

    void getOptions(v8::Isolate* isolate, 
        v8::Local<v8::Object> options,
        std::string* title,
        std::string* defaultPath,
        Filters* filters,
        std::string* buttonLabel,
        base::ListValue** properties,
        int* fileDialogProperty) {
        base::DictionaryValue optionsDict;
        if (options.IsEmpty() || !gin::Converter<base::DictionaryValue>::FromV8(isolate, options, &optionsDict))
            return;

        optionsDict.GetString("title", title);
        optionsDict.GetString("defaultPath", defaultPath);
        optionsDict.GetString("buttonLabel", buttonLabel);
        optionsDict.GetList("properties", properties);
        *fileDialogProperty = propertiesToEnum(*properties);

        base::ListValue* filtersList = nullptr;
        optionsDict.GetList("filters", &filtersList);
        if (!filtersList)
            return;

        for (size_t i = 0; i < filtersList->GetSize(); ++i) {
            base::DictionaryValue* filtersItem;
            if (!filtersList->GetDictionary(i, &filtersItem))
                continue;
            std::string name;
            filtersItem->GetString("name", &name);

            base::ListValue* extensionsList;
            if (!filtersItem->GetList("extensions", &extensionsList))
                continue;

            Filter filter;
            std::vector<std::string> extensions;
            for (size_t j = 0; j < extensionsList->GetSize(); ++j) {
                std::string extension;
                if (!extensionsList->GetString(j, &extension))
                    continue;
                if (extension.empty() || ((size_t)-1) != extension.find(L'.'))
                    continue;
                extensions.push_back(extension);
            }
            if (0 == extensions.size())
                extensions.push_back("*");
            filter.first = name;
            filter.second = extensions;
            filters->push_back(filter);
        }
    }

    int propertiesToEnum(base::ListValue* properties) {
        int out = 0;
        if (!properties)
            return out;

        for (size_t i = 0; i < properties->GetSize(); ++i) {
            std::string propertie;
            properties->GetString(i, &propertie);
            if ("openFile" == propertie)
                out |= FILE_DIALOG_OPEN_FILE;
            else if ("openDirectory" == propertie)
                out |= FILE_DIALOG_OPEN_DIRECTORY;
            else if ("multiSelections" == propertie)
                out |= FILE_DIALOG_MULTI_SELECTIONS;
            else if ("showHiddenFiles" == propertie)
                out |= FILE_DIALOG_SHOW_HIDDEN_FILES;
            else if ("createDirectory" == propertie)
                out |= FILE_DIALOG_CREATE_DIRECTORY;
            else if ("promptToCreate" == propertie)
                out |= FILE_DIALOG_PROMPT_TO_CREATE;
        }
        return out;
    }

//     std::vector<std::wstring> showOpenFile() {
//         HRESULT hr = S_OK;
//         std::vector<std::wstring> filePaths;
// 
//         IFileOpenDialog *fileDlg = NULL;
//         hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDlg));
//         if (FAILED(hr)) return filePaths;
//         //ON_SCOPE_EXIT([&] { fileDlg->Release(); });
// 
//         IKnownFolderManager *pkfm = NULL;
//         hr = CoCreateInstance(CLSID_KnownFolderManager,
//             NULL,
//             CLSCTX_INPROC_SERVER,
//             IID_PPV_ARGS(&pkfm));
//         if (FAILED(hr)) return filePaths;
//         //ON_SCOPE_EXIT([&] { pkfm->Release(); });
// 
//         IKnownFolder *pKnownFolder = NULL;
//         hr = pkfm->GetFolder(FOLDERID_PublicMusic, &pKnownFolder);
//         if (FAILED(hr)) return filePaths;
//         //ON_SCOPE_EXIT([&] { pKnownFolder->Release(); });
// 
//         IShellItem *psi = NULL;
//         hr = pKnownFolder->GetShellItem(0, IID_PPV_ARGS(&psi));
//         if (FAILED(hr)) return filePaths;
//         //ON_SCOPE_EXIT([&] { psi->Release(); });
// 
//         hr = fileDlg->AddPlace(psi, FDAP_BOTTOM);
//         COMDLG_FILTERSPEC rgSpec[] = { 
//             { L"音乐文件", L"*.mp3;*.wav;" }
//         };
//         fileDlg->SetFileTypes(1, rgSpec);
// 
//         DWORD dwOptions;
//         fileDlg->GetOptions(&dwOptions);
//         fileDlg->SetOptions(dwOptions | FOS_ALLOWMULTISELECT);
//         hr = fileDlg->Show(NULL);
//         if (SUCCEEDED(hr)) {
//             IShellItemArray *pRets;
//             hr = fileDlg->GetResults(&pRets);
//             if (SUCCEEDED(hr)) {
//                 DWORD count;
//                 pRets->GetCount(&count);
//                 for (DWORD i = 0; i < count; i++) {
//                     IShellItem *pRet;
//                     LPWSTR nameBuffer;
//                     pRets->GetItemAt(i, &pRet);
//                     pRet->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &nameBuffer);
//                     filePaths.push_back(std::wstring(nameBuffer));
//                     pRet->Release();
//                     CoTaskMemFree(nameBuffer);
//                 }
//                 pRets->Release();
//             }
//         }
//         return filePaths;
//     }

    void pushStringToVector(std::vector<wchar_t>* buffer, const std::wstring& str) {
        for (size_t i = 0; i < str.length(); ++i) {
            buffer->push_back(str[i]);
        }
    }

    void convertFilters(const Filters& filters, std::vector<wchar_t>* buffer) {
        for (size_t i = 0; i < filters.size(); ++i) {
            Filter filter = filters[i];
            std::wstring name = base::UTF8ToWide(filter.first);
            pushStringToVector(buffer, name);
            buffer->push_back(L'\0');

            for (size_t j = 0; j < filter.second.size(); ++j) {
                std::string extension = filter.second[j];
                std::wstring extensionW = base::UTF8ToWide(extension);
                extensionW.insert(0, L"*.");
                extensionW.append(L";");
                pushStringToVector(buffer, extensionW);
            }
            buffer->push_back(L'\0');
        }
        buffer->push_back(L'\0');
    }

    bool showOpenOrSaveDialog(
        bool isOpenOrSave,
        HWND parentWindow,
        const std::string& title,
        const std::string& buttonLabel,
        const std::string& defaultPath,
        const Filters& filters,
        int properties,
        base::ListValue* paths) {
        std::wstring titleW = base::UTF8ToWide(title);
        std::wstring defaultPathW = base::UTF8ToWide(defaultPath);
        std::vector<wchar_t> filtersStr;

        if (properties & FILE_DIALOG_OPEN_DIRECTORY) {
            std::wstring resuleDir;
            selectDir(parentWindow, titleW, defaultPathW, &resuleDir);
            paths->AppendString(base::WideToUTF8(resuleDir));
            return true;
        }

        convertFilters(filters, &filtersStr);

        OPENFILENAMEW ofn = { 0 };
        std::vector<wchar_t> fileResult;
        fileResult.resize(1 * MAX_PATH + 1);

        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = parentWindow;
        ofn.lpstrFilter = &filtersStr[0]; // L"Picture File(*.bmp,*.jpg)\0*.bmp;*.jpg;\0\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFile = &fileResult[0];
        ofn.nMaxFile = 1 * MAX_PATH;
        ofn.lpstrFileTitle = nullptr;// titleW.c_str();
        ofn.nMaxFileTitle = 0;// titleW.size();
        ofn.lpstrInitialDir = defaultPathW.c_str();
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;

        if (properties & FILE_DIALOG_MULTI_SELECTIONS)
            ofn.Flags |= OFN_ALLOWMULTISELECT;

        BOOL b = isOpenOrSave ? GetOpenFileNameW(&ofn) : GetSaveFileNameW(&ofn);
        if (b) {
            splitPathFromGetOpenFileNameResult(fileResult, paths);
            return true;
        }

        if (FNERR_BUFFERTOOSMALL != CommDlgExtendedError())
            return false;

        unsigned short size = *((unsigned short*)&fileResult[0]);
        fileResult.resize(size + 2);
        ofn.lpstrFile = &fileResult[0];
        ofn.nMaxFile = size;
        b = isOpenOrSave ? GetOpenFileNameW(&ofn) : GetSaveFileNameW(&ofn);
        if (b) {
            splitPathFromGetOpenFileNameResult(fileResult, paths);
            return true;
        }

        return false;
    }

    void splitPathFromGetOpenFileNameResult(const std::vector<wchar_t>& fileResult, base::ListValue* paths) {
        std::vector<std::wstring> pathsTemp;
        const wchar_t* begin = &fileResult[0];
        const wchar_t* end = nullptr;
        for (size_t i = 0; i < fileResult.size() - 1 && L'\0' != *begin; ++i) {
            if (L'\0' == fileResult[i]) {
                end = &fileResult[0] + i;
                if (end == begin)
                    return;

                std::wstring path(begin, end - begin);
                pathsTemp.push_back((path));

                begin = &fileResult[0] + i + 1;
            }
        }

        if (0 == pathsTemp.size())
            return;

        if (1 == pathsTemp.size())
            paths->AppendString(base::WideToUTF8(pathsTemp[0]));

        if (2 <= pathsTemp.size()) {
            std::wstring rootPath = pathsTemp[0];
            if (1 >= rootPath.size())
                return;

            if (L'\\' != rootPath[rootPath.size() - 1])
                rootPath += L'\\';
            for (size_t i = 1; i < pathsTemp.size(); ++i) {
                std::wstring path = rootPath;
                path += pathsTemp[i];
                paths->AppendString(base::WideToUTF8(path));
            }
        }

        return;
    }

    static int CALLBACK browseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
        switch (uMsg) {
        case BFFM_INITIALIZED:
            ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
            break;
        }

        return 0;
    }

    unsigned selectDir(HWND parentWindow,
        const std::wstring& title,
        const std::wstring& defaultPath,
        std::wstring* strDir) {
        std::vector<wchar_t> szDir;
        szDir.resize(MAX_PATH);
        BROWSEINFO bi;
        bi.hwndOwner = parentWindow;
        bi.pidlRoot = NULL;
        bi.pszDisplayName = &szDir[0];
        bi.lpszTitle = title.c_str();
        bi.iImage = 0;

        bi.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
        bi.lpfn = browseCallbackProc;
        bi.lParam = (LPARAM)(LPCWSTR)defaultPath.c_str();

        LPITEMIDLIST lp = SHBrowseForFolder(&bi);
        if (lp && SHGetPathFromIDList(lp, &szDir[0])) {
            *strDir = &szDir[0];
            if (0 != strDir->size() && L'\\' != strDir->at(strDir->size() - 1))
                strDir->append(L"\\");
            return IDOK;
        }

        return IDCANCEL;
    }
    
    static const int ID_COMBO_ADDR = 0x47c;
    static const int ID_LEFT_TOOBAR = 0x4A0;
    static LONG g_lOriWndProc;

    LRESULT static __stdcall  _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_COMMAND: {
                if (wParam == IDOK) {
                    std::vector<wchar_t> wcDirPath;
                    wcDirPath.resize(MAX_PATH);
                    HWND hComboAddr = GetDlgItem(hwnd, ID_COMBO_ADDR);
                    if (hComboAddr != NULL)
                        GetWindowText(hComboAddr, &wcDirPath[0], MAX_PATH);
                    
                    if (!wcslen(&wcDirPath[0]))
                        break;
                    
                    DWORD dwAttr = GetFileAttributes(&wcDirPath[0]);
                    if (dwAttr != -1 && (FILE_ATTRIBUTE_DIRECTORY & dwAttr)) {
                        LPOPENFILENAMEW oFn = (LPOPENFILENAME)GetProp(hwnd, L"OPENFILENAME");
                        if (oFn) {
                            int size = oFn->nMaxFile > MAX_PATH ? MAX_PATH : oFn->nMaxFile;
                            memcpy(oFn->lpstrFile, &wcDirPath[0], size * sizeof(wchar_t));
                            ::RemoveProp(hwnd, L"OPENFILENAME");
                            ::EndDialog(hwnd, 1);
                        } else {
                            ::EndDialog(hwnd, 0);
                        }
                    }
                    break;
                }
                //////////////////////////////////////////////////////////////////////////
                //如果是左边toolbar发出的WM_COMMOND消息（即点击左边的toolbar）, 则清空OK按钮旁的组合框。
                HWND hCtrl = (HWND)lParam;
                if (hCtrl == NULL) {
                    break;
                }
                int ctrlId = ::GetDlgCtrlID(hCtrl);
                if (ctrlId == ID_LEFT_TOOBAR) {
                    HWND hComboAddr = ::GetDlgItem(hwnd, ID_COMBO_ADDR);
                    if (hComboAddr != NULL) {
                        ::SetWindowTextW(hComboAddr, L"");
                    }
                }
            }
            break;
        }
        int i = CallWindowProc((WNDPROC)g_lOriWndProc, hwnd, uMsg, wParam, lParam);
        return i;
    }

    UINT_PTR static __stdcall folderProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) {
        //参考reactos可知，hdlg 是一个隐藏的对话框，其父窗口为打开文件对话框， OK，CANCEL按钮等控件的消息在父窗口处理。
        if (uiMsg != WM_NOTIFY)
            return 1;
        
        LPOFNOTIFY lpOfNotify = (LPOFNOTIFY)lParam;
        if (lpOfNotify->hdr.code == CDN_INITDONE) {
            SetPropW(GetParent(hdlg), L"OPENFILENAME", (HANDLE)(lpOfNotify->lpOFN));
            g_lOriWndProc = ::SetWindowLongW(::GetParent(hdlg), GWL_WNDPROC, (LONG)_WndProc);
        }

        if (lpOfNotify->hdr.code != CDN_SELCHANGE)
            return 1;

        std::vector<wchar_t> wcDirPath;
        wcDirPath.resize(MAX_PATH);
        CommDlg_OpenSave_GetFilePathW(::GetParent(hdlg), &wcDirPath[0], sizeof(wchar_t) * MAX_PATH);
        HWND hComboAddr = ::GetDlgItem(::GetParent(hdlg), ID_COMBO_ADDR);
        if (NULL == hComboAddr)
            return 1;

        size_t pathSize = wcslen(&wcDirPath[0]);
        if (0 != pathSize) { //去掉文件夹快捷方式的后缀名。
            if (pathSize >= 4) {
                wchar_t* wcExtension = ::PathFindExtensionW(&wcDirPath[0]);
                if (wcslen(wcExtension)) {
                    wcExtension = ::CharLowerW(wcExtension);
                    if (!wcscmp(wcExtension, L".lnk")) {
                        wcDirPath[pathSize - 4] = L'\0';
                    }
                }
            }

            ::SetWindowTextW(hComboAddr, &wcDirPath[0]);
        } else {
            ::SetWindowTextW(hComboAddr, L"");
        }

        return 1;
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
};

LONG Dialog::g_lOriWndProc = 0;

v8::Persistent<v8::Function> Dialog::constructor;
gin::WrapperInfo Dialog::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeDialogApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Dialog::init(env->isolate(), target);
}

static const char BrowserDialogNative[] =
"console.log('BrowserDialogNative');"
"exports = function {};";

static NodeNative nativeBrowserDialogNative{ "Dialog", BrowserDialogNative, sizeof(BrowserDialogNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_dialog, initializeDialogApi, &nativeBrowserDialogNative)

}