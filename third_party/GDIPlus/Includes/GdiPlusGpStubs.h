/**************************************************************************\
*
* Copyright (c) 1998-2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusGpStubs.h
*
* Abstract:
*
*   GDI+ Native C++ public header file
*
\**************************************************************************/

#ifndef _GDIPLUSGPSTUBS_H
#define _GDIPLUSGPSTUBS_H

//---------------------------------------------------------------------------
// GDI+ classes for forward reference
//---------------------------------------------------------------------------

class Graphics;
class Pen;
class Brush;
class Matrix;
class Bitmap;
class Metafile;
class GraphicsPath;
class PathIterator;
class Region;
class Image;
class TextureBrush;
class HatchBrush;
class SolidBrush;
class LinearGradientBrush;
class PathGradientBrush;
class Font;
class FontFamily;
class FontCollection;
class InstalledFontCollection;
class PrivateFontCollection;
class ImageAttributes;
class CachedBitmap;

//---------------------------------------------------------------------------
// Internal GDI+ classes for internal type checking
//---------------------------------------------------------------------------
class GpGraphics {};

class GpBrush {};
class GpTexture : public GpBrush {};
class GpSolidFill : public GpBrush {};
class GpLineGradient : public GpBrush {};
class GpPathGradient : public GpBrush {};
class GpHatch : public GpBrush {};

class GpPen {};
class GpCustomLineCap {};
class GpAdjustableArrowCap : public GpCustomLineCap {};

class GpImage {};
class GpBitmap : public GpImage {};
class GpMetafile : public GpImage {};
class GpImageAttributes {};

class GpPath {};
class GpRegion {};
class GpPathIterator {};

class GpFontFamily {};
class GpFont {};
class GpStringFormat {};
class GpFontCollection {};
class GpInstalledFontCollection : public GpFontCollection {};
class GpPrivateFontCollection : public GpFontCollection {};

class GpCachedBitmap;

typedef Status GpStatus;
typedef FillMode GpFillMode;
typedef WrapMode GpWrapMode;
typedef Unit GpUnit;
typedef CoordinateSpace GpCoordinateSpace;
typedef PointF GpPointF;
typedef Point GpPoint;
typedef RectF GpRectF;
typedef Rect GpRect;
typedef SizeF GpSizeF;
typedef HatchStyle GpHatchStyle;
typedef DashStyle GpDashStyle;
typedef LineCap GpLineCap;
typedef DashCap GpDashCap;


typedef PenAlignment GpPenAlignment;

typedef LineJoin GpLineJoin;
typedef PenType GpPenType;

typedef Matrix GpMatrix;
typedef BrushType GpBrushType;
typedef MatrixOrder GpMatrixOrder;
typedef FlushIntention GpFlushIntention;
typedef PathData GpPathData;

#endif  // !_GDIPLUSGPSTUBS.HPP

