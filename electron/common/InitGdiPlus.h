
#ifndef common_InitGdiPlus_h
#define common_InitGdiPlus_h

#undef min
#undef max
#include <Unknwn.h>

namespace atom {
 
extern CLSID s_bmpClsid;
extern CLSID s_jpgClsid;
extern CLSID s_pngClsid;

bool initGDIPlusClsids();

}

#endif // common_InitGdiPlus_h