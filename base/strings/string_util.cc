
#include "base/strings/string_util.h"

#include "wtf/text/WTFStringUtil.h"

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

std::wstring UTF8ToWide(const std::string& src) {
    std::wstring output;
    WTF::Vector<UChar> out = WTF::ensureUTF16UChar(WTF::String(src.c_str(), src.size()));
    output.append((const wchar_t*)out.data(), out.size());
    return output;
}

std::wstring ASCIIToWide(const std::string& ascii) {
    //DCHECK(base::IsStringASCII(ascii)) << ascii;
    WTF::String str(ascii.data(), ascii.size());
    Vector<UChar> ustring = WTF::ensureUTF16UChar(str);
    return std::wstring(ustring.data(), ustring.size());
}

std::string UTF16ToASCII(const string16& utf16) {
    //DCHECK(IsStringASCII(utf16)) << UTF16ToUTF8(utf16);
    WTF::String str(utf16.data(), utf16.size());
    CString ascii = str.ascii();
    return std::string(ascii.data());
}

}