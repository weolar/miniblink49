
#ifndef FALSE
#define FALSE 0
#endif

#include "third_party/icu/source/common/unicode/umachine.h"
#include "third_party/icu/source/common/unicode/uchar.h"
#include "third_party/icu/source/common/unicode/utrie2.h"
#include "third_party/icu/source/common/unicode/uprops.h"

#define INCLUDED_FROM_UCHAR_C
#include "third_party/icu/source/common/uchar_props_data.h"

int8_t u_charType(UChar32 c) {
    uint32_t props;
    GET_PROPS(c, props);
    return (int8_t)GET_CATEGORY(props);
}