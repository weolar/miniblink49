
#include "config.h"
#include <ctype.h>

#include "QtGlobal.h"
#include "qunicodetables.h"
#include "UnicodeQt4.h"

namespace WTF {
namespace Unicode {

int toLower(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
{
    int i = 0;
    for (i = 0; i < resultLength && i < srcLength; ++i) {
        result[i] = tolower(src[i]);
    }
    *error = false;
    return i;
}

int toUpper(UChar* result, int resultLength, const UChar* src, int srcLength, bool* error)
{
    int i = 0;
    for (i = 0; i < resultLength && i < srcLength; ++i) {
        result[i] = toupper(src[i]);
    }
    *error = false;
    return i;
}

bool isChineseUtf16Char(UChar c)
{
    return 0x4E00 <= c && c <= 0x9Fff;
}

} // namespace Unicode
} // namespace WTF
/*!
Returns the case folded equivalent of the character. For most Unicode characters this
is the same as toLowerCase().
*/
QChar QChar::toCaseFolded() const
{
    return ucs + QUnicodeTables::qGetProp(ucs)->caseFoldDiff;
}

/*!
\overload
Returns the case folded equivalent of the UCS-4-encoded character specified
by \a ucs4. For most Unicode characters this is the same as toLowerCase().
*/
uint QChar::toCaseFolded(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return ucs4;
    return ucs4 + QUnicodeTables::qGetProp(ucs4)->caseFoldDiff;
}

/*!
\overload
Returns the case folded equivalent of the UCS-2-encoded character specified
by \a ucs2. For most Unicode characters this is the same as toLowerCase().
*/
ushort QChar::toCaseFolded(ushort ucs2)
{
    return ucs2 + QUnicodeTables::qGetProp(ucs2)->caseFoldDiff;
}

/*!
Returns the lowercase equivalent if the character is uppercase or titlecase;
otherwise returns the character itself.
*/
QChar QChar::toLower() const
{
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs);
    if (!p->lowerCaseSpecial)
        return ucs + p->lowerCaseDiff;
    return ucs;
}

/*!
\overload
Returns the lowercase equivalent of the UCS-4-encoded character specified
by \a ucs4 if the character is uppercase or titlecase; otherwise returns
the character itself.
*/
uint QChar::toLower(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return ucs4;
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs4);
    if (!p->lowerCaseSpecial)
        return ucs4 + p->lowerCaseDiff;
    return ucs4;
}

/*!
\overload
Returns the lowercase equivalent of the UCS-2-encoded character specified
by \a ucs2 if the character is uppercase or titlecase; otherwise returns
the character itself.
*/
ushort QChar::toLower(ushort ucs2)
{
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs2);
    if (!p->lowerCaseSpecial)
        return ucs2 + p->lowerCaseDiff;
    return ucs2;
}

/*!
Returns the uppercase equivalent if the character is lowercase or titlecase;
otherwise returns the character itself.
*/
QChar QChar::toUpper() const
{
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs);
    if (!p->upperCaseSpecial)
        return ucs + p->upperCaseDiff;
    return ucs;
}

/*!
\overload
Returns the uppercase equivalent of the UCS-4-encoded character specified
by \a ucs4 if the character is lowercase or titlecase; otherwise returns
the character itself.
*/
uint QChar::toUpper(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return ucs4;
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs4);
    if (!p->upperCaseSpecial)
        return ucs4 + p->upperCaseDiff;
    return ucs4;
}

/*!
\overload
Returns the uppercase equivalent of the UCS-2-encoded character specified
by \a ucs2 if the character is lowercase or titlecase; otherwise returns
the character itself.
*/
ushort QChar::toUpper(ushort ucs2)
{
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs2);
    if (!p->upperCaseSpecial)
        return ucs2 + p->upperCaseDiff;
    return ucs2;
}

/*!
Returns the title case equivalent if the character is lowercase or uppercase;
otherwise returns the character itself.
*/
QChar QChar::toTitleCase() const
{
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs);
    if (!p->titleCaseSpecial)
        return ucs + p->titleCaseDiff;
    return ucs;
}

/*!
\overload
Returns the title case equivalent of the UCS-4-encoded character specified
by \a ucs4 if the character is lowercase or uppercase; otherwise returns
the character itself.
*/
uint QChar::toTitleCase(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return ucs4;
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs4);
    if (!p->titleCaseSpecial)
        return ucs4 + p->titleCaseDiff;
    return ucs4;
}

/*!
\overload
Returns the title case equivalent of the UCS-2-encoded character specified
by \a ucs2 if the character is lowercase or uppercase; otherwise returns
the character itself.
*/
ushort QChar::toTitleCase(ushort ucs2)
{
    const QUnicodeTables::Properties *p = QUnicodeTables::qGetProp(ucs2);
    if (!p->titleCaseSpecial)
        return ucs2 + p->titleCaseDiff;
    return ucs2;
}

/*!
Returns the character's direction.
*/
QChar::Direction QChar::direction() const
{
    return (QChar::Direction) QUnicodeTables::qGetProp(ucs)->direction;
}

/*!
\overload
Returns the direction of the UCS-4-encoded character specified by \a ucs4.
*/
QChar::Direction QChar::direction(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return QChar::DirL;
    return (QChar::Direction) QUnicodeTables::qGetProp(ucs4)->direction;
}

/*!
\overload
Returns the direction of the UCS-2-encoded character specified by \a ucs2.
*/
QChar::Direction QChar::direction(ushort ucs2)
{
    return (QChar::Direction) QUnicodeTables::qGetProp(ucs2)->direction;
}

/*!
\overload
Returns the category of the UCS-2-encoded character specified by \a ucs2.
*/
QChar::Category QChar::category(ushort ucs2)
{
    return (QChar::Category) QUnicodeTables::qGetProp(ucs2)->category;
}

/*!
\overload
\since 4.3
Returns the category of the UCS-4-encoded character specified by \a ucs4.
*/
QChar::Category QChar::category(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return QChar::NoCategory;
    return (QChar::Category) QUnicodeTables::qGetProp(ucs4)->category;
}

/*!
Returns the mirrored character if this character is a mirrored
character; otherwise returns the character itself.

\sa hasMirrored()
*/
QChar QChar::mirroredChar() const
{
    return ucs + QUnicodeTables::qGetProp(ucs)->mirrorDiff;
}

/*!
\overload
Returns the mirrored character if the UCS-4-encoded character specified
by \a ucs4 is a mirrored character; otherwise returns the character itself.

\sa hasMirrored()
*/
uint QChar::mirroredChar(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return ucs4;
    return ucs4 + QUnicodeTables::qGetProp(ucs4)->mirrorDiff;
}

/*!
\overload
Returns the mirrored character if the UCS-2-encoded character specified
by \a ucs2 is a mirrored character; otherwise returns the character itself.

\sa hasMirrored()
*/
ushort QChar::mirroredChar(ushort ucs2)
{
    return ucs2 + QUnicodeTables::qGetProp(ucs2)->mirrorDiff;
}

/*!
Returns the tag defining the composition of the character. Returns
QChar::Single if no decomposition exists.
*/
QChar::Decomposition QChar::decompositionTag() const
{
    return decompositionTag(ucs);
}


/*!
\overload
Returns the tag defining the composition of the UCS-4-encoded character
specified by \a ucs4. Returns QChar::Single if no decomposition exists.
*/
QChar::Decomposition QChar::decompositionTag(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return QChar::NoDecomposition;
    const unsigned short index = GET_DECOMPOSITION_INDEX(ucs4);
    if (index == 0xffff)
        return QChar::NoDecomposition;
    return (QChar::Decomposition)(QUnicodeTables::uc_decomposition_map[index] & 0xff);
}

/*!
Returns the combining class for the character as defined in the
Unicode standard. This is mainly useful as a positioning hint for
marks attached to a base character.

The Qt text rendering engine uses this information to correctly
position non-spacing marks around a base character.
*/
unsigned char QChar::combiningClass() const
{
    return (unsigned char) QUnicodeTables::qGetProp(ucs)->combiningClass;
}

/*!
\overload
Returns the combining class for the UCS-4-encoded character specified by
\a ucs4, as defined in the Unicode standard.
*/
unsigned char QChar::combiningClass(uint ucs4)
{
    if (ucs4 > UNICODE_LAST_CODEPOINT)
        return 0;
    return (unsigned char) QUnicodeTables::qGetProp(ucs4)->combiningClass;
}

/*!
\overload
Returns the combining class for the UCS-2-encoded character specified by
\a ucs2, as defined in the Unicode standard.
*/
unsigned char QChar::combiningClass(ushort ucs2)
{
    return (unsigned char) QUnicodeTables::qGetProp(ucs2)->combiningClass;
}