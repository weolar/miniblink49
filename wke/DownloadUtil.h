
#ifndef wke_DownloadUtil_h
#define wke_DownloadUtil_h

#include "base/strings/string_util.h"
#include "wtf/text/WTFStringUtil.h"

static std::wstring getSaveNameFromUrl(const std::string& url)
{
    if (0 == url.size())
        return L"";

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
    return base::UTF8ToWide(path);
}

static unsigned char fromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z')
        y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z')
        y = x - 'a' + 10;
    else if (x >= '0' && x <= '9')
        y = x - '0';
    else
        DebugBreak();
    return y;
}

static std::string urlDecode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) {
        if (str[i] == '+')
            strTemp += ' ';
        else if (str[i] == '%') {
            if (i + 2 >= length)
                break;
            unsigned char high = fromHex((unsigned char)str[++i]);
            unsigned char low = fromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        } else
            strTemp += str[i];
    }
    return strTemp;
}

// attachment; filename="11??1.docx"; filename*=UTF-8''11%E6%B5%8B%E8%AF%951.docx
static std::wstring getSaveNameFromContentDisposition(const std::string& str)
{
    if (0 == str.size())
        return L"";

    if (std::string::npos == str.find("attachment"))
        return L"";

    if (std::string::npos == str.find("name"))
        return L"";

    size_t pos1 = str.find("UTF-8''");
    if (std::string::npos == pos1) {
        pos1 = str.find('=');
        if (std::string::npos == pos1)
            return L"";
        pos1++;
    } else {
        pos1 += 7; // UTF-8''
    }

    size_t pos2 = str.size();
    std::string path = str.substr(pos1, pos2 - pos1);
    if (path.empty())
        return L"";

    if ('"' == path[0])
        path.erase(0 , 1);
    if (path.empty())
        return L"";
    if ('"' == path[path.size() - 1])
        path.erase(path.size() - 1, 1);
    if (path.empty())
        return L"";

    path = urlDecode(path);
    //return common::mulByteToUtf16(path, WTF::isTextUTF8(path.c_str(), path.size()) ? CP_UTF8 : CP_ACP);
    if (WTF::isTextUTF8(path.c_str(), path.size()))
        return base::UTF8ToWide(path);
    else
        return base::ASCIIToWide(path);
}

static std::wstring getSaveName(std::string contentDisposition, std::string url)
{
    std::wstring result = getSaveNameFromContentDisposition(contentDisposition);
    if (!result.empty())
        return result;
    result = getSaveNameFromUrl(url);

    std::string resultUtf8 = base::UTF16ToUTF8(result.c_str());

    const utf8* str = wkeUtilDecodeURLEscape(resultUtf8.c_str());
    std::string strUtf8(str);
    result = base::UTF8ToWide(strUtf8);

    return result;
}

#endif // download_DownloadUtil_h