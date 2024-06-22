
#ifndef content_ui_SaveImage_h
#define content_ui_SaveImage_h

#include "content/browser/PostTaskHelper.h"
#include "content/browser/WebPageImpl.h"
#include "net/ActivatingObjCheck.h"
#include "wke/wkeWebView.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/image-encoders/skia/PNGImageEncoder.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/Atomics.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include <string>
#include <xstring>
#include <process.h>

namespace content {

extern WebPageImpl* g_saveImageingWebPage;

static Vector<UChar> getSaveImageFromUrl(const std::string& url)
{
    const wchar_t defaultPathW[] = L"unknow.png";
    Vector<UChar> defaultPath;
    defaultPath.resize(sizeof(defaultPathW) / sizeof(wchar_t));
    wcscpy(defaultPath.data(), defaultPathW);
    
    if (0 == url.size())
        return defaultPath;

    size_t pos1 = url.find_last_of('\\');
    if (std::string::npos == pos1)
        pos1 = url.size() - 1;
    else
        pos1++;

    size_t pos2 = url.find_last_of('/');
    if (std::string::npos == pos2)
        pos2 = url.size() - 1;
    else
        pos2++;

    size_t pos = pos1 < pos2 ? pos1 : pos2;
    if (std::string::npos == pos || url.size() - 1 == pos)
        pos = 0;

    size_t pos3 = url.find('?', pos);
    if (std::string::npos == pos3)
        pos3 = url.size();

    if (pos3 < pos)
        pos3 = url.size();
    std::string path = url.substr(pos, pos3 - pos);

    if (path.size() > 256)
        return defaultPath;

    return ensureUTF16UChar(String(path.c_str(), path.size()), true);
}

struct SaveImageInfo {
    Vector<unsigned char> imageData;
    Vector<UChar> url;
    int id;
    HWND hwnd;
};

static int s_saveImageCount = 0;

static DWORD __stdcall saveImageThread(void* param)
{
    const wchar_t* filter = L"Image Files(*.gif;*.jpeg;*.png)\0*.gif;*.jpeg;*.png\0All Files(*.*)\0*.*\0\0";
    Vector<wchar_t> fileNameBuf;
    const int fileNameBufLen = 8192;
    fileNameBuf.resize(fileNameBufLen);

    SaveImageInfo* info = (SaveImageInfo*)(param);
    wcscpy(fileNameBuf.data(), (LPWSTR)info->url.data());

    OPENFILENAMEW ofn;
    memset(&ofn, 0, sizeof(OPENFILENAMEW));
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = info->hwnd;
    ofn.hInstance = nullptr;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = fileNameBuf.data();
    ofn.nMaxFile = fileNameBufLen - 2;
    ofn.lpstrTitle = (LPWSTR)L"Í¼Æ¬Áí´æÎª...";
    ofn.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
    BOOL retVal = ::GetSaveFileNameW(&ofn);

    if (retVal) {
        HANDLE fileHandle = ::CreateFileW((LPCWSTR)fileNameBuf.data(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (fileHandle && INVALID_HANDLE_VALUE != fileHandle) {
            DWORD numberOfBytesWrite = 0;
            retVal = ::WriteFile(fileHandle, info->imageData.data(), info->imageData.size(), &numberOfBytesWrite, nullptr);
            ::CloseHandle(fileHandle);
        }
    }
    
    postTaskToMainThread(FROM_HERE, [info] {
        if (net::ActivatingObjCheck::inst()->isActivating(info->id))
            g_saveImageingWebPage->setIsMouseKeyMessageEnable(true);

        delete info;
        atomicDecrement(&s_saveImageCount);
        g_saveImageingWebPage = nullptr;
    });

    return 0;
}

bool saveImage(const blink::WebImage& image, const blink::WebURL& url)
{
    if (!g_saveImageingWebPage)
        return false;

    if (s_saveImageCount > 0)
        return true;
    atomicIncrement(&s_saveImageCount);

    SaveImageInfo* info = new SaveImageInfo();

    String urlString = ((blink::KURL)url).getUTF8String();
    info->url = getSaveImageFromUrl(urlString.utf8().data());
    info->hwnd = g_saveImageingWebPage->m_hWnd;
    const SkBitmap& bitmap = image.getSkBitmap();
    blink::PNGImageEncoder::encode(bitmap, &info->imageData);
    if (0 == info->imageData.size() || info->url.size() <= 1) {
        delete info;
        return true;
    }

    g_saveImageingWebPage->setIsMouseKeyMessageEnable(false);
    info->id = g_saveImageingWebPage->wkeWebView()->getId();

    DWORD threadId = 0;
    HANDLE threadHandle = CreateThread(0, 0, saveImageThread, info, 0, &threadId);
    ::CloseHandle(threadHandle);
    return true;
}

}

#endif // content_ui_SaveImage_h