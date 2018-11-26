
#ifndef wke_wkeNetHook_h
#define wke_wkeNetHook_h

#include "wke/wkedefine.h"
#include "third_party/WebKit/Source/wtf/Vector.h"

namespace net {
struct FlattenHTTPBodyElement;
}

namespace wke {

wkePostBodyElements* flattenHTTPBodyElementToWke(const WTF::Vector<net::FlattenHTTPBodyElement*>& body);
void wkeflattenElementToBlink(const wkePostBodyElements& body, WTF::Vector<net::FlattenHTTPBodyElement*>* out);

}

#endif // wke_wkeNetHook_h