/*
******************************************************************************
*
*   Copyright (C) 2001-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  utrie2.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2008aug16 (starting from a copy of utrie.h)
*   created by: Markus W. Scherer
*/

#ifndef __UTRIE2_H__
#define __UTRIE2_H__

/** Build-time trie structure. */
struct UNewTrie2;
typedef struct UNewTrie2 UNewTrie2;

/*
* Trie structure definition.
*
* Either the data table is 16 bits wide and accessed via the index
* pointer, with each index item increased by indexLength;
* in this case, data32==NULL, and data16 is used for direct ASCII access.
*
* Or the data table is 32 bits wide and accessed via the data32 pointer.
*/
struct UTrie2 {
    /* protected: used by macros and functions for reading values */
    const uint16_t *index;
    const uint16_t *data16;     /* for fast UTF-8 ASCII access, if 16b data */
    const uint32_t *data32;     /* NULL if 16b data is used via index */

    int32_t indexLength, dataLength;
    uint16_t index2NullOffset;  /* 0xffff if there is no dedicated index-2 null block */
    uint16_t dataNullOffset;
    uint32_t initialValue;
    /** Value returned for out-of-range code points and illegal UTF-8. */
    uint32_t errorValue;

    /* Start of the last range which ends at U+10ffff, and its value. */
    UChar32 highStart;
    int32_t highValueIndex;

    /* private: used by builder and unserialization functions */
    void *memory;           /* serialized bytes; NULL if not frozen yet */
    int32_t length;         /* number of serialized bytes at memory; 0 if not frozen yet */
    UBool isMemoryOwned;    /* TRUE if the trie owns the memory */
    UBool padding1;
    int16_t padding2;
    UNewTrie2 *newTrie;     /* builder object; NULL when frozen */
};

/**
* Trie constants, defining shift widths, index array lengths, etc.
*
* These are needed for the runtime macros but users can treat these as
* implementation details and skip to the actual public API further below.
*/
enum {
    /** Shift size for getting the index-1 table offset. */
    UTRIE2_SHIFT_1 = 6 + 5,

    /** Shift size for getting the index-2 table offset. */
    UTRIE2_SHIFT_2 = 5,

    /**
    * Difference between the two shift sizes,
    * for getting an index-1 offset from an index-2 offset. 6=11-5
    */
    UTRIE2_SHIFT_1_2 = UTRIE2_SHIFT_1 - UTRIE2_SHIFT_2,

    /**
    * Number of index-1 entries for the BMP. 32=0x20
    * This part of the index-1 table is omitted from the serialized form.
    */
    UTRIE2_OMITTED_BMP_INDEX_1_LENGTH = 0x10000 >> UTRIE2_SHIFT_1,

    /** Number of code points per index-1 table entry. 2048=0x800 */
    UTRIE2_CP_PER_INDEX_1_ENTRY = 1 << UTRIE2_SHIFT_1,

    /** Number of entries in an index-2 block. 64=0x40 */
    UTRIE2_INDEX_2_BLOCK_LENGTH = 1 << UTRIE2_SHIFT_1_2,

    /** Mask for getting the lower bits for the in-index-2-block offset. */
    UTRIE2_INDEX_2_MASK = UTRIE2_INDEX_2_BLOCK_LENGTH - 1,

    /** Number of entries in a data block. 32=0x20 */
    UTRIE2_DATA_BLOCK_LENGTH = 1 << UTRIE2_SHIFT_2,

    /** Mask for getting the lower bits for the in-data-block offset. */
    UTRIE2_DATA_MASK = UTRIE2_DATA_BLOCK_LENGTH - 1,

    /**
    * Shift size for shifting left the index array values.
    * Increases possible data size with 16-bit index values at the cost
    * of compactability.
    * This requires data blocks to be aligned by UTRIE2_DATA_GRANULARITY.
    */
    UTRIE2_INDEX_SHIFT = 2,

    /** The alignment size of a data block. Also the granularity for compaction. */
    UTRIE2_DATA_GRANULARITY = 1 << UTRIE2_INDEX_SHIFT,

    /* Fixed layout of the first part of the index array. ------------------- */

    /**
    * The BMP part of the index-2 table is fixed and linear and starts at offset 0.
    * Length=2048=0x800=0x10000>>UTRIE2_SHIFT_2.
    */
    UTRIE2_INDEX_2_OFFSET = 0,

    /**
    * The part of the index-2 table for U+D800..U+DBFF stores values for
    * lead surrogate code _units_ not code _points_.
    * Values for lead surrogate code _points_ are indexed with this portion of the table.
    * Length=32=0x20=0x400>>UTRIE2_SHIFT_2. (There are 1024=0x400 lead surrogates.)
    */
    UTRIE2_LSCP_INDEX_2_OFFSET = 0x10000 >> UTRIE2_SHIFT_2,
    UTRIE2_LSCP_INDEX_2_LENGTH = 0x400 >> UTRIE2_SHIFT_2,

    /** Count the lengths of both BMP pieces. 2080=0x820 */
    UTRIE2_INDEX_2_BMP_LENGTH = UTRIE2_LSCP_INDEX_2_OFFSET + UTRIE2_LSCP_INDEX_2_LENGTH,

    /**
    * The 2-byte UTF-8 version of the index-2 table follows at offset 2080=0x820.
    * Length 32=0x20 for lead bytes C0..DF, regardless of UTRIE2_SHIFT_2.
    */
    UTRIE2_UTF8_2B_INDEX_2_OFFSET = UTRIE2_INDEX_2_BMP_LENGTH,
    UTRIE2_UTF8_2B_INDEX_2_LENGTH = 0x800 >> 6,  /* U+0800 is the first code point after 2-byte UTF-8 */

    /**
    * The index-1 table, only used for supplementary code points, at offset 2112=0x840.
    * Variable length, for code points up to highStart, where the last single-value range starts.
    * Maximum length 512=0x200=0x100000>>UTRIE2_SHIFT_1.
    * (For 0x100000 supplementary code points U+10000..U+10ffff.)
    *
    * The part of the index-2 table for supplementary code points starts
    * after this index-1 table.
    *
    * Both the index-1 table and the following part of the index-2 table
    * are omitted completely if there is only BMP data.
    */
    UTRIE2_INDEX_1_OFFSET = UTRIE2_UTF8_2B_INDEX_2_OFFSET + UTRIE2_UTF8_2B_INDEX_2_LENGTH,
    UTRIE2_MAX_INDEX_1_LENGTH = 0x100000 >> UTRIE2_SHIFT_1,

    /*
    * Fixed layout of the first part of the data array. -----------------------
    * Starts with 4 blocks (128=0x80 entries) for ASCII.
    */

    /**
    * The illegal-UTF-8 data block follows the ASCII block, at offset 128=0x80.
    * Used with linear access for single bytes 0..0xbf for simple error handling.
    * Length 64=0x40, not UTRIE2_DATA_BLOCK_LENGTH.
    */
    UTRIE2_BAD_UTF8_DATA_OFFSET = 0x80,

    /** The start of non-linear-ASCII data blocks, at offset 192=0xc0. */
    UTRIE2_DATA_START_OFFSET = 0xc0
};


/** Internal low-level trie getter. Returns a data index. */
#define _UTRIE2_INDEX_RAW(offset, trieIndex, c) \
    (((int32_t)((trieIndex)[(offset)+((c)>>UTRIE2_SHIFT_2)]) \
    <<UTRIE2_INDEX_SHIFT)+ \
    ((c)&UTRIE2_DATA_MASK))

/** Internal trie getter from a UTF-16 single/lead code unit. Returns the data index. */
#define _UTRIE2_INDEX_FROM_U16_SINGLE_LEAD(trieIndex, c) _UTRIE2_INDEX_RAW(0, trieIndex, c)

/** Internal trie getter from a UTF-16 single/lead code unit. Returns the data. */
#define _UTRIE2_GET_FROM_U16_SINGLE_LEAD(trie, data, c) \
    (trie)->data[_UTRIE2_INDEX_FROM_U16_SINGLE_LEAD((trie)->index, c)]

/** Internal trie getter from a supplementary code point below highStart. Returns the data index. */
#define _UTRIE2_INDEX_FROM_SUPP(trieIndex, c) \
    (((int32_t)((trieIndex)[ \
        (trieIndex)[(UTRIE2_INDEX_1_OFFSET-UTRIE2_OMITTED_BMP_INDEX_1_LENGTH)+ \
                      ((c)>>UTRIE2_SHIFT_1)]+ \
        (((c)>>UTRIE2_SHIFT_2)&UTRIE2_INDEX_2_MASK)]) \
    <<UTRIE2_INDEX_SHIFT)+ \
    ((c)&UTRIE2_DATA_MASK))

/**
* Internal trie getter from a code point, with checking that c is in 0..10FFFF.
* Returns the data index.
*/
#define _UTRIE2_INDEX_FROM_CP(trie, asciiOffset, c) \
    ((uint32_t)(c)<0xd800 ? \
        _UTRIE2_INDEX_RAW(0, (trie)->index, c) : \
        (uint32_t)(c)<=0xffff ? \
            _UTRIE2_INDEX_RAW( \
                (c)<=0xdbff ? UTRIE2_LSCP_INDEX_2_OFFSET-(0xd800>>UTRIE2_SHIFT_2) : 0, \
                (trie)->index, c) : \
            (uint32_t)(c)>0x10ffff ? \
                (asciiOffset)+UTRIE2_BAD_UTF8_DATA_OFFSET : \
                (c)>=(trie)->highStart ? \
                    (trie)->highValueIndex : \
                    _UTRIE2_INDEX_FROM_SUPP((trie)->index, c))

/**
* Internal trie getter from a code point, with checking that c is in 0..10FFFF.
* Returns the data.
*/
#define _UTRIE2_GET(trie, data, asciiOffset, c) \
    (trie)->data[_UTRIE2_INDEX_FROM_CP(trie, asciiOffset, c)]

#define UTRIE2_GET16(trie, c) _UTRIE2_GET((trie), index, (trie)->indexLength, (c))

#endif // __UTRIE2_H__