// Code generated from InspectorInstrumentation.idl

#ifndef InspectorConsoleInstrumentationInl_h
#define InspectorConsoleInstrumentationInl_h

#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/ScriptArguments.h"

namespace blink {

class ConsoleMessage;

namespace InspectorInstrumentation {

CORE_EXPORT void addMessageToConsoleImpl(InstrumentingAgents*, ConsoleMessage*);

inline void addMessageToConsole(ExecutionContext* context, ConsoleMessage* consoleMessage)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        addMessageToConsoleImpl(agents, consoleMessage);
}

CORE_EXPORT void consoleProfileImpl(InstrumentingAgents*, ExecutionContext*, const String&);

inline void consoleProfile(ExecutionContext* context, const String& title)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleProfileImpl(agents, context, title);
}

CORE_EXPORT void consoleProfileEndImpl(InstrumentingAgents*, const String&);

inline void consoleProfileEnd(ExecutionContext* context, const String& title)
{   
    FAST_RETURN_IF_NO_FRONTENDS(void());
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleProfileEndImpl(agents, title);
}

CORE_EXPORT void consoleMessagesClearedImpl(InstrumentingAgents*);

inline void consoleMessagesCleared(ExecutionContext* context)
{   
    if (InstrumentingAgents* agents = instrumentingAgentsFor(context))
        consoleMessagesClearedImpl(agents);
}

} // namespace InspectorInstrumentation

} // namespace blink

#endif // !defined(InspectorConsoleInstrumentationInl_h)
