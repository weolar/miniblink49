/******************************Module*Header*******************************\
* Module Name: mcdrv.h
*
* Server-side data structure for MCD driver interface.  These structures and
* values are used by the MCD driver to process calls made to the driver.
*
* Copyright (c) 1996-1997 Microsoft Corporation
*
\**************************************************************************/

#ifndef _MCDRV_H
#define _MCDRV_H

//
// The MCD version is defaulted to the most conservative version number.
// Override these defines if you wish to compile a driver with different
// version information.
//

#ifndef MCD_VER_MAJOR
#define MCD_VER_MAJOR               1
#endif

#ifndef MCD_VER_MINOR
#define MCD_VER_MINOR               0
#endif

#define MCD_MAX_USER_CLIP_PLANES    6

#define MCDRV_MEM_BUSY              1
#define MCDRV_MEM_NOT_BUSY          2

typedef ENUMRECTS MCDENUMRECTS;
typedef HANDLE MCDHANDLE;
typedef float MCDFLOAT;
typedef double MCDDOUBLE;

#define MCDENGDLLNAME	"MCDSRV32.DLL"
#define MCDFUNCS 3076

#define MCDRV_MEM_DMA   0x0001

#define MCDRV_TEXTURE_RESIDENT	0x0001

typedef struct _MCDRIVERINFO {
    ULONG verMajor;
    ULONG verMinor;
    ULONG verDriver;
    CHAR  idStr[200];
    ULONG drvMemFlags;
    ULONG drvBatchMemSizeMax;
} MCDDRIVERINFO;

typedef struct _MCDWINDOW
{
    RECTL clientRect;               // Rectangle describing current window
                                    //   client area
    RECTL clipBoundsRect;           // Bounding rectangle for the individual
                                    //   clipping rectangles
    MCDENUMRECTS *pClip;            // List of rectangles describing the
                                    //   current clip region intersected
                                    //   with the current scissors rectangle
    MCDENUMRECTS *pClipUnscissored; // Unscissored version of above
    VOID *pvUser;
} MCDWINDOW;

// Rendering context

// MCD context createFlags:
//
// MCDCONTEXT_SWAPSYNC          If set, synchronize MCDrvSwap to VSYNC
//                              for a tearless swap (if possible).
//
// MCDCONTEXT_IO_PRIORITY       If set, allow OpenGL to have a higher priority
//                              on the bus (if possible, let OpenGL "hog" the
//                              bus).
//

#define MCDCONTEXT_SWAPSYNC         0x00000001
#define MCDCONTEXT_IO_PRIORITY      0x00000002

typedef struct _MCDRC
{
    LONG iPixelFormat;              // Pixel format for the RC
    LONG iLayerPlane;               // Layer plane for the RC
    ULONG createFlags;              // Creation flags
    ULONG userFlags;                // User-specified flags
    VOID *pvUser;                   // User-specified pointer for expansion
} MCDRC;


#define MCDRCINFO_NOVIEWPORTADJUST   0x0001
#define MCDRCINFO_Y_LOWER_LEFT       0x0002
#define MCDRCINFO_DEVCOLORSCALE      0x0004
#define MCDRCINFO_DEVZSCALE          0x0008

// If set the system provides state updates
// with fine-grained state updates rather
// than through RENDERSTATE.  This is
// required if the driver exports
// MCDrvProcess.
#define MCDRCINFO_FINE_GRAINED_STATE 0x0010

typedef struct _MCRCINFO
{
    ULONG requestFlags;
    MCDFLOAT redScale;
    MCDFLOAT greenScale;
    MCDFLOAT blueScale;
    MCDFLOAT alphaScale;
    MCDDOUBLE zScale;               // This is a double to preserve accuracy
    ULONG depthBufferMax;
    LONG viewportXAdjust;
    LONG viewportYAdjust;
    ULONG reserved[12];
} MCDRCINFO;


// MCD pixel format descriptor

typedef struct _MCDPIXELFORMAT {
    WORD  nSize;
    DWORD dwFlags;                  // Any combination of:
                                    //
                                    //      PFD_DOUBLEBUFFER
                                    //      PFD_NEED_PALETTE
                                    //      PFD_NEED_SYSTEM_PALETTE
                                    //      PFD_SWAP_EXCHANGE
                                    //      PFD_SWAP_COPY
                                    //      PFD_SWAP_LAYER_BUFFERS

    BYTE  iPixelType;               // One of the following:
                                    //
                                    //      PFD_TYPE_RGBA
                                    //      PFD_TYPE_COLORINDEX

    BYTE  cColorBits;
    BYTE  cRedBits;
    BYTE  cRedShift;
    BYTE  cGreenBits;
    BYTE  cGreenShift;
    BYTE  cBlueBits;
    BYTE  cBlueShift;
    BYTE  cAlphaBits;
    BYTE  cAlphaShift;
    BYTE  cDepthBits;               // Number of significant depth bits
    BYTE  cDepthShift;
    BYTE  cDepthBufferBits;         // Element size of depth buffer
                                    // (eg, a depth buffer with cDepthBits = 24
                                    // might have a cDepthBufferBits = 32)
    BYTE  cStencilBits;
    BYTE  cOverlayPlanes;           // Count of up to 15 overlay planes
    BYTE  cUnderlayPlanes;          // Count of up to 15 underlay planes
    DWORD dwTransparentColor;       // If there is an underlay plane, specifies
                                    // transparent color or index.
} MCDPIXELFORMAT;

// MCD layer plane descriptor

typedef struct _MCDLAYERPLANE {
    WORD  nSize;
    WORD  nVersion;
    DWORD dwFlags;                  // Any combination of:
                                    //
                                    //      LPD_SUPPORT_OPENGL
                                    //      LPD_SUPPORT_GDI
                                    //      LPD_DOUBLEBUFFER
                                    //      LPD_STEREO
                                    //      LPD_SWAP_EXCHANGE
                                    //      LPD_SWAP_COPY
                                    //      LPD_TRANSPARANT
                                    //      LPD_SHARE_DEPTH
                                    //      LPD_SHARE_STENCIL
                                    //      LPD_SHARE_ACCUM

    BYTE  iPixelType;               // One of the following:
                                    //
                                    //      LPD_TYPE_RGBA
                                    //      LPD_TYPE_COLORINDEX

    BYTE  cColorBits;
    BYTE  cRedBits;
    BYTE  cRedShift;
    BYTE  cGreenBits;
    BYTE  cGreenShift;
    BYTE  cBlueBits;
    BYTE  cBlueShift;
    BYTE  cAlphaBits;
    BYTE  cAlphaShift;
    BYTE  cAuxBuffers;
    BYTE  iLayerPlane;
    COLORREF crTransparent;

} MCDLAYERPLANE;


//
// Basic rendering types:
//

typedef struct _MCDCOLOR {
    MCDFLOAT r, g, b, a;
} MCDCOLOR;

typedef struct _MCDCOORD {
    MCDFLOAT x, y, z, w;
} MCDCOORD;


//
// Texture structures:
//

typedef struct __MCDMIPMAPLEVEL {
    UCHAR *pTexels;                     // pointer to client texture data
    LONG width, height;
    LONG widthImage, heightImage;       // Image dimensions without the border
    MCDFLOAT widthImagef, heightImagef; // Floatin-point versions of above
    LONG widthLog2, heightLog2;         // Log2 of above
    LONG border;                        // Border size
    LONG requestedFormat;               // Requested internal format
    LONG baseFormat;                    // Base format
    LONG internalFormat;                // Actual internal format

    LONG redSize;                       // Component resolution
    LONG greenSize;
    LONG blueSize;
    LONG alphaSize;
    LONG luminanceSize;
    LONG intensitySize;

    ULONG reserved;

} MCDMIPMAPLEVEL;

typedef struct __MCDTEXTURESTATE {

    ULONG sWrapMode;                    // Wrap modes
    ULONG tWrapMode;

    ULONG minFilter;                    // Min/mag filters               
    ULONG magFilter;

    MCDCOLOR borderColor;               // Border color

} MCDTEXTURESTATE;

typedef struct __MCDTEXTUREOBJSTATE {
    ULONG name;                         // "name" of texture object
    MCDFLOAT priority;                  // priority of the texture object
} MCDTEXTUREOBJSTATE;

typedef struct __MCDTEXTUREDATA {
    MCDTEXTURESTATE textureState;
    MCDTEXTUREOBJSTATE textureObjState;
    MCDMIPMAPLEVEL *level;
    ULONG textureDimension;

    // Support for texture palettes:

    ULONG paletteSize;
    RGBQUAD *paletteData;
    ULONG paletteBaseFormat;            // Type of palette data
    ULONG paletteRequestedFormat;

} MCDTEXTUREDATA;

typedef struct _MCDTEXTURE {
    MCDTEXTUREDATA *pMCDTextureData;
    VOID *pSurface;
    ULONG createFlags;
    ULONG_PTR textureKey;       // Must be filled in by the driver
    ULONG userFlags;
    VOID *pvUser;
} MCDTEXTURE;


//
// Memory block.
//

typedef struct _MCDMEM {
    ULONG memSize;
    ULONG createFlags;
    UCHAR *pMemBase;
    ULONG userFlags;
    VOID *pvUser;
} MCDMEM;


//
// Generic description prefix for any state change.
//

typedef struct _MCDSTATE_PREFIX {
    ULONG state;
    ULONG size;
} MCDSTATE_PREFIX;

typedef struct _MCDSTATE {
    ULONG state;
    ULONG size;
    ULONG stateValue;
} MCDSTATE;

#define MCD_RENDER_STATE                0
#define MCD_PIXEL_STATE                 1
#define MCD_SCISSOR_RECT_STATE          2
#define MCD_TEXENV_STATE                3
#define MCD_ENABLE_STATE                4
#define MCD_TEXTURE_ENABLE_STATE        5
#define MCD_FOG_STATE                   6
#define MCD_SHADEMODEL_STATE            7
#define MCD_POINTDRAW_STATE             8
#define MCD_LINEDRAW_STATE              9
#define MCD_POLYDRAW_STATE              10
#define MCD_ALPHATEST_STATE             11
#define MCD_DEPTHTEST_STATE             12
#define MCD_BLEND_STATE                 13
#define MCD_LOGICOP_STATE               14
#define MCD_FRAMEBUF_STATE              15
#define MCD_LIGHT_MODEL_STATE           16
#define MCD_HINT_STATE                  17
#define MCD_CLIP_STATE                  18
#define MCD_STENCILTEST_STATE           19

//
// MCDSTATE_RENDER is derived from the MCDSTATE_PREFIX structure and is used
// to pass all MCD rendering state (MCDRENDERSTATE) in a single command
// via MCDrvState.
//
// State field names are derived from the GLenum constant names by removing
// the GL_ prefix, replacing the "_" separators with case changes, and
// adding the "Enable" suffix to state enables.
//
// For example:
//
//  GL_FOG_COLOR    becomes     fogColor
//  GL_POINT_SMOOTH becomes     pointSmoothEnable
//
// In addition, there are few multiple values that are accessed via a single
// GLenum.  For example, GL_POLYGON_MODE returns both a front and a back
// polygon mode, so:
//
//  GL_POLYGON_MODE becomes     polygonModeFront *and* polygonModeBack
//

// Enable flags for enables field in MCDRENDERSTATE

#define MCD_ALPHA_TEST_ENABLE                  (1 <<  0)
#define MCD_BLEND_ENABLE                       (1 <<  1)
#define MCD_INDEX_LOGIC_OP_ENABLE              (1 <<  2)
#define MCD_DITHER_ENABLE                      (1 <<  3)
#define MCD_DEPTH_TEST_ENABLE                  (1 <<  4)
#define MCD_FOG_ENABLE                         (1 <<  5)
#define MCD_LIGHTING_ENABLE                    (1 <<  6)
#define MCD_COLOR_MATERIAL_ENABLE              (1 <<  7)
#define MCD_LINE_STIPPLE_ENABLE                (1 <<  8)
#define MCD_LINE_SMOOTH_ENABLE                 (1 <<  9)
#define MCD_POINT_SMOOTH_ENABLE                (1 << 10)
#define MCD_POLYGON_SMOOTH_ENABLE              (1 << 11)
#define MCD_CULL_FACE_ENABLE                   (1 << 12)
#define MCD_POLYGON_STIPPLE_ENABLE             (1 << 13)
#define MCD_SCISSOR_TEST_ENABLE                (1 << 14)
#define MCD_STENCIL_TEST_ENABLE                (1 << 15)
#define MCD_TEXTURE_1D_ENABLE                  (1 << 16)
#define MCD_TEXTURE_2D_ENABLE                  (1 << 17)
#define MCD_TEXTURE_GEN_S_ENABLE               (1 << 18)
#define MCD_TEXTURE_GEN_T_ENABLE               (1 << 19)
#define MCD_TEXTURE_GEN_R_ENABLE               (1 << 20)
#define MCD_TEXTURE_GEN_Q_ENABLE               (1 << 21)
#define MCD_NORMALIZE_ENABLE                   (1 << 22)
#define MCD_AUTO_NORMAL_ENABLE                 (1 << 23) // Not currently used
#define MCD_POLYGON_OFFSET_POINT_ENABLE        (1 << 24)
#define MCD_POLYGON_OFFSET_LINE_ENABLE         (1 << 25)
#define MCD_POLYGON_OFFSET_FILL_ENABLE         (1 << 26)
#define MCD_COLOR_LOGIC_OP_ENABLE              (1 << 27)
#define MCD_SPECULAR_FOG_ENABLE                (1 << 29)

typedef struct _MCDRENDERSTATE {

    // state enables

    ULONG enables;

    // texture state

    BOOL textureEnabled;

    // fog state

    MCDCOLOR fogColor;
    MCDFLOAT fogIndex;
    MCDFLOAT fogDensity;
    MCDFLOAT fogStart;
    MCDFLOAT fogEnd;
    ULONG fogMode;

    // shading model state

    ULONG shadeModel;

    // point drawing state

    MCDFLOAT pointSize;

    // line drawing state

    MCDFLOAT lineWidth;
    USHORT lineStipplePattern;
    SHORT lineStippleRepeat;

    // polygon drawing state

    ULONG cullFaceMode;
    ULONG frontFace;
    ULONG polygonModeFront;
    ULONG polygonModeBack;
    BYTE polygonStipple[4*32];
    MCDFLOAT zOffsetFactor;
    MCDFLOAT zOffsetUnits;

    // stencil test state

    BOOL stencilTestFunc;
    USHORT stencilMask;
    USHORT stencilRef;
    ULONG stencilFail;
    ULONG stencilDepthFail;
    ULONG stencilDepthPass;

    // alpha test state

    ULONG alphaTestFunc;
    MCDFLOAT alphaTestRef;

    // depth test state

    ULONG depthTestFunc;

    // blend state

    ULONG blendSrc;
    ULONG blendDst;

    // logic op state

    ULONG logicOpMode;

    // frame buffer control state

    ULONG drawBuffer;
    ULONG indexWritemask;
    BOOL colorWritemask[4];
    BOOL depthWritemask;      // Called mask, but really a write enable
    USHORT stencilWritemask;
    MCDCOLOR colorClearValue;
    MCDFLOAT indexClearValue;
    MCDDOUBLE depthClearValue;
    USHORT stencilClearValue;

    // lighting

    BOOL twoSided;

    // clipping control

    MCDCOORD userClipPlanes[MCD_MAX_USER_CLIP_PLANES];

    // hints

    ULONG perspectiveCorrectionHint;
    ULONG pointSmoothHint;
    ULONG lineSmoothHint;
    ULONG polygonSmoothHint;
    ULONG fogHint;

} MCDRENDERSTATE;

typedef struct _MCDSTATE_RENDER {
    ULONG    state;     // must be MCD_RENDER_STATE
    ULONG    size;      // must be sizeof(MCDSTATE_RENDER)
    MCDRENDERSTATE allState;
} MCDSTATE_RENDER;

//
// MCDSTATEPIXEL is a variant of the MCDSTATE structure that is used
// to pass all pixel state (MCDPIXELSTATE) in a single command
// via MCDrvState.
//
// Note: for MCDrvDrawPixels, the MCDUNPACK structure can be overridden by
// the packed parameter to the function.  If set, the source of the data is
// a display list and the structure of the data for that call may be assumed
// to be:
//
//      swapEndian = FALSE
//      lsbFirst   = FALSE
//      lineLength = width (from MCDrvDrawPixels parameter list)
//      skipLines  = 0
//      skipPixels = 0
//      alignment  = 1
//

typedef struct _MCDPIXELTRANSFER {
    MCDFLOAT redScale, greenScale, blueScale, alphaScale, depthScale;
    MCDFLOAT redbias, greenBias, blueBias, aalphaBias, depthBias;
    MCDFLOAT zoomX;
    MCDFLOAT zoomY;

    LONG indexShift;
    LONG indexOffset;

    BOOL mapColor;
    BOOL mapStencil;
} MCDPIXELTRANSFER;

typedef struct _MCDPIXELPACK {
    BYTE swapEndian;
    BYTE lsbFirst;

    LONG lineLength;
    LONG skipLines;
    LONG skipPixels;
    LONG alignment;
    LONG lReserved;
} MCDPIXELPACK;

typedef struct _MCDPIXELUNPACK {
    BYTE swapEndian;
    BYTE lsbFirst;

    LONG lineLength;
    LONG skipLines;
    LONG skipPixels;
    LONG alignment;
    LONG lReserved;
} MCDPIXELUNPACK;

typedef struct _MCDPIXELSTATE {
    MCDPIXELTRANSFER pixelTransferModes;
    MCDPIXELPACK pixelPackModes;
    MCDPIXELUNPACK pixelUnpackModes;
    ULONG readBuffer;
    MCDCOORD rasterPos;
} MCDPIXELSTATE;

typedef struct _MCDSTATE_PIXEL {
    ULONG    state;     // must be MCD_PIXEL_STATE
    ULONG    size;      // must be sizeof(MCDSTATE_PIXEL)
    MCDPIXELSTATE pixelState;
} MCDSTATE_PIXEL;

//
// MCDSTATE_SCISSOR_RECT is a variant of the MCDSTATE structure
// that is used to pass the scissor rectangle to the MCD driver
// via MCDrvState.
//

typedef struct _MCDSTATE_SCISSOR_RECT {
    ULONG   state;      // must be MCD_SCISSOR_RECT_STATE
    ULONG   size;       // must be sizeof(MCDSTATE_SCISSOR_RECT)
    RECTL   scissorRect;
} MCDSTATE_SCISSOR_RECT;

//
// MCDSTATE_TEXENV is a variant of the MCDSTATE structure that is
// used to pass the texture environment state to the MCD driver
// via MCDrvState.
//

typedef struct _MCDTEXENVSTATE {
    ULONG    texEnvMode;
    MCDCOLOR texEnvColor;
} MCDTEXENVSTATE;

typedef struct _MCDSTATE_TEXENV {
    ULONG   state;      // must be MCD_TEXENV_STATE
    ULONG   size;       // must be sizeof(MCDSTATE_TEXENV)
    MCDTEXENVSTATE texEnvState;
} MCDSTATE_TEXENV;

//
// MCDVIEWPORT is used to pass viewport state to the MCD driver
// via MCDrvViewport.

typedef struct _MCDVIEWPORT {
    MCDFLOAT xScale, xCenter;
    MCDFLOAT yScale, yCenter;
    MCDFLOAT zScale, zCenter;
} MCDVIEWPORT;

//
// Fine-grained state updates.  Sent to a driver only if
// MCDRCINFO_FINE_GRAINED_STATE was returned on context creation.
//

// Enable state.
typedef struct _MCDENABLESTATE {
    ULONG enables;
} MCDENABLESTATE;

// Texture enable state.  Different from plain enables in that it
// indicates whether all texturing state is consistent and ready for
// texturing.
typedef struct _MCDTEXTUREENABLESTATE {
    BOOL textureEnabled;
} MCDTEXTUREENABLESTATE;

// Fog state.
typedef struct _MCDFOGSTATE {
    MCDCOLOR fogColor;
    MCDFLOAT fogIndex;
    MCDFLOAT fogDensity;
    MCDFLOAT fogStart;
    MCDFLOAT fogEnd;
    ULONG fogMode;
} MCDFOGSTATE;

// Shading model state.
typedef struct _MCDSHADELMODELSTATE {
    ULONG shadeModel;
} MCDSHADEMODELSTATE;

// Point drawing state.
typedef struct _MCDPOINTDRAWSTATE {
    MCDFLOAT pointSize;
} MCDPOINTDRAWSTATE;

// Line drawing state.
typedef struct _MCDLINEDRAWSTATE {
    MCDFLOAT lineWidth;
    USHORT lineStipplePattern;
    SHORT lineStippleRepeat;
} MCDLINEDRAWSTATE;

// Polygon drawing state.
typedef struct _MCDPOLYDRAWSTATE {
    ULONG cullFaceMode;
    ULONG frontFace;
    ULONG polygonModeFront;
    ULONG polygonModeBack;
    BYTE polygonStipple[4*32];
    MCDFLOAT zOffsetFactor;
    MCDFLOAT zOffsetUnits;
} MCDPOLYDRAWSTATE;

// Alpha test state.
typedef struct _MCDALPHATESTSTATE {
    ULONG alphaTestFunc;
    MCDFLOAT alphaTestRef;
} MCDALPHATESTSTATE;

// Depth test state.
typedef struct _MCDDEPTHTESTSTATE {
    ULONG depthTestFunc;
} MCDDEPTHTESTSTATE;

// Blend state.
typedef struct _MCDBLENDSTATE {
    ULONG blendSrc;
    ULONG blendDst;
} MCDBLENDSTATE;

// Logic op state.
typedef struct _MCDLOGICOPSTATE {
    ULONG logicOpMode;
} MCDLOGICOPSTATE;

// Frame buffer control state.
typedef struct _MCDFRAMEBUFSTATE {
    ULONG drawBuffer;
    ULONG indexWritemask;
    BOOL colorWritemask[4];
    BOOL depthWritemask;      // Called mask, but really a write enable.
    USHORT stencilWritemask;
    MCDCOLOR colorClearValue;
    MCDFLOAT indexClearValue;
    MCDDOUBLE depthClearValue;
    USHORT stencilClearValue;
} MCDFRAMEBUFSTATE;

// Light model state.
typedef struct _MCDLIGHTMODELSTATE {
    MCDCOLOR ambient;           // Scaled
    BOOL localViewer;
    BOOL twoSided;
} MCDLIGHTMODELSTATE;

// Hint state.
typedef struct _MCDHINTSTATE {
    ULONG perspectiveCorrectionHint;
    ULONG pointSmoothHint;
    ULONG lineSmoothHint;
    ULONG polygonSmoothHint;
    ULONG fogHint;
} MCDHINTSTATE;

// Clipping state.
typedef struct _MCDCLIPSTATE {
    ULONG userClipEnables;
    // Given by program
    MCDCOORD userClipPlanes[MCD_MAX_USER_CLIP_PLANES];
    // userClipPlanes transformed by modelview inverse
    MCDCOORD userClipPlanesInv[MCD_MAX_USER_CLIP_PLANES];
} MCDCLIPSTATE;

// Stencil test state.
typedef struct _MCDSTENCILTESTSTATE {
    BOOL stencilTestFunc;
    USHORT stencilMask;
    USHORT stencilRef;
    ULONG stencilFail;
    ULONG stencilDepthFail;
    ULONG stencilDepthPass;
} MCDSTENCILTESTSTATE;
        

//
// MCDBUF.bufFlags flags:
//
// MCDBUF_ENABLED       If set, direct buffer access is enabled (i.e., the
//                      bufOffset and bufStride values are valid and may
//                      be used to access the buffer).
//
// MCDBUF_NOCLIP        If set, indicates that clipping is not required
//                      for the current state of the window.
//

#define MCDBUF_ENABLED  0x00000001
#define MCDBUF_NOCLIP   0x00000002

typedef struct _MCDBUF {
    ULONG bufFlags;
    LONG  bufOffset;        // offset relative to beginning of framebuffer
    LONG  bufStride;
} MCDBUF;

typedef struct _MCDBUFFERS {
    MCDBUF mcdFrontBuf;
    MCDBUF mcdBackBuf;
    MCDBUF mcdDepthBuf;
} MCDBUFFERS;

//
// MCD surface flags:
//
// MCDSURFACE_HWND

#define MCDSURFACE_HWND             0x00000001

typedef struct _MCDSURFACE {
    MCDWINDOW *pWnd;                // Region support
    SURFOBJ *pso;
    WNDOBJ *pwo;
    ULONG reserved[4];
    ULONG surfaceFlags;
} MCDSURFACE;

#define MCDSPAN_FRONT   1
#define MCDSPAN_BACK    2
#define MCDSPAN_DEPTH   3

typedef struct _MCDSPAN {
    LONG x;
    LONG y;
    LONG numPixels;
    ULONG type;
    VOID *pPixels;
} MCDSPAN;

//
// MCDrvSwap flags
//

#define MCDSWAP_MAIN_PLANE      0x00000001
#define MCDSWAP_OVERLAY1        0x00000002
#define MCDSWAP_OVERLAY2        0x00000004
#define MCDSWAP_OVERLAY3        0x00000008
#define MCDSWAP_OVERLAY4        0x00000010
#define MCDSWAP_OVERLAY5        0x00000020
#define MCDSWAP_OVERLAY6        0x00000040
#define MCDSWAP_OVERLAY7        0x00000080
#define MCDSWAP_OVERLAY8        0x00000100
#define MCDSWAP_OVERLAY9        0x00000200
#define MCDSWAP_OVERLAY10       0x00000400
#define MCDSWAP_OVERLAY11       0x00000800
#define MCDSWAP_OVERLAY12       0x00001000
#define MCDSWAP_OVERLAY13       0x00002000
#define MCDSWAP_OVERLAY14       0x00004000
#define MCDSWAP_OVERLAY15       0x00008000
#define MCDSWAP_UNDERLAY1       0x00010000
#define MCDSWAP_UNDERLAY2       0x00020000
#define MCDSWAP_UNDERLAY3       0x00040000
#define MCDSWAP_UNDERLAY4       0x00080000
#define MCDSWAP_UNDERLAY5       0x00100000
#define MCDSWAP_UNDERLAY6       0x00200000
#define MCDSWAP_UNDERLAY7       0x00400000
#define MCDSWAP_UNDERLAY8       0x00800000
#define MCDSWAP_UNDERLAY9       0x01000000
#define MCDSWAP_UNDERLAY10      0x02000000
#define MCDSWAP_UNDERLAY11      0x04000000
#define MCDSWAP_UNDERLAY12      0x08000000
#define MCDSWAP_UNDERLAY13      0x10000000
#define MCDSWAP_UNDERLAY14      0x20000000
#define MCDSWAP_UNDERLAY15      0x40000000


//
// Clip codes:
//

#define MCD_CLIP_LEFT           0x00000001
#define MCD_CLIP_RIGHT          0x00000002
#define MCD_CLIP_BOTTOM         0x00000004
#define MCD_CLIP_TOP            0x00000008
#define MCD_CLIP_NEAR           0x00000010
#define MCD_CLIP_FAR            0x00000020
#define MCD_CLIP_MASK           0x0000003f

//
// Vertex flags:
//

#define MCDVERTEX_EDGEFLAG              0x00000001

//
// Color indices:
//

#define MCDVERTEX_FRONTFACE     0
#define MCDVERTEX_BACKFACE      1

//
// Note: vertex colors are scaled to the color depths reported in the
// pixel format.
//

typedef struct _MCDVERTEX {

    ULONG flags;                // vertex flags
    MCDCOLOR *pColor;           // pointer to active vertex color
    ULONG clipCode;             // clip code
    MCDFLOAT fog;               // fog value (0..1)
    MCDCOORD clipCoord;         // clip-space coordinate
    MCDCOORD windowCoord;       // window coordinate
    MCDCOORD texCoord;          // texture coordinate
    MCDCOORD normal;            // vertex normal
    MCDCOLOR colors[2];         // front and back vertex colors
    MCDCOORD eyeCoord;          // eye coordinate
} MCDVERTEX;

typedef struct _MCDCOMMAND MCDCOMMAND;

#define MCDCOMMAND_RESET_STIPPLE        0x00004000
#define MCDCOMMAND_RENDER_PRIMITIVE     0x00008000
#define MCDCOMMAND_SAME_COLOR           0x00040000

typedef struct _MCDCOMMAND {
    ULONG flags;                // flags for this command
    MCDVERTEX *pEndVertex;
    ULONG reserved2;
    ULONG reserved3;
    ULONG reserved4;
    ULONG reserved5;
    MCDVERTEX *pStartVertex;
    ULONG reserved7;
    ULONG reserved8;
    MCDVERTEX *pVertexBuffer;
    ULONG reserved10;
    ULONG reserved11;
    ULONG reserved12;
    ULONG reserved13;
    ULONG reserved14;
    ULONG command;              // primitive type or command (GL_TRIANGLES, etc.)
    ULONG clipCodes;
    ULONG reserved17;
    ULONG reserved18;
    MCDCOMMAND *pNextCmd;
    ULONG numIndices;
    UCHAR *pIndices;
    ULONG reserved22;
    ULONG textureKey;
} MCDCOMMAND;


// MCDDRIVER structure containing driver functions

typedef LONG     (*MCDRVDESCRIBEPIXELFORMATFUNC)(MCDSURFACE *pMCDSurface, LONG iPixelFormat,
                                                 ULONG nBytes, MCDPIXELFORMAT *pMCDPixelFmt, ULONG flags);
typedef BOOL     (*MCDRVDESCRIBELAYERPLANEFUNC)(MCDSURFACE *pMCDSurface, LONG iPixelFormat,
                                                LONG iLayerPlane, ULONG nBytes, MCDLAYERPLANE *pMCDLayerPlane,
                                                ULONG flags);
typedef LONG     (*MCDRVSETLAYERPALETTEFUNC)(MCDSURFACE *pMCDSurface, LONG iLayerPlane, BOOL bRealize, LONG cEntries, COLORREF *pcr);
typedef BOOL     (*MCDRVINFOFUNC)(MCDSURFACE *pMCDSurface, MCDDRIVERINFO *pMCDDriverInfo);
typedef ULONG    (*MCDRVCREATECONTEXTFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDRCINFO *pDrvRcInfo);
typedef ULONG    (*MCDRVDELETECONTEXTFUNC)(MCDRC *pRc, DHPDEV dhpdev);
typedef ULONG    (*MCDRVCREATETEXTUREFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDTEXTURE *pTex);
typedef ULONG    (*MCDRVUPDATESUBTEXTUREFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDTEXTURE *pTex, 
                                              ULONG lod, RECTL *pRect);
typedef ULONG    (*MCDRVUPDATETEXTUREPALETTEFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDTEXTURE *pTex, 
                                                  ULONG start, ULONG numEntries);
typedef ULONG    (*MCDRVUPDATETEXTUREPRIORITYFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDTEXTURE *pTex);
typedef ULONG    (*MCDRVUPDATETEXTURESTATEFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDTEXTURE *pTex);
typedef ULONG    (*MCDRVTEXTURESTATUSFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDTEXTURE *pTex);
typedef ULONG    (*MCDRVDELETETEXTUREFUNC)(MCDTEXTURE *pTex, DHPDEV dhpdev);
typedef ULONG    (*MCDRVCREATEMEMFUNC)(MCDSURFACE *pMCDSurface, MCDMEM *pMCDMem);
typedef ULONG    (*MCDRVDELETEMEMFUNC)(MCDMEM *pMCDMem, DHPDEV dhpdev);
typedef ULONG_PTR (*MCDRVDRAWFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDMEM *pMCDExecMem, UCHAR *pStart, UCHAR *pEnd);
typedef ULONG    (*MCDRVCLEARFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, ULONG buffers);
typedef ULONG    (*MCDRVSWAPFUNC)(MCDSURFACE *pMCDSurface, ULONG flags);
typedef ULONG    (*MCDRVSTATEFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDMEM *pMCDMem,
                                   UCHAR *pStart, LONG length, ULONG numStates);
typedef ULONG    (*MCDRVVIEWPORTFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDVIEWPORT *pMCDViewport);
typedef HDEV     (*MCDRVGETHDEVFUNC)(MCDSURFACE *pMCDSurface);
typedef ULONG    (*MCDRVSPANFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDMEM *pMCDMem,
                                  MCDSPAN *pMCDSpan, BOOL bRead);
typedef VOID     (*MCDRVTRACKWINDOWFUNC)(WNDOBJ *pWndObj, MCDWINDOW *pMCDWnd, ULONG flags);
typedef ULONG    (*MCDRVGETBUFFERSFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc, MCDBUFFERS *pMCDBuffers);
typedef ULONG    (*MCDRVALLOCBUFFERSFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc);
typedef ULONG    (*MCDRVBINDCONTEXTFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc);
typedef ULONG    (*MCDRVSYNCFUNC)(MCDSURFACE *pMCDSurface, MCDRC *pRc);
typedef ULONG    (*MCDRVDRAWPIXELS)(MCDSURFACE *pMcdSurface, MCDRC *pRc,
                                    ULONG width, ULONG height, ULONG format,
                                    ULONG type, VOID *pPixels, BOOL packed);
typedef ULONG    (*MCDRVREADPIXELS)(MCDSURFACE *pMcdSurface, MCDRC *pRc,
                                    LONG x, LONG y, ULONG width, ULONG height, ULONG format,
                                    ULONG type, VOID *pPixels);
typedef ULONG    (*MCDRVCOPYPIXELS)(MCDSURFACE *pMcdSurface, MCDRC *pRc,
                                    LONG x, LONG y, ULONG width, ULONG height, ULONG type);
typedef ULONG    (*MCDRVPIXELMAP)(MCDSURFACE *pMcdSurface, MCDRC *pRc,
                                  ULONG mapType, ULONG mapSize, VOID *pMap);

#define MCDDRIVER_V10_SIZE      (sizeof(ULONG)+31*sizeof(void *))

typedef struct _MCDDRIVER {
    ULONG                           ulSize;
    MCDRVDESCRIBEPIXELFORMATFUNC    pMCDrvDescribePixelFormat;
    MCDRVDESCRIBELAYERPLANEFUNC     pMCDrvDescribeLayerPlane;
    MCDRVSETLAYERPALETTEFUNC        pMCDrvSetLayerPalette;
    MCDRVINFOFUNC                   pMCDrvInfo;
    MCDRVCREATECONTEXTFUNC          pMCDrvCreateContext;
    MCDRVDELETECONTEXTFUNC          pMCDrvDeleteContext;
    MCDRVBINDCONTEXTFUNC            pMCDrvBindContext;
    MCDRVCREATETEXTUREFUNC          pMCDrvCreateTexture;
    MCDRVDELETETEXTUREFUNC          pMCDrvDeleteTexture;
    MCDRVUPDATESUBTEXTUREFUNC       pMCDrvUpdateSubTexture;
    MCDRVUPDATETEXTUREPALETTEFUNC   pMCDrvUpdateTexturePalette;
    MCDRVUPDATETEXTUREPRIORITYFUNC  pMCDrvUpdateTexturePriority;
    MCDRVUPDATETEXTURESTATEFUNC     pMCDrvUpdateTextureState;
    MCDRVTEXTURESTATUSFUNC          pMCDrvTextureStatus;
    MCDRVCREATEMEMFUNC              pMCDrvCreateMem;
    MCDRVDELETEMEMFUNC              pMCDrvDeleteMem;
    MCDRVDRAWFUNC                   pMCDrvDraw;
    MCDRVCLEARFUNC                  pMCDrvClear;
    MCDRVSWAPFUNC                   pMCDrvSwap;
    MCDRVSTATEFUNC                  pMCDrvState;
    MCDRVVIEWPORTFUNC               pMCDrvViewport;
    MCDRVGETHDEVFUNC                pMCDrvGetHdev;
    MCDRVSPANFUNC                   pMCDrvSpan;
    MCDRVTRACKWINDOWFUNC            pMCDrvTrackWindow;
    MCDRVALLOCBUFFERSFUNC           pMCDrvAllocBuffers;
    MCDRVGETBUFFERSFUNC             pMCDrvGetBuffers;
    MCDRVSYNCFUNC                   pMCDrvSync;
    MCDRVDRAWPIXELS                 pMCDrvDrawPixels;
    MCDRVREADPIXELS                 pMCDrvReadPixels;
    MCDRVCOPYPIXELS                 pMCDrvCopyPixels;
    MCDRVPIXELMAP                   pMCDrvPixelMap;
} MCDDRIVER;


// Top-level (global) driver functions established at DLL initialization time
// through MCD initialization. All other driver functions are obtained through
// the MCDrvGetEntryPoints funtion:

typedef BOOL  (*MCDRVGETENTRYPOINTSFUNC)(MCDSURFACE *pMCDSurface,
                                         MCDDRIVER *pMCDDriver);

typedef struct _MCDGLOBALDRIVERFUNCS {
    ULONG ulSize;
    MCDRVGETENTRYPOINTSFUNC     pMCDrvGetEntryPoints;
} MCDGLOBALDRIVERFUNCS;

// MCD Server engine functions:

#define MCDENGINITFUNCNAME              "MCDEngInit"
#define MCDENGINITEXFUNCNAME            "MCDEngInitEx"
#define MCDENGUNINITFUNCNAME            "MCDEngUninit"
#define MCDENGESCFILTERNAME             "MCDEngEscFilter"
#define MCDENGSETMEMSTATUSNAME          "MCDEngSetMemStatus"

typedef BOOL (WINAPI *MCDENGINITFUNC)(SURFOBJ *pso, 
                                      MCDRVGETENTRYPOINTSFUNC);
typedef BOOL (WINAPI *MCDENGINITEXFUNC)(SURFOBJ *pso,
                                        MCDGLOBALDRIVERFUNCS *pMCDGlobalDriverFuncs,
                                        void *pReserved);

typedef void (WINAPI *MCDENGUNINITFUNC)(SURFOBJ *pso);

typedef BOOL (WINAPI *MCDENGESCFILTERFUNC)(SURFOBJ *pso, ULONG iEsc,
                                           ULONG cjIn, VOID *pvIn,
                                           ULONG cjOut, VOID *pvOut, 
                                           ULONG *pRetVal);

typedef BOOL (WINAPI *MCDENGSETMEMSTATUSFUNC)(MCDMEM *pMCDMem, ULONG status);

#endif

