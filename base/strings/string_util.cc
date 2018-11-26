
#include "base/strings/string_util.h"

#include <windows.h>
#include <vector>

namespace base {

const wchar_t* kWhitespaceWide = L" ";
const char16* kWhitespaceUTF16 = L" ";
const char* kWhitespaceASCII = " ";

template<typename STR>
TrimPositions TrimStringT(const STR& input,
    const STR& trim_chars,
    TrimPositions positions,
    STR* output) {
    // Find the edges of leading/trailing whitespace as desired.
    const size_t last_char = input.length() - 1;
    const size_t first_good_char = (positions & TRIM_LEADING) ?
        input.find_first_not_of(trim_chars) : 0;
    const size_t last_good_char = (positions & TRIM_TRAILING) ?
        input.find_last_not_of(trim_chars) : last_char;

    // When the string was all whitespace, report that we stripped off whitespace
    // from whichever position the caller was interested in.  For empty input, we
    // stripped no whitespace, but we still need to clear |output|.
    if (input.empty() ||
        (first_good_char == STR::npos) || (last_good_char == STR::npos)) {
        bool input_was_empty = input.empty();  // in case output == &input
        output->resize(0);
        return input_was_empty ? TRIM_NONE : positions;
    }

    // Trim the whitespace.
    *output = input.substr(first_good_char, last_good_char - first_good_char + 1);

    // Return where we trimmed from.
    return static_cast<TrimPositions>(((first_good_char == 0) ? TRIM_NONE : TRIM_LEADING) | ((last_good_char == last_char) ? TRIM_NONE : TRIM_TRAILING));
}

TrimPositions TrimWhitespaceASCII(const std::string& input, TrimPositions positions, std::string* output) {
    std::string whitespaceASCII(" ");
    return TrimStringT(input, whitespaceASCII, positions, output);
}

TrimPositions TrimWhitespace(const string16& input, TrimPositions positions, string16* output) {
    return TrimStringT(input, base::string16(kWhitespaceUTF16), positions, output);
}

// This function is only for backward-compatibility.
// To be removed when all callers are updated.
TrimPositions TrimWhitespace(const std::string& input, TrimPositions positions, std::string* output) {
    return TrimWhitespaceASCII(input, positions, output);
}

std::string ToLowerASCII(const std::string& str) {
    std::string ret;
    ret.reserve(str.size());
    for (size_t i = 0; i < str.size(); i++)
        ret += (ToLowerASCII(str[i]));
    return ret;
}

std::string ToUpperASCII(const std::string& str) {
    std::string ret;
    ret.reserve(str.size());
    for (size_t i = 0; i < str.size(); i++)
        ret += (ToUpperASCII(str[i]));
    return ret;
}

std::wstring UTF8ToWide(const std::string& utf8) {
    std::wstring utf16;
    size_t n = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), nullptr, 0);
    if (0 == n)
        return L"";
    std::vector<wchar_t> wbuf(n);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), utf8.size(), &wbuf[0], n);
    utf16.resize(n + 5);
    utf16.assign(&wbuf[0], n);
    return utf16;
}

std::wstring ASCIIToWide(const std::string& ascii) {
    return UTF8ToWide(ascii);
}

std::string UTF16ToASCII(const string16& utf16) {
    return WideToUTF8(utf16);
}

std::string WideToUTF8(const string16& utf16) {
    std::string utf8;
    size_t n = ::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), utf16.size(), NULL, 0, NULL, NULL);
    if (0 == n)
        return "";
    std::vector<char> buf(n + 1);
    ::WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &buf[0], n, NULL, NULL);
    utf8.resize(n);
    utf8.assign(&buf[0], n);
    return utf8;
}

std::string UTF16ToUTF8(const string16& utf16) {
    return WideToUTF8(utf16);
}

}