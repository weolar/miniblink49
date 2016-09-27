/**************************************************************************\
*
* Copyright (c) 1998-2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusStringFormat.h
*
* Abstract:
*
*   String format specification for DrawString and text APIs
*
\**************************************************************************/

#ifndef _GDIPLUSSTRINGFORMAT_H
#define _GDIPLUSSTRINGFORMAT_H


class StringFormat : public GdiplusBase
{
public:
    friend class Graphics;
    friend class GraphicsPath;


    StringFormat(
        IN INT     formatFlags = 0,
        IN LANGID  language = LANG_NEUTRAL
    )
    {
        nativeFormat = NULL;
        lastError = DllExports::GdipCreateStringFormat(
            formatFlags,
            language,
            &nativeFormat
        );
    }

    static const StringFormat *GenericDefault();
    static const StringFormat *GenericTypographic();

    // Constructor based on existing string format

    StringFormat(
        IN const StringFormat *format
    )
    {
        nativeFormat = NULL;
        lastError = DllExports::GdipCloneStringFormat(
            format ? format->nativeFormat : NULL,
            &nativeFormat
        );
    }

    StringFormat *Clone() const
    {
        GpStringFormat *clonedStringFormat = NULL;

        lastError = DllExports::GdipCloneStringFormat(
            nativeFormat,
            &clonedStringFormat
        );

        if (lastError == Ok)
            return new StringFormat(clonedStringFormat, lastError);
        else
            return NULL;
    }

    ~StringFormat()
    {
        DllExports::GdipDeleteStringFormat(nativeFormat);
    }

    Status SetFormatFlags(IN INT flags)
    {
        return SetStatus(DllExports::GdipSetStringFormatFlags(
            nativeFormat,
            flags
        ));
    }

    INT GetFormatFlags() const
    {
        INT flags;
        SetStatus(DllExports::GdipGetStringFormatFlags(nativeFormat, &flags));
        return flags;
    }

#ifndef DCR_USE_NEW_152154
    Status SetLineSpacing(
        IN REAL        amount = 1.0f,
        IN LineSpacing method = LineSpacingRecommended
    )
    {
        return SetStatus(DllExports::GdipSetStringFormatLineSpacing(
            nativeFormat,
            amount,
            method
        ));
    }
#endif

    Status SetAlignment(IN StringAlignment align)
    {
        return SetStatus(DllExports::GdipSetStringFormatAlign(
            nativeFormat,
            align
        ));
    }

    StringAlignment GetAlignment() const
    {
        StringAlignment alignment;
        SetStatus(DllExports::GdipGetStringFormatAlign(
            nativeFormat,
            &alignment
        ));
        return alignment;
    }

    Status SetLineAlignment(IN StringAlignment align)
    {
        return SetStatus(DllExports::GdipSetStringFormatLineAlign(
            nativeFormat,
            align
        ));
    }

    StringAlignment GetLineAlignment() const
    {
        StringAlignment alignment;
        SetStatus(DllExports::GdipGetStringFormatLineAlign(
            nativeFormat,
            &alignment
        ));
        return alignment;
    }

    Status SetHotkeyPrefix(IN HotkeyPrefix hotkeyPrefix)
    {
        return SetStatus(DllExports::GdipSetStringFormatHotkeyPrefix(
            nativeFormat,
            (INT)hotkeyPrefix
        ));
    }

    HotkeyPrefix GetHotkeyPrefix() const
    {
        HotkeyPrefix hotkeyPrefix;
        SetStatus(DllExports::GdipGetStringFormatHotkeyPrefix(
            nativeFormat,
            (INT*)&hotkeyPrefix
        ));
        return hotkeyPrefix;
    }

    Status SetTabStops(
        IN REAL    firstTabOffset,
        IN INT     count,
        IN const REAL    *tabStops
    )
    {
        return SetStatus(DllExports::GdipSetStringFormatTabStops(
            nativeFormat,
            firstTabOffset,
            count,
            tabStops
        ));
    }

    INT GetTabStopCount() const
    {
        INT count;
        SetStatus(DllExports::GdipGetStringFormatTabStopCount(nativeFormat, &count));
        return count;
    }

    Status GetTabStops(
        IN INT     count,
        OUT REAL   *firstTabOffset,
        OUT REAL   *tabStops
    ) const
    {
        return SetStatus(DllExports::GdipGetStringFormatTabStops(
            nativeFormat,
            count,
            firstTabOffset,
            tabStops
        ));
    }

#ifdef DCR_USE_NEW_146933
    Status SetDigitSubstitution(
        IN LANGID                language,
        IN StringDigitSubstitute substitute
    )
    {
        return SetStatus(DllExports::GdipSetStringFormatDigitSubstitution(
            nativeFormat,
            language,
            substitute
        ));
    }

    LANGID GetDigitSubstitutionLanguage(
    ) const
    {
        LANGID language;
        SetStatus(DllExports::GdipGetStringFormatDigitSubstitution(
            nativeFormat,
            &language,
            NULL
        ));
        return language;
    }

    StringDigitSubstitute GetDigitSubstitutionMethod(
    ) const
    {
        StringDigitSubstitute substitute;
        SetStatus(DllExports::GdipGetStringFormatDigitSubstitution(
            nativeFormat,
            NULL,
            &substitute
        ));
        return substitute;
    }
#endif // DCR_USE_NEW_146933

    // String trimming. How to handle more text than can be displayed
    // in the limits available.

    Status SetTrimming(IN StringTrimming trimming)
    {
        return SetStatus(DllExports::GdipSetStringFormatTrimming(
            nativeFormat,
            trimming
        ));
    }

    StringTrimming StringFormat::GetTrimming() const
    {
        StringTrimming trimming;
        SetStatus(DllExports::GdipGetStringFormatTrimming(
            nativeFormat,
            &trimming
        ));
        return trimming;
    }

#ifdef DCR_USE_NEW_174340
    Status SetMeasurableCharacterRanges(
        IN INT                  rangeCount,
        IN const CharacterRange *ranges
    )
    {
        return SetStatus(DllExports::GdipSetStringFormatMeasurableCharacterRanges(
            nativeFormat,
            rangeCount,
            ranges
        ));
    }

    INT GetMeasurableCharacterRangeCount()
    {
        INT count;
        SetStatus(DllExports::GdipGetStringFormatMeasurableCharacterRangeCount(
            nativeFormat,
            &count
        ));
        return count;
    }
#endif

    // GetLastStatus - return last error code and clear error code

    Status GetLastStatus() const
    {
        Status lastStatus = lastError;
        lastError = Ok;

        return lastStatus;
    }

protected:

    Status SetStatus(GpStatus newStatus) const
    {
        if (newStatus == Ok)
        {
            return Ok;
        }
        else
        {
            return lastError = newStatus;
        }
    }


// Not allowed and move to private
    StringFormat(const StringFormat &source)
    {
        nativeFormat = NULL;
        lastError = DllExports::GdipCloneStringFormat(
            source.nativeFormat,
            &nativeFormat
        );
    }

    StringFormat& operator=(const StringFormat &source)
    {
        DllExports::GdipDeleteStringFormat(nativeFormat);
        lastError = DllExports::GdipCloneStringFormat(
            source.nativeFormat,
            &nativeFormat
        );
        return *this;
    }


    // private constructor for copy
    StringFormat(GpStringFormat * clonedStringFormat, Status status)
    {
        lastError = status;
        nativeFormat = clonedStringFormat;

    }

    GpStringFormat *nativeFormat;
    mutable Status  lastError;
};

// Generic constant string formats.

static BYTE GenericTypographicStringFormatBuffer[sizeof(StringFormat)] = {0};
static BYTE GenericDefaultStringFormatBuffer[sizeof(StringFormat)] = {0};

static StringFormat *GenericTypographicStringFormat = NULL;
static StringFormat *GenericDefaultStringFormat     = NULL;

// Define the generic string formats


inline const StringFormat *StringFormat::GenericDefault()
{
    if (GenericDefaultStringFormat != NULL)
    {
        return GenericDefaultStringFormat;
    }

    GenericDefaultStringFormat =
        (StringFormat*)GenericDefaultStringFormatBuffer;

    GenericDefaultStringFormat->lastError =
        DllExports::GdipStringFormatGetGenericDefault(
            &(GenericDefaultStringFormat->nativeFormat)
        );

    return GenericDefaultStringFormat;
}

inline const StringFormat *StringFormat::GenericTypographic()
{
    if (GenericTypographicStringFormat != NULL)
    {
        return GenericTypographicStringFormat;
    }

    GenericTypographicStringFormat =
        (StringFormat*)GenericTypographicStringFormatBuffer;

    GenericTypographicStringFormat->lastError =
        DllExports::GdipStringFormatGetGenericTypographic(
            &GenericTypographicStringFormat->nativeFormat
        );

    return GenericTypographicStringFormat;
}

#endif // !_GDIPLUSSTRINGFORMAT_H
