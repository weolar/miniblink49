
#ifndef InspectorProtocolVersion_h
#define InspectorProtocolVersion_h

#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

String inspectorProtocolVersion() { return "1.1"; }

int inspectorProtocolVersionMajor() { return 1; }

int inspectorProtocolVersionMinor() { return 1; }

bool supportsInspectorProtocolVersion(const String& version)
{
    Vector<String> tokens;
    version.split(".", tokens);
    if (tokens.size() != 2)
        return false;

    bool ok = true;
    int major = tokens[0].toInt(&ok);
    if (!ok || major != 1)
        return false;

    int minor = tokens[1].toInt(&ok);
    if (!ok || minor > 1)
        return false;

    return true;
}

}

#endif // !defined(InspectorProtocolVersion_h)
