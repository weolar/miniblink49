/**************************************************************************\
* 
* Copyright (c) 1999-2000  Microsoft Corporation
*
* Module Name:
*
*   GdiplusImaging.h
*
* Abstract:
*
*   GUIDs defined and used by the imaging library
*
\**************************************************************************/
#ifndef _GDIPLUSIMAGING_H
#define _GDIPLUSIMAGING_H

//---------------------------------------------------------------------------
// Image file format identifiers
//---------------------------------------------------------------------------

DEFINE_GUID(ImageFormatUndefined, 0xb96b3ca9,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatMemoryBMP, 0xb96b3caa,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatBMP, 0xb96b3cab,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatEMF, 0xb96b3cac,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatWMF, 0xb96b3cad,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatJPEG, 0xb96b3cae,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatPNG, 0xb96b3caf,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatGIF, 0xb96b3cb0,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatTIFF, 0xb96b3cb1,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatEXIF, 0xb96b3cb2,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
#ifndef DCR_USE_NEW_140855
DEFINE_GUID(ImageFormatPhotoCD, 0xb96b3cb3,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
DEFINE_GUID(ImageFormatFlashPIX, 0xb96b3cb4,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);
#endif
DEFINE_GUID(ImageFormatIcon, 0xb96b3cb5,0x0728,0x11d3,0x9d,0x7b,0x00,0x00,0xf8,0x1e,0xf3,0x2e);

//---------------------------------------------------------------------------
// Predefined multi-frame dimension IDs
//---------------------------------------------------------------------------

DEFINE_GUID(FrameDimensionTime, 0x6aedbd6d,0x3fb5,0x418a,0x83,0xa6,0x7f,0x45,0x22,0x9d,0xc8,0x72);
DEFINE_GUID(FrameDimensionResolution, 0x84236f7b,0x3bd3,0x428f,0x8d,0xab,0x4e,0xa1,0x43,0x9c,0xa3,0x15);
DEFINE_GUID(FrameDimensionPage, 0x7462dc86,0x6180,0x4c7e,0x8e,0x3f,0xee,0x73,0x33,0xa7,0xa4,0x83);

//---------------------------------------------------------------------------
// Property sets
//---------------------------------------------------------------------------

DEFINE_GUID(FormatIDImageInformation, 0xe5836cbe,0x5eef,0x4f1d,0xac,0xde,0xae,0x4c,0x43,0xb6,0x08,0xce);
DEFINE_GUID(FormatIDJpegAppHeaders, 0x1c4afdcd,0x6177,0x43cf,0xab,0xc7,0x5f,0x51,0xaf,0x39,0xee,0x85);

#ifndef DCR_USE_NEW_140855
//---------------------------------------------------------------------------
// Decoder parameter sets
//---------------------------------------------------------------------------
DEFINE_GUID(DecoderTransColor, 0xb7a98c8f,0xdce7,0x457d,0xbf,0xa5,0xce,0xa7,0x1b,0xd1,0x4d,0xd6);
DEFINE_GUID(DecoderTransRange, 0xabeed189,0xd988,0x4d03,0xb4,0x25,0x57,0x10,0x55,0xc7,0x6a,0xd1);
DEFINE_GUID(DecoderOutputChannel, 0x2ff8f51e,0x724d,0x45fe,0x86,0xce,0x17,0x77,0xa0,0x56,0xda,0x60);
DEFINE_GUID(DecoderIconRes, 0x5c656eec,0xe94f,0x45ba,0xa6,0xf6,0x10,0x62,0xe8,0x5f,0x4a,0x7f);
#endif

//---------------------------------------------------------------------------
// Encoder parameter sets
//---------------------------------------------------------------------------

DEFINE_GUID(EncoderCompression, 0xe09d739d,0xccd4,0x44ee,0x8e,0xba,0x3f,0xbf,0x8b,0xe4,0xfc,0x58);
DEFINE_GUID(EncoderColorDepth, 0x66087055,0xad66,0x4c7c,0x9a,0x18,0x38,0xa2,0x31,0x0b,0x83,0x37);
DEFINE_GUID(EncoderScanMethod, 0x3a4e2661,0x3109,0x4e56,0x85,0x36,0x42,0xc1,0x56,0xe7,0xdc,0xfa);
DEFINE_GUID(EncoderVersion, 0x24d18c76,0x814a,0x41a4,0xbf,0x53,0x1c,0x21,0x9c,0xcc,0xf7,0x97);
DEFINE_GUID(EncoderRenderMethod, 0x6d42c53a,0x229a,0x4825,0x8b,0xb7,0x5c,0x99,0xe2,0xb9,0xa8,0xb8);
DEFINE_GUID(EncoderQuality, 0x1d5be4b5,0xfa4a,0x452d,0x9c,0xdd,0x5d,0xb3,0x51,0x05,0xe7,0xeb);
DEFINE_GUID(EncoderTransformation,0x8d0eb2d1,0xa58e,0x4ea8,0xaa,0x14,0x10,0x80,0x74,0xb7,0xb6,0xf9);
DEFINE_GUID(EncoderLuminanceTable,0xedb33bce,0x0266,0x4a77,0xb9,0x04,0x27,0x21,0x60,0x99,0xe7,0x17);
DEFINE_GUID(EncoderChrominanceTable,0xf2e455dc,0x09b3,0x4316,0x82,0x60,0x67,0x6a,0xda,0x32,0x48,0x1c);
DEFINE_GUID(EncoderSaveFlag,0x292266fc,0xac40,0x47bf,0x8c, 0xfc, 0xa8, 0x5b, 0x89, 0xa6, 0x55, 0xde);

DEFINE_GUID(CodecIImageBytes,0x025d1823,0x6c7d,0x447b,0xbb, 0xdb, 0xa3, 0xcb, 0xc3, 0xdf, 0xa2, 0xfc);

MIDL_INTERFACE("025D1823-6C7D-447B-BBDB-A3CBC3DFA2FC")
IImageBytes : public IUnknown
{
public:
    // Return total number of bytes in the IStream

    STDMETHOD(CountBytes)(
        OUT UINT *pcb
        ) = 0;
    
    // Locks "cb" bytes, starting from "ulOffset" in the stream, and returns the
    // pointer to the beginning of the locked memory chunk in "ppvBytes"

    STDMETHOD(LockBytes)(
        IN UINT cb,
        IN ULONG ulOffset,
        OUT const VOID ** ppvBytes
        ) = 0;

    // Unlocks "cb" bytes, pointed by "pvBytes", starting from "ulOffset" in the
    // stream

    STDMETHOD(UnlockBytes)(
        IN const VOID *pvBytes,
        IN UINT cb,
        IN ULONG ulOffset
        ) = 0;
};

//--------------------------------------------------------------------------
// ImageCodecInfo structure
//--------------------------------------------------------------------------

class ImageCodecInfo
{
public:          
    CLSID Clsid;
    GUID  FormatID;
    const WCHAR* CodecName;
    const WCHAR* DllName;
    const WCHAR* FormatDescription;
    const WCHAR* FilenameExtension;
    const WCHAR* MimeType;
    DWORD Flags;
    DWORD Version;
    DWORD SigCount;
    DWORD SigSize;
    const BYTE* SigPattern;
    const BYTE* SigMask;
};

//--------------------------------------------------------------------------
// Information flags about image codecs
//--------------------------------------------------------------------------

enum ImageCodecFlags
{
    ImageCodecFlagsEncoder            = 0x00000001,
    ImageCodecFlagsDecoder            = 0x00000002,
    ImageCodecFlagsSupportBitmap      = 0x00000004,
    ImageCodecFlagsSupportVector      = 0x00000008,
    ImageCodecFlagsSeekableEncode     = 0x00000010,
    ImageCodecFlagsBlockingDecode     = 0x00000020,

    ImageCodecFlagsBuiltin            = 0x00010000,
    ImageCodecFlagsSystem             = 0x00020000,
    ImageCodecFlagsUser               = 0x00040000
};

//---------------------------------------------------------------------------
// Access modes used when calling Image::LockBits
//---------------------------------------------------------------------------

enum ImageLockMode
{
    ImageLockModeRead        = 0x0001,
    ImageLockModeWrite       = 0x0002,
    ImageLockModeUserInputBuf= 0x0004
};

//---------------------------------------------------------------------------
// Information about image pixel data
//---------------------------------------------------------------------------

class BitmapData
{
public:
    UINT Width;
    UINT Height;
    INT Stride;
    PixelFormat PixelFormat;
    VOID* Scan0;
    UINT_PTR Reserved;
};

//---------------------------------------------------------------------------
// Image flags
//---------------------------------------------------------------------------

enum ImageFlags
{
    ImageFlagsNone                = 0,

    // Low-word: shared with SINKFLAG_x

    ImageFlagsScalable            = 0x0001,
    ImageFlagsHasAlpha            = 0x0002,
    ImageFlagsHasTranslucent      = 0x0004,
    ImageFlagsPartiallyScalable   = 0x0008,

    // Low-word: color space definition

    ImageFlagsColorSpaceRGB       = 0x0010,
    ImageFlagsColorSpaceCMYK      = 0x0020,
    ImageFlagsColorSpaceGRAY      = 0x0040,
    ImageFlagsColorSpaceYCBCR     = 0x0080,
    ImageFlagsColorSpaceYCCK      = 0x0100,
 
    // Low-word: image size info

    ImageFlagsHasRealDPI          = 0x1000,
    ImageFlagsHasRealPixelSize    = 0x2000,

    // High-word

    ImageFlagsReadOnly            = 0x00010000,
    ImageFlagsCaching             = 0x00020000
};

enum RotateFlipType
{
    RotateNoneFlipNone = 0,
    Rotate90FlipNone   = 1,
    Rotate180FlipNone  = 2,
    Rotate270FlipNone  = 3,

    RotateNoneFlipX    = 4,
    Rotate90FlipX      = 5,
    Rotate180FlipX     = 6,
    Rotate270FlipX     = 7,

    RotateNoneFlipY    = Rotate180FlipX,
    Rotate90FlipY      = Rotate270FlipX,
    Rotate180FlipY     = RotateNoneFlipX,
    Rotate270FlipY     = Rotate90FlipX,

    RotateNoneFlipXY   = Rotate180FlipNone,
    Rotate90FlipXY     = Rotate270FlipNone,
    Rotate180FlipXY    = RotateNoneFlipNone,
    Rotate270FlipXY    = Rotate90FlipNone
};

//---------------------------------------------------------------------------
// Encoder Parameter structure
//---------------------------------------------------------------------------
class EncoderParameter
{
public:
    GUID    Guid;               // GUID of the parameter
    ULONG   NumberOfValues;     // Number of the parameter values
    ULONG   Type;               // Value type, like ValueTypeLONG  etc.
    VOID*   Value;              // A pointer to the parameter values
};

//---------------------------------------------------------------------------
// Encoder Parameters structure
//---------------------------------------------------------------------------
class EncoderParameters
{
public:
    UINT Count;                      // Number of parameters in this structure
    EncoderParameter Parameter[1];   // Parameter values
};

//---------------------------------------------------------------------------
// Property Item
//---------------------------------------------------------------------------
class PropertyItem
{
public:
    PROPID  id;                 // ID of this property
    ULONG   length;             // Length of the property value, in bytes
    WORD    type;               // Type of the value, as one of TAG_TYPE_XXX
                                // defined above
    VOID*   value;              // property value
};

#ifdef DCR_USE_NEW_140857
//---------------------------------------------------------------------------
// Image property types 
//---------------------------------------------------------------------------
#define PropertyTagTypeByte        1
#define PropertyTagTypeASCII       2
#define PropertyTagTypeShort       3
#define PropertyTagTypeLong        4
#define PropertyTagTypeRational    5
#define PropertyTagTypeUndefined   7
#define PropertyTagTypeSLONG       9
#define PropertyTagTypeSRational  10
#endif

//---------------------------------------------------------------------------
// Image property ID tags
//---------------------------------------------------------------------------

#define PropertyTagExifIFD             0x8769
#define PropertyTagGpsIFD              0x8825

#define PropertyTagNewSubfileType      0x00FE
#define PropertyTagSubfileType         0x00FF
#define PropertyTagImageWidth          0x0100
#define PropertyTagImageHeight         0x0101
#define PropertyTagBitsPerSample       0x0102
#define PropertyTagCompression         0x0103
#define PropertyTagPhotometricInterp   0x0106
#define PropertyTagThreshHolding       0x0107
#define PropertyTagCellWidth           0x0108
#define PropertyTagCellHeight          0x0109
#define PropertyTagFillOrder           0x010A
#define PropertyTagDocumentName        0x010D
#define PropertyTagImageDescription    0x010E
#define PropertyTagEquipMake           0x010F
#define PropertyTagEquipModel          0x0110
#define PropertyTagStripOffsets        0x0111
#define PropertyTagOrientation         0x0112
#define PropertyTagSamplesPerPixel     0x0115
#define PropertyTagRowsPerStrip        0x0116
#define PropertyTagStripBytesCount     0x0117
#define PropertyTagMinSampleValue      0x0118
#define PropertyTagMaxSampleValue      0x0119
#define PropertyTagXResolution         0x011A   // Image resolution in width direction
#define PropertyTagYResolution         0x011B   // Image resolution in height direction
#define PropertyTagPlanarConfig        0x011C   // Image data arrangement
#define PropertyTagPageName            0x011D
#define PropertyTagXPosition           0x011E
#define PropertyTagYPosition           0x011F
#define PropertyTagFreeOffset          0x0120
#define PropertyTagFreeByteCounts      0x0121
#define PropertyTagGrayResponseUnit    0x0122
#define PropertyTagGrayResponseCurve   0x0123
#define PropertyTagT4Option            0x0124
#define PropertyTagT6Option            0x0125
#define PropertyTagResolutionUnit      0x0128   // Unit of X and Y resolution
#define PropertyTagPageNumber          0x0129
#define PropertyTagTransferFuncition   0x012D
#define PropertyTagSoftwareUsed        0x0131
#define PropertyTagDateTime            0x0132
#define PropertyTagArtist              0x013B
#define PropertyTagHostComputer        0x013C
#define PropertyTagPredictor           0x013D
#define PropertyTagWhitePoint          0x013E
#define PropertyTagPrimaryChromaticities 0x013F
#define PropertyTagColorMap            0x0140
#define PropertyTagHalftoneHints       0x0141
#define PropertyTagTileWidth           0x0142
#define PropertyTagTileLength          0x0143
#define PropertyTagTileOffset          0x0144
#define PropertyTagTileByteCounts      0x0145
#define PropertyTagInkSet              0x014C
#define PropertyTagInkNames            0x014D
#define PropertyTagNumberOfInks        0x014E
#define PropertyTagDotRange            0x0150
#define PropertyTagTargetPrinter       0x0151
#define PropertyTagExtraSamples        0x0152
#define PropertyTagSampleFormat        0x0153
#define PropertyTagSMinSampleValue     0x0154
#define PropertyTagSMaxSampleValue     0x0155
#define PropertyTagTransferRange       0x0156

#define PropertyTagJPEGProc            0x0200
#define PropertyTagJPEGInterFormat     0x0201
#define PropertyTagJPEGInterLength     0x0202
#define PropertyTagJPEGRestartInterval 0x0203
#define PropertyTagJPEGLosslessPredictors  0x0205
#define PropertyTagJPEGPointTransforms     0x0206
#define PropertyTagJPEGQTables         0x0207
#define PropertyTagJPEGDCTables        0x0208
#define PropertyTagJPEGACTables        0x0209

#define PropertyTagYCbCrCoefficients   0x0211
#define PropertyTagYCbCrSubsampling    0x0212
#define PropertyTagYCbCrPositioning    0x0213
#define PropertyTagREFBlackWhite       0x0214

#define PropertyTagICCProfile          0x8773   // This TAG is defined by ICC
                                                // for embedded ICC in TIFF
#define PropertyTagGamma               0x0301
#define PropertyTagICCProfileDescriptor 0x0302
#define PropertyTagSRGBRenderingIntent 0x0303

#define PropertyTagImageTitle          0x0320
#define PropertyTagCopyright           0x8298

// Extra TAGs (Like Adobe Image Information tags etc.)

#define PropertyTagResolutionXUnit           0x5001
#define PropertyTagResolutionYUnit           0x5002
#define PropertyTagResolutionXLengthUnit     0x5003
#define PropertyTagResolutionYLengthUnit     0x5004
#define PropertyTagPrintFlags                0x5005
#define PropertyTagPrintFlagsVersion         0x5006
#define PropertyTagPrintFlagsCrop            0x5007
#define PropertyTagPrintFlagsBleedWidth      0x5008
#define PropertyTagPrintFlagsBleedWidthScale 0x5009
#define PropertyTagHalftoneLPI               0x500A
#define PropertyTagHalftoneLPIUnit           0x500B
#define PropertyTagHalftoneDegree            0x500C
#define PropertyTagHalftoneShape             0x500D
#define PropertyTagHalftoneMisc              0x500E
#define PropertyTagHalftoneScreen            0x500F
#define PropertyTagJPEGQuality               0x5010
#define PropertyTagGridSize                  0x5011
#define PropertyTagThumbnailFormat           0x5012  // 1 = JPEG, 0 = RAW RGB
#define PropertyTagThumbnailWidth            0x5013
#define PropertyTagThumbnailHeight           0x5014
#define PropertyTagThumbnailColorDepth       0x5015
#define PropertyTagThumbnailPlanes           0x5016
#define PropertyTagThumbnailRawBytes         0x5017
#define PropertyTagThumbnailSize             0x5018
#define PropertyTagThumbnailCompressedSize   0x5019
#define PropertyTagColorTransferFunction     0x501A
#define PropertyTagThumbnailData             0x501B// RAW thumbnail bits in
                                                   // JPEG format or RGB format
                                                   // depends on
                                                   // PropertyTagThumbnailFormat

// Thumbnail related TAGs
                                                
#define PropertyTagThumbnailImageWidth       0x5020  // Thumbnail width
#define PropertyTagThumbnailImageHeight      0x5021  // Thumbnail height
#define PropertyTagThumbnailBitsPerSample    0x5022  // Number of bits per
                                                     // component
#define PropertyTagThumbnailCompression      0x5023  // Compression Scheme
#define PropertyTagThumbnailPhotometricInterp 0x5024 // Pixel composition
#define PropertyTagThumbnailImageDescription 0x5025  // Image Tile
#define PropertyTagThumbnailEquipMake        0x5026  // Manufacturer of Image
                                                     // Input equipment
#define PropertyTagThumbnailEquipModel       0x5027  // Model of Image input
                                                     // equipment
#define PropertyTagThumbnailStripOffsets     0x5028  // Image data location
#define PropertyTagThumbnailOrientation      0x5029  // Orientation of image
#define PropertyTagThumbnailSamplesPerPixel  0x502A  // Number of components
#define PropertyTagThumbnailRowsPerStrip     0x502B  // Number of rows per strip
#define PropertyTagThumbnailStripBytesCount  0x502C  // Bytes per compressed
                                                     // strip
#define PropertyTagThumbnailResolutionX      0x502D  // Resolution in width
                                                     // direction
#define PropertyTagThumbnailResolutionY      0x502E  // Resolution in height
                                                     // direction
#define PropertyTagThumbnailPlanarConfig     0x502F  // Image data arrangement
#define PropertyTagThumbnailResolutionUnit   0x5030  // Unit of X and Y
                                                     // Resolution
#define PropertyTagThumbnailTransferFunction 0x5031  // Transfer function
#define PropertyTagThumbnailSoftwareUsed     0x5032  // Software used
#define PropertyTagThumbnailDateTime         0x5033  // File change date and
                                                     // time
#define PropertyTagThumbnailArtist           0x5034  // Person who created the
                                                     // image
#define PropertyTagThumbnailWhitePoint       0x5035  // White point chromaticity
#define PropertyTagThumbnailPrimaryChromaticities 0x5036 
                                                     // Chromaticities of
                                                     // primaries
#define PropertyTagThumbnailYCbCrCoefficients 0x5037 // Color space transforma-
                                                     // tion coefficients
#define PropertyTagThumbnailYCbCrSubsampling 0x5038  // Subsampling ratio of Y
                                                     // to C
#define PropertyTagThumbnailYCbCrPositioning 0x5039  // Y and C position
#define PropertyTagThumbnailRefBlackWhite    0x503A  // Pair of black and white
                                                     // reference values
#define PropertyTagThumbnailCopyRight        0x503B  // CopyRight holder

#define PropertyTagLuminanceTable            0x5090
#define PropertyTagChrominanceTable          0x5091

#define PropertyTagFrameDelay                0x5100
#define PropertyTagLoopCount                 0x5101

#define PropertyTagPixelUnit         0x5110  // Unit specifier for pixel/unit
#define PropertyTagPixelPerUnitX     0x5111  // Pixels per unit in X
#define PropertyTagPixelPerUnitY     0x5112  // Pixels per unit in Y
#define PropertyTagPaletteHistogram  0x5113  // Palette histogram

// EXIF specific tag

#define PropertyTagExifExposureTime  0x829A
#define PropertyTagExifFNumber       0x829D

#define PropertyTagExifExposureProg  0x8822
#define PropertyTagExifSpectralSense 0x8824
#define PropertyTagExifISOSpeed      0x8827
#define PropertyTagExifOECF          0x8828

#define PropertyTagExifVer            0x9000
#define PropertyTagExifDTOrig         0x9003 // Date & time of original
#define PropertyTagExifDTDigitized    0x9004 // Date & time of digital data generation

#define PropertyTagExifCompConfig     0x9101
#define PropertyTagExifCompBPP        0x9102

#define PropertyTagExifShutterSpeed   0x9201
#define PropertyTagExifAperture       0x9202
#define PropertyTagExifBrightness     0x9203
#define PropertyTagExifExposureBias   0x9204
#define PropertyTagExifMaxAperture    0x9205
#define PropertyTagExifSubjectDist    0x9206
#define PropertyTagExifMeteringMode   0x9207
#define PropertyTagExifLightSource    0x9208
#define PropertyTagExifFlash          0x9209
#define PropertyTagExifFocalLength    0x920A
#define PropertyTagExifMakerNote      0x927C
#define PropertyTagExifUserComment    0x9286
#define PropertyTagExifDTSubsec       0x9290  // Date & Time subseconds
#define PropertyTagExifDTOrigSS       0x9291  // Date & Time original subseconds
#define PropertyTagExifDTDigSS        0x9292  // Date & TIme digitized subseconds

#define PropertyTagExifFPXVer         0xA000
#define PropertyTagExifColorSpace     0xA001
#define PropertyTagExifPixXDim        0xA002
#define PropertyTagExifPixYDim        0xA003
#define PropertyTagExifRelatedWav     0xA004  // related sound file
#define PropertyTagExifInterop        0xA005
#define PropertyTagExifFlashEnergy    0xA20B
#define PropertyTagExifSpatialFR      0xA20C  // Spatial Frequency Response
#define PropertyTagExifFocalXRes      0xA20E  // Focal Plane X Resolution
#define PropertyTagExifFocalYRes      0xA20F  // Focal Plane Y Resolution
#define PropertyTagExifFocalResUnit   0xA210  // Focal Plane Resolution Unit
#define PropertyTagExifSubjectLoc     0xA214
#define PropertyTagExifExposureIndex  0xA215
#define PropertyTagExifSensingMethod  0xA217
#define PropertyTagExifFileSource     0xA300
#define PropertyTagExifSceneType      0xA301
#define PropertyTagExifCfaPattern     0xA302

#define PropertyTagGpsVer             0x0000
#define PropertyTagGpsLatitudeRef     0x0001
#define PropertyTagGpsLatitude        0x0002
#define PropertyTagGpsLongitudeRef    0x0003
#define PropertyTagGpsLongitude       0x0004
#define PropertyTagGpsAltitudeRef     0x0005
#define PropertyTagGpsAltitude        0x0006
#define PropertyTagGpsGpsTime         0x0007
#define PropertyTagGpsGpsSatellites   0x0008
#define PropertyTagGpsGpsStatus       0x0009
#define PropertyTagGpsGpsMeasureMode  0x00A
#define PropertyTagGpsGpsDop          0x000B  // Measurement precision
#define PropertyTagGpsSpeedRef        0x000C
#define PropertyTagGpsSpeed           0x000D
#define PropertyTagGpsTrackRef        0x000E
#define PropertyTagGpsTrack           0x000F
#define PropertyTagGpsImgDirRef       0x0010
#define PropertyTagGpsImgDir          0x0011
#define PropertyTagGpsMapDatum        0x0012
#define PropertyTagGpsDestLatRef      0x0013
#define PropertyTagGpsDestLat         0x0014
#define PropertyTagGpsDestLongRef     0x0015
#define PropertyTagGpsDestLong        0x0016
#define PropertyTagGpsDestBearRef     0x0017
#define PropertyTagGpsDestBear        0x0018
#define PropertyTagGpsDestDistRef     0x0019
#define PropertyTagGpsDestDist        0x001A

#endif
