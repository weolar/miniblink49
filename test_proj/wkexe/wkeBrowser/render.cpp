#include "renderGDI.h"
//#include "renderD3D.h"

CRender* CRender::create(RenderType type)
{
//     if (type == D3D_RENDER)
//         return new CRenderD3D;

    return new CRenderGDI;
}
