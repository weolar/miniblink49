
#include "base/json/json_util.h"
#include "base/strings/string_util.h"
#include "base/strings/string_piece.h"

#include <vector>
#include <limits>
//#include "third_party/WebKit/Source/wtf/dtoa.h"

// namespace WTF {
// double parseDouble(const unsigned char* string, size_t length, size_t& parsedLength);
// }

namespace base {

std::string IntToString(int value)
{
    char buffer[32] = { 0 };
    sprintf(buffer, "%d", value);
    return std::string(buffer);
}

std::string UintToString(unsigned int value)
{
    char buffer[32] = { 0 };
    sprintf(buffer, "%u", value);
    return std::string(buffer);
}

std::string Int64ToString(int64 value)
{
    char buffer[32] = { 0 };
    sprintf(buffer, "%I64d", value);
    return std::string(buffer);
}

std::string DoubleToString(double value)
{
    // According to g_fmt.cc, it is sufficient to declare a buffer of size 32.
    char buffer[32];
    sprintf(buffer, "%f", (float)value);
    return std::string(buffer);
}

// Format string for printing a \uXXXX escape sequence.
const char kU16EscapeFormat[] = "\\u%04X";

// The code point to output for an invalid input code unit.
const uint32 kReplacementCodePoint = 0xFFFD;

// Used below in EscapeSpecialCodePoint().
COMPILE_ASSERT('<' == 0x3C, less_than_sign_is_0x3c);

// Try to escape the |code_point| if it is a known special character. If
// successful, returns true and appends the escape sequence to |dest|. This
// isn't required by the spec, but it's more readable by humans.
bool EscapeSpecialCodePoint(uint32 code_point, std::string* dest)
{
    // WARNING: if you add a new case here, you need to update the reader as well.
    // Note: \v is in the reader, but not here since the JSON spec doesn't
    // allow it.
    switch (code_point) {
    case '\b':
        dest->append("\\b");
        break;
    case '\f':
        dest->append("\\f");
        break;
    case '\n':
        dest->append("\\n");
        break;
    case '\r':
        dest->append("\\r");
        break;
    case '\t':
        dest->append("\\t");
        break;
    case '\\':
        dest->append("\\\\");
        break;
    case '"':
        dest->append("\\\"");
        break;
        // Escape < to prevent script execution; escaping > is not necessary and
        // not doing so save a few bytes.
    case '<':
        dest->append("\\u003C");
        break;
        // Escape the "Line Separator" and "Paragraph Separator" characters, since
        // they should be treated like a new line \r or \n.
    case 0x2028:
        dest->append("\\u2028");
        break;
    case 0x2029:
        dest->append("\\u2029");
        break;
    default:
        return false;
    }
    return true;
}

const uint8 utf8_countTrailBytes[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3,
    3, 3, 3,    /* illegal in Unicode */
    4, 4, 4, 4, /* illegal in Unicode */
    5, 5,       /* illegal in Unicode */
    0, 0        /* illegal bytes 0xfe and 0xff */
};

static const int32_t utf8_minLegal[4] = { 0, 0x80, 0x800, 0x10000 };

static const int32_t utf8_errorValue[6] = {
    CBUTF8_ERROR_VALUE_1, CBUTF8_ERROR_VALUE_2, CBUTF_ERROR_VALUE, 0x10ffff,
    0x3ffffff, 0x7fffffff
};

int32_t utf8_nextCharSafeBody(const uint8 *s, int32 *pi, int32 length, int32_t c, int strict)
{
    int32 i = *pi;
    uint8 count = CBU8_COUNT_TRAIL_BYTES(c);
    if ((i)+count <= (length)) {
        uint8 trail, illegal = 0;

        CBU8_MASK_LEAD_BYTE((c), count);
        /* count==0 for illegally leading trail bytes and the illegal bytes 0xfe and 0xff */
        switch (count) {
            /* each branch falls through to the next one */
        case 5:
        case 4:
            /* count>=4 is always illegal: no more than 3 trail bytes in Unicode's UTF-8 */
            illegal = 1;
            break;
        case 3:
            trail = s[(i)++];
            (c) = ((c) << 6) | (trail & 0x3f);
            if (c<0x110) {
                illegal |= (trail & 0xc0) ^ 0x80;
            } else {
                /* code point>0x10ffff, outside Unicode */
                illegal = 1;
                break;
            }
        case 2:
            trail = s[(i)++];
            (c) = ((c) << 6) | (trail & 0x3f);
            illegal |= (trail & 0xc0) ^ 0x80;
        case 1:
            trail = s[(i)++];
            (c) = ((c) << 6) | (trail & 0x3f);
            illegal |= (trail & 0xc0) ^ 0x80;
            break;
        case 0:
            if (strict >= 0) {
                return CBUTF8_ERROR_VALUE_1;
            } else {
                return CBU_SENTINEL;
            }
            /* no default branch to optimize switch()  - all values are covered */
        }

        /*
        * All the error handling should return a value
        * that needs count bytes so that UTF8_GET_CHAR_SAFE() works right.
        *
        * Starting with Unicode 3.0.1, non-shortest forms are illegal.
        * Starting with Unicode 3.2, surrogate code points must not be
        * encoded in UTF-8, and there are no irregular sequences any more.
        *
        * U8_ macros (new in ICU 2.4) return negative values for error conditions.
        */

        /* correct sequence - all trail bytes have (b7..b6)==(10)? */
        /* illegal is also set if count>=4 */
        if (illegal || (c)<utf8_minLegal[count] || (CBU_IS_SURROGATE(c) && strict != -2)) {
            /* error handling */
            uint8 errorCount = count;
            /* don't go beyond this sequence */
            i = *pi;
            while (count>0 && CBU8_IS_TRAIL(s[i])) {
                ++(i);
                --count;
            }
            if (strict >= 0) {
                c = utf8_errorValue[errorCount - count];
            } else {
                c = CBU_SENTINEL;
            }
        } else if ((strict)>0 && CBU_IS_UNICODE_NONCHAR(c)) {
            /* strict: forbid non-characters like U+fffe */
            c = utf8_errorValue[count];
        }
    } else /* too few bytes left */ {
        /* error handling */
        int32 i0 = i;
        /* don't just set (i)=(length) in case there is an illegal sequence */
        while ((i)<(length) && CBU8_IS_TRAIL(s[i])) {
            ++(i);
        }
        if (strict >= 0) {
            c = utf8_errorValue[i - i0];
        } else {
            c = CBU_SENTINEL;
        }
    }
    *pi = i;
    return c;
}

bool ReadUnicodeCharacter(const char* src, int32 src_len, int32* char_index, uint32* code_point_out)
{
    // U8_NEXT expects to be able to use -1 to signal an error, so we must
    // use a signed type for code_point.  But this function returns false
    // on error anyway, so code_point_out is unsigned.
    int32 code_point;
    CBU8_NEXT(src, *char_index, src_len, code_point);
    *code_point_out = static_cast<uint32>(code_point);

    // The ICU macro above moves to the next char, we want to point to the last
    // char consumed.
    (*char_index)--;

    // Validate the decoded value.
    return IsValidCodepoint(code_point);
}

size_t WriteUnicodeCharacter(uint32 code_point, std::string* output)
{
    if (code_point <= 0x7f) {
        // Fast path the common case of one byte.
        *output += (static_cast<char>(code_point));
        return 1;
    }

    // CBU8_APPEND_UNSAFE can append up to 4 bytes.
    size_t char_offset = output->length();
    size_t original_char_offset = char_offset;
    output->resize(char_offset + CBU8_MAX_LENGTH);

    CBU8_APPEND_UNSAFE(&(*output)[0], char_offset, code_point);

    // CBU8_APPEND_UNSAFE will advance our pointer past the inserted character, so
    // it will represent the new length of the string.
    output->resize(char_offset);
    return char_offset - original_char_offset;
}

inline int baseVsnprintf(char* buffer, size_t size,
    const char* format, va_list arguments)
{
    //int length = vsnprintf_s(buffer, size, size - 1, format, arguments);
    int length = _vsnprintf(buffer, size - 1, format, arguments);
    if (length < 0)
        return _vscprintf(format, arguments);
    return length;
}

inline int vsnprintfT(char* buffer,
    size_t buf_size,
    const char* format,
    va_list argptr)
{
    return baseVsnprintf(buffer, buf_size, format, argptr);
}

#ifndef va_copy
#define va_copy(destination, source) ((destination) = (source))
#endif

// Templatized backend for StringPrintF/StringAppendF. This does not finalize
// the va_list, the caller is expected to do that.
template <class StringType>
static void StringAppendVT(StringType* dst,
    const typename StringType::value_type* format,
    va_list ap)
{
    // First try with a small fixed size buffer.
    // This buffer size should be kept in sync with StringUtilTest.GrowBoundary
    // and StringUtilTest.StringPrintfBounds.
    typename StringType::value_type stack_buf[1024];

    va_list ap_copy;
    va_copy(ap_copy, ap);

#if !defined(OS_WIN)
    ScopedClearErrno clear_errno;
#endif
    int result = vsnprintfT(stack_buf, arraysize(stack_buf), format, ap_copy);
    va_end(ap_copy);

    if (result >= 0 && result < static_cast<int>(arraysize(stack_buf))) {
        // It fit.
        dst->append(stack_buf, result);
        return;
    }

    // Repeatedly increase buffer size until it fits.
    int mem_length = arraysize(stack_buf);
    while (true) {
        if (result < 0) {
#if defined(OS_WIN)
            // On Windows, vsnprintfT always returns the number of characters in a
            // fully-formatted string, so if we reach this point, something else is
            // wrong and no amount of buffer-doubling is going to fix it.
            return;
#else
            if (errno != 0 && errno != EOVERFLOW)
                return;
            // Try doubling the buffer size.
            mem_length *= 2;
#endif
        } else {
            // We need exactly "result + 1" characters.
            mem_length = result + 1;
        }

        if (mem_length > 32 * 1024 * 1024) {
            // That should be plenty, don't try anything larger.  This protects
            // against huge allocations when using vsnprintfT implementations that
            // return -1 for reasons other than overflow without setting errno.
            //DLOG(WARNING) << "Unable to printf the requested string due to size.";
            return;
        }

        std::vector<typename StringType::value_type> mem_buf(mem_length);

        // NOTE: You can only use a va_list once.  Since we're in a while loop, we
        // need to make a new copy each time so we don't use up the original.
        va_copy(ap_copy, ap);
        result = vsnprintfT(&mem_buf[0], mem_length, format, ap_copy);
        va_end(ap_copy);

        if ((result >= 0) && (result < mem_length)) {
            // It fit.
            dst->append(&mem_buf[0], result);
            return;
        }
    }
}

void StringAppendV(std::string* dst, const char* format, va_list ap)
{
    StringAppendVT(dst, format, ap);
}

void StringAppendF(std::string* dst, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    StringAppendV(dst, format, ap);
    va_end(ap);
}

template <typename S>
bool EscapeJSONStringImpl(const S& str, bool put_in_quotes, std::string* dest)
{
    bool did_replacement = false;

    if (put_in_quotes)
        *dest += ('"');

    // Casting is necessary because ICU uses int32. Try and do so safely.
    if (str.length() > static_cast<size_t>(kint32max))
        *(int*)1 = 1;
    const int32 length = static_cast<int32>(str.length());

    for (int32 i = 0; i < length; ++i) {
        uint32 code_point;
        if (!ReadUnicodeCharacter(str.data(), length, &i, &code_point)) {
            code_point = kReplacementCodePoint;
            did_replacement = true;
        }

        if (EscapeSpecialCodePoint(code_point, dest))
            continue;

        // Escape non-printing characters.
        if (code_point < 32)
            StringAppendF(dest, kU16EscapeFormat, code_point);
        else
            WriteUnicodeCharacter(code_point, dest);
    }

    if (put_in_quotes)
        *dest += ('"');

    return !did_replacement;
}

bool EscapeJSONString(const std::string& str, bool put_in_quotes, std::string* dest)
{
    return EscapeJSONStringImpl(str, put_in_quotes, dest);
}


// Utility to convert a character to a digit in a given base
template<typename CHAR, int BASE, bool BASE_LTE_10> class BaseCharToDigit {
};

// Faster specialization for bases <= 10
template<typename CHAR, int BASE> class BaseCharToDigit<CHAR, BASE, true> {
public:
    static bool Convert(CHAR c, uint8* digit)
    {
        if (c >= '0' && c < '0' + BASE) {
            *digit = static_cast<uint8>(c - '0');
            return true;
        }
        return false;
    }
};

// Specialization for bases where 10 < base <= 36
template<typename CHAR, int BASE> class BaseCharToDigit<CHAR, BASE, false> {
public:
    static bool Convert(CHAR c, uint8* digit)
    {
        if (c >= '0' && c <= '9') {
            *digit = c - '0';
        } else if (c >= 'a' && c < 'a' + BASE - 10) {
            *digit = c - 'a' + 10;
        } else if (c >= 'A' && c < 'A' + BASE - 10) {
            *digit = c - 'A' + 10;
        } else {
            return false;
        }
        return true;
    }
};

template<int BASE, typename CHAR> bool CharToDigit(CHAR c, uint8* digit)
{
    return BaseCharToDigit<CHAR, BASE, BASE <= 10>::Convert(c, digit);
}

// There is an IsUnicodeWhitespace for wchars defined in string_util.h, but it
// is locale independent, whereas the functions we are replacing were
// locale-dependent. TBD what is desired, but for the moment let's not
// introduce a change in behaviour.
template<typename CHAR> class WhitespaceHelper {
};

template<> class WhitespaceHelper<char> {
public:
    static bool Invoke(char c)
    {
        return 0 != isspace(static_cast<unsigned char>(c));
    }
};

template<> class WhitespaceHelper<char16> {
public:
    static bool Invoke(char16 c)
    {
        return 0 != iswspace(c);
    }
};

template<typename CHAR> bool LocalIsWhitespace(CHAR c)
{
    return WhitespaceHelper<CHAR>::Invoke(c);
}

// IteratorRangeToNumberTraits should provide:
//  - a typedef for iterator_type, the iterator type used as input.
//  - a typedef for value_type, the target numeric type.
//  - static functions min, max (returning the minimum and maximum permitted
//    values)
//  - constant kBase, the base in which to interpret the input
template<typename IteratorRangeToNumberTraits>
class IteratorRangeToNumber {
public:
    typedef IteratorRangeToNumberTraits traits;
    typedef typename traits::iterator_type const_iterator;
    typedef typename traits::value_type value_type;

    // Generalized iterator-range-to-number conversion.
    //
    static bool Invoke(const_iterator begin,
        const_iterator end,
        value_type* output)
    {
        bool valid = true;

        while (begin != end && LocalIsWhitespace(*begin)) {
            valid = false;
            ++begin;
        }

        if (begin != end && *begin == '-') {
            if (!std::numeric_limits<value_type>::is_signed) {
                valid = false;
            } else if (!Negative::Invoke(begin + 1, end, output)) {
                valid = false;
            }
        } else {
            if (begin != end && *begin == '+') {
                ++begin;
            }
            if (!Positive::Invoke(begin, end, output)) {
                valid = false;
            }
        }

        return valid;
    }

private:
    // Sign provides:
    //  - a static function, CheckBounds, that determines whether the next digit
    //    causes an overflow/underflow
    //  - a static function, Increment, that appends the next digit appropriately
    //    according to the sign of the number being parsed.
    template<typename Sign>
    class Base {
    public:
        static bool Invoke(const_iterator begin, const_iterator end,
            typename traits::value_type* output)
        {
            *output = 0;

            if (begin == end) {
                return false;
            }

            // Note: no performance difference was found when using template
            // specialization to remove this check in bases other than 16
            if (traits::kBase == 16 && end - begin > 2 && *begin == '0' &&
                (*(begin + 1) == 'x' || *(begin + 1) == 'X')) {
                begin += 2;
            }

            for (const_iterator current = begin; current != end; ++current) {
                uint8 new_digit = 0;

                if (!CharToDigit<traits::kBase>(*current, &new_digit)) {
                    return false;
                }

                if (current != begin) {
                    if (!Sign::CheckBounds(output, new_digit)) {
                        return false;
                    }
                    *output *= traits::kBase;
                }

                Sign::Increment(new_digit, output);
            }
            return true;
        }
    };

    class Positive : public Base<Positive> {
    public:
        static bool CheckBounds(value_type* output, uint8 new_digit)
        {
            if (*output > static_cast<value_type>(traits::max() / traits::kBase) ||
                (*output == static_cast<value_type>(traits::max() / traits::kBase) &&
                    new_digit > traits::max() % traits::kBase)) {
                *output = traits::max();
                return false;
            }
            return true;
        }
        static void Increment(uint8 increment, value_type* output)
        {
            *output += increment;
        }
    };

    class Negative : public Base<Negative> {
    public:
        static bool CheckBounds(value_type* output, uint8 new_digit)
        {
            if (*output < traits::min() / traits::kBase ||
                (*output == traits::min() / traits::kBase &&
                    new_digit > 0 - traits::min() % traits::kBase)) {
                *output = traits::min();
                return false;
            }
            return true;
        }
        static void Increment(uint8 increment, value_type* output)
        {
            *output -= increment;
        }
    };
};

template<typename ITERATOR, typename VALUE, int BASE>
class BaseIteratorRangeToNumberTraits {
public:
    typedef ITERATOR iterator_type;
    typedef VALUE value_type;
    static value_type min()
    {
        return std::numeric_limits<value_type>::min();
    }
    static value_type max()
    {
        return std::numeric_limits<value_type>::max();
    }
    static const int kBase = BASE;
};

template<typename ITERATOR>
class BaseHexIteratorRangeToIntTraits
    : public BaseIteratorRangeToNumberTraits<ITERATOR, int, 16> {
};

template<typename ITERATOR>
class BaseHexIteratorRangeToUIntTraits
    : public BaseIteratorRangeToNumberTraits<ITERATOR, uint32, 16> {
};

template<typename ITERATOR>
class BaseHexIteratorRangeToInt64Traits
    : public BaseIteratorRangeToNumberTraits<ITERATOR, int64, 16> {
};

template<typename ITERATOR>
class BaseHexIteratorRangeToUInt64Traits
    : public BaseIteratorRangeToNumberTraits<ITERATOR, uint64, 16> {
};

typedef BaseHexIteratorRangeToIntTraits<StringPiece::const_iterator>
HexIteratorRangeToIntTraits;

typedef BaseHexIteratorRangeToUIntTraits<StringPiece::const_iterator>
HexIteratorRangeToUIntTraits;

typedef BaseHexIteratorRangeToInt64Traits<StringPiece::const_iterator>
HexIteratorRangeToInt64Traits;

typedef BaseHexIteratorRangeToUInt64Traits<StringPiece::const_iterator>
HexIteratorRangeToUInt64Traits;

bool HexStringToInt(const StringPiece& input, int* output)
{
    return IteratorRangeToNumber<HexIteratorRangeToIntTraits>::Invoke(
        input.begin(), input.end(), output);
}

template <typename VALUE, int BASE>
class StringPieceToNumberTraits
    : public BaseIteratorRangeToNumberTraits<StringPiece::const_iterator,
    VALUE,
    BASE> {
};

template <typename VALUE>
bool StringToIntImpl(const StringPiece& input, VALUE* output)
{
    return IteratorRangeToNumber<StringPieceToNumberTraits<VALUE, 10> >::Invoke(
        input.begin(), input.end(), output);
}

bool StringToInt(const StringPiece& input, int* output)
{
    return StringToIntImpl(input, output);
}

bool StringToDouble(const std::string& input, double* output)
{
    size_t parsedLength = 0;
//     double result = WTF::parseDouble((const unsigned char*)input.c_str(), input.size(), parsedLength);
//     if (0 == parsedLength)
//         return false;

    char* endptr = nullptr;
    double result = strtod(input.c_str(), &endptr);
    if (*endptr)
        return false;

    *output = result;
    return true;
}

}