/**************************************************************************\
*
* Copyright (c) 1998-2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusMetafile.h
*
* Abstract:
*
*   Metafile related declarations
*
\**************************************************************************/

#ifndef _GDIPLUSMETAFILE_H
#define _GDIPLUSMETAFILE_H

class Metafile : public Image
{
public:
    friend class Image;

    // Read a metafile
    Metafile()
    {
        SetNativeImage(NULL);
        lastResult = Ok;
    }

    // Playback a metafile from a HMETAFILE
    // If deleteWmf is TRUE, then when the metafile is deleted,
    // the hWmf will also be deleted.  Otherwise, it won't be.
    Metafile(IN HMETAFILE hWmf,
             IN const APMFileHeader * apmFileHeader,
             IN BOOL deleteWmf = FALSE)
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipCreateMetafileFromWmf(hWmf, deleteWmf, apmFileHeader, &metafile);

        SetNativeImage(metafile);
    }

    // Playback a metafile from a HENHMETAFILE
    // If deleteEmf is TRUE, then when the metafile is deleted,
    // the hEmf will also be deleted.  Otherwise, it won't be.
    Metafile(IN HENHMETAFILE hEmf,
             IN BOOL deleteEmf = FALSE)
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipCreateMetafileFromEmf(hEmf, deleteEmf, &metafile);

        SetNativeImage(metafile);
    }

    // Playback a metafile from a file
    Metafile(IN const WCHAR* filename)
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipCreateMetafileFromFile(filename, &metafile);

        SetNativeImage(metafile);
    }

    // Playback a WMF metafile from a file
    Metafile(IN const WCHAR* filename,
             IN const APMFileHeader * apmFileHeader
            )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipCreateMetafileFromWmfFile(filename, apmFileHeader, &metafile);

        SetNativeImage(metafile);
    }

    // Playback a metafile from a stream
    Metafile(IN IStream* stream)
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipCreateMetafileFromStream(stream, &metafile);

        SetNativeImage(metafile);
    }

    // Record a metafile to memory
    Metafile(
        IN HDC                 referenceHdc,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafile(
                        referenceHdc, type, NULL, MetafileFrameUnitGdi,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    // Record a metafile to memory
    Metafile(
        IN HDC                 referenceHdc,
        IN const RectF &       frameRect,
        IN MetafileFrameUnit   frameUnit   = MetafileFrameUnitGdi,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafile(
                        referenceHdc, type, &frameRect, frameUnit,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    // Record a metafile to memory
    Metafile(
        IN HDC                 referenceHdc,
        IN const Rect &        frameRect,
        IN MetafileFrameUnit   frameUnit   = MetafileFrameUnitGdi,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafileI(
                        referenceHdc, type, &frameRect, frameUnit,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    // Record a metafile to a file
    Metafile(
        IN const WCHAR*        fileName,
        IN HDC                 referenceHdc,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafileFileName(fileName,
                        referenceHdc, type, NULL, MetafileFrameUnitGdi,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    // Record a metafile to a file
    Metafile(
        IN const WCHAR*        fileName,
        IN HDC                 referenceHdc,
        IN const RectF &       frameRect,
        IN MetafileFrameUnit   frameUnit   = MetafileFrameUnitGdi,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafileFileName(fileName,
                        referenceHdc, type, &frameRect, frameUnit,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    // Record a metafile to a file
    Metafile(
        IN const WCHAR*        fileName,
        IN HDC                 referenceHdc,
        IN const Rect &        frameRect,
        IN MetafileFrameUnit   frameUnit   = MetafileFrameUnitGdi,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafileFileNameI(fileName,
                        referenceHdc, type, &frameRect, frameUnit,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    // Record a metafile to a stream
    Metafile(
        IN IStream *           stream,
        IN HDC                 referenceHdc,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafileStream(stream,
                        referenceHdc, type, NULL, MetafileFrameUnitGdi,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    // Record a metafile to a stream
    Metafile(
        IN IStream *           stream,
        IN HDC                 referenceHdc,
        IN const RectF &       frameRect,
        IN MetafileFrameUnit   frameUnit   = MetafileFrameUnitGdi,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafileStream(stream,
                        referenceHdc, type, &frameRect, frameUnit,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    // Write a metafile to a stream with down-level GDI records
    Metafile(
        IN IStream *           stream,
        IN HDC                 referenceHdc,
        IN const Rect &        frameRect,
        IN MetafileFrameUnit   frameUnit   = MetafileFrameUnitGdi,
        IN EmfType             type        = EmfTypeEmfPlusDual,
        IN const WCHAR *       description = NULL
        )
    {
        GpMetafile *    metafile = NULL;

        lastResult = DllExports::GdipRecordMetafileStreamI(stream,
                        referenceHdc, type, &frameRect, frameUnit,
                        description, &metafile);

        SetNativeImage(metafile);
    }

    static Status GetMetafileHeader(
        IN HMETAFILE           hWmf,
        IN const APMFileHeader *     apmFileHeader,
        OUT MetafileHeader *   header
        )
    {
        return DllExports::GdipGetMetafileHeaderFromWmf(hWmf, apmFileHeader, header);
    }

    static Status GetMetafileHeader(
        IN HENHMETAFILE        hEmf,
        OUT MetafileHeader *   header
        )
    {
        return DllExports::GdipGetMetafileHeaderFromEmf(hEmf, header);
    }

    static Status GetMetafileHeader(
        IN const WCHAR*        filename,
        OUT MetafileHeader *   header
        )
    {
        return DllExports::GdipGetMetafileHeaderFromFile(filename, header);
    }

    static Status GetMetafileHeader(
        IN IStream *           stream,
        OUT MetafileHeader *   header
        )
    {
        return DllExports::GdipGetMetafileHeaderFromStream(stream, header);
    }

    Status GetMetafileHeader(
        OUT MetafileHeader *    header
        ) const
    {
        return SetStatus(DllExports::GdipGetMetafileHeaderFromMetafile(
                                              (GpMetafile *)nativeImage,
                                              header));
    }

    // Once this method is called, the Metafile object is in an invalid state
    // and can no longer be used.  It is the responsiblity of the caller to
    // invoke DeleteEnhMetaFile to delete this hEmf.

    HENHMETAFILE GetHENHMETAFILE()
    {
        HENHMETAFILE hEmf;

        SetStatus(DllExports::GdipGetHemfFromMetafile((GpMetafile *)nativeImage, &hEmf));

        return hEmf;
    }

    // Used in conjuction with Graphics::EnumerateMetafile to play an EMF+
    // The data must be DWORD aligned if it's an EMF or EMF+.  It must be
    // WORD aligned if it's a WMF.
    Status
    PlayRecord(
        IN EmfPlusRecordType   recordType,
        IN UINT                flags,
        IN UINT                dataSize,
        IN const BYTE *        data
        ) const
    {
        return SetStatus(DllExports::GdipPlayMetafileRecord(
                                (GpMetafile *)nativeImage,
                                recordType,
                                flags,
                                dataSize,
                                data));
    }

    // If you're using a printer HDC for the metafile, but you want the
    // metafile rasterized at screen resolution, then use this API to set
    // the rasterization dpi of the metafile to the screen resolution,
    // e.g. 96 dpi or 120 dpi.
    Status SetDownLevelRasterizationLimit(
        IN UINT     metafileRasterizationLimitDpi
        )
    {
        return SetStatus(DllExports::GdipSetMetafileDownLevelRasterizationLimit(
                                (GpMetafile *)nativeImage,
                                metafileRasterizationLimitDpi));
    }

    UINT GetDownLevelRasterizationLimit() const
    {
        UINT    metafileRasterizationLimitDpi = 0;

        SetStatus(DllExports::GdipGetMetafileDownLevelRasterizationLimit(
                                (GpMetafile *)nativeImage,
                                &metafileRasterizationLimitDpi));

        return metafileRasterizationLimitDpi;
    }

    static UINT Metafile::EmfToWmfBits(
        IN HENHMETAFILE       hemf,
        IN UINT               cbData16,
        IN LPBYTE             pData16,
        IN INT                iMapMode = MM_ANISOTROPIC,
        IN EmfToWmfBitsFlags  eFlags = EmfToWmfBitsFlagsDefault
    )
    {
        return DllExports::GdipEmfToWmfBits(
            hemf,
            cbData16,
            pData16,
            iMapMode,
            eFlags);
    }

#ifdef DCR_USE_NEW_250932

private:
    Metafile(const Metafile &);
    Metafile& operator=(const Metafile &);

#endif
};

#endif // !_METAFILE_H
