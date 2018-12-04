
#include "content/web_impl_win/WebFileUtilitiesImpl.h"

#include "third_party/WebKit/public/platform/WebFileInfo.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/wtf/CryptographicallyRandomNumber.h"

#define PURE = 0

#include <windows.h>
#include <Shlwapi.h>

namespace content {

static const ULONGLONG kSecondsFromFileTimeToTimet = 11644473600;

static bool getFindData(String path, WIN32_FIND_DATAW& findData)
{
    Vector<UChar> upath = WTF::ensureUTF16UChar(path, true);
    HANDLE handle = ::FindFirstFileW(upath.data(), &findData);
    if (handle == INVALID_HANDLE_VALUE)
        return false;
    ::FindClose(handle);
    return true;
}

String pathByAppendingComponent(const String& path, const String& component)
{
    Vector<UChar> buffer(MAX_PATH);

    if (path.length() + 1 > buffer.size())
        return String();

    //StringView(path).getCharactersWithUpconvert(buffer.data());
    Vector<UChar> path16 = WTF::ensureUTF16UChar(path, true);
    for (size_t i = 0; i < path.length(); ++i) {
        buffer[i] = path16[i];
    }
    //buffer[path.length()] = '\0';

    if (!PathAppendW(buffer.data(), component.charactersWithNullTermination().data()))
        return String();

    buffer.shrink(wcslen(buffer.data()));

    return String(buffer.data(), buffer.size());
}

String openTemporaryFile(const String&, HANDLE& handle)
{
    handle = INVALID_HANDLE_VALUE;

    wchar_t tempPath[MAX_PATH];
    int tempPathLength = ::GetTempPathW(WTF_ARRAY_LENGTH(tempPath), tempPath);
    if (tempPathLength <= 0 || tempPathLength > WTF_ARRAY_LENGTH(tempPath))
        return String();

    String proposedPath;
    do {
        wchar_t tempFile[] = L"XXXXXXXX.tmp"; // Use 8.3 style name (more characters aren't helpful due to 8.3 short file names)
        const int randomPartLength = 8;
        WTF::cryptographicallyRandomValues(tempFile, randomPartLength * sizeof(wchar_t));

        // Limit to valid filesystem characters, also excluding others that could be problematic, like punctuation.
        // don't include both upper and lowercase since Windows file systems are typically not case sensitive.
        const char validChars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < randomPartLength; ++i)
            tempFile[i] = validChars[tempFile[i] % (sizeof(validChars) - 1)];

        ASSERT(wcslen(tempFile) == WTF_ARRAY_LENGTH(tempFile) - 1);

        proposedPath = pathByAppendingComponent(tempPath, tempFile);
        if (proposedPath.isEmpty())
            break;

        // use CREATE_NEW to avoid overwriting an existing file with the same name
        handle = ::CreateFileW(proposedPath.charactersWithNullTermination().data(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
    } while (!(INVALID_HANDLE_VALUE == handle) && GetLastError() == ERROR_ALREADY_EXISTS);

    if (!(INVALID_HANDLE_VALUE != handle))
        return String();

    return proposedPath;
}

int writeToFile(HANDLE handle, const char* data, int length)
{
    if (!(INVALID_HANDLE_VALUE != handle))
        return -1;

    DWORD bytesWritten;
    bool success = ::WriteFile(handle, data, length, &bytesWritten, 0);

    if (!success)
        return -1;
    return static_cast<int>(bytesWritten);
}

String pathGetFileName(const String& path)
{
    return String(::PathFindFileNameW(WTF::ensureUTF16UChar(path, true).data()));
}

bool fileExists(const String& path)
{
    WIN32_FIND_DATAW findData;
    return getFindData(path, findData);
}

static void getFileModificationTimeFromFindData(const WIN32_FIND_DATAW& findData, time_t& time)
{
    ULARGE_INTEGER fileTime;
    fileTime.HighPart = findData.ftLastWriteTime.dwHighDateTime;
    fileTime.LowPart = findData.ftLastWriteTime.dwLowDateTime;

    // Information about converting time_t to FileTime is available at http://msdn.microsoft.com/en-us/library/ms724228%28v=vs.85%29.aspx
    time = fileTime.QuadPart / 10000000 - kSecondsFromFileTimeToTimet;
}

static bool getFileSizeFromFindData(const WIN32_FIND_DATAW& findData, long long& size)
{
    ULARGE_INTEGER fileSize;
    fileSize.HighPart = findData.nFileSizeHigh;
    fileSize.LowPart = findData.nFileSizeLow;

    if (fileSize.QuadPart > static_cast<ULONGLONG>(std::numeric_limits<long long>::max()))
        return false;

    size = fileSize.QuadPart;
    return true;
}

WebFileUtilitiesImpl::WebFileUtilitiesImpl()
{

}

bool WebFileUtilitiesImpl::getFileInfo(const blink::WebString& path, blink::WebFileInfo& result)
{
    WIN32_FIND_DATAW findData;
    if (!getFindData(path, findData))
        return false;

    time_t time;
    getFileModificationTimeFromFindData(findData, time);
    result.modificationTime = (double)time;

    getFileSizeFromFindData(findData, result.length);

    result.type = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? blink::WebFileInfo::TypeDirectory : blink::WebFileInfo::TypeFile;
    result.platformPath = path;

    return true;
}

blink::WebString WebFileUtilitiesImpl::directoryName(const blink::WebString& path) 
{
    String pathString(path);
    String name = pathString.left(pathString.length() - pathGetFileName(pathString).length());
    if (name.characterStartingAt(name.length() - 1) == '\\') {
        // Remove any trailing "\".
        name.truncate(name.length() - 1);
    }
    return name;
}

blink::WebString WebFileUtilitiesImpl::baseName(const blink::WebString& path)
{
    if (path.isNull() || path.isEmpty())
        return "";
    Vector<UChar> result = WTF::ensureUTF16UChar(path, true);
    if (result.isEmpty())
        return "";
    ::PathStripPathW(result.data());
    return String(result.data());
}

bool WebFileUtilitiesImpl::isDirectory(const blink::WebString& path)
{ 
    return ::PathIsDirectoryW(WTF::ensureUTF16UChar(path, true).data());
}

blink::WebURL WebFileUtilitiesImpl::filePathToURL(const blink::WebString& path)
{
    blink::KURL result;
    String temp = "file:///";
    temp.append(WTF::ensureUTF16String(path));
    temp.replace("%", "%25");
    temp.replace(";", "%3B");
    temp.replace("#", "%23");
    temp.replace("?", "%3F");
    return blink::KURL(blink::ParsedURLString, temp);
}

}