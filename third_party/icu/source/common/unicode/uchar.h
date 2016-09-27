/*
**********************************************************************
*   Copyright (C) 1997-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
* File UCHAR.H
*
* Modification History:
*
*   Date        Name        Description
*   04/02/97    aliu        Creation.
*   03/29/99    helena      Updated for C APIs.
*   4/15/99     Madhu       Updated for C Implementation and Javadoc
*   5/20/99     Madhu       Added the function u_getVersion()
*   8/19/1999   srl         Upgraded scripts to Unicode 3.0
*   8/27/1999   schererm    UCharDirection constants: U_...
*   11/11/1999  weiv        added u_isalnum(), cleaned comments
*   01/11/2000  helena      Renamed u_getVersion to u_getUnicodeVersion().
******************************************************************************
*/

#ifndef UCHAR_H
#define UCHAR_H

#include "umachine.h"

int8_t u_charType(UChar32 c);

/** The lowest Unicode code point value. Code points are non-negative. @stable ICU 2.0 */
#define UCHAR_MIN_VALUE 0

/**
* The highest Unicode code point value (scalar value) according to
* The Unicode Standard. This is a 21-bit value (20.1 bits, rounded up).
* For a single character, UChar32 is a simple type that can hold any code point value.
*
* @see UChar32
* @stable ICU 2.0
*/
#define UCHAR_MAX_VALUE 0x10ffff

#define U_GC_CN_MASK    U_MASK(U_GENERAL_OTHER_TYPES)

/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_LU_MASK    U_MASK(U_UPPERCASE_LETTER)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_LL_MASK    U_MASK(U_LOWERCASE_LETTER)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_LT_MASK    U_MASK(U_TITLECASE_LETTER)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_LM_MASK    U_MASK(U_MODIFIER_LETTER)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_LO_MASK    U_MASK(U_OTHER_LETTER)

/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_MN_MASK    U_MASK(U_NON_SPACING_MARK)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_ME_MASK    U_MASK(U_ENCLOSING_MARK)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_MC_MASK    U_MASK(U_COMBINING_SPACING_MARK)

/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_ND_MASK    U_MASK(U_DECIMAL_DIGIT_NUMBER)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_NL_MASK    U_MASK(U_LETTER_NUMBER)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_NO_MASK    U_MASK(U_OTHER_NUMBER)

/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_ZS_MASK    U_MASK(U_SPACE_SEPARATOR)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_ZL_MASK    U_MASK(U_LINE_SEPARATOR)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_ZP_MASK    U_MASK(U_PARAGRAPH_SEPARATOR)

/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_CC_MASK    U_MASK(U_CONTROL_CHAR)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_CF_MASK    U_MASK(U_FORMAT_CHAR)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_CO_MASK    U_MASK(U_PRIVATE_USE_CHAR)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_CS_MASK    U_MASK(U_SURROGATE)

/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_PD_MASK    U_MASK(U_DASH_PUNCTUATION)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_PS_MASK    U_MASK(U_START_PUNCTUATION)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_PE_MASK    U_MASK(U_END_PUNCTUATION)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_PC_MASK    U_MASK(U_CONNECTOR_PUNCTUATION)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_PO_MASK    U_MASK(U_OTHER_PUNCTUATION)

/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_SM_MASK    U_MASK(U_MATH_SYMBOL)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_SC_MASK    U_MASK(U_CURRENCY_SYMBOL)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_SK_MASK    U_MASK(U_MODIFIER_SYMBOL)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_SO_MASK    U_MASK(U_OTHER_SYMBOL)

/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_PI_MASK    U_MASK(U_INITIAL_PUNCTUATION)
/** Mask constant for a UCharCategory. @stable ICU 2.1 */
#define U_GC_PF_MASK    U_MASK(U_FINAL_PUNCTUATION)


/** Mask constant for multiple UCharCategory bits (L Letters). @stable ICU 2.1 */
#define U_GC_L_MASK \
            (U_GC_LU_MASK|U_GC_LL_MASK|U_GC_LT_MASK|U_GC_LM_MASK|U_GC_LO_MASK)

/** Mask constant for multiple UCharCategory bits (LC Cased Letters). @stable ICU 2.1 */
#define U_GC_LC_MASK \
            (U_GC_LU_MASK|U_GC_LL_MASK|U_GC_LT_MASK)

/** Mask constant for multiple UCharCategory bits (M Marks). @stable ICU 2.1 */
#define U_GC_M_MASK (U_GC_MN_MASK|U_GC_ME_MASK|U_GC_MC_MASK)

/** Mask constant for multiple UCharCategory bits (N Numbers). @stable ICU 2.1 */
#define U_GC_N_MASK (U_GC_ND_MASK|U_GC_NL_MASK|U_GC_NO_MASK)

/** Mask constant for multiple UCharCategory bits (Z Separators). @stable ICU 2.1 */
#define U_GC_Z_MASK (U_GC_ZS_MASK|U_GC_ZL_MASK|U_GC_ZP_MASK)

/** Mask constant for multiple UCharCategory bits (C Others). @stable ICU 2.1 */
#define U_GC_C_MASK \
            (U_GC_CN_MASK|U_GC_CC_MASK|U_GC_CF_MASK|U_GC_CO_MASK|U_GC_CS_MASK)

/** Mask constant for multiple UCharCategory bits (P Punctuation). @stable ICU 2.1 */
#define U_GC_P_MASK \
            (U_GC_PD_MASK|U_GC_PS_MASK|U_GC_PE_MASK|U_GC_PC_MASK|U_GC_PO_MASK| \
             U_GC_PI_MASK|U_GC_PF_MASK)

/** Mask constant for multiple UCharCategory bits (S Symbols). @stable ICU 2.1 */
#define U_GC_S_MASK (U_GC_SM_MASK|U_GC_SC_MASK|U_GC_SK_MASK|U_GC_SO_MASK)

/**
* Get a single-bit bit set (a flag) from a bit number 0..31.
* @stable ICU 2.1
*/
#define U_MASK(x) ((uint32_t)1<<(x))

/* constants and macros for access to the data ------------------------------ */

/* getting a uint32_t properties word from the data */
#define GET_PROPS(c, result) ((result)=UTRIE2_GET16(&propsTrie, c));

#define U_GET_GC_MASK(c) U_MASK(u_charType(c))

/**
* Data for enumerated Unicode general category types.
* See http://www.unicode.org/Public/UNIDATA/UnicodeData.html .
* @stable ICU 2.0
*/
typedef enum UCharCategory
{
    /*
    * Note: UCharCategory constants and their API comments are parsed by preparseucd.py.
    * It matches pairs of lines like
    *     / ** <Unicode 2-letter General_Category value> comment... * /
    *     U_<[A-Z_]+> = <integer>,
    */

    /** Non-category for unassigned and non-character code points. @stable ICU 2.0 */
    U_UNASSIGNED = 0,
    /** Cn "Other, Not Assigned (no characters in [UnicodeData.txt] have this property)" (same as U_UNASSIGNED!) @stable ICU 2.0 */
    U_GENERAL_OTHER_TYPES = 0,
    /** Lu @stable ICU 2.0 */
    U_UPPERCASE_LETTER = 1,
    /** Ll @stable ICU 2.0 */
    U_LOWERCASE_LETTER = 2,
    /** Lt @stable ICU 2.0 */
    U_TITLECASE_LETTER = 3,
    /** Lm @stable ICU 2.0 */
    U_MODIFIER_LETTER = 4,
    /** Lo @stable ICU 2.0 */
    U_OTHER_LETTER = 5,
    /** Mn @stable ICU 2.0 */
    U_NON_SPACING_MARK = 6,
    /** Me @stable ICU 2.0 */
    U_ENCLOSING_MARK = 7,
    /** Mc @stable ICU 2.0 */
    U_COMBINING_SPACING_MARK = 8,
    /** Nd @stable ICU 2.0 */
    U_DECIMAL_DIGIT_NUMBER = 9,
    /** Nl @stable ICU 2.0 */
    U_LETTER_NUMBER = 10,
    /** No @stable ICU 2.0 */
    U_OTHER_NUMBER = 11,
    /** Zs @stable ICU 2.0 */
    U_SPACE_SEPARATOR = 12,
    /** Zl @stable ICU 2.0 */
    U_LINE_SEPARATOR = 13,
    /** Zp @stable ICU 2.0 */
    U_PARAGRAPH_SEPARATOR = 14,
    /** Cc @stable ICU 2.0 */
    U_CONTROL_CHAR = 15,
    /** Cf @stable ICU 2.0 */
    U_FORMAT_CHAR = 16,
    /** Co @stable ICU 2.0 */
    U_PRIVATE_USE_CHAR = 17,
    /** Cs @stable ICU 2.0 */
    U_SURROGATE = 18,
    /** Pd @stable ICU 2.0 */
    U_DASH_PUNCTUATION = 19,
    /** Ps @stable ICU 2.0 */
    U_START_PUNCTUATION = 20,
    /** Pe @stable ICU 2.0 */
    U_END_PUNCTUATION = 21,
    /** Pc @stable ICU 2.0 */
    U_CONNECTOR_PUNCTUATION = 22,
    /** Po @stable ICU 2.0 */
    U_OTHER_PUNCTUATION = 23,
    /** Sm @stable ICU 2.0 */
    U_MATH_SYMBOL = 24,
    /** Sc @stable ICU 2.0 */
    U_CURRENCY_SYMBOL = 25,
    /** Sk @stable ICU 2.0 */
    U_MODIFIER_SYMBOL = 26,
    /** So @stable ICU 2.0 */
    U_OTHER_SYMBOL = 27,
    /** Pi @stable ICU 2.0 */
    U_INITIAL_PUNCTUATION = 28,
    /** Pf @stable ICU 2.0 */
    U_FINAL_PUNCTUATION = 29,
    /** One higher than the last enum UCharCategory constant. @stable ICU 2.0 */
    U_CHAR_CATEGORY_COUNT
} UCharCategory;

#define U_GC_MN_MASK    U_MASK(U_NON_SPACING_MARK)
#define U_GC_ME_MASK    U_MASK(U_ENCLOSING_MARK)
#define U_GC_MC_MASK    U_MASK(U_COMBINING_SPACING_MARK)

/** Mask constant for multiple UCharCategory bits (M Marks). @stable ICU 2.1 */
#define U_GC_M_MASK (U_GC_MN_MASK|U_GC_ME_MASK|U_GC_MC_MASK)

#endif // UCHAR_H