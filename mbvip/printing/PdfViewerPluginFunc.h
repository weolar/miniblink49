
#include "third_party/npapi/bindings/npapi.h"
#include "third_party/npapi/bindings/npfunctions.h"
#include "third_party/npapi/bindings/npruntime.h"

namespace printing {

NPError __stdcall PdfViewerPluginNPInitialize(NPNetscapeFuncs* browserFuncs);
NPError __stdcall PdfViewerPluginNPGetEntryPoints(NPPluginFuncs* pluginFuncs);
void __stdcall PdfViewerPluginNPShutdown(void);

}