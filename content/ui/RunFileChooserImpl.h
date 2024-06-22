
#ifndef content_browser_RunFileChooserImpl_h
#define content_browser_RunFileChooserImpl_h

#include "third_party/WebKit/public/web/WebFileChooserParams.h"
#include "third_party/WebKit/public/web/WebFileChooserCompletion.h"
#include "third_party/WebKit/public/platform/WebVector.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebFileUtilities.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "content/web_impl_win/WebMimeRegistryImpl.h"
#include "net/ActivatingObjCheck.h"
#include <vector>
#include <shlwapi.h>
#include <ShlObj.h>
#include <process.h>

namespace content {

static void appendStringToVector(std::vector<char>* result, const Vector<char>& str)
{
    result->reserve(result->size() + str.size());
    const char* p = str.data();
    const char* end = p + str.size();
    while (p < end) {
        result->push_back(*p);
        ++p;
    }
}

static void appendStringToVector(std::vector<char>* result, const std::vector<char>& str)
{
    result->reserve(result->size() + str.size());
    const char* p = str.data();
    const char* end = p + str.size();
    while (p < end) {
        result->push_back(*p);
        ++p;
    }
}

static void appendStringToVector(std::vector<char>* result, const std::string& str)
{
    Vector<char> strBuf;
    strBuf.resize(str.size());
    memcpy(strBuf.data(), str.c_str(), str.size());
    appendStringToVector(result, strBuf);
}

std::string extentionForMimeType(const Vector<char>& mimeType)
{
    const char* end = mimeType.data() + mimeType.size();
    const char* p = end - 1;
    while (p > mimeType.data()) {
        if ('/' == *p) {
            return std::string(p + 1, end);
        }
        --p;
    }
    return "*";
}

std::string extentionForMimeType(const std::string& mimeType)
{
    Vector<char> mimeTypeBuf;
    mimeTypeBuf.resize(mimeType.size());
    memcpy(mimeTypeBuf.data(), mimeType.c_str(), mimeType.size());
    return extentionForMimeType(mimeTypeBuf);
}

static Vector<char> getMimeType(const WebString& mime)
{
    String mimeType = mime;
    if (mimeType.isNull() || mimeType.isEmpty())
        return Vector<char>();

    WebMimeRegistryImpl* mimeRegistry = (WebMimeRegistryImpl*)blink::Platform::current()->mimeRegistry();
    Vector<blink::WebString> mimeTypes = mimeRegistry->extensionsForMimeType(mime);

    if (mimeType.isNull() || mimeType.isEmpty()) {
        mimeType = mime;
        if (mimeType[0] == '.')
            mimeType.remove(0);
    }
    Vector<char> mimeTypeBuf = WTF::ensureStringToUTF8(mimeType, false);
    return mimeTypeBuf;
}

static void addForExtensions(const Vector<blink::WebString>& exts, const String& description, std::vector<char>* filter)
{
    if (exts.isEmpty())
        return;

    appendStringToVector(filter, description.utf8().data());
    filter->push_back('\0');

    for (Vector<blink::WebString>::const_iterator it = exts.begin(); it != exts.end(); ++it) {
        std::string str = "*.";
        str += it->utf8().data();
        appendStringToVector(filter, str);
        filter->push_back(';');
    }
    filter->push_back('\0');
}

// static bool runFileChooserTest(const blink::WebFileChooserParams& params, blink::WebFileChooserCompletion* completion)
// {
//     std::vector<wchar_t> fileNameBuf;
//     const int fileNameBufLen = 8192;
//     fileNameBuf.resize(fileNameBufLen);
//     memset(&fileNameBuf[0], 0, sizeof(wchar_t) * fileNameBufLen);
// 
//     OPENFILENAMEW ofn = { 0 };
//     ofn.lStructSize = sizeof(OPENFILENAMEW);
//     ofn.hwndOwner = nullptr;
//     ofn.hInstance = nullptr;
//     ofn.lpstrFilter = L"*/*";
//     ofn.lpstrFile = &fileNameBuf[0];
//     ofn.nMaxFile = fileNameBufLen - 2;
//     ofn.lpstrTitle = L"hahahahahah";
//     ofn.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_NOCHANGEDIR;
//     OPENFILENAMEW* ofnPtr = &ofn;
// 
//     bool saveAs = params.saveAs;
//     bool multiSelect = params.multiSelect;
//     BOOL retVal = FALSE;
//     BOOL finish = FALSE;
//     std::function<void(void)>* callback = new std::function<void(void)>([ofnPtr, saveAs, multiSelect, &retVal, &finish] {
//         ofnPtr->hwndOwner = createHideWindow();
//         if (saveAs) {
//             ofnPtr->Flags = OFN_OVERWRITEPROMPT;
//             retVal = ::GetSaveFileNameW(ofnPtr);
//         } else if (multiSelect) {
//             ofnPtr->Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
//             retVal = ::GetOpenFileNameW(ofnPtr);
//         } else {
//             ofnPtr->Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
//             retVal = ::GetOpenFileNameW(ofnPtr);
//         }
//         finish = TRUE;
//     });
// 
//     unsigned int threadIdentifier = 0;
//     HANDLE threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, getOpenFileNameThread, callback, 0, &threadIdentifier));
//     while (!finish) {
//         ::Sleep(50);
//     }
//     ::CloseHandle(threadHandle);
// 
//     //////////////////////////////////////////////////////////////////////////
// 
//     blink::WebFileChooserCompletion::SelectedFileInfo info;
//     const std::wstring& filePath = L"C:\\Users\\weo\\Desktop\\1.gif";
// 
//     info.path = String(filePath.c_str());
//     info.displayName = blink::Platform::current()->fileUtilities()->baseName(info.path);
//     const std::wstring& fileSystemURL = L"file:///" + filePath;
//     info.fileSystemURL = blink::KURL(blink::ParsedURLString, String(fileSystemURL.c_str()));
// 
//     long long fileSizeResult = 0;
//     if (!getFileSize(filePath.c_str(), fileSizeResult))
//         fileSizeResult = 0;
//     info.modificationTime = 0;
//     info.length = fileSizeResult;
//     info.isDirectory = ::PathIsDirectoryW(filePath.c_str());
// 
//     blink::WebVector<blink::WebFileChooserCompletion::SelectedFileInfo> wsFileNames((size_t)1);
//     wsFileNames[0] = info;
// 
//     completion->didChooseFile(wsFileNames);
//     return true;
// }

// 现在把所有项都归并到一个项了
static void addForExtensions2(const Vector<blink::WebString>& exts, std::vector<char>* filter)
{
    if (exts.isEmpty())
        return;

    for (Vector<blink::WebString>::const_iterator it = exts.begin(); it != exts.end(); ++it) {
        std::string str = "*.";
        str += it->utf8().data();

        appendStringToVector(filter, str);
        filter->push_back(';');
    }
}

class OpenFileNameWrap {
public:
    OpenFileNameWrap(const blink::WebFileChooserParams& params)
    {
        m_isDirectory = params.directory;

        // image/gif, image/jpeg, image/*
        // Image Files(*.gif;*.jpeg;*.png)\0*.gif;*.jpeg;*.png\0All Files(*.*)\0*.*\0\0
        std::vector<char> filter;

//         appendStringToVector(&filter, "Custom Files");
//         filter.push_back('\0');
//
//         for (size_t i = 0; i < params.acceptTypes.size(); ++i) {
//             String mimeType = params.acceptTypes[i];
//             if (mimeType.isNull() || mimeType.isEmpty())
//                 continue;
//             if ('.' == mimeType[0])
//                 mimeType.remove(0, 1);
//             mimeType = mimeType.lower();
// 
//             WebMimeRegistryImpl* mimeRegistry = (WebMimeRegistryImpl*)blink::Platform::current()->mimeRegistry();
//             Vector<blink::WebString> exts = mimeRegistry->extensionsForMimeType(mimeType);
//             addForExtensions2(exts, mimeType, &filter);
//         }
//         filter.push_back('\0');

        Vector<blink::WebString> exts;
        for (size_t i = 0; i < params.acceptTypes.size(); ++i) {
            String mimeType = params.acceptTypes[i];
            if (mimeType.isNull() || mimeType.isEmpty())
                continue;
            if ('.' == mimeType[0])
                mimeType.remove(0, 1);
            mimeType = mimeType.lower();

            WebMimeRegistryImpl* mimeRegistry = (WebMimeRegistryImpl*)blink::Platform::current()->mimeRegistry();
            Vector<blink::WebString> tempExts = mimeRegistry->extensionsForMimeType(mimeType);

            for (size_t j = 0; j < tempExts.size(); ++j) {
                exts.append(tempExts[j]);
            }
        }

        std::vector<char> filterItem;
        addForExtensions2(exts, &filterItem);

        if (filterItem.size() > 0) {
            appendStringToVector(&filter, "Custom Files(");
            appendStringToVector(&filter, filterItem); // 第一遍是描述
            filter.push_back(')');
            filter.push_back('\0');
            appendStringToVector(&filter, filterItem); // 第二遍是给系统看的
            filter.push_back('\0');
        }

        appendStringToVector(&filter, "All Files");
        filter.push_back('\0');
        appendStringToVector(&filter, "*.*");
        filter.push_back('\0');
        filter.push_back('\0');

        String title = params.title;
        if (title.isEmpty())
            title = L"选择文件(Select File)";

        const int fileNameBufLen = 8192;
        m_fileNameBuf.resize(fileNameBufLen);
        memset(&m_fileNameBuf[0], 0, sizeof(wchar_t) * fileNameBufLen);

        String initialValue = params.initialValue;
        Vector<UChar> initialValueBuf;
        if (!initialValue.isNull() && !initialValue.isEmpty()) {
            initialValueBuf = WTF::ensureUTF16UChar(initialValue, true);
            if (initialValueBuf.size() < fileNameBufLen - 1)
                wcscpy(&m_fileNameBuf[0], initialValueBuf.data());
        }

        filter.push_back('\0');

        WTF::MByteToWChar(&filter[0], filter.size() - 1, &m_filterW, CP_UTF8);

        m_titleBuf = WTF::ensureUTF16UChar(title, true);

        memset(&m_ofn, 0, sizeof(OPENFILENAMEW));
        m_ofn.lStructSize = sizeof(OPENFILENAMEW);
        m_ofn.hwndOwner = nullptr;
        m_ofn.hInstance = nullptr;
        m_ofn.lpstrFilter = &m_filterW[0];
        m_ofn.lpstrFile = &m_fileNameBuf[0];
        m_ofn.nMaxFile = fileNameBufLen - 2;
        m_ofn.lpstrTitle = m_titleBuf.data();
        m_ofn.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_NOCHANGEDIR;
    }

    OPENFILENAMEW* getInfo()
    {
        return &m_ofn;
    }

    const wchar_t* getFileNameBuf()
    {
        return &m_fileNameBuf[0];
    }

    bool isDirectory() const { return m_isDirectory; }

private:
    OPENFILENAMEW m_ofn;
    std::vector<wchar_t> m_fileNameBuf;
    std::vector<UChar> m_filterW;
    Vector<UChar> m_titleBuf;

    bool m_isDirectory;
};

class ResponseOpenFileInfoTask : public blink::WebThread::Task {
public:
    ResponseOpenFileInfoTask(
        blink::WebFileChooserCompletion* completion, 
        int webviewId, 
        OpenFileNameWrap* wrap, 
        BOOL retVal, 
        std::function<void(void)>* completionCallback
        )
    {
        m_completion = completion;
        m_webviewId = webviewId;
        m_wrap = wrap;
        m_retVal = retVal;
        m_completionCallback = completionCallback;
    }

    virtual ~ResponseOpenFileInfoTask()
    {
        delete m_wrap;

        (*m_completionCallback)();
        delete m_completionCallback;
    }

    virtual void run() override
    {
        if (!net::ActivatingObjCheck::inst()->isActivating(m_webviewId))
            return;

        std::vector<std::wstring> selectedFiles;
        std::vector<std::wstring> selectedFilesRef;

        if (m_retVal) {
            // Figure out if the user selected multiple files.  If fileNameBuf is a directory, then multiple files were selected!
            if ((m_wrap->getInfo()->Flags & OFN_ALLOWMULTISELECT) && (::GetFileAttributesW(m_wrap->getFileNameBuf()) & FILE_ATTRIBUTE_DIRECTORY)) {
                std::wstring dirName = m_wrap->getFileNameBuf();
                const wchar_t* p = m_wrap->getFileNameBuf() + wcslen(m_wrap->getFileNameBuf()) + 1;
                while (*p) {
                    selectedFiles.push_back(dirName);
                    selectedFiles.back().append(L"\\");
                    selectedFiles.back().append(p);
                    p += wcslen(p) + 1;
                }
                selectedFilesRef.resize(selectedFiles.size());
                for (size_t i = 0; i < selectedFiles.size(); ++i) {
                    selectedFilesRef[i] = selectedFiles[i];
                }
            } else {
                std::wstring name(m_wrap->getFileNameBuf());
                selectedFilesRef.push_back(name);
            }
        }

        if (0 == selectedFilesRef.size()) {
            m_completion->didChooseFile(blink::WebVector<blink::WebFileChooserCompletion::SelectedFileInfo>());
            return;
        }

        blink::WebVector<blink::WebFileChooserCompletion::SelectedFileInfo> wsFileNames(selectedFilesRef.size());
        for (size_t i = 0; i < selectedFilesRef.size(); ++i) {
            blink::WebFileChooserCompletion::SelectedFileInfo info;
            const std::wstring& filePath = selectedFilesRef[i];

            info.path = String(filePath.c_str());
            //info.displayName = blink::Platform::current()->fileUtilities()->baseName(info.path);
            const std::wstring& fileSystemURL = L"file:///" + filePath;
            if (m_wrap->isDirectory())
                info.path = String(filePath.c_str());
            else
                info.fileSystemURL = blink::KURL(blink::ParsedURLString, String(fileSystemURL.c_str()));            

            long long fileSizeResult = 0;
            if (!blink::getFileSize(filePath.c_str(), fileSizeResult))
                fileSizeResult = 0;
            
            info.modificationTime = 0;
            info.length = fileSizeResult;
            info.isDirectory = ::PathIsDirectoryW(filePath.c_str());

            wsFileNames[i] = info;
        }
        m_completion->didChooseFile(wsFileNames);
    }

private:
    blink::WebFileChooserCompletion* m_completion;
    int m_webviewId;
    OpenFileNameWrap* m_wrap;
    BOOL m_retVal;
    std::function<void(void)>* m_completionCallback;
};

static DWORD __stdcall getOpenFileNameThread(void* param)
{
    std::function<void(void)>* callback = (std::function<void(void)>*)param;
    (*callback)();

    int count = 0;
    MSG msg = { 0 };
    while (true) {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }
        if (++count > 100)
            break;
        ::Sleep(5);
    }

    delete callback;
    return 0;
}

static HWND createHideWindow()
{
    WCHAR* s_fileChooserClassName = L"ChooserClass";
    static bool hasRegister = false;
    if (!hasRegister) {
        WNDCLASS wc = { 0 };
        wc.style = 0;
        wc.lpfnWndProc = ::DefWindowProcW;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = NULL;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = s_fileChooserClassName;
        ::RegisterClass(&wc);
        hasRegister = true;
    }
    
    HWND hWnd = ::CreateWindowW(s_fileChooserClassName, s_fileChooserClassName, WS_OVERLAPPEDWINDOW, 2, 2, 1, 1, NULL, NULL, NULL, NULL);

    ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    ::ShowWindow(hWnd, SW_HIDE);

    return hWnd;
}

static bool runFileChooserImpl(HWND hWnd, const blink::WebFileChooserParams& params, blink::WebFileChooserCompletion* completion, int webviewId, std::function<void(void)>* completionCallback)
{
    //////////////////////////////////////////////////////////////////////////
    //return runFileChooserTest(params, completion);
    //////////////////////////////////////////////////////////////////////////

    OpenFileNameWrap* wrap = new OpenFileNameWrap(params);

    bool saveAs = params.saveAs;
    bool multiSelect = params.multiSelect;

    std::function<void(void)>* callback = new std::function<void(void)>([hWnd, wrap, saveAs, multiSelect, completion, webviewId, completionCallback] {
        BOOL retVal = FALSE;
        HWND hwndOwner = hWnd; //  createHideWindow(); // ::GetActiveWindow();
        
        if (wrap->isDirectory()) {
            LPITEMIDLIST pil = NULL;
            INITCOMMONCONTROLSEX InitCtrls = { 0 };
            BROWSEINFO bi = { 0 };
            bi.hwndOwner = hwndOwner;
            bi.iImage = 0;
            bi.lParam = NULL;
            bi.lpfn = NULL;
            bi.lpszTitle = L"请选择文件路径";
            bi.pszDisplayName = wrap->getInfo()->lpstrFile;
            bi.ulFlags = BIF_BROWSEINCLUDEFILES;

            //InitCommonControlsEx(&InitCtrls); // 在调用函数SHBrowseForFolder之前需要调用该函数初始化相关环境
            pil = ::SHBrowseForFolder(&bi);
            if (NULL != pil) {
                ::SHGetPathFromIDList(pil, wrap->getInfo()->lpstrFile);//获取用户选择的文件路径
                retVal = TRUE;
            }
        } else {
            wrap->getInfo()->hwndOwner = hwndOwner;

            if (saveAs) {
                wrap->getInfo()->Flags = OFN_OVERWRITEPROMPT;
                retVal = ::GetSaveFileNameW(wrap->getInfo());
            } else if (multiSelect) {
                wrap->getInfo()->Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
                retVal = ::GetOpenFileNameW(wrap->getInfo());
            } else {
                wrap->getInfo()->Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
                retVal = ::GetOpenFileNameW(wrap->getInfo());
            }
        }

        blink::Platform::current()->mainThread()->postTask(FROM_HERE, new ResponseOpenFileInfoTask(completion, webviewId, wrap, retVal, completionCallback));
    });

    DWORD threadId = 0;
    HANDLE threadHandle = CreateThread(0, 0, getOpenFileNameThread, callback, 0, &threadId);
    ::CloseHandle(threadHandle);
    return true;
}

}

#endif // content_browser_RunFileChooser_h