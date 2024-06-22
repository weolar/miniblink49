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
#include "uscript.h"
#include "utypes.h"
#include "utrie2.h"

int8_t u_charType(UChar32 c);

/**
 * Constants for Unicode blocks, see the Unicode Data file Blocks.txt
 * @stable ICU 2.0
 */
enum UBlockCode {
    /*
     * Note: UBlockCode constants are parsed by preparseucd.py.
     * It matches lines like
     *     UBLOCK_<Unicode Block value name> = <integer>,
     */

    /** New No_Block value in Unicode 4. @stable ICU 2.6 */
    UBLOCK_NO_BLOCK = 0, /*[none]*/ /* Special range indicating No_Block */

    /** @stable ICU 2.0 */
    UBLOCK_BASIC_LATIN = 1, /*[0000]*/

    /** @stable ICU 2.0 */
    UBLOCK_LATIN_1_SUPPLEMENT=2, /*[0080]*/

    /** @stable ICU 2.0 */
    UBLOCK_LATIN_EXTENDED_A =3, /*[0100]*/

    /** @stable ICU 2.0 */
    UBLOCK_LATIN_EXTENDED_B =4, /*[0180]*/

    /** @stable ICU 2.0 */
    UBLOCK_IPA_EXTENSIONS =5, /*[0250]*/

    /** @stable ICU 2.0 */
    UBLOCK_SPACING_MODIFIER_LETTERS =6, /*[02B0]*/

    /** @stable ICU 2.0 */
    UBLOCK_COMBINING_DIACRITICAL_MARKS =7, /*[0300]*/

    /**
     * Unicode 3.2 renames this block to "Greek and Coptic".
     * @stable ICU 2.0
     */
    UBLOCK_GREEK =8, /*[0370]*/

    /** @stable ICU 2.0 */
    UBLOCK_CYRILLIC =9, /*[0400]*/

    /** @stable ICU 2.0 */
    UBLOCK_ARMENIAN =10, /*[0530]*/

    /** @stable ICU 2.0 */
    UBLOCK_HEBREW =11, /*[0590]*/

    /** @stable ICU 2.0 */
    UBLOCK_ARABIC =12, /*[0600]*/

    /** @stable ICU 2.0 */
    UBLOCK_SYRIAC =13, /*[0700]*/

    /** @stable ICU 2.0 */
    UBLOCK_THAANA =14, /*[0780]*/

    /** @stable ICU 2.0 */
    UBLOCK_DEVANAGARI =15, /*[0900]*/

    /** @stable ICU 2.0 */
    UBLOCK_BENGALI =16, /*[0980]*/

    /** @stable ICU 2.0 */
    UBLOCK_GURMUKHI =17, /*[0A00]*/

    /** @stable ICU 2.0 */
    UBLOCK_GUJARATI =18, /*[0A80]*/

    /** @stable ICU 2.0 */
    UBLOCK_ORIYA =19, /*[0B00]*/

    /** @stable ICU 2.0 */
    UBLOCK_TAMIL =20, /*[0B80]*/

    /** @stable ICU 2.0 */
    UBLOCK_TELUGU =21, /*[0C00]*/

    /** @stable ICU 2.0 */
    UBLOCK_KANNADA =22, /*[0C80]*/

    /** @stable ICU 2.0 */
    UBLOCK_MALAYALAM =23, /*[0D00]*/

    /** @stable ICU 2.0 */
    UBLOCK_SINHALA =24, /*[0D80]*/

    /** @stable ICU 2.0 */
    UBLOCK_THAI =25, /*[0E00]*/

    /** @stable ICU 2.0 */
    UBLOCK_LAO =26, /*[0E80]*/

    /** @stable ICU 2.0 */
    UBLOCK_TIBETAN =27, /*[0F00]*/

    /** @stable ICU 2.0 */
    UBLOCK_MYANMAR =28, /*[1000]*/

    /** @stable ICU 2.0 */
    UBLOCK_GEORGIAN =29, /*[10A0]*/

    /** @stable ICU 2.0 */
    UBLOCK_HANGUL_JAMO =30, /*[1100]*/

    /** @stable ICU 2.0 */
    UBLOCK_ETHIOPIC =31, /*[1200]*/

    /** @stable ICU 2.0 */
    UBLOCK_CHEROKEE =32, /*[13A0]*/

    /** @stable ICU 2.0 */
    UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS =33, /*[1400]*/

    /** @stable ICU 2.0 */
    UBLOCK_OGHAM =34, /*[1680]*/

    /** @stable ICU 2.0 */
    UBLOCK_RUNIC =35, /*[16A0]*/

    /** @stable ICU 2.0 */
    UBLOCK_KHMER =36, /*[1780]*/

    /** @stable ICU 2.0 */
    UBLOCK_MONGOLIAN =37, /*[1800]*/

    /** @stable ICU 2.0 */
    UBLOCK_LATIN_EXTENDED_ADDITIONAL =38, /*[1E00]*/

    /** @stable ICU 2.0 */
    UBLOCK_GREEK_EXTENDED =39, /*[1F00]*/

    /** @stable ICU 2.0 */
    UBLOCK_GENERAL_PUNCTUATION =40, /*[2000]*/

    /** @stable ICU 2.0 */
    UBLOCK_SUPERSCRIPTS_AND_SUBSCRIPTS =41, /*[2070]*/

    /** @stable ICU 2.0 */
    UBLOCK_CURRENCY_SYMBOLS =42, /*[20A0]*/

    /**
     * Unicode 3.2 renames this block to "Combining Diacritical Marks for Symbols".
     * @stable ICU 2.0
     */
    UBLOCK_COMBINING_MARKS_FOR_SYMBOLS =43, /*[20D0]*/

    /** @stable ICU 2.0 */
    UBLOCK_LETTERLIKE_SYMBOLS =44, /*[2100]*/

    /** @stable ICU 2.0 */
    UBLOCK_NUMBER_FORMS =45, /*[2150]*/

    /** @stable ICU 2.0 */
    UBLOCK_ARROWS =46, /*[2190]*/

    /** @stable ICU 2.0 */
    UBLOCK_MATHEMATICAL_OPERATORS =47, /*[2200]*/

    /** @stable ICU 2.0 */
    UBLOCK_MISCELLANEOUS_TECHNICAL =48, /*[2300]*/

    /** @stable ICU 2.0 */
    UBLOCK_CONTROL_PICTURES =49, /*[2400]*/

    /** @stable ICU 2.0 */
    UBLOCK_OPTICAL_CHARACTER_RECOGNITION =50, /*[2440]*/

    /** @stable ICU 2.0 */
    UBLOCK_ENCLOSED_ALPHANUMERICS =51, /*[2460]*/

    /** @stable ICU 2.0 */
    UBLOCK_BOX_DRAWING =52, /*[2500]*/

    /** @stable ICU 2.0 */
    UBLOCK_BLOCK_ELEMENTS =53, /*[2580]*/

    /** @stable ICU 2.0 */
    UBLOCK_GEOMETRIC_SHAPES =54, /*[25A0]*/

    /** @stable ICU 2.0 */
    UBLOCK_MISCELLANEOUS_SYMBOLS =55, /*[2600]*/

    /** @stable ICU 2.0 */
    UBLOCK_DINGBATS =56, /*[2700]*/

    /** @stable ICU 2.0 */
    UBLOCK_BRAILLE_PATTERNS =57, /*[2800]*/

    /** @stable ICU 2.0 */
    UBLOCK_CJK_RADICALS_SUPPLEMENT =58, /*[2E80]*/

    /** @stable ICU 2.0 */
    UBLOCK_KANGXI_RADICALS =59, /*[2F00]*/

    /** @stable ICU 2.0 */
    UBLOCK_IDEOGRAPHIC_DESCRIPTION_CHARACTERS =60, /*[2FF0]*/

    /** @stable ICU 2.0 */
    UBLOCK_CJK_SYMBOLS_AND_PUNCTUATION =61, /*[3000]*/

    /** @stable ICU 2.0 */
    UBLOCK_HIRAGANA =62, /*[3040]*/

    /** @stable ICU 2.0 */
    UBLOCK_KATAKANA =63, /*[30A0]*/

    /** @stable ICU 2.0 */
    UBLOCK_BOPOMOFO =64, /*[3100]*/

    /** @stable ICU 2.0 */
    UBLOCK_HANGUL_COMPATIBILITY_JAMO =65, /*[3130]*/

    /** @stable ICU 2.0 */
    UBLOCK_KANBUN =66, /*[3190]*/

    /** @stable ICU 2.0 */
    UBLOCK_BOPOMOFO_EXTENDED =67, /*[31A0]*/

    /** @stable ICU 2.0 */
    UBLOCK_ENCLOSED_CJK_LETTERS_AND_MONTHS =68, /*[3200]*/

    /** @stable ICU 2.0 */
    UBLOCK_CJK_COMPATIBILITY =69, /*[3300]*/

    /** @stable ICU 2.0 */
    UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A =70, /*[3400]*/

    /** @stable ICU 2.0 */
    UBLOCK_CJK_UNIFIED_IDEOGRAPHS =71, /*[4E00]*/

    /** @stable ICU 2.0 */
    UBLOCK_YI_SYLLABLES =72, /*[A000]*/

    /** @stable ICU 2.0 */
    UBLOCK_YI_RADICALS =73, /*[A490]*/

    /** @stable ICU 2.0 */
    UBLOCK_HANGUL_SYLLABLES =74, /*[AC00]*/

    /** @stable ICU 2.0 */
    UBLOCK_HIGH_SURROGATES =75, /*[D800]*/

    /** @stable ICU 2.0 */
    UBLOCK_HIGH_PRIVATE_USE_SURROGATES =76, /*[DB80]*/

    /** @stable ICU 2.0 */
    UBLOCK_LOW_SURROGATES =77, /*[DC00]*/

    /**
     * Same as UBLOCK_PRIVATE_USE.
     * Until Unicode 3.1.1, the corresponding block name was "Private Use",
     * and multiple code point ranges had this block.
     * Unicode 3.2 renames the block for the BMP PUA to "Private Use Area" and
     * adds separate blocks for the supplementary PUAs.
     *
     * @stable ICU 2.0
     */
    UBLOCK_PRIVATE_USE_AREA =78, /*[E000]*/
    /**
     * Same as UBLOCK_PRIVATE_USE_AREA.
     * Until Unicode 3.1.1, the corresponding block name was "Private Use",
     * and multiple code point ranges had this block.
     * Unicode 3.2 renames the block for the BMP PUA to "Private Use Area" and
     * adds separate blocks for the supplementary PUAs.
     *
     * @stable ICU 2.0
     */
    UBLOCK_PRIVATE_USE = UBLOCK_PRIVATE_USE_AREA,

    /** @stable ICU 2.0 */
    UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS =79, /*[F900]*/

    /** @stable ICU 2.0 */
    UBLOCK_ALPHABETIC_PRESENTATION_FORMS =80, /*[FB00]*/

    /** @stable ICU 2.0 */
    UBLOCK_ARABIC_PRESENTATION_FORMS_A =81, /*[FB50]*/

    /** @stable ICU 2.0 */
    UBLOCK_COMBINING_HALF_MARKS =82, /*[FE20]*/

    /** @stable ICU 2.0 */
    UBLOCK_CJK_COMPATIBILITY_FORMS =83, /*[FE30]*/

    /** @stable ICU 2.0 */
    UBLOCK_SMALL_FORM_VARIANTS =84, /*[FE50]*/

    /** @stable ICU 2.0 */
    UBLOCK_ARABIC_PRESENTATION_FORMS_B =85, /*[FE70]*/

    /** @stable ICU 2.0 */
    UBLOCK_SPECIALS =86, /*[FFF0]*/

    /** @stable ICU 2.0 */
    UBLOCK_HALFWIDTH_AND_FULLWIDTH_FORMS =87, /*[FF00]*/

    /* New blocks in Unicode 3.1 */

    /** @stable ICU 2.0 */
    UBLOCK_OLD_ITALIC = 88, /*[10300]*/
    /** @stable ICU 2.0 */
    UBLOCK_GOTHIC = 89, /*[10330]*/
    /** @stable ICU 2.0 */
    UBLOCK_DESERET = 90, /*[10400]*/
    /** @stable ICU 2.0 */
    UBLOCK_BYZANTINE_MUSICAL_SYMBOLS = 91, /*[1D000]*/
    /** @stable ICU 2.0 */
    UBLOCK_MUSICAL_SYMBOLS = 92, /*[1D100]*/
    /** @stable ICU 2.0 */
    UBLOCK_MATHEMATICAL_ALPHANUMERIC_SYMBOLS = 93, /*[1D400]*/
    /** @stable ICU 2.0 */
    UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B  = 94, /*[20000]*/
    /** @stable ICU 2.0 */
    UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT = 95, /*[2F800]*/
    /** @stable ICU 2.0 */
    UBLOCK_TAGS = 96, /*[E0000]*/

    /* New blocks in Unicode 3.2 */

    /** @stable ICU 3.0  */
    UBLOCK_CYRILLIC_SUPPLEMENT = 97, /*[0500]*/
    /**
     * Unicode 4.0.1 renames the "Cyrillic Supplementary" block to "Cyrillic Supplement".
     * @stable ICU 2.2
     */
    UBLOCK_CYRILLIC_SUPPLEMENTARY = UBLOCK_CYRILLIC_SUPPLEMENT, 
    /** @stable ICU 2.2 */
    UBLOCK_TAGALOG = 98, /*[1700]*/
    /** @stable ICU 2.2 */
    UBLOCK_HANUNOO = 99, /*[1720]*/
    /** @stable ICU 2.2 */
    UBLOCK_BUHID = 100, /*[1740]*/
    /** @stable ICU 2.2 */
    UBLOCK_TAGBANWA = 101, /*[1760]*/
    /** @stable ICU 2.2 */
    UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A = 102, /*[27C0]*/
    /** @stable ICU 2.2 */
    UBLOCK_SUPPLEMENTAL_ARROWS_A = 103, /*[27F0]*/
    /** @stable ICU 2.2 */
    UBLOCK_SUPPLEMENTAL_ARROWS_B = 104, /*[2900]*/
    /** @stable ICU 2.2 */
    UBLOCK_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B = 105, /*[2980]*/
    /** @stable ICU 2.2 */
    UBLOCK_SUPPLEMENTAL_MATHEMATICAL_OPERATORS = 106, /*[2A00]*/
    /** @stable ICU 2.2 */
    UBLOCK_KATAKANA_PHONETIC_EXTENSIONS = 107, /*[31F0]*/
    /** @stable ICU 2.2 */
    UBLOCK_VARIATION_SELECTORS = 108, /*[FE00]*/
    /** @stable ICU 2.2 */
    UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_A = 109, /*[F0000]*/
    /** @stable ICU 2.2 */
    UBLOCK_SUPPLEMENTARY_PRIVATE_USE_AREA_B = 110, /*[100000]*/

    /* New blocks in Unicode 4 */

    /** @stable ICU 2.6 */
    UBLOCK_LIMBU = 111, /*[1900]*/
    /** @stable ICU 2.6 */
    UBLOCK_TAI_LE = 112, /*[1950]*/
    /** @stable ICU 2.6 */
    UBLOCK_KHMER_SYMBOLS = 113, /*[19E0]*/
    /** @stable ICU 2.6 */
    UBLOCK_PHONETIC_EXTENSIONS = 114, /*[1D00]*/
    /** @stable ICU 2.6 */
    UBLOCK_MISCELLANEOUS_SYMBOLS_AND_ARROWS = 115, /*[2B00]*/
    /** @stable ICU 2.6 */
    UBLOCK_YIJING_HEXAGRAM_SYMBOLS = 116, /*[4DC0]*/
    /** @stable ICU 2.6 */
    UBLOCK_LINEAR_B_SYLLABARY = 117, /*[10000]*/
    /** @stable ICU 2.6 */
    UBLOCK_LINEAR_B_IDEOGRAMS = 118, /*[10080]*/
    /** @stable ICU 2.6 */
    UBLOCK_AEGEAN_NUMBERS = 119, /*[10100]*/
    /** @stable ICU 2.6 */
    UBLOCK_UGARITIC = 120, /*[10380]*/
    /** @stable ICU 2.6 */
    UBLOCK_SHAVIAN = 121, /*[10450]*/
    /** @stable ICU 2.6 */
    UBLOCK_OSMANYA = 122, /*[10480]*/
    /** @stable ICU 2.6 */
    UBLOCK_CYPRIOT_SYLLABARY = 123, /*[10800]*/
    /** @stable ICU 2.6 */
    UBLOCK_TAI_XUAN_JING_SYMBOLS = 124, /*[1D300]*/
    /** @stable ICU 2.6 */
    UBLOCK_VARIATION_SELECTORS_SUPPLEMENT = 125, /*[E0100]*/

    /* New blocks in Unicode 4.1 */

    /** @stable ICU 3.4 */
    UBLOCK_ANCIENT_GREEK_MUSICAL_NOTATION = 126, /*[1D200]*/
    /** @stable ICU 3.4 */
    UBLOCK_ANCIENT_GREEK_NUMBERS = 127, /*[10140]*/
    /** @stable ICU 3.4 */
    UBLOCK_ARABIC_SUPPLEMENT = 128, /*[0750]*/
    /** @stable ICU 3.4 */
    UBLOCK_BUGINESE = 129, /*[1A00]*/
    /** @stable ICU 3.4 */
    UBLOCK_CJK_STROKES = 130, /*[31C0]*/
    /** @stable ICU 3.4 */
    UBLOCK_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT = 131, /*[1DC0]*/
    /** @stable ICU 3.4 */
    UBLOCK_COPTIC = 132, /*[2C80]*/
    /** @stable ICU 3.4 */
    UBLOCK_ETHIOPIC_EXTENDED = 133, /*[2D80]*/
    /** @stable ICU 3.4 */
    UBLOCK_ETHIOPIC_SUPPLEMENT = 134, /*[1380]*/
    /** @stable ICU 3.4 */
    UBLOCK_GEORGIAN_SUPPLEMENT = 135, /*[2D00]*/
    /** @stable ICU 3.4 */
    UBLOCK_GLAGOLITIC = 136, /*[2C00]*/
    /** @stable ICU 3.4 */
    UBLOCK_KHAROSHTHI = 137, /*[10A00]*/
    /** @stable ICU 3.4 */
    UBLOCK_MODIFIER_TONE_LETTERS = 138, /*[A700]*/
    /** @stable ICU 3.4 */
    UBLOCK_NEW_TAI_LUE = 139, /*[1980]*/
    /** @stable ICU 3.4 */
    UBLOCK_OLD_PERSIAN = 140, /*[103A0]*/
    /** @stable ICU 3.4 */
    UBLOCK_PHONETIC_EXTENSIONS_SUPPLEMENT = 141, /*[1D80]*/
    /** @stable ICU 3.4 */
    UBLOCK_SUPPLEMENTAL_PUNCTUATION = 142, /*[2E00]*/
    /** @stable ICU 3.4 */
    UBLOCK_SYLOTI_NAGRI = 143, /*[A800]*/
    /** @stable ICU 3.4 */
    UBLOCK_TIFINAGH = 144, /*[2D30]*/
    /** @stable ICU 3.4 */
    UBLOCK_VERTICAL_FORMS = 145, /*[FE10]*/

    /* New blocks in Unicode 5.0 */

    /** @stable ICU 3.6 */
    UBLOCK_NKO = 146, /*[07C0]*/
    /** @stable ICU 3.6 */
    UBLOCK_BALINESE = 147, /*[1B00]*/
    /** @stable ICU 3.6 */
    UBLOCK_LATIN_EXTENDED_C = 148, /*[2C60]*/
    /** @stable ICU 3.6 */
    UBLOCK_LATIN_EXTENDED_D = 149, /*[A720]*/
    /** @stable ICU 3.6 */
    UBLOCK_PHAGS_PA = 150, /*[A840]*/
    /** @stable ICU 3.6 */
    UBLOCK_PHOENICIAN = 151, /*[10900]*/
    /** @stable ICU 3.6 */
    UBLOCK_CUNEIFORM = 152, /*[12000]*/
    /** @stable ICU 3.6 */
    UBLOCK_CUNEIFORM_NUMBERS_AND_PUNCTUATION = 153, /*[12400]*/
    /** @stable ICU 3.6 */
    UBLOCK_COUNTING_ROD_NUMERALS = 154, /*[1D360]*/

    /* New blocks in Unicode 5.1 */

    /** @stable ICU 4.0 */
    UBLOCK_SUNDANESE = 155, /*[1B80]*/
    /** @stable ICU 4.0 */
    UBLOCK_LEPCHA = 156, /*[1C00]*/
    /** @stable ICU 4.0 */
    UBLOCK_OL_CHIKI = 157, /*[1C50]*/
    /** @stable ICU 4.0 */
    UBLOCK_CYRILLIC_EXTENDED_A = 158, /*[2DE0]*/
    /** @stable ICU 4.0 */
    UBLOCK_VAI = 159, /*[A500]*/
    /** @stable ICU 4.0 */
    UBLOCK_CYRILLIC_EXTENDED_B = 160, /*[A640]*/
    /** @stable ICU 4.0 */
    UBLOCK_SAURASHTRA = 161, /*[A880]*/
    /** @stable ICU 4.0 */
    UBLOCK_KAYAH_LI = 162, /*[A900]*/
    /** @stable ICU 4.0 */
    UBLOCK_REJANG = 163, /*[A930]*/
    /** @stable ICU 4.0 */
    UBLOCK_CHAM = 164, /*[AA00]*/
    /** @stable ICU 4.0 */
    UBLOCK_ANCIENT_SYMBOLS = 165, /*[10190]*/
    /** @stable ICU 4.0 */
    UBLOCK_PHAISTOS_DISC = 166, /*[101D0]*/
    /** @stable ICU 4.0 */
    UBLOCK_LYCIAN = 167, /*[10280]*/
    /** @stable ICU 4.0 */
    UBLOCK_CARIAN = 168, /*[102A0]*/
    /** @stable ICU 4.0 */
    UBLOCK_LYDIAN = 169, /*[10920]*/
    /** @stable ICU 4.0 */
    UBLOCK_MAHJONG_TILES = 170, /*[1F000]*/
    /** @stable ICU 4.0 */
    UBLOCK_DOMINO_TILES = 171, /*[1F030]*/

    /* New blocks in Unicode 5.2 */

    /** @stable ICU 4.4 */
    UBLOCK_SAMARITAN = 172, /*[0800]*/
    /** @stable ICU 4.4 */
    UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED = 173, /*[18B0]*/
    /** @stable ICU 4.4 */
    UBLOCK_TAI_THAM = 174, /*[1A20]*/
    /** @stable ICU 4.4 */
    UBLOCK_VEDIC_EXTENSIONS = 175, /*[1CD0]*/
    /** @stable ICU 4.4 */
    UBLOCK_LISU = 176, /*[A4D0]*/
    /** @stable ICU 4.4 */
    UBLOCK_BAMUM = 177, /*[A6A0]*/
    /** @stable ICU 4.4 */
    UBLOCK_COMMON_INDIC_NUMBER_FORMS = 178, /*[A830]*/
    /** @stable ICU 4.4 */
    UBLOCK_DEVANAGARI_EXTENDED = 179, /*[A8E0]*/
    /** @stable ICU 4.4 */
    UBLOCK_HANGUL_JAMO_EXTENDED_A = 180, /*[A960]*/
    /** @stable ICU 4.4 */
    UBLOCK_JAVANESE = 181, /*[A980]*/
    /** @stable ICU 4.4 */
    UBLOCK_MYANMAR_EXTENDED_A = 182, /*[AA60]*/
    /** @stable ICU 4.4 */
    UBLOCK_TAI_VIET = 183, /*[AA80]*/
    /** @stable ICU 4.4 */
    UBLOCK_MEETEI_MAYEK = 184, /*[ABC0]*/
    /** @stable ICU 4.4 */
    UBLOCK_HANGUL_JAMO_EXTENDED_B = 185, /*[D7B0]*/
    /** @stable ICU 4.4 */
    UBLOCK_IMPERIAL_ARAMAIC = 186, /*[10840]*/
    /** @stable ICU 4.4 */
    UBLOCK_OLD_SOUTH_ARABIAN = 187, /*[10A60]*/
    /** @stable ICU 4.4 */
    UBLOCK_AVESTAN = 188, /*[10B00]*/
    /** @stable ICU 4.4 */
    UBLOCK_INSCRIPTIONAL_PARTHIAN = 189, /*[10B40]*/
    /** @stable ICU 4.4 */
    UBLOCK_INSCRIPTIONAL_PAHLAVI = 190, /*[10B60]*/
    /** @stable ICU 4.4 */
    UBLOCK_OLD_TURKIC = 191, /*[10C00]*/
    /** @stable ICU 4.4 */
    UBLOCK_RUMI_NUMERAL_SYMBOLS = 192, /*[10E60]*/
    /** @stable ICU 4.4 */
    UBLOCK_KAITHI = 193, /*[11080]*/
    /** @stable ICU 4.4 */
    UBLOCK_EGYPTIAN_HIEROGLYPHS = 194, /*[13000]*/
    /** @stable ICU 4.4 */
    UBLOCK_ENCLOSED_ALPHANUMERIC_SUPPLEMENT = 195, /*[1F100]*/
    /** @stable ICU 4.4 */
    UBLOCK_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT = 196, /*[1F200]*/
    /** @stable ICU 4.4 */
    UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C = 197, /*[2A700]*/

    /* New blocks in Unicode 6.0 */

    /** @stable ICU 4.6 */
    UBLOCK_MANDAIC = 198, /*[0840]*/
    /** @stable ICU 4.6 */
    UBLOCK_BATAK = 199, /*[1BC0]*/
    /** @stable ICU 4.6 */
    UBLOCK_ETHIOPIC_EXTENDED_A = 200, /*[AB00]*/
    /** @stable ICU 4.6 */
    UBLOCK_BRAHMI = 201, /*[11000]*/
    /** @stable ICU 4.6 */
    UBLOCK_BAMUM_SUPPLEMENT = 202, /*[16800]*/
    /** @stable ICU 4.6 */
    UBLOCK_KANA_SUPPLEMENT = 203, /*[1B000]*/
    /** @stable ICU 4.6 */
    UBLOCK_PLAYING_CARDS = 204, /*[1F0A0]*/
    /** @stable ICU 4.6 */
    UBLOCK_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS = 205, /*[1F300]*/
    /** @stable ICU 4.6 */
    UBLOCK_EMOTICONS = 206, /*[1F600]*/
    /** @stable ICU 4.6 */
    UBLOCK_TRANSPORT_AND_MAP_SYMBOLS = 207, /*[1F680]*/
    /** @stable ICU 4.6 */
    UBLOCK_ALCHEMICAL_SYMBOLS = 208, /*[1F700]*/
    /** @stable ICU 4.6 */
    UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D = 209, /*[2B740]*/

    /* New blocks in Unicode 6.1 */

    /** @stable ICU 49 */
    UBLOCK_ARABIC_EXTENDED_A = 210, /*[08A0]*/
    /** @stable ICU 49 */
    UBLOCK_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS = 211, /*[1EE00]*/
    /** @stable ICU 49 */
    UBLOCK_CHAKMA = 212, /*[11100]*/
    /** @stable ICU 49 */
    UBLOCK_MEETEI_MAYEK_EXTENSIONS = 213, /*[AAE0]*/
    /** @stable ICU 49 */
    UBLOCK_MEROITIC_CURSIVE = 214, /*[109A0]*/
    /** @stable ICU 49 */
    UBLOCK_MEROITIC_HIEROGLYPHS = 215, /*[10980]*/
    /** @stable ICU 49 */
    UBLOCK_MIAO = 216, /*[16F00]*/
    /** @stable ICU 49 */
    UBLOCK_SHARADA = 217, /*[11180]*/
    /** @stable ICU 49 */
    UBLOCK_SORA_SOMPENG = 218, /*[110D0]*/
    /** @stable ICU 49 */
    UBLOCK_SUNDANESE_SUPPLEMENT = 219, /*[1CC0]*/
    /** @stable ICU 49 */
    UBLOCK_TAKRI = 220, /*[11680]*/

    /* New blocks in Unicode 7.0 */

    /** @stable ICU 54 */
    UBLOCK_BASSA_VAH = 221, /*[16AD0]*/
    /** @stable ICU 54 */
    UBLOCK_CAUCASIAN_ALBANIAN = 222, /*[10530]*/
    /** @stable ICU 54 */
    UBLOCK_COPTIC_EPACT_NUMBERS = 223, /*[102E0]*/
    /** @stable ICU 54 */
    UBLOCK_COMBINING_DIACRITICAL_MARKS_EXTENDED = 224, /*[1AB0]*/
    /** @stable ICU 54 */
    UBLOCK_DUPLOYAN = 225, /*[1BC00]*/
    /** @stable ICU 54 */
    UBLOCK_ELBASAN = 226, /*[10500]*/
    /** @stable ICU 54 */
    UBLOCK_GEOMETRIC_SHAPES_EXTENDED = 227, /*[1F780]*/
    /** @stable ICU 54 */
    UBLOCK_GRANTHA = 228, /*[11300]*/
    /** @stable ICU 54 */
    UBLOCK_KHOJKI = 229, /*[11200]*/
    /** @stable ICU 54 */
    UBLOCK_KHUDAWADI = 230, /*[112B0]*/
    /** @stable ICU 54 */
    UBLOCK_LATIN_EXTENDED_E = 231, /*[AB30]*/
    /** @stable ICU 54 */
    UBLOCK_LINEAR_A = 232, /*[10600]*/
    /** @stable ICU 54 */
    UBLOCK_MAHAJANI = 233, /*[11150]*/
    /** @stable ICU 54 */
    UBLOCK_MANICHAEAN = 234, /*[10AC0]*/
    /** @stable ICU 54 */
    UBLOCK_MENDE_KIKAKUI = 235, /*[1E800]*/
    /** @stable ICU 54 */
    UBLOCK_MODI = 236, /*[11600]*/
    /** @stable ICU 54 */
    UBLOCK_MRO = 237, /*[16A40]*/
    /** @stable ICU 54 */
    UBLOCK_MYANMAR_EXTENDED_B = 238, /*[A9E0]*/
    /** @stable ICU 54 */
    UBLOCK_NABATAEAN = 239, /*[10880]*/
    /** @stable ICU 54 */
    UBLOCK_OLD_NORTH_ARABIAN = 240, /*[10A80]*/
    /** @stable ICU 54 */
    UBLOCK_OLD_PERMIC = 241, /*[10350]*/
    /** @stable ICU 54 */
    UBLOCK_ORNAMENTAL_DINGBATS = 242, /*[1F650]*/
    /** @stable ICU 54 */
    UBLOCK_PAHAWH_HMONG = 243, /*[16B00]*/
    /** @stable ICU 54 */
    UBLOCK_PALMYRENE = 244, /*[10860]*/
    /** @stable ICU 54 */
    UBLOCK_PAU_CIN_HAU = 245, /*[11AC0]*/
    /** @stable ICU 54 */
    UBLOCK_PSALTER_PAHLAVI = 246, /*[10B80]*/
    /** @stable ICU 54 */
    UBLOCK_SHORTHAND_FORMAT_CONTROLS = 247, /*[1BCA0]*/
    /** @stable ICU 54 */
    UBLOCK_SIDDHAM = 248, /*[11580]*/
    /** @stable ICU 54 */
    UBLOCK_SINHALA_ARCHAIC_NUMBERS = 249, /*[111E0]*/
    /** @stable ICU 54 */
    UBLOCK_SUPPLEMENTAL_ARROWS_C = 250, /*[1F800]*/
    /** @stable ICU 54 */
    UBLOCK_TIRHUTA = 251, /*[11480]*/
    /** @stable ICU 54 */
    UBLOCK_WARANG_CITI = 252, /*[118A0]*/

    /** @stable ICU 2.0 */
    UBLOCK_COUNT = 253,

    /** @stable ICU 2.0 */
    UBLOCK_INVALID_CODE=-1
};

#define UPROPS_BLOCK_MASK       0x0001ff00
#define UPROPS_BLOCK_SHIFT      8

UBlockCode ublock_getCode(UChar32 c);

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

#define UPROPS_SCRIPT_MASK 0x000000ff
#define UPROPS_SCRIPT_X_MASK 0x00c000ff
#define UPROPS_SCRIPT_X_WITH_COMMON 0x400000
#define UPROPS_SCRIPT_X_WITH_INHERITED 0x800000
#define UPROPS_SCRIPT_X_WITH_OTHER 0xc00000
#define U_FAILURE(x) ((x)>U_ZERO_ERROR)

#define U_GC_MN_MASK    U_MASK(U_NON_SPACING_MARK)
#define U_GC_ME_MASK    U_MASK(U_ENCLOSING_MARK)
#define U_GC_MC_MASK    U_MASK(U_COMBINING_SPACING_MARK)

/** Mask constant for multiple UCharCategory bits (M Marks). @stable ICU 2.1 */
#define U_GC_M_MASK (U_GC_MN_MASK|U_GC_ME_MASK|U_GC_MC_MASK)

uint32_t u_getUnicodeProperties(UChar32 c, int32_t column);
UScriptCode uscript_getScript(UChar32 c, UErrorCode* pErrorCode);
UBool uscript_hasScript(UChar32 c, UScriptCode sc);

#endif // UCHAR_H