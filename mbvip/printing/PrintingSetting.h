
#ifndef printing_PrintingSetting_h
#define printing_PrintingSetting_h

#include <xstring>

namespace printing {

// struct DefaultPrinterSettings {
//     int structSize; // 4 * 10
//     BOOL isLandscape;
//     BOOL isPrintHeadFooter;
//     BOOL isPrintBackgroud;
//     int edgeDistanceLeft;
//     int edgeDistanceTop;
//     int edgeDistanceRight;
//     int edgeDistanceBottom;
//     int copies;
//     int paperType;
// 
//     DefaultPrinterSettings()
//     {
//         isLandscape = false;
//         isPrintHeadFooter = false;
//         isPrintBackgroud = true;
//         edgeDistanceLeft = 0;
//         edgeDistanceTop = 11;
//         edgeDistanceRight = 0;
//         edgeDistanceBottom = 10;
//         copies = 1;
//         paperType = 9;
//     }
// };

extern mbDefaultPrinterSettings* s_defaultPrinterSettings;
extern int g_edgeDistance[4]; // 以前默认是10.现在改成0
}

#endif