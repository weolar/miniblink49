
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
#include <vector>
#include <shlwapi.h>

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

static bool runFileChooserImpl(const blink::WebFileChooserParams& params, blink::WebFileChooserCompletion* completion)
{
    // image/gif, image/jpeg, image/*
    // Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0
    std::vector<char> filter;
    HashSet<String> extsNoRepeat;
    for (size_t i = 0; i < params.acceptTypes.size(); ++i) {
        String mimeType = params.acceptTypes[0];

        WebMimeRegistryImpl* mimeRegistry = (WebMimeRegistryImpl*)blink::Platform::current()->mimeRegistry();
        Vector<blink::WebString> exts = mimeRegistry->extensionsForMimeType(mimeType);
        for (size_t j = 0; j <exts.size(); ++j) {
            String ext = exts[j];
            extsNoRepeat.add(ext);
        }
    }

    for (HashSet<String>::iterator it = extsNoRepeat.begin(); it != extsNoRepeat.end(); ++it) {
        std::string description = "*.";
        description += it->utf8().data();
        appendStringToVector(&filter, description);
        filter.push_back('\0');

        appendStringToVector(&filter, description);
        filter.push_back('\0');
    }
    
    appendStringToVector(&filter, "All Files");
    filter.push_back('\0');
    appendStringToVector(&filter, "*.*");
    filter.push_back('\0');
    filter.push_back('\0');

    String title = params.title;
    if (title.isEmpty())
        title = "Select File";

    std::vector<wchar_t> fileNameBuf;
    const int fileNameBufLen = 8192;
    fileNameBuf.resize(fileNameBufLen);
    memset(&fileNameBuf[0], 0, sizeof(wchar_t) * fileNameBufLen);

    String initialValue = params.initialValue;
    Vector<UChar> initialValueBuf;
    if (!initialValue.isNull() && !initialValue.isEmpty()) {
        initialValueBuf = WTF::ensureUTF16UChar(initialValue, true);
        if (initialValueBuf.size() < fileNameBufLen - 1)
            wcscpy(&fileNameBuf[0], initialValueBuf.data());
    }

    filter.push_back('\0');
    std::vector<UChar> filterW;
    WTF::MByteToWChar(&filter[0], filter.size() - 1, &filterW, CP_UTF8);

    Vector<UChar> titleBuf = WTF::ensureUTF16UChar(title, true);

    OPENFILENAMEW ofn = { 0 };
    ofn.lStructSize = sizeof(OPENFILENAMEW);
    ofn.hwndOwner = nullptr;
    ofn.hInstance = nullptr;
    ofn.lpstrFilter = &filterW[0];
    ofn.lpstrFile = &fileNameBuf[0];
    ofn.nMaxFile = fileNameBufLen - 2;
    ofn.lpstrTitle = titleBuf.data();
    ofn.Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_NOCHANGEDIR;

    BOOL retVal = FALSE;
    if (params.saveAs) {
        ofn.Flags = OFN_OVERWRITEPROMPT;
        retVal = ::GetSaveFileNameW(&ofn);
    } else if (params.multiSelect) {
        ofn.Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
        retVal = ::GetOpenFileNameW(&ofn);
    } else {
        ofn.Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
        retVal = ::GetOpenFileNameW(&ofn);
    }

    if (!retVal)
        return false;

    std::vector<std::wstring> selectedFiles;
    std::vector<std::wstring> selectedFilesRef;

    if (retVal) {
        // Figure out if the user selected multiple files.  If fileNameBuf is
        // a directory, then multiple files were selected!
        if ((ofn.Flags & OFN_ALLOWMULTISELECT) && (::GetFileAttributesW(&fileNameBuf[0]) & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring dirName = &fileNameBuf[0];
            const wchar_t* p = &fileNameBuf[0] + wcslen(&fileNameBuf[0]) + 1;
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
            selectedFilesRef.push_back(std::wstring(&fileNameBuf[0]));
        }
    }

    if (0 == selectedFilesRef.size())
        return false;

    blink::WebVector<blink::WebFileChooserCompletion::SelectedFileInfo> wsFileNames(selectedFilesRef.size());
    for (size_t i = 0; i < selectedFilesRef.size(); ++i) {
        blink::WebFileChooserCompletion::SelectedFileInfo info;
        const std::wstring& filePath = selectedFilesRef[i];
        
        info.path = String(filePath.c_str());
        info.displayName = blink::Platform::current()->fileUtilities()->baseName(info.path);
        const std::wstring& fileSystemURL = L"file:///" + filePath;
        info.fileSystemURL = blink::KURL(blink::ParsedURLString, String(fileSystemURL.c_str()));

        long long fileSizeResult = 0;
        if (!getFileSize(filePath.c_str(), fileSizeResult))
            fileSizeResult = 0;
        info.modificationTime = 0;
        info.length = fileSizeResult;
        info.isDirectory = ::PathIsDirectoryW(filePath.c_str());

        wsFileNames[i] = info;
    }
    completion->didChooseFile(wsFileNames);

    return true;
}

}

#endif // content_browser_RunFileChooser_h