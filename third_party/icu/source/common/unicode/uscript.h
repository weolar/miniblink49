#ifndef uscript_h
#define uscript_h

typedef enum UScriptCode {
    /*
    * Note: UScriptCode constants and their ISO script code comments
    * are parsed by preparseucd.py.
    * It matches lines like
    *     USCRIPT_<Unicode Script value name> = <integer>,  / * <ISO script code> * /
    */

    /** @stable ICU 2.2 */
    USCRIPT_INVALID_CODE = -1,
    /** @stable ICU 2.2 */
    USCRIPT_COMMON = 0,  /* Zyyy */
    /** @stable ICU 2.2 */
    USCRIPT_INHERITED = 1,  /* Zinh */ /* "Code for inherited script", for non-spacing combining marks; also Qaai */
    /** @stable ICU 2.2 */
    USCRIPT_ARABIC = 2,  /* Arab */
    /** @stable ICU 2.2 */
    USCRIPT_ARMENIAN = 3,  /* Armn */
    /** @stable ICU 2.2 */
    USCRIPT_BENGALI = 4,  /* Beng */
    /** @stable ICU 2.2 */
    USCRIPT_BOPOMOFO = 5,  /* Bopo */
    /** @stable ICU 2.2 */
    USCRIPT_CHEROKEE = 6,  /* Cher */
    /** @stable ICU 2.2 */
    USCRIPT_COPTIC = 7,  /* Copt */
    /** @stable ICU 2.2 */
    USCRIPT_CYRILLIC = 8,  /* Cyrl */
    /** @stable ICU 2.2 */
    USCRIPT_DESERET = 9,  /* Dsrt */
    /** @stable ICU 2.2 */
    USCRIPT_DEVANAGARI = 10,  /* Deva */
    /** @stable ICU 2.2 */
    USCRIPT_ETHIOPIC = 11,  /* Ethi */
    /** @stable ICU 2.2 */
    USCRIPT_GEORGIAN = 12,  /* Geor */
    /** @stable ICU 2.2 */
    USCRIPT_GOTHIC = 13,  /* Goth */
    /** @stable ICU 2.2 */
    USCRIPT_GREEK = 14,  /* Grek */
    /** @stable ICU 2.2 */
    USCRIPT_GUJARATI = 15,  /* Gujr */
    /** @stable ICU 2.2 */
    USCRIPT_GURMUKHI = 16,  /* Guru */
    /** @stable ICU 2.2 */
    USCRIPT_HAN = 17,  /* Hani */
    /** @stable ICU 2.2 */
    USCRIPT_HANGUL = 18,  /* Hang */
    /** @stable ICU 2.2 */
    USCRIPT_HEBREW = 19,  /* Hebr */
    /** @stable ICU 2.2 */
    USCRIPT_HIRAGANA = 20,  /* Hira */
    /** @stable ICU 2.2 */
    USCRIPT_KANNADA = 21,  /* Knda */
    /** @stable ICU 2.2 */
    USCRIPT_KATAKANA = 22,  /* Kana */
    /** @stable ICU 2.2 */
    USCRIPT_KHMER = 23,  /* Khmr */
    /** @stable ICU 2.2 */
    USCRIPT_LAO = 24,  /* Laoo */
    /** @stable ICU 2.2 */
    USCRIPT_LATIN = 25,  /* Latn */
    /** @stable ICU 2.2 */
    USCRIPT_MALAYALAM = 26,  /* Mlym */
    /** @stable ICU 2.2 */
    USCRIPT_MONGOLIAN = 27,  /* Mong */
    /** @stable ICU 2.2 */
    USCRIPT_MYANMAR = 28,  /* Mymr */
    /** @stable ICU 2.2 */
    USCRIPT_OGHAM = 29,  /* Ogam */
    /** @stable ICU 2.2 */
    USCRIPT_OLD_ITALIC = 30,  /* Ital */
    /** @stable ICU 2.2 */
    USCRIPT_ORIYA = 31,  /* Orya */
    /** @stable ICU 2.2 */
    USCRIPT_RUNIC = 32,  /* Runr */
    /** @stable ICU 2.2 */
    USCRIPT_SINHALA = 33,  /* Sinh */
    /** @stable ICU 2.2 */
    USCRIPT_SYRIAC = 34,  /* Syrc */
    /** @stable ICU 2.2 */
    USCRIPT_TAMIL = 35,  /* Taml */
    /** @stable ICU 2.2 */
    USCRIPT_TELUGU = 36,  /* Telu */
    /** @stable ICU 2.2 */
    USCRIPT_THAANA = 37,  /* Thaa */
    /** @stable ICU 2.2 */
    USCRIPT_THAI = 38,  /* Thai */
    /** @stable ICU 2.2 */
    USCRIPT_TIBETAN = 39,  /* Tibt */
    /** Canadian_Aboriginal script. @stable ICU 2.6 */
    USCRIPT_CANADIAN_ABORIGINAL = 40,  /* Cans */
    /** Canadian_Aboriginal script (alias). @stable ICU 2.2 */
    USCRIPT_UCAS = USCRIPT_CANADIAN_ABORIGINAL,
    /** @stable ICU 2.2 */
    USCRIPT_YI = 41,  /* Yiii */
    /* New scripts in Unicode 3.2 */
    /** @stable ICU 2.2 */
    USCRIPT_TAGALOG = 42,  /* Tglg */
    /** @stable ICU 2.2 */
    USCRIPT_HANUNOO = 43,  /* Hano */
    /** @stable ICU 2.2 */
    USCRIPT_BUHID = 44,  /* Buhd */
    /** @stable ICU 2.2 */
    USCRIPT_TAGBANWA = 45,  /* Tagb */

    /* New scripts in Unicode 4 */
    /** @stable ICU 2.6 */
    USCRIPT_BRAILLE = 46,  /* Brai */
    /** @stable ICU 2.6 */
    USCRIPT_CYPRIOT = 47,  /* Cprt */
    /** @stable ICU 2.6 */
    USCRIPT_LIMBU = 48,  /* Limb */
    /** @stable ICU 2.6 */
    USCRIPT_LINEAR_B = 49,  /* Linb */
    /** @stable ICU 2.6 */
    USCRIPT_OSMANYA = 50,  /* Osma */
    /** @stable ICU 2.6 */
    USCRIPT_SHAVIAN = 51,  /* Shaw */
    /** @stable ICU 2.6 */
    USCRIPT_TAI_LE = 52,  /* Tale */
    /** @stable ICU 2.6 */
    USCRIPT_UGARITIC = 53,  /* Ugar */

    /** New script code in Unicode 4.0.1 @stable ICU 3.0 */
    USCRIPT_KATAKANA_OR_HIRAGANA = 54,/*Hrkt */

    /* New scripts in Unicode 4.1 */
    /** @stable ICU 3.4 */
    USCRIPT_BUGINESE = 55, /* Bugi */
    /** @stable ICU 3.4 */
    USCRIPT_GLAGOLITIC = 56, /* Glag */
    /** @stable ICU 3.4 */
    USCRIPT_KHAROSHTHI = 57, /* Khar */
    /** @stable ICU 3.4 */
    USCRIPT_SYLOTI_NAGRI = 58, /* Sylo */
    /** @stable ICU 3.4 */
    USCRIPT_NEW_TAI_LUE = 59, /* Talu */
    /** @stable ICU 3.4 */
    USCRIPT_TIFINAGH = 60, /* Tfng */
    /** @stable ICU 3.4 */
    USCRIPT_OLD_PERSIAN = 61, /* Xpeo */

    /* New script codes from Unicode and ISO 15924 */
    /** @stable ICU 3.6 */
    USCRIPT_BALINESE = 62, /* Bali */
    /** @stable ICU 3.6 */
    USCRIPT_BATAK = 63, /* Batk */
    /** @stable ICU 3.6 */
    USCRIPT_BLISSYMBOLS = 64, /* Blis */
    /** @stable ICU 3.6 */
    USCRIPT_BRAHMI = 65, /* Brah */
    /** @stable ICU 3.6 */
    USCRIPT_CHAM = 66, /* Cham */
    /** @stable ICU 3.6 */
    USCRIPT_CIRTH = 67, /* Cirt */
    /** @stable ICU 3.6 */
    USCRIPT_OLD_CHURCH_SLAVONIC_CYRILLIC = 68, /* Cyrs */
    /** @stable ICU 3.6 */
    USCRIPT_DEMOTIC_EGYPTIAN = 69, /* Egyd */
    /** @stable ICU 3.6 */
    USCRIPT_HIERATIC_EGYPTIAN = 70, /* Egyh */
    /** @stable ICU 3.6 */
    USCRIPT_EGYPTIAN_HIEROGLYPHS = 71, /* Egyp */
    /** @stable ICU 3.6 */
    USCRIPT_KHUTSURI = 72, /* Geok */
    /** @stable ICU 3.6 */
    USCRIPT_SIMPLIFIED_HAN = 73, /* Hans */
    /** @stable ICU 3.6 */
    USCRIPT_TRADITIONAL_HAN = 74, /* Hant */
    /** @stable ICU 3.6 */
    USCRIPT_PAHAWH_HMONG = 75, /* Hmng */
    /** @stable ICU 3.6 */
    USCRIPT_OLD_HUNGARIAN = 76, /* Hung */
    /** @stable ICU 3.6 */
    USCRIPT_HARAPPAN_INDUS = 77, /* Inds */
    /** @stable ICU 3.6 */
    USCRIPT_JAVANESE = 78, /* Java */
    /** @stable ICU 3.6 */
    USCRIPT_KAYAH_LI = 79, /* Kali */
    /** @stable ICU 3.6 */
    USCRIPT_LATIN_FRAKTUR = 80, /* Latf */
    /** @stable ICU 3.6 */
    USCRIPT_LATIN_GAELIC = 81, /* Latg */
    /** @stable ICU 3.6 */
    USCRIPT_LEPCHA = 82, /* Lepc */
    /** @stable ICU 3.6 */
    USCRIPT_LINEAR_A = 83, /* Lina */
    /** @stable ICU 4.6 */
    USCRIPT_MANDAIC = 84, /* Mand */
    /** @stable ICU 3.6 */
    USCRIPT_MANDAEAN = USCRIPT_MANDAIC,
    /** @stable ICU 3.6 */
    USCRIPT_MAYAN_HIEROGLYPHS = 85, /* Maya */
    /** @stable ICU 4.6 */
    USCRIPT_MEROITIC_HIEROGLYPHS = 86, /* Mero */
    /** @stable ICU 3.6 */
    USCRIPT_MEROITIC = USCRIPT_MEROITIC_HIEROGLYPHS,
    /** @stable ICU 3.6 */
    USCRIPT_NKO = 87, /* Nkoo */
    /** @stable ICU 3.6 */
    USCRIPT_ORKHON = 88, /* Orkh */
    /** @stable ICU 3.6 */
    USCRIPT_OLD_PERMIC = 89, /* Perm */
    /** @stable ICU 3.6 */
    USCRIPT_PHAGS_PA = 90, /* Phag */
    /** @stable ICU 3.6 */
    USCRIPT_PHOENICIAN = 91, /* Phnx */
    /** @stable ICU 52 */
    USCRIPT_MIAO = 92, /* Plrd */
    /** @stable ICU 3.6 */
    USCRIPT_PHONETIC_POLLARD = USCRIPT_MIAO,
    /** @stable ICU 3.6 */
    USCRIPT_RONGORONGO = 93, /* Roro */
    /** @stable ICU 3.6 */
    USCRIPT_SARATI = 94, /* Sara */
    /** @stable ICU 3.6 */
    USCRIPT_ESTRANGELO_SYRIAC = 95, /* Syre */
    /** @stable ICU 3.6 */
    USCRIPT_WESTERN_SYRIAC = 96, /* Syrj */
    /** @stable ICU 3.6 */
    USCRIPT_EASTERN_SYRIAC = 97, /* Syrn */
    /** @stable ICU 3.6 */
    USCRIPT_TENGWAR = 98, /* Teng */
    /** @stable ICU 3.6 */
    USCRIPT_VAI = 99, /* Vaii */
    /** @stable ICU 3.6 */
    USCRIPT_VISIBLE_SPEECH = 100,/* Visp */
    /** @stable ICU 3.6 */
    USCRIPT_CUNEIFORM = 101,/* Xsux */
    /** @stable ICU 3.6 */
    USCRIPT_UNWRITTEN_LANGUAGES = 102,/* Zxxx */
    /** @stable ICU 3.6 */
    USCRIPT_UNKNOWN = 103,/* Zzzz */ /* Unknown="Code for uncoded script", for unassigned code points */

    /** @stable ICU 3.8 */
    USCRIPT_CARIAN = 104,/* Cari */
    /** @stable ICU 3.8 */
    USCRIPT_JAPANESE = 105,/* Jpan */
    /** @stable ICU 3.8 */
    USCRIPT_LANNA = 106,/* Lana */
    /** @stable ICU 3.8 */
    USCRIPT_LYCIAN = 107,/* Lyci */
    /** @stable ICU 3.8 */
    USCRIPT_LYDIAN = 108,/* Lydi */
    /** @stable ICU 3.8 */
    USCRIPT_OL_CHIKI = 109,/* Olck */
    /** @stable ICU 3.8 */
    USCRIPT_REJANG = 110,/* Rjng */
    /** @stable ICU 3.8 */
    USCRIPT_SAURASHTRA = 111,/* Saur */
    /** @stable ICU 3.8 */
    USCRIPT_SIGN_WRITING = 112,/* Sgnw */
    /** @stable ICU 3.8 */
    USCRIPT_SUNDANESE = 113,/* Sund */
    /** @stable ICU 3.8 */
    USCRIPT_MOON = 114,/* Moon */
    /** @stable ICU 3.8 */
    USCRIPT_MEITEI_MAYEK = 115,/* Mtei */

    /** @stable ICU 4.0 */
    USCRIPT_IMPERIAL_ARAMAIC = 116,/* Armi */
    /** @stable ICU 4.0 */
    USCRIPT_AVESTAN = 117,/* Avst */
    /** @stable ICU 4.0 */
    USCRIPT_CHAKMA = 118,/* Cakm */
    /** @stable ICU 4.0 */
    USCRIPT_KOREAN = 119,/* Kore */
    /** @stable ICU 4.0 */
    USCRIPT_KAITHI = 120,/* Kthi */
    /** @stable ICU 4.0 */
    USCRIPT_MANICHAEAN = 121,/* Mani */
    /** @stable ICU 4.0 */
    USCRIPT_INSCRIPTIONAL_PAHLAVI = 122,/* Phli */
    /** @stable ICU 4.0 */
    USCRIPT_PSALTER_PAHLAVI = 123,/* Phlp */
    /** @stable ICU 4.0 */
    USCRIPT_BOOK_PAHLAVI = 124,/* Phlv */
    /** @stable ICU 4.0 */
    USCRIPT_INSCRIPTIONAL_PARTHIAN = 125,/* Prti */
    /** @stable ICU 4.0 */
    USCRIPT_SAMARITAN = 126,/* Samr */
    /** @stable ICU 4.0 */
    USCRIPT_TAI_VIET = 127,/* Tavt */
    /** @stable ICU 4.0 */
    USCRIPT_MATHEMATICAL_NOTATION = 128,/* Zmth */
    /** @stable ICU 4.0 */
    USCRIPT_SYMBOLS = 129,/* Zsym */

    /** @stable ICU 4.4 */
    USCRIPT_BAMUM = 130,/* Bamu */
    /** @stable ICU 4.4 */
    USCRIPT_LISU = 131,/* Lisu */
    /** @stable ICU 4.4 */
    USCRIPT_NAKHI_GEBA = 132,/* Nkgb */
    /** @stable ICU 4.4 */
    USCRIPT_OLD_SOUTH_ARABIAN = 133,/* Sarb */

    /** @stable ICU 4.6 */
    USCRIPT_BASSA_VAH = 134,/* Bass */
    /** @stable ICU 54 */
    USCRIPT_DUPLOYAN = 135,/* Dupl */
#ifndef U_HIDE_DEPRECATED_API
    /** @deprecated ICU 54 Typo, use USCRIPT_DUPLOYAN */
    USCRIPT_DUPLOYAN_SHORTAND = USCRIPT_DUPLOYAN,
#endif  /* U_HIDE_DEPRECATED_API */
    /** @stable ICU 4.6 */
    USCRIPT_ELBASAN = 136,/* Elba */
    /** @stable ICU 4.6 */
    USCRIPT_GRANTHA = 137,/* Gran */
    /** @stable ICU 4.6 */
    USCRIPT_KPELLE = 138,/* Kpel */
    /** @stable ICU 4.6 */
    USCRIPT_LOMA = 139,/* Loma */
    /** Mende Kikakui @stable ICU 4.6 */
    USCRIPT_MENDE = 140,/* Mend */
    /** @stable ICU 4.6 */
    USCRIPT_MEROITIC_CURSIVE = 141,/* Merc */
    /** @stable ICU 4.6 */
    USCRIPT_OLD_NORTH_ARABIAN = 142,/* Narb */
    /** @stable ICU 4.6 */
    USCRIPT_NABATAEAN = 143,/* Nbat */
    /** @stable ICU 4.6 */
    USCRIPT_PALMYRENE = 144,/* Palm */
    /** @stable ICU 54 */
    USCRIPT_KHUDAWADI = 145,/* Sind */
    /** @stable ICU 4.6 */
    USCRIPT_SINDHI = USCRIPT_KHUDAWADI,
    /** @stable ICU 4.6 */
    USCRIPT_WARANG_CITI = 146,/* Wara */

    /** @stable ICU 4.8 */
    USCRIPT_AFAKA = 147,/* Afak */
    /** @stable ICU 4.8 */
    USCRIPT_JURCHEN = 148,/* Jurc */
    /** @stable ICU 4.8 */
    USCRIPT_MRO = 149,/* Mroo */
    /** @stable ICU 4.8 */
    USCRIPT_NUSHU = 150,/* Nshu */
    /** @stable ICU 4.8 */
    USCRIPT_SHARADA = 151,/* Shrd */
    /** @stable ICU 4.8 */
    USCRIPT_SORA_SOMPENG = 152,/* Sora */
    /** @stable ICU 4.8 */
    USCRIPT_TAKRI = 153,/* Takr */
    /** @stable ICU 4.8 */
    USCRIPT_TANGUT = 154,/* Tang */
    /** @stable ICU 4.8 */
    USCRIPT_WOLEAI = 155,/* Wole */

    /** @stable ICU 49 */
    USCRIPT_ANATOLIAN_HIEROGLYPHS = 156,/* Hluw */
    /** @stable ICU 49 */
    USCRIPT_KHOJKI = 157,/* Khoj */
    /** @stable ICU 49 */
    USCRIPT_TIRHUTA = 158,/* Tirh */

    /** @stable ICU 52 */
    USCRIPT_CAUCASIAN_ALBANIAN = 159,/* Aghb */
    /** @stable ICU 52 */
    USCRIPT_MAHAJANI = 160,/* Mahj */

    /** @stable ICU 54 */
    USCRIPT_AHOM = 161,/* Ahom */
    /** @stable ICU 54 */
    USCRIPT_HATRAN = 162,/* Hatr */
    /** @stable ICU 54 */
    USCRIPT_MODI = 163,/* Modi */
    /** @stable ICU 54 */
    USCRIPT_MULTANI = 164,/* Mult */
    /** @stable ICU 54 */
    USCRIPT_PAU_CIN_HAU = 165,/* Pauc */
    /** @stable ICU 54 */
    USCRIPT_SIDDHAM = 166,/* Sidd */

    /* Private use codes from Qaaa - Qabx are not supported */

    /** @stable ICU 2.2 */
    USCRIPT_CODE_LIMIT = 167
} UScriptCode;


#endif // uscript_h