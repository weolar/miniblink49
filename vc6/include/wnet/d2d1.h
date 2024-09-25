
#ifndef _D2D1_H_
#define _D2D1_H_

typedef enum D2D1_FILL_MODE {
    D2D1_FILL_MODE_ALTERNATE = 0,
    D2D1_FILL_MODE_WINDING = 1,
    D2D1_FILL_MODE_FORCE_DWORD = 0xffffffff
} D2D1_FILL_MODE;

typedef enum D2D1_PATH_SEGMENT {
    D2D1_PATH_SEGMENT_NONE = 0x00000000,
    D2D1_PATH_SEGMENT_FORCE_UNSTROKED = 0x00000001,
    D2D1_PATH_SEGMENT_FORCE_ROUND_LINE_JOIN = 0x00000002,
    D2D1_PATH_SEGMENT_FORCE_DWORD = 0xffffffff
} D2D1_PATH_SEGMENT;

typedef enum D2D1_FIGURE_BEGIN {
    D2D1_FIGURE_BEGIN_FILLED = 0,
    D2D1_FIGURE_BEGIN_HOLLOW = 1,
    D2D1_FIGURE_BEGIN_FORCE_DWORD = 0xffffffff
} D2D1_FIGURE_BEGIN;

typedef struct D2D_POINT_2U {
    UINT32 x;
    UINT32 y;
} D2D_POINT_2U;

typedef struct D2D_POINT_2F {
    FLOAT x;
    FLOAT y;
} D2D_POINT_2F;

typedef D2D_POINT_2U D2D1_POINT_2U;
typedef D2D_POINT_2F D2D1_POINT_2F;

typedef struct D2D1_BEZIER_SEGMENT {
    D2D1_POINT_2F point1;
    D2D1_POINT_2F point2;
    D2D1_POINT_2F point3;
} D2D1_BEZIER_SEGMENT;

typedef enum D2D1_FIGURE_END {
    D2D1_FIGURE_END_OPEN = 0,
    D2D1_FIGURE_END_CLOSED = 1,
    D2D1_FIGURE_END_FORCE_DWORD = 0xffffffff
} D2D1_FIGURE_END;

class __declspec(uuid("2cd9069e-12e2-11dc-9fed-001143a055f9")) ID2D1SimplifiedGeometrySink : public IUnknown {
    STDMETHOD_(void, SetFillMode)(
        D2D1_FILL_MODE fillMode
        ) PURE;

    STDMETHOD_(void, SetSegmentFlags)(
        D2D1_PATH_SEGMENT vertexFlags
        ) PURE;

    STDMETHOD_(void, BeginFigure)(
        D2D1_POINT_2F startPoint,
        D2D1_FIGURE_BEGIN figureBegin
        ) PURE;

    STDMETHOD_(void, AddLines)(
        __in_ecount(pointsCount) CONST D2D1_POINT_2F* points,
        UINT pointsCount
        ) PURE;

    STDMETHOD_(void, AddBeziers)(
        __in_ecount(beziersCount) CONST D2D1_BEZIER_SEGMENT* beziers,
        UINT beziersCount
        ) PURE;

    STDMETHOD_(void, EndFigure)(
        D2D1_FIGURE_END figureEnd
        ) PURE;

    STDMETHOD(Close)(
        ) PURE;
}; // interface ID2D1SimplifiedGeometrySink

#endif