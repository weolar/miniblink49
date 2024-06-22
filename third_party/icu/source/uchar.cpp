
#ifndef FALSE
#define FALSE 0
#endif

#include "third_party/icu/source/common/unicode/uchar.h"
#include "third_party/icu/source/common/unicode/umachine.h"
#include "third_party/icu/source/common/unicode/uprops.h"
#include "third_party/icu/source/common/unicode/utrie2.h"

#define INCLUDED_FROM_UCHAR_C
#include "third_party/icu/source/common/uchar_props_data.h"

int8_t u_charType(UChar32 c)
{
    uint32_t props;
    GET_PROPS(c, props);
    return (int8_t)GET_CATEGORY(props);
}

uint32_t u_getUnicodeProperties(UChar32 c, int32_t column)
{
    ASSERT(column >= 0);
    if (column >= propsVectorsColumns) {
        return 0;
    } else {
        uint16_t vecIndex = UTRIE2_GET16(&propsVectorsTrie, c);
        return propsVectors[vecIndex + column];
    }
}

UBlockCode ublock_getCode(UChar32 c)
{
    return (UBlockCode)((u_getUnicodeProperties(c, 0) & UPROPS_BLOCK_MASK) >> UPROPS_BLOCK_SHIFT);
}

#define UPROPS_SCRIPT_MASK 0x000000ff
#define UPROPS_SCRIPT_X_MASK 0x00c000ff
#define UPROPS_SCRIPT_X_WITH_COMMON 0x400000
#define UPROPS_SCRIPT_X_WITH_INHERITED 0x800000
#define UPROPS_SCRIPT_X_WITH_OTHER 0xc00000
#define U_FAILURE(x) ((x)>U_ZERO_ERROR)

UScriptCode uscript_getScript(UChar32 c, UErrorCode* pErrorCode)
{
    uint32_t scriptX;
    if (pErrorCode == NULL || U_FAILURE(*pErrorCode)) {
        return USCRIPT_INVALID_CODE;
    }
    if ((uint32_t)c > 0x10ffff) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        return USCRIPT_INVALID_CODE;
    }
    scriptX = u_getUnicodeProperties(c, 0) & UPROPS_SCRIPT_X_MASK;
    if (scriptX < UPROPS_SCRIPT_X_WITH_COMMON) {
        return (UScriptCode)scriptX;
    } else if (scriptX < UPROPS_SCRIPT_X_WITH_INHERITED) {
        return USCRIPT_COMMON;
    } else if (scriptX < UPROPS_SCRIPT_X_WITH_OTHER) {
        return USCRIPT_INHERITED;
    } else {
        return (UScriptCode)scriptExtensions[scriptX & UPROPS_SCRIPT_MASK];
    }
}

UBool uscript_hasScript(UChar32 c, UScriptCode sc)
{
    return false;
}