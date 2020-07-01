
#ifndef net_DefaultFullPath_h
#define net_DefaultFullPath_h

#include "third_party/WebKit/Source/wtf/text/WTFString.h"

namespace net {

String getDefaultLocalStorageFullPath();
void setDefaultLocalStorageFullPath(const String& path);

}

#endif // net_DefaultFullPath_h